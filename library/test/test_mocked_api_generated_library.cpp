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
#include "test_vulkan_profiles.hpp"

void initProfile(MockVulkanAPI& mock, const VpProfileProperties& profile, uint32_t apiVersion = VK_API_VERSION_1_3,
                 int profileAreas = PROFILE_AREA_ALL_BITS) {
    mock.SetInstanceAPIVersion(apiVersion);
    mock.SetDeviceAPIVersion(apiVersion);

    if (profileAreas & PROFILE_AREA_EXTENSIONS_BIT) {
        uint32_t extensions_count = 0;
        vpGetProfileDeviceExtensionProperties(&profile, nullptr, &extensions_count, nullptr);
        std::vector<VkExtensionProperties> extensions(extensions_count);
        vpGetProfileDeviceExtensionProperties(&profile, nullptr, &extensions_count, &extensions[0]);
        mock.SetDeviceExtensions(mock.vkPhysicalDevice, extensions);
    }

    if (profileAreas & PROFILE_AREA_FEATURES_BIT) {
        VkPhysicalDeviceFeatures2 features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, nullptr};
        vpGetProfileFeatures(&profile, nullptr, &features);
        mock.SetFeatures({VK_STRUCT(features)});
    }

    if (profileAreas & PROFILE_AREA_PROPERTIES_BIT) {
        VkPhysicalDeviceProperties2 props{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, nullptr};
        vpGetProfileProperties(&profile, nullptr, &props);
        mock.SetProperties({VK_STRUCT(props)});
    }

    if (profileAreas & PROFILE_AREA_FORMATS_BIT) {
        uint32_t formatCount = 0;
        vpGetProfileFormats(&profile, nullptr, &formatCount, nullptr);
        std::vector<VkFormat> formats(formatCount);
        vpGetProfileFormats(&profile, nullptr, &formatCount, &formats[0]);

        for (std::size_t i = 0, n = formats.size(); i < n; ++i) {
            VkFormatProperties2KHR formatProps{VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR};
            vpGetProfileFormatProperties(&profile, nullptr, formats[i], &formatProps);
            mock.AddFormat(formats[i], {VK_STRUCT(formatProps)});
        }
    }
}

void fixProperties(MockVulkanAPI& mock) {
    mock.ClearProfileAreas(PROFILE_AREA_PROPERTIES_BIT);

    VkPhysicalDeviceProperties2 props{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, nullptr};
    props.properties.limits.maxImageDimension1D = 8192;
    props.properties.limits.maxImageDimension2D = 8192;
    props.properties.limits.maxImageDimension3D = 4096;
    props.properties.limits.maxImageDimensionCube = 4096;
    mock.SetProperties({VK_STRUCT(props)});
}

