/*
 * Copyright (C) 2021-2025 Valve Corporation
 * Copyright (C) 2021-2025 LunarG, Inc.
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
 * Authors:
 * - Ziga Markus <ziga@lunarg.com>
 * - Christophe Riccio <christophe@lunarg.com>
 * - Mark Lobodzinski <mark@lunarg.com>
 */

#include <vulkan/vulkan_core.h>

#include <gtest/gtest.h>
#include "profiles_test_helper.h"

static VkPhysicalDevice gpu_profile;
static VkPhysicalDevice gpu_native;
static profiles_test::VulkanInstanceBuilder inst_builder;

class TestsPromoted : public VkTestFramework {
   public:
    TestsPromoted(){};
    ~TestsPromoted(){};

    bool IsVersionSupported(uint32_t api_version) {
        VkPhysicalDeviceProperties2 gpu_props{};
        gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props);
        if (gpu_props.properties.apiVersion < api_version) {
            printf("Profile not supported on device, skipping test.\n");
            return false;
        }
        return true;
    }

    static void SetUpTestSuite() {
        VkResult err = VK_SUCCESS;

        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_promoted_api.json";
        const char* profile_name_data = "VP_LUNARG_test_api";
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_FEATURES_BIT", "SIMULATE_PROPERTIES_BIT",  "SIMULATE_EXTENSIONS_BIT"};

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
            {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
            {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]}
        };

        err = inst_builder.init(settings);
        EXPECT_EQ(VK_SUCCESS, err);

        err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu_profile);
        EXPECT_EQ(VK_SUCCESS, err);
        EXPECT_TRUE(gpu_profile);

        err = inst_builder.getPhysicalDevice(profiles_test::MODE_NATIVE, &gpu_native);
        EXPECT_EQ(VK_SUCCESS, err);
        EXPECT_TRUE(gpu_native);
    };

    static void TearDownTestSuite() { 
        inst_builder.reset(); 
    };
};

TEST_F(TestsPromoted, TestVulkan11Properties) {
#ifdef VK_VERSION_1_2
    if (!IsVersionSupported(VK_API_VERSION_1_2)) {
        printf("Required API version is not supported on device, skipping test.\n");
        return;
    }

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    VkPhysicalDeviceVulkan11Properties properties_profile{};
    properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
    gpu_props.pNext = &properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props);

    VkPhysicalDeviceVulkan11Properties properties_native{}; // For capabilities that can't be modified
    properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
    gpu_props.pNext = &properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props);

    EXPECT_EQ(properties_profile.subgroupSize, 64);
    EXPECT_EQ(properties_profile.subgroupSupportedStages & VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_VERTEX_BIT);
    EXPECT_EQ(properties_profile.subgroupSupportedOperations & VK_SUBGROUP_FEATURE_BASIC_BIT, VK_SUBGROUP_FEATURE_BASIC_BIT);
    EXPECT_EQ(properties_profile.subgroupQuadOperationsInAllStages, VK_TRUE);
    EXPECT_EQ(properties_profile.maxMultiviewViewCount, 212);
    EXPECT_EQ(properties_profile.maxMultiviewInstanceIndex, 213);
    EXPECT_EQ(properties_profile.protectedNoFault, properties_native.protectedNoFault);
    EXPECT_EQ(properties_profile.maxPerSetDescriptors, 214);
    EXPECT_EQ(properties_profile.maxMemoryAllocationSize, 215);
#endif
}

