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

#include "device_info.h"

#include <sys/system_properties.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <vector>

#include "vulkan_loader.h"

namespace {

std::string GetSystemProperty(const char* name) {
    char value[PROP_VALUE_MAX] = {};
    const int len = __system_property_get(name, value);
    if (len <= 0) return std::string();
    return std::string(value, static_cast<size_t>(len));
}

// Copies a fixed-size, possibly non-terminated char array into a std::string.
template <size_t N>
std::string FixedString(const char (&buffer)[N]) {
    return std::string(buffer, strnlen(buffer, N));
}

const char* DeviceTypeToString(VkPhysicalDeviceType type) {
    switch (type) {
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            return "Integrated GPU";
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            return "Discrete GPU";
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            return "Virtual GPU";
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            return "CPU";
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        default:
            return "Other";
    }
}

const char* DriverIdToString(VkDriverId id) {
    switch (id) {
        case VK_DRIVER_ID_AMD_PROPRIETARY:
            return "AMD proprietary";
        case VK_DRIVER_ID_AMD_OPEN_SOURCE:
            return "AMD open source";
        case VK_DRIVER_ID_MESA_RADV:
            return "Mesa RADV";
        case VK_DRIVER_ID_NVIDIA_PROPRIETARY:
            return "NVIDIA proprietary";
        case VK_DRIVER_ID_INTEL_PROPRIETARY_WINDOWS:
            return "Intel proprietary (Windows)";
        case VK_DRIVER_ID_INTEL_OPEN_SOURCE_MESA:
            return "Intel open source Mesa";
        case VK_DRIVER_ID_IMAGINATION_PROPRIETARY:
            return "Imagination proprietary";
        case VK_DRIVER_ID_QUALCOMM_PROPRIETARY:
            return "Qualcomm proprietary";
        case VK_DRIVER_ID_ARM_PROPRIETARY:
            return "Arm proprietary";
        case VK_DRIVER_ID_GOOGLE_SWIFTSHADER:
            return "Google SwiftShader";
        case VK_DRIVER_ID_GGP_PROPRIETARY:
            return "GGP proprietary";
        case VK_DRIVER_ID_BROADCOM_PROPRIETARY:
            return "Broadcom proprietary";
        case VK_DRIVER_ID_MESA_LLVMPIPE:
            return "Mesa llvmpipe";
        case VK_DRIVER_ID_MOLTENVK:
            return "MoltenVK";
        case VK_DRIVER_ID_COREAVI_PROPRIETARY:
            return "CoreAVI proprietary";
        case VK_DRIVER_ID_JUICE_PROPRIETARY:
            return "Juice proprietary";
        case VK_DRIVER_ID_VERISILICON_PROPRIETARY:
            return "VeriSilicon proprietary";
        case VK_DRIVER_ID_MESA_TURNIP:
            return "Mesa Turnip";
        case VK_DRIVER_ID_MESA_V3DV:
            return "Mesa V3DV";
        case VK_DRIVER_ID_MESA_PANVK:
            return "Mesa PanVK";
        case VK_DRIVER_ID_SAMSUNG_PROPRIETARY:
            return "Samsung proprietary";
        case VK_DRIVER_ID_MESA_VENUS:
            return "Mesa Venus";
        case VK_DRIVER_ID_MESA_DOZEN:
            return "Mesa Dozen";
        case VK_DRIVER_ID_MESA_NVK:
            return "Mesa NVK";
        case VK_DRIVER_ID_IMAGINATION_OPEN_SOURCE_MESA:
            return "Imagination open source Mesa";
        case VK_DRIVER_ID_MESA_HONEYKRISP:
            return "Mesa Honeykrisp";
        default:
            return nullptr;
    }
}

}  // namespace

std::string VulkanVersionToString(uint32_t version) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%u.%u.%u", VK_API_VERSION_MAJOR(version), VK_API_VERSION_MINOR(version),
             VK_API_VERSION_PATCH(version));
    return buffer;
}

std::string VendorIdToString(uint32_t vendorID) {
    switch (vendorID) {
        case 0x1002:
            return "AMD";
        case 0x1010:
            return "Imagination Technologies";
        case 0x10DE:
            return "NVIDIA";
        case 0x13B5:
            return "Arm";
        case 0x14E4:
            return "Broadcom";
        case 0x1AE0:
            return "Google";
        case 0x5143:
            return "Qualcomm";
        case 0x8086:
            return "Intel";
        case 0x144D:
            return "Samsung";
        case 0x19E5:
            return "Huawei";
        case 0x10005:
            return "Mesa";
        default: {
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "Unknown (0x%04X)", vendorID);
            return buffer;
        }
    }
}

