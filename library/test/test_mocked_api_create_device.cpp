/*
 * Copyright (c) 2021-2023 Valve Corporation
 * Copyright (c) 2021-2023 LunarG, Inc.
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
 * - Daniel Rakos <daniel.rakos@rastergrid.com>
 */

#include "mock_vulkan_api.hpp"
#include <vulkan/vulkan_profiles.hpp>

TEST(mocked_api_create_device, default_extensions) {
    MockVulkanAPI mock;

    VpProfileProperties profile{ VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION };

    VkDeviceQueueCreateInfo queueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
    queueCreateInfo.queueFamilyIndex = 0;
    queueCreateInfo.queueCount = 1;

    VkDeviceCreateInfo inCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    inCreateInfo.queueCreateInfoCount = 1;
    inCreateInfo.pQueueCreateInfos = &queueCreateInfo;

    std::vector<const char *> outExtensions(
        sizeof(detail::VP_KHR_ROADMAP_2022::vulkan13requirements_roadmap2022::deviceExtensions) /
        sizeof(detail::VP_KHR_ROADMAP_2022::vulkan13requirements_roadmap2022::deviceExtensions[0]));
    for (size_t i = 0; i < outExtensions.size(); ++i) {
        outExtensions[i] = detail::VP_KHR_ROADMAP_2022::vulkan13requirements_roadmap2022::deviceExtensions[i].extensionName;
    }

    VkDeviceCreateInfo outCreateInfo = inCreateInfo;
    outCreateInfo.enabledExtensionCount = static_cast<uint32_t>(outExtensions.size());
    outCreateInfo.ppEnabledExtensionNames = outExtensions.data();

    VkPhysicalDeviceVulkan13Features outFeatures13{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
    VkPhysicalDeviceVulkan12Features outFeatures12{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, &outFeatures13 };
    VkPhysicalDeviceVulkan11Features outFeatures11{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, &outFeatures12 };
    VkPhysicalDeviceFeatures2 outFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &outFeatures11 };
    vpGetProfileFeatures(&profile, nullptr, &outFeatures);

    mock.SetExpectedDeviceCreateInfo(&outCreateInfo, {
        VK_STRUCT(outFeatures),
        VK_STRUCT(outFeatures11),
        VK_STRUCT(outFeatures12),
        VK_STRUCT(outFeatures13)
    });

    VpDeviceCreateInfo createInfo{ &inCreateInfo, 0, 1, &profile };

    VkDevice device = VK_NULL_HANDLE;
    VkResult result = vpCreateDevice(mock.vkPhysicalDevice, &createInfo, &mock.vkAllocator, &device);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_TRUE(device == mock.vkDevice);
}

TEST(mocked_api_create_device, merge_extensions) {
    MockVulkanAPI mock;

    VpProfileProperties profile{ VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION };

    VkDeviceQueueCreateInfo queueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
    queueCreateInfo.queueFamilyIndex = 0;
    queueCreateInfo.queueCount = 1;

    std::vector<const char *> inExtensions{
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_GLOBAL_PRIORITY_EXTENSION_NAME
    };

    VkDeviceCreateInfo inCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    inCreateInfo.queueCreateInfoCount = 1;
    inCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    inCreateInfo.enabledExtensionCount = static_cast<uint32_t>(inExtensions.size());
    inCreateInfo.ppEnabledExtensionNames = inExtensions.data();

    std::vector<const char *> outExtensions(std::size(detail::VP_KHR_ROADMAP_2022::vulkan13requirements_roadmap2022::deviceExtensions));
    for (size_t i = 0; i < outExtensions.size(); ++i) {
        outExtensions[i] = detail::VP_KHR_ROADMAP_2022::vulkan13requirements_roadmap2022::deviceExtensions[i].extensionName;
    }
    outExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    VkDeviceCreateInfo outCreateInfo = inCreateInfo;
    outCreateInfo.enabledExtensionCount = static_cast<uint32_t>(outExtensions.size());
    outCreateInfo.ppEnabledExtensionNames = outExtensions.data();

    VkPhysicalDeviceVulkan13Features outFeatures13{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
    VkPhysicalDeviceVulkan12Features outFeatures12{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, &outFeatures13 };
    VkPhysicalDeviceVulkan11Features outFeatures11{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, &outFeatures12 };
    VkPhysicalDeviceFeatures2 outFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &outFeatures11 };
    vpGetProfileFeatures(&profile, nullptr, &outFeatures);

    mock.SetExpectedDeviceCreateInfo(&outCreateInfo, {
        VK_STRUCT(outFeatures),
        VK_STRUCT(outFeatures11),
        VK_STRUCT(outFeatures12),
        VK_STRUCT(outFeatures13)
    });

    VpDeviceCreateInfo createInfo{ &inCreateInfo, 0, 1, &profile };

    VkDevice device = VK_NULL_HANDLE;
    VkResult result = vpCreateDevice(mock.vkPhysicalDevice, &createInfo, &mock.vkAllocator, &device);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_TRUE(device == mock.vkDevice);
}

