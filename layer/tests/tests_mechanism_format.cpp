/*
 * Copyright (C) 2021-2026 Valve Corporation
 * Copyright (C) 2021-2026 LunarG, Inc.
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
#include <vulkan/vulkan_beta.h>

#include <gtest/gtest.h>
#include "profiles_test_helper.h"

#include <cstdarg>

class TestsMechanismFormat : public VkTestFramework {
   public:
    TestsMechanismFormat(){};
    ~TestsMechanismFormat(){};

    static void SetUpTestSuite() {}
    static void TearDownTestSuite(){};
};

TEST_F(TestsMechanismFormat, TestFullySupportedFormat) {
    TEST_DESCRIPTION("Test format capabilities of a supported format");

    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_baseline_formats.json";
    const char* profile_name_data = "VP_LUNARG_test_formats";
    const std::vector<const char*> simulate_capabilities = {"SIMULATE_FORMATS_BIT"};

    std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
        {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT,
         static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]}};

    err = inst_builder.init(settings);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice gpu;
    err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);
    if (err != VK_SUCCESS) {
        printf("Profile not supported on device, skipping test.\n");
        return;
    }

    {
        VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
        VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT |
                                                       VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
                                                       VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
        VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;

        VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
        VkFormatProperties format_properties;
        vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

        EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
        EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
        EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

        VkFormatProperties2 format_properties2 = {};
        format_properties2.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
        vkGetPhysicalDeviceFormatProperties2(gpu, format, &format_properties2);

        EXPECT_EQ(format_properties2.formatProperties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
        EXPECT_EQ(format_properties2.formatProperties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
        EXPECT_EQ(format_properties2.formatProperties.bufferFeatures & buffer_features, buffer_features);

#ifdef VK_KHR_format_feature_flags2
        VkFormatProperties3 format_properties3 = {};
        format_properties3.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3;
        format_properties2.pNext = &format_properties3;
        vkGetPhysicalDeviceFormatProperties2(gpu, format, &format_properties2);

        VkFormatFeatureFlags2KHR linear_tiling_features2 = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
        VkFormatFeatureFlags2KHR optimal_tiling_features2 = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT |
                                                            VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
                                                            VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
        VkFormatFeatureFlags2KHR buffer_features2 = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;

        EXPECT_EQ(format_properties3.linearTilingFeatures & linear_tiling_features2, linear_tiling_features2);
        EXPECT_EQ(format_properties3.optimalTilingFeatures & optimal_tiling_features2, optimal_tiling_features2);
        EXPECT_EQ(format_properties3.bufferFeatures & buffer_features2, buffer_features2);
#endif
    }
}

TEST_F(TestsMechanismFormat, TestParsingAllFormatProperties) {
    TEST_DESCRIPTION("Test all different ways of setting image formats");

    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_formats.json";
    const char* profile_name_data = "VP_LUNARG_test_formats";
    const std::vector<const char*> simulate_capabilities = {"SIMULATE_MAX_ENUM"};

    std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
        {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT,
         static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]}};

    err = inst_builder.init(settings);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice gpu;
    err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);
    if (err != VK_SUCCESS) {
        printf("Profile not supported on device, skipping test.\n");
        return;
    }

    {
        VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
        VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
        VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;

        VkFormat format = VK_FORMAT_R4G4_UNORM_PACK8;
        VkFormatProperties format_properties;
        vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

        EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
        EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
        EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

        VkFormatProperties2 format_properties2 = {};
        format_properties2.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
        vkGetPhysicalDeviceFormatProperties2(gpu, format, &format_properties2);

        EXPECT_EQ(format_properties2.formatProperties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
        EXPECT_EQ(format_properties2.formatProperties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
        EXPECT_EQ(format_properties2.formatProperties.bufferFeatures & buffer_features, buffer_features);

#ifdef VK_KHR_format_feature_flags2
        VkFormatProperties3 format_properties3 = {};
        format_properties3.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3;
        format_properties2.pNext = &format_properties3;
        vkGetPhysicalDeviceFormatProperties2(gpu, format, &format_properties2);

        VkFormatFeatureFlags2KHR linear_tiling_features2 = VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BLEND_BIT;
        VkFormatFeatureFlags2KHR optimal_tiling_features2 = VK_FORMAT_FEATURE_2_VERTEX_BUFFER_BIT;
        VkFormatFeatureFlags2KHR buffer_features2 = VK_FORMAT_FEATURE_2_STORAGE_IMAGE_BIT;

        EXPECT_EQ(format_properties3.linearTilingFeatures & linear_tiling_features2, linear_tiling_features2);
        EXPECT_EQ(format_properties3.optimalTilingFeatures & optimal_tiling_features2, optimal_tiling_features2);
        EXPECT_EQ(format_properties3.bufferFeatures & buffer_features2, buffer_features2);
#endif
    }

    {
        VkFormat format = VK_FORMAT_R4G4B4A4_UNORM_PACK16;

        VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
        VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
        VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

        VkFormatProperties format_properties;
        vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

        EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
        EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
        EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

        VkFormatProperties2 format_properties2 = {};
        format_properties2.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
        vkGetPhysicalDeviceFormatProperties2(gpu, format, &format_properties2);

        EXPECT_EQ(format_properties2.formatProperties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
        EXPECT_EQ(format_properties2.formatProperties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
        EXPECT_EQ(format_properties2.formatProperties.bufferFeatures & buffer_features, buffer_features);

#ifdef VK_KHR_format_feature_flags2
        VkFormatProperties3 format_properties3 = {};
        format_properties3.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3;
        format_properties2.pNext = &format_properties3;
        vkGetPhysicalDeviceFormatProperties2(gpu, format, &format_properties2);

        VkFormatFeatureFlags2KHR linear_tiling_features2 = VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT;
        VkFormatFeatureFlags2KHR optimal_tiling_features2 = VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BLEND_BIT;
        VkFormatFeatureFlags2KHR buffer_features2 = VK_FORMAT_FEATURE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

        EXPECT_EQ(format_properties3.linearTilingFeatures & linear_tiling_features2, linear_tiling_features2);
        EXPECT_EQ(format_properties3.optimalTilingFeatures & optimal_tiling_features2, optimal_tiling_features2);
        EXPECT_EQ(format_properties3.bufferFeatures & buffer_features2, buffer_features2);
#endif
    }

#ifdef VK_KHR_format_feature_flags2
    {
        VkFormat format = VK_FORMAT_B4G4R4A4_UNORM_PACK16;

        VkFormatProperties3 format_properties3 = {};
        format_properties3.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3;
        VkFormatProperties2 format_properties2 = {};
        format_properties2.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
        format_properties2.pNext = &format_properties3;
        vkGetPhysicalDeviceFormatProperties2(gpu, format, &format_properties2);

        VkFormatFeatureFlags2KHR linear_tiling_features = VK_FORMAT_FEATURE_2_STORAGE_TEXEL_BUFFER_ATOMIC_BIT_KHR |
                                                          VK_FORMAT_FEATURE_2_STORAGE_IMAGE_BIT |
                                                          VK_FORMAT_FEATURE_2_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
        VkFormatFeatureFlags2KHR optimal_tiling_features = VK_FORMAT_FEATURE_2_STORAGE_IMAGE_BIT_KHR;
        VkFormatFeatureFlags2KHR buffer_features = VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BLEND_BIT_KHR;

        EXPECT_EQ(format_properties3.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
        EXPECT_EQ(format_properties3.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
        EXPECT_EQ(format_properties3.bufferFeatures & buffer_features, buffer_features);
    }
#endif

    {
        VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;

        VkFormatProperties format_properties;
        vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

        VkFormatFeatureFlags linear_tiling_features =
            VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
        VkFormatFeatureFlags optimal_tiling_features =
            VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
        VkFormatFeatureFlags buffer_features =
            VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;

        EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
        EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
        EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

#ifdef VK_KHR_format_feature_flags2
        VkFormatProperties3KHR format_properties3 = {};
        format_properties3.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3_KHR;
        VkFormatProperties2 format_properties2 = {};
        format_properties2.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
        format_properties2.pNext = &format_properties3;
        vkGetPhysicalDeviceFormatProperties2(gpu, format, &format_properties2);

        VkFormatFeatureFlags2KHR linear_tiling_features2 =
            VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT_KHR | VK_FORMAT_FEATURE_2_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
        VkFormatFeatureFlags2KHR optimal_tiling_features2 =
            VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BLEND_BIT_KHR | VK_FORMAT_FEATURE_2_STORAGE_IMAGE_BIT;
        VkFormatFeatureFlags2KHR buffer_features2 =
            VK_FORMAT_FEATURE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BLEND_BIT;

        EXPECT_EQ(format_properties3.linearTilingFeatures & linear_tiling_features2, linear_tiling_features2);
        EXPECT_EQ(format_properties3.optimalTilingFeatures & optimal_tiling_features2, optimal_tiling_features2);
        EXPECT_EQ(format_properties3.bufferFeatures & buffer_features2, buffer_features2);
    }
#endif
}

TEST_F(TestsMechanismFormat, TestFormatPropertiesPNextChainStructures) {
    TEST_DESCRIPTION("Test the VkFormatProperties2 pNext chain structures, other than VkFormatProperties3, set by a profile");

    profiles_test::VulkanInstanceBuilder inst_builder;

    const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_formats_pnext.json";
    const char* profile_name_data = "VP_LUNARG_test_formats_pnext";
    const std::vector<const char*> simulate_capabilities = {"SIMULATE_MAX_ENUM"};

    std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
        {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT,
         static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]}};

    ASSERT_EQ(inst_builder.init(settings), VK_SUCCESS);

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    if (inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu) != VK_SUCCESS) {
        printf("Profile not supported on device, skipping test.\n");
        return;
    }

    const VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;

    VkSubpassResolvePerformanceQueryEXT subpass_resolve_performance_query{VK_STRUCTURE_TYPE_SUBPASS_RESOLVE_PERFORMANCE_QUERY_EXT};
    VkTensorFormatPropertiesARM tensor_format_properties{VK_STRUCTURE_TYPE_TENSOR_FORMAT_PROPERTIES_ARM, &subpass_resolve_performance_query};
    VkDrmFormatModifierPropertiesListEXT drm_format_modifier_properties_list{VK_STRUCTURE_TYPE_DRM_FORMAT_MODIFIER_PROPERTIES_LIST_EXT,
                                                                            &tensor_format_properties};
    VkFormatProperties2 format_properties2{VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2, &drm_format_modifier_properties_list};
    vkGetPhysicalDeviceFormatProperties2(gpu, format, &format_properties2);

    EXPECT_EQ(subpass_resolve_performance_query.optimal, VK_TRUE);
    EXPECT_EQ(tensor_format_properties.linearTilingTensorFeatures, VK_FORMAT_FEATURE_2_TENSOR_SHADER_BIT_ARM);
    EXPECT_EQ(tensor_format_properties.optimalTilingTensorFeatures,
              VK_FORMAT_FEATURE_2_TENSOR_SHADER_BIT_ARM | VK_FORMAT_FEATURE_2_TENSOR_IMAGE_ALIASING_BIT_ARM);

    // The DRM format modifiers are returned with the Vulkan two-call idiom
    ASSERT_EQ(drm_format_modifier_properties_list.drmFormatModifierCount, 2u);

    std::vector<VkDrmFormatModifierPropertiesEXT> drm_format_modifier_properties(drm_format_modifier_properties_list.drmFormatModifierCount);
    drm_format_modifier_properties_list.pDrmFormatModifierProperties = drm_format_modifier_properties.data();
    vkGetPhysicalDeviceFormatProperties2(gpu, format, &format_properties2);

    ASSERT_EQ(drm_format_modifier_properties_list.drmFormatModifierCount, 2u);
    EXPECT_EQ(drm_format_modifier_properties[0].drmFormatModifier, 0u);
    EXPECT_EQ(drm_format_modifier_properties[0].drmFormatModifierPlaneCount, 1u);
    EXPECT_EQ(drm_format_modifier_properties[0].drmFormatModifierTilingFeatures,
              static_cast<VkFormatFeatureFlags>(VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT));
    EXPECT_EQ(drm_format_modifier_properties[1].drmFormatModifier, 72057594037927935ull);
    EXPECT_EQ(drm_format_modifier_properties[1].drmFormatModifierPlaneCount, 2u);
    EXPECT_EQ(drm_format_modifier_properties[1].drmFormatModifierTilingFeatures,
              static_cast<VkFormatFeatureFlags>(VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT));

    // A smaller array only receives the first DRM format modifiers
    drm_format_modifier_properties_list.drmFormatModifierCount = 1;
    drm_format_modifier_properties[1] = VkDrmFormatModifierPropertiesEXT{};
    vkGetPhysicalDeviceFormatProperties2(gpu, format, &format_properties2);

    EXPECT_EQ(drm_format_modifier_properties_list.drmFormatModifierCount, 1u);
    EXPECT_EQ(drm_format_modifier_properties[0].drmFormatModifierPlaneCount, 1u);
    EXPECT_EQ(drm_format_modifier_properties[1].drmFormatModifierPlaneCount, 0u);

    // A format not defined by the profile has no value
    VkTensorFormatPropertiesARM other_tensor_format_properties{VK_STRUCTURE_TYPE_TENSOR_FORMAT_PROPERTIES_ARM};
    VkFormatProperties2 other_format_properties2{VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2, &other_tensor_format_properties};
    vkGetPhysicalDeviceFormatProperties2(gpu, VK_FORMAT_R8G8B8A8_SNORM, &other_format_properties2);

    EXPECT_EQ(other_tensor_format_properties.optimalTilingTensorFeatures, 0u);
    EXPECT_EQ(other_tensor_format_properties.linearTilingTensorFeatures, 0u);
}
