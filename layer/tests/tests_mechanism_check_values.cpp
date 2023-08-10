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
 * Author: Christophe Riccio <christophe@lunarg.com>
 * Author: Mark Lobodzinski <mark@lunarg.com>
 */

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_beta.h>

#include <gtest/gtest.h>
#include "profiles_test_helper.h"

#include <cstdarg>

class TestsMechanismCheckValues : public VkTestFramework {
   public:
    TestsMechanismCheckValues(){};
    ~TestsMechanismCheckValues(){};

    static void SetUpTestSuite() {}
    static void TearDownTestSuite(){};
};

TEST_F(TestsMechanismCheckValues, reading_flags) {
    TEST_DESCRIPTION("Test profile specifying flags bits");

#if defined(VK_NV_fragment_shading_rate_enums) && defined(VK_KHR_fragment_shading_rate) && defined(VK_KHR_shader_float_controls)
    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_api_alternate.json";
    const char* profile_name_data = "VP_LUNARG_test_api";
    VkBool32 emulate_portability_data = VK_TRUE;
    const std::vector<const char*> simulate_capabilities = {"SIMULATE_MAX_ENUM"};

    std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
        {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data},
        {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]}
    };

    err = inst_builder.init(settings);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice gpu_profile = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu_profile);
    if (gpu_profile == VK_NULL_HANDLE) return;

    VkPhysicalDevice gpu_native = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu_native);
    if (gpu_native == VK_NULL_HANDLE) return;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    VkPhysicalDeviceFragmentShadingRateEnumsPropertiesNV fragment_shading_rate_enums_properties_profile{};
    fragment_shading_rate_enums_properties_profile.sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_ENUMS_PROPERTIES_NV;

    VkPhysicalDeviceFragmentShadingRatePropertiesKHR fragment_shading_rate_properties_profile{};
    fragment_shading_rate_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR;
    fragment_shading_rate_properties_profile.pNext = &fragment_shading_rate_enums_properties_profile;

    VkPhysicalDeviceFloatControlsPropertiesKHR float_control_properties_profile{};
    float_control_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT_CONTROLS_PROPERTIES_KHR;
    float_control_properties_profile.pNext = &fragment_shading_rate_properties_profile;

    VkPhysicalDeviceFragmentShadingRateEnumsPropertiesNV fragment_shading_rate_enums_properties_native{};
    fragment_shading_rate_enums_properties_native.sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_ENUMS_PROPERTIES_NV;

    VkPhysicalDeviceFragmentShadingRatePropertiesKHR fragment_shading_rate_properties_native{};
    fragment_shading_rate_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR;
    fragment_shading_rate_properties_native.pNext = &fragment_shading_rate_enums_properties_native;

    VkPhysicalDeviceFloatControlsPropertiesKHR float_control_properties_native{};
    float_control_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT_CONTROLS_PROPERTIES_KHR;
    float_control_properties_native.pNext = &fragment_shading_rate_properties_native;

    gpu_props.pNext = &float_control_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props);

    gpu_props.pNext = &float_control_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props);

    // These values can't be overridden
    EXPECT_EQ(float_control_properties_profile.denormBehaviorIndependence,
              float_control_properties_native.denormBehaviorIndependence);
    EXPECT_EQ(float_control_properties_profile.roundingModeIndependence, float_control_properties_native.roundingModeIndependence);

    // These values are overridden
    EXPECT_EQ(fragment_shading_rate_properties_profile.maxFragmentShadingRateRasterizationSamples, VK_SAMPLE_COUNT_2_BIT);
    EXPECT_EQ(fragment_shading_rate_enums_properties_profile.maxFragmentShadingRateInvocationCount, VK_SAMPLE_COUNT_2_BIT);
#endif
}

TEST_F(TestsMechanismCheckValues, reading_sparseProperties) {
    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_device_properties.json";
    const char* profile_name_data = "VP_LUNARG_test_device_properties";
    const std::vector<const char*> simulate_capabilities = {"SIMULATE_MAX_ENUM"};
    VkBool32 emulate_portability_data = VK_TRUE;

    std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
        {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]},
        {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data}
    };

    err = inst_builder.init(settings);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice gpu_profile = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu_profile);
    if (gpu_profile == VK_NULL_HANDLE) return;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props);

    // These values are overridden
    EXPECT_EQ(gpu_props.properties.limits.maxImageDimension1D, 4096);
    EXPECT_EQ(gpu_props.properties.limits.maxImageDimension2D, 4096);
    EXPECT_EQ(gpu_props.properties.limits.maxImageDimension3D, 4096);

    EXPECT_EQ(gpu_props.properties.sparseProperties.residencyStandard2DBlockShape, VK_TRUE);
    EXPECT_EQ(gpu_props.properties.sparseProperties.residencyStandard2DMultisampleBlockShape, VK_TRUE);
    EXPECT_EQ(gpu_props.properties.sparseProperties.residencyStandard3DBlockShape, VK_TRUE);
    EXPECT_EQ(gpu_props.properties.sparseProperties.residencyAlignedMipSize, VK_FALSE);
    EXPECT_EQ(gpu_props.properties.sparseProperties.residencyNonResidentStrict, VK_FALSE);
}

TEST_F(TestsMechanismCheckValues, reading_mutable_descriptor_type) {
    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_device_features.json";
    const char* profile_name_data = "VP_LUNARG_test_device_features";
    const std::vector<const char*> simulate_capabilities = {"SIMULATE_FEATURES_BIT"};
    VkBool32 emulate_portability_data = VK_FALSE;

    std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
        {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]},
        {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data}};

    err = inst_builder.init(settings);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice gpu_profile = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu_profile);
    if (gpu_profile == VK_NULL_HANDLE) return;

    if (!profiles_test::IsExtensionSupported(gpu_profile, "VK_EXT_mutable_descriptor_type")) return;

    VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT mutable_descriptor_features{};
    mutable_descriptor_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MUTABLE_DESCRIPTOR_TYPE_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 gpu_features{};
    gpu_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    gpu_features.pNext = &mutable_descriptor_features;
    vkGetPhysicalDeviceFeatures2(gpu_profile, &gpu_features);

    EXPECT_EQ(mutable_descriptor_features.mutableDescriptorType, VK_TRUE);
}
