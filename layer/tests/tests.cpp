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

#include <gtest/gtest.h>
#include "profiles_test_helper.h"

#include <array>

class LayerTests : public VkTestFramework {
  public:
   LayerTests(){};
   ~LayerTests(){};

    static void SetUpTestSuite() {
    }

    static void TearDownTestSuite(){};
};

#ifndef __ANDROID__
TEST_F(LayerTests, TestDesktop2023) {
    TEST_DESCRIPTION("Test specifying device extensions to be excluded from being reported by the device");

    static const char* profile_dir_data = JSON_PROFILES_PATH;
    static const char* profile_name_data = "VP_LUNARG_desktop_baseline_2023";
    VkBool32 emulate_portability_data = VK_TRUE;
    VkBool32 debug_fail_on_error = VK_FALSE;
    const std::vector<const char*> debug_reports = {"DEBUG_REPORT_ERROR_BIT", "DEBUG_REPORT_WARNING_BIT",
                                                    "DEBUG_REPORT_NOTIFICATION_BIT", "DEBUG_REPORT_DEBUG_BIT"};

    std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileDirs, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, {&profile_dir_data}},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, {&profile_name_data}},
        {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data},
        {kLayerName, kLayerSettingsDebugFailOnError, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &debug_fail_on_error},
        {kLayerName, kLayerSettingsDebugReports, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(debug_reports.size()), &debug_reports[0]}
    };

    profiles_test::VulkanInstanceBuilder inst_builder;
    VkResult err = inst_builder.init(settings);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice gpu;
    err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);
    if (err != VK_SUCCESS) {
        printf("Profile not supported on device, skipping test.\n");
        return;
    }

    VkPhysicalDeviceProperties gpu_props{};
    vkGetPhysicalDeviceProperties(gpu, &gpu_props);

    ASSERT_EQ(gpu_props.limits.maxImageDimension1D, 16384);
    ASSERT_EQ(gpu_props.limits.maxImageDimension2D, 16384);
    ASSERT_EQ(gpu_props.limits.maxImageDimension3D, 2048);
}
#endif//__ANDROID__

TEST_F(LayerTests, TestSetCombinationMode) {
    TEST_DESCRIPTION("Test different profile capabilities settings");

    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    std::vector<VkExtensionProperties> device_extensions;
    {
        err = inst_builder.init();
        ASSERT_EQ(err, VK_SUCCESS);


        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(profiles_test::MODE_NATIVE, &gpu);

        if (err != VK_SUCCESS) {
            printf("Profile not supported on device, skipping test.\n");
        } else {
            uint32_t count;
            vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, nullptr);
            device_extensions.resize(count);
            vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, device_extensions.data());
        }

        inst_builder.reset();
    }

    {
        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_api.json";
        const char* profile_name_data = "VP_LUNARG_test_api";
        VkBool32 emulate_portability_data = VK_TRUE;
        const char* simulate_capabilities = "";

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, {&profile_file_data}},
            {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, {&profile_name_data}},
            {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data},
            {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, 0, {&simulate_capabilities}}
        };

        err = inst_builder.init(settings);
        ASSERT_EQ(err, VK_SUCCESS);

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);

        if (err != VK_SUCCESS) {
            printf("Profile not supported on device, skipping test.\n");
        } else {
            VkPhysicalDeviceProperties gpu_props{};
            vkGetPhysicalDeviceProperties(gpu, &gpu_props);

            VkPhysicalDeviceFeatures gpu_feats{};
            vkGetPhysicalDeviceFeatures(gpu, &gpu_feats);

            uint32_t count;
            vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, nullptr);
            std::vector<VkExtensionProperties> extensions(count);
            vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, extensions.data());

            size_t portability_subset_add = 1;
            for (const auto& ext : device_extensions) {
                if (strcmp(ext.extensionName, "VK_KHR_portability_subset") == 0) {
                    portability_subset_add = 0;
                    break;
                }
            }
            ASSERT_EQ(device_extensions.size() + portability_subset_add, extensions.size());
        }

        inst_builder.reset();
    }

    {
        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_api.json";
        const char* profile_name_data = "VP_LUNARG_test_api";
        VkBool32 emulate_portability_data = VK_TRUE;
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_EXTENSIONS_BIT"};

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
        } else {
            VkPhysicalDeviceProperties gpu_props{};
            vkGetPhysicalDeviceProperties(gpu, &gpu_props);

            VkPhysicalDeviceFeatures gpu_feats{};
            vkGetPhysicalDeviceFeatures(gpu, &gpu_feats);

            uint32_t count;
            vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, nullptr);
            std::vector<VkExtensionProperties> extensions(count);
            vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, extensions.data());

            ASSERT_EQ(268, extensions.size()); // Number of extensions in "VP_LUNARG_test_api.json"
        }

        inst_builder.reset();
    }

    {
        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_api.json";
        const char* profile_name_data = "VP_LUNARG_test_api";
        VkBool32 emulate_portability_data = VK_TRUE;
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_EXTENSIONS_BIT"};

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
        } else {
            VkPhysicalDeviceProperties gpu_props{};
            vkGetPhysicalDeviceProperties(gpu, &gpu_props);

            VkPhysicalDeviceFeatures gpu_feats{};
            vkGetPhysicalDeviceFeatures(gpu, &gpu_feats);

            uint32_t count;
            vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, nullptr);
            std::vector<VkExtensionProperties> extensions(count);
            vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, extensions.data());

            ASSERT_GE(extensions.size(), 234);
        }

        inst_builder.reset();
    }

}