TEST(mocked_api_generated_library, internal_code) {
    detail::FeaturesChain features;

    VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT physicalDeviceVertexAttributeDivisorFeaturesEXT{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT, nullptr};
    VkPhysicalDeviceVulkan11Features physicalDeviceVulkan11Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, &physicalDeviceVertexAttributeDivisorFeaturesEXT};
    VkPhysicalDeviceVulkan12Features physicalDeviceVulkan12Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, &physicalDeviceVulkan11Features};
    VkPhysicalDeviceFeatures2KHR physicalDeviceFeatures2KHR{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR, &physicalDeviceVulkan12Features};

    VkBaseOutStructure* structure = detail::vpExtractStructure(&physicalDeviceFeatures2KHR, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES);
    EXPECT_EQ(structure->sType, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES);
    EXPECT_EQ(physicalDeviceVulkan12Features.pNext, &physicalDeviceVertexAttributeDivisorFeaturesEXT);

    std::vector<VkStructureType> structureTypes;
    structureTypes.push_back(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT);
    structureTypes.push_back(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT);
    structureTypes.push_back(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES);

    VkPhysicalDeviceVulkan13Features outFeatures13{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
    VkPhysicalDeviceVulkan12Features outFeatures12{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, &outFeatures13};
    VkPhysicalDeviceVulkan11Features outFeatures11{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, &outFeatures12};
    VkPhysicalDeviceFeatures2 outFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &outFeatures11};
    VkBaseOutStructure* additionalFeatures = reinterpret_cast<VkBaseOutStructure*>(&outFeatures);

    outFeatures.features.drawIndirectFirstInstance = VK_TRUE;
    outFeatures.features.fullDrawIndexUint32 = VK_TRUE;
    outFeatures.features.depthClamp = VK_TRUE;
    outFeatures11.storageBuffer16BitAccess = VK_TRUE;
    outFeatures11.shaderDrawParameters = VK_TRUE;

    detail::GatherStructureTypes(structureTypes, additionalFeatures);
    EXPECT_EQ(structureTypes[0], VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT);
    EXPECT_EQ(structureTypes[1], VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT);
    EXPECT_EQ(structureTypes[2], VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES);
    EXPECT_EQ(structureTypes[3], VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2);
    EXPECT_EQ(structureTypes[4], VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES);
    EXPECT_EQ(structureTypes[5], VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES);

    features.Build(structureTypes);

    features.requiredFeaturesChain.features.depthClamp = VK_TRUE;
    features.requiredFeaturesChain.features.depthBiasClamp = VK_TRUE;
    features.physicalDeviceVulkan11Features.storageBuffer16BitAccess = VK_TRUE;
    features.physicalDeviceVulkan11Features.uniformAndStorageBuffer16BitAccess = VK_TRUE;

    VkDeviceCreateInfo VkCreateInfo{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, &outFeatures};
    VpDeviceCreateInfo VpCreateInfo{&VkCreateInfo, 0};

    features.ApplyFeatures(&VpCreateInfo);

    EXPECT_EQ(features.requiredFeaturesChain.features.drawIndirectFirstInstance, VK_TRUE);
    EXPECT_EQ(features.requiredFeaturesChain.features.fullDrawIndexUint32, VK_TRUE);
    EXPECT_EQ(features.requiredFeaturesChain.features.depthClamp, VK_TRUE);
    EXPECT_EQ(features.requiredFeaturesChain.features.depthBiasClamp, VK_TRUE);

    EXPECT_EQ(features.physicalDeviceVulkan11Features.storageBuffer16BitAccess, VK_TRUE);
    EXPECT_EQ(features.physicalDeviceVulkan11Features.shaderDrawParameters, VK_TRUE);
    EXPECT_EQ(features.physicalDeviceVulkan11Features.uniformAndStorageBuffer16BitAccess, VK_TRUE);
}

TEST(mocked_api_generated_library, create_device) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_LUNARG_TEST_PROFILE_B_NAME, VP_LUNARG_TEST_PROFILE_B_SPEC_VERSION};

    uint32_t extension_property_count = 0;
    vpGetProfileDeviceExtensionProperties(&profile, nullptr, &extension_property_count, nullptr);
    std::vector<VkExtensionProperties> extension_properties(extension_property_count);
    vpGetProfileDeviceExtensionProperties(&profile, nullptr, &extension_property_count, &extension_properties[0]);

    std::vector<const char*> extensions(extension_property_count);
    for (std::size_t i = 0, n = extensions.size(); i < n; ++i) {
        extensions[i] = extension_properties[i].extensionName;
    }

    VkPhysicalDeviceFeatures2 features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, nullptr};
    vpGetProfileFeatures(&profile, nullptr, &features);

    VkDeviceQueueCreateInfo queueCreateInfo{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    queueCreateInfo.queueFamilyIndex = 0;
    queueCreateInfo.queueCount = 1;

    VkDeviceCreateInfo inCreateInfo{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    inCreateInfo.queueCreateInfoCount = 1;
    inCreateInfo.pQueueCreateInfos = &queueCreateInfo;

    VkDeviceCreateInfo outCreateInfo = inCreateInfo;
    outCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    outCreateInfo.ppEnabledExtensionNames = extensions.data();

    mock.SetExpectedDeviceCreateInfo(&outCreateInfo, {VK_STRUCT(features)});

    VpDeviceCreateInfo createInfo{&inCreateInfo, 0, 1, &profile};

    VkDevice device = VK_NULL_HANDLE;
    VkResult result = vpCreateDevice(mock.vkPhysicalDevice, &createInfo, &mock.vkAllocator, &device);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_TRUE(device == mock.vkDevice);
}

TEST(mocked_api_generated_library, check_support_profile_a) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_LUNARG_TEST_PROFILE_A_NAME, VP_LUNARG_TEST_PROFILE_A_SPEC_VERSION};

    VkBool32 multiple_variants = VK_TRUE;
    VkResult result = vpHasMultipleVariantsProfile(&profile, &multiple_variants);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(multiple_variants, VK_FALSE);

    const uint32_t api_version = vpGetProfileAPIVersion(&profile);
    EXPECT_EQ(VK_API_VERSION_MAJOR(api_version), 1);
    EXPECT_EQ(VK_API_VERSION_MINOR(api_version), 2);
    EXPECT_EQ(VK_API_VERSION_PATCH(api_version), 224);

    uint32_t extensions_count = 0;
    vpGetProfileDeviceExtensionProperties(&profile, nullptr, &extensions_count, nullptr);
    std::vector<VkExtensionProperties> extensions(extensions_count);
    vpGetProfileDeviceExtensionProperties(&profile, nullptr, &extensions_count, &extensions[0]);

    VkPhysicalDeviceFeatures2 features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, nullptr};
    vpGetProfileFeatures(&profile, nullptr, &features);

    VkPhysicalDeviceProperties2 props{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, nullptr};
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
    result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_TRUE);
}

