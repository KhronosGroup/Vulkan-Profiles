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
 * Author: Christophe Riccio <christophe@lunarg.com>
 * Author: Mark Lobodzinski <mark@lunarg.com>
 */

#include <vulkan/vulkan_core.h>

#include <gtest/gtest.h>
#include "profiles_test_helper.h"

#ifdef _WIN32
#ifdef _DEBUG
static const char* CONFIG_PATH = "bin/Debug";
#else
static const char* CONFIG_PATH = "bin/Release";
#endif
#else
static const char* CONFIG_PATH = "lib";
#endif

static VkInstance instance;
static VkPhysicalDevice gpu;
static profiles_test::VulkanInstanceBuilder inst_builder;


class TestsCapabilitiesGenerated : public VkTestFramework {
  public:
    TestsCapabilitiesGenerated(){};
    ~TestsCapabilitiesGenerated(){};

    static void SetUpTestSuite() {
        VkResult err = VK_SUCCESS;

        const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
        profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

        inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_api_generated.json";
        settings.emulate_portability = true;
        settings.profile_name = "VP_LUNARG_test_api";
        settings.simulate_capabilities = SimulateCapabilityFlag::SIMULATE_ALL_CAPABILITIES;
        settings.debug_reports = DEBUG_REPORT_ERROR_BIT;
        err = inst_builder.makeInstance(&settings);

        instance = inst_builder.getInstance();
        err = inst_builder.getPhysicalDevice(&gpu);
    };

    static void TearDownTestSuite() {
        if (instance != VK_NULL_HANDLE) {
            vkDestroyInstance(instance, nullptr);
            instance = VK_NULL_HANDLE;
        }
    };

};

