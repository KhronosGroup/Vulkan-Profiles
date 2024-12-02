/*
 * Copyright (c) 2023-2023 Valve Corporation
 * Copyright (c) 2023-2023 LunarG, Inc.
 * Copyright (c) 2024 RasterGrid Kft.
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

    if (profileAreas & PROFILE_AREA_QUEUE_FAMILIES_BIT) {
        uint32_t queue_family_count = 0;
        vpGetProfileQueueFamilyProperties(&profile, nullptr, &queue_family_count, nullptr);
        std::vector<VkQueueFamilyProperties2KHR> props(queue_family_count, {VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2, nullptr});
        std::vector<VkQueueFamilyVideoPropertiesKHR> video_props(queue_family_count,
                                                                 {VK_STRUCTURE_TYPE_QUEUE_FAMILY_VIDEO_PROPERTIES_KHR, nullptr});
        for (uint32_t i = 0; i < queue_family_count; ++i) {
            props[i].pNext = &video_props[i];
        }
        vpGetProfileQueueFamilyProperties(&profile, nullptr, &queue_family_count, props.data());
        for (uint32_t i = 0; i < queue_family_count; ++i) {
            mock.AddQueueFamily({VK_STRUCT(props[i]), VK_STRUCT(video_props[i])});
        }
    }

    if (profileAreas & PROFILE_AREA_FORMATS_BIT) {
        uint32_t formatCount = 0;
        vpGetProfileFormats(&profile, nullptr, &formatCount, nullptr);
        if (formatCount > 0) {
            std::vector<VkFormat> formats(formatCount);
            vpGetProfileFormats(&profile, nullptr, &formatCount, &formats[0]);

            for (std::size_t i = 0, n = formats.size(); i < n; ++i) {
                VkFormatProperties2KHR formatProps{VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR};
                vpGetProfileFormatProperties(&profile, nullptr, formats[i], &formatProps);
                mock.AddFormat(formats[i], {VK_STRUCT(formatProps)});
            }
        }
    }

    auto get_video_profile = [&](uint32_t video_profile_index) {
        VkVideoDecodeH264ProfileInfoKHR decode_h264{VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_PROFILE_INFO_KHR};
        VkVideoDecodeH265ProfileInfoKHR decode_h265{VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_PROFILE_INFO_KHR};
        VkVideoProfileInfoKHR profile_info{VK_STRUCTURE_TYPE_VIDEO_PROFILE_INFO_KHR};

        uint32_t struct_type_count = 0;
        vpGetProfileVideoProfileInfoStructureTypes(&profile, nullptr, video_profile_index, &struct_type_count, nullptr);
        std::vector<VkStructureType> struct_types(struct_type_count);
        vpGetProfileVideoProfileInfoStructureTypes(&profile, nullptr, video_profile_index, &struct_type_count, struct_types.data());

        std::vector<VulkanStructData> struct_data{};
        for (auto struct_type : struct_types) {
            switch (struct_type) {
                case VK_STRUCTURE_TYPE_VIDEO_PROFILE_INFO_KHR:
                    break;
                case VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_PROFILE_INFO_KHR:
                    profile_info.pNext = &decode_h264;
                    break;
                case VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_PROFILE_INFO_KHR:
                    profile_info.pNext = &decode_h265;
                    break;
                default:
                    assert(false);
                    break;
            }
        }

        vpGetProfileVideoProfileInfo(&profile, nullptr, video_profile_index, &profile_info);
        return VulkanVideoProfile(&profile_info);
    };

    if (profileAreas & PROFILE_AREA_VIDEO_CAPABILITIES_BIT) {
        mock.InitVideoEntryPoints();

        uint32_t video_profile_count = 0;
        vpGetProfileVideoProfiles(&profile, nullptr, &video_profile_count, nullptr);
        for (uint32_t video_profile_index = 0; video_profile_index < video_profile_count; ++video_profile_index) {
            auto video_profile = get_video_profile(video_profile_index);

            VkVideoDecodeH264CapabilitiesKHR h264_decode{VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_CAPABILITIES_KHR, nullptr};
            VkVideoDecodeH265CapabilitiesKHR h265_decode{VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_CAPABILITIES_KHR, &h264_decode};
            VkVideoCapabilitiesKHR caps{VK_STRUCTURE_TYPE_VIDEO_CAPABILITIES_KHR, &h265_decode};
            vpGetProfileVideoCapabilities(&profile, nullptr, video_profile_index, &caps);

            uint32_t struct_type_count = 0;
            vpGetProfileVideoCapabilityStructureTypes(&profile, nullptr, video_profile_index, &struct_type_count, nullptr);
            std::vector<VkStructureType> struct_types(struct_type_count);
            vpGetProfileVideoCapabilityStructureTypes(&profile, nullptr, video_profile_index, &struct_type_count,
                                                      struct_types.data());

            std::vector<VulkanStructData> struct_data{};
            for (auto struct_type : struct_types) {
                switch (struct_type) {
                    case VK_STRUCTURE_TYPE_VIDEO_CAPABILITIES_KHR:
                        struct_data.emplace_back(VK_STRUCT(caps));
                        break;
                    case VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_CAPABILITIES_KHR:
                        struct_data.emplace_back(VK_STRUCT(h264_decode));
                        break;
                    case VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_CAPABILITIES_KHR:
                        struct_data.emplace_back(VK_STRUCT(h265_decode));
                        break;
                    default:
                        break;
                }
            }

            mock.AddVideoCapabilities(video_profile, std::move(struct_data));
        }
    }

    if (profileAreas & PROFILE_AREA_VIDEO_FORMATS_BIT) {
        mock.InitVideoEntryPoints();

        uint32_t video_profile_count = 0;
        vpGetProfileVideoProfiles(&profile, nullptr, &video_profile_count, nullptr);
        for (uint32_t video_profile_index = 0; video_profile_index < video_profile_count; ++video_profile_index) {
            auto video_profile = get_video_profile(video_profile_index);

            uint32_t format_count = 0;
            vpGetProfileVideoFormatProperties(&profile, nullptr, video_profile_index, &format_count, nullptr);

            if (format_count > 0) {
                std::vector<VkVideoFormatPropertiesKHR> props(format_count,
                                                              {VK_STRUCTURE_TYPE_VIDEO_FORMAT_PROPERTIES_KHR, nullptr});
                vpGetProfileVideoFormatProperties(&profile, nullptr, video_profile_index, &format_count, props.data());

                for (uint32_t i = 0; i < format_count; ++i) {
                    mock.AddVideoFormat(video_profile, {VK_STRUCT(props[i])});
                }
            }
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

TEST(mocked_api_generated_library, check_support_profile_queue_families) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_RASTERGRID_TEST_QUEUE_FAMILIES_NAME, VP_RASTERGRID_TEST_QUEUE_FAMILIES_SPEC_VERSION};

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

    EXPECT_EQ(extensions_count, 5);

    EXPECT_STREQ(extensions[0].extensionName, "VK_KHR_video_decode_h264");
    EXPECT_STREQ(extensions[1].extensionName, "VK_KHR_video_decode_h265");
    EXPECT_STREQ(extensions[2].extensionName, "VK_KHR_video_decode_queue");
    EXPECT_STREQ(extensions[3].extensionName, "VK_KHR_video_encode_queue");
    EXPECT_STREQ(extensions[4].extensionName, "VK_KHR_video_queue");

    uint32_t queue_family_count = 0;
    vpGetProfileQueueFamilyProperties(&profile, nullptr, &queue_family_count, nullptr);

    EXPECT_EQ(queue_family_count, 4);

    std::vector<VkQueueFamilyProperties2> queue_family_props(queue_family_count,
                                                             {VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2, nullptr});
    std::vector<VkQueueFamilyVideoPropertiesKHR> queue_family_video_props(
        queue_family_count, {VK_STRUCTURE_TYPE_QUEUE_FAMILY_VIDEO_PROPERTIES_KHR, nullptr});
    for (uint32_t i = 0; i < queue_family_count; ++i) {
        queue_family_props[i].pNext = &queue_family_video_props[i];
    }
    vpGetProfileQueueFamilyProperties(&profile, nullptr, &queue_family_count, queue_family_props.data());

    EXPECT_EQ(queue_family_count, 4);

    EXPECT_EQ(queue_family_props[0].queueFamilyProperties.queueFlags,
              VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT);
    EXPECT_EQ(queue_family_props[0].queueFamilyProperties.queueCount, 1);
    EXPECT_EQ(queue_family_video_props[0].videoCodecOperations, 0);

    EXPECT_EQ(queue_family_props[1].queueFamilyProperties.queueFlags, VK_QUEUE_VIDEO_DECODE_BIT_KHR);
    EXPECT_EQ(queue_family_props[1].queueFamilyProperties.queueCount, 1);
    EXPECT_EQ(queue_family_video_props[1].videoCodecOperations, VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_KHR);

    EXPECT_EQ(queue_family_props[2].queueFamilyProperties.queueFlags, VK_QUEUE_VIDEO_DECODE_BIT_KHR);
    EXPECT_EQ(queue_family_props[2].queueFamilyProperties.queueCount, 1);
    EXPECT_EQ(queue_family_video_props[2].videoCodecOperations, VK_VIDEO_CODEC_OPERATION_DECODE_H265_BIT_KHR);

    EXPECT_EQ(queue_family_props[3].queueFamilyProperties.queueFlags, VK_QUEUE_VIDEO_ENCODE_BIT_KHR);
    EXPECT_EQ(queue_family_props[3].queueFamilyProperties.queueCount, 1);
    EXPECT_EQ(queue_family_video_props[3].videoCodecOperations, 0);

    mock.SetDeviceExtensions(mock.vkPhysicalDevice, extensions);
    mock.SetInstanceAPIVersion(VK_API_VERSION_1_3);
    mock.SetDeviceAPIVersion(VK_API_VERSION_1_3);
    for (uint32_t i = 0; i < queue_family_count; ++i) {
        mock.AddQueueFamily({VK_STRUCT(queue_family_props[i]), VK_STRUCT(queue_family_video_props[i])});
    }

    VkBool32 supported = VK_FALSE;
    result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_TRUE);

    // Test also that it's fine to expose multiple required queue flags on the same queue family
    queue_family_props[0].queueFamilyProperties.queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
    queue_family_props[0].queueFamilyProperties.queueCount = 1;
    queue_family_video_props[0].videoCodecOperations = 0;

    queue_family_props[1].queueFamilyProperties.queueFlags = VK_QUEUE_VIDEO_DECODE_BIT_KHR | VK_QUEUE_VIDEO_ENCODE_BIT_KHR;
    queue_family_props[1].queueFamilyProperties.queueCount = 1;
    queue_family_video_props[1].videoCodecOperations =
        VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_KHR | VK_VIDEO_CODEC_OPERATION_DECODE_H265_BIT_KHR;

    mock.ClearProfileAreas(PROFILE_AREA_QUEUE_FAMILIES_BIT);
    for (uint32_t i = 0; i < 2; ++i) {
        mock.AddQueueFamily({VK_STRUCT(queue_family_props[i]), VK_STRUCT(queue_family_video_props[i])});
    }

    supported = VK_FALSE;
    result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_TRUE);

    // But if queue flags required by a single queue family are split across multiple queue families, then it is not accepted
    queue_family_props[0].queueFamilyProperties.queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT;
    queue_family_props[0].queueFamilyProperties.queueCount = 1;
    queue_family_video_props[0].videoCodecOperations = 0;

    queue_family_props[1].queueFamilyProperties.queueFlags = VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
    queue_family_props[1].queueFamilyProperties.queueCount = 1;
    queue_family_video_props[1].videoCodecOperations = 0;

    queue_family_props[2].queueFamilyProperties.queueFlags = VK_QUEUE_VIDEO_DECODE_BIT_KHR | VK_QUEUE_VIDEO_ENCODE_BIT_KHR;
    queue_family_props[2].queueFamilyProperties.queueCount = 1;
    queue_family_video_props[2].videoCodecOperations =
        VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_KHR | VK_VIDEO_CODEC_OPERATION_DECODE_H265_BIT_KHR;

    mock.ClearProfileAreas(PROFILE_AREA_QUEUE_FAMILIES_BIT);
    for (uint32_t i = 0; i < 3; ++i) {
        mock.AddQueueFamily({VK_STRUCT(queue_family_props[i]), VK_STRUCT(queue_family_video_props[i])});
    }

    supported = VK_TRUE;
    result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_FALSE);
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

TEST(mocked_api_generated_library, check_support_variants_queue_family_reflection) {
    const VpProfileProperties profile{VP_LUNARG_TEST_VARIANTS_NAME, VP_LUNARG_TEST_VARIANTS_SPEC_VERSION};

    VkResult result = VK_SUCCESS;
    uint32_t queue_family_count = 0;
    std::vector<VkQueueFamilyProperties2KHR> props{};

    const VpProfileProperties profileUnknown{"pouet", 1};
    result = vpGetProfileQueueFamilyProperties(&profileUnknown, nullptr, &queue_family_count, nullptr);
    EXPECT_EQ(result, VK_ERROR_UNKNOWN);

    result = vpGetProfileQueueFamilyProperties(&profile, nullptr, &queue_family_count, nullptr);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(queue_family_count, 4);

    props.resize(queue_family_count, {VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2, nullptr});
    result = vpGetProfileQueueFamilyProperties(&profile, nullptr, &queue_family_count, props.data());
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(queue_family_count, 4);

    EXPECT_EQ(props[0].queueFamilyProperties.queueFlags, VK_QUEUE_TRANSFER_BIT);
    EXPECT_EQ(props[0].queueFamilyProperties.queueCount, 2);
    EXPECT_EQ(props[1].queueFamilyProperties.queueFlags, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);
    EXPECT_EQ(props[1].queueFamilyProperties.queueCount, 2);
    EXPECT_EQ(props[2].queueFamilyProperties.queueFlags, VK_QUEUE_COMPUTE_BIT);
    EXPECT_EQ(props[2].queueFamilyProperties.queueCount, 2);
    EXPECT_EQ(props[3].queueFamilyProperties.queueFlags, VK_QUEUE_PROTECTED_BIT);
    EXPECT_EQ(props[3].queueFamilyProperties.queueCount, 1);

    queue_family_count = 2;
    props.clear();
    props.resize(queue_family_count, {VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2, nullptr});
    result = vpGetProfileQueueFamilyProperties(&profile, nullptr, &queue_family_count, props.data());
    EXPECT_EQ(result, VK_INCOMPLETE);
    EXPECT_EQ(queue_family_count, 2);

    EXPECT_EQ(props[0].queueFamilyProperties.queueFlags, VK_QUEUE_TRANSFER_BIT);
    EXPECT_EQ(props[0].queueFamilyProperties.queueCount, 2);
    EXPECT_EQ(props[1].queueFamilyProperties.queueFlags, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);
    EXPECT_EQ(props[1].queueFamilyProperties.queueCount, 2);

    result = vpGetProfileQueueFamilyProperties(&profile, "block", &queue_family_count, nullptr);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(queue_family_count, 1);

    props.clear();
    props.resize(queue_family_count, {VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2, nullptr});
    result = vpGetProfileQueueFamilyProperties(&profile, "block", &queue_family_count, props.data());
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(queue_family_count, 1);

    EXPECT_EQ(props[0].queueFamilyProperties.queueFlags, VK_QUEUE_TRANSFER_BIT);
    EXPECT_EQ(props[0].queueFamilyProperties.queueCount, 2);

    result = vpGetProfileQueueFamilyProperties(&profile, "variant_a", &queue_family_count, nullptr);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(queue_family_count, 1);

    props.clear();
    props.resize(queue_family_count, {VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2, nullptr});
    result = vpGetProfileQueueFamilyProperties(&profile, "variant_a", &queue_family_count, props.data());
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(queue_family_count, 1);

    EXPECT_EQ(props[0].queueFamilyProperties.queueFlags, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);
    EXPECT_EQ(props[0].queueFamilyProperties.queueCount, 2);

    result = vpGetProfileQueueFamilyProperties(&profile, "variant_b", &queue_family_count, nullptr);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(queue_family_count, 2);

    props.clear();
    props.resize(queue_family_count, {VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2, nullptr});
    result = vpGetProfileQueueFamilyProperties(&profile, "variant_b", &queue_family_count, props.data());
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(queue_family_count, 2);

    EXPECT_EQ(props[0].queueFamilyProperties.queueFlags, VK_QUEUE_COMPUTE_BIT);
    EXPECT_EQ(props[0].queueFamilyProperties.queueCount, 2);
    EXPECT_EQ(props[1].queueFamilyProperties.queueFlags, VK_QUEUE_PROTECTED_BIT);
    EXPECT_EQ(props[1].queueFamilyProperties.queueCount, 1);

    result = vpGetProfileQueueFamilyProperties(&profile, "variant_unknown", &queue_family_count, nullptr);
    EXPECT_EQ(result, VK_INCOMPLETE);

    const VpProfileProperties profile2{VP_RASTERGRID_TEST_VIDEO_PROFILES_NAME, VP_RASTERGRID_TEST_VIDEO_PROFILES_SPEC_VERSION};

    uint32_t structure_type_count = 0;
    result = vpGetProfileQueueFamilyStructureTypes(&profile2, "pouet", &structure_type_count, nullptr);
    EXPECT_EQ(result, VK_INCOMPLETE);
    EXPECT_EQ(structure_type_count, 0);

    result = vpGetProfileQueueFamilyStructureTypes(&profile2, nullptr, &structure_type_count, nullptr);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(structure_type_count, 2);

    std::vector<VkStructureType> structure_types(structure_type_count);
    result = vpGetProfileQueueFamilyStructureTypes(&profile2, nullptr, &structure_type_count, structure_types.data());
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(structure_type_count, 2);
    EXPECT_EQ(structure_types[0], VK_STRUCTURE_TYPE_QUEUE_FAMILY_VIDEO_PROPERTIES_KHR);
    EXPECT_EQ(structure_types[1], VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2);
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

TEST(mocked_api_generated_library, check_support_variants_queue_family_success_2variants) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_LUNARG_TEST_VARIANTS_NAME, VP_LUNARG_TEST_VARIANTS_SPEC_VERSION};

    initProfile(mock, profile);
    fixProperties(mock);

    mock.ClearProfileAreas(PROFILE_AREA_QUEUE_FAMILIES_BIT);

    VkQueueFamilyProperties2KHR props{VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2};

    props.queueFamilyProperties.queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
    props.queueFamilyProperties.queueCount = 2;
    mock.AddQueueFamily({VK_STRUCT(props)});

    props.queueFamilyProperties.queueFlags = VK_QUEUE_TRANSFER_BIT | VK_QUEUE_PROTECTED_BIT;
    props.queueFamilyProperties.queueCount = 1;
    mock.AddQueueFamily({VK_STRUCT(props)});

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
    EXPECT_STREQ(block_properties[1].blockName, "variant_a");
}

TEST(mocked_api_generated_library, check_support_variants_queue_family_success_1variant) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_LUNARG_TEST_VARIANTS_NAME, VP_LUNARG_TEST_VARIANTS_SPEC_VERSION};

    initProfile(mock, profile);
    fixProperties(mock);

    mock.ClearProfileAreas(PROFILE_AREA_QUEUE_FAMILIES_BIT);

    VkQueueFamilyProperties2KHR props{VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2};

    props.queueFamilyProperties.queueFlags = VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
    props.queueFamilyProperties.queueCount = 2;
    mock.AddQueueFamily({VK_STRUCT(props)});

    props.queueFamilyProperties.queueFlags = VK_QUEUE_TRANSFER_BIT | VK_QUEUE_PROTECTED_BIT;
    props.queueFamilyProperties.queueCount = 1;
    mock.AddQueueFamily({VK_STRUCT(props)});

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

TEST(mocked_api_generated_library, check_support_variants_queue_family_fail) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_LUNARG_TEST_VARIANTS_NAME, VP_LUNARG_TEST_VARIANTS_SPEC_VERSION};

    initProfile(mock, profile);
    fixProperties(mock);

    mock.ClearProfileAreas(PROFILE_AREA_QUEUE_FAMILIES_BIT);

    VkQueueFamilyProperties2KHR props{VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2};

    props.queueFamilyProperties.queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT;
    props.queueFamilyProperties.queueCount = 2;
    mock.AddQueueFamily({VK_STRUCT(props)});

    props.queueFamilyProperties.queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_PROTECTED_BIT;
    props.queueFamilyProperties.queueCount = 1;
    mock.AddQueueFamily({VK_STRUCT(props)});

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

TEST(mocked_api_generated_library, check_support_variants_format_success_1variant) {
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

TEST(mocked_api_generated_library, check_support_variants_video_profile_reflection) {
    const VpProfileProperties profile{VP_RASTERGRID_TEST_VIDEO_PROFILES_NAME, VP_RASTERGRID_TEST_VIDEO_PROFILES_SPEC_VERSION};

    VkResult result = VK_SUCCESS;
    uint32_t video_profile_count = 0;
    std::vector<VpVideoProfileProperties> props{};

    const VpProfileProperties profileUnknown{"pouet", 1};
    result = vpGetProfileVideoProfiles(&profileUnknown, nullptr, &video_profile_count, nullptr);
    EXPECT_EQ(result, VK_ERROR_UNKNOWN);

    result = vpGetProfileVideoProfiles(&profile, nullptr, &video_profile_count, nullptr);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(video_profile_count, 3);

    props.resize(video_profile_count);
    result = vpGetProfileVideoProfiles(&profile, nullptr, &video_profile_count, props.data());
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(video_profile_count, 3);

    EXPECT_STREQ(props[0].name, "H.264 Decode (4:2:0 8-bit) Main progressive");
    EXPECT_STREQ(props[1].name, "H.265 Decode (4:2:0 8-bit) Main");
    EXPECT_STREQ(props[2].name, "H.265 Decode (4:2:0 10-bit) Main 10");

    video_profile_count = 2;
    props.clear();
    props.resize(video_profile_count);

    result = vpGetProfileVideoProfiles(&profile, nullptr, &video_profile_count, props.data());
    EXPECT_EQ(result, VK_INCOMPLETE);
    EXPECT_EQ(video_profile_count, 2);

    EXPECT_STREQ(props[0].name, "H.264 Decode (4:2:0 8-bit) Main progressive");
    EXPECT_STREQ(props[1].name, "H.265 Decode (4:2:0 8-bit) Main");

    result = vpGetProfileVideoProfiles(&profile, "block", &video_profile_count, nullptr);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(video_profile_count, 0);

    result = vpGetProfileVideoProfiles(&profile, "variant_h264", &video_profile_count, nullptr);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(video_profile_count, 1);

    props.clear();
    props.resize(video_profile_count);
    result = vpGetProfileVideoProfiles(&profile, "variant_h264", &video_profile_count, props.data());
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(video_profile_count, 1);

    EXPECT_STREQ(props[0].name, "H.264 Decode (4:2:0 8-bit) Main progressive");

    result = vpGetProfileVideoProfiles(&profile, "variant_h265", &video_profile_count, nullptr);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(video_profile_count, 2);

    props.clear();
    props.resize(video_profile_count);
    result = vpGetProfileVideoProfiles(&profile, "variant_h265", &video_profile_count, props.data());
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(video_profile_count, 2);

    EXPECT_STREQ(props[0].name, "H.265 Decode (4:2:0 8-bit) Main");
    EXPECT_STREQ(props[1].name, "H.265 Decode (4:2:0 10-bit) Main 10");

    result = vpGetProfileVideoProfiles(&profile, "variant_unknown", &video_profile_count, nullptr);
    EXPECT_EQ(result, VK_INCOMPLETE);
}

TEST(mocked_api_generated_library, check_support_variants_video_profile_info_reflection) {
    const VpProfileProperties profile{VP_RASTERGRID_TEST_VIDEO_PROFILES_NAME, VP_RASTERGRID_TEST_VIDEO_PROFILES_SPEC_VERSION};

    VkResult result = VK_SUCCESS;

    VkVideoDecodeH264ProfileInfoKHR profile_info_h264{VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_PROFILE_INFO_KHR, nullptr};
    VkVideoDecodeH265ProfileInfoKHR profile_info_h265{VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_PROFILE_INFO_KHR, &profile_info_h264};
    VkVideoProfileInfoKHR profile_info{VK_STRUCTURE_TYPE_VIDEO_PROFILE_INFO_KHR, &profile_info_h265};

    auto clear_chain = [&] {
        profile_info_h264 = VkVideoDecodeH264ProfileInfoKHR{VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_PROFILE_INFO_KHR, nullptr};
        profile_info_h265 =
            VkVideoDecodeH265ProfileInfoKHR{VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_PROFILE_INFO_KHR, &profile_info_h264};
        profile_info = VkVideoProfileInfoKHR{VK_STRUCTURE_TYPE_VIDEO_PROFILE_INFO_KHR, &profile_info_h265};
    };

    // H.264 Decode (4:2:0 8-bit) Main progressive

    auto check_h264_main = [&] {
        EXPECT_EQ(profile_info.videoCodecOperation, VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_KHR);
        EXPECT_EQ(profile_info.chromaSubsampling, VK_VIDEO_CHROMA_SUBSAMPLING_420_BIT_KHR);
        EXPECT_EQ(profile_info.lumaBitDepth, VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR);
        EXPECT_EQ(profile_info.chromaBitDepth, VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR);
        EXPECT_EQ(profile_info_h264.stdProfileIdc, STD_VIDEO_H264_PROFILE_IDC_MAIN);
        EXPECT_EQ(profile_info_h264.pictureLayout, VK_VIDEO_DECODE_H264_PICTURE_LAYOUT_PROGRESSIVE_KHR);
    };

    clear_chain();
    result = vpGetProfileVideoProfileInfo(&profile, nullptr, 0, &profile_info);
    EXPECT_EQ(result, VK_SUCCESS);
    check_h264_main();

    clear_chain();
    result = vpGetProfileVideoProfileInfo(&profile, "block", 0, &profile_info);
    EXPECT_EQ(result, VK_ERROR_UNKNOWN);

    clear_chain();
    result = vpGetProfileVideoProfileInfo(&profile, "variant_h264", 0, &profile_info);
    EXPECT_EQ(result, VK_SUCCESS);
    check_h264_main();

    clear_chain();
    result = vpGetProfileVideoProfileInfo(&profile, "variant_h265", 0, &profile_info);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_NE(profile_info.videoCodecOperation, VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_KHR);

    // H.265 Decode (4:2:0 8-bit) Main

    auto check_h265_main = [&] {
        EXPECT_EQ(profile_info.videoCodecOperation, VK_VIDEO_CODEC_OPERATION_DECODE_H265_BIT_KHR);
        EXPECT_EQ(profile_info.chromaSubsampling, VK_VIDEO_CHROMA_SUBSAMPLING_420_BIT_KHR);
        EXPECT_EQ(profile_info.lumaBitDepth, VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR);
        EXPECT_EQ(profile_info.chromaBitDepth, VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR);
        EXPECT_EQ(profile_info_h265.stdProfileIdc, STD_VIDEO_H265_PROFILE_IDC_MAIN);
    };

    clear_chain();
    result = vpGetProfileVideoProfileInfo(&profile, nullptr, 1, &profile_info);
    EXPECT_EQ(result, VK_SUCCESS);
    check_h265_main();

    clear_chain();
    result = vpGetProfileVideoProfileInfo(&profile, "block", 0, &profile_info);
    EXPECT_EQ(result, VK_ERROR_UNKNOWN);

    clear_chain();
    result = vpGetProfileVideoProfileInfo(&profile, "variant_h264", 0, &profile_info);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_NE(profile_info.videoCodecOperation, VK_VIDEO_CODEC_OPERATION_DECODE_H265_BIT_KHR);

    clear_chain();
    result = vpGetProfileVideoProfileInfo(&profile, "variant_h265", 0, &profile_info);
    EXPECT_EQ(result, VK_SUCCESS);
    check_h265_main();

    // H.265 Decode (4:2:0 10-bit) Main 10

    auto check_h265_main_10 = [&] {
        EXPECT_EQ(profile_info.videoCodecOperation, VK_VIDEO_CODEC_OPERATION_DECODE_H265_BIT_KHR);
        EXPECT_EQ(profile_info.chromaSubsampling, VK_VIDEO_CHROMA_SUBSAMPLING_420_BIT_KHR);
        EXPECT_EQ(profile_info.lumaBitDepth, VK_VIDEO_COMPONENT_BIT_DEPTH_10_BIT_KHR);
        EXPECT_EQ(profile_info.chromaBitDepth, VK_VIDEO_COMPONENT_BIT_DEPTH_10_BIT_KHR);
        EXPECT_EQ(profile_info_h265.stdProfileIdc, STD_VIDEO_H265_PROFILE_IDC_MAIN_10);
    };

    clear_chain();
    result = vpGetProfileVideoProfileInfo(&profile, nullptr, 2, &profile_info);
    EXPECT_EQ(result, VK_SUCCESS);
    check_h265_main_10();

    clear_chain();
    result = vpGetProfileVideoProfileInfo(&profile, "block", 2, &profile_info);
    EXPECT_EQ(result, VK_ERROR_UNKNOWN);

    clear_chain();
    result = vpGetProfileVideoProfileInfo(&profile, "variant_h264", 1, &profile_info);
    EXPECT_EQ(result, VK_ERROR_UNKNOWN);

    clear_chain();
    result = vpGetProfileVideoProfileInfo(&profile, "variant_h265", 1, &profile_info);
    EXPECT_EQ(result, VK_SUCCESS);
    check_h265_main_10();

    // Structure types

    uint32_t structure_type_count = 0;
    std::vector<VkStructureType> structure_types{};

    for (uint32_t i = 0; i < 3; ++i) {
        result = vpGetProfileVideoProfileInfoStructureTypes(&profile, nullptr, i, &structure_type_count, nullptr);
        EXPECT_EQ(result, VK_SUCCESS);
        EXPECT_EQ(structure_type_count, 2);

        structure_types.clear();
        structure_types.resize(structure_type_count);
        result = vpGetProfileVideoProfileInfoStructureTypes(&profile, nullptr, i, &structure_type_count, structure_types.data());
        EXPECT_EQ(result, VK_SUCCESS);
        EXPECT_EQ(structure_type_count, 2);

        EXPECT_EQ(structure_types[0], VK_STRUCTURE_TYPE_VIDEO_PROFILE_INFO_KHR);
        if (i == 0) {
            EXPECT_EQ(structure_types[1], VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_PROFILE_INFO_KHR);
        } else {
            EXPECT_EQ(structure_types[1], VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_PROFILE_INFO_KHR);
        }
    }
}

TEST(mocked_api_generated_library, check_support_variants_video_capability_reflection) {
    const VpProfileProperties profile{VP_RASTERGRID_TEST_VIDEO_PROFILES_NAME, VP_RASTERGRID_TEST_VIDEO_PROFILES_SPEC_VERSION};

    VkResult result = VK_SUCCESS;

    VkVideoDecodeH264CapabilitiesKHR caps_h264{VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_CAPABILITIES_KHR, nullptr};
    VkVideoDecodeH265CapabilitiesKHR caps_h265{VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_CAPABILITIES_KHR, &caps_h264};
    VkVideoCapabilitiesKHR caps{VK_STRUCTURE_TYPE_VIDEO_CAPABILITIES_KHR, &caps_h265};

    auto clear_chain = [&] {
        caps_h264 = VkVideoDecodeH264CapabilitiesKHR{VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_CAPABILITIES_KHR, nullptr};
        caps_h265 = VkVideoDecodeH265CapabilitiesKHR{VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_CAPABILITIES_KHR, &caps_h264};
        caps = VkVideoCapabilitiesKHR{VK_STRUCTURE_TYPE_VIDEO_CAPABILITIES_KHR, &caps_h265};
    };

    // H.264 Decode (4:2:0 8-bit) Main progressive

    auto check_h264_main = [&] {
        EXPECT_EQ(caps.maxCodedExtent.width, 1920);
        EXPECT_EQ(caps.maxCodedExtent.height, 1080);
        EXPECT_EQ(caps.maxDpbSlots, 17);
        EXPECT_EQ(caps.maxActiveReferencePictures, 16);
        EXPECT_EQ(caps_h264.maxLevelIdc, STD_VIDEO_H264_LEVEL_IDC_5_2);
    };

    clear_chain();
    result = vpGetProfileVideoCapabilities(&profile, nullptr, 0, &caps);
    EXPECT_EQ(result, VK_SUCCESS);
    check_h264_main();

    clear_chain();
    result = vpGetProfileVideoCapabilities(&profile, "block", 0, &caps);
    EXPECT_EQ(result, VK_ERROR_UNKNOWN);

    clear_chain();
    result = vpGetProfileVideoCapabilities(&profile, "variant_h264", 0, &caps);
    EXPECT_EQ(result, VK_SUCCESS);
    check_h264_main();

    clear_chain();
    result = vpGetProfileVideoCapabilities(&profile, "variant_h265", 0, &caps);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(caps_h264.maxLevelIdc, 0);

    // H.265 Decode (4:2:0 8-bit) Main

    auto check_h265_main = [&] {
        EXPECT_EQ(caps.maxCodedExtent.width, 3840);
        EXPECT_EQ(caps.maxCodedExtent.height, 2160);
        EXPECT_EQ(caps.maxDpbSlots, 8);
        EXPECT_EQ(caps.maxActiveReferencePictures, 4);
        EXPECT_EQ(caps_h265.maxLevelIdc, STD_VIDEO_H265_LEVEL_IDC_6_0);
    };

    clear_chain();
    result = vpGetProfileVideoCapabilities(&profile, nullptr, 1, &caps);
    EXPECT_EQ(result, VK_SUCCESS);
    check_h265_main();

    clear_chain();
    result = vpGetProfileVideoCapabilities(&profile, "block", 0, &caps);
    EXPECT_EQ(result, VK_ERROR_UNKNOWN);

    clear_chain();
    result = vpGetProfileVideoCapabilities(&profile, "variant_h264", 0, &caps);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(caps_h265.maxLevelIdc, 0);

    clear_chain();
    result = vpGetProfileVideoCapabilities(&profile, "variant_h265", 0, &caps);
    EXPECT_EQ(result, VK_SUCCESS);
    check_h265_main();

    // H.265 Decode (4:2:0 10-bit) Main 10

    auto check_h265_main_10 = [&] {
        EXPECT_EQ(caps.maxCodedExtent.width, 720);
        EXPECT_EQ(caps.maxCodedExtent.height, 480);
        EXPECT_EQ(caps.maxDpbSlots, 2);
        EXPECT_EQ(caps.maxActiveReferencePictures, 1);
        EXPECT_EQ(caps_h265.maxLevelIdc, STD_VIDEO_H265_LEVEL_IDC_5_0);
    };

    clear_chain();
    result = vpGetProfileVideoCapabilities(&profile, nullptr, 2, &caps);
    EXPECT_EQ(result, VK_SUCCESS);
    check_h265_main_10();

    clear_chain();
    result = vpGetProfileVideoCapabilities(&profile, "block", 2, &caps);
    EXPECT_EQ(result, VK_ERROR_UNKNOWN);

    clear_chain();
    result = vpGetProfileVideoCapabilities(&profile, "variant_h264", 1, &caps);
    EXPECT_EQ(result, VK_ERROR_UNKNOWN);

    clear_chain();
    result = vpGetProfileVideoCapabilities(&profile, "variant_h265", 1, &caps);
    EXPECT_EQ(result, VK_SUCCESS);
    check_h265_main_10();

    // Structure types

    uint32_t structure_type_count = 0;
    std::vector<VkStructureType> structure_types{};

    for (uint32_t i = 0; i < 3; ++i) {
        result = vpGetProfileVideoCapabilityStructureTypes(&profile, nullptr, i, &structure_type_count, nullptr);
        EXPECT_EQ(result, VK_SUCCESS);
        EXPECT_EQ(structure_type_count, 2);

        structure_types.clear();
        structure_types.resize(structure_type_count);
        result = vpGetProfileVideoCapabilityStructureTypes(&profile, nullptr, i, &structure_type_count, structure_types.data());
        EXPECT_EQ(result, VK_SUCCESS);
        EXPECT_EQ(structure_type_count, 2);

        EXPECT_EQ(structure_types[0], VK_STRUCTURE_TYPE_VIDEO_CAPABILITIES_KHR);
        if (i == 0) {
            EXPECT_EQ(structure_types[1], VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_CAPABILITIES_KHR);
        } else {
            EXPECT_EQ(structure_types[1], VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_CAPABILITIES_KHR);
        }
    }
}

TEST(mocked_api_generated_library, check_support_variants_video_format_reflection) {
    const VpProfileProperties profile{VP_RASTERGRID_TEST_VIDEO_PROFILES_NAME, VP_RASTERGRID_TEST_VIDEO_PROFILES_SPEC_VERSION};

    VkResult result = VK_SUCCESS;

    uint32_t video_format_count = 0;
    std::vector<VkVideoFormatPropertiesKHR> props{};

    // H.264 Decode (4:2:0 8-bit) Main progressive

    auto check_h264_main = [&] {
        EXPECT_EQ(video_format_count, 2);

        EXPECT_EQ(props[0].format, VK_FORMAT_G8_B8R8_2PLANE_420_UNORM);
        EXPECT_EQ(props[0].imageCreateFlags, VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT | VK_IMAGE_CREATE_EXTENDED_USAGE_BIT);
        EXPECT_EQ(props[0].imageType, VK_IMAGE_TYPE_2D);
        EXPECT_EQ(props[0].imageUsageFlags, VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR | VK_IMAGE_USAGE_SAMPLED_BIT |
                                                VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);

        EXPECT_EQ(props[1].format, VK_FORMAT_G8_B8R8_2PLANE_420_UNORM);
        EXPECT_EQ(props[1].imageCreateFlags, 0);
        EXPECT_EQ(props[1].imageType, VK_IMAGE_TYPE_2D);
        EXPECT_EQ(props[1].imageUsageFlags, VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR);
    };

    result = vpGetProfileVideoFormatProperties(&profile, nullptr, 0, &video_format_count, nullptr);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(video_format_count, 2);

    props.clear();
    props.resize(video_format_count, {VK_STRUCTURE_TYPE_VIDEO_FORMAT_PROPERTIES_KHR, nullptr});
    result = vpGetProfileVideoFormatProperties(&profile, nullptr, 0, &video_format_count, props.data());
    EXPECT_EQ(result, VK_SUCCESS);
    check_h264_main();

    props.clear();
    props.resize(video_format_count, {VK_STRUCTURE_TYPE_VIDEO_FORMAT_PROPERTIES_KHR, nullptr});
    result = vpGetProfileVideoFormatProperties(&profile, "block", 0, &video_format_count, props.data());
    EXPECT_EQ(result, VK_ERROR_UNKNOWN);

    result = vpGetProfileVideoFormatProperties(&profile, "variant_h264", 0, &video_format_count, nullptr);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(video_format_count, 2);

    props.clear();
    props.resize(video_format_count, {VK_STRUCTURE_TYPE_VIDEO_FORMAT_PROPERTIES_KHR, nullptr});
    result = vpGetProfileVideoFormatProperties(&profile, "variant_h264", 0, &video_format_count, props.data());
    EXPECT_EQ(result, VK_SUCCESS);
    check_h264_main();

    // H.265 Decode (4:2:0 8-bit) Main

    auto check_h265_main = [&] {
        EXPECT_EQ(video_format_count, 2);

        EXPECT_EQ(props[0].format, VK_FORMAT_G8_B8R8_2PLANE_420_UNORM);
        EXPECT_EQ(props[0].imageCreateFlags, VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT | VK_IMAGE_CREATE_EXTENDED_USAGE_BIT);
        EXPECT_EQ(props[0].imageType, VK_IMAGE_TYPE_2D);
        EXPECT_EQ(props[0].imageUsageFlags,
                  VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);

        EXPECT_EQ(props[1].format, VK_FORMAT_G8_B8R8_2PLANE_420_UNORM);
        EXPECT_EQ(props[1].imageCreateFlags, 0);
        EXPECT_EQ(props[1].imageType, VK_IMAGE_TYPE_2D);
        EXPECT_EQ(props[1].imageUsageFlags, VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
    };

    result = vpGetProfileVideoFormatProperties(&profile, nullptr, 1, &video_format_count, nullptr);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(video_format_count, 2);

    props.clear();
    props.resize(video_format_count, {VK_STRUCTURE_TYPE_VIDEO_FORMAT_PROPERTIES_KHR, nullptr});
    result = vpGetProfileVideoFormatProperties(&profile, nullptr, 1, &video_format_count, props.data());
    EXPECT_EQ(result, VK_SUCCESS);
    check_h265_main();

    props.clear();
    props.resize(video_format_count, {VK_STRUCTURE_TYPE_VIDEO_FORMAT_PROPERTIES_KHR, nullptr});
    result = vpGetProfileVideoFormatProperties(&profile, "block", 1, &video_format_count, props.data());
    EXPECT_EQ(result, VK_ERROR_UNKNOWN);

    result = vpGetProfileVideoFormatProperties(&profile, "variant_h265", 0, &video_format_count, nullptr);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(video_format_count, 2);

    props.clear();
    props.resize(video_format_count, {VK_STRUCTURE_TYPE_VIDEO_FORMAT_PROPERTIES_KHR, nullptr});
    result = vpGetProfileVideoFormatProperties(&profile, "variant_h265", 0, &video_format_count, props.data());
    EXPECT_EQ(result, VK_SUCCESS);
    check_h265_main();

    // H.265 Decode (4:2:0 10-bit) Main 10

    auto check_h265_main_10 = [&] {
        EXPECT_EQ(video_format_count, 1);

        EXPECT_EQ(props[0].format, VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16);
        EXPECT_EQ(props[0].imageCreateFlags, VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT | VK_IMAGE_CREATE_EXTENDED_USAGE_BIT);
        EXPECT_EQ(props[0].imageType, VK_IMAGE_TYPE_2D);
        EXPECT_EQ(props[0].imageUsageFlags, VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR | VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR |
                                                VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
    };

    result = vpGetProfileVideoFormatProperties(&profile, nullptr, 2, &video_format_count, nullptr);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(video_format_count, 1);

    props.clear();
    props.resize(video_format_count, {VK_STRUCTURE_TYPE_VIDEO_FORMAT_PROPERTIES_KHR, nullptr});
    result = vpGetProfileVideoFormatProperties(&profile, nullptr, 2, &video_format_count, props.data());
    EXPECT_EQ(result, VK_SUCCESS);
    check_h265_main_10();

    props.clear();
    props.resize(video_format_count, {VK_STRUCTURE_TYPE_VIDEO_FORMAT_PROPERTIES_KHR, nullptr});
    result = vpGetProfileVideoFormatProperties(&profile, "block", 2, &video_format_count, props.data());
    EXPECT_EQ(result, VK_ERROR_UNKNOWN);

    result = vpGetProfileVideoFormatProperties(&profile, "variant_h265", 0, &video_format_count, nullptr);
    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(video_format_count, 2);

    props.clear();
    props.resize(video_format_count, {VK_STRUCTURE_TYPE_VIDEO_FORMAT_PROPERTIES_KHR, nullptr});
    result = vpGetProfileVideoFormatProperties(&profile, "variant_h265", 1, &video_format_count, props.data());
    EXPECT_EQ(result, VK_SUCCESS);
    check_h265_main_10();

    // Incomplete

    video_format_count = 1;
    props.clear();
    props.resize(video_format_count, {VK_STRUCTURE_TYPE_VIDEO_FORMAT_PROPERTIES_KHR, nullptr});
    result = vpGetProfileVideoFormatProperties(&profile, nullptr, 0, &video_format_count, props.data());
    EXPECT_EQ(result, VK_INCOMPLETE);

    EXPECT_EQ(props[0].format, VK_FORMAT_G8_B8R8_2PLANE_420_UNORM);
    EXPECT_EQ(props[0].imageCreateFlags, VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT | VK_IMAGE_CREATE_EXTENDED_USAGE_BIT);
    EXPECT_EQ(props[0].imageType, VK_IMAGE_TYPE_2D);
    EXPECT_EQ(props[0].imageUsageFlags, VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR | VK_IMAGE_USAGE_SAMPLED_BIT |
                                            VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
}

TEST(mocked_api_generated_library, check_support_variants_video_profile_success_2variants) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_RASTERGRID_TEST_VIDEO_PROFILES_NAME, VP_RASTERGRID_TEST_VIDEO_PROFILES_SPEC_VERSION};

    initProfile(mock, profile);

    VkBool32 supported = VK_FALSE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_TRUE);

    std::vector<VpBlockProperties> block_properties(10);
    uint32_t block_property_count = static_cast<uint32_t>(block_properties.size());

    supported = VK_FALSE;
    result = vpGetPhysicalDeviceProfileVariantsSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported,
                                                       &block_property_count, &block_properties[0]);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_TRUE);

    EXPECT_EQ(block_property_count, 2);
    EXPECT_STREQ(block_properties[0].blockName, "block");
    EXPECT_STREQ(block_properties[1].blockName, "variant_h264");
}

TEST(mocked_api_generated_library, check_support_variants_video_profile_success_1variant) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_RASTERGRID_TEST_VIDEO_PROFILES_NAME, VP_RASTERGRID_TEST_VIDEO_PROFILES_SPEC_VERSION};

    VkVideoDecodeH264ProfileInfoKHR profile_info_h264{VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_PROFILE_INFO_KHR, nullptr};
    profile_info_h264.stdProfileIdc = STD_VIDEO_H264_PROFILE_IDC_MAIN;
    profile_info_h264.pictureLayout = VK_VIDEO_DECODE_H264_PICTURE_LAYOUT_PROGRESSIVE_KHR;
    VkVideoProfileInfoKHR profile_info{VK_STRUCTURE_TYPE_VIDEO_PROFILE_INFO_KHR, &profile_info_h264};
    profile_info.videoCodecOperation = VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_KHR;
    profile_info.chromaSubsampling = VK_VIDEO_CHROMA_SUBSAMPLING_420_BIT_KHR;
    profile_info.lumaBitDepth = VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR;
    profile_info.chromaBitDepth = VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR;

    // Missing capability
    {
        initProfile(mock, profile);
        mock.RemoveVideoCapabilities(profile_info);

        VkBool32 supported = VK_FALSE;
        VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

        EXPECT_EQ(result, VK_SUCCESS);
        EXPECT_EQ(supported, VK_TRUE);

        std::vector<VpBlockProperties> block_properties(10);
        uint32_t block_property_count = static_cast<uint32_t>(block_properties.size());

        supported = VK_FALSE;
        result = vpGetPhysicalDeviceProfileVariantsSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported,
                                                           &block_property_count, &block_properties[0]);

        EXPECT_EQ(result, VK_SUCCESS);
        EXPECT_EQ(supported, VK_TRUE);

        EXPECT_EQ(block_property_count, 2);
        EXPECT_STREQ(block_properties[0].blockName, "block");
        EXPECT_STREQ(block_properties[1].blockName, "variant_h265");
    }

    // Missing format
    {
        mock.ClearProfileAreas(PROFILE_AREA_ALL_BITS);
        initProfile(mock, profile);
        mock.RemoveVideoFormats(profile_info);

        VkBool32 supported = VK_FALSE;
        VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

        EXPECT_EQ(result, VK_SUCCESS);
        EXPECT_EQ(supported, VK_TRUE);

        std::vector<VpBlockProperties> block_properties(10);
        uint32_t block_property_count = static_cast<uint32_t>(block_properties.size());

        supported = VK_FALSE;
        result = vpGetPhysicalDeviceProfileVariantsSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported,
                                                           &block_property_count, &block_properties[0]);

        EXPECT_EQ(result, VK_SUCCESS);
        EXPECT_EQ(supported, VK_TRUE);

        EXPECT_EQ(block_property_count, 2);
        EXPECT_STREQ(block_properties[0].blockName, "block");
        EXPECT_STREQ(block_properties[1].blockName, "variant_h265");
    }
}

TEST(mocked_api_generated_library, check_support_variants_video_profile_fail) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_RASTERGRID_TEST_VIDEO_PROFILES_NAME, VP_RASTERGRID_TEST_VIDEO_PROFILES_SPEC_VERSION};

    // Missing capability
    {
        initProfile(mock, profile);
        mock.ClearProfileAreas(PROFILE_AREA_VIDEO_CAPABILITIES_BIT);

        VkBool32 supported = VK_FALSE;
        VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

        EXPECT_EQ(result, VK_SUCCESS);
        EXPECT_EQ(supported, VK_FALSE);

        std::vector<VpBlockProperties> block_properties(10);
        uint32_t block_property_count = static_cast<uint32_t>(block_properties.size());

        supported = VK_FALSE;
        result = vpGetPhysicalDeviceProfileVariantsSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported,
                                                           &block_property_count, &block_properties[0]);

        EXPECT_EQ(result, VK_SUCCESS);
        EXPECT_EQ(supported, VK_FALSE);

        EXPECT_EQ(block_property_count, 2);
        EXPECT_STREQ(block_properties[0].blockName, "variant_h264");
        EXPECT_STREQ(block_properties[1].blockName, "variant_h265");
    }

    // Missing format
    {
        mock.ClearProfileAreas(PROFILE_AREA_ALL_BITS);
        initProfile(mock, profile);
        mock.ClearProfileAreas(PROFILE_AREA_VIDEO_FORMATS_BIT);

        VkBool32 supported = VK_FALSE;
        VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

        EXPECT_EQ(result, VK_SUCCESS);
        EXPECT_EQ(supported, VK_FALSE);

        std::vector<VpBlockProperties> block_properties(10);
        uint32_t block_property_count = static_cast<uint32_t>(block_properties.size());

        supported = VK_FALSE;
        result = vpGetPhysicalDeviceProfileVariantsSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported,
                                                           &block_property_count, &block_properties[0]);

        EXPECT_EQ(result, VK_SUCCESS);
        EXPECT_EQ(supported, VK_FALSE);

        EXPECT_EQ(block_property_count, 2);
        EXPECT_STREQ(block_properties[0].blockName, "variant_h264");
        EXPECT_STREQ(block_properties[1].blockName, "variant_h265");
    }
}

TEST(mocked_api_generated_library, check_support_wildcard_video_profiles) {
    MockVulkanAPI mock;

    const VpProfileProperties profile{VP_RASTERGRID_TEST_WILDCARD_VIDEO_PROFILES_NAME,
                                      VP_RASTERGRID_TEST_WILDCARD_VIDEO_PROFILES_SPEC_VERSION};

    initProfile(mock, profile);
    mock.ClearProfileAreas(PROFILE_AREA_VIDEO_CAPABILITIES_BIT | PROFILE_AREA_VIDEO_FORMATS_BIT);

    VkVideoDecodeH264ProfileInfoKHR h264_decode{VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_PROFILE_INFO_KHR, nullptr};
    h264_decode.stdProfileIdc = STD_VIDEO_H264_PROFILE_IDC_MAIN;
    h264_decode.pictureLayout = VK_VIDEO_DECODE_H264_PICTURE_LAYOUT_PROGRESSIVE_KHR;

    VkVideoProfileInfoKHR profile_info{VK_STRUCTURE_TYPE_VIDEO_PROFILE_INFO_KHR, &h264_decode};
    profile_info.videoCodecOperation = VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_KHR;
    profile_info.chromaSubsampling = VK_VIDEO_CHROMA_SUBSAMPLING_420_BIT_KHR;
    profile_info.lumaBitDepth = VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR;
    profile_info.chromaBitDepth = VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR;

    VulkanVideoProfile h264_decode_profile(&profile_info);

    // No video profiles at all
    {
        VkBool32 supported = VK_FALSE;
        VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

        EXPECT_EQ(result, VK_SUCCESS);
        EXPECT_EQ(supported, VK_FALSE);

        std::vector<VpBlockProperties> block_properties(10);
        uint32_t block_property_count = static_cast<uint32_t>(block_properties.size());

        supported = VK_FALSE;
        result = vpGetPhysicalDeviceProfileVariantsSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported,
                                                           &block_property_count, &block_properties[0]);

        EXPECT_EQ(result, VK_SUCCESS);
        EXPECT_EQ(supported, VK_FALSE);

        EXPECT_EQ(block_property_count, 3);
        EXPECT_STREQ(block_properties[0].blockName, "general");
        EXPECT_STREQ(block_properties[1].blockName, "decode_420_8bit");
        EXPECT_STREQ(block_properties[2].blockName, "decode_h264");
    }

    // Add a 4:2:0 8-bit decode profile but without a sampleable decode output or extended usage support
    {
        VkVideoDecodeH264CapabilitiesKHR h264_decode_caps{VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_CAPABILITIES_KHR, nullptr};
        h264_decode_caps.maxLevelIdc = STD_VIDEO_H264_LEVEL_IDC_6_2;

        VkVideoDecodeCapabilitiesKHR decode_caps{VK_STRUCTURE_TYPE_VIDEO_DECODE_CAPABILITIES_KHR, &h264_decode_caps};
        decode_caps.flags = VK_VIDEO_DECODE_CAPABILITY_DPB_AND_OUTPUT_DISTINCT_BIT_KHR;

        VkVideoCapabilitiesKHR caps{VK_STRUCTURE_TYPE_VIDEO_CAPABILITIES_KHR, &decode_caps};
        caps.maxCodedExtent.width = 3840;
        caps.maxCodedExtent.height = 2160;
        caps.maxDpbSlots = 16;
        caps.maxActiveReferencePictures = 15;

        mock.AddVideoCapabilities(h264_decode_profile, {VK_STRUCT(caps), VK_STRUCT(decode_caps), VK_STRUCT(h264_decode_caps)});

        VkVideoFormatPropertiesKHR dst_format{VK_STRUCTURE_TYPE_VIDEO_FORMAT_PROPERTIES_KHR, nullptr};
        dst_format.format = VK_FORMAT_G8_B8R8_2PLANE_420_UNORM;
        dst_format.imageCreateFlags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
        dst_format.imageType = VK_IMAGE_TYPE_2D;
        dst_format.imageTiling = VK_IMAGE_TILING_LINEAR;
        dst_format.imageUsageFlags = VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR;

        mock.AddVideoFormat(h264_decode_profile, {VK_STRUCT(dst_format)});

        VkVideoFormatPropertiesKHR dpb_format{VK_STRUCTURE_TYPE_VIDEO_FORMAT_PROPERTIES_KHR, nullptr};
        dpb_format.format = VK_FORMAT_G8_B8R8_2PLANE_420_UNORM;
        dpb_format.imageCreateFlags = 0;
        dpb_format.imageType = VK_IMAGE_TYPE_2D;
        dpb_format.imageTiling = VK_IMAGE_TILING_LINEAR;
        dpb_format.imageUsageFlags = VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR;

        mock.AddVideoFormat(h264_decode_profile, {VK_STRUCT(dpb_format)});

        VkBool32 supported = VK_FALSE;
        VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

        EXPECT_EQ(result, VK_SUCCESS);
        EXPECT_EQ(supported, VK_FALSE);

        std::vector<VpBlockProperties> block_properties(10);
        uint32_t block_property_count = static_cast<uint32_t>(block_properties.size());

        supported = VK_FALSE;
        result = vpGetPhysicalDeviceProfileVariantsSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported,
                                                           &block_property_count, &block_properties[0]);

        EXPECT_EQ(result, VK_SUCCESS);
        EXPECT_EQ(supported, VK_FALSE);

        EXPECT_EQ(block_property_count, 2);
        EXPECT_STREQ(block_properties[0].blockName, "decode_420_8bit");
        EXPECT_STREQ(block_properties[1].blockName, "decode_h264");
    }

    // Add another decode output format with the needed attributes, but triplanar, making it fail decode_420_8bit block check
    {
        VkVideoFormatPropertiesKHR dst_format{VK_STRUCTURE_TYPE_VIDEO_FORMAT_PROPERTIES_KHR, nullptr};
        dst_format.format = VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM;
        dst_format.imageCreateFlags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT | VK_IMAGE_CREATE_EXTENDED_USAGE_BIT;
        dst_format.imageType = VK_IMAGE_TYPE_2D;
        dst_format.imageTiling = VK_IMAGE_TILING_OPTIMAL;
        dst_format.imageUsageFlags = VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR | VK_IMAGE_USAGE_SAMPLED_BIT;

        mock.AddVideoFormat(h264_decode_profile, {VK_STRUCT(dst_format)});

        VkBool32 supported = VK_FALSE;
        VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

        EXPECT_EQ(result, VK_SUCCESS);
        EXPECT_EQ(supported, VK_FALSE);

        std::vector<VpBlockProperties> block_properties(10);
        uint32_t block_property_count = static_cast<uint32_t>(block_properties.size());

        supported = VK_FALSE;
        result = vpGetPhysicalDeviceProfileVariantsSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported,
                                                           &block_property_count, &block_properties[0]);

        EXPECT_EQ(result, VK_SUCCESS);
        EXPECT_EQ(supported, VK_FALSE);

        EXPECT_EQ(block_property_count, 1);
        EXPECT_STREQ(block_properties[0].blockName, "decode_420_8bit");
    }

    // Add another decode output format with extended usage support but not sampleable
    {
        VkVideoFormatPropertiesKHR dst_format{VK_STRUCTURE_TYPE_VIDEO_FORMAT_PROPERTIES_KHR, nullptr};
        dst_format.format = VK_FORMAT_G8_B8R8_2PLANE_420_UNORM;
        dst_format.imageCreateFlags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT | VK_IMAGE_CREATE_EXTENDED_USAGE_BIT;
        dst_format.imageType = VK_IMAGE_TYPE_2D;
        dst_format.imageTiling = VK_IMAGE_TILING_OPTIMAL;
        dst_format.imageUsageFlags = VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR;

        mock.AddVideoFormat(h264_decode_profile, {VK_STRUCT(dst_format)});

        VkBool32 supported = VK_FALSE;
        VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

        EXPECT_EQ(result, VK_SUCCESS);
        EXPECT_EQ(supported, VK_TRUE);
    }

    // Change capabilities to lower DPB slots and references, making it fail decode_h264 block check
    {
        mock.RemoveVideoCapabilities(profile_info);

        VkVideoDecodeH264CapabilitiesKHR h264_decode_caps{VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_CAPABILITIES_KHR, nullptr};
        h264_decode_caps.maxLevelIdc = STD_VIDEO_H264_LEVEL_IDC_6_2;

        VkVideoDecodeCapabilitiesKHR decode_caps{VK_STRUCTURE_TYPE_VIDEO_DECODE_CAPABILITIES_KHR, &h264_decode_caps};
        decode_caps.flags = VK_VIDEO_DECODE_CAPABILITY_DPB_AND_OUTPUT_DISTINCT_BIT_KHR;

        VkVideoCapabilitiesKHR caps{VK_STRUCTURE_TYPE_VIDEO_CAPABILITIES_KHR, &decode_caps};
        caps.maxCodedExtent.width = 3840;
        caps.maxCodedExtent.height = 2160;
        caps.maxDpbSlots = 15;
        caps.maxActiveReferencePictures = 14;

        mock.AddVideoCapabilities(h264_decode_profile, {VK_STRUCT(caps), VK_STRUCT(decode_caps), VK_STRUCT(h264_decode_caps)});

        VkBool32 supported = VK_FALSE;
        VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

        EXPECT_EQ(result, VK_SUCCESS);
        EXPECT_EQ(supported, VK_FALSE);

        std::vector<VpBlockProperties> block_properties(10);
        uint32_t block_property_count = static_cast<uint32_t>(block_properties.size());

        supported = VK_FALSE;
        result = vpGetPhysicalDeviceProfileVariantsSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported,
                                                           &block_property_count, &block_properties[0]);

        EXPECT_EQ(result, VK_SUCCESS);
        EXPECT_EQ(supported, VK_FALSE);

        EXPECT_EQ(block_property_count, 1);
        EXPECT_STREQ(block_properties[0].blockName, "decode_h264");
    }

    // Finally, change the capabilities to fail the general block
    {
        mock.RemoveVideoCapabilities(profile_info);

        VkVideoDecodeH264CapabilitiesKHR h264_decode_caps{VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_CAPABILITIES_KHR, nullptr};
        h264_decode_caps.maxLevelIdc = STD_VIDEO_H264_LEVEL_IDC_6_2;

        VkVideoDecodeCapabilitiesKHR decode_caps{VK_STRUCTURE_TYPE_VIDEO_DECODE_CAPABILITIES_KHR, &h264_decode_caps};
        decode_caps.flags = VK_VIDEO_DECODE_CAPABILITY_DPB_AND_OUTPUT_DISTINCT_BIT_KHR;

        VkVideoCapabilitiesKHR caps{VK_STRUCTURE_TYPE_VIDEO_CAPABILITIES_KHR, &decode_caps};
        caps.maxCodedExtent.width = 2048;
        caps.maxCodedExtent.height = 1024;
        caps.maxDpbSlots = 16;
        caps.maxActiveReferencePictures = 15;

        mock.AddVideoCapabilities(h264_decode_profile, {VK_STRUCT(caps), VK_STRUCT(decode_caps), VK_STRUCT(h264_decode_caps)});

        VkBool32 supported = VK_FALSE;
        VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

        EXPECT_EQ(result, VK_SUCCESS);
        EXPECT_EQ(supported, VK_FALSE);

        std::vector<VpBlockProperties> block_properties(10);
        uint32_t block_property_count = static_cast<uint32_t>(block_properties.size());

        supported = VK_FALSE;
        result = vpGetPhysicalDeviceProfileVariantsSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported,
                                                           &block_property_count, &block_properties[0]);

        EXPECT_EQ(result, VK_SUCCESS);
        EXPECT_EQ(supported, VK_FALSE);

        EXPECT_EQ(block_property_count, 1);
        EXPECT_STREQ(block_properties[0].blockName, "general");
    }
}
