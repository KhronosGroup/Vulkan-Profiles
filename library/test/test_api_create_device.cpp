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
#ifndef VULKAN_PROFILES_HEADER_ONLY
#include <vulkan/debug/vulkan_profiles.hpp>
#else
#include <vulkan/debug/vulkan_profiles.h>
#endif

#include <cstdio>
#include <memory>
#include <vector>

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

TEST(api_create_device_profile, overrite_with_profile_only) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_BASELINE_2022_NAME, VP_LUNARG_DESKTOP_BASELINE_2022_SPEC_VERSION};

    VkDeviceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pNext = nullptr;
    info.queueCreateInfoCount = 1;
    info.pQueueCreateInfos = &scaffold->queueCreateInfo;
    info.enabledExtensionCount = 0;
    info.ppEnabledExtensionNames = nullptr;
    info.pEnabledFeatures = nullptr;

    VpDeviceCreateInfo profileInfo = {};
    profileInfo.pCreateInfo = &info;
    profileInfo.pProfile = &profile;

    VkDevice device = VK_NULL_HANDLE;
    VkResult res = vpCreateDevice(scaffold->physicalDevice, &profileInfo, nullptr, &device);
    EXPECT_TRUE(res == VK_SUCCESS);
    EXPECT_TRUE(device != VK_NULL_HANDLE);
}

TEST(api_create_device_profile, overrite_with_supported_extensions) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_BASELINE_2022_NAME, VP_LUNARG_DESKTOP_BASELINE_2022_SPEC_VERSION};

    static const char* extensions[] = {"VK_KHR_image_format_list", "VK_KHR_maintenance3", "VK_KHR_imageless_framebuffer"};

    VkDeviceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pNext = nullptr;
    info.queueCreateInfoCount = 1;
    info.pQueueCreateInfos = &scaffold->queueCreateInfo;
    info.enabledExtensionCount = countof(extensions);
    info.ppEnabledExtensionNames = extensions;
    info.pEnabledFeatures = nullptr;

    VpDeviceCreateInfo profileInfo = {};
    profileInfo.pCreateInfo = &info;
    profileInfo.pProfile = &profile;
    profileInfo.flags = VP_DEVICE_CREATE_OVERRIDE_EXTENSIONS_BIT;

    VkDevice device = VK_NULL_HANDLE;
    VkResult res = vpCreateDevice(scaffold->physicalDevice, &profileInfo, nullptr, &device);
    EXPECT_TRUE(res == VK_SUCCESS);
    EXPECT_TRUE(device != VK_NULL_HANDLE);
}

TEST(api_create_device_profile, overrite_with_unsupported_extensions) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_BASELINE_2022_NAME, VP_LUNARG_DESKTOP_BASELINE_2022_SPEC_VERSION};

    static const char* extensions[] = {"VK_LUNARG_doesnot_exist", "VK_GTRUC_automagic_rendering",
                                       "VK_GTRUC_portability_everywhere"};

    VkDeviceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pNext = nullptr;
    info.queueCreateInfoCount = 1;
    info.pQueueCreateInfos = &scaffold->queueCreateInfo;
    info.enabledExtensionCount = countof(extensions);
    info.ppEnabledExtensionNames = extensions;
    info.pEnabledFeatures = nullptr;

    VpDeviceCreateInfo profileInfo = {};
    profileInfo.pCreateInfo = &info;
    profileInfo.pProfile = &profile;
    profileInfo.flags = VP_DEVICE_CREATE_OVERRIDE_EXTENSIONS_BIT;

    VkDevice device = VK_NULL_HANDLE;
    VkResult res = vpCreateDevice(scaffold->physicalDevice, &profileInfo, nullptr, &device);
    EXPECT_TRUE(res != VK_SUCCESS);
    EXPECT_TRUE(device == VK_NULL_HANDLE);
}


TEST(api_create_device_profile, overrite_with_enabled_features) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_BASELINE_2022_NAME, VP_LUNARG_DESKTOP_BASELINE_2022_SPEC_VERSION};

    VkPhysicalDeviceFeatures enabledFeatures = {};
    enabledFeatures.robustBufferAccess = VK_TRUE;

    VkDeviceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pNext = nullptr;
    info.queueCreateInfoCount = 1;
    info.pQueueCreateInfos = &scaffold->queueCreateInfo;
    info.enabledExtensionCount = 0;
    info.ppEnabledExtensionNames = nullptr;
    info.pEnabledFeatures = &enabledFeatures;

    VpDeviceCreateInfo profileInfo = {};
    profileInfo.pCreateInfo = &info;
    profileInfo.pProfile = &profile;

    VkDevice device = VK_NULL_HANDLE;
    VkResult res = vpCreateDevice(scaffold->physicalDevice, &profileInfo, nullptr, &device);
    EXPECT_TRUE(res == VK_SUCCESS);
    EXPECT_TRUE(device != VK_NULL_HANDLE);
}

