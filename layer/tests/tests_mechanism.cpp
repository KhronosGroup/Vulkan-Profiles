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
#include <fstream>

class TestsMechanism : public VkTestFramework {
   public:
    TestsMechanism(){}
    ~TestsMechanism(){}

    static void SetUpTestSuite(){}
    static void TearDownTestSuite(){}
};

// Collects the profiles layer messages reported to a VK_EXT_debug_utils messenger
struct DebugUtilsMessages {
    std::vector<std::pair<VkDebugUtilsMessageSeverityFlagBitsEXT, std::string>> messages;

    bool Contains(VkDebugUtilsMessageSeverityFlagBitsEXT severity, const char* text) const {
        for (const auto& message : messages) {
            if (message.first == severity && message.second.find(text) != std::string::npos) {
                return true;
            }
        }
        return false;
    }
};

static VKAPI_ATTR VkBool32 VKAPI_CALL CollectDebugUtilsMessage(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                               VkDebugUtilsMessageTypeFlagsEXT type,
                                                               const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                               void* user_data) {
    (void)type;
    // The loader also reports its own messages to the messengers
    if (callback_data->pMessageIdName != nullptr && strcmp(callback_data->pMessageIdName, kLayerName) == 0) {
        static_cast<DebugUtilsMessages*>(user_data)->messages.push_back({severity, callback_data->pMessage});
    }
    return VK_FALSE;
}

