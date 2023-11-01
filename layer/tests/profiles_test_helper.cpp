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
#elif __ANDROID__
#include <android/log.h>
#endif

#include "profiles_test_helper.h"

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

VkResult profiles_test::VulkanInstanceBuilder::init(uint32_t apiVersion, const std::vector<VkLayerSettingEXT>& settings) {
    _layer_names.push_back(kLayerName);

    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", TEST_BINARY_PATH);

    VkApplicationInfo app_info{GetDefaultApplicationInfo()};
    app_info.apiVersion = apiVersion;
    

    VkInstanceCreateInfo inst_create_info = {};

#ifdef __APPLE__
    _extension_names.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    inst_create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

    inst_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    inst_create_info.pApplicationInfo = &app_info;
    inst_create_info.pNext = nullptr;
    inst_create_info.enabledLayerCount = 0;
    inst_create_info.ppEnabledLayerNames = nullptr;
    inst_create_info.enabledExtensionCount = _extension_names.empty() ? 0 : static_cast<uint32_t>(_extension_names.size());
    inst_create_info.ppEnabledExtensionNames = _extension_names.empty() ? nullptr : _extension_names.data();

    VkResult result = vkCreateInstance(&inst_create_info, nullptr, &_instances[MODE_NATIVE]);
    if (result != VK_SUCCESS) return result;

    VkLayerSettingsCreateInfoEXT layer_settings_create_info{
        VK_STRUCTURE_TYPE_LAYER_SETTINGS_CREATE_INFO_EXT, nullptr,
        settings.empty() ? 0 : static_cast<uint32_t>(settings.size()),
        settings.empty() ? nullptr : &settings[0]};

    inst_create_info.enabledLayerCount = static_cast<uint32_t>(_layer_names.size());
    inst_create_info.ppEnabledLayerNames = _layer_names.data();
    inst_create_info.pNext = settings.empty() ? nullptr : &layer_settings_create_info;
    return vkCreateInstance(&inst_create_info, nullptr, &_instances[MODE_PROFILE]);
}

VkResult profiles_test::VulkanInstanceBuilder::init(uint32_t apiVersion) { 
    return this->init(apiVersion, std::vector<VkLayerSettingEXT>());
}

VkResult profiles_test::VulkanInstanceBuilder::init(const std::vector<VkLayerSettingEXT>& settings) {
    return this->init(GetDefaultApplicationInfo().apiVersion, settings);
}

VkResult profiles_test::VulkanInstanceBuilder::init() {
    return this->init(GetDefaultApplicationInfo().apiVersion, std::vector<VkLayerSettingEXT>());
}

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

bool profiles_test::IsExtensionSupported(VkPhysicalDevice physical_device, const char* extension_name) {
    uint32_t property_count = 0;
    vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &property_count, nullptr);
    if (property_count == 0) return false;

    std::vector<VkExtensionProperties> properties(static_cast<std::size_t>(property_count));
    vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &property_count, &properties[0]);

    for (std::size_t i = 0, n = properties.size(); i < n; ++i) {
        if (strcmp(properties[i].extensionName, extension_name) == 0) {
            return true;
        }
    }

    return false;
}