TEST(mocked_api_generated_library, check_support_profile_b) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_LUNARG_TEST_PROFILE_B_NAME, VP_LUNARG_TEST_PROFILE_B_SPEC_VERSION};

    VkBool32 multiple_variants = VK_TRUE;
    VkResult result = vpHasMultipleVariantsProfile(&profile, &multiple_variants);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(multiple_variants, VK_FALSE);

    const uint32_t api_version = vpGetProfileAPIVersion(&profile);
    EXPECT_EQ(VK_API_VERSION_MAJOR(api_version), 1);
    EXPECT_EQ(VK_API_VERSION_MINOR(api_version), 3);
    EXPECT_EQ(VK_API_VERSION_PATCH(api_version), 224);

    uint32_t extensions_count = 0;
    vpGetProfileDeviceExtensionProperties(&profile, nullptr, &extensions_count, nullptr);
    std::vector<VkExtensionProperties> extensions(extensions_count);
    vpGetProfileDeviceExtensionProperties(&profile, nullptr, &extensions_count, &extensions[0]);

    EXPECT_STREQ(extensions[0].extensionName, "VK_KHR_get_memory_requirements2");
    EXPECT_STREQ(extensions[1].extensionName, "VK_KHR_driver_properties");

    VkPhysicalDeviceFeatures2 features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, nullptr};
    vpGetProfileFeatures(&profile, nullptr, &features);

    EXPECT_EQ(features.features.depthBiasClamp, VK_TRUE);
    EXPECT_EQ(features.features.depthClamp, VK_TRUE);
    EXPECT_EQ(features.features.drawIndirectFirstInstance, VK_TRUE);

    VkPhysicalDeviceProperties2 props{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, nullptr};
    vpGetProfileProperties(&profile, nullptr, &props);

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
    result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_TRUE);
}

TEST(mocked_api_generated_library, check_support_profile_c) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_LUNARG_TEST_PROFILE_C_NAME, VP_LUNARG_TEST_PROFILE_C_SPEC_VERSION};

    VkBool32 multiple_variants = VK_TRUE;
    VkResult result = vpHasMultipleVariantsProfile(&profile, &multiple_variants);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(multiple_variants, VK_FALSE);

    const uint32_t api_version = vpGetProfileAPIVersion(&profile);
    EXPECT_EQ(VK_API_VERSION_MAJOR(api_version), 1);
    EXPECT_EQ(VK_API_VERSION_MINOR(api_version), 3);
    EXPECT_EQ(VK_API_VERSION_PATCH(api_version), 225);

    uint32_t extensions_count = 0;
    vpGetProfileDeviceExtensionProperties(&profile, nullptr, &extensions_count, nullptr);
    std::vector<VkExtensionProperties> extensions(extensions_count);
    vpGetProfileDeviceExtensionProperties(&profile, nullptr, &extensions_count, &extensions[0]);

    EXPECT_STREQ(extensions[0].extensionName, "VK_KHR_get_memory_requirements2");
    EXPECT_STREQ(extensions[1].extensionName, "VK_KHR_driver_properties");
    EXPECT_STREQ(extensions[2].extensionName, "VK_KHR_create_renderpass2");

    VkPhysicalDeviceFeatures2 features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, nullptr};
    vpGetProfileFeatures(&profile, nullptr, &features);

    EXPECT_EQ(features.features.depthBiasClamp, VK_TRUE);
    EXPECT_EQ(features.features.depthClamp, VK_TRUE);
    EXPECT_EQ(features.features.drawIndirectFirstInstance, VK_TRUE);
    EXPECT_EQ(features.features.fullDrawIndexUint32, VK_TRUE);

    VkPhysicalDeviceProperties2 props{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, nullptr};
    vpGetProfileProperties(&profile, nullptr, &props);

    EXPECT_EQ(props.properties.limits.maxImageDimension1D, 4096);
    EXPECT_EQ(props.properties.limits.maxImageDimension2D, 16384);
    EXPECT_EQ(props.properties.limits.maxImageDimension3D, 4096);
    EXPECT_EQ(props.properties.limits.maxImageDimensionCube, 4096);

    // For VP_LUNARG_test_profile_a support
    props.properties.limits.maxImageDimension3D = 4096;

    mock.SetDeviceExtensions(mock.vkPhysicalDevice, extensions);
    mock.SetInstanceAPIVersion(VK_API_VERSION_1_3);
    mock.SetDeviceAPIVersion(VK_API_VERSION_1_3);
    mock.SetFeatures({VK_STRUCT(features)});
    mock.SetProperties({VK_STRUCT(props)});

    VkBool32 supported = VK_FALSE;
    result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_TRUE);
}