static VkDebugUtilsMessengerCreateInfoEXT GetDebugUtilsMessengerCreateInfo(DebugUtilsMessages* messages) {
    VkDebugUtilsMessengerCreateInfoEXT create_info{VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
    create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = CollectDebugUtilsMessage;
    create_info.pUserData = messages;
    return create_info;
}

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
        ASSERT_EQ(result, VK_SUCCESS);

        EXPECT_TRUE(extCount >= 1);

        std::vector<VkExtensionProperties> ext(extCount);
        result = vkEnumerateInstanceExtensionProperties("VK_LAYER_KHRONOS_profiles", &extCount, ext.data());
        ASSERT_EQ(result, VK_SUCCESS);

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
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_EXTENSIONS_BIT"};
        const char* debug_reports_data = "DEBUG_REPORT_MAX_ENUM";

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
            {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
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

        EXPECT_STREQ("VK_KHR_maintenance3", ext[0].extensionName);

        inst_builder.reset();
    }

    // Override everything but the layer only has extensions
    {
        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_device_extensions.json";
        const char* profile_name_data = "VP_LUNARG_test_device_extensions";
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_MAX_ENUM"};
        const char* debug_reports_data = "DEBUG_REPORT_MAX_ENUM";

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
            {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
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

        EXPECT_STREQ("VK_KHR_maintenance3", ext[0].extensionName);

        inst_builder.reset();
    }

    {
        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_selecting_profile.json";
        const char* profile_name_data = "VP_LUNARG_test_selecting_profile";
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_PROPERTIES_BIT", "SIMULATE_EXTENSIONS_BIT"};

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
            {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
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
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_PROPERTIES_BIT", "SIMULATE_EXTENSIONS_BIT"};

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
            {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
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

TEST_F(TestsMechanism, selecting_profile_file_not_found) {
    TEST_DESCRIPTION("Test selecting profile from a profiles file which is not found");

    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    // Only override extensions
    {
        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_not_found.json";
        const char* profile_name_data = "VP_LUNARG_test_not_found";
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_EXTENSIONS_BIT"};
        const char* debug_reports_data = "DEBUG_REPORT_MAX_ENUM";

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
            {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
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
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_MAX_ENUM"};
        const char* debug_reports_data = "DEBUG_REPORT_MAX_ENUM";

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileDirs, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_dirs_data},
            {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
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

        EXPECT_STREQ("VK_KHR_maintenance3", ext[0].extensionName);

        inst_builder.reset();
    }
}

TEST_F(TestsMechanism, selecting_profile_dirs_not_found) {
    TEST_DESCRIPTION("Test selecting profile from a profiles dir using a profile file path");

    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    {
        const char* profile_dirs_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_device_extensions.json"; // Not a dir!
        const char* profile_name_data = "VP_LUNARG_test_device_extensions";
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_MAX_ENUM"};
        const char* debug_reports_data = "DEBUG_REPORT_MAX_ENUM";

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileDirs, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_dirs_data},
            {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
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

        uint32_t extCount = 0;
        result = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extCount, nullptr);
        ASSERT_EQ(result, VK_SUCCESS);

        EXPECT_EQ(1, extCount);

        std::vector<VkExtensionProperties> ext(extCount);
        result = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extCount, ext.data());
        ASSERT_EQ(result, VK_SUCCESS);

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
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_MAX_ENUM"};
        const char* debug_reports_data = "DEBUG_REPORT_MAX_ENUM";

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileDirs, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_dirs_data},
            {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
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
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_API_VERSION_BIT", "SIMULATE_FEATURES_BIT",
                                                                "SIMULATE_PROPERTIES_BIT", "SIMULATE_EXTENSIONS_BIT"};
        const char* debug_reports_data = "DEBUG_REPORT_MAX_ENUM";

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
            {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
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
        const std::vector<const char*> simulate_capabilities = {"SIMULATE_API_VERSION_BIT", "SIMULATE_EXTENSIONS_BIT"};
        const char* default_feature_values_data = "DEFAULT_FEATURE_VALUES_FALSE";
        const char* debug_reports_data = "DEBUG_REPORT_MAX_ENUM";

        const std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
            {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
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
    const std::vector<const char*> simulate_capabilities = {"SIMULATE_MAX_ENUM"};
    VkBool32 debug_fail_on_error = VK_TRUE;

    std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
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
    const std::vector<const char*> simulate_capabilities = {"SIMULATE_MAX_ENUM"};
    VkBool32 debug_fail_on_error = VK_TRUE;

    std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
        {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]},
        {kLayerName, kLayerSettingsDebugFailOnError, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &debug_fail_on_error}};

    profiles_test::VulkanInstanceBuilder inst_builder;
    VkResult err = inst_builder.init(settings);
    EXPECT_EQ(err, VK_SUCCESS);
}

TEST_F(TestsMechanism, debug_utils_layer_extension) {
    TEST_DESCRIPTION("Test that the layer provides VK_EXT_debug_utils");

    profiles_test::VulkanInstanceBuilder inst_builder;
    ASSERT_EQ(inst_builder.init(), VK_SUCCESS);

    uint32_t count = 0;
    ASSERT_EQ(vkEnumerateInstanceExtensionProperties(kLayerName, &count, nullptr), VK_SUCCESS);
    std::vector<VkExtensionProperties> properties(count);
    ASSERT_EQ(vkEnumerateInstanceExtensionProperties(kLayerName, &count, properties.data()), VK_SUCCESS);

    bool found = false;
    for (const VkExtensionProperties& property : properties) {
        if (strcmp(property.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0) {
            found = true;
            EXPECT_EQ(property.specVersion, static_cast<uint32_t>(VK_EXT_DEBUG_UTILS_SPEC_VERSION));
        }
    }
    EXPECT_TRUE(found);

    inst_builder.reset();
}

TEST_F(TestsMechanism, debug_utils_instance_messenger) {
    TEST_DESCRIPTION("Test that the messenger of VkInstanceCreateInfo::pNext reports the messages of vkCreateInstance and vkDestroyInstance only");

    DebugUtilsMessages messages;
    const VkDebugUtilsMessengerCreateInfoEXT messenger_create_info = GetDebugUtilsMessengerCreateInfo(&messages);

    profiles_test::VulkanInstanceBuilder inst_builder;
    inst_builder.addExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    inst_builder.setNext(&messenger_create_info);

    // The missing profile file makes the layer report an error during vkCreateInstance
    const char* profile_file_data = JSON_TEST_FILES_PATH "missing_profile_file.json";
    const char* profile_dirs_data = JSON_TEST_FILES_PATH;
    const char* profile_name_data = "VP_LUNARG_test_device_extensions";

    std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
        {kLayerName, kLayerSettingsProfileDirs, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_dirs_data},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data}};

    ASSERT_EQ(inst_builder.init(settings), VK_SUCCESS);

    EXPECT_TRUE(messages.Contains(VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, "Fail to open file"));
    EXPECT_TRUE(messages.Contains(VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT, "CreateInstance"));
    EXPECT_FALSE(messages.Contains(VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT, "DestroyInstance"));

    // The messenger is not active after vkCreateInstance
    const std::size_t create_instance_count = messages.messages.size();
    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);
    EXPECT_EQ(messages.messages.size(), create_instance_count);

    // The messenger is active again during vkDestroyInstance
    inst_builder.reset();
    EXPECT_TRUE(messages.Contains(VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT, "DestroyInstance"));
}

TEST_F(TestsMechanism, debug_utils_messenger) {
    TEST_DESCRIPTION("Test that a messenger created with vkCreateDebugUtilsMessengerEXT reports the layer messages until it is destroyed");

    profiles_test::VulkanInstanceBuilder inst_builder;
    inst_builder.addExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    const char* profile_dirs_data = JSON_TEST_FILES_PATH;
    const char* profile_name_data = "VP_LUNARG_test_device_extensions";
    const std::vector<const char*> simulate_capabilities = {"SIMULATE_MAX_ENUM"};

    std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileDirs, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_dirs_data},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
        {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT,
         static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]}};

    ASSERT_EQ(inst_builder.init(settings), VK_SUCCESS);

    VkInstance instance = inst_builder.getInstance(profiles_test::MODE_PROFILE);
    PFN_vkCreateDebugUtilsMessengerEXT create_messenger =
        reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    PFN_vkDestroyDebugUtilsMessengerEXT destroy_messenger =
        reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    ASSERT_NE(create_messenger, nullptr);
    ASSERT_NE(destroy_messenger, nullptr);

    DebugUtilsMessages messages;
    const VkDebugUtilsMessengerCreateInfoEXT messenger_create_info = GetDebugUtilsMessengerCreateInfo(&messages);
    VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
    ASSERT_EQ(create_messenger(instance, &messenger_create_info, nullptr, &messenger), VK_SUCCESS);

    // Loading the profile for the physical devices is reported
    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);
    EXPECT_FALSE(messages.messages.empty());

    // A destroyed messenger doesn't report the messages of vkDestroyInstance
    destroy_messenger(instance, messenger, nullptr);
    const std::size_t destroy_messenger_count = messages.messages.size();
    inst_builder.reset();
    EXPECT_EQ(messages.messages.size(), destroy_messenger_count);
    EXPECT_FALSE(messages.Contains(VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT, "DestroyInstance"));
}

