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

struct TestInit {
    TestInit() : instance(VK_NULL_HANDLE), physical_device(VK_NULL_HANDLE) {}

    ~TestInit() {
        if (this->instance != VK_NULL_HANDLE) {
            vkDestroyInstance(this->instance, nullptr);
            this->instance = VK_NULL_HANDLE;
        }
    }

    void init() {
        VkResult err = VK_SUCCESS;

        const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
        profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

        inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_promoted_api.json";
        settings.profile_name = "VP_LUNARG_test_api";
        settings.simulate_capabilities = SimulateCapabilityFlag::SIMULATE_EXTENSIONS_BIT |
                                         SimulateCapabilityFlag::SIMULATE_FEATURES_BIT |
                                         SimulateCapabilityFlag::SIMULATE_PROPERTIES_BIT;

        err = inst_builder.makeInstance(&settings);
        ASSERT_EQ(err, VK_SUCCESS);

        this->instance = inst_builder.getInstance();

        err = inst_builder.getPhysicalDevice(&this->physical_device);
        ASSERT_EQ(err, VK_SUCCESS);
    }

    VkInstance instance;
    VkPhysicalDevice physical_device;
    profiles_test::VulkanInstanceBuilder inst_builder;
};

static TestInit test;

VkPhysicalDevice GetPhysicalDevice() {
    if (test.instance == VK_NULL_HANDLE) {
        test.init();
    }

    return test.physical_device;
}

bool IsVersionSupported(uint32_t api_version) {
    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    vkGetPhysicalDeviceProperties2(test.physical_device, &gpu_props);
    if (gpu_props.properties.apiVersion < api_version) {
        printf("Profile not supported on device, skipping test.\n");
        return false;
    }
    return true;
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    VkPhysicalDevice gpu = GetPhysicalDevice();
    if (gpu == VK_NULL_HANDLE) return -1;

    int result = RUN_ALL_TESTS();

    if (test.instance != VK_NULL_HANDLE) {
        vkDestroyInstance(test.instance, nullptr);
        test.instance = VK_NULL_HANDLE;
    }

    return result;
}

TEST(layer_promoted, TestVulkan11Properties) {
#ifdef VK_VERSION_1_2
    VkPhysicalDevice gpu = GetPhysicalDevice();
    if (gpu == VK_NULL_HANDLE) return;
    if (!IsVersionSupported(VK_API_VERSION_1_2)) {
        printf("Required API version is not supported on device, skipping test.\n");
        return;
    }

    VkPhysicalDeviceVulkan11Properties vulkan_11_properties{};
    vulkan_11_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &vulkan_11_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(vulkan_11_properties.subgroupSize, 211);
    EXPECT_EQ(vulkan_11_properties.subgroupSupportedStages & VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_VERTEX_BIT);
    EXPECT_EQ(vulkan_11_properties.subgroupSupportedOperations & VK_SUBGROUP_FEATURE_BASIC_BIT, VK_SUBGROUP_FEATURE_BASIC_BIT);
    EXPECT_EQ(vulkan_11_properties.subgroupQuadOperationsInAllStages, VK_TRUE);
    EXPECT_EQ(vulkan_11_properties.maxMultiviewViewCount, 212);
    EXPECT_EQ(vulkan_11_properties.maxMultiviewInstanceIndex, 213);
    EXPECT_EQ(vulkan_11_properties.protectedNoFault, VK_TRUE);
    EXPECT_EQ(vulkan_11_properties.maxPerSetDescriptors, 214);
    EXPECT_EQ(vulkan_11_properties.maxMemoryAllocationSize, 215);
#endif
}

