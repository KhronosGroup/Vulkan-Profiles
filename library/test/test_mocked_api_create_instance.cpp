/*
 * Copyright (c) 2021-2023 Valve Corporation
 * Copyright (c) 2021-2023 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Authors:
 * - Daniel Rakos <daniel.rakos@rastergrid.com>
 */

#include "mock_vulkan_api.hpp"
#include <vulkan/vulkan_android.h>
#include <vulkan/vulkan_profiles.hpp>

TEST(mocked_api_create_instance, vulkan10_no_app_info) {
    MockVulkanAPI mock;

    VpProfileProperties profile{ VP_ANDROID_BASELINE_2021_NAME, VP_ANDROID_BASELINE_2021_SPEC_VERSION };

    std::vector<const char*> dummyLayerNames{ "VK_DUMMY_layer1", "VK_DUMMY_layer2" };

    VkInstanceCreateInfo inCreateInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    inCreateInfo.enabledLayerCount = static_cast<uint32_t>(dummyLayerNames.size());
    inCreateInfo.ppEnabledLayerNames = dummyLayerNames.data();

    std::vector<const char*> outExtensions{ VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME };

    VkApplicationInfo outAppInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
    outAppInfo.apiVersion = VP_ANDROID_BASELINE_2021_MIN_API_VERSION;

    VkInstanceCreateInfo outCreateInfo = inCreateInfo;
    outCreateInfo.pApplicationInfo = &outAppInfo;
    outCreateInfo.enabledExtensionCount = static_cast<uint32_t>(outExtensions.size());
    outCreateInfo.ppEnabledExtensionNames = outExtensions.data();

    mock.SetExpectedInstanceCreateInfo(&outCreateInfo, {});

    VpInstanceCreateInfo createInfo{ &inCreateInfo, &profile, VP_INSTANCE_CREATE_OVERRIDE_EXTENSIONS_BIT };

    VkInstance instance = VK_NULL_HANDLE;
    VkResult result = vpCreateInstance(&createInfo, &mock.vkAllocator, &instance);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_TRUE(instance == mock.vkInstance);
}

TEST(mocked_api_create_instance, vulkan10_with_app_info) {
    MockVulkanAPI mock;

    VpProfileProperties profile{ VP_ANDROID_BASELINE_2021_NAME, VP_ANDROID_BASELINE_2021_SPEC_VERSION };

    std::vector<const char*> dummyLayerNames{ "VK_DUMMY_layer1" };

    VkApplicationInfo inAppInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
    inAppInfo.pApplicationName = "Dummy application name";
    inAppInfo.applicationVersion = VK_MAKE_VERSION(1, 3, 5);
    inAppInfo.pEngineName = "Dummy engine name";
    inAppInfo.engineVersion = VK_MAKE_VERSION(2, 4, 6);
    inAppInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo inCreateInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    inCreateInfo.pApplicationInfo = &inAppInfo;
    inCreateInfo.enabledLayerCount = static_cast<uint32_t>(dummyLayerNames.size());
    inCreateInfo.ppEnabledLayerNames = dummyLayerNames.data();

    std::vector<const char*> outExtensions{ VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME };

    VkInstanceCreateInfo outCreateInfo = inCreateInfo;
    outCreateInfo.enabledExtensionCount = static_cast<uint32_t>(outExtensions.size());
    outCreateInfo.ppEnabledExtensionNames = outExtensions.data();

    mock.SetExpectedInstanceCreateInfo(&outCreateInfo, {});

    VpInstanceCreateInfo createInfo{ &inCreateInfo, &profile, VP_INSTANCE_CREATE_OVERRIDE_EXTENSIONS_BIT };

    VkInstance instance = VK_NULL_HANDLE;
    VkResult result = vpCreateInstance(&createInfo, &mock.vkAllocator, &instance);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_TRUE(instance == mock.vkInstance);
}

TEST(mocked_api_create_instance, vulkan11) {
    MockVulkanAPI mock;

    VpProfileProperties profile{ VP_ANDROID_BASELINE_2021_NAME, VP_ANDROID_BASELINE_2021_SPEC_VERSION };

    std::vector<const char*> dummyLayerNames{ "VK_DUMMY_layer1", "VK_DUMMY_layer2", "VK_DUMMY_layer3" };

    VkApplicationInfo inAppInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
    inAppInfo.pApplicationName = "Dummy application name";
    inAppInfo.applicationVersion = VK_MAKE_VERSION(1, 3, 5);
    inAppInfo.pEngineName = "Dummy engine name";
    inAppInfo.engineVersion = VK_MAKE_VERSION(2, 4, 6);
    inAppInfo.apiVersion = VK_API_VERSION_1_1;

    VkInstanceCreateInfo inCreateInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    inCreateInfo.pApplicationInfo = &inAppInfo;
    inCreateInfo.enabledLayerCount = static_cast<uint32_t>(dummyLayerNames.size());
    inCreateInfo.ppEnabledLayerNames = dummyLayerNames.data();

    VkInstanceCreateInfo outCreateInfo = inCreateInfo;

    mock.SetExpectedInstanceCreateInfo(&outCreateInfo, {});

    VpInstanceCreateInfo createInfo{ &inCreateInfo, &profile, VP_INSTANCE_CREATE_OVERRIDE_EXTENSIONS_BIT };

    VkInstance instance = VK_NULL_HANDLE;
    VkResult result = vpCreateInstance(&createInfo, &mock.vkAllocator, &instance);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_TRUE(instance == mock.vkInstance);
}

