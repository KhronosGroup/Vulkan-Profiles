/*
 * Copyright (c) 2020-2021 Valve Corporation
 * Copyright (c) 2020-2021 LunarG, Inc.
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

std::string profiles_test::setCombinationModeToString(SetCombinationMode mode) {
    switch (mode) {
        case SetCombinationMode::SET_CHECK_SUPPORT:
            return "check_support";
        case SetCombinationMode::SET_FROM_PROFILE:
            return "from_profile";
        case SetCombinationMode::SET_FROM_DEVICE:
            return "from_device";
        default:
            break;
    }

    return "check_support";
}

void profiles_test::setDevsimFilenames(std::vector<std::string>& filepaths) {
    std::stringstream out;
#ifdef _WIN32
    char seperator = ';';
#else
    char seperator = ':';
#endif // _WIN32

    for (std::string& filepath : filepaths) {
        std::string config_filepath = profiles_test::getAbsolutePath(filepath);
        out << filepath << seperator;
    }
    profiles_test::setEnvironmentSetting("VK_KHRONOS_PROFILES_FILENAME", out.str().c_str());
}

void profiles_test::setDevsimDebugEnable(bool enable) {
    if (enable)
        profiles_test::setEnvironmentSetting("VK_KHRONOS_PROFILES_DEBUG_ENABLE", "true");
    else
        profiles_test::setEnvironmentSetting("VK_KHRONOS_PROFILES_DEBUG_ENABLE", "false");
}

void profiles_test::setDevsimExitOnError(bool enable) {
    if (enable)
        profiles_test::setEnvironmentSetting("VK_KHRONOS_PROFILES_EXIT_ON_ERROR", "true");
    else
        profiles_test::setEnvironmentSetting("VK_KHRONOS_PROFILES_EXIT_ON_ERROR", "false");
}

void profiles_test::setDevsimEmulatePortabilitySubsetExtension(bool enable) {
    if (enable)
        profiles_test::setEnvironmentSetting("VK_KHRONOS_PROFILES_EMULATE_PORTABILITY", "true");
    else
        profiles_test::setEnvironmentSetting("VK_KHRONOS_PROFILES_EMULATE_PORTABILITY", "false");
}

void profiles_test::setDevsimModifyExtensionList(SetCombinationMode mode) {
    profiles_test::setEnvironmentSetting("VK_KHRONOS_PROFILES_MODIFY_EXTENSION_LIST",
                                       setCombinationModeToString(mode).c_str());
}

void profiles_test::setDevsimModifyMemoryFlags(bool enable) {
    if (enable)
        profiles_test::setEnvironmentSetting("VK_KHRONOS_PROFILES_MODIFY_MEMORY_FLAGS", "true");
    else
        profiles_test::setEnvironmentSetting("VK_KHRONOS_PROFILES_MODIFY_MEMORY_FLAGS", "false");
}

void profiles_test::setDevsimModifyFormatList(SetCombinationMode mode) {
    profiles_test::setEnvironmentSetting("VK_KHRONOS_PROFILES_MODIFY_FORMAT_LIST",
                                       setCombinationModeToString(mode).c_str());
}

void profiles_test::setDevsimModifyFormatProperties(SetCombinationMode mode) {
    profiles_test::setEnvironmentSetting("VK_KHRONOS_PROFILES_MODIFY_FORMAT_PROPERTIES",
                                       setCombinationModeToString(mode).c_str());
}

void profiles_test::setDevsimModifySurfaceFormats(SetCombinationMode mode) {
    profiles_test::setEnvironmentSetting("VK_KHRONOS_PROFILES_MODIFY_SURFACE_FORMAT",
                                       setCombinationModeToString(mode).c_str());
}

void profiles_test::setDevsimModifyPresentModes(SetCombinationMode mode) {
    profiles_test::setEnvironmentSetting("VK_KHRONOS_PROFILES_MODIFY_PRESENT_MODES",
                                       setCombinationModeToString(mode).c_str());
}

void profiles_test::setDevsimProfileName(const std::string& profile) {
    profiles_test::setEnvironmentSetting("VK_KHRONOS_PROFILES_PROFILE_NAME", profile.c_str());
}

VkApplicationInfo profiles_test::GetDefaultApplicationInfo() {
    VkApplicationInfo out{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
    out.apiVersion = VK_API_VERSION_1_0;
    out.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    out.pApplicationName = "devsim_tests";
    out.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    out.pEngineName = "devsim_tests";

    return out;
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