TEST(mocked_api_create_device, default_features) {
    MockVulkanAPI mock;

    VpProfileProperties profile{ VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION };

    VkDeviceQueueCreateInfo queueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
    queueCreateInfo.queueFamilyIndex = 0;
    queueCreateInfo.queueCount = 1;

    VkDeviceCreateInfo inCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    inCreateInfo.queueCreateInfoCount = 1;
    inCreateInfo.pQueueCreateInfos = &queueCreateInfo;

    std::vector<const char *> outExtensions(std::size(detail::VP_KHR_ROADMAP_2022::vulkan13requirements_roadmap2022::deviceExtensions));
    for (size_t i = 0; i < outExtensions.size(); ++i) {
        outExtensions[i] = detail::VP_KHR_ROADMAP_2022::vulkan13requirements_roadmap2022::deviceExtensions[i].extensionName;
    }

    VkDeviceCreateInfo outCreateInfo = inCreateInfo;
    outCreateInfo.enabledExtensionCount = static_cast<uint32_t>(outExtensions.size());
    outCreateInfo.ppEnabledExtensionNames = outExtensions.data();

    VkPhysicalDeviceVulkan13Features outFeatures13{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
    VkPhysicalDeviceVulkan12Features outFeatures12{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, &outFeatures13 };
    VkPhysicalDeviceVulkan11Features outFeatures11{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, &outFeatures12 };
    VkPhysicalDeviceFeatures2 outFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &outFeatures11 };
    vpGetProfileFeatures(&profile, nullptr, &outFeatures);

    mock.SetExpectedDeviceCreateInfo(&outCreateInfo, {
        VK_STRUCT(outFeatures),
        VK_STRUCT(outFeatures11),
        VK_STRUCT(outFeatures12),
        VK_STRUCT(outFeatures13)
    });

    VpDeviceCreateInfo createInfo{ &inCreateInfo, 0, 1, &profile };

    VkDevice device = VK_NULL_HANDLE;
    VkResult result = vpCreateDevice(mock.vkPhysicalDevice, &createInfo, &mock.vkAllocator, &device);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_TRUE(device == mock.vkDevice);
}