TEST_F(TestsPromoted, TestVulkan11Features) {
#ifdef VK_VERSION_1_2
    if (!IsVersionSupported(VK_API_VERSION_1_2)) {
        printf("Required API version is not supported on device, skipping test.\n");
        return;
    }

    VkPhysicalDeviceFeatures2 gpu_features;
    gpu_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

    VkPhysicalDeviceVulkan11Features profile_features{};
    profile_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;

    gpu_features.pNext = &profile_features;
    vkGetPhysicalDeviceFeatures2(gpu_profile, &gpu_features);

    EXPECT_EQ(profile_features.storageBuffer16BitAccess, VK_TRUE);
    EXPECT_EQ(profile_features.uniformAndStorageBuffer16BitAccess, VK_TRUE);
    EXPECT_EQ(profile_features.storagePushConstant16, VK_TRUE);
    EXPECT_EQ(profile_features.storageInputOutput16, VK_TRUE);
    EXPECT_EQ(profile_features.multiview, VK_TRUE);
    EXPECT_EQ(profile_features.multiviewGeometryShader, VK_TRUE);
    EXPECT_EQ(profile_features.multiviewTessellationShader, VK_TRUE);
    EXPECT_EQ(profile_features.variablePointersStorageBuffer, VK_TRUE);
    EXPECT_EQ(profile_features.variablePointers, VK_TRUE);
    EXPECT_EQ(profile_features.protectedMemory, VK_TRUE);
    EXPECT_EQ(profile_features.samplerYcbcrConversion, VK_TRUE);
    EXPECT_EQ(profile_features.shaderDrawParameters, VK_TRUE);
#endif
}

