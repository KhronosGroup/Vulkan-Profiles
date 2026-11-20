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

#include <algorithm>
#include <cstdarg>
#include <cstring>
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

TEST_F(TestsMechanism, selecting_profile_dirs_missing_dir) {
    TEST_DESCRIPTION("Test selecting profile from a profiles dir listed after a directory that doesn't exist");

    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    {
        // The missing directory is reported and ignored, the profile is loaded from the next directory
        const char* profile_dirs_data = JSON_TEST_FILES_PATH "missing_profiles_dir," JSON_TEST_FILES_PATH;
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

        // Extensions of both the requested profile and its required profile are exposed
        uint32_t count = 0;
        vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, nullptr);
        std::vector<VkExtensionProperties> extensions(count);
        vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, extensions.data());

        const auto has_extension = [&extensions](const char* name) {
            return std::any_of(extensions.begin(), extensions.end(),
                               [name](const VkExtensionProperties& ext) { return std::strcmp(ext.extensionName, name) == 0; });
        };

        EXPECT_TRUE(has_extension(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME));  // VP_LUNARG_test_required_profiles2
        EXPECT_TRUE(has_extension(VK_KHR_SWAPCHAIN_EXTENSION_NAME));        // VP_LUNARG_test_required_profiles

        inst_builder.reset();
    }
}

TEST_F(TestsMechanism, selecting_profile_with_required_profiles_extensions) {
    TEST_DESCRIPTION("Test that the extensions of a required profile are kept while loading the requested profile");

    const char* profile_dirs_data = JSON_TEST_FILES_PATH;
    const char* profile_name_data = "VP_LUNARG_test_required_profiles_extensions";
    const std::vector<const char*> simulate_capabilities = {"SIMULATE_MAX_ENUM"};
    const char* debug_reports_data = "DEBUG_REPORT_MAX_ENUM";
    const VkBool32 debug_fail_on_error = VK_TRUE;

    const std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileDirs, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_dirs_data},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
        {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT,
         static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]},
        {kLayerName, kLayerSettingsDebugReports, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &debug_reports_data},
        {kLayerName, kLayerSettingsDebugFailOnError, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &debug_fail_on_error}};

    profiles_test::VulkanInstanceBuilder inst_builder;
    VkResult err = inst_builder.init(settings);
    ASSERT_EQ(err, VK_SUCCESS);

    {
        // The layer loads the profile on every physical device, and debug_fail_on_error makes vkEnumeratePhysicalDevices fail
        // when any of them doesn't support the profile extensions, such as the Dozen devices on Windows ARM.
        const VkInstance instance_native = inst_builder.getInstance(profiles_test::MODE_NATIVE);

        uint32_t gpu_count = 0;
        err = vkEnumeratePhysicalDevices(instance_native, &gpu_count, nullptr);
        ASSERT_EQ(err, VK_SUCCESS);
        std::vector<VkPhysicalDevice> gpus_native(gpu_count);
        err = vkEnumeratePhysicalDevices(instance_native, &gpu_count, gpus_native.data());
        ASSERT_EQ(err, VK_SUCCESS);

        for (const VkPhysicalDevice gpu_native : gpus_native) {
            uint32_t count = 0;
            vkEnumerateDeviceExtensionProperties(gpu_native, nullptr, &count, nullptr);
            std::vector<VkExtensionProperties> device_extensions(count);
            vkEnumerateDeviceExtensionProperties(gpu_native, nullptr, &count, device_extensions.data());

            const bool supported =
                std::any_of(device_extensions.begin(), device_extensions.end(), [](const VkExtensionProperties& ext) {
                    return std::strcmp(ext.extensionName, VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME) == 0;
                });
            if (!supported) {
                VkPhysicalDeviceProperties properties{};
                vkGetPhysicalDeviceProperties(gpu_native, &properties);
                inst_builder.reset();
                GTEST_SKIP() << VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME << " not supported by " << properties.deviceName;
            }
        }
    }

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevicePushDescriptorPropertiesKHR push_descriptor_properties{};
    push_descriptor_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props2{};
    gpu_props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props2.pNext = &push_descriptor_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props2);

    EXPECT_EQ(31u, push_descriptor_properties.maxPushDescriptors);

    inst_builder.reset();
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

