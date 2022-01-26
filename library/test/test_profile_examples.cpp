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
#include <vulkan/debug/vulkan_profiles.hpp>
#else
#include <vulkan/debug/vulkan_profiles.h>
#endif

#define countof(arr) sizeof(arr) / sizeof(arr[0])

static TestScaffold* scaffold = nullptr;

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    ::scaffold = new TestScaffold;

    int result = RUN_ALL_TESTS();

    delete ::scaffold;
    ::scaffold = nullptr;

    return result;
}

#if defined(VK_VERSION_1_2) || defined(VK_KHR_maintenance3)
// In this example, a typical usage of profiles with additional features and extensions
TEST(test_profile, example_add_features_add_extensions) {
    VpProfileProperties profile{VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, VP_LUNARG_DESKTOP_PORTABILITY_2021_SPEC_VERSION};

    VkBool32 supported = VK_FALSE;
    vpGetPhysicalDeviceProfileSupport(scaffold->instance, scaffold->physicalDevice, &profile, &supported);
    EXPECT_EQ(VK_TRUE, supported);

    static const char* extensions[] = {VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME, VK_KHR_MAINTENANCE3_EXTENSION_NAME};

    VkPhysicalDeviceFeatures2 deviceFeatures2 = {};
    deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    deviceFeatures2.features.depthBiasClamp = VK_TRUE;
    deviceFeatures2.features.depthBounds = VK_FALSE;
    deviceFeatures2.features.depthClamp = VK_TRUE;

    VkDeviceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pNext = &deviceFeatures2;
    info.queueCreateInfoCount = 1;
    info.pQueueCreateInfos = &scaffold->queueCreateInfo;
    info.pEnabledFeatures = nullptr;
    info.enabledExtensionCount = countof(extensions);
    info.ppEnabledExtensionNames = extensions;

    VpDeviceCreateInfo profileInfo = {};
    profileInfo.pCreateInfo = &info;
    profileInfo.pProfile = &profile;
    profileInfo.flags = VP_DEVICE_CREATE_MERGE_EXTENSIONS_BIT | VP_DEVICE_CREATE_OVERRIDE_FEATURES_BIT;

    VkDevice device = VK_NULL_HANDLE;
    VkResult res = vpCreateDevice(scaffold->physicalDevice, &profileInfo, nullptr, &device);
    EXPECT_EQ(VK_SUCCESS, res);
}
#endif

#if defined(VK_VERSION_1_1) || defined(VK_EXT_subgroup_size_control)
// In this example, we are using vpGetProfileStructures to initialize each application structure individually
TEST(test_profile, example_individual_override_features) {
    VpProfileProperties profile{VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, VP_LUNARG_DESKTOP_PORTABILITY_2021_SPEC_VERSION};

    VkBool32 supported = VK_FALSE;
    vpGetPhysicalDeviceProfileSupport(scaffold->instance, scaffold->physicalDevice, &profile, &supported);
    EXPECT_EQ(VK_TRUE, supported);

    // This structure is not part of the profile, so it will remains unchanged by vpGetProfileStructures
    VkPhysicalDeviceSubgroupSizeControlFeaturesEXT deviceSubgroupFeatures = {};
    deviceSubgroupFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES_EXT;
    vpGetProfileFeatures(&profile, &deviceSubgroupFeatures);
    deviceSubgroupFeatures.subgroupSizeControl = VK_TRUE;

    // This structure is not part of the profile, so it will be updated by vpGetProfileStructures
    VkPhysicalDeviceFeatures2 deviceFeatures2 = {};
    deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    vpGetProfileFeatures(&profile, &deviceFeatures2);
    deviceFeatures2.pNext = &deviceSubgroupFeatures;
    deviceFeatures2.features.robustBufferAccess = VK_FALSE;

    VkDeviceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pNext = &deviceFeatures2;
    info.queueCreateInfoCount = 1;
    info.pQueueCreateInfos = &scaffold->queueCreateInfo;

    VpDeviceCreateInfo profileInfo = {};
    profileInfo.pCreateInfo = &info;
    profileInfo.pProfile = &profile;
    profileInfo.flags = VP_DEVICE_CREATE_OVERRIDE_FEATURES_BIT;

    VkDevice device = VK_NULL_HANDLE;
    VkResult res = vpCreateDevice(scaffold->physicalDevice, &profileInfo, nullptr, &device);
    EXPECT_EQ(VK_SUCCESS, res);
}
#endif

