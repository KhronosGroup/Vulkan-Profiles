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

class TestsMechanism : public VkTestFramework {
   public:
    TestsMechanism(){};
    ~TestsMechanism(){};

    static void SetUpTestSuite() {}
    static void TearDownTestSuite(){};
};

TEST_F(TestsMechanism, default_feature_values) {
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
        settings.default_feature_values = DEFAULT_FEATURE_VALUES_FALSE;

        err = inst_builder.init(&settings);
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

        VkPhysicalDevice16BitStorageFeaturesKHR gpu_features_16bits_storage{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES_KHR};
        VkPhysicalDeviceFeatures2 gpu_features2{};
        gpu_features2.pNext = &gpu_features_16bits_storage;
        vkGetPhysicalDeviceFeatures2(gpu, &gpu_features2);

        EXPECT_EQ(gpu_features2.features.independentBlend, VK_FALSE);
        EXPECT_EQ(gpu_features_16bits_storage.storageBuffer16BitAccess, VK_FALSE);

        inst_builder.reset();
    }
}

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

        EXPECT_EQ(47, extCount);

        std::vector<VkExtensionProperties> ext(extCount);
        result = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extCount, ext.data());
        ASSERT_EQ(err, VK_SUCCESS);
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

        EXPECT_EQ(70, extCount);

        std::vector<VkExtensionProperties> ext(extCount);
        result = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extCount, ext.data());
        ASSERT_EQ(err, VK_SUCCESS);
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
}

TEST_F(TestsMechanism, selecting_profile) {
    TEST_DESCRIPTION("Test selecting profile from a profiles file with multiple profiles");

    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    {
        /*
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
    TEST_DESCRIPTION("Test profile specifying flags bits");

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
    EXPECT_EQ(float_control_properties_profile.denormBehaviorIndependence,
              float_control_properties_native.denormBehaviorIndependence);
    EXPECT_EQ(float_control_properties_profile.roundingModeIndependence, float_control_properties_native.roundingModeIndependence);

    // These values are overridden
    EXPECT_EQ(fragment_shading_rate_properties_profile.maxFragmentShadingRateRasterizationSamples, VK_SAMPLE_COUNT_2_BIT);
    EXPECT_EQ(fragment_shading_rate_enums_properties_profile.maxFragmentShadingRateInvocationCount, VK_SAMPLE_COUNT_2_BIT);
#endif
}

TEST_F(TestsMechanism, reading_duplicated_members) {
    TEST_DESCRIPTION("Test profile using duplicated members");

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

TEST_F(TestsMechanism, capabilities_or) {
    TEST_DESCRIPTION("Test profile using capabilities or");

    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    VkProfileLayerSettingsEXT settings;
    settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_capabilities.json";
    settings.emulate_portability = true;
    settings.profile_name = "VP_LUNARG_test_capabilities_or";
    settings.simulate_capabilities = SIMULATE_MAX_ENUM;
    settings.debug_fail_on_error = true;

    err = inst_builder.init(&settings);
    EXPECT_EQ(err, VK_SUCCESS);
}

TEST_F(TestsMechanism, TestParsingAllFormatProperties) {
    TEST_DESCRIPTION("Test all different ways of setting image formats");

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

TEST_F(TestsMechanism, profile_variants_all) {
    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    {
        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_variants.json";
        settings.emulate_portability = false;
        settings.profile_name = "VP_LUNARG_test_variants";
        settings.simulate_capabilities =
            SIMULATE_API_VERSION_BIT | SIMULATE_FEATURES_BIT | SIMULATE_PROPERTIES_BIT | SIMULATE_EXTENSIONS_BIT;
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

TEST_F(TestsMechanism, force_physical_device) {
    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    {
        VkProfileLayerSettingsEXT settings;
        settings.force_device = FORCE_DEVICE_OFF;
        settings.force_device_name = "NVIDIA";
        settings.force_device_uuid = "BC4A01B15641805847A8151A395A80C7";

        err = inst_builder.init(&settings);
        ASSERT_EQ(err, VK_SUCCESS);

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);
        if (err != VK_SUCCESS) {
            printf("Profile not supported on device, skipping test.\n");
            inst_builder.reset();
            return;
        }

        inst_builder.reset();
    }
}