TEST(mocked_api_generated_library, check_support_variants_reflection) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_LUNARG_TEST_VARIANTS_NAME, VP_LUNARG_TEST_VARIANTS_SPEC_VERSION};

    VkBool32 multiple_variants = VK_FALSE;
    VkResult result = vpHasMultipleVariantsProfile(&profile, &multiple_variants);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(multiple_variants, VK_TRUE);

    const uint32_t api_version = vpGetProfileAPIVersion(&profile);
    EXPECT_EQ(VK_API_VERSION_MAJOR(api_version), 1);
    EXPECT_EQ(VK_API_VERSION_MINOR(api_version), 3);
    EXPECT_EQ(VK_API_VERSION_PATCH(api_version), 204);

    uint32_t extensions_count = 0;
    result = vpGetProfileDeviceExtensionProperties(&profile, nullptr, &extensions_count, nullptr);
    EXPECT_EQ(result, VK_SUCCESS);
    std::vector<VkExtensionProperties> extensions(extensions_count);
    result = vpGetProfileDeviceExtensionProperties(&profile, nullptr, &extensions_count, &extensions[0]);
    EXPECT_EQ(result, VK_SUCCESS);

    EXPECT_STREQ(extensions[0].extensionName, "VK_KHR_driver_properties");
    EXPECT_STREQ(extensions[1].extensionName, "VK_KHR_get_memory_requirements2");

    // Check behavior for unknown block
    result = vpGetProfileDeviceExtensionProperties(&profile, "pouet", &extensions_count, nullptr);
    EXPECT_EQ(result, VK_INCOMPLETE);
    EXPECT_EQ(extensions_count, 0);

    VkPhysicalDeviceFeatures2 features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, nullptr};
    vpGetProfileFeatures(&profile, nullptr, &features);

    EXPECT_EQ(features.features.depthBiasClamp, VK_TRUE);
    EXPECT_EQ(features.features.depthClamp, VK_TRUE);
    EXPECT_EQ(features.features.drawIndirectFirstInstance, VK_TRUE);
    EXPECT_EQ(features.features.fullDrawIndexUint32, VK_TRUE);
}

TEST(mocked_api_generated_library, check_support_variants_instance_extensions_reflection) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_LUNARG_TEST_VARIANTS_NAME, VP_LUNARG_TEST_VARIANTS_SPEC_VERSION};

    VkResult result = VK_SUCCESS;

    uint32_t extensions_count = 0;
    result = vpGetProfileInstanceExtensionProperties(&profile, "block", &extensions_count, nullptr);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(extensions_count, 0);
}

TEST(mocked_api_generated_library, check_support_variants_device_extensions_reflection) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_LUNARG_TEST_VARIANTS_NAME, VP_LUNARG_TEST_VARIANTS_SPEC_VERSION};

    VkResult result = VK_SUCCESS;

    uint32_t extensions_count = 0;
    result = vpGetProfileDeviceExtensionProperties(&profile, "block", &extensions_count, nullptr);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(extensions_count, 1);

    std::vector<VkExtensionProperties> extensions(extensions_count);
    result = vpGetProfileDeviceExtensionProperties(&profile, "block", &extensions_count, &extensions[0]);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(extensions_count, 1);

    EXPECT_STREQ(extensions[0].extensionName, "VK_KHR_driver_properties");
}

TEST(mocked_api_generated_library, check_support_variants_feature_reflection) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_LUNARG_TEST_VARIANTS_NAME, VP_LUNARG_TEST_VARIANTS_SPEC_VERSION};

    VkPhysicalDeviceFeatures2 features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, nullptr};
    VkResult result = vpGetProfileFeatures(&profile, "block", &features);
    EXPECT_EQ(result, VK_SUCCESS);

    EXPECT_EQ(features.features.depthBiasClamp, VK_TRUE);
    EXPECT_EQ(features.features.depthClamp, VK_TRUE);
    EXPECT_EQ(features.features.drawIndirectFirstInstance, VK_FALSE);
    EXPECT_EQ(features.features.fullDrawIndexUint32, VK_FALSE);
}

