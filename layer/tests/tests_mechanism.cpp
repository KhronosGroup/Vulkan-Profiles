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
 * Author: Mark Lobodzinski <mark@lunarg.com>
 */

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_beta.h>

#include <gtest/gtest.h>
#include "profiles_test_helper.h"

class TestsMechanism : public VkTestFramework {
  public:
   TestsMechanism(){};
   ~TestsMechanism(){};

    static void SetUpTestSuite() {}

    static void TearDownTestSuite(){ 
    };
};

TEST_F(TestsMechanism, api_versions) {
    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    // Vulkan 1.0
    {
        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_api_1_0.json";
        settings.emulate_portability = false;
        settings.profile_name = "VP_LUNARG_test_api_1_0";
        settings.simulate_capabilities = SIMULATE_API_VERSION_BIT | SIMULATE_EXTENSIONS_BIT;
        settings.debug_reports = DEBUG_REPORT_MAX_ENUM;

        err = inst_builder.init(&settings);
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

        EXPECT_EQ(0, extCount);

        inst_builder.reset();
    }

    // Vulkan 1.0 + emulate_portability = true
    {
        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_api_1_0.json";
        settings.emulate_portability = true;
        settings.profile_name = "VP_LUNARG_test_api_1_0";
        settings.simulate_capabilities = SIMULATE_API_VERSION_BIT | SIMULATE_EXTENSIONS_BIT;
        settings.debug_reports = DEBUG_REPORT_MAX_ENUM;

        err = inst_builder.init(&settings);
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

        EXPECT_STREQ(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME, ext[0].extensionName);

        inst_builder.reset();
    }

    // Vulkan 1.1
    {
        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_api_1_1.json";
        settings.emulate_portability = false;
        settings.profile_name = "VP_LUNARG_test_api_1_1";
        settings.simulate_capabilities = SIMULATE_API_VERSION_BIT | SIMULATE_EXTENSIONS_BIT;
        settings.debug_reports = DEBUG_REPORT_MAX_ENUM;

        err = inst_builder.init(&settings);
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

        EXPECT_EQ(23, extCount);

        std::vector<VkExtensionProperties> ext(extCount);
        result = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extCount, ext.data());
        ASSERT_EQ(err, VK_SUCCESS);

        EXPECT_STREQ(VK_KHR_MULTIVIEW_EXTENSION_NAME, ext[0].extensionName);
        EXPECT_STREQ(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, ext[1].extensionName);
        EXPECT_STREQ(VK_KHR_DEVICE_GROUP_EXTENSION_NAME, ext[2].extensionName);
        EXPECT_STREQ(VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME, ext[3].extensionName);
        EXPECT_STREQ(VK_KHR_MAINTENANCE_1_EXTENSION_NAME, ext[4].extensionName);
        EXPECT_STREQ(VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME, ext[5].extensionName);
        EXPECT_STREQ(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME, ext[6].extensionName);
        EXPECT_STREQ(VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME, ext[7].extensionName);
        EXPECT_STREQ(VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME, ext[8].extensionName);
        EXPECT_STREQ(VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME, ext[9].extensionName);
        EXPECT_STREQ(VK_KHR_16BIT_STORAGE_EXTENSION_NAME, ext[10].extensionName);
        EXPECT_STREQ(VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE_EXTENSION_NAME, ext[11].extensionName);
        EXPECT_STREQ(VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME, ext[12].extensionName);
        EXPECT_STREQ(VK_KHR_EXTERNAL_FENCE_EXTENSION_NAME, ext[13].extensionName);
        EXPECT_STREQ(VK_KHR_MAINTENANCE_2_EXTENSION_NAME, ext[14].extensionName);
        EXPECT_STREQ(VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME, ext[15].extensionName);
        EXPECT_STREQ(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME, ext[16].extensionName);
        EXPECT_STREQ(VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME, ext[17].extensionName);
        EXPECT_STREQ(VK_KHR_RELAXED_BLOCK_LAYOUT_EXTENSION_NAME, ext[18].extensionName);
        EXPECT_STREQ(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME, ext[19].extensionName);
        EXPECT_STREQ(VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME, ext[20].extensionName);
        EXPECT_STREQ(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME, ext[21].extensionName);
        EXPECT_STREQ(VK_KHR_MAINTENANCE_3_EXTENSION_NAME, ext[22].extensionName);

        inst_builder.reset();
    }

    // Vulkan 1.1 + emulate_portability = true
    {
        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_api_1_1.json";
        settings.emulate_portability = true;
        settings.profile_name = "VP_LUNARG_test_api_1_1";
        settings.simulate_capabilities = SIMULATE_API_VERSION_BIT | SIMULATE_EXTENSIONS_BIT;
        settings.debug_reports = DEBUG_REPORT_MAX_ENUM;

        err = inst_builder.init(&settings);
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

        EXPECT_EQ(24, extCount);

        std::vector<VkExtensionProperties> ext(extCount);
        result = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extCount, ext.data());
        ASSERT_EQ(err, VK_SUCCESS);

        EXPECT_STREQ(VK_KHR_MULTIVIEW_EXTENSION_NAME, ext[0].extensionName);
        EXPECT_STREQ(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, ext[1].extensionName);
        EXPECT_STREQ(VK_KHR_DEVICE_GROUP_EXTENSION_NAME, ext[2].extensionName);
        EXPECT_STREQ(VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME, ext[3].extensionName);
        EXPECT_STREQ(VK_KHR_MAINTENANCE_1_EXTENSION_NAME, ext[4].extensionName);
        EXPECT_STREQ(VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME, ext[5].extensionName);
        EXPECT_STREQ(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME, ext[6].extensionName);
        EXPECT_STREQ(VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME, ext[7].extensionName);
        EXPECT_STREQ(VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME, ext[8].extensionName);
        EXPECT_STREQ(VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME, ext[9].extensionName);
        EXPECT_STREQ(VK_KHR_16BIT_STORAGE_EXTENSION_NAME, ext[10].extensionName);
        EXPECT_STREQ(VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE_EXTENSION_NAME, ext[11].extensionName);
        EXPECT_STREQ(VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME, ext[12].extensionName);
        EXPECT_STREQ(VK_KHR_EXTERNAL_FENCE_EXTENSION_NAME, ext[13].extensionName);
        EXPECT_STREQ(VK_KHR_MAINTENANCE_2_EXTENSION_NAME, ext[14].extensionName);
        EXPECT_STREQ(VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME, ext[15].extensionName);
        EXPECT_STREQ(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME, ext[16].extensionName);
        EXPECT_STREQ(VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME, ext[17].extensionName);
        EXPECT_STREQ(VK_KHR_RELAXED_BLOCK_LAYOUT_EXTENSION_NAME, ext[18].extensionName);
        EXPECT_STREQ(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME, ext[19].extensionName);
        EXPECT_STREQ(VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME, ext[20].extensionName);
        EXPECT_STREQ(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME, ext[21].extensionName);
        EXPECT_STREQ(VK_KHR_MAINTENANCE_3_EXTENSION_NAME, ext[22].extensionName);
        EXPECT_STREQ(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME, ext[23].extensionName);

        inst_builder.reset();
    }
    /*
    // Vulkan 1.2
    {
        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_api_1_2.json";
        settings.emulate_portability = false;
        settings.profile_name = "VP_LUNARG_test_api_1_2";
        settings.simulate_capabilities = SIMULATE_API_VERSION_BIT | SIMULATE_EXTENSIONS_BIT;
        settings.debug_reports = DEBUG_REPORT_MAX_ENUM;

        err = inst_builder.init(&settings);
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

        EXPECT_EQ(0, extCount);

        std::vector<VkExtensionProperties> ext(extCount);
        result = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extCount, ext.data());
        ASSERT_EQ(err, VK_SUCCESS);

        EXPECT_STREQ(VK_KHR_MULTIVIEW_EXTENSION_NAME, ext[0].extensionName);
        EXPECT_STREQ(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, ext[1].extensionName);
        EXPECT_STREQ(VK_KHR_DEVICE_GROUP_EXTENSION_NAME, ext[2].extensionName);
        EXPECT_STREQ(VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME, ext[3].extensionName);
        EXPECT_STREQ(VK_KHR_MAINTENANCE_1_EXTENSION_NAME, ext[4].extensionName);
        EXPECT_STREQ(VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME, ext[5].extensionName);
        EXPECT_STREQ(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME, ext[6].extensionName);
        EXPECT_STREQ(VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME, ext[7].extensionName);
        EXPECT_STREQ(VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME, ext[8].extensionName);
        EXPECT_STREQ(VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME, ext[9].extensionName);
        EXPECT_STREQ(VK_KHR_16BIT_STORAGE_EXTENSION_NAME, ext[10].extensionName);
        EXPECT_STREQ(VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE_EXTENSION_NAME, ext[11].extensionName);
        EXPECT_STREQ(VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME, ext[12].extensionName);
        EXPECT_STREQ(VK_KHR_EXTERNAL_FENCE_EXTENSION_NAME, ext[13].extensionName);
        EXPECT_STREQ(VK_KHR_MAINTENANCE_2_EXTENSION_NAME, ext[14].extensionName);
        EXPECT_STREQ(VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME, ext[15].extensionName);
        EXPECT_STREQ(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME, ext[16].extensionName);
        EXPECT_STREQ(VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME, ext[17].extensionName);
        EXPECT_STREQ(VK_KHR_RELAXED_BLOCK_LAYOUT_EXTENSION_NAME, ext[18].extensionName);
        EXPECT_STREQ(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME, ext[19].extensionName);
        EXPECT_STREQ(VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME, ext[20].extensionName);
        EXPECT_STREQ(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME, ext[21].extensionName);
        EXPECT_STREQ(VK_KHR_MAINTENANCE_3_EXTENSION_NAME, ext[22].extensionName);

        inst_builder.reset();
    }

    // Vulkan 1.3
    {
        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_api_1_3.json";
        settings.emulate_portability = false;
        settings.profile_name = "VP_LUNARG_test_api_1_3";
        settings.simulate_capabilities = SIMULATE_API_VERSION_BIT | SIMULATE_EXTENSIONS_BIT;
        settings.debug_reports = DEBUG_REPORT_MAX_ENUM;

        err = inst_builder.init(&settings);
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

        EXPECT_EQ(0, extCount);

        std::vector<VkExtensionProperties> ext(extCount);
        result = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extCount, ext.data());
        ASSERT_EQ(err, VK_SUCCESS);

        EXPECT_STREQ(VK_KHR_MULTIVIEW_EXTENSION_NAME, ext[0].extensionName);
        EXPECT_STREQ(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, ext[1].extensionName);
        EXPECT_STREQ(VK_KHR_DEVICE_GROUP_EXTENSION_NAME, ext[2].extensionName);
        EXPECT_STREQ(VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME, ext[3].extensionName);
        EXPECT_STREQ(VK_KHR_MAINTENANCE_1_EXTENSION_NAME, ext[4].extensionName);
        EXPECT_STREQ(VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME, ext[5].extensionName);
        EXPECT_STREQ(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME, ext[6].extensionName);
        EXPECT_STREQ(VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME, ext[7].extensionName);
        EXPECT_STREQ(VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME, ext[8].extensionName);
        EXPECT_STREQ(VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME, ext[9].extensionName);
        EXPECT_STREQ(VK_KHR_16BIT_STORAGE_EXTENSION_NAME, ext[10].extensionName);
        EXPECT_STREQ(VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE_EXTENSION_NAME, ext[11].extensionName);
        EXPECT_STREQ(VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME, ext[12].extensionName);
        EXPECT_STREQ(VK_KHR_EXTERNAL_FENCE_EXTENSION_NAME, ext[13].extensionName);
        EXPECT_STREQ(VK_KHR_MAINTENANCE_2_EXTENSION_NAME, ext[14].extensionName);
        EXPECT_STREQ(VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME, ext[15].extensionName);
        EXPECT_STREQ(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME, ext[16].extensionName);
        EXPECT_STREQ(VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME, ext[17].extensionName);
        EXPECT_STREQ(VK_KHR_RELAXED_BLOCK_LAYOUT_EXTENSION_NAME, ext[18].extensionName);
        EXPECT_STREQ(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME, ext[19].extensionName);
        EXPECT_STREQ(VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME, ext[20].extensionName);
        EXPECT_STREQ(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME, ext[21].extensionName);
        EXPECT_STREQ(VK_KHR_MAINTENANCE_3_EXTENSION_NAME, ext[22].extensionName);

        inst_builder.reset();
    }*/
}