#if defined(VK_VERSION_1_1) || defined(VK_EXT_subgroup_size_control)
// In this example, we are using vpGetProfileStructures to initial the entire application structure chain
TEST(test_profile, example_collective_override_features) {
    VpProfileProperties profile{VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, VP_LUNARG_DESKTOP_PORTABILITY_2021_SPEC_VERSION};

    VkBool32 supported = VK_FALSE;
    vpGetPhysicalDeviceProfileSupport(scaffold->instance, scaffold->physicalDevice, &profile, &supported);
    EXPECT_EQ(VK_TRUE, supported);

    // This structure is not part of the profile, so it will remains unchanged by vpGetProfileStructures
    VkPhysicalDeviceSubgroupSizeControlFeaturesEXT deviceSubgroupFeatures = {};
    deviceSubgroupFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES_EXT;

    // This structure is not part of the profile, so it will be updated by vpGetProfileStructures
    VkPhysicalDeviceFeatures2 deviceFeatures2 = {};
    deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    deviceFeatures2.pNext = &deviceSubgroupFeatures;

    // Load the profile definition
    vpGetProfileFeatures(&profile, &deviceFeatures2);

    deviceSubgroupFeatures.subgroupSizeControl = VK_TRUE;
    deviceFeatures2.features.robustBufferAccess = VK_FALSE;

    VkDeviceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pNext = &deviceFeatures2;
    info.queueCreateInfoCount = 1;
    info.pQueueCreateInfos = &scaffold->queueCreateInfo;

    VpDeviceCreateInfo profileInfo = {};
    profileInfo.pCreateInfo = &info;
    profileInfo.pProfile = &profile;
    profileInfo.flags = VP_DEVICE_CREATE_OVERRIDE_FEATURES_BIT;

    VkDevice device = VK_NULL_HANDLE;
    VkResult res = vpCreateDevice(scaffold->physicalDevice, &profileInfo, nullptr, &device);
    EXPECT_EQ(VK_SUCCESS, res);
}
#endif

// In this example, we are using VP_DEVICE_CREATE_DISABLE_ROBUST_BUFFER_ACCESS_BIT to disable robustBufferAccess
TEST(test_profile, example_flag_disable_robust_access) {
    VpProfileProperties profile{VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, VP_LUNARG_DESKTOP_PORTABILITY_2021_SPEC_VERSION};

    VkBool32 supported = VK_FALSE;
    vpGetPhysicalDeviceProfileSupport(scaffold->instance, scaffold->physicalDevice, &profile, &supported);
    EXPECT_EQ(VK_TRUE, supported);

    // Regardness of robustBufferAccess = VK_TRUE in the profile, because of VP_DEVICE_CREATE_DISABLE_ROBUST_BUFFER_ACCESS_BIT is set
    // robustBufferAccess is effectively set to VK_FALSE when creating the VkDevice

    VkDeviceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.queueCreateInfoCount = 1;
    info.pQueueCreateInfos = &scaffold->queueCreateInfo;

    VpDeviceCreateInfo profileInfo = {};
    profileInfo.pCreateInfo = &info;
    profileInfo.pProfile = &profile;
    profileInfo.flags = VP_DEVICE_CREATE_DISABLE_ROBUST_BUFFER_ACCESS_BIT;

    VkDevice device = VK_NULL_HANDLE;
    VkResult res = vpCreateDevice(scaffold->physicalDevice, &profileInfo, nullptr, &device);
    EXPECT_EQ(VK_SUCCESS, res);
}
