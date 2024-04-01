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
    EXPECT_TRUE(propertyCount > 1);

    std::vector<VpProfileProperties> properties(propertyCount);
    VkResult result1 = vpGetProfiles(&propertyCount, &properties[0]);
    EXPECT_EQ(VK_SUCCESS, result1);
    EXPECT_TRUE(propertyCount > 1);

    EXPECT_STREQ(VP_KHR_ROADMAP_2022_NAME, properties[0].profileName);
    EXPECT_EQ(VP_KHR_ROADMAP_2022_SPEC_VERSION, properties[0].specVersion);
}

TEST(api_get_profiles_beta, partial) {
    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfiles(&propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_TRUE(propertyCount > 1);

    propertyCount = 1;

    std::vector<VpProfileProperties> properties(propertyCount);
    properties[0].specVersion = 0;
    VkResult result1 = vpGetProfiles(&propertyCount, &properties[0]);
    EXPECT_EQ(VK_INCOMPLETE, result1);
    EXPECT_EQ(propertyCount, 1);

    EXPECT_STREQ(VP_KHR_ROADMAP_2022_NAME, properties[0].profileName);
    EXPECT_EQ(VP_KHR_ROADMAP_2022_SPEC_VERSION, properties[0].specVersion);
}

TEST(api_get_profile_required_profiles, full) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2024_NAME, VP_KHR_ROADMAP_2024_SPEC_VERSION};

    uint32_t count = 0;
    VkResult result0 = vpGetProfileRequiredProfiles(&profile, &count, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(1, count);

    std::vector<VpProfileProperties> data(count);
    VkResult result1 = vpGetProfileRequiredProfiles(&profile, &count, &data[0]);
    EXPECT_STREQ(VP_KHR_ROADMAP_2022_NAME, data[0].profileName);
    EXPECT_EQ(VK_SUCCESS, result1);
    EXPECT_EQ(1, count);
}

TEST(api_get_profile_api_version, get) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION};

    uint32_t version = vpGetProfileAPIVersion(&profile);

    EXPECT_EQ(VK_MAKE_API_VERSION(0, 1, 3, 204), version);
}

TEST(api_get_profile_device_extension_properties, full) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION};

    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfileDeviceExtensionProperties(&profile, nullptr, &propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(1, propertyCount);

    propertyCount = 2;

    std::vector<VkExtensionProperties> properties(propertyCount);
    VkResult result1 = vpGetProfileDeviceExtensionProperties(&profile, nullptr, &propertyCount, &properties[0]);
    EXPECT_EQ(VK_SUCCESS, result1);
    EXPECT_EQ(1, propertyCount);

    EXPECT_STREQ(VK_KHR_GLOBAL_PRIORITY_EXTENSION_NAME, properties[0].extensionName);
}

TEST(api_get_profile_device_extension_properties, partial) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2024_NAME, VP_KHR_ROADMAP_2024_SPEC_VERSION};

    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfileDeviceExtensionProperties(&profile, nullptr, &propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(15, propertyCount);

    propertyCount = 5;

    std::vector<VkExtensionProperties> properties(propertyCount);
    VkResult result1 = vpGetProfileDeviceExtensionProperties(&profile, nullptr, &propertyCount, &properties[0]);
    EXPECT_EQ(VK_INCOMPLETE, result1);
    EXPECT_EQ(5, propertyCount);

    EXPECT_STREQ(VK_KHR_GLOBAL_PRIORITY_EXTENSION_NAME, properties[0].extensionName);
    EXPECT_STREQ(VK_KHR_DYNAMIC_RENDERING_LOCAL_READ_EXTENSION_NAME, properties[1].extensionName);
}

TEST(api_get_profile_instance_extension_properties, full) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2024_NAME, VP_KHR_ROADMAP_2024_SPEC_VERSION};

    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfileInstanceExtensionProperties(&profile, nullptr, &propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(0, propertyCount);

    propertyCount = 2;

    std::vector<VkExtensionProperties> properties(propertyCount);
    VkResult result1 = vpGetProfileInstanceExtensionProperties(&profile, nullptr, &propertyCount, &properties[0]);
    EXPECT_EQ(VK_SUCCESS, result1);
    EXPECT_EQ(0, propertyCount);
}

TEST(api_get_profile_fallbacks, empty) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION};

    uint32_t count = 0;
    VkResult result0 = vpGetProfileFallbacks(&profile, &count, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(0, count);

    count = 1;

    std::vector<VpProfileProperties> data(count);
    VkResult result1 = vpGetProfileFallbacks(&profile, &count, &data[0]);
    EXPECT_EQ(VK_SUCCESS, result1);
    EXPECT_EQ(0, count);
}