TEST_F(LayerTests, TestExtensionNotSupported) {
    TEST_DESCRIPTION("Test using a profile with an unsupported extension for the device");

    const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_api.json";
    const char* profile_name_data = "VP_LUNARG_test_api";
    VkBool32 emulate_portability_data = VK_TRUE;
    const std::vector<const char*> simulate_capabilities = {"SIMULATE_EXTENSIONS_BIT"};
    VkBool32 debug_fail_on_error = VK_TRUE;

    std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
        {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data},
        {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]},
        {kLayerName, kLayerSettingsDebugFailOnError, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &debug_fail_on_error}};

    profiles_test::VulkanInstanceBuilder inst_builder;
    VkResult err = inst_builder.init(settings);
    ASSERT_EQ(err, VK_SUCCESS);

    std::vector<VkExtensionProperties> device_extensions;
    {
        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(profiles_test::MODE_NATIVE, &gpu);
        ASSERT_EQ(err, VK_SUCCESS);

        uint32_t count;
        vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, nullptr);
        device_extensions.resize(count);
        vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, device_extensions.data());
    }

    {
        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);

        if (device_extensions.size() < 233) {
            ASSERT_EQ(err, VK_ERROR_INITIALIZATION_FAILED);
        }
    }
}

#ifndef __ANDROID__
TEST_F(LayerTests, TestExcludingDeviceExtensions) {
    TEST_DESCRIPTION("Test specifying device extensions to be excluded from being reported by the device");

    VkResult err = VK_SUCCESS;

    const char* profile_file_data = JSON_PROFILES_PATH "VP_LUNARG_desktop_baseline.json";
    const char* profile_name_data = "VP_LUNARG_desktop_baseline_2023";
    VkBool32 emulate_portability_data = VK_TRUE;
    VkBool32 debug_fail_on_error = VK_FALSE;
    const std::vector<const char*> exclude_device_extensions = {
        VK_KHR_MAINTENANCE_1_EXTENSION_NAME,
        VK_KHR_MAINTENANCE_2_EXTENSION_NAME,
        VK_KHR_MAINTENANCE_3_EXTENSION_NAME,
        VK_KHR_MAINTENANCE_4_EXTENSION_NAME
    };

    std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
        {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data},
        {kLayerName, kLayerSettingsExcludeDeviceExtensions, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(exclude_device_extensions.size()), &exclude_device_extensions[0]},
        {kLayerName, kLayerSettingsDebugFailOnError, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &debug_fail_on_error}};

    profiles_test::VulkanInstanceBuilder inst_builder;
    err = inst_builder.init(settings);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice gpu;
    err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);
    if (err != VK_SUCCESS) {
        printf("Profile not supported on device, skipping test.\n");
        return;
    }

    uint32_t count = 0;
    vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, nullptr);
    std::vector<VkExtensionProperties> device_extensions(count);
    vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, device_extensions.data());

    bool maintenance1 = false;
    bool maintenance2 = false;
    bool maintenance3 = false;
    bool maintenance4 = false;

    for (const auto& ext : device_extensions) {
        if (strcmp(ext.extensionName, VK_KHR_MAINTENANCE_1_EXTENSION_NAME) == 0) {
            maintenance1 = true;
        }
        if (strcmp(ext.extensionName, VK_KHR_MAINTENANCE_2_EXTENSION_NAME) == 0) {
            maintenance2 = true;
        }
        if (strcmp(ext.extensionName, VK_KHR_MAINTENANCE_3_EXTENSION_NAME) == 0) {
            maintenance3 = true;
        }
        if (strcmp(ext.extensionName, VK_KHR_MAINTENANCE_4_EXTENSION_NAME) == 0) {
            maintenance4 = true;
        }
    }

    ASSERT_FALSE(maintenance1);
    ASSERT_FALSE(maintenance2);
    ASSERT_FALSE(maintenance3);
    ASSERT_FALSE(maintenance4);
}
#endif

