/*
 * Copyright (c) 2021-2024 Valve Corporation
 * Copyright (c) 2021-2024 LunarG, Inc.
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
 * - Daniel Rakos <daniel.rakos@rastergrid.com>
 */

#include "mock_vulkan_api.hpp"
#include "test_vulkan_profiles_host_image_copy.hpp"

TEST(mocked_api_generated_library_host_image_library, check_support_profile) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_LUNARG_TEST_HOST_IMAGE_COPY_BASIC_NAME, VP_LUNARG_TEST_HOST_IMAGE_COPY_BASIC_SPEC_VERSION};

    uint32_t extensions_count = 0;
    vpGetProfileDeviceExtensionProperties(&profile, nullptr, &extensions_count, nullptr);
    std::vector<VkExtensionProperties> extensions(extensions_count);
    vpGetProfileDeviceExtensionProperties(&profile, nullptr, &extensions_count, &extensions[0]);

    VkPhysicalDeviceFeatures2 features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, nullptr};
    vpGetProfileFeatures(&profile, nullptr, &features);

    VkPhysicalDeviceHostImageCopyPropertiesEXT props_host_image_copy{
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_IMAGE_COPY_PROPERTIES_EXT, nullptr};

    VkPhysicalDeviceProperties2 props{
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, &props_host_image_copy};
    vpGetProfileProperties(&profile, nullptr, &props);

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
    VkBool32 result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_TRUE);
}
