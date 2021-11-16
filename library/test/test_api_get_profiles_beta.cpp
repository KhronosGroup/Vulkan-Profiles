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

TEST(api_get_profiles_beta, full) {
    uint32_t pPropertyCount = 0;
    vpGetProfiles(&pPropertyCount, nullptr);
    EXPECT_EQ(3, pPropertyCount);

    std::vector<VpProfileProperties> pProperties(pPropertyCount);
    vpGetProfiles(&pPropertyCount, &pProperties[0]);
    EXPECT_EQ(3, pPropertyCount);

    EXPECT_STREQ(VP_KHR_ROADMAP_2022_NAME, pProperties[0].profileName);
    EXPECT_EQ(VP_KHR_ROADMAP_2022_SPEC_VERSION, pProperties[0].specVersion);

    EXPECT_STREQ(VP_LUNARG_DESKTOP_PORTABILITY_2022_SUBSET_NAME, pProperties[1].profileName);
    EXPECT_EQ(VP_LUNARG_DESKTOP_PORTABILITY_2022_SUBSET_SPEC_VERSION, pProperties[1].specVersion);

    EXPECT_STREQ(VP_LUNARG_DESKTOP_PORTABILITY_2022_NAME, pProperties[2].profileName);
    EXPECT_EQ(VP_LUNARG_DESKTOP_PORTABILITY_2022_SPEC_VERSION, pProperties[2].specVersion);
}

TEST(api_get_profiles_beta, partial) {
    uint32_t pPropertyCount = 0;
    vpGetProfiles(&pPropertyCount, nullptr);
    EXPECT_EQ(3, pPropertyCount);

    pPropertyCount = 1;

    std::vector<VpProfileProperties> pProperties(pPropertyCount);
    vpGetProfiles(&pPropertyCount, &pProperties[0]);
    EXPECT_EQ(1, pPropertyCount);

    EXPECT_STREQ(VP_KHR_ROADMAP_2022_NAME, pProperties[0].profileName);
    EXPECT_EQ(VP_KHR_ROADMAP_2022_SPEC_VERSION, pProperties[0].specVersion);
}