#ifndef __ANDROID__
TEST_F(LayerTests, TestExcludingFormats) {
    TEST_DESCRIPTION("Test specifying image formats to be excluded from being reported by the device");

    const char* profile_file_data = JSON_PROFILES_PATH "VP_LUNARG_desktop_baseline.json";
    VkBool32 emulate_portability_data = VK_TRUE;
    VkBool32 debug_fail_on_error = VK_TRUE;
    const std::vector<const char*> exclude_formats = {"VK_FORMAT_R8G8B8A8_UNORM"};

    std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
        {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data},
        {kLayerName, kLayerSettingsDebugFailOnError, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &debug_fail_on_error},
        {kLayerName, kLayerSettingsExcludeFormats, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(exclude_formats.size()), &exclude_formats[0]}};

    profiles_test::VulkanInstanceBuilder inst_builder;
    VkResult err = inst_builder.init(settings);    
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice gpu;
    err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);
    if (err != VK_SUCCESS) {
        printf("Profile not supported on device, skipping test.\n");
        return;
    }

    VkFormatProperties format_properties = {};
    vkGetPhysicalDeviceFormatProperties(gpu, VK_FORMAT_R8G8B8A8_UNORM, &format_properties);

    ASSERT_EQ(format_properties.linearTilingFeatures, 0);
    ASSERT_EQ(format_properties.optimalTilingFeatures, 0);
    ASSERT_EQ(format_properties.bufferFeatures, 0);
}
#endif

TEST_F(LayerTests, TestProfileLayerSettingsEXT_EnumerateExtensions) {
    const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_device_extensions.json";
    const char* profile_name_data = "VP_LUNARG_test_device_extensions";
    VkBool32 emulate_portability_data = VK_FALSE;
    VkBool32 debug_fail_on_error = VK_FALSE;
    const std::vector<const char*> simulate_capabilities = {"SIMULATE_MAX_ENUM"};

    std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
        {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data},
        {kLayerName, kLayerSettingsDebugFailOnError, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &debug_fail_on_error},
        {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]}};

    profiles_test::VulkanInstanceBuilder inst_builder;
    VkResult err = inst_builder.init(VK_API_VERSION_1_0, settings);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice gpu;
    err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);

    uint32_t count = 0;
    vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, nullptr);
    ASSERT_EQ(count, 1);
}