TEST_F(TestsMechanism, selecting_profile) {
    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    { /*
        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_instance_extensions.json";
        settings.emulate_portability = false;
        settings.profile_name = "VP_LUNARG_test_instance_extensions";
        settings.simulate_capabilities = SIMULATE_EXTENSIONS_BIT;

        err = inst_builder.init(&settings);
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
        result = vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
        ASSERT_EQ(err, VK_SUCCESS);

        std::vector<VkExtensionProperties> ext(extCount);
        result = vkEnumerateInstanceExtensionProperties(nullptr, &extCount, ext.data());
        ASSERT_EQ(err, VK_SUCCESS);

        EXPECT_STREQ("VK_KHR_get_physical_device_properties2", ext[0].extensionName);

        inst_builder.reset();
        */
    }

    // Only override extensions
    {
        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_device_extensions.json";
        settings.emulate_portability = false;
        settings.profile_name = "VP_LUNARG_test_device_extensions";
        settings.simulate_capabilities = SIMULATE_EXTENSIONS_BIT;
        settings.debug_reports = DEBUG_REPORT_MAX_ENUM;

        err = inst_builder.init(&settings);
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
        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_device_extensions.json";
        settings.emulate_portability = false;
        settings.profile_name = "VP_LUNARG_test_device_extensions";
        settings.simulate_capabilities = SIMULATE_MAX_ENUM;
        settings.debug_reports = DEBUG_REPORT_MAX_ENUM;

        err = inst_builder.init(&settings);
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
        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_selecting_profile.json";
        settings.emulate_portability = true;
        settings.profile_name = "VP_LUNARG_test_selecting_profile";
        settings.simulate_capabilities = SIMULATE_EXTENSIONS_BIT | SIMULATE_PROPERTIES_BIT;

        err = inst_builder.init(&settings);
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
        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_selecting_profile.json";
        settings.emulate_portability = true;
        settings.profile_name = "VP_LUNARG_test_selecting_profile_subset";
        settings.simulate_capabilities = SIMULATE_EXTENSIONS_BIT | SIMULATE_PROPERTIES_BIT;

        err = inst_builder.init(&settings);
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

TEST_F(TestsMechanism, reading_flags) {
#if defined(VK_NV_fragment_shading_rate_enums) && defined(VK_KHR_fragment_shading_rate) && defined(VK_KHR_shader_float_controls)
    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    VkProfileLayerSettingsEXT settings;
    settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_api_alternate.json";
    settings.emulate_portability = true;
    settings.profile_name = "VP_LUNARG_test_api";
    settings.simulate_capabilities = SIMULATE_MAX_ENUM;

    err = inst_builder.init(&settings);
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
    EXPECT_EQ(float_control_properties_profile.denormBehaviorIndependence, float_control_properties_native.denormBehaviorIndependence);
    EXPECT_EQ(float_control_properties_profile.roundingModeIndependence, float_control_properties_native.roundingModeIndependence);

    // These values are overridden
    EXPECT_EQ(fragment_shading_rate_properties_profile.maxFragmentShadingRateRasterizationSamples, VK_SAMPLE_COUNT_2_BIT);
    EXPECT_EQ(fragment_shading_rate_enums_properties_profile.maxFragmentShadingRateInvocationCount, VK_SAMPLE_COUNT_2_BIT);
#endif
}

TEST_F(TestsMechanism, reading_duplicated_members) {
    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    VkProfileLayerSettingsEXT settings;
    settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_duplicated.json";
    settings.emulate_portability = true;
    settings.profile_name = "VP_LUNARG_test_duplicated";
    settings.simulate_capabilities = SIMULATE_MAX_ENUM;
    settings.debug_fail_on_error = true;

    err = inst_builder.init(&settings);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice physical_device;
    err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &physical_device);
    ASSERT_EQ(err, VK_ERROR_INITIALIZATION_FAILED);
}


TEST_F(TestsMechanism, TestParsingAllFormatProperties) {
    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    VkProfileLayerSettingsEXT settings;
    settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_formats.json";
    settings.emulate_portability = true;
    settings.profile_name = "VP_LUNARG_test_formats";
    settings.simulate_capabilities = SIMULATE_MAX_ENUM;

    err = inst_builder.init(&settings);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice gpu;
    err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);
    if (err != VK_SUCCESS) {
        printf("Profile not supported on device, skipping test.\n");
        return;
    }

    {
        VkFormat format = VK_FORMAT_R4G4_UNORM_PACK8;
        VkFormatProperties format_properties;
        vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

        VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
        VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
        VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;

        EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
        EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
        EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
    }

    {
        VkFormat format = VK_FORMAT_R4G4B4A4_UNORM_PACK16;
        VkFormatProperties2 format_properties2 = {};
        format_properties2.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
        vkGetPhysicalDeviceFormatProperties2(gpu, format, &format_properties2);

        const VkFormatProperties& format_properties = format_properties2.formatProperties;

        VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
        VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
        VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

        EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
        EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
        EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
    }

#ifdef VK_KHR_format_feature_flags2
    {
        VkFormat format = VK_FORMAT_B4G4R4A4_UNORM_PACK16;
        VkFormatProperties2 format_properties2 = {};
        format_properties2.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
        VkFormatProperties3 format_properties3 = {};
        format_properties3.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3;
        format_properties2.pNext = &format_properties3;
        vkGetPhysicalDeviceFormatProperties2(gpu, format, &format_properties2);

        VkFormatFeatureFlags2KHR linear_tiling_features = VK_FORMAT_FEATURE_2_STORAGE_TEXEL_BUFFER_ATOMIC_BIT_KHR;
        VkFormatFeatureFlags2KHR optimal_tiling_features = VK_FORMAT_FEATURE_2_STORAGE_IMAGE_BIT_KHR;
        VkFormatFeatureFlags2KHR buffer_features = VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BLEND_BIT_KHR;

        EXPECT_EQ(format_properties3.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
        EXPECT_EQ(format_properties3.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
        EXPECT_EQ(format_properties3.bufferFeatures & buffer_features, buffer_features);
    }
#endif
}