TEST(mocked_api_create_device, legacy_enabled_features) {
    MockVulkanAPI mock;

    VpProfileProperties profile{ VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION };

    VkDeviceQueueCreateInfo queueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
    queueCreateInfo.queueFamilyIndex = 0;
    queueCreateInfo.queueCount = 1;

    VkPhysicalDeviceMultiviewFeaturesKHR multiviewFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR };
    multiviewFeatures.multiview = VK_TRUE;

    VkPhysicalDeviceFeatures2 inFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
    inFeatures.features.fragmentStoresAndAtomics = VK_TRUE;
    inFeatures.features.occlusionQueryPrecise = VK_TRUE;

    VkDeviceCreateInfo inCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, &multiviewFeatures };
    inCreateInfo.queueCreateInfoCount = 1;
    inCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    inCreateInfo.pEnabledFeatures = &inFeatures.features;

    std::vector<const char *> outExtensions(
        std::size(detail::VP_KHR_ROADMAP_2022::vulkan13requirements_roadmap2022::deviceExtensions));
    for (size_t i = 0; i < outExtensions.size(); ++i) {
        outExtensions[i] = detail::VP_KHR_ROADMAP_2022::vulkan13requirements_roadmap2022::deviceExtensions[i].extensionName;
    }

    VkDeviceCreateInfo outCreateInfo = inCreateInfo;
    outCreateInfo.enabledExtensionCount = static_cast<uint32_t>(outExtensions.size());
    outCreateInfo.ppEnabledExtensionNames = outExtensions.data();
    outCreateInfo.pEnabledFeatures = nullptr;

    VkPhysicalDeviceVulkan13Features outFeatures13{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
    VkPhysicalDeviceVulkan12Features outFeatures12{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, &outFeatures13 };
    VkPhysicalDeviceVulkan11Features outFeatures11{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, &outFeatures12 };
    VkPhysicalDeviceFeatures2 outFeatures2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &outFeatures11};
    outFeatures2.features = inFeatures.features;
    vpGetProfileFeatures(&profile, nullptr, &outFeatures2);

    mock.SetExpectedDeviceCreateInfo(&outCreateInfo, {
        VK_STRUCT(outFeatures2),
        VK_STRUCT(outFeatures11),
        VK_STRUCT(outFeatures12),
        VK_STRUCT(outFeatures13),
        VK_STRUCT(multiviewFeatures)
    });

    VpDeviceCreateInfo createInfo{ &inCreateInfo, 0, 1, &profile };

    VkDevice device = VK_NULL_HANDLE;
    VkResult result = vpCreateDevice(mock.vkPhysicalDevice, &createInfo, &mock.vkAllocator, &device);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_TRUE(device == mock.vkDevice);
}

TEST(mocked_api_create_device, disable_robust_buffer_access) {
    MockVulkanAPI mock;

    VpProfileProperties profile{ VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION };

    VkDeviceQueueCreateInfo queueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
    queueCreateInfo.queueFamilyIndex = 0;
    queueCreateInfo.queueCount = 1;

    VkDeviceCreateInfo inCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    inCreateInfo.queueCreateInfoCount = 1;
    inCreateInfo.pQueueCreateInfos = &queueCreateInfo;

    std::vector<const char *> outExtensions(std::size(detail::VP_KHR_ROADMAP_2022::vulkan13requirements_roadmap2022::deviceExtensions));
    for (size_t i = 0; i < outExtensions.size(); ++i) {
        outExtensions[i] = detail::VP_KHR_ROADMAP_2022::vulkan13requirements_roadmap2022::deviceExtensions[i].extensionName;
    }

    VkDeviceCreateInfo outCreateInfo = inCreateInfo;
    outCreateInfo.enabledExtensionCount = static_cast<uint32_t>(outExtensions.size());
    outCreateInfo.ppEnabledExtensionNames = outExtensions.data();

    VkPhysicalDeviceVulkan13Features outFeatures13{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
    VkPhysicalDeviceVulkan12Features outFeatures12{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, &outFeatures13 };
    VkPhysicalDeviceVulkan11Features outFeatures11{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, &outFeatures12 };
    VkPhysicalDeviceFeatures2 outFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &outFeatures11 };
    vpGetProfileFeatures(&profile, nullptr, &outFeatures);

    outFeatures.features.robustBufferAccess = VK_FALSE;

    mock.SetExpectedDeviceCreateInfo(&outCreateInfo, {
        VK_STRUCT(outFeatures),
        VK_STRUCT(outFeatures11),
        VK_STRUCT(outFeatures12),
        VK_STRUCT(outFeatures13)
    });

    VpDeviceCreateInfo createInfo{&inCreateInfo, VP_DEVICE_CREATE_DISABLE_ROBUST_BUFFER_ACCESS_BIT, 1, &profile};

    VkDevice device = VK_NULL_HANDLE;
    VkResult result = vpCreateDevice(mock.vkPhysicalDevice, &createInfo, &mock.vkAllocator, &device);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_TRUE(device == mock.vkDevice);
}


