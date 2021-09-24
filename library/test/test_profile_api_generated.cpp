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

#include <vulkan/vulkan_profiles.hpp>

#include <cstdio>
#include <memory>
#include <vector>

static const float DEFAULT_QUEUE_PRIORITY(0.0f);

class TestScaffold {
   public:
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    uint32_t queueFamilyIndex;
    VkDeviceQueueCreateInfo queueCreateInfo;

    TestScaffold() {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Testing scaffold";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 2, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 2, 0);
        appInfo.apiVersion = VK_API_VERSION_1_2;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        VkResult res = vkCreateInstance(&createInfo, nullptr, &instance);
        EXPECT_TRUE(res == VK_SUCCESS);

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        EXPECT_TRUE(deviceCount > 0);

        std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());
        physicalDevice = physicalDevices[0];
        queueCreateInfo = {};
        uint32_t queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());
        for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++) {
            if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                queueFamilyIndex = i;
                queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueCreateInfo.queueFamilyIndex = i;
                queueCreateInfo.queueCount = 1;
                queueCreateInfo.pQueuePriorities = &DEFAULT_QUEUE_PRIORITY;
                break;
            }
        }
    }
};

TEST(test_profile_generated, enumerate) {
    TestScaffold scaffold;

    uint32_t profileCount = 0;
    vpGetPhysicalDeviceProfiles(scaffold.physicalDevice, &profileCount, nullptr);
    EXPECT_TRUE(profileCount > 0);

    std::vector<VpProfile> profiles;
    profiles.resize(profileCount);
    vpGetPhysicalDeviceProfiles(scaffold.physicalDevice, &profileCount, &profiles[0]);

    for (VpProfile profile : profiles) {
        std::printf("Profile supported: %d\n", profile);
    }
}

TEST(test_profile_create_generated, create_device) {
    TestScaffold scaffold;

    uint32_t profileCount = 0;
    vpGetPhysicalDeviceProfiles(scaffold.physicalDevice, &profileCount, nullptr);
    EXPECT_TRUE(profileCount > 0);

    std::vector<VpProfile> profiles(profileCount);
    vpGetPhysicalDeviceProfiles(scaffold.physicalDevice, &profileCount, &profiles[0]);

    for (VpProfile profile : profiles) {
        std::printf("Profile supported: %d\n", profile);
    }

    for (VpProfile profile : profiles) {
        TestScaffold scaffold;
        VkDevice device;
        VkDeviceCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        info.queueCreateInfoCount = 1;
        info.pQueueCreateInfos = &scaffold.queueCreateInfo;
        info.enabledExtensionCount = 0;
        info.ppEnabledExtensionNames = nullptr;
        auto res = vpCreateDevice(scaffold.physicalDevice, profile, &info, nullptr, &device);
        if (res != VK_SUCCESS) {
            std::printf("FAILURE: %d\n", res);
        } else {
            std::printf("SUCCESS?\n");
        }
    }
}