TEST(mocked_api_create_instance, default_extensions) {
    MockVulkanAPI mock;

    VpProfileProperties profile{ VP_ANDROID_BASELINE_2021_NAME, VP_ANDROID_BASELINE_2021_SPEC_VERSION };

    VkApplicationInfo inAppInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
    inAppInfo.apiVersion = VK_API_VERSION_1_1;

    VkInstanceCreateInfo inCreateInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    inCreateInfo.pApplicationInfo = &inAppInfo;

    std::vector<const char*> outExtensions(sizeof(detail::VP_ANDROID_BASELINE_2021::instanceExtensions) /
                                           sizeof(detail::VP_ANDROID_BASELINE_2021::instanceExtensions[0]));
    for (size_t i = 0; i < outExtensions.size(); ++i) {
        outExtensions[i] = detail::VP_ANDROID_BASELINE_2021::instanceExtensions[i].extensionName;
    }

    VkInstanceCreateInfo outCreateInfo = inCreateInfo;
    outCreateInfo.enabledExtensionCount = static_cast<uint32_t>(outExtensions.size());
    outCreateInfo.ppEnabledExtensionNames = outExtensions.data();

    mock.SetExpectedInstanceCreateInfo(&outCreateInfo, {});

    VpInstanceCreateInfo createInfo{ &inCreateInfo, &profile, 0 };

    VkInstance instance = VK_NULL_HANDLE;
    VkResult result = vpCreateInstance(&createInfo, &mock.vkAllocator, &instance);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_TRUE(instance == mock.vkInstance);
}

TEST(mocked_api_create_instance, default_extensions_negative) {
    MockVulkanAPI mock;

    VpProfileProperties profile{ VP_ANDROID_BASELINE_2021_NAME, VP_ANDROID_BASELINE_2021_SPEC_VERSION };

    VkApplicationInfo inAppInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
    inAppInfo.apiVersion = VK_API_VERSION_1_1;

    std::vector<const char*> inExtensions{
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_DISPLAY_EXTENSION_NAME,
        VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME
    };

    VkInstanceCreateInfo inCreateInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    inCreateInfo.pApplicationInfo = &inAppInfo;
    inCreateInfo.enabledExtensionCount = static_cast<uint32_t>(inExtensions.size());
    inCreateInfo.ppEnabledExtensionNames = inExtensions.data();

    std::vector<const char*> outExtensions(sizeof(detail::VP_ANDROID_BASELINE_2021::instanceExtensions) /
                                           sizeof(detail::VP_ANDROID_BASELINE_2021::instanceExtensions[0]));
    for (size_t i = 0; i < outExtensions.size(); ++i) {
        outExtensions[i] = detail::VP_ANDROID_BASELINE_2021::instanceExtensions[i].extensionName;
    }

    VkInstanceCreateInfo outCreateInfo = inCreateInfo;
    outCreateInfo.enabledExtensionCount = static_cast<uint32_t>(outExtensions.size());
    outCreateInfo.ppEnabledExtensionNames = outExtensions.data();

    mock.SetExpectedInstanceCreateInfo(&outCreateInfo, {});

    VpInstanceCreateInfo createInfo{ &inCreateInfo, &profile, 0 };

    VkInstance instance = VK_NULL_HANDLE;
    VkResult result = vpCreateInstance(&createInfo, &mock.vkAllocator, &instance);

    // Currently application must not specify its own extension list if no override or merge
    // flag is used. This leaves us the door open to use merge behavior as the default in
    // the future if we ever decide to do so.
    EXPECT_EQ(result, VK_ERROR_UNKNOWN);
    EXPECT_TRUE(instance == VK_NULL_HANDLE);
}