TEST(api_create_device_profile, overrite_with_pnext_features) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_BASELINE_2022_NAME, VP_LUNARG_DESKTOP_BASELINE_2022_SPEC_VERSION};

    VkPhysicalDeviceSubgroupSizeControlFeaturesEXT deviceFeatures = {};
    deviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES_EXT;
    deviceFeatures.pNext = nullptr;
    deviceFeatures.subgroupSizeControl = VK_FALSE;
    deviceFeatures.computeFullSubgroups = VK_TRUE;

    VkDeviceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pNext = &deviceFeatures;
    info.queueCreateInfoCount = 1;
    info.pQueueCreateInfos = &scaffold->queueCreateInfo;
    info.enabledExtensionCount = 0;
    info.ppEnabledExtensionNames = nullptr;
    info.pEnabledFeatures = nullptr;

    VpDeviceCreateInfo profileInfo = {};
    profileInfo.pCreateInfo = &info;
    profileInfo.pProfile = &profile;

    VkDevice device = VK_NULL_HANDLE;
    VkResult res = vpCreateDevice(scaffold->physicalDevice, &profileInfo, nullptr, &device);
    EXPECT_TRUE(res == VK_SUCCESS);
    EXPECT_TRUE(device != VK_NULL_HANDLE);
}

TEST(api_create_device_profile, with_extensions_flag) {
    const VpProfileProperties profile{VP_LUNARG_DESKTOP_BASELINE_2022_NAME, VP_LUNARG_DESKTOP_BASELINE_2022_SPEC_VERSION};

    static const char* extensions[] = {VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME, VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME};

    VkDeviceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pNext = nullptr;
    info.queueCreateInfoCount = 1;
    info.pQueueCreateInfos = &scaffold->queueCreateInfo;
    info.pEnabledFeatures = nullptr;

    VpDeviceCreateInfo profileInfo = {};
    profileInfo.pCreateInfo = &info;
    profileInfo.pProfile = &profile;

    // override profile extensions
    {
        info.enabledExtensionCount = countof(extensions);
        info.ppEnabledExtensionNames = extensions;
        profileInfo.flags = VP_DEVICE_CREATE_OVERRIDE_EXTENSIONS_BIT;

        VkDevice device = VK_NULL_HANDLE;
        VkResult res = vpCreateDevice(scaffold->physicalDevice, &profileInfo, nullptr, &device);
        EXPECT_TRUE(res == VK_SUCCESS);
        EXPECT_TRUE(device != VK_NULL_HANDLE);
    }

    // add extensions to the profile extension list
    {
        info.enabledExtensionCount = countof(extensions);
        info.ppEnabledExtensionNames = extensions;
        profileInfo.flags = VP_DEVICE_CREATE_MERGE_EXTENSIONS_BIT;

        VkDevice device = VK_NULL_HANDLE;
        VkResult res = vpCreateDevice(scaffold->physicalDevice, &profileInfo, nullptr, &device);
        EXPECT_TRUE(res == VK_SUCCESS);
        EXPECT_TRUE(device != VK_NULL_HANDLE);
    }

    // add extensions to the profile extension list but the list is empty
    {
        info.enabledExtensionCount = 0;
        info.ppEnabledExtensionNames = nullptr;
        profileInfo.flags = VP_DEVICE_CREATE_MERGE_EXTENSIONS_BIT;

        VkDevice device = VK_NULL_HANDLE;
        VkResult res = vpCreateDevice(scaffold->physicalDevice, &profileInfo, nullptr, &device);
        EXPECT_TRUE(res == VK_SUCCESS);
        EXPECT_TRUE(device != VK_NULL_HANDLE);
    }
}

TEST(api_get_profile_support, unsupported_name) {
    VpProfileProperties profile{"Bouuahhhh", VP_LUNARG_DESKTOP_BASELINE_2022_SPEC_VERSION};

    VkBool32 supported = VK_FALSE;
    vpGetPhysicalDeviceProfileSupport(scaffold->instance, scaffold->physicalDevice, &profile, &supported);
    EXPECT_EQ(VK_FALSE, supported);
}

TEST(api_get_profile_support, unsupported_version) {
    VpProfileProperties profile{VP_LUNARG_DESKTOP_BASELINE_2022_NAME, VP_LUNARG_DESKTOP_BASELINE_2022_SPEC_VERSION + 1};

    VkBool32 supported = VK_FALSE;
    vpGetPhysicalDeviceProfileSupport(scaffold->instance, scaffold->physicalDevice, &profile, &supported);
    EXPECT_EQ(VK_FALSE, supported);
}

TEST(api_get_profile_support, empty) {
    VpProfileProperties profile = {};

    VkBool32 supported = VK_FALSE;
    vpGetPhysicalDeviceProfileSupport(scaffold->instance, scaffold->physicalDevice, &profile, &supported);
    EXPECT_EQ(VK_FALSE, supported);
}