TEST_F(TestsMechanism, debug_utils_explicit_debug_actions) {
    TEST_DESCRIPTION("Test that explicitly set debug actions are still used when there is a debug utils messenger");

    DebugUtilsMessages messages;
    const VkDebugUtilsMessengerCreateInfoEXT messenger_create_info = GetDebugUtilsMessengerCreateInfo(&messages);

    profiles_test::VulkanInstanceBuilder inst_builder;
    inst_builder.addExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    inst_builder.setNext(&messenger_create_info);

    // The missing profile file makes the layer report an error during vkCreateInstance
    const char* profile_file_data = JSON_TEST_FILES_PATH "missing_profile_file.json";
    const char* profile_dirs_data = JSON_TEST_FILES_PATH;
    const char* profile_name_data = "VP_LUNARG_test_device_extensions";
    const char* debug_actions_data = "DEBUG_ACTION_FILE_BIT";
#if defined(__ANDROID__)
    // The working directory of the test application is not writable on Android, unlike the directory of the test files
    const char* debug_filename_data = JSON_TEST_FILES_PATH "profiles_layer_debug_utils_test_log.txt";
#else
    const char* debug_filename_data = "profiles_layer_debug_utils_test_log.txt";
#endif
    const VkBool32 debug_file_clear_data = VK_TRUE;

    std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
        {kLayerName, kLayerSettingsProfileDirs, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_dirs_data},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
        {kLayerName, kLayerSettingsDebugActions, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &debug_actions_data},
        {kLayerName, kLayerSettingsDebugFilename, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &debug_filename_data},
        {kLayerName, kLayerSettingsDebugFileClear, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &debug_file_clear_data}};

    ASSERT_EQ(inst_builder.init(settings), VK_SUCCESS);

    // Destroying the instance closes the log file
    inst_builder.reset();

    EXPECT_TRUE(messages.Contains(VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, "Fail to open file"));

    std::ifstream log_file(debug_filename_data);
    ASSERT_TRUE(log_file.is_open());
    const std::string log((std::istreambuf_iterator<char>(log_file)), std::istreambuf_iterator<char>());
    EXPECT_NE(log.find("Fail to open file"), std::string::npos);
}