static VkResult CreateTestDevice(VkPhysicalDevice gpu, const VkPhysicalDeviceFeatures* enabled_features, const void* next,
                                 VkDevice* device) {
    const float queue_priority = 1.0f;

    VkDeviceQueueCreateInfo queue_create_info{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    queue_create_info.queueFamilyIndex = 0;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;

    VkDeviceCreateInfo device_create_info{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    device_create_info.pNext = next;
    device_create_info.queueCreateInfoCount = 1;
    device_create_info.pQueueCreateInfos = &queue_create_info;
    device_create_info.pEnabledFeatures = enabled_features;

    return vkCreateDevice(gpu, &device_create_info, nullptr, device);
}

TEST_F(TestsMechanism, create_device_unsupported_feature) {
    TEST_DESCRIPTION("Test that vkCreateDevice returns VK_ERROR_FEATURE_NOT_PRESENT for a feature the simulated device doesn't support");

    profiles_test::VulkanInstanceBuilder inst_builder;

    // The profile has no features, so all features are reported unsupported with DEFAULT_FEATURE_VALUES_FALSE
    const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_api_1_0.json";
    const char* profile_name_data = "VP_LUNARG_test_api_1_0";
    const char* default_feature_values_data = "DEFAULT_FEATURE_VALUES_FALSE";

    const std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
        {kLayerName, kLayerSettingsDefaultFeatureValues, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &default_feature_values_data}};

    ASSERT_EQ(inst_builder.init(settings), VK_SUCCESS);

    VkPhysicalDevice gpu_native = VK_NULL_HANDLE;
    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    if (inst_builder.getPhysicalDevice(profiles_test::MODE_NATIVE, &gpu_native) != VK_SUCCESS ||
        inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu) != VK_SUCCESS) {
        printf("Profile not supported on device, skipping test.\n");
        inst_builder.reset();
        return;
    }

    // The device supports the feature, only the simulated device doesn't
    VkPhysicalDeviceFeatures native_features{};
    vkGetPhysicalDeviceFeatures(gpu_native, &native_features);
    if (!native_features.independentBlend) {
        printf("independentBlend not supported on device, skipping test.\n");
        inst_builder.reset();
        return;
    }

    VkPhysicalDeviceFeatures reported_features{};
    vkGetPhysicalDeviceFeatures(gpu, &reported_features);
    EXPECT_EQ(reported_features.independentBlend, VK_FALSE);

    // Requested with VkDeviceCreateInfo::pEnabledFeatures
    VkPhysicalDeviceFeatures enabled_features{};
    enabled_features.independentBlend = VK_TRUE;
    VkDevice device = VK_NULL_HANDLE;
    EXPECT_EQ(CreateTestDevice(gpu, &enabled_features, nullptr, &device), VK_ERROR_FEATURE_NOT_PRESENT);

    // Requested with VkPhysicalDeviceFeatures2
    VkPhysicalDeviceFeatures2 enabled_features2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
    enabled_features2.features.independentBlend = VK_TRUE;
    EXPECT_EQ(CreateTestDevice(gpu, nullptr, &enabled_features2, &device), VK_ERROR_FEATURE_NOT_PRESENT);

    // No requested feature
    ASSERT_EQ(CreateTestDevice(gpu, nullptr, nullptr, &device), VK_SUCCESS);
    vkDestroyDevice(device, nullptr);

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

TEST_F(TestsMechanism, create_device_supported_feature) {
    TEST_DESCRIPTION("Test that vkCreateDevice succeeds for a feature the simulated device supports");

    profiles_test::VulkanInstanceBuilder inst_builder;

    // The profile has no features, so the device features are reported with DEFAULT_FEATURE_VALUES_DEVICE
    const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_api_1_0.json";
    const char* profile_name_data = "VP_LUNARG_test_api_1_0";
    const char* default_feature_values_data = "DEFAULT_FEATURE_VALUES_DEVICE";

    const std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
        {kLayerName, kLayerSettingsDefaultFeatureValues, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &default_feature_values_data}};

    ASSERT_EQ(inst_builder.init(settings), VK_SUCCESS);

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    if (inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu) != VK_SUCCESS) {
        printf("Profile not supported on device, skipping test.\n");
        inst_builder.reset();
        return;
    }

    VkPhysicalDeviceFeatures reported_features{};
    vkGetPhysicalDeviceFeatures(gpu, &reported_features);
    if (!reported_features.independentBlend) {
        printf("independentBlend not supported on device, skipping test.\n");
        inst_builder.reset();
        return;
    }

    VkPhysicalDeviceFeatures enabled_features{};
    enabled_features.independentBlend = VK_TRUE;
    VkDevice device = VK_NULL_HANDLE;
    ASSERT_EQ(CreateTestDevice(gpu, &enabled_features, nullptr, &device), VK_SUCCESS);
    vkDestroyDevice(device, nullptr);

    inst_builder.reset();
}

