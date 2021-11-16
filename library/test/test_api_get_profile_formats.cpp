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

#define VK_ENABLE_BETA_EXTENSIONS 1

#include "test.hpp"
#ifndef VULKAN_PROFILES_HEADER_ONLY
#include <vulkan/vulkan_profiles.hpp>
#else
#include <vulkan/vulkan_profiles.h>
#endif

TEST(api_get_profile_formats, full) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2022_NAME, 1};

    uint32_t formatCount = 0;
    vpGetProfileFormats(&profile, &formatCount, nullptr);
    EXPECT_EQ(66, formatCount);

    std::vector<VkFormat> formats(formatCount);
    vpGetProfileFormats(&profile, &formatCount, &formats[0]);
    EXPECT_EQ(66, formatCount);

    EXPECT_EQ(VK_FORMAT_R8_UNORM, formats[0]);
    EXPECT_EQ(VK_FORMAT_R8_SNORM, formats[1]);
    EXPECT_EQ(VK_FORMAT_R8_UINT, formats[2]);
}

TEST(api_get_profile_formats, partial) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2022_NAME, 1};

    uint32_t formatCount = 0;
    vpGetProfileFormats(&profile, &formatCount, nullptr);
    EXPECT_EQ(66, formatCount);

    formatCount = 3;

    std::vector<VkFormat> formats(formatCount);
    vpGetProfileFormats(&profile, &formatCount, &formats[0]);
    EXPECT_EQ(3, formatCount);

    EXPECT_EQ(VK_FORMAT_R8_UNORM, formats[0]);
    EXPECT_EQ(VK_FORMAT_R8_SNORM, formats[1]);
    EXPECT_EQ(VK_FORMAT_R8_UINT, formats[2]);
}

TEST(api_get_profile_formats, unspecified) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, 1};

    uint32_t formatCount = 0;
    vpGetProfileFormats(&profile, &formatCount, nullptr);
    EXPECT_EQ(0, formatCount);
}

TEST(api_get_profile_formats, properties_single) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2022_NAME, 1};

    VkFormatProperties2 properties2 = {};
    properties2.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    vpGetProfileFormatProperties(&profile, VK_FORMAT_D16_UNORM, &properties2);
    EXPECT_EQ(0, properties2.formatProperties.bufferFeatures);
    EXPECT_EQ(VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
                  VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                  VK_FORMAT_FEATURE_TRANSFER_DST_BIT,
              properties2.formatProperties.optimalTilingFeatures);
    EXPECT_EQ(0, properties2.formatProperties.linearTilingFeatures);

    VkFormatProperties3KHR properties3 = {};
    properties3.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3_KHR;
    vpGetProfileFormatProperties(&profile, VK_FORMAT_D16_UNORM, &properties3);
    EXPECT_EQ(0, properties3.bufferFeatures);
    EXPECT_EQ(VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
                  VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                  VK_FORMAT_FEATURE_TRANSFER_DST_BIT,
              properties3.optimalTilingFeatures);
    EXPECT_EQ(0, properties3.linearTilingFeatures);
}

TEST(api_get_profile_formats, properties_chained) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2022_NAME, 1};

    VkFormatProperties2 properties2 = {};
    properties2.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    properties2.pNext = nullptr;

    VkFormatProperties3KHR properties3 = {};
    properties3.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3_KHR;
    properties3.pNext = &properties2;

    vpGetProfileFormatProperties(&profile, VK_FORMAT_D16_UNORM, &properties3);

    EXPECT_EQ(0, properties2.formatProperties.bufferFeatures);
    EXPECT_EQ(VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
                  VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                  VK_FORMAT_FEATURE_TRANSFER_DST_BIT,
              properties2.formatProperties.optimalTilingFeatures);
    EXPECT_EQ(0, properties2.formatProperties.linearTilingFeatures);

    EXPECT_EQ(0, properties3.bufferFeatures);
    EXPECT_EQ(VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
                  VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                  VK_FORMAT_FEATURE_TRANSFER_DST_BIT,
              properties3.optimalTilingFeatures);
    EXPECT_EQ(0, properties3.linearTilingFeatures);
}
