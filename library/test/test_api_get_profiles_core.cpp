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

//#define VK_ENABLE_BETA_EXTENSIONS <= checking the library build and report the right number of profiles when it's not defined

#include "test.hpp"
#ifndef VULKAN_PROFILES_HEADER_ONLY
#include <vulkan/vulkan_profiles.hpp>
#else
#include <vulkan/vulkan_profiles.h>
#endif

auto containsProfile = [](const std::vector<VpProfileProperties>& vec, const char* name, uint32_t version) {
    for (std::size_t i = 0; i < vec.size(); ++i)
        if (strcmp(vec[i].profileName, name) == 0 && vec[i].specVersion == version) return true;
    return false;
};

TEST(api_get_profiles_core, full) {
    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfiles(&propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(2, propertyCount);

    propertyCount = 3;

    std::vector<VpProfileProperties> properties(propertyCount);
    VkResult result1 = vpGetProfiles(&propertyCount, &properties[0]);
    EXPECT_EQ(VK_SUCCESS, result1);
    EXPECT_EQ(2, propertyCount);

    EXPECT_TRUE(containsProfile(properties, VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION));
    EXPECT_TRUE(containsProfile(properties, VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, VP_LUNARG_DESKTOP_PORTABILITY_2021_SPEC_VERSION));
}

TEST(api_get_profiles_core, partial) {
    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfiles(&propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(2, propertyCount);

    propertyCount = 1;

    std::vector<VpProfileProperties> properties(propertyCount);
    properties[0].specVersion = 0;
    VkResult result1 = vpGetProfiles(&propertyCount, &properties[0]);
    EXPECT_EQ(VK_INCOMPLETE, result1);
    EXPECT_EQ(1, propertyCount);

    EXPECT_NE(properties[0].specVersion, 0);
}