static DebugUtilsMessages LoadProfileWithMessenger(const char* profile_file_data, const char* profile_name_data) {
    profiles_test::VulkanInstanceBuilder inst_builder;
    inst_builder.addExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data}};

    DebugUtilsMessages messages;
    EXPECT_EQ(inst_builder.init(settings), VK_SUCCESS);

    VkInstance instance = inst_builder.getInstance(profiles_test::MODE_PROFILE);
    PFN_vkCreateDebugUtilsMessengerEXT create_messenger =
        reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    PFN_vkDestroyDebugUtilsMessengerEXT destroy_messenger =
        reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    EXPECT_NE(create_messenger, nullptr);
    EXPECT_NE(destroy_messenger, nullptr);
    if (create_messenger == nullptr || destroy_messenger == nullptr) {
        inst_builder.reset();
        return messages;
    }

    const VkDebugUtilsMessengerCreateInfoEXT messenger_create_info = GetDebugUtilsMessengerCreateInfo(&messages);
    VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
    EXPECT_EQ(create_messenger(instance, &messenger_create_info, nullptr, &messenger), VK_SUCCESS);

    // The profile is loaded for the physical devices
    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);

    destroy_messenger(instance, messenger, nullptr);
    inst_builder.reset();
    return messages;
}

TEST_F(TestsMechanism, profile_newer_vulkan_header) {
    TEST_DESCRIPTION("Test that a profile written against a newer Vulkan Header than the layer is reported");

    const DebugUtilsMessages newer_messages =
        LoadProfileWithMessenger(JSON_TEST_FILES_PATH "VP_LUNARG_test_newer_header.json", "VP_LUNARG_test_newer_header");
    EXPECT_TRUE(newer_messages.Contains(VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
                                        "the profile is written against Vulkan Header 99999"));

    // The schema patch version of 'profiles-0.8.0-204' is not mistaken for the Vulkan Header version
    const DebugUtilsMessages older_messages =
        LoadProfileWithMessenger(JSON_TEST_FILES_PATH "VP_LUNARG_test_api_1_0.json", "VP_LUNARG_test_api_1_0");
    EXPECT_FALSE(older_messages.messages.empty());
    EXPECT_FALSE(older_messages.Contains(VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT, "the profile is written against Vulkan Header"));
}

TEST_F(TestsMechanism, capabilities_depends) {
    TEST_DESCRIPTION("Test that the capability blocks are only loaded when the simulated device satisfies their 'depends' condition");

    profiles_test::VulkanInstanceBuilder inst_builder;

    const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_depends.json";
    const char* profile_name_data = "VP_LUNARG_test_depends";
    const std::vector<const char*> simulate_capabilities = {"SIMULATE_API_VERSION_BIT", "SIMULATE_FEATURES_BIT", "SIMULATE_EXTENSIONS_BIT"};
    const char* default_feature_values_data = "DEFAULT_FEATURE_VALUES_FALSE";

    const std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
        {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]},
        {kLayerName, kLayerSettingsDefaultFeatureValues, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &default_feature_values_data}};

    ASSERT_EQ(inst_builder.init(settings), VK_SUCCESS);

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    if (inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu) != VK_SUCCESS) {
        printf("Profile not supported on device, skipping test.\n");
        inst_builder.reset();
        return;
    }

    VkPhysicalDeviceFeatures features{};
    vkGetPhysicalDeviceFeatures(gpu, &features);

    // Unconditional capability block
    EXPECT_EQ(features.depthClamp, VK_TRUE);
    // 'depends' satisfied by the simulated depthClamp, loaded after the unconditional capability blocks
    EXPECT_EQ(features.sampleRateShading, VK_TRUE);
    // 'depends' not satisfied
    EXPECT_EQ(features.geometryShader, VK_FALSE);
    // 'depends' satisfied by the simulated Vulkan 1.1 version and the simulated depthClamp
    EXPECT_EQ(features.dualSrcBlend, VK_TRUE);
    // 'depends' not satisfied by the simulated extensions
    EXPECT_EQ(features.logicOp, VK_FALSE);

    inst_builder.reset();
}