std::string DecodeDriverVersion(uint32_t vendorID, uint32_t driverVersion) {
    char buffer[96];
    switch (vendorID) {
        case 0x10DE:  // NVIDIA: 10.8.8.6 bit layout
            snprintf(buffer, sizeof(buffer), "%u.%u.%u.%u", (driverVersion >> 22) & 0x3FFu,
                     (driverVersion >> 14) & 0xFFu, (driverVersion >> 6) & 0xFFu, driverVersion & 0x3Fu);
            return buffer;
        case 0x5143:  // Qualcomm: recent drivers use the standard encoding; older ones used 512.x.y
            if (driverVersion & 0x80000000u) {
                snprintf(buffer, sizeof(buffer), "512.%u.%u (0x%08X)", (driverVersion >> 12) & 0x3FFu,
                         driverVersion & 0xFFFu, driverVersion);
                return buffer;
            }
            break;
        default:
            break;
    }
    snprintf(buffer, sizeof(buffer), "%u.%u.%u (0x%08X)", VK_API_VERSION_MAJOR(driverVersion),
             VK_API_VERSION_MINOR(driverVersion), VK_API_VERSION_PATCH(driverVersion), driverVersion);
    return buffer;
}

SystemInfo QuerySystemInfo() {
    SystemInfo info;
    info.manufacturer = GetSystemProperty("ro.product.manufacturer");
    info.model = GetSystemProperty("ro.product.model");
    info.device = GetSystemProperty("ro.product.device");
    info.androidVersion = GetSystemProperty("ro.build.version.release");
    info.sdkLevel = GetSystemProperty("ro.build.version.sdk");
    info.securityPatch = GetSystemProperty("ro.build.version.security_patch");
    info.socManufacturer = GetSystemProperty("ro.soc.manufacturer");
    info.socModel = GetSystemProperty("ro.soc.model");
    info.hardwareVulkan = GetSystemProperty("ro.hardware.vulkan");
    info.gfxDriver0 = GetSystemProperty("ro.gfx.driver.0");
    info.gfxDriver1 = GetSystemProperty("ro.gfx.driver.1");
    info.abi = GetSystemProperty("ro.product.cpu.abi");
    return info;
}

DeviceInfo QueryDeviceInfo(const VulkanLoader& vk, VkPhysicalDevice physicalDevice, uint32_t instanceApiVersion) {
    DeviceInfo info;

    VkPhysicalDeviceProperties props{};
    vk.GetPhysicalDeviceProperties(physicalDevice, &props);

    info.deviceName = FixedString(props.deviceName);
    info.deviceType = DeviceTypeToString(props.deviceType);
    info.vendorID = props.vendorID;
    info.deviceID = props.deviceID;
    info.vendorName = VendorIdToString(props.vendorID);
    info.apiVersionRaw = props.apiVersion;
    info.apiVersion = VulkanVersionToString(props.apiVersion);
    info.driverVersionRaw = props.driverVersion;
    info.driverVersion = DecodeDriverVersion(props.vendorID, props.driverVersion);

    // Device extensions (needed to decide whether VK_KHR_driver_properties can be queried).
    bool hasDriverPropertiesExt = false;
    if (vk.EnumerateDeviceExtensionProperties != nullptr) {
        uint32_t count = 0;
        if (vk.EnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, nullptr) == VK_SUCCESS && count > 0) {
            std::vector<VkExtensionProperties> extensions(count);
            if (vk.EnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, extensions.data()) == VK_SUCCESS) {
                extensions.resize(count);
                info.deviceExtensionCount = count;
                for (const VkExtensionProperties& ext : extensions) {
                    if (strncmp(ext.extensionName, VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME,
                                VK_MAX_EXTENSION_NAME_SIZE) == 0) {
                        hasDriverPropertiesExt = true;
                    }
                }
            }
        }
    }

    // VkPhysicalDeviceDriverProperties is available with Vulkan 1.2 or VK_KHR_driver_properties,
    // and requires vkGetPhysicalDeviceProperties2 (instance 1.1+ or VK_KHR_get_physical_device_properties2).
    const bool deviceSupportsDriverProps = (props.apiVersion >= VK_API_VERSION_1_2) || hasDriverPropertiesExt;
    const bool effectiveApi12 =
        (std::min(instanceApiVersion, props.apiVersion) >= VK_API_VERSION_1_2) || hasDriverPropertiesExt;

    if (vk.GetPhysicalDeviceProperties2 != nullptr && deviceSupportsDriverProps && effectiveApi12) {
        VkPhysicalDeviceDriverProperties driverProps{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES};
        VkPhysicalDeviceProperties2 props2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
        props2.pNext = &driverProps;
        vk.GetPhysicalDeviceProperties2(physicalDevice, &props2);

        info.hasDriverProperties = true;
        info.driverName = FixedString(driverProps.driverName);
        info.driverInfo = FixedString(driverProps.driverInfo);

        const char* driverIdName = DriverIdToString(driverProps.driverID);
        if (driverIdName != nullptr) {
            info.driverID = driverIdName;
        } else {
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%d", static_cast<int>(driverProps.driverID));
            info.driverID = buffer;
        }

        char conformance[64];
        snprintf(conformance, sizeof(conformance), "%u.%u.%u.%u", driverProps.conformanceVersion.major,
                 driverProps.conformanceVersion.minor, driverProps.conformanceVersion.subminor,
                 driverProps.conformanceVersion.patch);
        info.conformanceVersion = conformance;
    }

    return info;
}
