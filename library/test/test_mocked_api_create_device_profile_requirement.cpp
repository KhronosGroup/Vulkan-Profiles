/*
 * Copyright (c) 2023-2023 Valve Corporation
 * Copyright (c) 2023-2023 LunarG, Inc.
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

#include "mock_vulkan_api.hpp"
#include "../../profiles/test/data/VP_LUNARG_test_profile_requirements/vulkan_profiles.hpp"

TEST(mocked_api_create_device_profile_requirements, check_support_profile_a) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_LUNARG_TEST_PROFILE_A_NAME, VP_LUNARG_TEST_PROFILE_A_SPEC_VERSION};

    const uint32_t api_version = vpGetProfileAPIVersion(&profile);
    EXPECT_EQ(VK_API_VERSION_MAJOR(api_version), 1);
    EXPECT_EQ(VK_API_VERSION_MINOR(api_version), 2);
    EXPECT_EQ(VK_API_VERSION_PATCH(api_version), 224);

    uint32_t extensions_count = 0;
    vpGetProfileDeviceExtensionProperties(&profile, &extensions_count, nullptr);
    std::vector<VkExtensionProperties> extensions(extensions_count);
    vpGetProfileDeviceExtensionProperties(&profile, &extensions_count, &extensions[0]);

    EXPECT_STREQ(extensions[0].extensionName, "VK_KHR_get_memory_requirements2");

    VkPhysicalDeviceFeatures2 features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, nullptr};
    vpGetProfileFeatures(&profile, &features);

    EXPECT_EQ(features.features.depthBiasClamp, VK_TRUE);
    EXPECT_EQ(features.features.depthClamp, VK_TRUE);
    EXPECT_EQ(features.features.drawIndirectFirstInstance, VK_FALSE);

    VkPhysicalDeviceProperties2 props{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, nullptr};
    vpGetProfileProperties(&profile, &props);

    EXPECT_EQ(props.properties.limits.maxImageDimension1D, 0);
    EXPECT_EQ(props.properties.limits.maxImageDimension2D, 4096);
    EXPECT_EQ(props.properties.limits.maxImageDimension3D, 4096);
    EXPECT_EQ(props.properties.limits.maxImageDimensionCube, 4096);

    mock.SetDeviceExtensions(mock.vkPhysicalDevice, extensions);
    mock.SetInstanceAPIVersion(VK_API_VERSION_1_3);
    mock.SetDeviceAPIVersion(VK_API_VERSION_1_3);
    mock.SetFeatures({VK_STRUCT(features)});
    mock.SetProperties({VK_STRUCT(props)});
    mock.SetDeviceExtensions(mock.vkPhysicalDevice, extensions);

    VkBool32 supported = VK_FALSE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_TRUE);
}

TEST(mocked_api_create_device_profile_requirements, check_support_profile_b) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_LUNARG_TEST_PROFILE_B_NAME, VP_LUNARG_TEST_PROFILE_B_SPEC_VERSION};

    const uint32_t api_version = vpGetProfileAPIVersion(&profile);
    EXPECT_EQ(VK_API_VERSION_MAJOR(api_version), 1);
    EXPECT_EQ(VK_API_VERSION_MINOR(api_version), 3);
    EXPECT_EQ(VK_API_VERSION_PATCH(api_version), 224);

    uint32_t extensions_count = 0;
    vpGetProfileDeviceExtensionProperties(&profile, &extensions_count, nullptr);
    std::vector<VkExtensionProperties> extensions(extensions_count);
    vpGetProfileDeviceExtensionProperties(&profile, &extensions_count, &extensions[0]);

    EXPECT_STREQ(extensions[0].extensionName, "VK_KHR_get_memory_requirements2");
    EXPECT_STREQ(extensions[1].extensionName, "VK_KHR_driver_properties");

    VkPhysicalDeviceFeatures2 features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, nullptr};
    vpGetProfileFeatures(&profile, &features);

    EXPECT_EQ(features.features.depthBiasClamp, VK_TRUE);
    EXPECT_EQ(features.features.depthClamp, VK_TRUE);
    EXPECT_EQ(features.features.drawIndirectFirstInstance, VK_TRUE);

    VkPhysicalDeviceProperties2 props{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, nullptr};
    vpGetProfileProperties(&profile, &props);

    EXPECT_EQ(props.properties.limits.maxImageDimension1D, 4096);
    EXPECT_EQ(props.properties.limits.maxImageDimension2D, 8192);
    EXPECT_EQ(props.properties.limits.maxImageDimension3D, 2048);
    EXPECT_EQ(props.properties.limits.maxImageDimensionCube, 4096);

    // For VP_LUNARG_test_profile_a support
    props.properties.limits.maxImageDimension3D = 4096;

    mock.SetDeviceExtensions(mock.vkPhysicalDevice, extensions);
    mock.SetInstanceAPIVersion(VK_API_VERSION_1_3);
    mock.SetDeviceAPIVersion(VK_API_VERSION_1_3);
    mock.SetFeatures({VK_STRUCT(features)});
    mock.SetProperties({VK_STRUCT(props)});

    VkBool32 supported = VK_FALSE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_TRUE);
}

/*
TEST(mocked_api_create_device_profile_requirements, default_features) {
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
    vpGetProfileFeatures(&profile, &outFeatures);

    mock.SetExpectedDeviceCreateInfo(&outCreateInfo, {
        VK_STRUCT(outFeatures),
        VK_STRUCT(outFeatures11),
        VK_STRUCT(outFeatures12),
        VK_STRUCT(outFeatures13)
    });

    VpDeviceCreateInfo createInfo{ &inCreateInfo, &profile, 0 };

    VkDevice device = VK_NULL_HANDLE;
    VkResult result = vpCreateDevice(mock.vkPhysicalDevice, &createInfo, &mock.vkAllocator, &device);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_TRUE(device == mock.vkDevice);
}
*/
