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

class TestsMechanismApiVersion : public VkTestFramework {
   public:
    TestsMechanismApiVersion(){};
    ~TestsMechanismApiVersion(){};

    static void SetUpTestSuite() {}
    static void TearDownTestSuite(){};
};

static std::string format(const char* message, ...) {
    std::size_t const STRING_BUFFER(4096);

    assert(message != nullptr);
    assert(strlen(message) >= 0 && strlen(message) < STRING_BUFFER);

    char buffer[STRING_BUFFER];
    va_list list;

    va_start(list, message);
    vsnprintf(buffer, STRING_BUFFER, message, list);
    va_end(list);

    return buffer;
}

TEST_F(TestsMechanismApiVersion, api_version_properties) {
    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    {
        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_api_version.json";

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, {&profile_file_data}}
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

        std::string version = format("%d.%d.%d",
            VK_API_VERSION_MAJOR(gpu_props.apiVersion),
            VK_API_VERSION_MINOR(gpu_props.apiVersion),
            VK_API_VERSION_PATCH(gpu_props.apiVersion));

        EXPECT_STREQ("1.1.128", version.c_str());
        EXPECT_EQ(4198528, gpu_props.apiVersion);
        EXPECT_EQ(4096u, gpu_props.limits.maxImageDimension1D);

        inst_builder.reset();
    }
}

TEST_F(TestsMechanismApiVersion, api_version_profile) {
    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    {
        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_api_version_profile.json";

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, {&profile_file_data}}};

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

        std::string version = format("%d.%d.%d",
            VK_API_VERSION_MAJOR(gpu_props.apiVersion),
            VK_API_VERSION_MINOR(gpu_props.apiVersion),
            VK_API_VERSION_PATCH(gpu_props.apiVersion));

        EXPECT_STREQ("1.1.142", version.c_str());
        EXPECT_EQ(4198542, gpu_props.apiVersion);
        EXPECT_EQ(4096u, gpu_props.limits.maxImageDimension1D);

        inst_builder.reset();
    }
}

