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

TEST(api_get_profiles_beta, full) {
    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfiles(&propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(3, propertyCount);

    propertyCount = 4;

    std::vector<VpProfileProperties> properties(propertyCount);
    VkResult result1 = vpGetProfiles(&propertyCount, &properties[0]);
    EXPECT_EQ(VK_SUCCESS, result1);
    EXPECT_EQ(3, propertyCount);

    EXPECT_STREQ(VP_KHR_ROADMAP_2022_NAME, properties[0].profileName);
    EXPECT_EQ(VP_KHR_ROADMAP_2022_SPEC_VERSION, properties[0].specVersion);

    EXPECT_STREQ(VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, properties[1].profileName);
    EXPECT_EQ(VP_LUNARG_DESKTOP_PORTABILITY_2021_SPEC_VERSION, properties[1].specVersion);

    EXPECT_STREQ(VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME, properties[2].profileName);
    EXPECT_EQ(VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_SPEC_VERSION, properties[2].specVersion);
}

TEST(api_get_profiles_beta, partial) {
    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfiles(&propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(3, propertyCount);

    propertyCount = 1;

    std::vector<VpProfileProperties> properties(propertyCount);
    properties[0].specVersion = 0;
    VkResult result1 = vpGetProfiles(&propertyCount, &properties[0]);
    EXPECT_EQ(VK_INCOMPLETE, result1);
    EXPECT_EQ(1, propertyCount);

    EXPECT_STREQ(VP_KHR_ROADMAP_2022_NAME, properties[0].profileName);
    EXPECT_EQ(VP_KHR_ROADMAP_2022_SPEC_VERSION, properties[0].specVersion);
}

TEST(api_get_profile_device_extension_properties, full) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, 1};

    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfileDeviceExtensionProperties(&profile, &propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(19, propertyCount);

    propertyCount = 23;

    std::vector<VkExtensionProperties> properties(propertyCount);
    VkResult result1 = vpGetProfileDeviceExtensionProperties(&profile, &propertyCount, &properties[0]);
    EXPECT_EQ(VK_SUCCESS, result1);
    EXPECT_EQ(19, propertyCount);

    EXPECT_STREQ(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME, properties[0].extensionName);
    EXPECT_STREQ(VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME, properties[1].extensionName);
    EXPECT_STREQ(VK_EXT_ROBUSTNESS_2_EXTENSION_NAME, properties[2].extensionName);
    EXPECT_STREQ(VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME, properties[3].extensionName);
}

TEST(api_get_profile_device_extension_properties, partial) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, 1};

    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfileDeviceExtensionProperties(&profile, &propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(19, propertyCount);

    propertyCount = 5;

    std::vector<VkExtensionProperties> properties(propertyCount);
    VkResult result1 = vpGetProfileDeviceExtensionProperties(&profile, &propertyCount, &properties[0]);
    EXPECT_EQ(VK_INCOMPLETE, result1);
    EXPECT_EQ(5, propertyCount);

    EXPECT_STREQ(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME, properties[0].extensionName);
    EXPECT_STREQ(VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME, properties[1].extensionName);
    EXPECT_STREQ(VK_EXT_ROBUSTNESS_2_EXTENSION_NAME, properties[2].extensionName);
    EXPECT_STREQ(VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME, properties[3].extensionName);
}

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