TEST(layer_promoted, TestVulkan11Features) {
#ifdef VK_VERSION_1_2
    VkPhysicalDevice gpu = GetPhysicalDevice();
    if (gpu == VK_NULL_HANDLE) return;
    if (!IsVersionSupported(VK_API_VERSION_1_2)) {
        printf("Required API version is not supported on device, skipping test.\n");
        return;
    }

    VkPhysicalDeviceVulkan11Features vulkan_11_features{};
    vulkan_11_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &vulkan_11_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(vulkan_11_features.storageBuffer16BitAccess, VK_TRUE);
    EXPECT_EQ(vulkan_11_features.uniformAndStorageBuffer16BitAccess, VK_TRUE);
    EXPECT_EQ(vulkan_11_features.storagePushConstant16, VK_TRUE);
    EXPECT_EQ(vulkan_11_features.storageInputOutput16, VK_TRUE);
    EXPECT_EQ(vulkan_11_features.multiview, VK_TRUE);
    EXPECT_EQ(vulkan_11_features.multiviewGeometryShader, VK_TRUE);
    EXPECT_EQ(vulkan_11_features.multiviewTessellationShader, VK_TRUE);
    EXPECT_EQ(vulkan_11_features.variablePointersStorageBuffer, VK_TRUE);
    EXPECT_EQ(vulkan_11_features.variablePointers, VK_TRUE);
    EXPECT_EQ(vulkan_11_features.protectedMemory, VK_TRUE);
    EXPECT_EQ(vulkan_11_features.samplerYcbcrConversion, VK_TRUE);
    EXPECT_EQ(vulkan_11_features.shaderDrawParameters, VK_TRUE);
#endif
}

