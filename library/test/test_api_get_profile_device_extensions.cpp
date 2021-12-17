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
 * - Christophe Riccio <christophe@lunarg.com>
 */

#include "test.hpp"
#ifndef VULKAN_PROFILES_HEADER_ONLY
#include <vulkan/vulkan_profiles.hpp>
#else
#include <vulkan/vulkan_profiles.h>
#endif

TEST(api_get_profile_device_extension_properties, full) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, 1};

    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfileDeviceExtensionProperties(&profile, &propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(21, propertyCount);

    propertyCount = 23;

    std::vector<VkExtensionProperties> properties(propertyCount);
    VkResult result1 = vpGetProfileDeviceExtensionProperties(&profile, &propertyCount, &properties[0]);
    EXPECT_EQ(VK_SUCCESS, result1);
    EXPECT_EQ(21, propertyCount);

    EXPECT_STREQ(VK_KHR_8BIT_STORAGE_EXTENSION_NAME, properties[0].extensionName);
    EXPECT_STREQ(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME, properties[1].extensionName);
    EXPECT_STREQ(VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME, properties[2].extensionName);
    EXPECT_STREQ(VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME, properties[3].extensionName);
    EXPECT_STREQ(VK_KHR_IMAGE_FORMAT_LIST_EXTENSION_NAME, properties[4].extensionName);
}

TEST(api_get_profile_device_extension_properties, partial) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, 1};

    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfileDeviceExtensionProperties(&profile, &propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(21, propertyCount);

    propertyCount = 5;

    std::vector<VkExtensionProperties> properties(propertyCount);
    VkResult result1 = vpGetProfileDeviceExtensionProperties(&profile, &propertyCount, &properties[0]);
    EXPECT_EQ(VK_INCOMPLETE, result1);
    EXPECT_EQ(5, propertyCount);

    EXPECT_STREQ(VK_KHR_8BIT_STORAGE_EXTENSION_NAME, properties[0].extensionName);
    EXPECT_STREQ(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME, properties[1].extensionName);
    EXPECT_STREQ(VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME, properties[2].extensionName);
    EXPECT_STREQ(VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME, properties[3].extensionName);
    EXPECT_STREQ(VK_KHR_IMAGE_FORMAT_LIST_EXTENSION_NAME, properties[4].extensionName);
}
