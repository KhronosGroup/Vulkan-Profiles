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

#pragma once

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cinttypes>
#include <cstring>
#include <csignal>

#include <functional>
#include <unordered_map>
#include <vector>
#include <array>
#include <fstream>
#include <mutex>
#include <sstream>

#include "vulkan/vk_layer.h"
#include "vulkan/vulkan_beta.h"
#include "vulkan/vulkan_screen.h"
#include "vk_layer_table.h"
#include "generated/vk_enum_string_helper.h"
#include <vulkan/layer/vk_layer_settings.h>

#include "profiles.h"

//typedef std::vector<std::pair<std::string, int>> List;

//void LayerSettingsLog(const char *pSettingName, const char *pMessage);

std::string format(const char *message, ...);

//std::string GetString(const List &list);
std::string GetString(const std::vector<std::string> &strings);
std::string GetUUIDString(const uint8_t deviceUUID[VK_UUID_SIZE]);

std::string format_device_support_string(VkFormatFeatureFlags format_features);
std::string format_device_support_string(VkFormatFeatureFlags2 format_features);

inline bool IsFormatSupported(const VkFormatProperties &props) {
    // Per [SPEC] section 30.3.2 "Format Properties":
    // "... if no format feature flags are supported, the format itself is not supported ..."
    return !(!props.linearTilingFeatures && !props.optimalTilingFeatures && !props.bufferFeatures);
}

typedef std::unordered_map<uint32_t /*VkFormat*/, VkFormatProperties> MapOfVkFormatProperties;
typedef std::unordered_map<uint32_t /*VkFormat*/, VkFormatProperties3> MapOfVkFormatProperties3;
typedef std::unordered_map<uint32_t /*VkFormat*/, VkDrmFormatModifierPropertiesList2EXT> MapOfVkDrmFormatModifierProperties;
typedef std::vector<VkExtensionProperties> ArrayOfVkExtensionProperties;
typedef std::unordered_map<std::string, VkExtensionProperties> MapOfVkExtensionProperties;

struct QueueFamilyProperties {
    VkQueueFamilyProperties2 properties_2 = {};
    VkQueueFamilyGlobalPriorityPropertiesKHR global_priority_properties_ = {};
    VkQueueFamilyVideoPropertiesKHR video_properties_ = {};
    VkQueueFamilyCheckpointPropertiesNV checkpoint_properties_ = {};
    VkQueueFamilyCheckpointProperties2NV checkpoint_properties_2_ = {};
    VkQueueFamilyQueryResultStatusPropertiesKHR query_result_status_properties_ = {};

    QueueFamilyProperties() {
        properties_2.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
        global_priority_properties_.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_GLOBAL_PRIORITY_PROPERTIES_EXT;
        video_properties_.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_VIDEO_PROPERTIES_KHR;
        checkpoint_properties_.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_CHECKPOINT_PROPERTIES_NV;
        checkpoint_properties_2_.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_CHECKPOINT_PROPERTIES_2_NV;
        query_result_status_properties_.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_QUERY_RESULT_STATUS_PROPERTIES_KHR;
    }
};

typedef std::vector<QueueFamilyProperties> ArrayOfVkQueueFamilyProperties;

// Get all elements from a vkEnumerate*() lambda into a std::vector.
template <typename T>
VkResult EnumerateAll(std::vector<T> &vect, std::function<VkResult(uint32_t *, T *)> func) {
    VkResult result = VK_INCOMPLETE;
    do {
        uint32_t count = 0;
        result = func(&count, nullptr);
        assert(result == VK_SUCCESS);
        vect.resize(count);
        result = func(&count, vect.data());
    } while (result == VK_INCOMPLETE);
    return result;
}

template <typename T>
static VkResult EnumerateProperties(uint32_t src_count, const T *src_props, uint32_t *dst_count, T *dst_props) {
    assert(dst_count);
    if (!dst_props || !src_props) {
        *dst_count = src_count;
        return VK_SUCCESS;
    }

    const uint32_t copy_count = (*dst_count < src_count) ? *dst_count : src_count;
    memcpy(dst_props, src_props, copy_count * sizeof(T));
    *dst_count = copy_count;
    return (copy_count == src_count) ? VK_SUCCESS : VK_INCOMPLETE;
}

VkResult EnumerateExtensions(const MapOfVkExtensionProperties &source, uint32_t *dst_count, VkExtensionProperties *dst_props);

bool QueueFamilyMatch(const VkQueueFamilyProperties &device, const VkQueueFamilyProperties &profile);

bool GlobalPriorityMatch(const VkQueueFamilyGlobalPriorityPropertiesKHR &device,
                         const VkQueueFamilyGlobalPriorityPropertiesKHR &profile);

inline bool HasFlags(VkFlags deviceFlags, VkFlags profileFlags) {
    return (deviceFlags & profileFlags) == profileFlags;
}

inline bool HasFlags(VkFlags64 deviceFlags, VkFlags64 profileFlags) {
    return (deviceFlags & profileFlags) == profileFlags;
}

std::string ToLower(const std::string &s);

std::string ToUpper(const std::string &s);