TEST(mocked_api_generated_library, check_support_variants_property_reflection) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_LUNARG_TEST_VARIANTS_NAME, VP_LUNARG_TEST_VARIANTS_SPEC_VERSION};

    VkPhysicalDeviceProperties2 props{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, nullptr};

    const VpProfileProperties profileUnknown{"pouet", 1};
    VkResult result = vpGetProfileProperties(&profileUnknown, nullptr, &props);
    EXPECT_EQ(result, VK_ERROR_UNKNOWN);

    result = vpGetProfileProperties(&profile, nullptr, &props);
    EXPECT_EQ(result, VK_ERROR_UNKNOWN);

    EXPECT_EQ(props.properties.limits.maxImageArrayLayers, 0);
    EXPECT_EQ(props.properties.limits.maxImageDimension1D, 0);
    EXPECT_EQ(props.properties.limits.maxImageDimension2D, 0);
    EXPECT_EQ(props.properties.limits.maxImageDimension3D, 0);
    EXPECT_EQ(props.properties.limits.maxImageDimensionCube, 0);

    result = vpGetProfileProperties(&profile, "block", &props);
    EXPECT_EQ(result, VK_SUCCESS);

    EXPECT_EQ(props.properties.limits.maxImageArrayLayers, 0);
    EXPECT_EQ(props.properties.limits.maxImageDimension1D, 4096);
    EXPECT_EQ(props.properties.limits.maxImageDimension2D, 4096);
    EXPECT_EQ(props.properties.limits.maxImageDimension3D, 2048);
    EXPECT_EQ(props.properties.limits.maxImageDimensionCube, 0);

    result = vpGetProfileProperties(&profile, "variant_a", &props);
    EXPECT_EQ(result, VK_SUCCESS);

    EXPECT_EQ(props.properties.limits.maxImageArrayLayers, 0);
    EXPECT_EQ(props.properties.limits.maxImageDimension1D, 8192);
    EXPECT_EQ(props.properties.limits.maxImageDimension2D, 8192);
    EXPECT_EQ(props.properties.limits.maxImageDimension3D, 2048);
    EXPECT_EQ(props.properties.limits.maxImageDimensionCube, 0);

    result = vpGetProfileProperties(&profile, "variant_b", &props);
    EXPECT_EQ(result, VK_SUCCESS);

    EXPECT_EQ(props.properties.limits.maxImageArrayLayers, 0);
    EXPECT_EQ(props.properties.limits.maxImageDimension1D, 4096);
    EXPECT_EQ(props.properties.limits.maxImageDimension2D, 8192);
    EXPECT_EQ(props.properties.limits.maxImageDimension3D, 4096);
    EXPECT_EQ(props.properties.limits.maxImageDimensionCube, 4096);

    result = vpGetProfileProperties(&profile, "variant_unknown", &props);
    EXPECT_EQ(result, VK_INCOMPLETE);
}

