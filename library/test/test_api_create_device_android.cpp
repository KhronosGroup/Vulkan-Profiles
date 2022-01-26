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
#include <vulkan/vulkan_android.h>
#include <vulkan/debug/vulkan_profiles.hpp>

TEST(library_api, vpCreateDevice) {
    TestScaffold scaffold;

    const VpProfileProperties profile = {VP_ANDROID_BASELINE_2021_NAME, VP_ANDROID_BASELINE_2021_SPEC_VERSION};

    VkDeviceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pNext = nullptr;
    info.queueCreateInfoCount = 1;
    info.pQueueCreateInfos = &scaffold.queueCreateInfo;
    info.enabledExtensionCount = 0;
    info.ppEnabledExtensionNames = nullptr;
    info.pEnabledFeatures = nullptr;

    VpDeviceCreateInfo profileInfo = {};
    profileInfo.pCreateInfo = &info;
    profileInfo.pProfile = &profile;

    VkDevice device = VK_NULL_HANDLE;
    VkResult res = vpCreateDevice(scaffold.physicalDevice, &profileInfo, nullptr, &device);
#ifdef __ANDROID_API__
    EXPECT_TRUE(res == VK_SUCCESS);
    EXPECT_TRUE(device != VK_NULL_HANDLE);
#else
    EXPECT_TRUE(res != VK_SUCCESS);
#endif
}

TEST(library_api, vpGetPhysicalDeviceProfileSupport) {
    TestScaffold scaffold;

    VpProfileProperties profile{VP_ANDROID_BASELINE_2021_NAME, VP_ANDROID_BASELINE_2021_SPEC_VERSION};

    VkBool32 supported = VK_FALSE;
    vpGetPhysicalDeviceProfileSupport(scaffold.instance, scaffold.physicalDevice, &profile, &supported);

#ifdef __ANDROID_API__
    EXPECT_EQ(VK_TRUE, supported);
#else
    EXPECT_EQ(VK_FALSE, supported);
#endif
}
