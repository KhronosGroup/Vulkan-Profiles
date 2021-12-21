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

TEST(api_get_profile_instance_extension_properties, full) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, 1};

    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfileInstanceExtensionProperties(&profile, &propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(0, propertyCount);

    propertyCount = 2;

    std::vector<VkExtensionProperties> properties(propertyCount);
    VkResult result1 = vpGetProfileInstanceExtensionProperties(&profile, &propertyCount, &properties[0]);
    EXPECT_EQ(VK_SUCCESS, result1);
    EXPECT_EQ(0, propertyCount);
}

