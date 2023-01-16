/*
 * Copyright (C) 2021-2023 Valve Corporation
 * Copyright (C) 2021-2023 LunarG, Inc.
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

static VkPhysicalDevice gpu_native = VK_NULL_HANDLE;
static VkPhysicalDevice gpu_profile = VK_NULL_HANDLE;
static profiles_test::VulkanInstanceBuilder inst_builder;

class TestsUnion : public VkTestFramework {
   public:
    TestsUnion(){};
    ~TestsUnion(){};

    static void SetUpTestSuite(){
        VkResult err = VK_SUCCESS;

        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_combine_union.json";
        settings.emulate_portability = true;
        settings.profile_name = "VP_LUNARG_test_combine_union";
        settings.simulate_capabilities = SIMULATE_MAX_ENUM;
        settings.debug_reports =
            DEBUG_REPORT_ERROR_BIT | DEBUG_REPORT_WARNING_BIT | DEBUG_REPORT_NOTIFICATION_BIT | DEBUG_REPORT_DEBUG_BIT;

        err = inst_builder.init(&settings);
        ASSERT_EQ(err, VK_SUCCESS);

        err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu_profile);
        ASSERT_EQ(err, VK_SUCCESS);

        err = inst_builder.getPhysicalDevice(profiles_test::MODE_NATIVE, &gpu_native);
        ASSERT_EQ(err, VK_SUCCESS);
    }

    static void TearDownTestSuite(){ 
        inst_builder.reset();
    };
};

TEST_F(TestsUnion, Extension) {
    uint32_t count;
    vkEnumerateDeviceExtensionProperties(gpu_profile, nullptr, &count, nullptr);
    std::vector<VkExtensionProperties> device_extensions(count);
    vkEnumerateDeviceExtensionProperties(gpu_profile, nullptr, &count, device_extensions.data());

    ASSERT_STREQ("VK_AMD_device_coherent_memory", device_extensions[0].extensionName);
    ASSERT_STREQ("VK_EXT_extended_dynamic_state", device_extensions[1].extensionName);
    ASSERT_STREQ("VK_EXT_shader_demote_to_helper_invocation", device_extensions[2].extensionName);
    ASSERT_STREQ("VK_KHR_maintenance1", device_extensions[3].extensionName);
    ASSERT_STREQ("VK_KHR_maintenance2", device_extensions[4].extensionName);
    ASSERT_STREQ("VK_KHR_maintenance3", device_extensions[5].extensionName);
    ASSERT_STREQ("VK_KHR_variable_pointers", device_extensions[6].extensionName);
    ASSERT_STREQ("VK_KHR_portability_subset", device_extensions[7].extensionName);
}

TEST_F(TestsUnion, Feature) {
    VkPhysicalDeviceFeatures gpu_features{};
    vkGetPhysicalDeviceFeatures(gpu_profile, &gpu_features);

    EXPECT_EQ(gpu_features.depthBiasClamp, VK_TRUE);
    EXPECT_EQ(gpu_features.depthClamp, VK_FALSE);
    EXPECT_EQ(gpu_features.drawIndirectFirstInstance, VK_TRUE);
    EXPECT_EQ(gpu_features.multiDrawIndirect, VK_TRUE);
    EXPECT_EQ(gpu_features.sampleRateShading, VK_TRUE);
    EXPECT_EQ(gpu_features.shaderClipDistance, VK_FALSE);
    EXPECT_EQ(gpu_features.samplerAnisotropy, VK_TRUE);
    EXPECT_EQ(gpu_features.multiViewport, VK_FALSE);
}

TEST_F(TestsUnion, Properties) {
    VkPhysicalDeviceProperties gpu_props{};
    vkGetPhysicalDeviceProperties(gpu_profile, &gpu_props);

    EXPECT_EQ(gpu_props.limits.maxImageDimension1D, 16384u);
    EXPECT_EQ(gpu_props.limits.maxImageDimension2D, 8192u);
    EXPECT_EQ(gpu_props.limits.maxComputeWorkGroupCount[0], 65535u);
    EXPECT_EQ(gpu_props.limits.maxComputeWorkGroupCount[1], 65535u);
    EXPECT_EQ(gpu_props.limits.maxComputeWorkGroupCount[2], 4096u);
    EXPECT_EQ(gpu_props.limits.mipmapPrecisionBits, 4u);
    EXPECT_TRUE(std::abs(gpu_props.limits.maxSamplerAnisotropy - 16.0) < 0.0001);
    EXPECT_EQ(gpu_props.limits.viewportBoundsRange[0], -32768);
    EXPECT_EQ(gpu_props.limits.viewportBoundsRange[1], 32767);
    EXPECT_EQ(gpu_props.limits.minMemoryMapAlignment, 2048);
    EXPECT_EQ(gpu_props.limits.minTexelOffset, -8);
    EXPECT_EQ(gpu_props.limits.maxTexelOffset, 7u);
    EXPECT_EQ(gpu_props.limits.framebufferColorSampleCounts,
              static_cast<VkSampleCountFlags>(VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_2_BIT | VK_SAMPLE_COUNT_4_BIT));
    EXPECT_EQ(gpu_props.limits.pointSizeRange[0], 2.0);
    EXPECT_EQ(gpu_props.limits.pointSizeRange[1], 64.0);
    EXPECT_EQ(gpu_props.limits.pointSizeGranularity, 2.0);
}

// Noauto limittype member should not be modified
TEST_F(TestsUnion, Noauto) {
    VkPhysicalDeviceProperties gpu_props_native{};
    vkGetPhysicalDeviceProperties(gpu_native, &gpu_props_native);

    VkPhysicalDeviceProperties gpu_props_profile{};
    vkGetPhysicalDeviceProperties(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(gpu_props_profile.deviceID, gpu_props_native.deviceID);
    EXPECT_EQ(gpu_props_profile.vendorID, gpu_props_native.vendorID);
    EXPECT_STREQ(gpu_props_profile.deviceName, gpu_props_native.deviceName);
    EXPECT_EQ(gpu_props_profile.deviceType, gpu_props_native.deviceType);
}

// Noauto limittype member should not be modified
TEST_F(TestsUnion, Structure) {
    VkPhysicalDeviceCoherentMemoryFeaturesAMD featuresCoherentMemory{};
    featuresCoherentMemory.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COHERENT_MEMORY_FEATURES_AMD;

    VkPhysicalDeviceVariablePointersFeaturesKHR featuresVariablePointers{};
    featuresVariablePointers.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES;
    featuresVariablePointers.pNext = &featuresCoherentMemory;

    VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT featuresHelperInvocation{};
    featuresHelperInvocation.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES_EXT;
    featuresHelperInvocation.pNext = &featuresVariablePointers;

    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT features_dynamic_state{};
    features_dynamic_state.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;
    features_dynamic_state.pNext = &featuresHelperInvocation;

    VkPhysicalDeviceFeatures2KHR features{};
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR;
    features.pNext = &features_dynamic_state;

    vkGetPhysicalDeviceFeatures2(gpu_profile, &features);

    EXPECT_EQ(featuresCoherentMemory.deviceCoherentMemory, VK_FALSE);
    EXPECT_EQ(featuresVariablePointers.variablePointersStorageBuffer, VK_FALSE);
    EXPECT_EQ(featuresHelperInvocation.shaderDemoteToHelperInvocation, VK_TRUE);
    EXPECT_EQ(features_dynamic_state.extendedDynamicState, VK_TRUE);
}

TEST_F(TestsUnion, Format_Properties) {
    const VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    const VkFormatFeatureFlags linear_tiling_features = static_cast<VkFormatFeatureFlags>(0);
    const VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    const VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}
/*
TEST_F(TestsUnion, Format_Properties2KHR) {
    const VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
    VkFormatProperties2 format_properties;
    vkGetPhysicalDeviceFormatProperties2(gpu_profile, format, &format_properties);

    const VkFormatFeatureFlags linear_tiling_features = static_cast<VkFormatFeatureFlags>(0);
    const VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    const VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;

    EXPECT_EQ(format_properties.formatProperties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.formatProperties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.formatProperties.bufferFeatures & buffer_features, buffer_features);
}
*/