TEST_F(LayerTests, TestNotSettingProfileFile) {
    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    std::vector<VkExtensionProperties> extensions_default;
    {
        err = inst_builder.init(VK_API_VERSION_1_0);
        ASSERT_EQ(err, VK_SUCCESS);

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);
        ASSERT_EQ(err, VK_SUCCESS);

        uint32_t count = 0;
        vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, nullptr);
        extensions_default.resize(count);
        vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, extensions_default.data());

        inst_builder.reset();
    }

    std::vector<VkExtensionProperties> extensions_settings;
    {
        std::vector<VkLayerSettingEXT> settings;

        err = inst_builder.init(VK_API_VERSION_1_0, settings);
        ASSERT_EQ(err, VK_SUCCESS);

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);
        ASSERT_EQ(err, VK_SUCCESS);

        uint32_t count = 0;
        vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, nullptr);
        extensions_settings.resize(count);
        vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, extensions_settings.data());

        inst_builder.reset();
    }

    ASSERT_EQ(extensions_settings.size(), extensions_default.size());
}

TEST_F(LayerTests, TestExcludedExtensions) {
#ifdef VK_EXT_shader_atomic_float2
    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    VkPhysicalDeviceTransformFeedbackPropertiesEXT device_properties{};
    device_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT;
    {
        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_api.json";
        const char* profile_name_data = "VP_LUNARG_test_api";
        VkBool32 emulate_portability_data = VK_TRUE;
        VkBool32 debug_fail_on_error = VK_FALSE;
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_API_VERSION_BIT"};
        const char* debug_reports = "";

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
            {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
            {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data},
            {kLayerName, kLayerSettingsDebugFailOnError, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &debug_fail_on_error},
            {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]},
            {kLayerName, kLayerSettingsDebugReports, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &debug_reports}
        };

        err = inst_builder.init(VK_API_VERSION_1_3, settings);
        ASSERT_EQ(err, VK_SUCCESS);

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);

        VkPhysicalDeviceProperties2 properties;
        properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        properties.pNext = &device_properties;
        vkGetPhysicalDeviceProperties2(gpu, &properties);
        inst_builder.reset();
    }

    {
        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_api.json";
        const char* profile_name_data = "VP_LUNARG_test_api";
        VkBool32 emulate_portability_data = VK_TRUE;
        VkBool32 debug_fail_on_error = VK_FALSE;
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_MAX_ENUM"};
        const std::vector<const char*> exclude_device_extensions = {VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME};
        const char* debug_reports = "";

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
            {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
            {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data},
            {kLayerName, kLayerSettingsDebugFailOnError, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &debug_fail_on_error},
            {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]},
            {kLayerName, kLayerSettingsExcludeDeviceExtensions, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(exclude_device_extensions.size()), &exclude_device_extensions[0]},
            {kLayerName, kLayerSettingsDebugReports, VK_LAYER_SETTING_TYPE_STRING_EXT, 0, {&debug_reports}}
        };

        err = inst_builder.init(VK_API_VERSION_1_3, settings);
        ASSERT_EQ(err, VK_SUCCESS);

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);

        VkPhysicalDeviceTransformFeedbackPropertiesEXT profile_properties{};
        profile_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT;

        VkPhysicalDeviceProperties2 properties;
        properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        properties.pNext = &profile_properties;
        vkGetPhysicalDeviceProperties2(gpu, &properties);

        ASSERT_EQ(device_properties.maxTransformFeedbackStreams, profile_properties.maxTransformFeedbackStreams);
        ASSERT_EQ(device_properties.maxTransformFeedbackBuffers, profile_properties.maxTransformFeedbackBuffers);
        ASSERT_EQ(device_properties.maxTransformFeedbackBufferSize, profile_properties.maxTransformFeedbackBufferSize);
        ASSERT_EQ(device_properties.maxTransformFeedbackStreamDataSize, profile_properties.maxTransformFeedbackStreamDataSize);
        ASSERT_EQ(device_properties.maxTransformFeedbackBufferDataSize, profile_properties.maxTransformFeedbackBufferDataSize);
        ASSERT_EQ(device_properties.maxTransformFeedbackBufferDataStride, profile_properties.maxTransformFeedbackBufferDataStride);
        ASSERT_EQ(device_properties.transformFeedbackQueries, profile_properties.transformFeedbackQueries);
        ASSERT_EQ(device_properties.transformFeedbackStreamsLinesTriangles, profile_properties.transformFeedbackStreamsLinesTriangles);
        ASSERT_EQ(device_properties.transformFeedbackRasterizationStreamSelect, profile_properties.transformFeedbackRasterizationStreamSelect);
        ASSERT_EQ(device_properties.transformFeedbackDraw, profile_properties.transformFeedbackDraw);
    }
