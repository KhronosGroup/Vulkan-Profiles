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

TEST(api_get_profile_memory_types, full) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, 1};

    uint32_t memoryPropertyFlagsCount = 0;
    VkResult result0 = vpGetProfileMemoryTypes(&profile, &memoryPropertyFlagsCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(2, memoryPropertyFlagsCount);

    std::vector<VkMemoryPropertyFlags> memoryPropertyFlags(memoryPropertyFlagsCount);
    VkResult result1 = vpGetProfileMemoryTypes(&profile, &memoryPropertyFlagsCount, &memoryPropertyFlags[0]);
    EXPECT_EQ(VK_SUCCESS, result1);
    EXPECT_EQ(2, memoryPropertyFlagsCount);

    EXPECT_EQ(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memoryPropertyFlags[0]);
    EXPECT_EQ(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
              memoryPropertyFlags[1]);
}

TEST(api_get_profile_memory_types, partial) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, 1};

    uint32_t memoryPropertyFlagsCount = 0;
    VkResult result0 = vpGetProfileMemoryTypes(&profile, &memoryPropertyFlagsCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(2, memoryPropertyFlagsCount);

    memoryPropertyFlagsCount = 1;

    std::vector<VkMemoryPropertyFlags> memoryPropertyFlags(memoryPropertyFlagsCount);
    VkResult result1 = vpGetProfileMemoryTypes(&profile, &memoryPropertyFlagsCount, &memoryPropertyFlags[0]);
    EXPECT_EQ(VK_INCOMPLETE, result1);
    EXPECT_EQ(1, memoryPropertyFlagsCount);

    EXPECT_EQ(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memoryPropertyFlags[0]);
}

TEST(api_get_profile_memory_types, unspecified) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, 1};

    uint32_t memoryPropertyFlagsCount = 0;
    VkResult result0 = vpGetProfileMemoryTypes(&profile, &memoryPropertyFlagsCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(0, memoryPropertyFlagsCount);
}
