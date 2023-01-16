/*
 * Copyright (c) 2021-2023 Valve Corporation
 * Copyright (c) 2021-2023 LunarG, Inc.
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
 * - Mark Lobodzinski <mark@lunarg.com>
 */

#include <sstream>
#include <stdlib.h>
#include <cctype>

#ifdef _WIN32
#include <Windows.h>
#elif ANDROID
#include <android/log.h>
#endif

#include "profiles_test_helper.h"

#ifdef _WIN32
#ifdef _DEBUG
static const char* CONFIG_PATH = "bin/Debug";
#else
static const char* CONFIG_PATH = "bin/Release";
#endif
#else
static const char* CONFIG_PATH = "lib";
#endif

// On Android, VK_MAKE_API_VERSION doesn't yet exist.
#ifndef VK_MAKE_API_VERSION
#define VK_MAKE_API_VERSION(variant, major, minor, patch) VK_MAKE_VERSION(major, minor, patch)
#endif

// TODO: When the layer path issue is resolved with CI (set outside of the tests) remove this function
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
    if (flags & SIMULATE_QUEUE_FAMILY_PROPERTIES_BIT) {
        if (need_comma) result += ",";
        result += "SIMULATE_QUEUE_FAMILY_PROPERTIES_BIT";
        need_comma = true;
    }

    return result;
}

VkApplicationInfo profiles_test::GetDefaultApplicationInfo() {
    VkApplicationInfo out{VK_STRUCTURE_TYPE_APPLICATION_INFO};
    out.apiVersion = VK_API_VERSION_1_3;
    out.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    out.pApplicationName = "profiles_tests";
    out.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    out.pEngineName = "profiles_tests";

    return out;
}

VkResult profiles_test::VulkanInstanceBuilder::getPhysicalDevice(Mode mode, VkPhysicalDevice* phys_dev) {
    *phys_dev = VK_NULL_HANDLE;
    VkResult res;
    uint32_t gpu_count = 0;
    res = vkEnumeratePhysicalDevices(_instances[mode], &gpu_count, nullptr);
    if (res != VK_SUCCESS) {
        return res;
    }
    std::vector<VkPhysicalDevice> gpus(gpu_count);
    res = vkEnumeratePhysicalDevices(_instances[mode], &gpu_count, gpus.data());
    if (res != VK_SUCCESS) {
        return res;
    }
    *phys_dev = gpus[0];
    return res;
}

VkResult profiles_test::VulkanInstanceBuilder::init(uint32_t apiVersion, void* pNext) {
    _layer_names.push_back("VK_LAYER_KHRONOS_profiles");

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    VkApplicationInfo app_info{GetDefaultApplicationInfo()};
    app_info.apiVersion = apiVersion;

    VkInstanceCreateInfo inst_create_info = {};
    inst_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    inst_create_info.pNext = pNext;
    inst_create_info.pApplicationInfo = &app_info;
    inst_create_info.enabledLayerCount = 0;
    inst_create_info.ppEnabledLayerNames = nullptr;
    inst_create_info.enabledExtensionCount = static_cast<uint32_t>(_extension_names.size());
    inst_create_info.ppEnabledExtensionNames = _extension_names.data();

    VkResult result = vkCreateInstance(&inst_create_info, nullptr, &_instances[MODE_NATIVE]);
    if (result != VK_SUCCESS) return result;

    inst_create_info.enabledLayerCount = static_cast<uint32_t>(_layer_names.size());
    inst_create_info.ppEnabledLayerNames = _layer_names.data();
    return vkCreateInstance(&inst_create_info, nullptr, &_instances[MODE_PROFILE]);
}

VkResult profiles_test::VulkanInstanceBuilder::init(uint32_t apiVersion) { 
    VkProfileLayerSettingsEXT settings = {};
    return this->init(apiVersion, &settings);
}

VkResult profiles_test::VulkanInstanceBuilder::init(void* pNext) {
    return this->init(GetDefaultApplicationInfo().apiVersion, pNext);
}

VkResult profiles_test::VulkanInstanceBuilder::init() { return this->init(GetDefaultApplicationInfo().apiVersion, nullptr); }

void profiles_test::VulkanInstanceBuilder::reset() {
    for (VkInstance& instance : _instances) {
        if (instance != VK_NULL_HANDLE) {
            vkDestroyInstance(instance, nullptr);
            instance = VK_NULL_HANDLE;
        }
    }

    _layer_names.clear();
    _extension_names.clear();
}
