/*
 * Copyright (C) 2021-2024 Valve Corporation
 * Copyright (C) 2021-2024 LunarG, Inc.
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

class TestsMechanism : public VkTestFramework {
   public:
    TestsMechanism(){};
    ~TestsMechanism(){};

    static void SetUpTestSuite() {}
    static void TearDownTestSuite(){};
};

TEST_F(TestsMechanism, check_extension) {
    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", TEST_BINARY_PATH);

    // Check Profiles layer is loaded
    {
        uint32_t layer_count = 0;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

        std::vector<VkLayerProperties> layer_names(layer_count);
        vkEnumerateInstanceLayerProperties(&layer_count, &layer_names[0]);

        EXPECT_TRUE(layer_count >= 1);

        bool layer_found = false;
        for (std::size_t i = 0, n = layer_names.size(); i < n; ++i) {
            if (layer_names[i].layerName == std::string("VK_LAYER_KHRONOS_profiles")) {
                layer_found = true;
                break;
            }
        }

        EXPECT_TRUE(layer_found);
    }

    // Check VK_EXT_layer_settings is exposed
    {
        uint32_t extCount = 0;
        VkResult result = vkEnumerateInstanceExtensionProperties("VK_LAYER_KHRONOS_profiles", &extCount, nullptr);
        ASSERT_EQ(err, VK_SUCCESS);

        EXPECT_TRUE(extCount >= 1);

        std::vector<VkExtensionProperties> ext(extCount);
        result = vkEnumerateInstanceExtensionProperties("VK_LAYER_KHRONOS_profiles", &extCount, ext.data());
        ASSERT_EQ(err, VK_SUCCESS);

        bool found_ext = false;
        for (std::size_t i = 0, n = ext.size(); i < n; ++i) {
            if (ext[i].extensionName == std::string(VK_EXT_LAYER_SETTINGS_EXTENSION_NAME)) {
                found_ext = true;
                break;
            }
        }

        EXPECT_TRUE(found_ext);
    }

    err = inst_builder.init();
    ASSERT_EQ(err, VK_SUCCESS);
}

TEST_F(TestsMechanism, selecting_profile_file) {
    TEST_DESCRIPTION("Test selecting profile from a profiles file with multiple profiles");

    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    // Only override extensions
    {
        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_device_extensions.json";
        const char* profile_name_data = "VP_LUNARG_test_device_extensions";
        VkBool32 emulate_portability_data = VK_FALSE;
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_EXTENSIONS_BIT"};
        const char* debug_reports_data = "DEBUG_REPORT_MAX_ENUM";

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
            {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
            {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data},
            {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]},
            {kLayerName, kLayerSettingsDebugReports, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &debug_reports_data}};

        err = inst_builder.init(settings);
        ASSERT_EQ(err, VK_SUCCESS);

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);
        if (err != VK_SUCCESS) {
            printf("Profile not supported on device, skipping test.\n");
            inst_builder.reset();
            return;
        }

        VkResult result = VK_SUCCESS;

        uint32_t extCount = 0;
        result = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extCount, nullptr);
        ASSERT_EQ(err, VK_SUCCESS);

        EXPECT_EQ(1, extCount);

        std::vector<VkExtensionProperties> ext(extCount);
        result = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extCount, ext.data());
        ASSERT_EQ(err, VK_SUCCESS);

        EXPECT_STREQ("VK_KHR_maintenance3", ext[0].extensionName);

        inst_builder.reset();
    }

    // Override everything but the layer only has extensions
    {
        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_device_extensions.json";
        const char* profile_name_data = "VP_LUNARG_test_device_extensions";
        VkBool32 emulate_portability_data = VK_FALSE;
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_MAX_ENUM"};
        const char* debug_reports_data = "DEBUG_REPORT_MAX_ENUM";

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
            {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
            {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data},
            {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]},
            {kLayerName, kLayerSettingsDebugReports, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &debug_reports_data}};

        err = inst_builder.init(settings);
        ASSERT_EQ(err, VK_SUCCESS);

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);
        if (err != VK_SUCCESS) {
            printf("Profile not supported on device, skipping test.\n");
            inst_builder.reset();
            return;
        }

        VkResult result = VK_SUCCESS;

        uint32_t extCount = 0;
        result = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extCount, nullptr);
        ASSERT_EQ(err, VK_SUCCESS);

        EXPECT_EQ(1, extCount);

        std::vector<VkExtensionProperties> ext(extCount);
        result = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extCount, ext.data());
        ASSERT_EQ(err, VK_SUCCESS);

        EXPECT_STREQ("VK_KHR_maintenance3", ext[0].extensionName);

        inst_builder.reset();
    }

    {
        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_selecting_profile.json";
        const char* profile_name_data = "VP_LUNARG_test_selecting_profile";
        VkBool32 emulate_portability_data = VK_TRUE;
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_PROPERTIES_BIT", "SIMULATE_EXTENSIONS_BIT"};

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
            {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
            {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data},
            {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]}
        };

        err = inst_builder.init(settings);
        ASSERT_EQ(err, VK_SUCCESS);

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);
        if (err != VK_SUCCESS) {
            printf("Profile not supported on device, skipping test.\n");
            inst_builder.reset();
            return;
        }

        VkPhysicalDeviceProperties gpu_props{};
        vkGetPhysicalDeviceProperties(gpu, &gpu_props);

        EXPECT_EQ(gpu_props.limits.maxImageDimension1D, 4096u);

        inst_builder.reset();
    }

    {
        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_selecting_profile.json";
        const char* profile_name_data = "VP_LUNARG_test_selecting_profile_subset";
        VkBool32 emulate_portability_data = VK_TRUE;
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_PROPERTIES_BIT", "SIMULATE_EXTENSIONS_BIT"};

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
            {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
            {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data},
            {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]}};

        err = inst_builder.init(settings);
        ASSERT_EQ(err, VK_SUCCESS);

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);
        if (err != VK_SUCCESS) {
            printf("Profile not supported on device, skipping test.\n");
            inst_builder.reset();
            return;
        }

        VkPhysicalDeviceProperties gpu_props{};
        vkGetPhysicalDeviceProperties(gpu, &gpu_props);

        EXPECT_EQ(gpu_props.limits.maxImageDimension1D, 2048u);

        inst_builder.reset();
    }
}

TEST_F(TestsMechanism, selecting_profile_dirs) {
    TEST_DESCRIPTION("Test selecting profile from a profiles file with multiple profiles");

    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    {
        const char* profile_dirs_data = JSON_TEST_FILES_PATH;
        const char* profile_name_data = "VP_LUNARG_test_device_extensions";
        VkBool32 emulate_portability_data = VK_FALSE;
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_MAX_ENUM"};
        const char* debug_reports_data = "DEBUG_REPORT_MAX_ENUM";

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileDirs, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_dirs_data},
            {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
            {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data},
            {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]},
            {kLayerName, kLayerSettingsDebugReports, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &debug_reports_data}};

        err = inst_builder.init(settings);
        ASSERT_EQ(err, VK_SUCCESS);

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);
        if (err != VK_SUCCESS) {
            printf("Profile not supported on device, skipping test.\n");
            inst_builder.reset();
            return;
        }

        VkResult result = VK_SUCCESS;

        uint32_t extCount = 0;
        result = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extCount, nullptr);
        ASSERT_EQ(err, VK_SUCCESS);

        EXPECT_EQ(1, extCount);

        std::vector<VkExtensionProperties> ext(extCount);
        result = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extCount, ext.data());
        ASSERT_EQ(err, VK_SUCCESS);

        EXPECT_STREQ("VK_KHR_maintenance3", ext[0].extensionName);

        inst_builder.reset();
    }
}

TEST_F(TestsMechanism, selecting_profile_with_required_profiles) {
    TEST_DESCRIPTION("Test loading a profile with a required profiles");

    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    {
        const char* profile_dirs_data = JSON_TEST_FILES_PATH;
        const char* profile_name_data = "VP_LUNARG_test_required_profiles2";
        VkBool32 emulate_portability_data = VK_FALSE;
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_MAX_ENUM"};
        const char* debug_reports_data = "DEBUG_REPORT_MAX_ENUM";

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileDirs, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_dirs_data},
            {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
            {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data},
            {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT,
             static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]},
            {kLayerName, kLayerSettingsDebugReports, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &debug_reports_data}};

        err = inst_builder.init(settings);
        ASSERT_EQ(err, VK_SUCCESS);

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);
        if (err != VK_SUCCESS) {
            printf("Profile not supported on device, skipping test.\n");
            inst_builder.reset();
            return;
        }

        VkResult result = VK_SUCCESS;

        VkPhysicalDeviceProperties2 gpu_props2{};
        gpu_props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        gpu_props2.pNext = nullptr;
        vkGetPhysicalDeviceProperties2(gpu, &gpu_props2);

        EXPECT_EQ(2048, gpu_props2.properties.limits.maxImageDimension1D);
        EXPECT_EQ(4096, gpu_props2.properties.limits.maxImageDimension2D);

        inst_builder.reset();
    }
}

TEST_F(TestsMechanism, profile_variants_all) {
    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    {
        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_variants.json";
        const char* profile_name_data = "VP_LUNARG_test_variants";
        VkBool32 emulate_portability_data = VK_FALSE;
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_API_VERSION_BIT", "SIMULATE_FEATURES_BIT",
                                                                "SIMULATE_PROPERTIES_BIT", "SIMULATE_EXTENSIONS_BIT"};
        const char* debug_reports_data = "DEBUG_REPORT_MAX_ENUM";

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
            {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
            {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data},
            {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT,
             static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]},
            {kLayerName, kLayerSettingsDebugReports, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &debug_reports_data}};

        err = inst_builder.init(settings);
        ASSERT_EQ(err, VK_SUCCESS);

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);
        if (err != VK_SUCCESS) {
            printf("Profile not supported on device, skipping test.\n");
            inst_builder.reset();
            return;
        }

        VkPhysicalDeviceProperties gpu_props{};
        vkGetPhysicalDeviceProperties(gpu, &gpu_props);

        EXPECT_EQ(2048, gpu_props.limits.maxImageDimension1D);
        EXPECT_EQ(1024, gpu_props.limits.maxImageDimension2D);

        VkPhysicalDeviceInlineUniformBlockPropertiesEXT inline_uniform_block_prop{};
        inline_uniform_block_prop.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_PROPERTIES;

        VkPhysicalDeviceProperties2 gpu_props2{};
        gpu_props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        gpu_props2.pNext = &inline_uniform_block_prop;
        vkGetPhysicalDeviceProperties2(gpu, &gpu_props2);

        EXPECT_EQ(4, inline_uniform_block_prop.maxDescriptorSetInlineUniformBlocks);

        VkPhysicalDevice16BitStorageFeaturesKHR storage_16bit{};
        storage_16bit.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES_KHR;

        VkPhysicalDeviceFeatures2KHR features{};
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR;
        features.pNext = &storage_16bit;

        vkGetPhysicalDeviceFeatures2(gpu, &features);

        EXPECT_EQ(VK_FALSE, features.features.depthClamp);
        EXPECT_EQ(VK_TRUE, features.features.depthBiasClamp);
        EXPECT_EQ(VK_FALSE, features.features.drawIndirectFirstInstance);
        EXPECT_EQ(VK_TRUE, features.features.imageCubeArray);
        EXPECT_EQ(VK_TRUE, storage_16bit.storageBuffer16BitAccess);
        EXPECT_EQ(VK_FALSE, storage_16bit.uniformAndStorageBuffer16BitAccess);
        EXPECT_EQ(VK_TRUE, storage_16bit.storagePushConstant16);
        EXPECT_EQ(VK_FALSE, storage_16bit.storageInputOutput16);

        inst_builder.reset();
    }
}

TEST_F(TestsMechanism, default_feature_values) {
    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    // Vulkan 1.0
    {
        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_api_1_0.json";
        const char* profile_name_data = "VP_LUNARG_test_api_1_0";
        VkBool32 emulate_portability_data = VK_FALSE;
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_API_VERSION_BIT", "SIMULATE_EXTENSIONS_BIT"};
        const char* default_feature_values_data = "DEFAULT_FEATURE_VALUES_FALSE";
        const char* debug_reports_data = "DEBUG_REPORT_MAX_ENUM";

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
            {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
            {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data},
            {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]},
            {kLayerName, kLayerSettingsDefaultFeatureValues, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &default_feature_values_data},
            {kLayerName, kLayerSettingsDebugReports, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &debug_reports_data}};

        err = inst_builder.init(settings);
        ASSERT_EQ(err, VK_SUCCESS);

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);
        if (err != VK_SUCCESS) {
            printf("Profile not supported on device, skipping test.\n");
            inst_builder.reset();
            return;
        }

        VkPhysicalDeviceFeatures gpu_features{};
        vkGetPhysicalDeviceFeatures(gpu, &gpu_features);

        EXPECT_EQ(gpu_features.independentBlend, VK_FALSE);

        VkPhysicalDevice16BitStorageFeaturesKHR gpu_features_16bits_storage{
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES_KHR};
        VkPhysicalDeviceFeatures2 gpu_features2{};
        gpu_features2.pNext = &gpu_features_16bits_storage;
        vkGetPhysicalDeviceFeatures2(gpu, &gpu_features2);

        EXPECT_EQ(gpu_features2.features.independentBlend, VK_FALSE);
        EXPECT_EQ(gpu_features_16bits_storage.storageBuffer16BitAccess, VK_FALSE);

        inst_builder.reset();
    }
}

TEST_F(TestsMechanism, reading_duplicated_members) {
    TEST_DESCRIPTION("Test profile using duplicated members");

    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_duplicated.json";
    const char* profile_name_data = "VP_LUNARG_test_duplicated";
    VkBool32 emulate_portability_data = VK_TRUE;
    const std::vector<const char*> simulate_capabilities = {"SIMULATE_MAX_ENUM"};
    VkBool32 debug_fail_on_error = VK_TRUE;

    std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
        {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data},
        {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]},
        {kLayerName, kLayerSettingsDebugFailOnError, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &debug_fail_on_error}
    };

    err = inst_builder.init(settings);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice physical_device;
    err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &physical_device);
    ASSERT_EQ(err, VK_ERROR_INITIALIZATION_FAILED);
}

TEST_F(TestsMechanism, capabilities_or) {
    TEST_DESCRIPTION("Test profile using capabilities or");

    const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_capabilities.json";
    const char* profile_name_data = "VP_LUNARG_test_capabilities_or";
    VkBool32 emulate_portability_data = VK_TRUE;
    const std::vector<const char*> simulate_capabilities = {"SIMULATE_MAX_ENUM"};
    VkBool32 debug_fail_on_error = VK_TRUE;

    std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
        {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data},
        {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]},
        {kLayerName, kLayerSettingsDebugFailOnError, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &debug_fail_on_error}};

    profiles_test::VulkanInstanceBuilder inst_builder;
    VkResult err = inst_builder.init(settings);
    EXPECT_EQ(err, VK_SUCCESS);
}
