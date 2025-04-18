/*
 * Copyright (C) 2021-2025 Valve Corporation
 * Copyright (C) 2021-2025 LunarG, Inc.
 * Copyright (c) 2024 RasterGrid Kft.
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
 */

#include <vulkan/vulkan_core.h>

#include <gtest/gtest.h>
#include "profiles_test_helper.h"

static VkPhysicalDevice gpu_native = VK_NULL_HANDLE;
static VkPhysicalDevice gpu_profile = VK_NULL_HANDLE;
static profiles_test::VulkanInstanceBuilder inst_builder;

class TestsIntersection : public VkTestFramework {
   public:
    TestsIntersection(){};
    ~TestsIntersection(){};

    static void SetUpTestSuite() {
        VkResult err = VK_SUCCESS;

        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_combine_intersect.json";
        const char* profile_name_data = "VP_LUNARG_test_combine_intersect"; 
        VkBool32 emulate_portability_data = VK_TRUE;
        const std::vector<const char*> simulate_capabilities = {
            "SIMULATE_API_VERSION_BIT", "SIMULATE_FEATURES_BIT", "SIMULATE_PROPERTIES_BIT", "SIMULATE_EXTENSIONS_BIT", "SIMULATE_FORMATS_BIT",
            "SIMULATE_QUEUE_FAMILY_PROPERTIES_BIT", "SIMULATE_VIDEO_CAPABILITIES_BIT", "SIMULATE_VIDEO_FORMATS_BIT"};
        const std::vector<const char*> debug_reports = {
            "DEBUG_REPORT_ERROR_BIT", "DEBUG_REPORT_WARNING_BIT", "DEBUG_REPORT_NOTIFICATION_BIT", "DEBUG_REPORT_DEBUG_BIT"};

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
            {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
            {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data},
            {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]},
            {kLayerName, kLayerSettingsDebugReports, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(debug_reports.size()), &debug_reports[0]}
        };

        err = inst_builder.init(VK_API_VERSION_1_3, settings);
        ASSERT_EQ(err, VK_SUCCESS);

        err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu_profile);
        ASSERT_EQ(err, VK_SUCCESS);

        err = inst_builder.getPhysicalDevice(profiles_test::MODE_NATIVE, &gpu_native);
        ASSERT_EQ(err, VK_SUCCESS);
    }

    static void TearDownTestSuite() { inst_builder.reset(); };
};

TEST_F(TestsIntersection, Extension) {
    uint32_t count;
    vkEnumerateDeviceExtensionProperties(gpu_profile, nullptr, &count, nullptr);
    std::vector<VkExtensionProperties> device_extensions(count);
    vkEnumerateDeviceExtensionProperties(gpu_profile, nullptr, &count, device_extensions.data());

    ASSERT_STREQ("VK_KHR_maintenance3", device_extensions[0].extensionName);
    ASSERT_STREQ("VK_KHR_portability_subset", device_extensions[1].extensionName);
}

TEST_F(TestsIntersection, Feature) {
    VkPhysicalDeviceFeatures gpu_features{};
    vkGetPhysicalDeviceFeatures(gpu_profile, &gpu_features);

    EXPECT_EQ(gpu_features.depthBiasClamp, VK_TRUE);
    EXPECT_EQ(gpu_features.depthClamp, VK_FALSE);
    EXPECT_EQ(gpu_features.largePoints, VK_TRUE);
}

TEST_F(TestsIntersection, Properties) {
    VkPhysicalDeviceProperties gpu_props{};
    vkGetPhysicalDeviceProperties(gpu_profile, &gpu_props);

    EXPECT_EQ(gpu_props.limits.maxImageDimension1D, 4096u);
    EXPECT_EQ(gpu_props.limits.maxComputeWorkGroupCount[0], 4096u);
    EXPECT_EQ(gpu_props.limits.maxComputeWorkGroupCount[1], 4096u);
    EXPECT_EQ(gpu_props.limits.maxComputeWorkGroupCount[2], 2048u);
    EXPECT_EQ(gpu_props.limits.mipmapPrecisionBits, 2u);
    EXPECT_TRUE(std::abs(gpu_props.limits.maxSamplerAnisotropy - 1.0) < 0.0001);
    EXPECT_EQ(gpu_props.limits.viewportBoundsRange[0], -16384);
    EXPECT_EQ(gpu_props.limits.viewportBoundsRange[1], 16384);
    EXPECT_EQ(gpu_props.limits.minMemoryMapAlignment, 64);
    EXPECT_EQ(gpu_props.limits.minTexelOffset, -4);
    EXPECT_EQ(gpu_props.limits.maxTexelOffset, 3);
    EXPECT_EQ(gpu_props.limits.framebufferColorSampleCounts, VK_SAMPLE_COUNT_4_BIT);
    EXPECT_EQ(gpu_props.limits.pointSizeRange[0], 4.0);
    EXPECT_EQ(gpu_props.limits.pointSizeRange[1], 32.0);
    EXPECT_EQ(gpu_props.limits.pointSizeGranularity, 4.0);

    EXPECT_EQ(gpu_props.sparseProperties.residencyAlignedMipSize, VK_FALSE);
}

// Noauto limittype member should not be modified
TEST_F(TestsIntersection, Noauto) {
    VkPhysicalDeviceProperties gpu_props_native{};
    vkGetPhysicalDeviceProperties(gpu_native, &gpu_props_native);

    VkPhysicalDeviceProperties gpu_props_profile{};
    vkGetPhysicalDeviceProperties(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(gpu_props_profile.deviceID, gpu_props_native.deviceID);
    EXPECT_EQ(gpu_props_profile.vendorID, gpu_props_native.vendorID);
    EXPECT_STREQ(gpu_props_profile.deviceName, gpu_props_native.deviceName);
    EXPECT_EQ(gpu_props_profile.deviceType, gpu_props_native.deviceType);
}

TEST_F(TestsIntersection, Format_Properties) {
    const VkFormat format = VK_FORMAT_R4G4_UNORM_PACK8;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    const VkFormatFeatureFlags linear_tiling_features = static_cast<VkFormatFeatureFlags>(VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT);
    const VkFormatFeatureFlags optimal_tiling_features = static_cast<VkFormatFeatureFlags>(0);
    const VkFormatFeatureFlags buffer_features = static_cast<VkFormatFeatureFlags>(VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT);

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsIntersection, Format_Properties3) {
    const VkFormat format = VK_FORMAT_R4G4B4A4_UNORM_PACK16;

    VkFormatProperties3 format_properties3;
    format_properties3.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3;
    format_properties3.pNext = nullptr;

    VkFormatProperties2 format_properties2;
    format_properties2.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    format_properties2.pNext = &format_properties3;

    vkGetPhysicalDeviceFormatProperties2(gpu_profile, format, &format_properties2);

    const VkFormatFeatureFlags linear_tiling_features = static_cast<VkFormatFeatureFlags>(0);
    const VkFormatFeatureFlags optimal_tiling_features = static_cast<VkFormatFeatureFlags>(0);
    const VkFormatFeatureFlags buffer_features = static_cast<VkFormatFeatureFlags>(VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BLEND_BIT);

    EXPECT_EQ(format_properties3.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties3.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties3.bufferFeatures & buffer_features, buffer_features);
}