#endif
}

TEST_F(LayerTests, TestQueueFamilyProperties) {
    VkResult err = VK_SUCCESS;

    const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_vkqueuefamilyproperties.json";
    const char* profile_name_data = "VP_LUNARG_test_vkqueuefamilyproperties";
    const std::vector<const char*> simulate_capabilities = {"SIMULATE_QUEUE_FAMILY_PROPERTIES_BIT"};
    VkBool32 emulate_portability_data = VK_TRUE;
    VkBool32 debug_fail_on_error = VK_FALSE;
    const char* debug_reports = "";

    std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, {&profile_file_data}},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, {&profile_name_data}},
        {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]},
        {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data},
        {kLayerName, kLayerSettingsDebugFailOnError, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &debug_fail_on_error},
        {kLayerName, kLayerSettingsDebugReports, VK_LAYER_SETTING_TYPE_STRING_EXT, 0, {&debug_reports}}
    };

    profiles_test::VulkanInstanceBuilder inst_builder;
    err = inst_builder.init(settings);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice gpu;
    err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);

    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &count, nullptr);
    std::vector<VkQueueFamilyProperties> qf_props(count);
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &count, qf_props.data());

    ASSERT_EQ(qf_props.size(), 2);
    ASSERT_EQ(qf_props[0].queueFlags, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT);
    ASSERT_EQ(qf_props[0].queueCount, 1);
    ASSERT_EQ(qf_props[0].timestampValidBits, 36);
    ASSERT_EQ(qf_props[0].minImageTransferGranularity.width, 1);
    ASSERT_EQ(qf_props[0].minImageTransferGranularity.height, 1);
    ASSERT_EQ(qf_props[0].minImageTransferGranularity.depth, 1);

    ASSERT_EQ(qf_props[1].queueFlags, VK_QUEUE_PROTECTED_BIT);
    ASSERT_EQ(qf_props[1].queueCount, 2);
    ASSERT_EQ(qf_props[1].timestampValidBits, 42);
    ASSERT_EQ(qf_props[1].minImageTransferGranularity.width, 16);
    ASSERT_EQ(qf_props[1].minImageTransferGranularity.height, 32);
    ASSERT_EQ(qf_props[1].minImageTransferGranularity.depth, 64);
}

