/* Copyright (c) 2022 The Khronos Group Inc.
 * Copyright (c) 2022 Valve Corporation
 * Copyright (c) 2022 LunarG, Inc.
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
 *
 * Author: Mark Lobodzinski <mark@lunarg.com>
 */

#pragma once
#include <string>
#include "vulkan/vulkan.h"
#include "profiles.h"
#include "../layer-utils/vk_layer_settings.h"

static const VkStructureType VK_STRUCTURE_TYPE_PROFILES_LAYER_SETTINGS_EXT = static_cast<VkStructureType>(3000300005);

typedef struct VkProfileLayerSettingsEXT {
    VkProfileLayerSettingsEXT()
    : sType(VK_STRUCTURE_TYPE_PROFILES_LAYER_SETTINGS_EXT) {}

    VkStructureType sType;
    void* pNext{};
    std::string profile_file{};
    std::string profile_name{};
    bool profile_validation{false};
    bool emulate_portability{true};
    SimulateCapabilityFlags simulate_capabilities{SIMULATE_API_VERSION_BIT | SIMULATE_FEATURES_BIT | SIMULATE_PROPERTIES_BIT};
    DebugActionFlags debug_actions{DEBUG_ACTION_STDOUT_BIT};
    std::string debug_filename{"profiles_layer_log.txt"};
    bool debug_file_discard{true};
    DebugReportFlags debug_reports{DEBUG_REPORT_WARNING_BIT | DEBUG_REPORT_ERROR_BIT};
    bool debug_fail_on_error{false};
    // VK_KHRONOS_PROFILES_DEBUG_REPORTS -- default is not set
    vku::Strings exclude_device_extensions;
    vku::Strings exclude_formats;
} VkProfileLayerSettingsEXT;