TEST(mocked_api_generated_library, check_support_variants_format_reflection) {
    MockVulkanAPI mock;
    VkResult result = VK_SUCCESS;

    const VpProfileProperties profile{VP_LUNARG_TEST_VARIANTS_NAME, VP_LUNARG_TEST_VARIANTS_SPEC_VERSION};

    uint32_t formatCount = 0;
    result = vpGetProfileFormats(&profile, "pouet", &formatCount, nullptr);
    EXPECT_EQ(result, VK_INCOMPLETE);
    EXPECT_EQ(formatCount, 0);

    result = vpGetProfileFormats(&profile, nullptr, &formatCount, nullptr);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(formatCount, 1);

    std::vector<VkFormat> formats(formatCount);
    result = vpGetProfileFormats(&profile, nullptr, &formatCount, &formats[0]);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(formatCount, 1);
    EXPECT_EQ(formats[0], VK_FORMAT_R8G8B8A8_UNORM);

    VkFormatProperties3KHR properties3{VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3_KHR};
    VkFormatProperties2KHR properties2{VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR, &properties3};
    result = vpGetProfileFormatProperties(&profile, "pouet", VK_FORMAT_R8G8B8A8_UNORM, &properties2);
    EXPECT_EQ(result, VK_INCOMPLETE);

    result = vpGetProfileFormatProperties(&profile, nullptr, VK_FORMAT_R8G8B8A8_UNORM, &properties2);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(properties2.formatProperties.bufferFeatures, 0);
    EXPECT_EQ(properties2.formatProperties.optimalTilingFeatures, 0);
    EXPECT_EQ(properties2.formatProperties.linearTilingFeatures,
              VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT);
    EXPECT_EQ(properties3.bufferFeatures, 0);
    EXPECT_EQ(properties3.optimalTilingFeatures, 0);
    EXPECT_EQ(properties3.linearTilingFeatures,
              VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT);

    properties2.formatProperties.linearTilingFeatures = 0;
    properties3.linearTilingFeatures = 0;
    result = vpGetProfileFormatProperties(&profile, "block", VK_FORMAT_R8G8B8A8_UNORM, &properties2);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(properties2.formatProperties.bufferFeatures, 0);
    EXPECT_EQ(properties2.formatProperties.optimalTilingFeatures, 0);
    EXPECT_EQ(properties2.formatProperties.linearTilingFeatures, VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
    EXPECT_EQ(properties3.bufferFeatures, 0);
    EXPECT_EQ(properties3.optimalTilingFeatures, 0);
    EXPECT_EQ(properties3.linearTilingFeatures, VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);

    properties2.formatProperties.linearTilingFeatures = 0;
    properties3.linearTilingFeatures = 0;
    result = vpGetProfileFormatProperties(&profile, "variant_b", VK_FORMAT_R8G8B8A8_UNORM, &properties2);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(properties2.formatProperties.bufferFeatures, 0);
    EXPECT_EQ(properties2.formatProperties.optimalTilingFeatures, 0);
    EXPECT_EQ(properties2.formatProperties.linearTilingFeatures, VK_FORMAT_FEATURE_BLIT_DST_BIT);
    EXPECT_EQ(properties3.bufferFeatures, 0);
    EXPECT_EQ(properties3.optimalTilingFeatures, 0);
    EXPECT_EQ(properties3.linearTilingFeatures, VK_FORMAT_FEATURE_BLIT_DST_BIT);

    uint32_t structureTypeCount = 0;
    result = vpGetProfileFormatStructureTypes(&profile, "pouet", &structureTypeCount, nullptr);
    EXPECT_EQ(result, VK_INCOMPLETE);
    EXPECT_EQ(structureTypeCount, 0);

    result = vpGetProfileFormatStructureTypes(&profile, nullptr, &structureTypeCount, nullptr);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(structureTypeCount, 2);

    std::vector<VkStructureType> structureTypes(structureTypeCount);
    result = vpGetProfileFormatStructureTypes(&profile, nullptr, &structureTypeCount, &structureTypes[0]);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(structureTypeCount, 2);
    EXPECT_EQ(structureTypes[0], VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
    EXPECT_EQ(structureTypes[1], VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3);
}

TEST(mocked_api_generated_library, check_support_variants_success_2variants) {
    MockVulkanAPI mock;
    const VpProfileProperties profile{VP_LUNARG_TEST_VARIANTS_NAME, VP_LUNARG_TEST_VARIANTS_SPEC_VERSION};

    initProfile(mock, profile);
    fixProperties(mock);

    VkBool32 supported = VK_FALSE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_TRUE);

    std::vector<VpBlockProperties> block_properties(10);
    uint32_t block_property_count = static_cast<uint32_t>(block_properties.size());

    result = vpGetPhysicalDeviceProfileVariantsSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported,
                                                       &block_property_count, &block_properties[0]);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_TRUE);
    EXPECT_EQ(block_property_count, 2);
    EXPECT_STREQ(block_properties[0].blockName, "block");
    EXPECT_STREQ(block_properties[1].blockName, "variant_a");
}

TEST(mocked_api_generated_library, check_support_variants_extensions_success_1variant) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_LUNARG_TEST_VARIANTS_NAME, VP_LUNARG_TEST_VARIANTS_SPEC_VERSION};

    initProfile(mock, profile);
    fixProperties(mock);

    {
        mock.ClearProfileAreas(PROFILE_AREA_EXTENSIONS_BIT);

        uint32_t extensions_count = 0;
        vpGetProfileDeviceExtensionProperties(&profile, nullptr, &extensions_count, nullptr);
        std::vector<VkExtensionProperties> extensions(extensions_count);
        vpGetProfileDeviceExtensionProperties(&profile, nullptr, &extensions_count, &extensions[0]);

        // To discard "variant_a" variant support
        extensions.resize(1);

        mock.SetDeviceExtensions(mock.vkPhysicalDevice, extensions);
    }

    VkBool32 supported = VK_FALSE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_TRUE);

    std::vector<VpBlockProperties> block_properties(10);
    uint32_t block_property_count = static_cast<uint32_t>(block_properties.size());

    result = vpGetPhysicalDeviceProfileVariantsSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported,
                                                       &block_property_count, &block_properties[0]);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_TRUE);
    EXPECT_EQ(block_property_count, 2);
    EXPECT_STREQ(block_properties[0].blockName, "block");
    EXPECT_STREQ(block_properties[1].blockName, "variant_b");
}