static void InitDependsAliasesProfile(profiles_test::VulkanInstanceBuilder &inst_builder, const char *profile_name_data) {
    const char *profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_depends_aliases.json";
    const std::vector<const char *> simulate_capabilities = {"SIMULATE_API_VERSION_BIT", "SIMULATE_FEATURES_BIT"};
    const char *default_feature_values_data = "DEFAULT_FEATURE_VALUES_FALSE";

    const std::vector<VkLayerSettingEXT> settings = {
        {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
        {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
        {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]},
        {kLayerName, kLayerSettingsDefaultFeatureValues, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &default_feature_values_data}};

    ASSERT_EQ(inst_builder.init(VK_API_VERSION_1_2, settings), VK_SUCCESS);
}

TEST_F(TestsMechanism, capabilities_depends_vulkan_11_features) {
    TEST_DESCRIPTION("Test that the 'depends' conditions naming the structures promoted to Vulkan 1.1 see the VkPhysicalDeviceVulkan11Features values of the profile");

    profiles_test::VulkanInstanceBuilder inst_builder;
    InitDependsAliasesProfile(inst_builder, "VP_LUNARG_test_depends_vulkan_11");

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    if (inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu) != VK_SUCCESS) {
        printf("Profile not supported on device, skipping test.\n");
        inst_builder.reset();
        return;
    }

    VkPhysicalDeviceProtectedMemoryFeatures protected_memory_features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES};
    VkPhysicalDeviceFeatures2 features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &protected_memory_features};
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    // VkPhysicalDeviceVulkan11Features::protectedMemory is transferred to the core VkPhysicalDeviceProtectedMemoryFeatures
    EXPECT_EQ(protected_memory_features.protectedMemory, VK_TRUE);
    // 'depends' naming VkPhysicalDeviceProtectedMemoryFeatures, set by the profile in VkPhysicalDeviceVulkan11Features
    EXPECT_EQ(features.features.sampleRateShading, VK_TRUE);
    // 'depends' naming VkPhysicalDeviceMultiviewFeatures, set by the profile in VkPhysicalDeviceVulkan11Features
    EXPECT_EQ(features.features.dualSrcBlend, VK_TRUE);
    // 'depends' naming VkPhysicalDevice16BitStorageFeatures, not set by the profile in VkPhysicalDeviceVulkan11Features
    EXPECT_EQ(features.features.logicOp, VK_FALSE);

    inst_builder.reset();
}

TEST_F(TestsMechanism, capabilities_depends_promoted_features) {
    TEST_DESCRIPTION("Test that the 'depends' conditions naming VkPhysicalDeviceVulkan11Features see the values of the promoted structures of the profile");

    profiles_test::VulkanInstanceBuilder inst_builder;
    InitDependsAliasesProfile(inst_builder, "VP_LUNARG_test_depends_promoted");

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    if (inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu) != VK_SUCCESS) {
        printf("Profile not supported on device, skipping test.\n");
        inst_builder.reset();
        return;
    }

    VkPhysicalDeviceVulkan11Features vulkan_11_features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES};
    VkPhysicalDeviceFeatures2 features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &vulkan_11_features};
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    // The core VkPhysicalDeviceProtectedMemoryFeatures::protectedMemory is transferred to VkPhysicalDeviceVulkan11Features
    EXPECT_EQ(vulkan_11_features.protectedMemory, VK_TRUE);
    EXPECT_EQ(vulkan_11_features.multiview, VK_TRUE);
    // 'depends' naming VkPhysicalDeviceVulkan11Features, set by the profile in the promoted structures
    EXPECT_EQ(features.features.sampleRateShading, VK_TRUE);

    inst_builder.reset();
}