TEST_F(TestsMechanismApiVersion, api_versions) {
    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    // Vulkan 1.0
    {
        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_api_1_0.json";
        const char* profile_name_data = "VP_LUNARG_test_api_1_0";
        VkBool32 emulate_portability_data = VK_FALSE;
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_API_VERSION_BIT", "SIMULATE_EXTENSIONS_BIT"};
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
        ASSERT_EQ(result, VK_SUCCESS);

        EXPECT_EQ(0, extCount);

        inst_builder.reset();
    }

    // Vulkan 1.0 + emulate_portability = true
    {
        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_api_1_0.json";
        const char* profile_name_data = "VP_LUNARG_test_api_1_0";
        VkBool32 emulate_portability_data = VK_TRUE;
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_API_VERSION_BIT", "SIMULATE_EXTENSIONS_BIT"};
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
        ASSERT_EQ(result, VK_SUCCESS);

        EXPECT_EQ(1, extCount);

        std::vector<VkExtensionProperties> ext(extCount);
        result = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extCount, ext.data());
        ASSERT_EQ(result, VK_SUCCESS);

        EXPECT_STREQ(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME, ext[0].extensionName);

        inst_builder.reset();
    }

    // Vulkan 1.1
    {
        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_api_1_1.json";
        const char* profile_name_data = "VP_LUNARG_test_api_1_1";
        VkBool32 emulate_portability_data = VK_FALSE;
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_API_VERSION_BIT", "SIMULATE_EXTENSIONS_BIT"};
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
        ASSERT_EQ(result, VK_SUCCESS);

        EXPECT_EQ(23, extCount);

        std::vector<VkExtensionProperties> ext(extCount);
        result = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extCount, ext.data());
        ASSERT_EQ(result, VK_SUCCESS);
        ASSERT_EQ(ext.size(), extCount);
        std::array expected_extensions = {VK_KHR_MULTIVIEW_EXTENSION_NAME,
                                          VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
                                          VK_KHR_DEVICE_GROUP_EXTENSION_NAME,
                                          VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
                                          VK_KHR_MAINTENANCE_1_EXTENSION_NAME,
                                          VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME,
                                          VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
                                          VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME,
                                          VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME,
                                          VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME,
                                          VK_KHR_16BIT_STORAGE_EXTENSION_NAME,
                                          VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE_EXTENSION_NAME,
                                          VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME,
                                          VK_KHR_EXTERNAL_FENCE_EXTENSION_NAME,
                                          VK_KHR_MAINTENANCE_2_EXTENSION_NAME,
                                          VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME,
                                          VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
                                          VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME,
                                          VK_KHR_RELAXED_BLOCK_LAYOUT_EXTENSION_NAME,
                                          VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
                                          VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME,
                                          VK_KHR_BIND_MEMORY_2_EXTENSION_NAME,
                                          VK_KHR_MAINTENANCE_3_EXTENSION_NAME};
        for (const auto& extension : ext) {
            bool found = false;
            for (const auto& expected : expected_extensions) {
                if (strcmp(expected, extension.extensionName) == 0) {
                    ASSERT_FALSE(found);  // make sure there are no duplicates
                    found = true;
                }
            }
            ASSERT_TRUE(found);
        }

        inst_builder.reset();
    }

    // Vulkan 1.1 + emulate_portability = true
    {
        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_api_1_1.json";
        const char* profile_name_data = "VP_LUNARG_test_api_1_1";
        VkBool32 emulate_portability_data = VK_TRUE;
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_API_VERSION_BIT", "SIMULATE_EXTENSIONS_BIT"};
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
        ASSERT_EQ(result, VK_SUCCESS);

        EXPECT_EQ(24, extCount);

        std::vector<VkExtensionProperties> ext(extCount);
        result = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extCount, ext.data());
        ASSERT_EQ(result, VK_SUCCESS);
        ASSERT_EQ(ext.size(), extCount);
        std::array expected_extensions = {VK_KHR_MULTIVIEW_EXTENSION_NAME,
                                          VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
                                          VK_KHR_DEVICE_GROUP_EXTENSION_NAME,
                                          VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
                                          VK_KHR_MAINTENANCE_1_EXTENSION_NAME,
                                          VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME,
                                          VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
                                          VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME,
                                          VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME,
                                          VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME,
                                          VK_KHR_16BIT_STORAGE_EXTENSION_NAME,
                                          VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE_EXTENSION_NAME,
                                          VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME,
                                          VK_KHR_EXTERNAL_FENCE_EXTENSION_NAME,
                                          VK_KHR_MAINTENANCE_2_EXTENSION_NAME,
                                          VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME,
                                          VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
                                          VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME,
                                          VK_KHR_RELAXED_BLOCK_LAYOUT_EXTENSION_NAME,
                                          VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
                                          VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME,
                                          VK_KHR_BIND_MEMORY_2_EXTENSION_NAME,
                                          VK_KHR_MAINTENANCE_3_EXTENSION_NAME,
                                          VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME};
        for (const auto& extension : ext) {
            bool found = false;
            for (const auto& expected : expected_extensions) {
                if (strcmp(expected, extension.extensionName) == 0) {
                    ASSERT_FALSE(found);  // make sure there are no duplicates
                    found = true;
                }
            }
            ASSERT_TRUE(found);
        }

        inst_builder.reset();
    }

    // Vulkan 1.2
    {
        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_api_1_2.json";
        const char* profile_name_data = "VP_LUNARG_test_api_1_2";
        VkBool32 emulate_portability_data = VK_FALSE;
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_API_VERSION_BIT", "SIMULATE_EXTENSIONS_BIT"};
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
        ASSERT_EQ(result, VK_SUCCESS);

        EXPECT_EQ(47, extCount);

        std::vector<VkExtensionProperties> ext(extCount);
        result = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extCount, ext.data());
        ASSERT_EQ(result, VK_SUCCESS);
        ASSERT_EQ(ext.size(), extCount);
        std::array expected_extensions = {VK_KHR_MULTIVIEW_EXTENSION_NAME,
                                          VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
                                          VK_KHR_DEVICE_GROUP_EXTENSION_NAME,
                                          VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
                                          VK_KHR_MAINTENANCE_1_EXTENSION_NAME,
                                          VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME,
                                          VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
                                          VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME,
                                          VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME,
                                          VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME,
                                          VK_KHR_16BIT_STORAGE_EXTENSION_NAME,
                                          VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE_EXTENSION_NAME,
                                          VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME,
                                          VK_KHR_EXTERNAL_FENCE_EXTENSION_NAME,
                                          VK_KHR_MAINTENANCE_2_EXTENSION_NAME,
                                          VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME,
                                          VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
                                          VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME,
                                          VK_KHR_RELAXED_BLOCK_LAYOUT_EXTENSION_NAME,
                                          VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
                                          VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME,
                                          VK_KHR_BIND_MEMORY_2_EXTENSION_NAME,
                                          VK_KHR_MAINTENANCE_3_EXTENSION_NAME,
                                          VK_KHR_SAMPLER_MIRROR_CLAMP_TO_EDGE_EXTENSION_NAME,
                                          VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME,
                                          VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME,
                                          VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
                                          VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME,
                                          VK_KHR_IMAGE_FORMAT_LIST_EXTENSION_NAME,
                                          VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
                                          VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME,
                                          VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME,
                                          VK_KHR_SHADER_SUBGROUP_EXTENDED_TYPES_EXTENSION_NAME,
                                          VK_KHR_8BIT_STORAGE_EXTENSION_NAME,
                                          VK_KHR_SHADER_ATOMIC_INT64_EXTENSION_NAME,
                                          VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME,
                                          VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME,
                                          VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
                                          VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
                                          VK_KHR_VULKAN_MEMORY_MODEL_EXTENSION_NAME,
                                          VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME,
                                          VK_KHR_SPIRV_1_4_EXTENSION_NAME,
                                          VK_KHR_SEPARATE_DEPTH_STENCIL_LAYOUTS_EXTENSION_NAME,
                                          VK_EXT_SEPARATE_STENCIL_USAGE_EXTENSION_NAME,
                                          VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT_EXTENSION_NAME,
                                          VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
                                          VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME};
        for (const auto& extension : ext) {
            bool found = false;
            for (const auto& expected : expected_extensions) {
                if (strcmp(expected, extension.extensionName) == 0) {
                    ASSERT_FALSE(found);  // make sure there are no duplicates
                    found = true;
                }
            }
            ASSERT_TRUE(found);
        }

        inst_builder.reset();
    }

    // Vulkan 1.3