TEST(mocked_api_generated_library, check_support_variants_extensions_fail) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_LUNARG_TEST_VARIANTS_NAME, VP_LUNARG_TEST_VARIANTS_SPEC_VERSION};

    initProfile(mock, profile);
    fixProperties(mock);

    {
        mock.ClearProfileAreas(PROFILE_AREA_EXTENSIONS_BIT);

        // To discard "variant_a" and "variant_b" variant support
        std::vector<VkExtensionProperties> extensions;

        mock.SetDeviceExtensions(mock.vkPhysicalDevice, extensions);
    }

    VkBool32 supported = VK_TRUE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_FALSE);

    std::vector<VpBlockProperties> block_properties(10);
    uint32_t block_property_count = static_cast<uint32_t>(block_properties.size());

    result = vpGetPhysicalDeviceProfileVariantsSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported,
                                                       &block_property_count, &block_properties[0]);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_FALSE);
    EXPECT_EQ(block_property_count, 3);
    EXPECT_STREQ(block_properties[0].blockName, "block");
    EXPECT_STREQ(block_properties[1].blockName, "variant_a");
    EXPECT_STREQ(block_properties[2].blockName, "variant_b");
}

TEST(mocked_api_generated_library, check_support_variants_features_success_1variants) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_LUNARG_TEST_VARIANTS_NAME, VP_LUNARG_TEST_VARIANTS_SPEC_VERSION};

    initProfile(mock, profile);
    fixProperties(mock);

    // To discard "variant_a" support
    {
        mock.ClearProfileAreas(PROFILE_AREA_FEATURES_BIT);

        VkPhysicalDeviceFeatures2 features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, nullptr};
        vpGetProfileFeatures(&profile, nullptr, &features);

        features.features.drawIndirectFirstInstance = false;

        mock.SetFeatures({VK_STRUCT(features)});
    }

    VkBool32 supported = VK_FALSE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_TRUE);

    std::vector<VpBlockProperties> block_properties(10);
    uint32_t block_property_count = static_cast<uint32_t>(block_properties.size());

    supported = VK_FALSE;
    result = vpGetPhysicalDeviceProfileVariantsSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported,
                                                       &block_property_count, &block_properties[0]);

    EXPECT_EQ(supported, VK_TRUE);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(block_property_count, 2);
    EXPECT_STREQ(block_properties[0].blockName, "block");
    EXPECT_STREQ(block_properties[1].blockName, "variant_b");
}

TEST(mocked_api_generated_library, check_support_variants_features_fail) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_LUNARG_TEST_VARIANTS_NAME, VP_LUNARG_TEST_VARIANTS_SPEC_VERSION};

    initProfile(mock, profile);
    fixProperties(mock);

    // To discard "variant_a" and "variant_b" support
    {
        mock.ClearProfileAreas(PROFILE_AREA_FEATURES_BIT);

        VkPhysicalDeviceFeatures2 features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, nullptr};
        vpGetProfileFeatures(&profile, nullptr, &features);

        features.features.drawIndirectFirstInstance = false;
        features.features.fullDrawIndexUint32 = false;

        mock.SetFeatures({VK_STRUCT(features)});
    }

    VkBool32 supported = VK_TRUE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_FALSE);

    std::vector<VpBlockProperties> block_properties(10);
    uint32_t block_property_count = static_cast<uint32_t>(block_properties.size());

    supported = VK_TRUE;
    result = vpGetPhysicalDeviceProfileVariantsSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported,
                                                       &block_property_count, &block_properties[0]);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_FALSE);
    EXPECT_EQ(block_property_count, 2);
}

TEST(mocked_api_generated_library, check_support_variants_properties_success_1variants) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_LUNARG_TEST_VARIANTS_NAME, VP_LUNARG_TEST_VARIANTS_SPEC_VERSION};

    initProfile(mock, profile);

    {
        mock.ClearProfileAreas(PROFILE_AREA_PROPERTIES_BIT);

        VkPhysicalDeviceProperties2 props{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, nullptr};
        vpGetProfileProperties(&profile, nullptr, &props);
        props.properties.limits.maxImageDimension1D = 8192;
        props.properties.limits.maxImageDimension2D = 8192;
        props.properties.limits.maxImageDimension3D = 4096;
        props.properties.limits.maxImageDimensionCube = 4096;

        // To discard "variant_a" variant support
        props.properties.limits.maxImageDimension1D = 4096;

        mock.SetProperties({VK_STRUCT(props)});
    }

    VkBool32 supported = VK_FALSE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_TRUE);

    std::vector<VpBlockProperties> block_properties(10);
    uint32_t block_property_count = static_cast<uint32_t>(block_properties.size());

    supported = VK_FALSE;
    result = vpGetPhysicalDeviceProfileVariantsSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported,
        &block_property_count, &block_properties[0]);

    EXPECT_EQ(supported, VK_TRUE);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(block_property_count, 2);
    EXPECT_STREQ(block_properties[0].blockName, "block");
    EXPECT_STREQ(block_properties[1].blockName, "variant_b");
}