TEST(api_get_profile_fallbacks, full) {
    const VpProfileProperties profile_subset = {VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME, 1};
    const VpProfileProperties profile_expect = {VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, 1};

    uint32_t count = 0;
    VkResult result0 = vpGetProfileFallbacks(&profile_subset, &count, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(1, count);

    count = 2;

    std::vector<VpProfileProperties> data(count);
    VkResult result1 = vpGetProfileFallbacks(&profile_subset, &count, &data[0]);
    EXPECT_EQ(VK_SUCCESS, result1);
    EXPECT_EQ(1, count);

    EXPECT_EQ(profile_expect.specVersion, data[0].specVersion);
    EXPECT_STREQ(profile_expect.profileName, data[0].profileName);
}

TEST(api_get_profile_fallbacks, partial) {
    const VpProfileProperties profile_subset = {VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME, 1};

    uint32_t count = 0;
    VkResult result0 = vpGetProfileFallbacks(&profile_subset, &count, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(1, count);

    std::vector<VpProfileProperties> data(count);

    count = 0;

    VkResult result1 = vpGetProfileFallbacks(&profile_subset, &count, &data[0]);
    EXPECT_EQ(VK_INCOMPLETE, result1);
    EXPECT_EQ(0, count);

    EXPECT_EQ(0, data[0].specVersion);
    EXPECT_STREQ("", data[0].profileName);
}


TEST(api_get_profile_formats, full) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, 1};

    uint32_t formatCount = 0;
    VkResult result0 = vpGetProfileFormats(&profile, &formatCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(64, formatCount);

    formatCount = 67;

    std::vector<VkFormat> formats(formatCount);
    VkResult result1 = vpGetProfileFormats(&profile, &formatCount, &formats[0]);
    EXPECT_EQ(VK_SUCCESS, result1);
    EXPECT_EQ(64, formatCount);

    EXPECT_EQ(VK_FORMAT_A2B10G10R10_UINT_PACK32, formats[0]);
    EXPECT_EQ(VK_FORMAT_A2B10G10R10_UNORM_PACK32, formats[1]);
    EXPECT_EQ(VK_FORMAT_A8B8G8R8_SINT_PACK32, formats[2]);
}

TEST(api_get_profile_formats, partial) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, 1};

    uint32_t formatCount = 0;
    VkResult result0 = vpGetProfileFormats(&profile, &formatCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(64, formatCount);

    formatCount = 3;

    std::vector<VkFormat> formats(formatCount);
    VkResult result1 = vpGetProfileFormats(&profile, &formatCount, &formats[0]);
    EXPECT_EQ(VK_INCOMPLETE, result1);
    EXPECT_EQ(3, formatCount);

    EXPECT_EQ(VK_FORMAT_A2B10G10R10_UINT_PACK32, formats[0]);
    EXPECT_EQ(VK_FORMAT_A2B10G10R10_UNORM_PACK32, formats[1]);
    EXPECT_EQ(VK_FORMAT_A8B8G8R8_SINT_PACK32, formats[2]);
}

TEST(api_get_profile_formats, unspecified) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, 1};

    uint32_t formatCount = 0;
    VkResult result0 = vpGetProfileFormats(&profile, &formatCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(0, formatCount);
}

TEST(api_get_profile_formats, properties_single) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, 1};

#if defined(VK_VERSION_1_1) || defined(VK_KHR_get_physical_device_properties2)
    VkFormatProperties2KHR properties2 = {};
    properties2.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR;
    vpGetProfileFormatProperties(&profile, VK_FORMAT_D16_UNORM, &properties2);
    EXPECT_EQ(0, properties2.formatProperties.bufferFeatures);
    EXPECT_EQ(VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
                  VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                  VK_FORMAT_FEATURE_TRANSFER_DST_BIT,
              properties2.formatProperties.optimalTilingFeatures);
    EXPECT_EQ(0, properties2.formatProperties.linearTilingFeatures);
#endif

#if defined(VK_VERSION_1_3) || defined(VK_KHR_format_feature_flags2)
    VkFormatProperties3KHR properties3 = {};
    properties3.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3_KHR;
    vpGetProfileFormatProperties(&profile, VK_FORMAT_D16_UNORM, &properties3);
    EXPECT_EQ(0, properties3.bufferFeatures);
    EXPECT_EQ(VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
                  VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                  VK_FORMAT_FEATURE_TRANSFER_DST_BIT,
              properties3.optimalTilingFeatures);
    EXPECT_EQ(0, properties3.linearTilingFeatures);
#endif
}

TEST(api_get_profile_formats, properties_chained) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, 1};