TEST(mocked_api_create_device, disable_robust_image_access) {
    MockVulkanAPI mock;

    VpProfileProperties profile{ VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION };

    VkDeviceQueueCreateInfo queueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
    queueCreateInfo.queueFamilyIndex = 0;
    queueCreateInfo.queueCount = 1;

    VkDeviceCreateInfo inCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    inCreateInfo.queueCreateInfoCount = 1;
    inCreateInfo.pQueueCreateInfos = &queueCreateInfo;

    std::vector<const char *> outExtensions(
        std::size(detail::VP_KHR_ROADMAP_2022::vulkan13requirements_roadmap2022::deviceExtensions));
    for (size_t i = 0; i < outExtensions.size(); ++i) {
        outExtensions[i] = detail::VP_KHR_ROADMAP_2022::vulkan13requirements_roadmap2022::deviceExtensions[i].extensionName;
    }

    VkDeviceCreateInfo outCreateInfo = inCreateInfo;
    outCreateInfo.enabledExtensionCount = static_cast<uint32_t>(outExtensions.size());
    outCreateInfo.ppEnabledExtensionNames = outExtensions.data();

    VkPhysicalDeviceVulkan13Features outFeatures13{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
    VkPhysicalDeviceVulkan12Features outFeatures12{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, &outFeatures13 };
    VkPhysicalDeviceVulkan11Features outFeatures11{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, &outFeatures12 };
    VkPhysicalDeviceFeatures2 outFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &outFeatures11 };
    vpGetProfileFeatures(&profile, nullptr, &outFeatures);

    outFeatures13.robustImageAccess = VK_FALSE;

    mock.SetExpectedDeviceCreateInfo(&outCreateInfo, {
        VK_STRUCT(outFeatures),
        VK_STRUCT(outFeatures11),
        VK_STRUCT(outFeatures12),
        VK_STRUCT(outFeatures13)
    });

    VpDeviceCreateInfo createInfo{&inCreateInfo, VP_DEVICE_CREATE_DISABLE_ROBUST_IMAGE_ACCESS_BIT, 1, &profile};

    VkDevice device = VK_NULL_HANDLE;
    VkResult result = vpCreateDevice(mock.vkPhysicalDevice, &createInfo, &mock.vkAllocator, &device);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_TRUE(device == mock.vkDevice);
}

TEST(mocked_api_create_device, disable_robust_access) {
    MockVulkanAPI mock;

    VpProfileProperties profile{ VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION };

    VkDeviceQueueCreateInfo queueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
    queueCreateInfo.queueFamilyIndex = 0;
    queueCreateInfo.queueCount = 1;

    VkDeviceCreateInfo inCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    inCreateInfo.queueCreateInfoCount = 1;
    inCreateInfo.pQueueCreateInfos = &queueCreateInfo;

    std::vector<const char *> outExtensions(std::size(detail::VP_KHR_ROADMAP_2022::vulkan13requirements_roadmap2022::deviceExtensions));
    for (size_t i = 0; i < outExtensions.size(); ++i) {
        outExtensions[i] = detail::VP_KHR_ROADMAP_2022::vulkan13requirements_roadmap2022::deviceExtensions[i].extensionName;
    }

    VkDeviceCreateInfo outCreateInfo = inCreateInfo;
    outCreateInfo.enabledExtensionCount = static_cast<uint32_t>(outExtensions.size());
    outCreateInfo.ppEnabledExtensionNames = outExtensions.data();

    VkPhysicalDeviceVulkan13Features outFeatures13{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
    VkPhysicalDeviceVulkan12Features outFeatures12{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, &outFeatures13 };
    VkPhysicalDeviceVulkan11Features outFeatures11{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, &outFeatures12 };
    VkPhysicalDeviceFeatures2 outFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &outFeatures11 };
    vpGetProfileFeatures(&profile, nullptr, &outFeatures);

    outFeatures.features.robustBufferAccess = VK_FALSE;
    outFeatures13.robustImageAccess = VK_FALSE;

    mock.SetExpectedDeviceCreateInfo(&outCreateInfo, {
        VK_STRUCT(outFeatures),
        VK_STRUCT(outFeatures11),
        VK_STRUCT(outFeatures12),
        VK_STRUCT(outFeatures13)
    });

    VpDeviceCreateInfo createInfo{&inCreateInfo, VP_DEVICE_CREATE_DISABLE_ROBUST_ACCESS, 1, &profile};

    VkDevice device = VK_NULL_HANDLE;
    VkResult result = vpCreateDevice(mock.vkPhysicalDevice, &createInfo, &mock.vkAllocator, &device);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_TRUE(device == mock.vkDevice);
}
