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

TEST(api_get_profile_fallbacks, full) {
    const VpProfileProperties profile_subset = {VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME, 1};
    const VpProfileProperties profile_expect = {VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, 1};

    uint32_t count = 0;
    VkResult result0 = vpGetProfileFallbacks(&profile_subset, &count, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(1, count);

    count = 2;

    std::vector<VpProfileProperties> data(count);
    VkResult result1 = vpGetProfileFallbacks(&profile_subset, &count, &data[0]);
    EXPECT_EQ(VK_SUCCESS, result1);
    EXPECT_EQ(1, count);

    EXPECT_EQ(profile_expect.specVersion, data[0].specVersion);
    EXPECT_STREQ(profile_expect.profileName, data[0].profileName);
}

TEST(api_get_profile_fallbacks, partial) {
    const VpProfileProperties profile_subset = {VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME, 1};

    uint32_t count = 0;
    VkResult result0 = vpGetProfileFallbacks(&profile_subset, &count, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(1, count);

    std::vector<VpProfileProperties> data(count);
    
    count = 0;

    VkResult result1 = vpGetProfileFallbacks(&profile_subset, &count, &data[0]);
    EXPECT_EQ(VK_INCOMPLETE, result1);
    EXPECT_EQ(0, count);

    EXPECT_EQ(0, data[0].specVersion);
    EXPECT_STREQ("", data[0].profileName);
}