TEST_F(TestsPromoted, TestVulkan12Properties) {
#ifdef VK_VERSION_1_2
    if (!IsVersionSupported(VK_API_VERSION_1_2)) {
        printf("Required API version is not supported on device, skipping test.\n");
        return;
    }

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    VkPhysicalDeviceVulkan12Properties properties_profile{};
    properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;
    gpu_props.pNext = &properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props);

    VkPhysicalDeviceVulkan12Properties properties_native{};  // For capabilities that can't be modified
    properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;
    gpu_props.pNext = &properties_native;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props);

    EXPECT_EQ(properties_profile.denormBehaviorIndependence, properties_native.denormBehaviorIndependence);
    EXPECT_EQ(properties_profile.roundingModeIndependence, properties_native.roundingModeIndependence);
    EXPECT_EQ(properties_profile.shaderSignedZeroInfNanPreserveFloat16, VK_TRUE);
    EXPECT_EQ(properties_profile.shaderSignedZeroInfNanPreserveFloat32, VK_TRUE);
    EXPECT_EQ(properties_profile.shaderSignedZeroInfNanPreserveFloat64, VK_TRUE);
    EXPECT_EQ(properties_profile.shaderDenormPreserveFloat16, VK_TRUE);
    EXPECT_EQ(properties_profile.shaderDenormPreserveFloat32, VK_TRUE);
    EXPECT_EQ(properties_profile.shaderDenormPreserveFloat64, VK_TRUE);
    EXPECT_EQ(properties_profile.shaderDenormFlushToZeroFloat16, VK_TRUE);
    EXPECT_EQ(properties_profile.shaderDenormFlushToZeroFloat32, VK_TRUE);
    EXPECT_EQ(properties_profile.shaderDenormFlushToZeroFloat64, VK_TRUE);
    EXPECT_EQ(properties_profile.shaderRoundingModeRTEFloat16, VK_TRUE);
    EXPECT_EQ(properties_profile.shaderRoundingModeRTEFloat32, VK_TRUE);
    EXPECT_EQ(properties_profile.shaderRoundingModeRTEFloat64, VK_TRUE);
    EXPECT_EQ(properties_profile.shaderRoundingModeRTZFloat16, VK_TRUE);
    EXPECT_EQ(properties_profile.shaderRoundingModeRTZFloat32, VK_TRUE);
    EXPECT_EQ(properties_profile.shaderRoundingModeRTZFloat64, VK_TRUE);
    EXPECT_EQ(properties_profile.maxUpdateAfterBindDescriptorsInAllPools, 216);
    EXPECT_EQ(properties_profile.shaderUniformBufferArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_EQ(properties_profile.shaderSampledImageArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_EQ(properties_profile.shaderStorageBufferArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_EQ(properties_profile.shaderStorageImageArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_EQ(properties_profile.shaderInputAttachmentArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_EQ(properties_profile.robustBufferAccessUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(properties_profile.quadDivergentImplicitLod, VK_TRUE);
    EXPECT_EQ(properties_profile.maxPerStageDescriptorUpdateAfterBindSamplers, 217);
    EXPECT_EQ(properties_profile.maxPerStageDescriptorUpdateAfterBindUniformBuffers, 218);
    EXPECT_EQ(properties_profile.maxPerStageDescriptorUpdateAfterBindStorageBuffers, 219);
    EXPECT_EQ(properties_profile.maxPerStageDescriptorUpdateAfterBindSampledImages, 220);
    EXPECT_EQ(properties_profile.maxPerStageDescriptorUpdateAfterBindStorageImages, 221);
    EXPECT_EQ(properties_profile.maxPerStageDescriptorUpdateAfterBindInputAttachments, 222);
    EXPECT_EQ(properties_profile.maxPerStageUpdateAfterBindResources, 223);
    EXPECT_EQ(properties_profile.maxDescriptorSetUpdateAfterBindSamplers, 224);
    EXPECT_EQ(properties_profile.maxDescriptorSetUpdateAfterBindUniformBuffers, 225);
    EXPECT_EQ(properties_profile.maxDescriptorSetUpdateAfterBindUniformBuffersDynamic, 226);
    EXPECT_EQ(properties_profile.maxDescriptorSetUpdateAfterBindStorageBuffers, 227);
    EXPECT_EQ(properties_profile.maxDescriptorSetUpdateAfterBindStorageBuffersDynamic, 228);
    EXPECT_EQ(properties_profile.maxDescriptorSetUpdateAfterBindSampledImages, 229);
    EXPECT_EQ(properties_profile.maxDescriptorSetUpdateAfterBindStorageImages, 230);
    EXPECT_EQ(properties_profile.maxDescriptorSetUpdateAfterBindInputAttachments, 231);
    EXPECT_EQ(properties_profile.supportedDepthResolveModes & VK_RESOLVE_MODE_AVERAGE_BIT, VK_RESOLVE_MODE_AVERAGE_BIT);
    EXPECT_EQ(properties_profile.supportedStencilResolveModes & VK_RESOLVE_MODE_AVERAGE_BIT, VK_RESOLVE_MODE_AVERAGE_BIT);
    EXPECT_EQ(properties_profile.independentResolveNone, VK_TRUE);
    EXPECT_EQ(properties_profile.independentResolve, VK_TRUE);
    EXPECT_EQ(properties_profile.filterMinmaxSingleComponentFormats, VK_TRUE);
    EXPECT_EQ(properties_profile.filterMinmaxImageComponentMapping, VK_TRUE);
    EXPECT_EQ(properties_profile.maxTimelineSemaphoreValueDifference, 232);
    EXPECT_EQ(properties_profile.framebufferIntegerColorSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
#endif
}

TEST_F(TestsPromoted, TestVulkan12Features) {
#ifdef VK_VERSION_1_2
    if (!IsVersionSupported(VK_API_VERSION_1_2)) {
        printf("Required API version is not supported on device, skipping test.\n");
        return;
    }

    VkPhysicalDeviceVulkan12Features vulkan_12_features{};
    vulkan_12_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &vulkan_12_features;
    vkGetPhysicalDeviceFeatures2(gpu_profile, &features);

    EXPECT_EQ(vulkan_12_features.samplerMirrorClampToEdge, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.drawIndirectCount, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.storageBuffer8BitAccess, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.uniformAndStorageBuffer8BitAccess, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.storagePushConstant8, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderBufferInt64Atomics, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderSharedInt64Atomics, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderFloat16, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderInt8, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.descriptorIndexing, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderInputAttachmentArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderUniformTexelBufferArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderStorageTexelBufferArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderUniformBufferArrayNonUniformIndexing, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderSampledImageArrayNonUniformIndexing, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderStorageBufferArrayNonUniformIndexing, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderStorageImageArrayNonUniformIndexing, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderInputAttachmentArrayNonUniformIndexing, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderUniformTexelBufferArrayNonUniformIndexing, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderStorageTexelBufferArrayNonUniformIndexing, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.descriptorBindingUniformBufferUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.descriptorBindingSampledImageUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.descriptorBindingStorageImageUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.descriptorBindingStorageBufferUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.descriptorBindingUniformTexelBufferUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.descriptorBindingStorageTexelBufferUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.descriptorBindingUpdateUnusedWhilePending, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.descriptorBindingPartiallyBound, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.descriptorBindingVariableDescriptorCount, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.runtimeDescriptorArray, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.samplerFilterMinmax, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.scalarBlockLayout, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.imagelessFramebuffer, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.uniformBufferStandardLayout, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderSubgroupExtendedTypes, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.separateDepthStencilLayouts, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.hostQueryReset, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.timelineSemaphore, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.bufferDeviceAddress, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.bufferDeviceAddressCaptureReplay, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.bufferDeviceAddressMultiDevice, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.vulkanMemoryModel, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.vulkanMemoryModelDeviceScope, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.vulkanMemoryModelAvailabilityVisibilityChains, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderOutputViewportIndex, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderOutputLayer, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.subgroupBroadcastDynamicId, VK_TRUE);
#endif
}

TEST_F(TestsPromoted, TestVulkan13Properties) {
#ifdef VK_VERSION_1_3
    if (!IsVersionSupported(VK_API_VERSION_1_3)) {
        printf("Required API version is not supported on device, skipping test.\n");
        return;
    }

    VkPhysicalDeviceVulkan13Properties vulkan_13_properties{};
    vulkan_13_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &vulkan_13_properties;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props);

    EXPECT_EQ(vulkan_13_properties.minSubgroupSize, 32);
    EXPECT_EQ(vulkan_13_properties.maxSubgroupSize, 32);
    EXPECT_EQ(vulkan_13_properties.maxComputeWorkgroupSubgroups, 340);
    EXPECT_EQ(vulkan_13_properties.requiredSubgroupSizeStages & (VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
              VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
    EXPECT_EQ(vulkan_13_properties.maxInlineUniformBlockSize, 271);
    EXPECT_EQ(vulkan_13_properties.maxPerStageDescriptorInlineUniformBlocks, 272);
    EXPECT_EQ(vulkan_13_properties.maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks, 273);
    EXPECT_EQ(vulkan_13_properties.maxDescriptorSetInlineUniformBlocks, 274);
    EXPECT_EQ(vulkan_13_properties.maxDescriptorSetUpdateAfterBindInlineUniformBlocks, 275);
    EXPECT_EQ(vulkan_13_properties.maxInlineUniformTotalSize, 76);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct8BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct8BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct8BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct4x8BitPackedUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct4x8BitPackedSignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct4x8BitPackedMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct16BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct16BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct16BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct32BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct32BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct32BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct64BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct64BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct64BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating8BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating8BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating8BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating4x8BitPackedUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating4x8BitPackedSignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating4x8BitPackedMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating16BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating16BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating16BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating32BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating32BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating32BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating64BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating64BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating64BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.storageTexelBufferOffsetAlignmentBytes, 352);
    EXPECT_EQ(vulkan_13_properties.uniformTexelBufferOffsetAlignmentBytes, 353);
    EXPECT_EQ(vulkan_13_properties.maxBufferSize, 260);
#endif
}

TEST_F(TestsPromoted, TestVulkan13Features) {
#ifdef VK_VERSION_1_2
    if (!IsVersionSupported(VK_API_VERSION_1_3)) {
        printf("Required API version is not supported on device, skipping test.\n");
        return;
    }

    VkPhysicalDeviceVulkan13Features vulkan_13_features{};
    vulkan_13_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &vulkan_13_features;
    vkGetPhysicalDeviceFeatures2(gpu_profile, &features);

    EXPECT_EQ(vulkan_13_features.robustImageAccess, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.inlineUniformBlock, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.descriptorBindingInlineUniformBlockUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.pipelineCreationCacheControl, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.privateData, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.shaderDemoteToHelperInvocation, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.shaderTerminateInvocation, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.subgroupSizeControl, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.computeFullSubgroups, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.synchronization2, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.textureCompressionASTC_HDR, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.shaderZeroInitializeWorkgroupMemory, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.dynamicRendering, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.shaderIntegerDotProduct, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.maintenance4, VK_TRUE);
#endif
}
