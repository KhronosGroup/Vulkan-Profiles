/*
 * Copyright (C) 2015-2022 Valve Corporation
 * Copyright (C) 2015-2022 LunarG, Inc.
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
#include "vk_layer_settings.h"

enum SimulateCapabilityFlag {
    SIMULATE_API_VERSION_BIT = 1 << 0,
    SIMULATE_FEATURES_BIT = 1 << 1,
    SIMULATE_PROPERTIES_BIT = 1 << 2,
    SIMULATE_EXTENSIONS_BIT = 1 << 3,
    SIMULATE_FORMATS_BIT = 1 << 4,
    SIMULATE_ALL_CAPABILITIES = 0xFFFFFFFF
};
typedef int SimulateCapabilityFlags;

enum DebugAction {
    DEBUG_ACTION_FILE_BIT = (1 << 0),
    DEBUG_ACTION_STDOUT_BIT = (1 << 1),
    DEBUG_ACTION_OUTPUT_BIT = (1 << 2),
    DEBUG_ACTION_BREAKPOINT_BIT = (1 << 3)
};
typedef int DebugActionFlags;

static DebugActionFlags GetDebugActionFlags(const vku::Strings &values) {
    DebugActionFlags result = 0;

    for (std::size_t i = 0, n = values.size(); i < n; ++i) {
        if (values[i] == "DEBUG_ACTION_FILE_BIT") {
            result |= DEBUG_ACTION_FILE_BIT;
        } else if (values[i] == "DEBUG_ACTION_STDOUT_BIT") {
            result |= DEBUG_ACTION_STDOUT_BIT;
        } else if (values[i] == "DEBUG_ACTION_OUTPUT_BIT") {
            result |= DEBUG_ACTION_OUTPUT_BIT;
        } else if (values[i] == "DEBUG_ACTION_BREAKPOINT_BIT") {
            result |= DEBUG_ACTION_BREAKPOINT_BIT;
        }
    }

    return result;
}

static std::string GetDebugActionsLog(DebugActionFlags flags) {
    std::string result = {};

    if (flags & DEBUG_ACTION_FILE_BIT) {
        result += "DEBUG_ACTION_FILE_BIT";
    }
    if (flags & DEBUG_ACTION_STDOUT_BIT) {
        if (!result.empty()) result += ", ";
        result += "DEBUG_ACTION_STDOUT_BIT";
    }
    if (flags & DEBUG_ACTION_OUTPUT_BIT) {
        if (!result.empty()) result += ", ";
        result += "DEBUG_ACTION_OUTPUT_BIT";
    }
    if (flags & DEBUG_ACTION_BREAKPOINT_BIT) {
        if (!result.empty()) result += ", ";
        result += "DEBUG_ACTION_BREAKPOINT_BIT";
    }

    return result;
}

enum DebugReport {
    DEBUG_REPORT_NOTIFICATION_BIT = (1 << 0),
    DEBUG_REPORT_WARNING_BIT = (1 << 1),
    DEBUG_REPORT_ERROR_BIT = (1 << 2),
    DEBUG_REPORT_DEBUG_BIT = (1 << 3)
};
typedef int DebugReportFlags;

static DebugReportFlags GetDebugReportFlags(const vku::Strings &values) {
    DebugReportFlags result = 0;

    for (std::size_t i = 0, n = values.size(); i < n; ++i) {
        if (values[i] == "DEBUG_REPORT_NOTIFICATION_BIT") {
            result |= DEBUG_REPORT_NOTIFICATION_BIT;
        } else if (values[i] == "DEBUG_REPORT_WARNING_BIT") {
            result |= DEBUG_REPORT_WARNING_BIT;
        } else if (values[i] == "DEBUG_REPORT_ERROR_BIT") {
            result |= DEBUG_REPORT_ERROR_BIT;
        } else if (values[i] == "DEBUG_REPORT_DEBUG_BIT") {
            result |= DEBUG_REPORT_DEBUG_BIT;
        }
    }

    return result;
}


static const VkStructureType VK_STRUCTURE_TYPE_PROFILES_LAYER_SETTINGS_EXT = static_cast<VkStructureType>(3000300005);

typedef struct VkProfileLayerSettingsEXT {
    VkProfileLayerSettingsEXT()
    : sType(VK_STRUCTURE_TYPE_PROFILES_LAYER_SETTINGS_EXT) {}

    VkStructureType sType;
    void* pNext{};
    std::string profile_file{};
    std::string profile_name{"${VP_DEFAULT}"};
    bool profile_validation{false};
    bool emulate_portability{true};
    SimulateCapabilityFlags simulate_capabilities{SIMULATE_API_VERSION_BIT | SIMULATE_FEATURES_BIT | SIMULATE_PROPERTIES_BIT};
    DebugActionFlags debug_actions{DEBUG_ACTION_STDOUT_BIT};
    std::string debug_filename{"profiles_layer_log.txt"};
    bool debug_file_discard{true};
    DebugReportFlags debug_reports{DEBUG_REPORT_WARNING_BIT | DEBUG_REPORT_ERROR_BIT};
    bool debug_fail_on_error{false};
    vku::Strings exclude_device_extensions;
    vku::Strings exclude_formats;
} VkProfileLayerSettingsEXT;
