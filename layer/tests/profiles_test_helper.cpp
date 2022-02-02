/*
 * Copyright (c) 2021-2022 Valve Corporation
 * Copyright (c) 2021-2022 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Authors:
 * - Jeremy Kniager <jeremyk@lunarg.com>
 */

#include <sstream>
#include <stdlib.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#include "profiles_test_helper.h"

// On Android, VK_MAKE_API_VERSION doesn't yet exist.
#ifndef VK_MAKE_API_VERSION
#define VK_MAKE_API_VERSION(variant, major, minor, patch) VK_MAKE_VERSION(major, minor, patch)
#endif

void profiles_test::setEnvironmentSetting(std::string setting, const char* val) {
#ifdef _WIN32
    _putenv_s(setting.c_str(), val);
#else
    setenv(setting.c_str(), val, 1);
#endif
}

void profiles_test::unsetEnvironmentSetting(std::string setting) {
#ifdef _WIN32
    _putenv_s(setting.c_str(), "");
#else
    unsetenv(setting.c_str());
#endif
}

std::string profiles_test::getAbsolutePath(std::string filepath) {
    std::string out;
#ifdef _WIN32
    char abs_path[_MAX_PATH];
    _fullpath(abs_path, filepath.c_str(), _MAX_PATH);
    out = abs_path;
#else
    char* abs_path = realpath(filepath.c_str(), nullptr);
    out = abs_path;
    delete[] abs_path;
#endif
    return out;
}

void profiles_test::setProfilesFilename(const std::string& filepath) {
    profiles_test::setEnvironmentSetting("VK_KHRONOS_PROFILES_PROFILE_FILE", filepath.c_str());
    profiles_test::setEnvironmentSetting("VK_KHRONOS_PROFILES_PROFILE_VALIDATION", "false"); // Never validate profile files, it's done by a dedicated test.
}

void profiles_test::setProfilesDebugEnable(bool enable) {
    if (enable)
        profiles_test::setEnvironmentSetting("VK_KHRONOS_PROFILES_DEBUG_ENABLE", "true");
    else
        profiles_test::setEnvironmentSetting("VK_KHRONOS_PROFILES_DEBUG_ENABLE", "false");
}

void profiles_test::setProfilesEmulatePortabilitySubsetExtension(bool enable) {
    if (enable)
        profiles_test::setEnvironmentSetting("VK_KHRONOS_PROFILES_EMULATE_PORTABILITY", "true");
    else
        profiles_test::setEnvironmentSetting("VK_KHRONOS_PROFILES_EMULATE_PORTABILITY", "false");
}

std::string profiles_test::GetSimulateCapabilitiesLog(SimulateCapabilityFlags flags) {
    std::string result = {};
    bool need_comma = false;

    if (flags & SIMULATE_API_VERSION_BIT) {
        result += "SIMULATE_API_VERSION_BIT";
        need_comma = true;
    }
    if (flags & SIMULATE_FEATURES_BIT) {
        if (need_comma) result += ",";
        result += "SIMULATE_FEATURES_BIT";
        need_comma = true;
    }
    if (flags & SIMULATE_PROPERTIES_BIT) {
        if (need_comma) result += ",";
        result += "SIMULATE_PROPERTIES_BIT";
        need_comma = true;
    }
    if (flags & SIMULATE_EXTENSIONS_BIT) {
        if (need_comma) result += ",";
        result += "SIMULATE_EXTENSIONS_BIT";
        need_comma = true;
    }
    if (flags & SIMULATE_FORMATS_BIT) {
        if (need_comma) result += ",";
        result += "SIMULATE_FORMATS_BIT";
        need_comma = true;
    }
    if (flags & SIMULATE_FORMAT_PROPERTIES_BIT) {
        if (need_comma) result += ",";
        result += "SIMULATE_FORMAT_PROPERTIES_BIT";
        need_comma = true;
    }

    return result;
}

void profiles_test::setProfilesSimulateCapabilities(SimulateCapabilityFlags flags) {
    profiles_test::setEnvironmentSetting("VK_KHRONOS_PROFILES_SIMULATE_CAPABILITIES", GetSimulateCapabilitiesLog(flags).c_str());
}

void profiles_test::setProfilesSimulateAllCapabilities() {
    setProfilesSimulateCapabilities(SIMULATE_API_VERSION_BIT | SIMULATE_FEATURES_BIT | SIMULATE_PROPERTIES_BIT |
                                    SIMULATE_EXTENSIONS_BIT | SIMULATE_FORMATS_BIT | SIMULATE_FORMAT_PROPERTIES_BIT);
}

void profiles_test::setProfilesProfileName(const std::string& profile) {
    profiles_test::setEnvironmentSetting("VK_KHRONOS_PROFILES_PROFILE_NAME", profile.c_str());
}

void profiles_test::setProfilesFailOnError(bool fail) {
    profiles_test::setEnvironmentSetting("VK_KHRONOS_PROFILES_DEBUG_FAIL_ON_ERROR", fail ? "true" : "false");
}

void profiles_test::setExcludeDeviceExtensions(const std::vector<std::string>& extensions) {
    std::string combined = {};
    for (const auto& ext : extensions) {
        if (!combined.empty()) combined += ';';
        combined += ext;
    }
    profiles_test::setEnvironmentSetting("VK_KHRONOS_PROFILES_EXCLUDE_DEVICE_EXTENSIONS", combined.c_str());
}

void profiles_test::setExcludeFormats(const std::vector<std::string>& formats) {
    std::string combined = {};
    for (const auto& format : formats) {
        if (!combined.empty()) combined += ';';
        combined += format;
    }
    profiles_test::setEnvironmentSetting("VK_KHRONOS_PROFILES_EXCLUDE_FORMATS", combined.c_str());
}

VkApplicationInfo profiles_test::GetDefaultApplicationInfo() {
    VkApplicationInfo out{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
    out.apiVersion = VK_API_VERSION_1_1;
    out.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    out.pApplicationName = "profiles_tests";
    out.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    out.pEngineName = "profiles_tests";

    return out;
}

VkResult profiles_test::VulkanInstanceBuilder::getPhysicalDevice(VkPhysicalDevice* phys_dev) {
    *phys_dev = VK_NULL_HANDLE;
    VkResult res;
    uint32_t gpu_count = 0;
    res = vkEnumeratePhysicalDevices(_instance, &gpu_count, nullptr);
    if (res != VK_SUCCESS) {
        return res;
    }
    std::vector<VkPhysicalDevice> gpus(gpu_count);
    res = vkEnumeratePhysicalDevices(_instance, &gpu_count, gpus.data());
    if (res != VK_SUCCESS) {
        return res;
    }
    *phys_dev = gpus[0];
    return res;
}

VkResult profiles_test::VulkanInstanceBuilder::makeInstance() {
    _inst_create_info.pApplicationInfo = &_app_info;
    _inst_create_info.enabledLayerCount = static_cast<uint32_t>(_layer_names.size());
    _inst_create_info.ppEnabledLayerNames = _layer_names.data();
    _inst_create_info.enabledExtensionCount = static_cast<uint32_t>(_extension_names.size());
    _inst_create_info.ppEnabledExtensionNames = _extension_names.data();

    return vkCreateInstance(&_inst_create_info, nullptr, &_instance);
}

void profiles_test::VulkanInstanceBuilder::reset() {
    _app_info = GetDefaultApplicationInfo();
    _inst_create_info = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    _instance = VK_NULL_HANDLE;

    _layer_names.clear();
    _extension_names.clear();
}