TEST(api_get_profile_formats, unspecified) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION};

    uint32_t formatCount = 0;
    VkResult result0 = vpGetProfileFormats(&profile, nullptr, &formatCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(0, formatCount);
}

TEST(api_get_profile_properties, get_properties2) {
    VkPhysicalDeviceProperties2 profileProperties2{};
    profileProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    profileProperties2.pNext = nullptr;

    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION};

    vpGetProfileProperties(&profile, nullptr, &profileProperties2);

    EXPECT_EQ(8192, profileProperties2.properties.limits.maxImageDimension1D);
    EXPECT_EQ(8192, profileProperties2.properties.limits.maxImageDimension2D);
    EXPECT_EQ(0, profileProperties2.properties.limits.maxImageDimension3D);
    EXPECT_EQ(8192, profileProperties2.properties.limits.maxImageDimensionCube);
    EXPECT_EQ(2048, profileProperties2.properties.limits.maxImageArrayLayers);
    EXPECT_EQ(7, profileProperties2.properties.limits.maxColorAttachments);
}

TEST(api_get_profile_structures, get_properties_chain) {
    VkPhysicalDeviceVulkan13Properties properties1{};
    properties1.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES;
    properties1.pNext = nullptr;

    VkPhysicalDeviceVulkan11Properties properties0{};
    properties0.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
    properties0.pNext = &properties1;

    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION};

    vpGetProfileProperties(&profile, nullptr, &properties0);

    EXPECT_EQ(1073741824, properties1.maxBufferSize);
    EXPECT_EQ(256, properties1.maxInlineUniformBlockSize);
    EXPECT_EQ(4, properties1.maxPerStageDescriptorInlineUniformBlocks);
    EXPECT_EQ(134217727, properties0.maxMultiviewInstanceIndex);
    EXPECT_EQ(6, properties0.maxMultiviewViewCount);
}

TEST(api_get_profile_structures, get_features) {
    VkPhysicalDeviceVulkan12Features deviceVulkan12Features = {};
    deviceVulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    deviceVulkan12Features.pNext = nullptr;

    VkPhysicalDeviceFeatures2 deviceFeatures2 = {};
    deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    deviceFeatures2.pNext = &deviceVulkan12Features;

    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION};

    vpGetProfileFeatures(&profile, nullptr, &deviceFeatures2);

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
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.subgroupBroadcastDynamicId);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.imagelessFramebuffer);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.shaderOutputLayer);
}

TEST(api_get_profile_feature_structure_types, properties_full) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION};

    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfileFeatureStructureTypes(&profile, nullptr, &propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(4, propertyCount);

    propertyCount = 5;

    std::vector<VkStructureType> properties(propertyCount);
    VkResult result1 = vpGetProfileFeatureStructureTypes(&profile, nullptr, &propertyCount, &properties[0]);
    EXPECT_EQ(VK_SUCCESS, result1);
    EXPECT_EQ(4, propertyCount);

    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, properties[0]);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, properties[1]);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES, properties[2]);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, properties[3]);
}

TEST(api_get_profile_feature_structure_types, properties_partial) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION};

    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfileFeatureStructureTypes(&profile, nullptr, &propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(4, propertyCount);

    propertyCount = 3;

    std::vector<VkStructureType> properties(propertyCount);
    VkResult result1 = vpGetProfileFeatureStructureTypes(&profile, nullptr, &propertyCount, &properties[0]);
    EXPECT_EQ(VK_INCOMPLETE, result1);
    EXPECT_EQ(3, propertyCount);

    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, properties[0]);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, properties[1]);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES, properties[2]);
}

TEST(api_get_profile_property_structure_types, properties_full) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION};

    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfilePropertyStructureTypes(&profile, nullptr, &propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(4, propertyCount);

    propertyCount = 5;

    std::vector<VkStructureType> properties(propertyCount);
    VkResult result1 = vpGetProfilePropertyStructureTypes(&profile, nullptr, &propertyCount, &properties[0]);
    EXPECT_EQ(VK_SUCCESS, result1);
    EXPECT_EQ(4, propertyCount);

    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES, properties[0]);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES, properties[1]);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES, properties[2]);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, properties[3]);
}

TEST(api_get_profile_property_structure_types, properties_partial) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION};

    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfilePropertyStructureTypes(&profile, nullptr, &propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(4, propertyCount);

    propertyCount = 3;

    std::vector<VkStructureType> properties(propertyCount);
    VkResult result1 = vpGetProfilePropertyStructureTypes(&profile, nullptr, &propertyCount, &properties[0]);
    EXPECT_EQ(VK_INCOMPLETE, result1);
    EXPECT_EQ(3, propertyCount);

    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES, properties[0]);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES, properties[1]);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES, properties[2]);
}
