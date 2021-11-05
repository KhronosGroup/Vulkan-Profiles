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
#include "vulkan_profiles.hpp"

TEST(api_get_profile_structures, get_properties2) {
    VkPhysicalDeviceProperties2 profileProperties2{};
    profileProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    profileProperties2.pNext = nullptr;

    const VpProfileProperties Profile = {VP_LUNARG_DESKTOP_PORTABILITY_2022_NAME, 1};

    vpGetProfileStructures(&Profile, &profileProperties2);

    EXPECT_EQ(16384, profileProperties2.properties.limits.maxImageDimension1D);
    EXPECT_EQ(16384, profileProperties2.properties.limits.maxImageDimension2D);
    EXPECT_EQ(2048, profileProperties2.properties.limits.maxImageDimension3D);
    EXPECT_EQ(16384, profileProperties2.properties.limits.maxImageDimensionCube);
    EXPECT_EQ(2048, profileProperties2.properties.limits.maxImageArrayLayers);
    EXPECT_EQ(8, profileProperties2.properties.limits.maxColorAttachments);
}

TEST(api_get_profile_structures, get_properties_single) {
    VkPhysicalDeviceDescriptorIndexingProperties properties{};
    properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES;
    properties.pNext = nullptr;

    const VpProfileProperties Profile = {VP_LUNARG_DESKTOP_PORTABILITY_2022_NAME, 1};

    vpGetProfileStructures(&Profile, &properties);

    EXPECT_EQ(1048576, properties.maxUpdateAfterBindDescriptorsInAllPools);
    EXPECT_EQ(16, properties.maxPerStageDescriptorUpdateAfterBindSamplers);
}

TEST(api_get_profile_structures, get_properties_chain) {
    VkPhysicalDeviceDescriptorIndexingProperties properties0{};
    properties0.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES;
    properties0.pNext = nullptr;

    VkPhysicalDeviceInlineUniformBlockPropertiesEXT properties1{};
    properties1.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_PROPERTIES_EXT;
    properties1.pNext = &properties0;

    const VpProfileProperties Profile = {VP_LUNARG_DESKTOP_PORTABILITY_2022_NAME, 1};

    vpGetProfileStructures(&Profile, &properties1);

    EXPECT_EQ(1048576, properties0.maxUpdateAfterBindDescriptorsInAllPools);
    EXPECT_EQ(16, properties0.maxPerStageDescriptorUpdateAfterBindSamplers);
    EXPECT_EQ(256, properties1.maxInlineUniformBlockSize);
}

TEST(api_get_profile_structures, get_features) {
    VkPhysicalDeviceVulkan12Features deviceVulkan12Features = {};
    deviceVulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    deviceVulkan12Features.pNext = nullptr;

    const VpProfileProperties Profile = {VP_KHR_ROADMAP_2022_NAME, 1};

    vpGetProfileStructures(&Profile, &deviceVulkan12Features);

    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.samplerMirrorClampToEdge);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.drawIndirectCount);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.storageBuffer8BitAccess);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.uniformAndStorageBuffer8BitAccess);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.storagePushConstant8);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.shaderBufferInt64Atomics);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.shaderSharedInt64Atomics);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.shaderFloat16);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.shaderInt8);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.descriptorIndexing);
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

TEST(api_get_profile_structures, properties_full) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, 1};

    uint32_t propertyCount = 0;
    vpGetProfileStructureProperties(&profile, &propertyCount, nullptr);
    EXPECT_EQ(17, propertyCount);

    std::vector<VpStructureProperties> properties(propertyCount);
    vpGetProfileStructureProperties(&profile, &propertyCount, &properties[0]);

    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, properties[0].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, properties[1].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, properties[2].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES_KHR, properties[3].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR, properties[4].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_WORKGROUP_MEMORY_FEATURES_KHR, properties[5].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES_EXT, properties[6].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT, properties[7].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES_EXT, properties[8].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES_EXT, properties[9].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES_EXT, properties[10].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES_EXT, properties[11].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT, properties[12].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT, properties[13].type);

    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES, properties[14].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES, properties[15].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, properties[16].type);
}

TEST(api_get_profile_structures, properties_partial) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, 1};

    uint32_t structureTypesCount = 0;
    vpGetProfileStructureProperties(&profile, &structureTypesCount, nullptr);
    EXPECT_EQ(17, structureTypesCount);

    structureTypesCount = 5;

    std::vector<VpStructureProperties> structureTypes(structureTypesCount);
    vpGetProfileStructureProperties(&profile, &structureTypesCount, &structureTypes[0]);
    EXPECT_EQ(5, structureTypesCount);

    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, structureTypes[0].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, structureTypes[1].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, structureTypes[2].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES_KHR, structureTypes[3].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR, structureTypes[4].type);
}