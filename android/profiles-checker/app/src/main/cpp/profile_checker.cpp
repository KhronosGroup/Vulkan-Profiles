/*
 * Copyright 2026 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "profile_checker.h"

#include <android/log.h>

#include <algorithm>
#include <cstring>
#include <set>
#include <string>
#include <vector>

#include "vulkan_loader.h"

// ---------------------------------------------------------------------------
// Debug message capture.
//
// The generated *debug* profiles library reports every unmet condition through
// VP_DEBUG_MESSAGE_CALLBACK. By defining the macro before including the header
// the library declares `void VpCheckerDebugCallback(const char*)` and calls it
// instead of logging to logcat. Messages are collected into a thread-local
// buffer while a support query runs.
// ---------------------------------------------------------------------------
#define VP_DEBUG_MESSAGE_CALLBACK VpCheckerDebugCallback
#include VP_CHECKER_GENERATED_HEADER

#include "vp_checker_build_info.h"  // generated at build time by gen_build_info.cmake

namespace {

constexpr const char* kLogTag = "VulkanProfilesChecker";

thread_local std::vector<std::string>* g_captureBuffer = nullptr;

class MessageCapture {
   public:
    MessageCapture() { g_captureBuffer = &messages_; }
    ~MessageCapture() { g_captureBuffer = nullptr; }
    MessageCapture(const MessageCapture&) = delete;
    MessageCapture& operator=(const MessageCapture&) = delete;

    const std::vector<std::string>& messages() const { return messages_; }

   private:
    std::vector<std::string> messages_;
};

}  // namespace

void VpCheckerDebugCallback(const char* message) {
    if (message == nullptr) return;
    if (g_captureBuffer != nullptr) {
        g_captureBuffer->emplace_back(message);
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, kLogTag, "[profiles] %s", message);
    }
}

namespace {

struct Prefix {
    const char* text;
    RequirementKind kind;
    bool stripPrefix;
};

// Known message prefixes emitted by gen_profiles_solution.py in debug mode.
const Prefix kPrefixes[] = {
    {"Unsupported extension: ", RequirementKind::Extension, true},
    {"Unsupported feature condition: ", RequirementKind::Feature, true},
    {"Unsupported properties condition: ", RequirementKind::Property, true},
    {"Unsupported format condition for ", RequirementKind::Format, true},
    {"Unsupported video capability condition: ", RequirementKind::VideoCapability, true},
    {"Unsupported API version", RequirementKind::ApiVersion, false},
    {"Unsupported Profile API version", RequirementKind::ApiVersion, false},
    {"Unsupported mandatory extension", RequirementKind::Extension, false},
    {"Unsupported requested ", RequirementKind::Other, false},
};

bool StartsWith(const std::string& s, const char* prefix) {
    const size_t len = strlen(prefix);
    return s.size() >= len && s.compare(0, len, prefix) == 0;
}

// Converts raw captured messages into de-duplicated, categorised requirements.
// Purely informational messages (not starting with "Unsupported") are dropped.
std::vector<UnmetRequirement> Categorize(const std::vector<std::string>& messages) {
    std::vector<UnmetRequirement> result;
    std::set<std::string> seen;
    for (const std::string& message : messages) {
        if (!StartsWith(message, "Unsupported")) continue;
        if (!seen.insert(message).second) continue;

        UnmetRequirement req;
        req.kind = RequirementKind::Other;
        req.detail = message;
        for (const Prefix& prefix : kPrefixes) {
            if (StartsWith(message, prefix.text)) {
                req.kind = prefix.kind;
                if (prefix.stripPrefix) req.detail = message.substr(strlen(prefix.text));
                break;
            }
        }
        result.push_back(std::move(req));
    }

    // Stable sort by kind so the UI can group them.
    std::stable_sort(result.begin(), result.end(),
                     [](const UnmetRequirement& a, const UnmetRequirement& b) { return a.kind < b.kind; });
    return result;
}

std::string ResultToString(VkResult result) {
    switch (result) {
        case VK_SUCCESS:
            return "VK_SUCCESS";
        case VK_INCOMPLETE:
            return "VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED:
            return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT:
            return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_UNKNOWN:
            return "VK_ERROR_UNKNOWN";
        default:
            return "VkResult(" + std::to_string(static_cast<int>(result)) + ")";
    }
}

// Display order: Android requirements, Android vulkan profiles, Khronos roadmap, then the rest.
int ProfileGroupRank(const std::string& name) {
    if (StartsWith(name, "VP_ANDROID_") && name.find("_requirements") != std::string::npos) return 0;
    if (StartsWith(name, "VP_ANDROID_")) return 1;
    if (StartsWith(name, "VP_KHR_")) return 2;
    return 3;
}

struct FunctionsGuard {
    VpFunctions functions = VK_NULL_HANDLE;
    ~FunctionsGuard() {
        if (functions != VK_NULL_HANDLE) vpDestroyFunctions(functions, nullptr);
    }
};

struct InstanceGuard {
    VkInstance instance = VK_NULL_HANDLE;
    PFN_vkDestroyInstance destroy = nullptr;
    ~InstanceGuard() {
        if (instance != VK_NULL_HANDLE && destroy != nullptr) destroy(instance, nullptr);
    }
};

constexpr uint32_t kMaxBlocks = 64;

}  // namespace

const char* RequirementKindName(RequirementKind kind) {
    switch (kind) {
        case RequirementKind::ApiVersion:
            return "API version";
        case RequirementKind::Extension:
            return "Extensions";
        case RequirementKind::Feature:
            return "Features";
        case RequirementKind::Property:
            return "Properties & limits";
        case RequirementKind::Format:
            return "Formats";
        case RequirementKind::VideoCapability:
            return "Video capabilities";
        case RequirementKind::Other:
        default:
            return "Other";
    }
}

namespace {

std::string AppVersionString() {
    return std::string(VP_CHECKER_VERSION_NAME) + " (" + std::to_string(VP_CHECKER_VERSION_CODE) + ")";
}

// ABI of the running process. Distinct from ro.product.cpu.abi: a universal APK
// on a 64-bit device runs arm64-v8a, but a single-ABI test build may run armeabi-v7a.
constexpr const char* kAppAbi =
#if defined(__aarch64__)
    "arm64-v8a";
#elif defined(__arm__)
    "armeabi-v7a";
#elif defined(__x86_64__)
    "x86_64";
#elif defined(__i386__)
    "x86";
#elif defined(__riscv)
    "riscv64";
#else
    "unknown";
#endif

std::string AppGitCommitString() {
    std::string commit = VP_CHECKER_GIT_COMMIT;
    if (VP_CHECKER_GIT_DIRTY) commit += "-dirty";
    return commit;
}

}  // namespace

std::string AppBuildIdentityLine() {
    // Plain ASCII separators: the bundled font rasterizer only covers Latin-1.
    return "v" + AppVersionString() + " " + kAppAbi + " | built " + VP_CHECKER_BUILD_TIME_UTC + " | " + AppGitCommitString();
}

CheckReport RunProfileChecks() {
    CheckReport report;
    report.appVersion = AppVersionString();
    report.buildTime = VP_CHECKER_BUILD_TIME_UTC;
    report.gitCommit = AppGitCommitString();
    report.appAbi = kAppAbi;
    report.vulkanHeadersVersion = VP_CHECKER_VULKAN_HEADERS_TAG;
    report.system = QuerySystemInfo();

    // ---- 1. Load the Vulkan loader -------------------------------------------------
    VulkanLoader vk;
    if (!vk.LoadGlobal(report.error)) {
        __android_log_print(ANDROID_LOG_ERROR, kLogTag, "%s", report.error.c_str());
        return report;
    }

    uint32_t instanceVersion = VK_API_VERSION_1_0;
    if (vk.EnumerateInstanceVersion != nullptr) {
        uint32_t version = 0;
        if (vk.EnumerateInstanceVersion(&version) == VK_SUCCESS && version != 0) instanceVersion = version;
    }
    // Do not request a variant / patch that is not meaningful for instance creation.
    instanceVersion = VK_MAKE_API_VERSION(0, VK_API_VERSION_MAJOR(instanceVersion),
                                          VK_API_VERSION_MINOR(instanceVersion), 0);
    report.instanceVersionRaw = instanceVersion;
    report.instanceVersion = VulkanVersionToString(instanceVersion);

    // ---- 2. Create the profiles library function table ----------------------------
    // With VK_NO_PROTOTYPES the library has no static Vulkan functions, and
    // vpCreateFunctions validates the global entry points immediately, so they
    // are provided through the create info (vpInitializeGlobalFunctions then
    // fills in anything else it needs from vkGetInstanceProcAddr).
    FunctionsGuard functionsGuard;
    {
        VpFunctionsCreateInfo createInfo{};
        createInfo.GetInstanceProcAddr = vk.GetInstanceProcAddr;
        createInfo.EnumerateInstanceVersion = vk.EnumerateInstanceVersion;
        createInfo.EnumerateInstanceExtensionProperties = vk.EnumerateInstanceExtensionProperties;
        createInfo.CreateInstance = vk.CreateInstance;
        VkResult result = vpCreateFunctions(&createInfo, nullptr, &functionsGuard.functions);
        if (result != VK_SUCCESS) {
            report.error = "vpCreateFunctions failed: " + ResultToString(result);
            return report;
        }
        result = vpInitializeGlobalFunctions(functionsGuard.functions, vk.GetInstanceProcAddr);
        if (result != VK_SUCCESS) {
            report.error = "vpInitializeGlobalFunctions failed: " + ResultToString(result);
            return report;
        }
    }
    VpFunctions functions = functionsGuard.functions;

    // ---- 3. Enumerate profiles compiled into the library ---------------------------
    std::vector<VpProfileProperties> profiles;
    {
        uint32_t count = 0;
        VkResult result = vpGetProfiles(functions, &count, nullptr);
        if (result != VK_SUCCESS || count == 0) {
            report.error = "No profiles available in the generated library";
            return report;
        }
        profiles.resize(count);
        result = vpGetProfiles(functions, &count, profiles.data());
        if (result != VK_SUCCESS) {
            report.error = "vpGetProfiles failed: " + ResultToString(result);
            return report;
        }
        profiles.resize(count);
    }
    std::sort(profiles.begin(), profiles.end(), [](const VpProfileProperties& a, const VpProfileProperties& b) {
        const std::string na(a.profileName, strnlen(a.profileName, VP_MAX_PROFILE_NAME_SIZE));
        const std::string nb(b.profileName, strnlen(b.profileName, VP_MAX_PROFILE_NAME_SIZE));
        const int ra = ProfileGroupRank(na), rb = ProfileGroupRank(nb);
        return ra != rb ? ra < rb : na < nb;
    });

    // ---- 4. Instance-level checks ---------------------------------------------------
    std::vector<ProfileResult> baseResults;
    baseResults.reserve(profiles.size());
    std::set<std::string> wantedInstanceExtensions;

    for (const VpProfileProperties& profile : profiles) {
        ProfileResult pr;
        pr.name.assign(profile.profileName, strnlen(profile.profileName, VP_MAX_PROFILE_NAME_SIZE));
        pr.specVersion = profile.specVersion;
        pr.minApiVersion = vpGetProfileAPIVersion(functions, &profile);

        VkBool32 multiple = VK_FALSE;
        if (vpHasMultipleVariantsProfile(functions, &profile, &multiple) == VK_SUCCESS) {
            pr.hasMultipleVariants = (multiple == VK_TRUE);
        }

        {
            MessageCapture capture;
            VkBool32 supported = VK_FALSE;
            const VkResult result = vpGetInstanceProfileSupport(functions, nullptr, &profile, &supported);
            if (result != VK_SUCCESS) {
                pr.queryFailed = true;
                pr.error = "vpGetInstanceProfileSupport failed: " + ResultToString(result);
            }
            pr.instanceSupported = (result == VK_SUCCESS && supported == VK_TRUE);
            pr.instanceUnmet = Categorize(capture.messages());
        }

        // Collect the instance extensions the profile wants so we can enable
        // whatever subset the loader actually exposes.
        uint32_t extCount = 0;
        if (vpGetProfileInstanceExtensionProperties(functions, &profile, nullptr, &extCount, nullptr) == VK_SUCCESS &&
            extCount > 0) {
            std::vector<VkExtensionProperties> exts(extCount);
            if (vpGetProfileInstanceExtensionProperties(functions, &profile, nullptr, &extCount, exts.data()) ==
                VK_SUCCESS) {
                for (uint32_t i = 0; i < extCount; ++i) {
                    wantedInstanceExtensions.emplace(exts[i].extensionName,
                                                     strnlen(exts[i].extensionName, VK_MAX_EXTENSION_NAME_SIZE));
                }
            }
        }

        baseResults.push_back(std::move(pr));
    }
    // Needed for the *2 queries on Vulkan 1.0 implementations.
    wantedInstanceExtensions.insert(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

    // ---- 5. Create a VkInstance -----------------------------------------------------
    std::vector<std::string> enabledExtensions;
    {
        uint32_t count = 0;
        std::vector<VkExtensionProperties> available;
        if (vk.EnumerateInstanceExtensionProperties(nullptr, &count, nullptr) == VK_SUCCESS && count > 0) {
            available.resize(count);
            if (vk.EnumerateInstanceExtensionProperties(nullptr, &count, available.data()) == VK_SUCCESS) {
                available.resize(count);
            } else {
                available.clear();
            }
        }
        for (const VkExtensionProperties& ext : available) {
            std::string name(ext.extensionName, strnlen(ext.extensionName, VK_MAX_EXTENSION_NAME_SIZE));
            if (wantedInstanceExtensions.count(name) != 0) enabledExtensions.push_back(std::move(name));
        }
    }
    std::vector<const char*> enabledExtensionPtrs;
    enabledExtensionPtrs.reserve(enabledExtensions.size());
    for (const std::string& ext : enabledExtensions) enabledExtensionPtrs.push_back(ext.c_str());
    report.enabledInstanceExtensions = enabledExtensions;

    InstanceGuard instanceGuard;
    {
        VkApplicationInfo appInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO};
        appInfo.pApplicationName = "Vulkan Profiles Checker";
        appInfo.applicationVersion = 1;
        appInfo.pEngineName = "Vulkan Profiles Checker";
        appInfo.engineVersion = 1;
        appInfo.apiVersion = instanceVersion;

        VkInstanceCreateInfo createInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensionPtrs.size());
        createInfo.ppEnabledExtensionNames = enabledExtensionPtrs.empty() ? nullptr : enabledExtensionPtrs.data();

        VkResult result = vk.CreateInstance(&createInfo, nullptr, &instanceGuard.instance);
        if (result != VK_SUCCESS) {
            // Retry without any extension in case one of them is refused.
            createInfo.enabledExtensionCount = 0;
            createInfo.ppEnabledExtensionNames = nullptr;
            report.enabledInstanceExtensions.clear();
            result = vk.CreateInstance(&createInfo, nullptr, &instanceGuard.instance);
        }
        if (result != VK_SUCCESS) {
            report.error = "vkCreateInstance failed: " + ResultToString(result);
            return report;
        }
    }
    vk.LoadInstance(instanceGuard.instance);
    instanceGuard.destroy = vk.DestroyInstance;

    {
        const VkResult result =
            vpInitializeInstanceFunctions(functions, instanceGuard.instance,
                                          VP_INSTANCE_FUNCTIONS_LOAD_KHR_GET_PHYSICAL_DEVICE_PROPERTIES2_BIT);
        if (result != VK_SUCCESS) {
            report.error = "vpInitializeInstanceFunctions failed: " + ResultToString(result) +
                           " (VK_KHR_get_physical_device_properties2 or Vulkan 1.1 is required)";
            return report;
        }
    }

    // ---- 6. Device-level checks -----------------------------------------------------
    std::vector<VkPhysicalDevice> physicalDevices;
    {
        uint32_t count = 0;
        if (vk.EnumeratePhysicalDevices(instanceGuard.instance, &count, nullptr) != VK_SUCCESS || count == 0) {
            report.error = "No Vulkan physical device found";
            return report;
        }
        physicalDevices.resize(count);
        if (vk.EnumeratePhysicalDevices(instanceGuard.instance, &count, physicalDevices.data()) != VK_SUCCESS) {
            report.error = "vkEnumeratePhysicalDevices failed";
            return report;
        }
        physicalDevices.resize(count);
    }

    for (VkPhysicalDevice physicalDevice : physicalDevices) {
        GpuReport gpu;
        gpu.info = QueryDeviceInfo(vk, physicalDevice, instanceVersion);
        gpu.profiles = baseResults;

        for (size_t i = 0; i < profiles.size(); ++i) {
            ProfileResult& pr = gpu.profiles[i];
            if (pr.queryFailed) continue;

            MessageCapture capture;
            VkBool32 supported = VK_FALSE;
            uint32_t blockCount = kMaxBlocks;
            std::vector<VpBlockProperties> blocks(kMaxBlocks);

            VkResult result = vpGetPhysicalDeviceProfileVariantsSupport(
                functions, instanceGuard.instance, physicalDevice, &profiles[i], &supported, &blockCount,
                blocks.data());
            if (result == VK_INCOMPLETE) {
                blockCount = std::min(blockCount, kMaxBlocks);
                result = VK_SUCCESS;
            }
            if (result != VK_SUCCESS) {
                pr.queryFailed = true;
                pr.error = "vpGetPhysicalDeviceProfileVariantsSupport failed: " + ResultToString(result);
                pr.deviceSupported = false;
            } else {
                pr.deviceSupported = (supported == VK_TRUE);
            }

            blockCount = std::min(blockCount, kMaxBlocks);
            std::set<std::string> seenBlocks;
            for (uint32_t b = 0; b < blockCount; ++b) {
                std::string blockName(blocks[b].blockName, strnlen(blocks[b].blockName, VP_MAX_PROFILE_NAME_SIZE));
                std::string profileName(blocks[b].profiles.profileName,
                                        strnlen(blocks[b].profiles.profileName, VP_MAX_PROFILE_NAME_SIZE));
                std::string label = blockName;
                if (!profileName.empty() && profileName != pr.name) label = profileName + " / " + blockName;
                if (seenBlocks.insert(label).second) pr.blocks.push_back(std::move(label));
            }

            pr.deviceUnmet = Categorize(capture.messages());
        }

        report.gpus.push_back(std::move(gpu));
    }

    report.ok = true;
    return report;
}
