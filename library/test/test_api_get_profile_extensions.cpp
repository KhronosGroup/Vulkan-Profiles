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
#include "vulkan_profiles.hpp"

TEST(api_get_profile_extension_properties, full) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, 1};

    uint32_t propertyCount = 0;
    vpGetProfileExtensionProperties(&profile, &propertyCount, nullptr);
    EXPECT_EQ(12, propertyCount);

    std::vector<VkExtensionProperties> properties(propertyCount);
    vpGetProfileExtensionProperties(&profile, &propertyCount, &properties[0]);

    EXPECT_STREQ(VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME, properties[0].extensionName);
    EXPECT_STREQ(VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME, properties[1].extensionName);
    EXPECT_STREQ(VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME, properties[2].extensionName);
    EXPECT_STREQ(VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME, properties[3].extensionName);
    EXPECT_STREQ(VK_EXT_PRIVATE_DATA_EXTENSION_NAME, properties[4].extensionName);
    EXPECT_STREQ(VK_EXT_PIPELINE_CREATION_CACHE_CONTROL_EXTENSION_NAME, properties[5].extensionName);
    EXPECT_STREQ(VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME, properties[6].extensionName);
    EXPECT_STREQ(VK_EXT_IMAGE_ROBUSTNESS_EXTENSION_NAME, properties[7].extensionName);
    EXPECT_STREQ(VK_KHR_ZERO_INITIALIZE_WORKGROUP_MEMORY_EXTENSION_NAME, properties[8].extensionName);
    EXPECT_STREQ(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME, properties[9].extensionName);
    EXPECT_STREQ(VK_KHR_SHADER_TERMINATE_INVOCATION_EXTENSION_NAME, properties[10].extensionName);
    EXPECT_STREQ(VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME, properties[11].extensionName);
}

TEST(api_get_profile_extension_properties, partial) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, 1};

    uint32_t propertyCount = 0;
    vpGetProfileExtensionProperties(&profile, &propertyCount, nullptr);
    EXPECT_EQ(12, propertyCount);

    propertyCount = 5;

    std::vector<VkExtensionProperties> properties(propertyCount);
    vpGetProfileExtensionProperties(&profile, &propertyCount, &properties[0]);
    EXPECT_EQ(5, propertyCount);

    EXPECT_STREQ(VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME, properties[0].extensionName);
    EXPECT_STREQ(VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME, properties[1].extensionName);
    EXPECT_STREQ(VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME, properties[2].extensionName);
    EXPECT_STREQ(VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME, properties[3].extensionName);
    EXPECT_STREQ(VK_EXT_PRIVATE_DATA_EXTENSION_NAME, properties[4].extensionName);
}