TEST(layer_promoted, TestVulkan12Properties) {
#ifdef VK_VERSION_1_2
    VkPhysicalDevice gpu = GetPhysicalDevice();
    if (gpu == VK_NULL_HANDLE) return;
    if (!IsVersionSupported(VK_API_VERSION_1_2)) {
        printf("Required API version is not supported on device, skipping test.\n");
        return;
    }

    VkPhysicalDeviceVulkan12Properties vulkan_12_properties{};
    vulkan_12_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &vulkan_12_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(vulkan_12_properties.denormBehaviorIndependence, VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL);
    EXPECT_EQ(vulkan_12_properties.roundingModeIndependence, VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL);
    EXPECT_EQ(vulkan_12_properties.shaderSignedZeroInfNanPreserveFloat16, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderSignedZeroInfNanPreserveFloat32, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderSignedZeroInfNanPreserveFloat64, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderDenormPreserveFloat16, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderDenormPreserveFloat32, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderDenormPreserveFloat64, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderDenormFlushToZeroFloat16, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderDenormFlushToZeroFloat32, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderDenormFlushToZeroFloat64, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderRoundingModeRTEFloat16, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderRoundingModeRTEFloat32, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderRoundingModeRTEFloat64, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderRoundingModeRTZFloat16, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderRoundingModeRTZFloat32, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderRoundingModeRTZFloat64, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.maxUpdateAfterBindDescriptorsInAllPools, 216);
    EXPECT_EQ(vulkan_12_properties.shaderUniformBufferArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderSampledImageArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderStorageBufferArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderStorageImageArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderInputAttachmentArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.robustBufferAccessUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.quadDivergentImplicitLod, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.maxPerStageDescriptorUpdateAfterBindSamplers, 217);
    EXPECT_EQ(vulkan_12_properties.maxPerStageDescriptorUpdateAfterBindUniformBuffers, 218);
    EXPECT_EQ(vulkan_12_properties.maxPerStageDescriptorUpdateAfterBindStorageBuffers, 219);
    EXPECT_EQ(vulkan_12_properties.maxPerStageDescriptorUpdateAfterBindSampledImages, 220);
    EXPECT_EQ(vulkan_12_properties.maxPerStageDescriptorUpdateAfterBindStorageImages, 221);
    EXPECT_EQ(vulkan_12_properties.maxPerStageDescriptorUpdateAfterBindInputAttachments, 222);
    EXPECT_EQ(vulkan_12_properties.maxPerStageUpdateAfterBindResources, 223);
    EXPECT_EQ(vulkan_12_properties.maxDescriptorSetUpdateAfterBindSamplers, 224);
    EXPECT_EQ(vulkan_12_properties.maxDescriptorSetUpdateAfterBindUniformBuffers, 225);
    EXPECT_EQ(vulkan_12_properties.maxDescriptorSetUpdateAfterBindUniformBuffersDynamic, 226);
    EXPECT_EQ(vulkan_12_properties.maxDescriptorSetUpdateAfterBindStorageBuffers, 227);
    EXPECT_EQ(vulkan_12_properties.maxDescriptorSetUpdateAfterBindStorageBuffersDynamic, 228);
    EXPECT_EQ(vulkan_12_properties.maxDescriptorSetUpdateAfterBindSampledImages, 229);
    EXPECT_EQ(vulkan_12_properties.maxDescriptorSetUpdateAfterBindStorageImages, 230);
    EXPECT_EQ(vulkan_12_properties.maxDescriptorSetUpdateAfterBindInputAttachments, 231);
    EXPECT_EQ(vulkan_12_properties.supportedDepthResolveModes & VK_RESOLVE_MODE_AVERAGE_BIT, VK_RESOLVE_MODE_AVERAGE_BIT);
    EXPECT_EQ(vulkan_12_properties.supportedStencilResolveModes & VK_RESOLVE_MODE_AVERAGE_BIT, VK_RESOLVE_MODE_AVERAGE_BIT);
    EXPECT_EQ(vulkan_12_properties.independentResolveNone, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.independentResolve, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.filterMinmaxSingleComponentFormats, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.filterMinmaxImageComponentMapping, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.maxTimelineSemaphoreValueDifference, 232);
    EXPECT_EQ(vulkan_12_properties.framebufferIntegerColorSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
#endif
}

TEST(layer_promoted, TestVulkan12Features) {
#ifdef VK_VERSION_1_2
    VkPhysicalDevice gpu = GetPhysicalDevice();
    if (gpu == VK_NULL_HANDLE) return;
    if (!IsVersionSupported(VK_API_VERSION_1_2)) {
        printf("Required API version is not supported on device, skipping test.\n");
        return;
    }

    VkPhysicalDeviceVulkan12Features vulkan_12_features{};
    vulkan_12_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &vulkan_12_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

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

TEST(layer_promoted, TestVulkan13Properties) {
#ifdef VK_VERSION_1_3
    VkPhysicalDevice gpu = GetPhysicalDevice();
    if (gpu == VK_NULL_HANDLE) return;
    if (!IsVersionSupported(VK_API_VERSION_1_3)) {
        printf("Required API version is not supported on device, skipping test.\n");
        return;
    }

    VkPhysicalDeviceVulkan13Properties vulkan_13_properties{};
    vulkan_13_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &vulkan_13_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(vulkan_13_properties.minSubgroupSize, 338);
    EXPECT_EQ(vulkan_13_properties.maxSubgroupSize, 339);
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
    EXPECT_EQ(vulkan_13_properties.storageTexelBufferOffsetSingleTexelAlignment, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.uniformTexelBufferOffsetAlignmentBytes, 353);
    EXPECT_EQ(vulkan_13_properties.uniformTexelBufferOffsetSingleTexelAlignment, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.maxBufferSize, 260);
#endif
}

TEST(layer_promoted, TestVulkan13Features) {
#ifdef VK_VERSION_1_2
    VkPhysicalDevice gpu = GetPhysicalDevice();
    if (gpu == VK_NULL_HANDLE) return;
    if (!IsVersionSupported(VK_API_VERSION_1_3)) {
        printf("Required API version is not supported on device, skipping test.\n");
        return;
    }

    VkPhysicalDeviceVulkan13Features vulkan_13_features{};
    vulkan_13_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &vulkan_13_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

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

