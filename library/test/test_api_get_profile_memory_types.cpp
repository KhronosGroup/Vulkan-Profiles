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

TEST(api_get_profile_memory_types, full) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2022_NAME, 1};

    uint32_t memoryPropertyFlagsCount = 0;
    vpGetProfileMemoryTypes(&profile, &memoryPropertyFlagsCount, nullptr);
    EXPECT_EQ(2, memoryPropertyFlagsCount);

    std::vector<VkMemoryPropertyFlags> memoryPropertyFlags(memoryPropertyFlagsCount);
    vpGetProfileMemoryTypes(&profile, &memoryPropertyFlagsCount, &memoryPropertyFlags[0]);
    EXPECT_EQ(2, memoryPropertyFlagsCount);

    EXPECT_EQ(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memoryPropertyFlags[0]);
    EXPECT_EQ(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
              memoryPropertyFlags[1]);
}

TEST(api_get_profile_memory_types, partial) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2022_NAME, 1};

    uint32_t memoryPropertyFlagsCount = 0;
    vpGetProfileMemoryTypes(&profile, &memoryPropertyFlagsCount, nullptr);
    EXPECT_EQ(2, memoryPropertyFlagsCount);

    memoryPropertyFlagsCount = 1;

    std::vector<VkMemoryPropertyFlags> memoryPropertyFlags(memoryPropertyFlagsCount);
    vpGetProfileMemoryTypes(&profile, &memoryPropertyFlagsCount, &memoryPropertyFlags[0]);
    EXPECT_EQ(1, memoryPropertyFlagsCount);

    EXPECT_EQ(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memoryPropertyFlags[0]);
}

TEST(api_get_profile_memory_types, unspecified) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, 1};

    uint32_t memoryPropertyFlagsCount = 0;
    vpGetProfileMemoryTypes(&profile, &memoryPropertyFlagsCount, nullptr);
    EXPECT_EQ(0, memoryPropertyFlagsCount);
}