#ifndef __APPLE__
    {
        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_api_1_3.json";
        const char* profile_name_data = "VP_LUNARG_test_api_1_3";
        VkBool32 emulate_portability_data = VK_FALSE;
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_API_VERSION_BIT", "SIMULATE_EXTENSIONS_BIT"};
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
        ASSERT_EQ(result, VK_SUCCESS);

        EXPECT_EQ(70, extCount);

        std::vector<VkExtensionProperties> ext(extCount);
        result = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extCount, ext.data());
        ASSERT_EQ(result, VK_SUCCESS);
        ASSERT_EQ(ext.size(), extCount);
        std::array expected_extensions = {VK_KHR_MULTIVIEW_EXTENSION_NAME,
                                          VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
                                          VK_KHR_DEVICE_GROUP_EXTENSION_NAME,
                                          VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
                                          VK_KHR_MAINTENANCE_1_EXTENSION_NAME,
                                          VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME,
                                          VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
                                          VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME,
                                          VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME,
                                          VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME,
                                          VK_KHR_16BIT_STORAGE_EXTENSION_NAME,
                                          VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE_EXTENSION_NAME,
                                          VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME,
                                          VK_KHR_EXTERNAL_FENCE_EXTENSION_NAME,
                                          VK_KHR_MAINTENANCE_2_EXTENSION_NAME,
                                          VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME,
                                          VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
                                          VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME,
                                          VK_KHR_RELAXED_BLOCK_LAYOUT_EXTENSION_NAME,
                                          VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
                                          VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME,
                                          VK_KHR_BIND_MEMORY_2_EXTENSION_NAME,
                                          VK_KHR_MAINTENANCE_3_EXTENSION_NAME,
                                          VK_KHR_SAMPLER_MIRROR_CLAMP_TO_EDGE_EXTENSION_NAME,
                                          VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME,
                                          VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME,
                                          VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
                                          VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME,
                                          VK_KHR_IMAGE_FORMAT_LIST_EXTENSION_NAME,
                                          VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
                                          VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME,
                                          VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME,
                                          VK_KHR_SHADER_SUBGROUP_EXTENDED_TYPES_EXTENSION_NAME,
                                          VK_KHR_8BIT_STORAGE_EXTENSION_NAME,
                                          VK_KHR_SHADER_ATOMIC_INT64_EXTENSION_NAME,
                                          VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME,
                                          VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME,
                                          VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
                                          VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
                                          VK_KHR_VULKAN_MEMORY_MODEL_EXTENSION_NAME,
                                          VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME,
                                          VK_KHR_SPIRV_1_4_EXTENSION_NAME,
                                          VK_KHR_SEPARATE_DEPTH_STENCIL_LAYOUTS_EXTENSION_NAME,
                                          VK_EXT_SEPARATE_STENCIL_USAGE_EXTENSION_NAME,
                                          VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT_EXTENSION_NAME,
                                          VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
                                          VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME,
                                          VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
                                          VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME,
                                          VK_EXT_PIPELINE_CREATION_FEEDBACK_EXTENSION_NAME,
                                          VK_EXT_TOOLING_INFO_EXTENSION_NAME,
                                          VK_KHR_SHADER_INTEGER_DOT_PRODUCT_EXTENSION_NAME,
                                          VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME,
                                          VK_EXT_PRIVATE_DATA_EXTENSION_NAME,
                                          VK_EXT_PIPELINE_CREATION_CACHE_CONTROL_EXTENSION_NAME,
                                          VK_KHR_ZERO_INITIALIZE_WORKGROUP_MEMORY_EXTENSION_NAME,
                                          VK_KHR_FORMAT_FEATURE_FLAGS_2_EXTENSION_NAME,
                                          VK_KHR_MAINTENANCE_4_EXTENSION_NAME,
                                          VK_EXT_TEXTURE_COMPRESSION_ASTC_HDR_EXTENSION_NAME,
                                          VK_KHR_SHADER_TERMINATE_INVOCATION_EXTENSION_NAME,
                                          VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME,
                                          VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME,
                                          VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME,
                                          VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME,
                                          VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
                                          VK_EXT_YCBCR_2PLANE_444_FORMATS_EXTENSION_NAME,
                                          VK_EXT_IMAGE_ROBUSTNESS_EXTENSION_NAME,
                                          VK_KHR_COPY_COMMANDS_2_EXTENSION_NAME,
                                          VK_EXT_4444_FORMATS_EXTENSION_NAME,
                                          VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME};
        for (const auto& extension : ext) {
            bool found = false;
            for (const auto& expected : expected_extensions) {
                if (strcmp(expected, extension.extensionName) == 0) {
                    ASSERT_FALSE(found);  // make sure there are no duplicates
                    found = true;
                }
            }
            ASSERT_TRUE(found);
        }

        inst_builder.reset();
    }
#endif//__APPLE__
}