#if defined(VK_VERSION_1_3) || defined(VK_KHR_format_feature_flags2)
    VkFormatProperties2KHR properties2 = {};
    properties2.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR;
    properties2.pNext = nullptr;

    VkFormatProperties3KHR properties3 = {};
    properties3.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3_KHR;
    properties3.pNext = &properties2;

    vpGetProfileFormatProperties(&profile, VK_FORMAT_D16_UNORM, &properties3);

    EXPECT_EQ(0, properties2.formatProperties.bufferFeatures);
    EXPECT_EQ(VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
                  VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                  VK_FORMAT_FEATURE_TRANSFER_DST_BIT,
              properties2.formatProperties.optimalTilingFeatures);
    EXPECT_EQ(0, properties2.formatProperties.linearTilingFeatures);

    EXPECT_EQ(0, properties3.bufferFeatures);
    EXPECT_EQ(VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
                  VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                  VK_FORMAT_FEATURE_TRANSFER_DST_BIT,
              properties3.optimalTilingFeatures);
    EXPECT_EQ(0, properties3.linearTilingFeatures);
#endif
}

TEST(api_get_profile_queue_families, full) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, 1};

    uint32_t count = 0;
    VkResult result0 = vpGetProfileQueueFamilyProperties(&profile, &count, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(1, count);

#if defined(VK_VERSION_1_1) || defined(VK_KHR_get_physical_device_properties2)
    std::vector<VkQueueFamilyProperties2KHR> data(count, {VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2_KHR});
    VkResult result1 = vpGetProfileQueueFamilyProperties(&profile, &count, &data[0]);
    EXPECT_EQ(VK_SUCCESS, result1);
    EXPECT_EQ(1, count);

    EXPECT_EQ(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT,
              data[0].queueFamilyProperties.queueFlags);
    EXPECT_EQ(1, data[0].queueFamilyProperties.queueCount);
    EXPECT_EQ(36, data[0].queueFamilyProperties.timestampValidBits);
    EXPECT_EQ(1, data[0].queueFamilyProperties.minImageTransferGranularity.width);
    EXPECT_EQ(1, data[0].queueFamilyProperties.minImageTransferGranularity.height);
    EXPECT_EQ(1, data[0].queueFamilyProperties.minImageTransferGranularity.depth);
#endif  //
}

TEST(api_get_profile_queue_families, unspecified) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, 1};

    uint32_t count = 0;
    VkResult result0 = vpGetProfileQueueFamilyProperties(&profile, &count, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(0, count);
}

TEST(api_get_profile_queue_families, list) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, 1};

    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfileQueueFamilyStructureTypes(&profile, &propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(1, propertyCount);

    std::vector<VkStructureType> properties(propertyCount);

    propertyCount = 0;
    VkResult result1 = vpGetProfileQueueFamilyStructureTypes(&profile, &propertyCount, &properties[0]);
    EXPECT_EQ(VK_INCOMPLETE, result1);
    EXPECT_EQ(0, propertyCount);
}


TEST(api_get_profile_properties, get_properties2) {
    VkPhysicalDeviceProperties2 profileProperties2{};
    profileProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    profileProperties2.pNext = nullptr;

    const VpProfileProperties Profile = {VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, 1};

    vpGetProfileProperties(&Profile, &profileProperties2);

    EXPECT_EQ(16384, profileProperties2.properties.limits.maxImageDimension1D);
    EXPECT_EQ(16384, profileProperties2.properties.limits.maxImageDimension2D);
    EXPECT_EQ(2048, profileProperties2.properties.limits.maxImageDimension3D);
    EXPECT_EQ(16384, profileProperties2.properties.limits.maxImageDimensionCube);
    EXPECT_EQ(2048, profileProperties2.properties.limits.maxImageArrayLayers);
    EXPECT_EQ(8, profileProperties2.properties.limits.maxColorAttachments);
}

TEST(api_get_profile_structures, get_properties_chain) {
    VkPhysicalDeviceDescriptorIndexingProperties properties0{};
    properties0.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES;
    properties0.pNext = nullptr;

    VkPhysicalDeviceMultiviewPropertiesKHR properties1{};
    properties1.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR;
    properties1.pNext = &properties0;

    VkPhysicalDeviceProperties2 properties2{};
    properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    properties2.pNext = &properties1;

    const VpProfileProperties Profile = {VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, 1};

    vpGetProfileProperties(&Profile, &properties2);

    EXPECT_EQ(1048576, properties0.maxUpdateAfterBindDescriptorsInAllPools);
    EXPECT_EQ(16, properties0.maxPerStageDescriptorUpdateAfterBindSamplers);
    EXPECT_EQ(134217727, properties1.maxMultiviewInstanceIndex);
    EXPECT_EQ(6, properties1.maxMultiviewViewCount);
}

