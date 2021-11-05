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

TEST(test_profile, VP_KHR_1_2_roadmap_2022_support) { 
    TestScaffold scaffold; 

    const VpProfileProperties profile{VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION};

    VkBool32 supported = VK_FALSE;
    VkResult result = vpGetDeviceProfileSupport(scaffold.physicalDevice, nullptr, &profile, &supported);
    EXPECT_EQ(VK_SUCCESS, result);

#ifdef __APPLE__
    EXPECT_TRUE(supported);
#else
    EXPECT_FALSE(supported);
#endif
}

TEST(test_profile, VP_LUNARG_1_1_desktop_portability_2022_support) {
    TestScaffold scaffold;

    const VpProfileProperties profile{VP_LUNARG_DESKTOP_PORTABILITY_2022_NAME,
                                      VP_LUNARG_DESKTOP_PORTABILITY_2022_SPEC_VERSION};

    VkBool32 supported = VK_FALSE;
    VkResult result = vpGetDeviceProfileSupport(scaffold.physicalDevice, nullptr, &profile, &supported);
    EXPECT_EQ(VK_SUCCESS, result);

#ifdef __APPLE__
    EXPECT_FALSE(supported);
#else
    EXPECT_TRUE(supported);
#endif
}

TEST(test_profile, VP_LUNARG_1_1_desktop_portability_2022_subset_support) {
    TestScaffold scaffold;

    const VpProfileProperties profile{VP_LUNARG_DESKTOP_PORTABILITY_2022_SUBSET_NAME,
                                VP_LUNARG_DESKTOP_PORTABILITY_2022_SUBSET_SPEC_VERSION};

    VkBool32 supported = VK_FALSE;
    VkResult result = vpGetDeviceProfileSupport(scaffold.physicalDevice, nullptr, &profile, &supported);
    EXPECT_EQ(VK_SUCCESS, result);

#ifdef __APPLE__
    EXPECT_TRUE(supported);
#else
    EXPECT_FALSE(supported);
#endif
}
