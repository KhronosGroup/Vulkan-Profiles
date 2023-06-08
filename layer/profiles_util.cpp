/*
 * Copyright (C) 2022-2023 Valve Corporation
 * Copyright (C) 2022-2023 LunarG, Inc.
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
 * Author: Christophe Riccio <christophe@lunarg.com>
 */

#include "profiles_util.h"
#include "profiles_settings.h"

//void LayerSettingsLog(const char* pSettingName, const char* pMessage) {
//    LogMessage(DEBUG_REPORT_ERROR_BIT, "%s : %s\n", pSettingName, pMessage);
//}

std::string format(const char *message, ...) {
    std::size_t const STRING_BUFFER(4096);

    assert(message != nullptr);
    assert(strlen(message) >= 0 && strlen(message) < STRING_BUFFER);

    char buffer[STRING_BUFFER];
    va_list list;

    va_start(list, message);
    vsnprintf(buffer, STRING_BUFFER, message, list);
    va_end(list);

    return buffer;
}

std::string ToLower(const std::string &s) {
    std::string result = s;
    for (auto &c : result) {
        c = (char)std::tolower(c);
    }
    return result;
}

std::string ToUpper(const std::string &s) {
    std::string result = s;
    for (auto &c : result) {
        c = (char)std::toupper(c);
    }
    return result;
}
/*
std::string GetString(const List &list) {
    std::string result;
    for (std::size_t i = 0, n = list.size(); i < n; ++i) {
        result += list[i].first;
        if (i < n - 1) result += ", ";
    }
    return result;
}
*/
std::string GetString(const std::vector<std::string> &strings) {
    std::string result;
    for (std::size_t i = 0, n = strings.size(); i < n; ++i) {
        result += strings[i];
        if (i < n - 1) result += ", ";
    }
    return result;
}

std::string GetUUIDString(const uint8_t deviceUUID[VK_UUID_SIZE]) {
    std::string result;

    for (std::size_t i = 0, n = VK_UUID_SIZE; i < n; ++i) {
        result += format("%02X", deviceUUID[i]);
    }

    return result;
}

std::string format_device_support_string(VkFormatFeatureFlags format_features) {
    if (format_features == 0) return std::string("does not support it");
    return ::format("only supports:\n\t\" % s\"", GetFormatFeatureString(format_features).c_str());
}

std::string format_device_support_string(VkFormatFeatureFlags2 format_features) {
    if (format_features == 0) return std::string("does not support it");
    return ::format("only supports:\n\t\" % s\"", GetFormatFeature2String(format_features).c_str());
}

VkResult EnumerateExtensions(const MapOfVkExtensionProperties &source, uint32_t *dst_count, VkExtensionProperties *dst_props) {
    assert(dst_count);
    if (!dst_props) {
        *dst_count = static_cast<uint32_t>(source.size());
        return VK_SUCCESS;
    }
    uint32_t count_written = 0;
    uint32_t src_count = static_cast<uint32_t>(source.size());
    const uint32_t copy_count = (*dst_count < src_count) ? *dst_count : src_count;
    *dst_count = copy_count;
    for (const auto &[ext_name, ext] : source) {
        dst_props[count_written] = ext;
        count_written++;

        if (count_written == *dst_count) {
            break;
        }
    }
    return (copy_count == src_count) ? VK_SUCCESS : VK_INCOMPLETE;
}

bool QueueFamilyMatch(const VkQueueFamilyProperties &device, const VkQueueFamilyProperties &profile) {
    if ((device.queueFlags & profile.queueFlags) != profile.queueFlags) {
        return false;
    } else if (device.queueCount < profile.queueCount) {
        return false;
    } else if (device.timestampValidBits < profile.timestampValidBits) {
        return false;
    } else if (profile.minImageTransferGranularity.width > 0 &&
               device.minImageTransferGranularity.width > profile.minImageTransferGranularity.width) {
        return false;
    } else if (profile.minImageTransferGranularity.height > 0 &&
               device.minImageTransferGranularity.height > profile.minImageTransferGranularity.height) {
        return false;
    } else if (profile.minImageTransferGranularity.depth > 0 &&
               device.minImageTransferGranularity.depth > profile.minImageTransferGranularity.depth) {
        return false;
    }
    return true;
}

bool GlobalPriorityMatch(const VkQueueFamilyGlobalPriorityPropertiesKHR &device,
                         const VkQueueFamilyGlobalPriorityPropertiesKHR &profile) {
    if (profile.priorityCount == 0) {
        return true;
    } else if (device.priorityCount != profile.priorityCount) {
        return false;
    }

    bool match = true;
    for (uint32_t i = 0; i < device.priorityCount; ++i) {
        if (device.priorities[i] != profile.priorities[i]) {
            match = false;
            break;
        }
    }
    return match;
}
