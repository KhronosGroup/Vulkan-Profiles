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

    ASSERT_STREQ("VK_KHR_maintenance1", device_extensions[0].extensionName);
    ASSERT_STREQ("VK_KHR_maintenance2", device_extensions[1].extensionName);
    ASSERT_STREQ("VK_KHR_maintenance3", device_extensions[2].extensionName);
    ASSERT_STREQ("VK_KHR_portability_subset", device_extensions[3].extensionName);
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
    EXPECT_EQ(gpu_props.limits.maxTexelOffset, 7);
    EXPECT_EQ(gpu_props.limits.framebufferColorSampleCounts, VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_2_BIT | VK_SAMPLE_COUNT_4_BIT);
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
