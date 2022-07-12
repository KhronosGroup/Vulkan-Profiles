/*
 * Copyright (C) 2021-2022 Valve Corporation
 * Copyright (C) 2021-2022 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Ziga Markus <ziga@lunarg.com>
 */

#include <vulkan/vulkan_core.h>

#include <gtest/gtest.h>
#include "profiles_test_helper.h"

static VkPhysicalDevice gpu = VK_NULL_HANDLE;
static profiles_test::VulkanInstanceBuilder inst_builder;

class TestsMergedIntersection : public VkTestFramework {
  public:
   TestsMergedIntersection(){};
   ~TestsMergedIntersection(){};

    static void SetUpTestSuite() {
        VkResult err = VK_SUCCESS;

        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "intersect.json";
        settings.emulate_portability = true;
        settings.profile_name = "VP_LUNARG_desktop_portability_2021";
        settings.simulate_capabilities = SIMULATE_MAX_ENUM;
        settings.debug_reports = DEBUG_REPORT_ERROR_BIT;

        err = inst_builder.init(&settings);
        ASSERT_EQ(err, VK_SUCCESS);

        err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);
        ASSERT_EQ(err, VK_SUCCESS);
    }

    static void TearDownTestSuite() { inst_builder.reset(); };
};

TEST_F(TestsMergedIntersection, TestDesktopPortability2022Limits) {
    VkPhysicalDeviceProperties gpu_props{};
    vkGetPhysicalDeviceProperties(gpu, &gpu_props);

    VkPhysicalDeviceFeatures gpu_feats{};
    vkGetPhysicalDeviceFeatures(gpu, &gpu_feats);

    VkPhysicalDeviceMemoryProperties gpu_mem_props{};
    vkGetPhysicalDeviceMemoryProperties(gpu, &gpu_mem_props);

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_family_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_family_props(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_family_count, queue_family_props.data());

    // Limits
    EXPECT_EQ(gpu_props.limits.maxImageDimension1D, 16384u);
    EXPECT_EQ(gpu_props.limits.maxImageDimension2D, 16384u);
    EXPECT_EQ(gpu_props.limits.maxImageDimension3D, 2048u);
    EXPECT_EQ(gpu_props.limits.maxImageDimensionCube, 16384u);
    EXPECT_EQ(gpu_props.limits.maxImageArrayLayers, 2048u);
    EXPECT_EQ(gpu_props.limits.maxTexelBufferElements, 67108864u);
    EXPECT_EQ(gpu_props.limits.maxUniformBufferRange, 65536u);
    EXPECT_EQ(gpu_props.limits.maxStorageBufferRange, 134217728u);
    EXPECT_EQ(gpu_props.limits.maxPushConstantsSize, 128u);
    EXPECT_EQ(gpu_props.limits.maxMemoryAllocationCount, 4096u);
    EXPECT_EQ(gpu_props.limits.maxSamplerAllocationCount, 1024u);
    EXPECT_EQ(gpu_props.limits.bufferImageGranularity, 1024);
    EXPECT_EQ(gpu_props.limits.maxBoundDescriptorSets, 8u);
    EXPECT_EQ(gpu_props.limits.maxPerStageDescriptorSamplers, 16u);
    EXPECT_EQ(gpu_props.limits.maxPerStageDescriptorUniformBuffers, 15u);
    EXPECT_EQ(gpu_props.limits.maxPerStageDescriptorStorageBuffers, 16u);
    EXPECT_EQ(gpu_props.limits.maxPerStageDescriptorSampledImages, 128u);
    EXPECT_EQ(gpu_props.limits.maxPerStageDescriptorStorageImages, 8u);
    EXPECT_EQ(gpu_props.limits.maxPerStageDescriptorInputAttachments, 8u);
    EXPECT_EQ(gpu_props.limits.maxPerStageResources, 128u);
    EXPECT_EQ(gpu_props.limits.maxDescriptorSetSamplers, 80u);
    EXPECT_EQ(gpu_props.limits.maxDescriptorSetUniformBuffers, 90u);
    EXPECT_EQ(gpu_props.limits.maxDescriptorSetUniformBuffersDynamic, 8u);
    EXPECT_EQ(gpu_props.limits.maxDescriptorSetStorageBuffers, 155u);
    EXPECT_EQ(gpu_props.limits.maxDescriptorSetStorageBuffersDynamic, 8u);
    EXPECT_EQ(gpu_props.limits.maxDescriptorSetSampledImages, 256u);
    EXPECT_EQ(gpu_props.limits.maxDescriptorSetStorageImages, 40u);
    EXPECT_EQ(gpu_props.limits.maxDescriptorSetInputAttachments, 8u);
    EXPECT_EQ(gpu_props.limits.maxVertexInputAttributes, 28u);
    EXPECT_EQ(gpu_props.limits.maxVertexInputBindings, 28u);
    EXPECT_EQ(gpu_props.limits.maxVertexInputAttributeOffset, 2047u);
    EXPECT_EQ(gpu_props.limits.maxVertexInputBindingStride, 2048u);
    EXPECT_EQ(gpu_props.limits.maxVertexOutputComponents, 124u);
    EXPECT_EQ(gpu_props.limits.maxTessellationGenerationLevel, 64u);
    EXPECT_EQ(gpu_props.limits.maxTessellationPatchSize, 32u);
    EXPECT_EQ(gpu_props.limits.maxTessellationControlPerVertexInputComponents, 124u);
    EXPECT_EQ(gpu_props.limits.maxTessellationControlPerVertexOutputComponents, 124u);
    EXPECT_EQ(gpu_props.limits.maxTessellationControlPerPatchOutputComponents, 120u);
    EXPECT_EQ(gpu_props.limits.maxTessellationControlTotalOutputComponents, 2048u);
    EXPECT_EQ(gpu_props.limits.maxTessellationEvaluationInputComponents, 124u);
    EXPECT_EQ(gpu_props.limits.maxTessellationEvaluationOutputComponents, 124u);
    EXPECT_EQ(gpu_props.limits.maxFragmentInputComponents, 116u);
    EXPECT_EQ(gpu_props.limits.maxFragmentOutputAttachments, 8u);
    EXPECT_EQ(gpu_props.limits.maxFragmentDualSrcAttachments, 1u);
    EXPECT_EQ(gpu_props.limits.maxFragmentCombinedOutputResources, 8u);
    EXPECT_EQ(gpu_props.limits.maxComputeSharedMemorySize, 32768u);
    EXPECT_EQ(gpu_props.limits.maxComputeWorkGroupCount[0], 65535u);
    EXPECT_EQ(gpu_props.limits.maxComputeWorkGroupCount[1], 65535u);
    EXPECT_EQ(gpu_props.limits.maxComputeWorkGroupCount[2], 65535u);
    EXPECT_EQ(gpu_props.limits.maxComputeWorkGroupInvocations, 1024u);
    EXPECT_EQ(gpu_props.limits.maxComputeWorkGroupSize[0], 1024u);
    EXPECT_EQ(gpu_props.limits.maxComputeWorkGroupSize[1], 1024u);
    EXPECT_EQ(gpu_props.limits.maxComputeWorkGroupSize[2], 64u);
    EXPECT_EQ(gpu_props.limits.subPixelPrecisionBits, 4u);
    EXPECT_EQ(gpu_props.limits.subTexelPrecisionBits, 4u);
    EXPECT_EQ(gpu_props.limits.mipmapPrecisionBits, 4u);
    EXPECT_EQ(gpu_props.limits.maxDrawIndexedIndexValue, 4294967295u);
    EXPECT_EQ(gpu_props.limits.maxDrawIndirectCount, 1073740000u);
    EXPECT_EQ(gpu_props.limits.maxSamplerLodBias, 4.0f);
    EXPECT_EQ(gpu_props.limits.maxSamplerAnisotropy, 16.0f);
    EXPECT_EQ(gpu_props.limits.maxViewports, 16u);
    EXPECT_EQ(gpu_props.limits.maxViewportDimensions[0], 16384u);
    EXPECT_EQ(gpu_props.limits.maxViewportDimensions[1], 16384u);
    EXPECT_EQ(gpu_props.limits.viewportBoundsRange[0], -32768.0f);
    EXPECT_EQ(gpu_props.limits.viewportBoundsRange[1], 32767.0f);
    EXPECT_EQ(gpu_props.limits.minMemoryMapAlignment, 4096u);
    EXPECT_EQ(gpu_props.limits.minTexelBufferOffsetAlignment, 256u);
    EXPECT_EQ(gpu_props.limits.minUniformBufferOffsetAlignment, 256u);
    EXPECT_EQ(gpu_props.limits.minStorageBufferOffsetAlignment, 64u);
    EXPECT_EQ(gpu_props.limits.minTexelOffset, -8);
    EXPECT_EQ(gpu_props.limits.maxTexelOffset, 7u);
    EXPECT_EQ(gpu_props.limits.minTexelGatherOffset, -8);
    EXPECT_EQ(gpu_props.limits.maxTexelGatherOffset, 7u);
    EXPECT_EQ(gpu_props.limits.minInterpolationOffset, -0.5f);
    EXPECT_EQ(gpu_props.limits.maxInterpolationOffset, 0.4375f);
    EXPECT_EQ(gpu_props.limits.subPixelInterpolationOffsetBits, 4u);
    EXPECT_EQ(gpu_props.limits.maxFramebufferWidth, 16384u);
    EXPECT_EQ(gpu_props.limits.maxFramebufferHeight, 16384u);
    EXPECT_EQ(gpu_props.limits.maxFramebufferLayers, 1024u);
    EXPECT_EQ(gpu_props.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(gpu_props.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
    EXPECT_EQ(gpu_props.limits.framebufferDepthSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(gpu_props.limits.framebufferDepthSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
    EXPECT_EQ(gpu_props.limits.framebufferStencilSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(gpu_props.limits.framebufferStencilSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
    EXPECT_EQ(gpu_props.limits.maxColorAttachments, 8u);
    EXPECT_EQ(gpu_props.limits.sampledImageColorSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(gpu_props.limits.sampledImageColorSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
    EXPECT_EQ(gpu_props.limits.sampledImageIntegerSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(gpu_props.limits.sampledImageIntegerSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
    EXPECT_EQ(gpu_props.limits.sampledImageDepthSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(gpu_props.limits.sampledImageDepthSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
    EXPECT_EQ(gpu_props.limits.sampledImageStencilSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(gpu_props.limits.sampledImageStencilSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
    EXPECT_EQ(gpu_props.limits.storageImageSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(gpu_props.limits.maxSampleMaskWords, 1u);
    EXPECT_EQ(gpu_props.limits.maxClipDistances, 8u);
    EXPECT_EQ(gpu_props.limits.maxCombinedClipAndCullDistances, 8u);
    EXPECT_EQ(gpu_props.limits.discreteQueuePriorities, 2u);
    EXPECT_EQ(gpu_props.limits.pointSizeRange[0], 1.0f);
    EXPECT_EQ(gpu_props.limits.pointSizeRange[1], 64.0f);
    EXPECT_EQ(gpu_props.limits.lineWidthRange[0], 1.0f);
    EXPECT_EQ(gpu_props.limits.lineWidthRange[1], 1.0f);
    EXPECT_EQ(gpu_props.limits.pointSizeGranularity, 1.0f);
    EXPECT_EQ(gpu_props.limits.lineWidthGranularity, 0.5f);

    // Features
    EXPECT_EQ(gpu_feats.depthBiasClamp, VK_TRUE);
    EXPECT_EQ(gpu_feats.depthClamp, VK_TRUE);
    EXPECT_EQ(gpu_feats.drawIndirectFirstInstance, VK_TRUE);
    EXPECT_EQ(gpu_feats.dualSrcBlend, VK_TRUE);
    EXPECT_EQ(gpu_feats.fillModeNonSolid, VK_TRUE);
    EXPECT_EQ(gpu_feats.fragmentStoresAndAtomics, VK_TRUE);
    EXPECT_EQ(gpu_feats.fullDrawIndexUint32, VK_TRUE);
    EXPECT_EQ(gpu_feats.imageCubeArray, VK_TRUE);
    EXPECT_EQ(gpu_feats.independentBlend, VK_TRUE);
    EXPECT_EQ(gpu_feats.inheritedQueries, VK_TRUE);
    EXPECT_EQ(gpu_feats.largePoints, VK_TRUE);
    EXPECT_EQ(gpu_feats.multiDrawIndirect, VK_TRUE);
    EXPECT_EQ(gpu_feats.multiViewport, VK_TRUE);
    EXPECT_EQ(gpu_feats.occlusionQueryPrecise, VK_TRUE);
    EXPECT_EQ(gpu_feats.robustBufferAccess, VK_TRUE);
    EXPECT_EQ(gpu_feats.sampleRateShading, VK_TRUE);
    EXPECT_EQ(gpu_feats.samplerAnisotropy, VK_TRUE);
    EXPECT_EQ(gpu_feats.shaderClipDistance, VK_TRUE);
    EXPECT_EQ(gpu_feats.shaderImageGatherExtended, VK_TRUE);
    EXPECT_EQ(gpu_feats.shaderSampledImageArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(gpu_feats.shaderStorageBufferArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(gpu_feats.shaderStorageImageArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(gpu_feats.shaderStorageImageExtendedFormats, VK_TRUE);
    EXPECT_EQ(gpu_feats.shaderStorageImageWriteWithoutFormat, VK_TRUE);
    EXPECT_EQ(gpu_feats.shaderTessellationAndGeometryPointSize, VK_TRUE);
    EXPECT_EQ(gpu_feats.shaderUniformBufferArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(gpu_feats.tessellationShader, VK_TRUE);
    EXPECT_EQ(gpu_feats.textureCompressionBC, VK_TRUE);
    EXPECT_EQ(gpu_feats.vertexPipelineStoresAndAtomics, VK_TRUE);
}

TEST_F(TestsMergedIntersection, TestSetCombinationMode) {
    uint32_t count;
    vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, nullptr);
    EXPECT_EQ(count, 43);
}

TEST_F(TestsMergedIntersection, TestPhysicalDeviceFeatures) {
    VkPhysicalDeviceFeatures2 features{};
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(features.features.depthBiasClamp, VK_TRUE);
    EXPECT_EQ(features.features.depthClamp, VK_TRUE);
    EXPECT_EQ(features.features.drawIndirectFirstInstance, VK_TRUE);
    EXPECT_EQ(features.features.dualSrcBlend, VK_TRUE);
    EXPECT_EQ(features.features.fillModeNonSolid, VK_TRUE);
    EXPECT_EQ(features.features.fragmentStoresAndAtomics, VK_TRUE);
    EXPECT_EQ(features.features.fullDrawIndexUint32, VK_TRUE);
    EXPECT_EQ(features.features.imageCubeArray, VK_TRUE);
    EXPECT_EQ(features.features.independentBlend, VK_TRUE);
    EXPECT_EQ(features.features.inheritedQueries, VK_TRUE);
    EXPECT_EQ(features.features.largePoints, VK_TRUE);
    EXPECT_EQ(features.features.multiDrawIndirect, VK_TRUE);
    EXPECT_EQ(features.features.multiViewport, VK_TRUE);
    EXPECT_EQ(features.features.occlusionQueryPrecise, VK_TRUE);
    EXPECT_EQ(features.features.robustBufferAccess, VK_TRUE);
    EXPECT_EQ(features.features.sampleRateShading, VK_TRUE);
    EXPECT_EQ(features.features.samplerAnisotropy, VK_TRUE);
    EXPECT_EQ(features.features.shaderClipDistance, VK_TRUE);
    EXPECT_EQ(features.features.shaderImageGatherExtended, VK_TRUE);
    EXPECT_EQ(features.features.shaderSampledImageArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(features.features.shaderStorageBufferArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(features.features.shaderStorageImageArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(features.features.shaderStorageImageExtendedFormats, VK_TRUE);
    EXPECT_EQ(features.features.shaderStorageImageWriteWithoutFormat, VK_TRUE);
    EXPECT_EQ(features.features.shaderTessellationAndGeometryPointSize, VK_TRUE);
    EXPECT_EQ(features.features.shaderUniformBufferArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(features.features.tessellationShader, VK_TRUE);
    EXPECT_EQ(features.features.textureCompressionBC, VK_TRUE);
    EXPECT_EQ(features.features.vertexPipelineStoresAndAtomics, VK_TRUE);
}

TEST_F(TestsMergedIntersection, Test16BitStorageFeatures) {
#ifdef VK_KHR_16bit_storage

    VkPhysicalDevice16BitStorageFeaturesKHR f_16_bit_storage_features{};
    f_16_bit_storage_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &f_16_bit_storage_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(f_16_bit_storage_features.storageBuffer16BitAccess, VK_TRUE);
    EXPECT_EQ(f_16_bit_storage_features.uniformAndStorageBuffer16BitAccess, VK_TRUE);

#endif
}

TEST_F(TestsMergedIntersection, Test8BitStorageFeatures) {
#ifdef VK_KHR_8bit_storage

    VkPhysicalDevice8BitStorageFeaturesKHR f_8_bit_storage_features{};
    f_8_bit_storage_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &f_8_bit_storage_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(f_8_bit_storage_features.storageBuffer8BitAccess, VK_TRUE);
    EXPECT_EQ(f_8_bit_storage_features.uniformAndStorageBuffer8BitAccess, VK_TRUE);

#endif
}

TEST_F(TestsMergedIntersection, TestDescriptorIndexingFeatures) {
#ifdef VK_EXT_descriptor_indexing

    VkPhysicalDeviceDescriptorIndexingFeaturesEXT descriptor_indexing_features{};
    descriptor_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &descriptor_indexing_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(descriptor_indexing_features.shaderUniformTexelBufferArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.shaderStorageTexelBufferArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.shaderSampledImageArrayNonUniformIndexing, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.shaderUniformTexelBufferArrayNonUniformIndexing, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.descriptorBindingSampledImageUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.descriptorBindingStorageImageUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.descriptorBindingStorageBufferUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.descriptorBindingUpdateUnusedWhilePending, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.descriptorBindingPartiallyBound, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.descriptorBindingVariableDescriptorCount, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.runtimeDescriptorArray, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.descriptorBindingUniformTexelBufferUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.descriptorBindingStorageTexelBufferUpdateAfterBind, VK_TRUE);

#endif
}

TEST_F(TestsMergedIntersection, TestHostQueryResetFeatures) {
#ifdef VK_EXT_host_query_reset

    VkPhysicalDeviceHostQueryResetFeaturesEXT host_query_reset_features{};
    host_query_reset_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &host_query_reset_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(host_query_reset_features.hostQueryReset, VK_TRUE);

#endif
}

TEST_F(TestsMergedIntersection, TestUniformBufferStandardLayoutFeatures) {
#ifdef VK_KHR_uniform_buffer_standard_layout

    VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR uniform_buffer_standard_layout_features{};
    uniform_buffer_standard_layout_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &uniform_buffer_standard_layout_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(uniform_buffer_standard_layout_features.uniformBufferStandardLayout, VK_TRUE);

#endif
}

TEST_F(TestsMergedIntersection, TestShaderDrawParametersFeatures) {
#ifdef VK_VERSION_1_1

    VkPhysicalDeviceShaderDrawParametersFeatures shader_draw_paramenters_features{};
    shader_draw_paramenters_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &shader_draw_paramenters_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(shader_draw_paramenters_features.shaderDrawParameters, VK_TRUE);

#endif
}

TEST_F(TestsMergedIntersection, TestShaderFloat16Int8Features) {
#ifdef VK_KHR_shader_float16_int8

    VkPhysicalDeviceShaderFloat16Int8FeaturesKHR shader_float16_int8_features{};
    shader_float16_int8_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &shader_float16_int8_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(shader_float16_int8_features.shaderInt8, VK_TRUE);

#endif
}

TEST_F(TestsMergedIntersection, TestVariablePointersFeatures) {
#ifdef VK_KHR_variable_pointers

    VkPhysicalDeviceVariablePointersFeaturesKHR variable_pointer_features{};
    variable_pointer_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &variable_pointer_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(variable_pointer_features.variablePointersStorageBuffer, VK_TRUE);
    EXPECT_EQ(variable_pointer_features.variablePointers, VK_TRUE);

#endif
}

TEST_F(TestsMergedIntersection, TestImagelessFramebufferFeatures) {
#ifdef VK_KHR_imageless_framebuffer

    VkPhysicalDeviceImagelessFramebufferFeaturesKHR imageless_framebuffer_features{};
    imageless_framebuffer_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &imageless_framebuffer_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(imageless_framebuffer_features.imagelessFramebuffer, VK_TRUE);

#endif
}

TEST_F(TestsMergedIntersection, TestDescriptorIndexingProperties) {
#ifdef VK_EXT_descriptor_indexing

    VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptor_indexing_properties{};
    descriptor_indexing_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &descriptor_indexing_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(descriptor_indexing_properties.maxUpdateAfterBindDescriptorsInAllPools, 1048576u);
    EXPECT_EQ(descriptor_indexing_properties.maxPerStageDescriptorUpdateAfterBindSamplers, 16u);
    EXPECT_EQ(descriptor_indexing_properties.maxPerStageDescriptorUpdateAfterBindUniformBuffers, 15u);
    EXPECT_EQ(descriptor_indexing_properties.maxPerStageDescriptorUpdateAfterBindStorageBuffers, 31u);
    EXPECT_EQ(descriptor_indexing_properties.maxPerStageDescriptorUpdateAfterBindSampledImages, 128u);
    EXPECT_EQ(descriptor_indexing_properties.maxPerStageDescriptorUpdateAfterBindStorageImages, 8u);
    EXPECT_EQ(descriptor_indexing_properties.maxPerStageDescriptorUpdateAfterBindInputAttachments, 128u);
    EXPECT_EQ(descriptor_indexing_properties.maxPerStageUpdateAfterBindResources, 159u);
    EXPECT_EQ(descriptor_indexing_properties.maxDescriptorSetUpdateAfterBindSamplers, 80u);
    EXPECT_EQ(descriptor_indexing_properties.maxDescriptorSetUpdateAfterBindUniformBuffers, 90u);
    EXPECT_EQ(descriptor_indexing_properties.maxDescriptorSetUpdateAfterBindUniformBuffersDynamic, 8u);
    EXPECT_EQ(descriptor_indexing_properties.maxDescriptorSetUpdateAfterBindStorageBuffers, 155u);
    EXPECT_EQ(descriptor_indexing_properties.maxDescriptorSetUpdateAfterBindStorageBuffersDynamic, 8u);
    EXPECT_EQ(descriptor_indexing_properties.maxDescriptorSetUpdateAfterBindSampledImages, 640u);
    EXPECT_EQ(descriptor_indexing_properties.maxDescriptorSetUpdateAfterBindStorageImages, 40u);
    EXPECT_EQ(descriptor_indexing_properties.maxDescriptorSetUpdateAfterBindInputAttachments, 8u);
#endif
}

TEST_F(TestsMergedIntersection, TestDepthStencilResolveProperties) {
#ifdef VK_KHR_depth_stencil_resolve

    VkPhysicalDeviceDepthStencilResolvePropertiesKHR depth_stencil_resolve_properties{};
    depth_stencil_resolve_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &depth_stencil_resolve_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(depth_stencil_resolve_properties.supportedDepthResolveModes & VK_RESOLVE_MODE_SAMPLE_ZERO_BIT,
              VK_RESOLVE_MODE_SAMPLE_ZERO_BIT);
    EXPECT_EQ(depth_stencil_resolve_properties.supportedStencilResolveModes & VK_RESOLVE_MODE_SAMPLE_ZERO_BIT,
              VK_RESOLVE_MODE_SAMPLE_ZERO_BIT);
    EXPECT_EQ(depth_stencil_resolve_properties.independentResolveNone, VK_TRUE);
    EXPECT_EQ(depth_stencil_resolve_properties.independentResolve, VK_TRUE);
#endif
}

TEST_F(TestsMergedIntersection, TestMaintenance3Properties) {
#ifdef VK_KHR_maintenance3

    VkPhysicalDeviceMaintenance3PropertiesKHR maintenance_3_properties{};
    maintenance_3_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &maintenance_3_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(maintenance_3_properties.maxPerSetDescriptors, 700u);
    EXPECT_EQ(maintenance_3_properties.maxMemoryAllocationSize, 1073741824u);
#endif
}

TEST_F(TestsMergedIntersection, TestFormatR8Snorm) {
    VkFormat format = VK_FORMAT_R8_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR8Uint) {
    VkFormat format = VK_FORMAT_R8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
                                                   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR8Sint) {
    VkFormat format = VK_FORMAT_R8_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
                                                   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR8G8Unorm) {
    VkFormat format = VK_FORMAT_R8G8_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR8G8Snorm) {
    VkFormat format = VK_FORMAT_R8G8_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR8G8Uint) {
    VkFormat format = VK_FORMAT_R8G8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
                                                   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR8G8Sint) {
    VkFormat format = VK_FORMAT_R8G8_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
                                                   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR8G8B8A8Unorm) {
    VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR8G8B8A8Snorm) {
    VkFormat format = VK_FORMAT_R8G8B8A8_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR8G8B8A8Uint) {
    VkFormat format = VK_FORMAT_R8G8B8A8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
                                                   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR8G8B8A8Sint) {
    VkFormat format = VK_FORMAT_R8G8B8A8_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
                                                   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR8G8B8A8Srgb) {
    VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatA8B8G8R8UnormPack32) {
    VkFormat format = VK_FORMAT_A8B8G8R8_UNORM_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatA8B8G8R8SnormPack32) {
    VkFormat format = VK_FORMAT_A8B8G8R8_SNORM_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatA8B8G8R8UintPack32) {
    VkFormat format = VK_FORMAT_A8B8G8R8_UINT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
                                                   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatA8B8G8R8SintPack32) {
    VkFormat format = VK_FORMAT_A8B8G8R8_SINT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
                                                   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatA8B8G8R8SrgbPack32) {
    VkFormat format = VK_FORMAT_A8B8G8R8_SRGB_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatA2B10G10R10UnormPack32) {
    VkFormat format = VK_FORMAT_A2B10G10R10_UNORM_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatA2B10G10R10UintPack32) {
    VkFormat format = VK_FORMAT_A2B10G10R10_UINT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
                                                   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR16Unorm) {
    VkFormat format = VK_FORMAT_R16_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR16Snorm) {
    VkFormat format = VK_FORMAT_R16_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR16Uint) {
    VkFormat format = VK_FORMAT_R16_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
                                                   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR16Sint) {
    VkFormat format = VK_FORMAT_R16_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
                                                   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR16Sfloat) {
    VkFormat format = VK_FORMAT_R16_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR16G16Unorm) {
    VkFormat format = VK_FORMAT_R16G16_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR16G16Snorm) {
    VkFormat format = VK_FORMAT_R16G16_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR16G16Uint) {
    VkFormat format = VK_FORMAT_R16G16_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
                                                   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR16G16Sint) {
    VkFormat format = VK_FORMAT_R16G16_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
                                                   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR16G16Sfloat) {
    VkFormat format = VK_FORMAT_R16G16_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR16G16B16A16Unorm) {
    VkFormat format = VK_FORMAT_R16G16B16A16_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR16G16B16A16Snorm) {
    VkFormat format = VK_FORMAT_R16G16B16A16_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR16G16B16A16Uint) {
    VkFormat format = VK_FORMAT_R16G16B16A16_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
                                                   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR16G16B16A16Sint) {
    VkFormat format = VK_FORMAT_R16G16B16A16_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
                                                   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR16G16B16A16Sfloat) {
    VkFormat format = VK_FORMAT_R16G16B16A16_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR32Uint) {
    VkFormat format = VK_FORMAT_R32_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
                                                   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR32Sint) {
    VkFormat format = VK_FORMAT_R32_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
                                                   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR32Sfloat) {
    VkFormat format = VK_FORMAT_R32_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR32G32Uint) {
    VkFormat format = VK_FORMAT_R32G32_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
                                                   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR32G32Sint) {
    VkFormat format = VK_FORMAT_R32G32_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
                                                   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR32G32Sfloat) {
    VkFormat format = VK_FORMAT_R32G32_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR32G32B32A32Uint) {
    VkFormat format = VK_FORMAT_R32G32B32A32_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
                                                   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR32G32B32A32Sint) {
    VkFormat format = VK_FORMAT_R32G32B32A32_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
                                                   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR32G32B32A32Sfloat) {
    VkFormat format = VK_FORMAT_R32G32B32A32_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatB10G11R11UfloatPack32) {
    VkFormat format = VK_FORMAT_B10G11R11_UFLOAT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatE5b9g9r9UfloatPack32) {
    VkFormat format = VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatD16Unorm) {
    VkFormat format = VK_FORMAT_D16_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatD32Sfloat) {
    VkFormat format = VK_FORMAT_D32_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatBC1RGBUnormBLock) {
    VkFormat format = VK_FORMAT_BC1_RGB_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatBC1RGBSrgbBLock) {
    VkFormat format = VK_FORMAT_BC1_RGB_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatBC1RGBAUnormBLock) {
    VkFormat format = VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatBC1RGBASrgbBLock) {
    VkFormat format = VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatBC2UnormBLock) {
    VkFormat format = VK_FORMAT_BC2_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatBC2SrgbBLock) {
    VkFormat format = VK_FORMAT_BC2_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatBC3UnormBLock) {
    VkFormat format = VK_FORMAT_BC3_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatBC3SrgbBLock) {
    VkFormat format = VK_FORMAT_BC3_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatBC4UnormBLock) {
    VkFormat format = VK_FORMAT_BC4_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatBC4SnormBLock) {
    VkFormat format = VK_FORMAT_BC4_SNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatBC5UnormBLock) {
    VkFormat format = VK_FORMAT_BC5_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatBC5SnormBLock) {
    VkFormat format = VK_FORMAT_BC5_SNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatBC6hUfloatBLock) {
    VkFormat format = VK_FORMAT_BC6H_UFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatBC6hSfloatBLock) {
    VkFormat format = VK_FORMAT_BC6H_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatBC7UnormBLock) {
    VkFormat format = VK_FORMAT_BC7_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatBC7SrgbBLock) {
    VkFormat format = VK_FORMAT_BC7_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsMergedIntersection, TestFormatR8Unorm) {
    VkFormat format = VK_FORMAT_R8_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}