TEST_F(TestsCapabilitiesGenerated, TestDeviceGeneratedCommandsPropertiesNV) {
#ifdef VK_NV_device_generated_commands
    VkPhysicalDeviceDeviceGeneratedCommandsPropertiesNV device_generated_commands_properties{};
    device_generated_commands_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &device_generated_commands_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(device_generated_commands_properties.maxGraphicsShaderGroupCount, 1);
    EXPECT_EQ(device_generated_commands_properties.maxIndirectSequenceCount, 2);
    EXPECT_EQ(device_generated_commands_properties.maxIndirectCommandsTokenCount, 3);
    EXPECT_EQ(device_generated_commands_properties.maxIndirectCommandsStreamCount, 4);
    EXPECT_EQ(device_generated_commands_properties.maxIndirectCommandsTokenOffset, 5);
    EXPECT_EQ(device_generated_commands_properties.maxIndirectCommandsStreamStride, 6);
    EXPECT_EQ(device_generated_commands_properties.minSequencesCountBufferOffsetAlignment, 7);
    EXPECT_EQ(device_generated_commands_properties.minSequencesIndexBufferOffsetAlignment, 8);
    EXPECT_EQ(device_generated_commands_properties.minIndirectCommandsBufferOffsetAlignment, 9);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestMultiDrawPropertiesEXT) {
#ifdef VK_EXT_multi_draw
    VkPhysicalDeviceMultiDrawPropertiesEXT multi_draw_properties{};
    multi_draw_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &multi_draw_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(multi_draw_properties.maxMultiDrawCount, 10);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestPushDescriptorPropertiesKHR) {
#ifdef VK_KHR_push_descriptor
    VkPhysicalDevicePushDescriptorPropertiesKHR push_descriptor_properties{};
    push_descriptor_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &push_descriptor_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(push_descriptor_properties.maxPushDescriptors, 11);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestMultiviewPropertiesKHR) {
#ifdef VK_KHR_multiview
    VkPhysicalDeviceMultiviewPropertiesKHR multiview_properties{};
    multiview_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &multiview_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(multiview_properties.maxMultiviewViewCount, 12);
    EXPECT_EQ(multiview_properties.maxMultiviewInstanceIndex, 13);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestDiscardRectanglePropertiesEXT) {
#ifdef VK_EXT_discard_rectangles
    VkPhysicalDeviceDiscardRectanglePropertiesEXT discard_rectangle_properties{};
    discard_rectangle_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DISCARD_RECTANGLE_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &discard_rectangle_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(discard_rectangle_properties.maxDiscardRectangles, 14);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestSamplerFilterMinmaxPropertiesEXT) {
#ifdef VK_EXT_sampler_filter_minmax
    VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT sampler_filter_minmax_properties{};
    sampler_filter_minmax_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_FILTER_MINMAX_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &sampler_filter_minmax_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(sampler_filter_minmax_properties.filterMinmaxSingleComponentFormats, VK_TRUE);
    EXPECT_EQ(sampler_filter_minmax_properties.filterMinmaxImageComponentMapping, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestSampleLocationsPropertiesEXT) {
#ifdef VK_EXT_sample_locations
    VkPhysicalDeviceSampleLocationsPropertiesEXT sample_locations_properties{};
    sample_locations_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLE_LOCATIONS_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &sample_locations_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(sample_locations_properties.sampleLocationSampleCounts, VK_SAMPLE_COUNT_2_BIT);
    EXPECT_EQ(sample_locations_properties.maxSampleLocationGridSize.width, 16);
    EXPECT_EQ(sample_locations_properties.maxSampleLocationGridSize.height, 17);
    EXPECT_EQ(sample_locations_properties.sampleLocationCoordinateRange[0], 18.0);
    EXPECT_EQ(sample_locations_properties.sampleLocationCoordinateRange[1], 18.0);
    EXPECT_EQ(sample_locations_properties.sampleLocationSubPixelBits, 19);
    EXPECT_EQ(sample_locations_properties.variableSampleLocations, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestBlendOperationAdvancedPropertiesEXT) {
#ifdef VK_EXT_blend_operation_advanced
    VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT blend_operation_advanced_properties{};
    blend_operation_advanced_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &blend_operation_advanced_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(blend_operation_advanced_properties.advancedBlendMaxColorAttachments, 20);
    EXPECT_EQ(blend_operation_advanced_properties.advancedBlendIndependentBlend, VK_TRUE);
    EXPECT_EQ(blend_operation_advanced_properties.advancedBlendNonPremultipliedSrcColor, VK_TRUE);
    EXPECT_EQ(blend_operation_advanced_properties.advancedBlendNonPremultipliedDstColor, VK_TRUE);
    EXPECT_EQ(blend_operation_advanced_properties.advancedBlendCorrelatedOverlap, VK_TRUE);
    EXPECT_EQ(blend_operation_advanced_properties.advancedBlendAllOperations, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestInlineUniformBlockPropertiesEXT) {
#ifdef VK_EXT_inline_uniform_block
    VkPhysicalDeviceInlineUniformBlockPropertiesEXT inline_uniform_block_properties{};
    inline_uniform_block_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &inline_uniform_block_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(inline_uniform_block_properties.maxInlineUniformBlockSize, 21);
    EXPECT_EQ(inline_uniform_block_properties.maxPerStageDescriptorInlineUniformBlocks, 22);
    EXPECT_EQ(inline_uniform_block_properties.maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks, 23);
    EXPECT_EQ(inline_uniform_block_properties.maxDescriptorSetInlineUniformBlocks, 24);
    EXPECT_EQ(inline_uniform_block_properties.maxDescriptorSetUpdateAfterBindInlineUniformBlocks, 25);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestMaintenance3PropertiesKHR) {
#ifdef VK_KHR_maintenance3
    VkPhysicalDeviceMaintenance3PropertiesKHR maintenance_3_properties{};
    maintenance_3_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &maintenance_3_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(maintenance_3_properties.maxPerSetDescriptors, 26);
    EXPECT_EQ(maintenance_3_properties.maxMemoryAllocationSize, 27);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestMaintenance4PropertiesKHR) {
#ifdef VK_KHR_maintenance4
    VkPhysicalDeviceMaintenance4PropertiesKHR maintenance_4_properties{};
    maintenance_4_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &maintenance_4_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(maintenance_4_properties.maxBufferSize, 28);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestFloatControlsPropertiesKHR) {
#ifdef VK_KHR_shader_float_controls
    VkPhysicalDeviceFloatControlsPropertiesKHR float_controls_properties{};
    float_controls_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT_CONTROLS_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &float_controls_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(float_controls_properties.denormBehaviorIndependence, VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_NONE_KHR);
    EXPECT_EQ(float_controls_properties.roundingModeIndependence, VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_32_BIT_ONLY);
    EXPECT_EQ(float_controls_properties.shaderSignedZeroInfNanPreserveFloat16, VK_TRUE);
    EXPECT_EQ(float_controls_properties.shaderSignedZeroInfNanPreserveFloat32, VK_TRUE);
    EXPECT_EQ(float_controls_properties.shaderSignedZeroInfNanPreserveFloat64, VK_TRUE);
    EXPECT_EQ(float_controls_properties.shaderDenormPreserveFloat16, VK_TRUE);
    EXPECT_EQ(float_controls_properties.shaderDenormPreserveFloat32, VK_TRUE);
    EXPECT_EQ(float_controls_properties.shaderDenormPreserveFloat64, VK_TRUE);
    EXPECT_EQ(float_controls_properties.shaderDenormFlushToZeroFloat16, VK_TRUE);
    EXPECT_EQ(float_controls_properties.shaderDenormFlushToZeroFloat32, VK_TRUE);
    EXPECT_EQ(float_controls_properties.shaderDenormFlushToZeroFloat64, VK_TRUE);
    EXPECT_EQ(float_controls_properties.shaderRoundingModeRTEFloat16, VK_TRUE);
    EXPECT_EQ(float_controls_properties.shaderRoundingModeRTEFloat32, VK_TRUE);
    EXPECT_EQ(float_controls_properties.shaderRoundingModeRTEFloat64, VK_TRUE);
    EXPECT_EQ(float_controls_properties.shaderRoundingModeRTZFloat16, VK_TRUE);
    EXPECT_EQ(float_controls_properties.shaderRoundingModeRTZFloat32, VK_TRUE);
    EXPECT_EQ(float_controls_properties.shaderRoundingModeRTZFloat64, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestExternalMemoryHostPropertiesEXT) {
#ifdef VK_EXT_external_memory_host
    VkPhysicalDeviceExternalMemoryHostPropertiesEXT external_memory_host_properties{};
    external_memory_host_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_HOST_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &external_memory_host_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(external_memory_host_properties.minImportedHostPointerAlignment, 31);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestConservativeRasterizationPropertiesEXT) {
#ifdef VK_EXT_conservative_rasterization
    VkPhysicalDeviceConservativeRasterizationPropertiesEXT conservative_rasterization_properties{};
    conservative_rasterization_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONSERVATIVE_RASTERIZATION_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &conservative_rasterization_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(conservative_rasterization_properties.primitiveOverestimationSize, 32.0);
    EXPECT_EQ(conservative_rasterization_properties.maxExtraPrimitiveOverestimationSize, 33.0);
    EXPECT_EQ(conservative_rasterization_properties.extraPrimitiveOverestimationSizeGranularity, 34.0);
    EXPECT_EQ(conservative_rasterization_properties.primitiveUnderestimation, VK_TRUE);
    EXPECT_EQ(conservative_rasterization_properties.conservativePointAndLineRasterization, VK_TRUE);
    EXPECT_EQ(conservative_rasterization_properties.degenerateTrianglesRasterized, VK_TRUE);
    EXPECT_EQ(conservative_rasterization_properties.degenerateLinesRasterized, VK_TRUE);
    EXPECT_EQ(conservative_rasterization_properties.fullyCoveredFragmentShaderInputVariable, VK_TRUE);
    EXPECT_EQ(conservative_rasterization_properties.conservativeRasterizationPostDepthCoverage, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestShaderCorePropertiesAMD) {
#ifdef VK_AMD_shader_core_properties
    VkPhysicalDeviceShaderCorePropertiesAMD shader_core_properties{};
    shader_core_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_AMD;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &shader_core_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(shader_core_properties.shaderEngineCount, 35);
    EXPECT_EQ(shader_core_properties.shaderArraysPerEngineCount, 36);
    EXPECT_EQ(shader_core_properties.computeUnitsPerShaderArray, 37);
    EXPECT_EQ(shader_core_properties.simdPerComputeUnit, 38);
    EXPECT_EQ(shader_core_properties.wavefrontsPerSimd, 39);
    EXPECT_EQ(shader_core_properties.wavefrontSize, 40);
    EXPECT_EQ(shader_core_properties.sgprsPerSimd, 41);
    EXPECT_EQ(shader_core_properties.minSgprAllocation, 42);
    EXPECT_EQ(shader_core_properties.maxSgprAllocation, 43);
    EXPECT_EQ(shader_core_properties.sgprAllocationGranularity, 44);
    EXPECT_EQ(shader_core_properties.vgprsPerSimd, 45);
    EXPECT_EQ(shader_core_properties.minVgprAllocation, 46);
    EXPECT_EQ(shader_core_properties.maxVgprAllocation, 47);
    EXPECT_EQ(shader_core_properties.vgprAllocationGranularity, 48);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestShaderCoreProperties2AMD) {
#ifdef VK_AMD_shader_core_properties2
    VkPhysicalDeviceShaderCoreProperties2AMD shader_core_properties_2{};
    shader_core_properties_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_2_AMD;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &shader_core_properties_2;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(shader_core_properties_2.activeComputeUnitCount, 49);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestDescriptorIndexingPropertiesEXT) {
#ifdef VK_EXT_descriptor_indexing
    VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptor_indexing_properties{};
    descriptor_indexing_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &descriptor_indexing_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(descriptor_indexing_properties.maxUpdateAfterBindDescriptorsInAllPools, 50);
    EXPECT_EQ(descriptor_indexing_properties.shaderUniformBufferArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_properties.shaderSampledImageArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_properties.shaderStorageBufferArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_properties.shaderStorageImageArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_properties.shaderInputAttachmentArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_properties.robustBufferAccessUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_properties.quadDivergentImplicitLod, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_properties.maxPerStageDescriptorUpdateAfterBindSamplers, 51);
    EXPECT_EQ(descriptor_indexing_properties.maxPerStageDescriptorUpdateAfterBindUniformBuffers, 52);
    EXPECT_EQ(descriptor_indexing_properties.maxPerStageDescriptorUpdateAfterBindStorageBuffers, 53);
    EXPECT_EQ(descriptor_indexing_properties.maxPerStageDescriptorUpdateAfterBindSampledImages, 54);
    EXPECT_EQ(descriptor_indexing_properties.maxPerStageDescriptorUpdateAfterBindStorageImages, 55);
    EXPECT_EQ(descriptor_indexing_properties.maxPerStageDescriptorUpdateAfterBindInputAttachments, 56);
    EXPECT_EQ(descriptor_indexing_properties.maxPerStageUpdateAfterBindResources, 57);
    EXPECT_EQ(descriptor_indexing_properties.maxDescriptorSetUpdateAfterBindSamplers, 58);
    EXPECT_EQ(descriptor_indexing_properties.maxDescriptorSetUpdateAfterBindUniformBuffers, 59);
    EXPECT_EQ(descriptor_indexing_properties.maxDescriptorSetUpdateAfterBindUniformBuffersDynamic, 60);
    EXPECT_EQ(descriptor_indexing_properties.maxDescriptorSetUpdateAfterBindStorageBuffers, 61);
    EXPECT_EQ(descriptor_indexing_properties.maxDescriptorSetUpdateAfterBindStorageBuffersDynamic, 62);
    EXPECT_EQ(descriptor_indexing_properties.maxDescriptorSetUpdateAfterBindSampledImages, 63);
    EXPECT_EQ(descriptor_indexing_properties.maxDescriptorSetUpdateAfterBindStorageImages, 64);
    EXPECT_EQ(descriptor_indexing_properties.maxDescriptorSetUpdateAfterBindInputAttachments, 65);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestTimelineSemaphorePropertiesKHR) {
#ifdef VK_KHR_timeline_semaphore
    VkPhysicalDeviceTimelineSemaphorePropertiesKHR timeline_semaphore_properties{};
    timeline_semaphore_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &timeline_semaphore_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(timeline_semaphore_properties.maxTimelineSemaphoreValueDifference, 66);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestVertexAttributeDivisorPropertiesEXT) {
#ifdef VK_EXT_vertex_attribute_divisor
    VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT vertex_attribute_divisor_properties{};
    vertex_attribute_divisor_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &vertex_attribute_divisor_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(vertex_attribute_divisor_properties.maxVertexAttribDivisor, 67);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestDepthStencilResolvePropertiesKHR) {
#ifdef VK_KHR_depth_stencil_resolve
    VkPhysicalDeviceDepthStencilResolvePropertiesKHR depth_stencil_resolve_properties{};
    depth_stencil_resolve_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &depth_stencil_resolve_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(depth_stencil_resolve_properties.supportedDepthResolveModes, VK_RESOLVE_MODE_MIN_BIT_KHR);
    EXPECT_EQ(depth_stencil_resolve_properties.supportedStencilResolveModes, VK_RESOLVE_MODE_MAX_BIT_KHR);
    EXPECT_EQ(depth_stencil_resolve_properties.independentResolveNone, VK_TRUE);
    EXPECT_EQ(depth_stencil_resolve_properties.independentResolve, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestTransformFeedbackPropertiesEXT) {
#ifdef VK_EXT_transform_feedback
    VkPhysicalDeviceTransformFeedbackPropertiesEXT transform_feedback_properties{};
    transform_feedback_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &transform_feedback_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(transform_feedback_properties.maxTransformFeedbackStreams, 70);
    EXPECT_EQ(transform_feedback_properties.maxTransformFeedbackBuffers, 71);
    EXPECT_EQ(transform_feedback_properties.maxTransformFeedbackBufferSize, 72);
    EXPECT_EQ(transform_feedback_properties.maxTransformFeedbackStreamDataSize, 73);
    EXPECT_EQ(transform_feedback_properties.maxTransformFeedbackBufferDataSize, 74);
    EXPECT_EQ(transform_feedback_properties.maxTransformFeedbackBufferDataStride, 75);
    EXPECT_EQ(transform_feedback_properties.transformFeedbackQueries, VK_TRUE);
    EXPECT_EQ(transform_feedback_properties.transformFeedbackStreamsLinesTriangles, VK_TRUE);
    EXPECT_EQ(transform_feedback_properties.transformFeedbackRasterizationStreamSelect, VK_TRUE);
    EXPECT_EQ(transform_feedback_properties.transformFeedbackDraw, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestShadingRateImagePropertiesNV) {
#ifdef VK_NV_shading_rate_image
    VkPhysicalDeviceShadingRateImagePropertiesNV shading_rate_image_properties{};
    shading_rate_image_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &shading_rate_image_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(shading_rate_image_properties.shadingRateTexelSize.width, 76);
    EXPECT_EQ(shading_rate_image_properties.shadingRateTexelSize.height, 77);
    EXPECT_EQ(shading_rate_image_properties.shadingRatePaletteSize, 78);
    EXPECT_EQ(shading_rate_image_properties.shadingRateMaxCoarseSamples, 79);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestMeshShaderPropertiesNV) {
#ifdef VK_NV_mesh_shader
    VkPhysicalDeviceMeshShaderPropertiesNV mesh_shader_properties{};
    mesh_shader_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &mesh_shader_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(mesh_shader_properties.maxDrawMeshTasksCount, 80);
    EXPECT_EQ(mesh_shader_properties.maxTaskWorkGroupInvocations, 81);
    EXPECT_EQ(mesh_shader_properties.maxTaskWorkGroupSize[0], 82);
    EXPECT_EQ(mesh_shader_properties.maxTaskWorkGroupSize[1], 82);
    EXPECT_EQ(mesh_shader_properties.maxTaskWorkGroupSize[2], 82);
    EXPECT_EQ(mesh_shader_properties.maxTaskTotalMemorySize, 83);
    EXPECT_EQ(mesh_shader_properties.maxTaskOutputCount, 84);
    EXPECT_EQ(mesh_shader_properties.maxMeshWorkGroupInvocations, 85);
    EXPECT_EQ(mesh_shader_properties.maxMeshWorkGroupSize[0], 86);
    EXPECT_EQ(mesh_shader_properties.maxMeshWorkGroupSize[1], 86);
    EXPECT_EQ(mesh_shader_properties.maxMeshWorkGroupSize[2], 86);
    EXPECT_EQ(mesh_shader_properties.maxMeshTotalMemorySize, 87);
    EXPECT_EQ(mesh_shader_properties.maxMeshOutputVertices, 88);
    EXPECT_EQ(mesh_shader_properties.maxMeshOutputPrimitives, 89);
    EXPECT_EQ(mesh_shader_properties.maxMeshMultiviewViewCount, 90);
    EXPECT_EQ(mesh_shader_properties.meshOutputPerVertexGranularity, 91);
    EXPECT_EQ(mesh_shader_properties.meshOutputPerPrimitiveGranularity, 92);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestAccelerationStructurePropertiesKHR) {
#ifdef VK_KHR_acceleration_structure
    VkPhysicalDeviceAccelerationStructurePropertiesKHR acceleration_structure_properties{};
    acceleration_structure_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &acceleration_structure_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(acceleration_structure_properties.maxGeometryCount, 93);
    EXPECT_EQ(acceleration_structure_properties.maxInstanceCount, 94);
    EXPECT_EQ(acceleration_structure_properties.maxPrimitiveCount, 95);
    EXPECT_EQ(acceleration_structure_properties.maxPerStageDescriptorAccelerationStructures, 96);
    EXPECT_EQ(acceleration_structure_properties.maxPerStageDescriptorUpdateAfterBindAccelerationStructures, 97);
    EXPECT_EQ(acceleration_structure_properties.maxDescriptorSetAccelerationStructures, 98);
    EXPECT_EQ(acceleration_structure_properties.maxDescriptorSetUpdateAfterBindAccelerationStructures, 99);
    EXPECT_EQ(acceleration_structure_properties.minAccelerationStructureScratchOffsetAlignment, 100);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestRayTracingPipelinePropertiesKHR) {
#ifdef VK_KHR_ray_tracing_pipeline
    VkPhysicalDeviceRayTracingPipelinePropertiesKHR ray_tracing_pipeline_properties{};
    ray_tracing_pipeline_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &ray_tracing_pipeline_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(ray_tracing_pipeline_properties.shaderGroupHandleSize, 101);
    EXPECT_EQ(ray_tracing_pipeline_properties.maxRayRecursionDepth, 102);
    EXPECT_EQ(ray_tracing_pipeline_properties.maxShaderGroupStride, 103);
    EXPECT_EQ(ray_tracing_pipeline_properties.shaderGroupBaseAlignment, 104);
    EXPECT_EQ(ray_tracing_pipeline_properties.shaderGroupHandleCaptureReplaySize, 105);
    EXPECT_EQ(ray_tracing_pipeline_properties.maxRayDispatchInvocationCount, 106);
    EXPECT_EQ(ray_tracing_pipeline_properties.shaderGroupHandleAlignment, 107);
    EXPECT_EQ(ray_tracing_pipeline_properties.maxRayHitAttributeSize, 108);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestRayTracingPropertiesNV) {
#ifdef VK_NV_ray_tracing
    VkPhysicalDeviceRayTracingPropertiesNV ray_tracing_properties{};
    ray_tracing_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &ray_tracing_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(ray_tracing_properties.shaderGroupHandleSize, 109);
    EXPECT_EQ(ray_tracing_properties.maxRecursionDepth, 110);
    EXPECT_EQ(ray_tracing_properties.maxShaderGroupStride, 111);
    EXPECT_EQ(ray_tracing_properties.shaderGroupBaseAlignment, 112);
    EXPECT_EQ(ray_tracing_properties.maxGeometryCount, 113);
    EXPECT_EQ(ray_tracing_properties.maxInstanceCount, 114);
    EXPECT_EQ(ray_tracing_properties.maxTriangleCount, 115);
    EXPECT_EQ(ray_tracing_properties.maxDescriptorSetAccelerationStructures, 116);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestFragmentDensityMapPropertiesEXT) {
#ifdef VK_EXT_fragment_density_map
    VkPhysicalDeviceFragmentDensityMapPropertiesEXT fragment_density_map_properties{};
    fragment_density_map_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &fragment_density_map_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(fragment_density_map_properties.minFragmentDensityTexelSize.width, 117);
    EXPECT_EQ(fragment_density_map_properties.minFragmentDensityTexelSize.height, 118);
    EXPECT_EQ(fragment_density_map_properties.maxFragmentDensityTexelSize.width, 119);
    EXPECT_EQ(fragment_density_map_properties.maxFragmentDensityTexelSize.height, 120);
    EXPECT_EQ(fragment_density_map_properties.fragmentDensityInvocations, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestFragmentDensityMap2PropertiesEXT) {
#ifdef VK_EXT_fragment_density_map2
    VkPhysicalDeviceFragmentDensityMap2PropertiesEXT fragment_density_map_2_properties{};
    fragment_density_map_2_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &fragment_density_map_2_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(fragment_density_map_2_properties.subsampledLoads, VK_TRUE);
    EXPECT_EQ(fragment_density_map_2_properties.subsampledCoarseReconstructionEarlyAccess, VK_TRUE);
    EXPECT_EQ(fragment_density_map_2_properties.maxSubsampledArrayLayers, 121);
    EXPECT_EQ(fragment_density_map_2_properties.maxDescriptorSetSubsampledSamplers, 122);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestFragmentDensityMapOffsetPropertiesQCOM) {
#ifdef VK_QCOM_fragment_density_map_offset
    VkPhysicalDeviceFragmentDensityMapOffsetPropertiesQCOM fragment_density_map_offset_properties{};
    fragment_density_map_offset_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_OFFSET_PROPERTIES_QCOM;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &fragment_density_map_offset_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(fragment_density_map_offset_properties.fragmentDensityOffsetGranularity.width, 123);
    EXPECT_EQ(fragment_density_map_offset_properties.fragmentDensityOffsetGranularity.height, 124);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestCooperativeMatrixPropertiesNV) {
#ifdef VK_NV_cooperative_matrix
    VkPhysicalDeviceCooperativeMatrixPropertiesNV cooperative_matrix_properties{};
    cooperative_matrix_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &cooperative_matrix_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(cooperative_matrix_properties.cooperativeMatrixSupportedStages, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestPerformanceQueryPropertiesKHR) {
#ifdef VK_KHR_performance_query
    VkPhysicalDevicePerformanceQueryPropertiesKHR performance_query_properties{};
    performance_query_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &performance_query_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(performance_query_properties.allowCommandBufferQueryCopies, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestShaderSMBuiltinsPropertiesNV) {
#ifdef VK_NV_shader_sm_builtins
    VkPhysicalDeviceShaderSMBuiltinsPropertiesNV shader_smbuiltins_properties{};
    shader_smbuiltins_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &shader_smbuiltins_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(shader_smbuiltins_properties.shaderSMCount, 126);
    EXPECT_EQ(shader_smbuiltins_properties.shaderWarpsPerSM, 127);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestTexelBufferAlignmentPropertiesEXT) {
#ifdef VK_EXT_texel_buffer_alignment
    VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT texel_buffer_alignment_properties{};
    texel_buffer_alignment_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &texel_buffer_alignment_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(texel_buffer_alignment_properties.storageTexelBufferOffsetAlignmentBytes, 128);
    EXPECT_EQ(texel_buffer_alignment_properties.storageTexelBufferOffsetSingleTexelAlignment, VK_TRUE);
    EXPECT_EQ(texel_buffer_alignment_properties.uniformTexelBufferOffsetAlignmentBytes, 129);
    EXPECT_EQ(texel_buffer_alignment_properties.uniformTexelBufferOffsetSingleTexelAlignment, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestSubgroupSizeControlPropertiesEXT) {
#ifdef VK_EXT_subgroup_size_control
    VkPhysicalDeviceSubgroupSizeControlPropertiesEXT subgroup_size_control_properties{};
    subgroup_size_control_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &subgroup_size_control_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(subgroup_size_control_properties.minSubgroupSize, 130);
    EXPECT_EQ(subgroup_size_control_properties.maxSubgroupSize, 131);
    EXPECT_EQ(subgroup_size_control_properties.maxComputeWorkgroupSubgroups, 132);
    EXPECT_EQ(subgroup_size_control_properties.requiredSubgroupSizeStages, VK_SHADER_STAGE_ANY_HIT_BIT_NV);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestSubpassShadingPropertiesHUAWEI) {
#ifdef VK_HUAWEI_subpass_shading
    VkPhysicalDeviceSubpassShadingPropertiesHUAWEI subpass_shading_properties{};
    subpass_shading_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_SHADING_PROPERTIES_HUAWEI;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &subpass_shading_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(subpass_shading_properties.maxSubpassShadingWorkgroupSizeAspectRatio, 134);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestLineRasterizationPropertiesEXT) {
#ifdef VK_EXT_line_rasterization
    VkPhysicalDeviceLineRasterizationPropertiesEXT line_rasterization_properties{};
    line_rasterization_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &line_rasterization_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(line_rasterization_properties.lineSubPixelPrecisionBits, 135);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestCustomBorderColorPropertiesEXT) {
#ifdef VK_EXT_custom_border_color
    VkPhysicalDeviceCustomBorderColorPropertiesEXT custom_border_color_properties{};
    custom_border_color_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &custom_border_color_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(custom_border_color_properties.maxCustomBorderColorSamplers, 136);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestRobustness2PropertiesEXT) {
#ifdef VK_EXT_robustness2
    VkPhysicalDeviceRobustness2PropertiesEXT robustness_2_properties{};
    robustness_2_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &robustness_2_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(robustness_2_properties.robustStorageBufferAccessSizeAlignment, 137);
    EXPECT_EQ(robustness_2_properties.robustUniformBufferAccessSizeAlignment, 138);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestPortabilitySubsetPropertiesKHR) {
#ifdef VK_KHR_portability_subset
    VkPhysicalDevicePortabilitySubsetPropertiesKHR portability_subset_properties{};
    portability_subset_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &portability_subset_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(portability_subset_properties.minVertexInputBindingStrideAlignment, 139);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestFragmentShadingRatePropertiesKHR) {
#ifdef VK_KHR_fragment_shading_rate
    VkPhysicalDeviceFragmentShadingRatePropertiesKHR fragment_shading_rate_properties{};
    fragment_shading_rate_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &fragment_shading_rate_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(fragment_shading_rate_properties.minFragmentShadingRateAttachmentTexelSize.width, 140);
    EXPECT_EQ(fragment_shading_rate_properties.minFragmentShadingRateAttachmentTexelSize.height, 141);
    EXPECT_EQ(fragment_shading_rate_properties.maxFragmentShadingRateAttachmentTexelSize.width, 142);
    EXPECT_EQ(fragment_shading_rate_properties.maxFragmentShadingRateAttachmentTexelSize.height, 143);
    EXPECT_EQ(fragment_shading_rate_properties.maxFragmentShadingRateAttachmentTexelSizeAspectRatio, 144);
    EXPECT_EQ(fragment_shading_rate_properties.primitiveFragmentShadingRateWithMultipleViewports, VK_TRUE);
    EXPECT_EQ(fragment_shading_rate_properties.layeredShadingRateAttachments, VK_TRUE);
    EXPECT_EQ(fragment_shading_rate_properties.fragmentShadingRateNonTrivialCombinerOps, VK_TRUE);
    EXPECT_EQ(fragment_shading_rate_properties.maxFragmentSize.width, 145);
    EXPECT_EQ(fragment_shading_rate_properties.maxFragmentSize.height, 146);
    EXPECT_EQ(fragment_shading_rate_properties.maxFragmentSizeAspectRatio, 147);
    EXPECT_EQ(fragment_shading_rate_properties.maxFragmentShadingRateCoverageSamples, 148);
    EXPECT_EQ(fragment_shading_rate_properties.maxFragmentShadingRateRasterizationSamples, VK_SAMPLE_COUNT_4_BIT);
    EXPECT_EQ(fragment_shading_rate_properties.fragmentShadingRateWithShaderDepthStencilWrites, VK_TRUE);
    EXPECT_EQ(fragment_shading_rate_properties.fragmentShadingRateWithSampleMask, VK_TRUE);
    EXPECT_EQ(fragment_shading_rate_properties.fragmentShadingRateWithShaderSampleMask, VK_TRUE);
    EXPECT_EQ(fragment_shading_rate_properties.fragmentShadingRateWithConservativeRasterization, VK_TRUE);
    EXPECT_EQ(fragment_shading_rate_properties.fragmentShadingRateWithFragmentShaderInterlock, VK_TRUE);
    EXPECT_EQ(fragment_shading_rate_properties.fragmentShadingRateWithCustomSampleLocations, VK_TRUE);
    EXPECT_EQ(fragment_shading_rate_properties.fragmentShadingRateStrictMultiplyCombiner, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestFragmentShadingRateEnumsPropertiesNV) {
#ifdef VK_NV_fragment_shading_rate_enums
    VkPhysicalDeviceFragmentShadingRateEnumsPropertiesNV fragment_shading_rate_enums_properties{};
    fragment_shading_rate_enums_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_ENUMS_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &fragment_shading_rate_enums_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(fragment_shading_rate_enums_properties.maxFragmentShadingRateInvocationCount, VK_SAMPLE_COUNT_8_BIT);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestProvokingVertexPropertiesEXT) {
#ifdef VK_EXT_provoking_vertex
    VkPhysicalDeviceProvokingVertexPropertiesEXT provoking_vertex_properties{};
    provoking_vertex_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &provoking_vertex_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(provoking_vertex_properties.provokingVertexModePerPipeline, VK_TRUE);
    EXPECT_EQ(provoking_vertex_properties.transformFeedbackPreservesTriangleFanProvokingVertex, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestShaderIntegerDotProductPropertiesKHR) {
#ifdef VK_KHR_shader_integer_dot_product
    VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR shader_integer_dot_product_properties{};
    shader_integer_dot_product_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &shader_integer_dot_product_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProduct8BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProduct8BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProduct8BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProduct4x8BitPackedUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProduct4x8BitPackedSignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProduct4x8BitPackedMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProduct16BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProduct16BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProduct16BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProduct32BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProduct32BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProduct32BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProduct64BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProduct64BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProduct64BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating8BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating8BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating8BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating4x8BitPackedUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating4x8BitPackedSignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating4x8BitPackedMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating16BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating16BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating16BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating32BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating32BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating32BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating64BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating64BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating64BitMixedSignednessAccelerated, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestFragmentShaderBarycentricPropertiesKHR) {
#ifdef VK_KHR_fragment_shader_barycentric
    VkPhysicalDeviceFragmentShaderBarycentricPropertiesKHR fragment_shader_barycentric_properties{};
    fragment_shader_barycentric_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_BARYCENTRIC_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &fragment_shader_barycentric_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(fragment_shader_barycentric_properties.triStripVertexOrderIndependentOfProvokingVertex, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestGraphicsPipelineLibraryPropertiesEXT) {
#ifdef VK_EXT_graphics_pipeline_library
    VkPhysicalDeviceGraphicsPipelineLibraryPropertiesEXT graphics_pipeline_library_properties{};
    graphics_pipeline_library_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GRAPHICS_PIPELINE_LIBRARY_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &graphics_pipeline_library_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(graphics_pipeline_library_properties.graphicsPipelineLibraryFastLinking, VK_TRUE);
    EXPECT_EQ(graphics_pipeline_library_properties.graphicsPipelineLibraryIndependentInterpolationDecoration, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_UNDEFINED) {
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R4G4_UNORM_PACK8) {
    VkFormat format = VK_FORMAT_R4G4_UNORM_PACK8;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R4G4B4A4_UNORM_PACK16) {
    VkFormat format = VK_FORMAT_R4G4B4A4_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B4G4R4A4_UNORM_PACK16) {
    VkFormat format = VK_FORMAT_B4G4R4A4_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R5G6B5_UNORM_PACK16) {
    VkFormat format = VK_FORMAT_R5G6B5_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B5G6R5_UNORM_PACK16) {
    VkFormat format = VK_FORMAT_B5G6R5_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R5G5B5A1_UNORM_PACK16) {
    VkFormat format = VK_FORMAT_R5G5B5A1_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B5G5R5A1_UNORM_PACK16) {
    VkFormat format = VK_FORMAT_B5G5R5A1_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A1R5G5B5_UNORM_PACK16) {
    VkFormat format = VK_FORMAT_A1R5G5B5_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8_UNORM) {
    VkFormat format = VK_FORMAT_R8_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8_SNORM) {
    VkFormat format = VK_FORMAT_R8_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8_USCALED) {
    VkFormat format = VK_FORMAT_R8_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8_SSCALED) {
    VkFormat format = VK_FORMAT_R8_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8_UINT) {
    VkFormat format = VK_FORMAT_R8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8_SINT) {
    VkFormat format = VK_FORMAT_R8_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_DISJOINT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8_SRGB) {
    VkFormat format = VK_FORMAT_R8_SRGB;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8_UNORM) {
    VkFormat format = VK_FORMAT_R8G8_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8_SNORM) {
    VkFormat format = VK_FORMAT_R8G8_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8_USCALED) {
    VkFormat format = VK_FORMAT_R8G8_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8_SSCALED) {
    VkFormat format = VK_FORMAT_R8G8_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8_UINT) {
    VkFormat format = VK_FORMAT_R8G8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8_SINT) {
    VkFormat format = VK_FORMAT_R8G8_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8_SRGB) {
    VkFormat format = VK_FORMAT_R8G8_SRGB;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8_UNORM) {
    VkFormat format = VK_FORMAT_R8G8B8_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8_SNORM) {
    VkFormat format = VK_FORMAT_R8G8B8_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8_USCALED) {
    VkFormat format = VK_FORMAT_R8G8B8_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8_SSCALED) {
    VkFormat format = VK_FORMAT_R8G8B8_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8_UINT) {
    VkFormat format = VK_FORMAT_R8G8B8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8_SINT) {
    VkFormat format = VK_FORMAT_R8G8B8_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8_SRGB) {
    VkFormat format = VK_FORMAT_R8G8B8_SRGB;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_BLIT_DST_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8_UNORM) {
    VkFormat format = VK_FORMAT_B8G8R8_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8_SNORM) {
    VkFormat format = VK_FORMAT_B8G8R8_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8_USCALED) {
    VkFormat format = VK_FORMAT_B8G8R8_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8_SSCALED) {
    VkFormat format = VK_FORMAT_B8G8R8_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8_UINT) {
    VkFormat format = VK_FORMAT_B8G8R8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8_SINT) {
    VkFormat format = VK_FORMAT_B8G8R8_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8_SRGB) {
    VkFormat format = VK_FORMAT_B8G8R8_SRGB;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8A8_UNORM) {
    VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8A8_SNORM) {
    VkFormat format = VK_FORMAT_R8G8B8A8_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8A8_USCALED) {
    VkFormat format = VK_FORMAT_R8G8B8A8_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8A8_SSCALED) {
    VkFormat format = VK_FORMAT_R8G8B8A8_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8A8_UINT) {
    VkFormat format = VK_FORMAT_R8G8B8A8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8A8_SINT) {
    VkFormat format = VK_FORMAT_R8G8B8A8_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8A8_SRGB) {
    VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8A8_UNORM) {
    VkFormat format = VK_FORMAT_B8G8R8A8_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8A8_SNORM) {
    VkFormat format = VK_FORMAT_B8G8R8A8_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8A8_USCALED) {
    VkFormat format = VK_FORMAT_B8G8R8A8_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_BLIT_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8A8_SSCALED) {
    VkFormat format = VK_FORMAT_B8G8R8A8_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8A8_UINT) {
    VkFormat format = VK_FORMAT_B8G8R8A8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8A8_SINT) {
    VkFormat format = VK_FORMAT_B8G8R8A8_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_BLIT_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8A8_SRGB) {
    VkFormat format = VK_FORMAT_B8G8R8A8_SRGB;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A8B8G8R8_UNORM_PACK32) {
    VkFormat format = VK_FORMAT_A8B8G8R8_UNORM_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A8B8G8R8_SNORM_PACK32) {
    VkFormat format = VK_FORMAT_A8B8G8R8_SNORM_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A8B8G8R8_USCALED_PACK32) {
    VkFormat format = VK_FORMAT_A8B8G8R8_USCALED_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A8B8G8R8_SSCALED_PACK32) {
    VkFormat format = VK_FORMAT_A8B8G8R8_SSCALED_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A8B8G8R8_UINT_PACK32) {
    VkFormat format = VK_FORMAT_A8B8G8R8_UINT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A8B8G8R8_SINT_PACK32) {
    VkFormat format = VK_FORMAT_A8B8G8R8_SINT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A8B8G8R8_SRGB_PACK32) {
    VkFormat format = VK_FORMAT_A8B8G8R8_SRGB_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A2R10G10B10_UNORM_PACK32) {
    VkFormat format = VK_FORMAT_A2R10G10B10_UNORM_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A2R10G10B10_SNORM_PACK32) {
    VkFormat format = VK_FORMAT_A2R10G10B10_SNORM_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A2R10G10B10_USCALED_PACK32) {
    VkFormat format = VK_FORMAT_A2R10G10B10_USCALED_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A2R10G10B10_SSCALED_PACK32) {
    VkFormat format = VK_FORMAT_A2R10G10B10_SSCALED_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A2R10G10B10_UINT_PACK32) {
    VkFormat format = VK_FORMAT_A2R10G10B10_UINT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A2R10G10B10_SINT_PACK32) {
    VkFormat format = VK_FORMAT_A2R10G10B10_SINT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A2B10G10R10_UNORM_PACK32) {
    VkFormat format = VK_FORMAT_A2B10G10R10_UNORM_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A2B10G10R10_SNORM_PACK32) {
    VkFormat format = VK_FORMAT_A2B10G10R10_SNORM_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A2B10G10R10_USCALED_PACK32) {
    VkFormat format = VK_FORMAT_A2B10G10R10_USCALED_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A2B10G10R10_SSCALED_PACK32) {
    VkFormat format = VK_FORMAT_A2B10G10R10_SSCALED_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A2B10G10R10_UINT_PACK32) {
    VkFormat format = VK_FORMAT_A2B10G10R10_UINT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A2B10G10R10_SINT_PACK32) {
    VkFormat format = VK_FORMAT_A2B10G10R10_SINT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16_UNORM) {
    VkFormat format = VK_FORMAT_R16_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16_SNORM) {
    VkFormat format = VK_FORMAT_R16_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16_USCALED) {
    VkFormat format = VK_FORMAT_R16_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16_SSCALED) {
    VkFormat format = VK_FORMAT_R16_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16_UINT) {
    VkFormat format = VK_FORMAT_R16_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16_SINT) {
    VkFormat format = VK_FORMAT_R16_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16_SFLOAT) {
    VkFormat format = VK_FORMAT_R16_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16_UNORM) {
    VkFormat format = VK_FORMAT_R16G16_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_DISJOINT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16_SNORM) {
    VkFormat format = VK_FORMAT_R16G16_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16_USCALED) {
    VkFormat format = VK_FORMAT_R16G16_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16_SSCALED) {
    VkFormat format = VK_FORMAT_R16G16_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16_UINT) {
    VkFormat format = VK_FORMAT_R16G16_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16_SINT) {
    VkFormat format = VK_FORMAT_R16G16_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16_SFLOAT) {
    VkFormat format = VK_FORMAT_R16G16_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16_UNORM) {
    VkFormat format = VK_FORMAT_R16G16B16_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16_SNORM) {
    VkFormat format = VK_FORMAT_R16G16B16_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16_USCALED) {
    VkFormat format = VK_FORMAT_R16G16B16_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16_SSCALED) {
    VkFormat format = VK_FORMAT_R16G16B16_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_BLIT_DST_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16_UINT) {
    VkFormat format = VK_FORMAT_R16G16B16_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16_SINT) {
    VkFormat format = VK_FORMAT_R16G16B16_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16_SFLOAT) {
    VkFormat format = VK_FORMAT_R16G16B16_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16A16_UNORM) {
    VkFormat format = VK_FORMAT_R16G16B16A16_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16A16_SNORM) {
    VkFormat format = VK_FORMAT_R16G16B16A16_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16A16_USCALED) {
    VkFormat format = VK_FORMAT_R16G16B16A16_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16A16_SSCALED) {
    VkFormat format = VK_FORMAT_R16G16B16A16_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16A16_UINT) {
    VkFormat format = VK_FORMAT_R16G16B16A16_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16A16_SINT) {
    VkFormat format = VK_FORMAT_R16G16B16A16_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16A16_SFLOAT) {
    VkFormat format = VK_FORMAT_R16G16B16A16_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R32_UINT) {
    VkFormat format = VK_FORMAT_R32_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R32_SINT) {
    VkFormat format = VK_FORMAT_R32_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R32_SFLOAT) {
    VkFormat format = VK_FORMAT_R32_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R32G32_UINT) {
    VkFormat format = VK_FORMAT_R32G32_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_BLIT_SRC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R32G32_SINT) {
    VkFormat format = VK_FORMAT_R32G32_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R32G32_SFLOAT) {
    VkFormat format = VK_FORMAT_R32G32_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R32G32B32_UINT) {
    VkFormat format = VK_FORMAT_R32G32B32_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R32G32B32_SINT) {
    VkFormat format = VK_FORMAT_R32G32B32_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R32G32B32_SFLOAT) {
    VkFormat format = VK_FORMAT_R32G32B32_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R32G32B32A32_UINT) {
    VkFormat format = VK_FORMAT_R32G32B32A32_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R32G32B32A32_SINT) {
    VkFormat format = VK_FORMAT_R32G32B32A32_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R32G32B32A32_SFLOAT) {
    VkFormat format = VK_FORMAT_R32G32B32A32_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R64_UINT) {
    VkFormat format = VK_FORMAT_R64_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R64_SINT) {
    VkFormat format = VK_FORMAT_R64_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R64_SFLOAT) {
    VkFormat format = VK_FORMAT_R64_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R64G64_UINT) {
    VkFormat format = VK_FORMAT_R64G64_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R64G64_SINT) {
    VkFormat format = VK_FORMAT_R64G64_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R64G64_SFLOAT) {
    VkFormat format = VK_FORMAT_R64G64_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R64G64B64_UINT) {
    VkFormat format = VK_FORMAT_R64G64B64_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R64G64B64_SINT) {
    VkFormat format = VK_FORMAT_R64G64B64_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R64G64B64_SFLOAT) {
    VkFormat format = VK_FORMAT_R64G64B64_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R64G64B64A64_UINT) {
    VkFormat format = VK_FORMAT_R64G64B64A64_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R64G64B64A64_SINT) {
    VkFormat format = VK_FORMAT_R64G64B64A64_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R64G64B64A64_SFLOAT) {
    VkFormat format = VK_FORMAT_R64G64B64A64_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B10G11R11_UFLOAT_PACK32) {
    VkFormat format = VK_FORMAT_B10G11R11_UFLOAT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_DISJOINT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_E5B9G9R9_UFLOAT_PACK32) {
    VkFormat format = VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_D16_UNORM) {
    VkFormat format = VK_FORMAT_D16_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_X8_D24_UNORM_PACK32) {
    VkFormat format = VK_FORMAT_X8_D24_UNORM_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_D32_SFLOAT) {
    VkFormat format = VK_FORMAT_D32_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_S8_UINT) {
    VkFormat format = VK_FORMAT_S8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_D16_UNORM_S8_UINT) {
    VkFormat format = VK_FORMAT_D16_UNORM_S8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_D24_UNORM_S8_UINT) {
    VkFormat format = VK_FORMAT_D24_UNORM_S8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_D32_SFLOAT_S8_UINT) {
    VkFormat format = VK_FORMAT_D32_SFLOAT_S8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC1_RGB_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_BC1_RGB_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC1_RGB_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_BC1_RGB_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC1_RGBA_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC1_RGBA_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC2_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_BC2_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC2_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_BC2_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC3_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_BC3_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC3_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_BC3_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_BLIT_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC4_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_BC4_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC4_SNORM_BLOCK) {
    VkFormat format = VK_FORMAT_BC4_SNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_DST_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC5_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_BC5_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC5_SNORM_BLOCK) {
    VkFormat format = VK_FORMAT_BC5_SNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC6H_UFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_BC6H_UFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC6H_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_BC6H_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC7_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_BC7_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC7_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_BC7_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ETC2_R8G8B8_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ETC2_R8G8B8_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_EAC_R11_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_EAC_R11_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_EAC_R11_SNORM_BLOCK) {
    VkFormat format = VK_FORMAT_EAC_R11_SNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_EAC_R11G11_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_EAC_R11G11_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_EAC_R11G11_SNORM_BLOCK) {
    VkFormat format = VK_FORMAT_EAC_R11G11_SNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_4x4_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_4x4_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_4x4_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_BLIT_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_5x4_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_5x4_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_5x4_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_5x5_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_5x5_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_5x5_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_6x5_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_6x5_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_6x5_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_6x6_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_6x6_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_6x6_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_8x5_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_8x5_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_8x5_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_8x6_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_8x6_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_8x6_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_8x8_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_8x8_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_8x8_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_10x5_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_10x5_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_10x5_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_10x6_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_10x6_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_10x6_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_10x8_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_10x8_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_10x8_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_10x10_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_10x10_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_10x10_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_12x10_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_12x10_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_12x10_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_12x12_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_12x12_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_12x12_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G8B8G8R8_422_UNORM) {
    VkFormat format = VK_FORMAT_G8B8G8R8_422_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_BLIT_DST_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8G8_422_UNORM) {
    VkFormat format = VK_FORMAT_B8G8R8G8_422_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G8_B8_R8_3PLANE_420_UNORM) {
    VkFormat format = VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G8_B8R8_2PLANE_420_UNORM) {
    VkFormat format = VK_FORMAT_G8_B8R8_2PLANE_420_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G8_B8_R8_3PLANE_422_UNORM) {
    VkFormat format = VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G8_B8R8_2PLANE_422_UNORM) {
    VkFormat format = VK_FORMAT_G8_B8R8_2PLANE_422_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G8_B8_R8_3PLANE_444_UNORM) {
    VkFormat format = VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R10X6_UNORM_PACK16) {
    VkFormat format = VK_FORMAT_R10X6_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R10X6G10X6_UNORM_2PACK16) {
    VkFormat format = VK_FORMAT_R10X6G10X6_UNORM_2PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16) {
    VkFormat format = VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16) {
    VkFormat format = VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16) {
    VkFormat format = VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16) {
    VkFormat format = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16) {
    VkFormat format = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16) {
    VkFormat format = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16) {
    VkFormat format = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16) {
    VkFormat format = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R12X4_UNORM_PACK16) {
    VkFormat format = VK_FORMAT_R12X4_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R12X4G12X4_UNORM_2PACK16) {
    VkFormat format = VK_FORMAT_R12X4G12X4_UNORM_2PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16) {
    VkFormat format = VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16) {
    VkFormat format = VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16) {
    VkFormat format = VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16) {
    VkFormat format = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16) {
    VkFormat format = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16) {
    VkFormat format = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16) {
    VkFormat format = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16) {
    VkFormat format = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G16B16G16R16_422_UNORM) {
    VkFormat format = VK_FORMAT_G16B16G16R16_422_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B16G16R16G16_422_UNORM) {
    VkFormat format = VK_FORMAT_B16G16R16G16_422_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G16_B16_R16_3PLANE_420_UNORM) {
    VkFormat format = VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G16_B16R16_2PLANE_420_UNORM) {
    VkFormat format = VK_FORMAT_G16_B16R16_2PLANE_420_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G16_B16_R16_3PLANE_422_UNORM) {
    VkFormat format = VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_BLIT_SRC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G16_B16R16_2PLANE_422_UNORM) {
    VkFormat format = VK_FORMAT_G16_B16R16_2PLANE_422_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G16_B16_R16_3PLANE_444_UNORM) {
    VkFormat format = VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G8_B8R8_2PLANE_444_UNORM) {
    VkFormat format = VK_FORMAT_G8_B8R8_2PLANE_444_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16) {
    VkFormat format = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16) {
    VkFormat format = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G16_B16R16_2PLANE_444_UNORM) {
    VkFormat format = VK_FORMAT_G16_B16R16_2PLANE_444_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A4R4G4B4_UNORM_PACK16) {
    VkFormat format = VK_FORMAT_A4R4G4B4_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A4B4G4R4_UNORM_PACK16) {
    VkFormat format = VK_FORMAT_A4B4G4R4_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_4x4_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_5x4_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_5x5_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_6x5_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_6x6_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_8x5_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_8x6_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_8x8_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_DISJOINT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_10x5_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_10x6_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_10x8_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_10x10_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_12x10_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_12x12_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG) {
    VkFormat format = VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG) {
    VkFormat format = VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG) {
    VkFormat format = VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG) {
    VkFormat format = VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG) {
    VkFormat format = VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG) {
    VkFormat format = VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG) {
    VkFormat format = VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG) {
    VkFormat format = VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