TEST(mocked_api_create_instance, override_extensions) {
    MockVulkanAPI mock;

    VpProfileProperties profile{ VP_ANDROID_BASELINE_2021_NAME, VP_ANDROID_BASELINE_2021_SPEC_VERSION };

    VkApplicationInfo inAppInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
    inAppInfo.apiVersion = VK_API_VERSION_1_1;

    std::vector<const char*> inExtensions{
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_DISPLAY_EXTENSION_NAME,
        VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME
    };

    VkInstanceCreateInfo inCreateInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    inCreateInfo.pApplicationInfo = &inAppInfo;
    inCreateInfo.enabledExtensionCount = static_cast<uint32_t>(inExtensions.size());
    inCreateInfo.ppEnabledExtensionNames = inExtensions.data();

    VkInstanceCreateInfo outCreateInfo = inCreateInfo;

    mock.SetExpectedInstanceCreateInfo(&outCreateInfo, {});

    VpInstanceCreateInfo createInfo{ &inCreateInfo, &profile, VP_INSTANCE_CREATE_OVERRIDE_EXTENSIONS_BIT };

    VkInstance instance = VK_NULL_HANDLE;
    VkResult result = vpCreateInstance(&createInfo, &mock.vkAllocator, &instance);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_TRUE(instance == mock.vkInstance);
}

TEST(mocked_api_create_instance, merge_extensions) {
    MockVulkanAPI mock;

    VpProfileProperties profile{ VP_ANDROID_BASELINE_2021_NAME, VP_ANDROID_BASELINE_2021_SPEC_VERSION };

    VkApplicationInfo inAppInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
    inAppInfo.apiVersion = VK_API_VERSION_1_1;

    std::vector<const char*> inExtensions{
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_DISPLAY_EXTENSION_NAME,
        VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME
    };

    VkInstanceCreateInfo inCreateInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    inCreateInfo.pApplicationInfo = &inAppInfo;
    inCreateInfo.enabledExtensionCount = static_cast<uint32_t>(inExtensions.size());
    inCreateInfo.ppEnabledExtensionNames = inExtensions.data();

    std::vector<const char*> outExtensions(sizeof(detail::VP_ANDROID_BASELINE_2021::instanceExtensions) /
                                           sizeof(detail::VP_ANDROID_BASELINE_2021::instanceExtensions[0]));
    for (size_t i = 0; i < outExtensions.size(); ++i) {
        outExtensions[i] = detail::VP_ANDROID_BASELINE_2021::instanceExtensions[i].extensionName;
    }
    outExtensions.push_back(VK_KHR_DISPLAY_EXTENSION_NAME);

    VkInstanceCreateInfo outCreateInfo = inCreateInfo;
    outCreateInfo.enabledExtensionCount = static_cast<uint32_t>(outExtensions.size());
    outCreateInfo.ppEnabledExtensionNames = outExtensions.data();

    mock.SetExpectedInstanceCreateInfo(&outCreateInfo, {});

    VpInstanceCreateInfo createInfo{ &inCreateInfo, &profile, VP_INSTANCE_CREATE_MERGE_EXTENSIONS_BIT };

    VkInstance instance = VK_NULL_HANDLE;
    VkResult result = vpCreateInstance(&createInfo, &mock.vkAllocator, &instance);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_TRUE(instance == mock.vkInstance);
}


TEST(mocked_api_create_instance, retain_chained_structs) {
    MockVulkanAPI mock;

    VpProfileProperties profile{ VP_ANDROID_BASELINE_2021_NAME, VP_ANDROID_BASELINE_2021_SPEC_VERSION };

    int dummyData;
    VkValidationFlagsEXT validationFlags{ VK_STRUCTURE_TYPE_VALIDATION_FLAGS_EXT };
    VkValidationFeaturesEXT validationFeatures{ VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT, &validationFlags };
    VkDebugReportCallbackCreateInfoEXT debugReportCallback{ VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT, &validationFeatures };
    debugReportCallback.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT;
    debugReportCallback.pUserData = &dummyData;

    VkInstanceCreateInfo inCreateInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, &debugReportCallback };

    std::vector<const char*> outExtensions{ VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME };

    VkApplicationInfo outAppInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
    outAppInfo.apiVersion = VP_ANDROID_BASELINE_2021_MIN_API_VERSION;

    VkInstanceCreateInfo outCreateInfo = inCreateInfo;
    outCreateInfo.pApplicationInfo = &outAppInfo;
    outCreateInfo.enabledExtensionCount = static_cast<uint32_t>(outExtensions.size());
    outCreateInfo.ppEnabledExtensionNames = outExtensions.data();

    mock.SetExpectedInstanceCreateInfo(&outCreateInfo, {
        VK_STRUCT(validationFlags),
        VK_STRUCT(validationFeatures),
        VK_STRUCT(debugReportCallback),
    });

    VpInstanceCreateInfo createInfo{ &inCreateInfo, &profile, VP_INSTANCE_CREATE_OVERRIDE_EXTENSIONS_BIT };

    VkInstance instance = VK_NULL_HANDLE;
    VkResult result = vpCreateInstance(&createInfo, &mock.vkAllocator, &instance);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_TRUE(instance == mock.vkInstance);
}