TEST_F(LayerTests, TestQueueFamilyPropertiesGlobalPriorityProperties) {
    VkResult err = VK_SUCCESS;

    const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_vkqueuefamilyproperties.json";
    const char* profile_name_data = "VP_LUNARG_test_vkqueuefamilyproperties";
    const std::vector<const char*> simulate_capabilities = {"SIMULATE_QUEUE_FAMILY_PROPERTIES_BIT"};
    VkBool32 emulate_portability_data = VK_TRUE;
    VkBool32 debug_fail_on_error = VK_FALSE;
    const char* debug_reports= "";

    std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
        {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]},
        {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data},
        {kLayerName, kLayerSettingsDebugFailOnError, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &debug_fail_on_error},
        {kLayerName, kLayerSettingsDebugReports, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &debug_reports}
    };

    profiles_test::VulkanInstanceBuilder inst_builder;
    err = inst_builder.init(settings);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice gpu;
    err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);

    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties2(gpu, &count, nullptr);
    std::vector<VkQueueFamilyProperties2> qf_props2(count);

    VkQueueFamilyGlobalPriorityPropertiesKHR qfgp = {};
    qfgp.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_GLOBAL_PRIORITY_PROPERTIES_KHR;
    qf_props2[0].pNext = &qfgp;

    vkGetPhysicalDeviceQueueFamilyProperties2(gpu, &count, qf_props2.data());
    ASSERT_EQ(qfgp.priorityCount, 4);
    ASSERT_EQ(qfgp.priorities[0], VK_QUEUE_GLOBAL_PRIORITY_LOW_EXT);
    ASSERT_EQ(qfgp.priorities[1], VK_QUEUE_GLOBAL_PRIORITY_MEDIUM_EXT);
    ASSERT_EQ(qfgp.priorities[2], VK_QUEUE_GLOBAL_PRIORITY_HIGH_EXT);
    ASSERT_EQ(qfgp.priorities[3], VK_QUEUE_GLOBAL_PRIORITY_REALTIME_EXT);
}

TEST_F(LayerTests, TestQueueFamilyCheckpointProperties) {
    TEST_DESCRIPTION("Test profile with VK_NV_device_diagnostic_checkpoints extension");

    VkResult err = VK_SUCCESS;

    const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_vkqueuefamilyproperties.json";
    const char* profile_name_data = "VP_LUNARG_test_vkqueuefamilyproperties";
    const std::vector<const char*> simulate_capabilities = {"SIMULATE_QUEUE_FAMILY_PROPERTIES_BIT"};
    VkBool32 emulate_portability_data = VK_TRUE;
    VkBool32 debug_fail_on_error = VK_FALSE;
    const char* debug_reports = "";

    std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, {&profile_file_data}},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, {&profile_name_data}},
        {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(simulate_capabilities.size()), {&simulate_capabilities[0]}},
        {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data},
        {kLayerName, kLayerSettingsDebugFailOnError, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &debug_fail_on_error},
        {kLayerName, kLayerSettingsDebugReports, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, {&debug_reports}}
    };

    profiles_test::VulkanInstanceBuilder inst_builder;
    err = inst_builder.init(settings);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice gpu;
    err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);

    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties2(gpu, &count, nullptr);
    std::vector<VkQueueFamilyProperties2> qf_props2(count);

    VkQueueFamilyCheckpointPropertiesNV checkpoint = {};
    checkpoint.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_CHECKPOINT_PROPERTIES_NV;
    qf_props2[0].pNext = &checkpoint;

    vkGetPhysicalDeviceQueueFamilyProperties2(gpu, &count, qf_props2.data());
    ASSERT_EQ(checkpoint.checkpointExecutionStageMask, VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_HOST_BIT);
}

TEST_F(LayerTests, TestQueueFamilyCheckpointProperties2) {
    TEST_DESCRIPTION("Test profile with VK_NV_device_diagnostic_checkpoints extension");

    VkResult err = VK_SUCCESS;

    const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_vkqueuefamilyproperties.json";
    const char* profile_name_data = "VP_LUNARG_test_vkqueuefamilyproperties";
    const std::vector<const char*> simulate_capabilities = {"SIMULATE_QUEUE_FAMILY_PROPERTIES_BIT"};
    VkBool32 emulate_portability_data = VK_TRUE;
    VkBool32 debug_fail_on_error = VK_FALSE;
    const char* debug_reports = "";

    std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, {&profile_file_data}},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, {&profile_name_data}},
        {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(simulate_capabilities.size()), {&simulate_capabilities[0]}},
        {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data},
        {kLayerName, kLayerSettingsDebugFailOnError, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &debug_fail_on_error},
        {kLayerName, kLayerSettingsDebugReports, VK_LAYER_SETTING_TYPE_STRING_EXT, 0, {&debug_reports}}
    };

    profiles_test::VulkanInstanceBuilder inst_builder;
    err = inst_builder.init(settings);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice gpu;
    err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);

    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties2(gpu, &count, nullptr);
    std::vector<VkQueueFamilyProperties2> qf_props2(count);

    VkQueueFamilyCheckpointProperties2NV checkpoint = {};
    checkpoint.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_CHECKPOINT_PROPERTIES_2_NV;
    qf_props2[0].pNext = &checkpoint;

    vkGetPhysicalDeviceQueueFamilyProperties2(gpu, &count, qf_props2.data());
    ASSERT_EQ(checkpoint.checkpointExecutionStageMask, VK_PIPELINE_STAGE_2_NONE_KHR |
                                                           VK_PIPELINE_STAGE_2_SUBPASS_SHADING_BIT_HUAWEI |
                                                           VK_PIPELINE_STAGE_2_INVOCATION_MASK_BIT_HUAWEI);
}