TEST(api_get_profile_structures, get_features) {
    VkPhysicalDeviceVulkan12Features deviceVulkan12Features = {};
    deviceVulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    deviceVulkan12Features.pNext = nullptr;

    VkPhysicalDeviceFeatures2 deviceFeatures2 = {};
    deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    deviceFeatures2.pNext = &deviceVulkan12Features;

    const VpProfileProperties Profile = {VP_KHR_ROADMAP_2022_NAME, 1};

    vpGetProfileFeatures(&Profile, &deviceFeatures2);

    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.samplerMirrorClampToEdge);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.drawIndirectCount);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.storageBuffer8BitAccess);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.uniformAndStorageBuffer8BitAccess);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.storagePushConstant8);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.shaderBufferInt64Atomics);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.shaderSharedInt64Atomics);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.shaderFloat16);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.shaderInt8);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.descriptorIndexing);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.shaderUniformTexelBufferArrayDynamicIndexing);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.uniformBufferStandardLayout);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.shaderSubgroupExtendedTypes);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.separateDepthStencilLayouts);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.hostQueryReset);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.bufferDeviceAddress);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.vulkanMemoryModel);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.vulkanMemoryModelDeviceScope);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.vulkanMemoryModelAvailabilityVisibilityChains);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.subgroupBroadcastDynamicId);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.imagelessFramebuffer);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.shaderOutputLayer);
}

TEST(api_get_profile_feature_structure_types, properties_full) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, 1};

    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfileFeatureStructureTypes(&profile, &propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(4, propertyCount);

    propertyCount = 5;

    std::vector<VkStructureType> properties(propertyCount);
    VkResult result1 = vpGetProfileFeatureStructureTypes(&profile, &propertyCount, &properties[0]);
    EXPECT_EQ(VK_SUCCESS, result1);
    EXPECT_EQ(4, propertyCount);

    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, properties[0]);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, properties[1]);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, properties[2]);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES, properties[3]);
}

TEST(api_get_profile_feature_structure_types, properties_partial) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, 1};

    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfileFeatureStructureTypes(&profile, &propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(4, propertyCount);

    propertyCount = 3;

    std::vector<VkStructureType> properties(propertyCount);
    VkResult result1 = vpGetProfileFeatureStructureTypes(&profile, &propertyCount, &properties[0]);
    EXPECT_EQ(VK_INCOMPLETE, result1);
    EXPECT_EQ(3, propertyCount);

    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, properties[0]);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, properties[1]);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, properties[2]);
}

TEST(api_get_profile_property_structure_types, properties_full) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, 1};

    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfilePropertyStructureTypes(&profile, &propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(4, propertyCount);

    propertyCount = 5;

    std::vector<VkStructureType> properties(propertyCount);
    VkResult result1 = vpGetProfilePropertyStructureTypes(&profile, &propertyCount, &properties[0]);
    EXPECT_EQ(VK_SUCCESS, result1);
    EXPECT_EQ(4, propertyCount);

    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, properties[0]);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES, properties[1]);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES, properties[2]);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES, properties[3]);
}

TEST(api_get_profile_property_structure_types, properties_partial) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, 1};

    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfilePropertyStructureTypes(&profile, &propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(4, propertyCount);

    propertyCount = 3;

    std::vector<VkStructureType> properties(propertyCount);
    VkResult result1 = vpGetProfilePropertyStructureTypes(&profile, &propertyCount, &properties[0]);
    EXPECT_EQ(VK_INCOMPLETE, result1);
    EXPECT_EQ(3, propertyCount);

    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, properties[0]);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES, properties[1]);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES, properties[2]);
}