TEST(mocked_api_generated_library, check_support_variants_properties_fail) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_LUNARG_TEST_VARIANTS_NAME, VP_LUNARG_TEST_VARIANTS_SPEC_VERSION};

    initProfile(mock, profile);

    {
        mock.ClearProfileAreas(PROFILE_AREA_PROPERTIES_BIT);

        VkPhysicalDeviceProperties2 props{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, nullptr};
        vpGetProfileProperties(&profile, nullptr, &props);
        props.properties.limits.maxImageDimension1D = 8192;
        props.properties.limits.maxImageDimension2D = 8192;
        props.properties.limits.maxImageDimension3D = 4096;
        props.properties.limits.maxImageDimensionCube = 4096;

        // To discard "variant_a" and "variant_b" support
        props.properties.limits.maxImageDimension1D = 4096;
        props.properties.limits.maxImageDimensionCube = 2048;
        
        mock.SetProperties({VK_STRUCT(props)});
    }

    VkBool32 supported = VK_TRUE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_FALSE);

    std::vector<VpBlockProperties> block_properties(10);
    uint32_t block_property_count = static_cast<uint32_t>(block_properties.size());

    supported = VK_TRUE;
    result = vpGetPhysicalDeviceProfileVariantsSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported,
                                                       &block_property_count, &block_properties[0]);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_FALSE);
    EXPECT_EQ(block_property_count, 2);
}

TEST(mocked_api_generated_library, check_support_variants_format_success_1variants) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_LUNARG_TEST_VARIANTS_NAME, VP_LUNARG_TEST_VARIANTS_SPEC_VERSION};

    initProfile(mock, profile);
    fixProperties(mock);

    // To discard "variant_a" support
    {
        mock.ClearProfileAreas(PROFILE_AREA_FORMATS_BIT);

        VkFormatProperties2KHR formatProps{VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR};
        vpGetProfileFormatProperties(&profile, nullptr, VK_FORMAT_R8G8B8A8_UNORM, &formatProps);
        formatProps.formatProperties.linearTilingFeatures =
            VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT;

        mock.AddFormat(VK_FORMAT_R8G8B8A8_UNORM, {VK_STRUCT(formatProps)});
    }

    VkBool32 supported = VK_FALSE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_TRUE);

    std::vector<VpBlockProperties> block_properties(10);
    uint32_t block_property_count = static_cast<uint32_t>(block_properties.size());

    supported = VK_FALSE;
    result = vpGetPhysicalDeviceProfileVariantsSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported,
                                                       &block_property_count, &block_properties[0]);

    EXPECT_EQ(supported, VK_TRUE);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(block_property_count, 2);
    EXPECT_STREQ(block_properties[0].blockName, "block");
    EXPECT_STREQ(block_properties[1].blockName, "variant_b");
}

TEST(mocked_api_generated_library, check_support_variants_format_fail) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_LUNARG_TEST_VARIANTS_NAME, VP_LUNARG_TEST_VARIANTS_SPEC_VERSION};

    initProfile(mock, profile);
    fixProperties(mock);

    // To discard "variant_a" and "variant_b" support
    {
        mock.ClearProfileAreas(PROFILE_AREA_FORMATS_BIT);

        VkFormatProperties2KHR formatProps{VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR};
        vpGetProfileFormatProperties(&profile, nullptr, VK_FORMAT_R8G8B8A8_UNORM, &formatProps);
        formatProps.formatProperties.linearTilingFeatures =
            VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;

        mock.AddFormat(VK_FORMAT_R8G8B8A8_UNORM, {VK_STRUCT(formatProps)});
    }

    VkBool32 supported = VK_TRUE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_FALSE);

    std::vector<VpBlockProperties> block_properties(10);
    uint32_t block_property_count = static_cast<uint32_t>(block_properties.size());

    supported = VK_TRUE;
    result = vpGetPhysicalDeviceProfileVariantsSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported,
                                                       &block_property_count, &block_properties[0]);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_FALSE);
    EXPECT_EQ(block_property_count, 2);
    EXPECT_STREQ(block_properties[0].blockName, "variant_a");
    EXPECT_STREQ(block_properties[1].blockName, "variant_b");
}