TEST_F(LayerTests, TestQueueFamilyPropertiesPartial) {
    TEST_DESCRIPTION("Test profile with queue family properties that does not specify all members");

    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    std::vector<VkQueueFamilyProperties> device_qf_props;
    {
        err = inst_builder.init();
        ASSERT_EQ(err, VK_SUCCESS);

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(profiles_test::MODE_NATIVE, &gpu);
        ASSERT_EQ(err, VK_SUCCESS);

        uint32_t count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(gpu, &count, nullptr);
        ASSERT_GT(count, static_cast<uint32_t>(0));
        if (count > 0) {
            device_qf_props.resize(count);
            vkGetPhysicalDeviceQueueFamilyProperties(gpu, &count, device_qf_props.data());
        }

        inst_builder.reset();
    }

    {
        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_vkqueuefamilyproperties.json";
        const char* profile_name_data = "VP_LUNARG_test_vkqueuefamilyproperties2";
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_QUEUE_FAMILY_PROPERTIES_BIT"};
        VkBool32 emulate_portability_data = VK_TRUE;
        VkBool32 debug_fail_on_error = VK_TRUE;

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
            {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
            {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]},
            {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data},
            {kLayerName, kLayerSettingsDebugFailOnError, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &debug_fail_on_error}
        };

        err = inst_builder.init(settings);
        if (err != VK_SUCCESS) {
            printf("Profile not supported on device, skipping test.\n");
            return;
        }

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);
        if (err != VK_SUCCESS) {
            printf("Profile not supported on device, skipping test.\n");
            return;
        }

        uint32_t count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(gpu, &count, nullptr);
        ASSERT_GT(count, static_cast<uint32_t>(0));
        if (count > 0) {
            std::vector<VkQueueFamilyProperties> qf_props(count);
            vkGetPhysicalDeviceQueueFamilyProperties(gpu, &count, qf_props.data());

            int32_t device_queue_index = -1;
            for (uint32_t i = 0; i < device_qf_props.size(); ++i) {
                if ((device_qf_props[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == VK_QUEUE_COMPUTE_BIT &&
                    device_qf_props[i].minImageTransferGranularity.width <= 4 &&
                    device_qf_props[i].minImageTransferGranularity.height <= 4) {
                    device_queue_index = static_cast<int32_t>(i);
                    break;
                }
            }

            ASSERT_EQ(count, 1u);
            ASSERT_EQ(qf_props[0].queueFlags, VK_QUEUE_COMPUTE_BIT);
            ASSERT_EQ(qf_props[0].queueCount, 1u);
            ASSERT_EQ(qf_props[0].minImageTransferGranularity.width, 4u);
            ASSERT_EQ(qf_props[0].minImageTransferGranularity.height, 4u);

            if (device_queue_index != -1) {
                ASSERT_EQ(qf_props[0].minImageTransferGranularity.depth, device_qf_props[static_cast<std::size_t>(device_queue_index)].minImageTransferGranularity.depth);
                ASSERT_EQ(qf_props[0].timestampValidBits, device_qf_props[static_cast<std::size_t>(device_queue_index)].timestampValidBits);
            }
        }
    }
}
