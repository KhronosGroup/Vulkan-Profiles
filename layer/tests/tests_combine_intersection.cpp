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

class TestsIntersection : public VkTestFramework {
   public:
    TestsIntersection(){};
    ~TestsIntersection(){};

    static void SetUpTestSuite() {
        VkResult err = VK_SUCCESS;

        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_combine_intersect.json";
        settings.emulate_portability = true;
        settings.profile_name = "VP_LUNARG_test_combine_intersect";
        settings.simulate_capabilities = SIMULATE_MAX_ENUM;
        settings.debug_reports =
            DEBUG_REPORT_ERROR_BIT | DEBUG_REPORT_WARNING_BIT | DEBUG_REPORT_NOTIFICATION_BIT | DEBUG_REPORT_DEBUG_BIT;

        err = inst_builder.init(&settings);
        ASSERT_EQ(err, VK_SUCCESS);

        err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);
        ASSERT_EQ(err, VK_SUCCESS);
    }

    static void TearDownTestSuite() { inst_builder.reset(); };
};

TEST_F(TestsIntersection, Extension) {
    uint32_t count;
    vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, nullptr);
    std::vector<VkExtensionProperties> device_extensions(count);
    vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, device_extensions.data());

    ASSERT_STREQ("VK_KHR_maintenance3", device_extensions[0].extensionName);
    ASSERT_STREQ("VK_KHR_portability_subset", device_extensions[1].extensionName);
}

TEST_F(TestsIntersection, Feature) {
    VkPhysicalDeviceFeatures gpu_features{};
    vkGetPhysicalDeviceFeatures(gpu, &gpu_features);

    EXPECT_EQ(gpu_features.depthBiasClamp, VK_TRUE);
    EXPECT_EQ(gpu_features.depthClamp, VK_FALSE);
/*
    EXPECT_EQ(gpu_features.drawIndirectFirstInstance, VK_FALSE);
    EXPECT_EQ(gpu_features.multiDrawIndirect, VK_FALSE);
    EXPECT_EQ(gpu_features.sampleRateShading, VK_FALSE);
    EXPECT_EQ(gpu_features.shaderClipDistance, VK_FALSE);
    EXPECT_EQ(gpu_features.samplerAnisotropy, VK_FALSE);
    EXPECT_EQ(gpu_features.multiViewport, VK_FALSE);
*/
}

TEST_F(TestsIntersection, Properties) {
    VkPhysicalDeviceProperties gpu_props{};
    vkGetPhysicalDeviceProperties(gpu, &gpu_props);

    EXPECT_EQ(gpu_props.limits.maxImageDimension1D, 4096u);
    EXPECT_EQ(gpu_props.limits.maxComputeWorkGroupCount[0], 4096u);
    EXPECT_EQ(gpu_props.limits.maxComputeWorkGroupCount[1], 4096u);
    EXPECT_EQ(gpu_props.limits.maxComputeWorkGroupCount[2], 2048u);
    EXPECT_EQ(gpu_props.limits.mipmapPrecisionBits, 2u);
    EXPECT_TRUE(std::abs(gpu_props.limits.maxSamplerAnisotropy - 1.0) < 0.0001);
    EXPECT_EQ(gpu_props.limits.viewportBoundsRange[0], -16384);
    EXPECT_EQ(gpu_props.limits.viewportBoundsRange[1], 16384);
    EXPECT_EQ(gpu_props.limits.minMemoryMapAlignment, 4096);
    EXPECT_EQ(gpu_props.limits.minTexelOffset, -4);
    EXPECT_EQ(gpu_props.limits.maxTexelOffset, 3);
    EXPECT_EQ(gpu_props.limits.framebufferColorSampleCounts, VK_SAMPLE_COUNT_4_BIT);
    EXPECT_EQ(gpu_props.limits.pointSizeRange[0], 4.0);
    EXPECT_EQ(gpu_props.limits.pointSizeRange[1], 32.0);
    EXPECT_EQ(gpu_props.limits.pointSizeGranularity, 4.0);
}
