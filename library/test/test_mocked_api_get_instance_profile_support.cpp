/*
 * Copyright (c) 2021-2025 Valve Corporation
 * Copyright (c) 2021-2025 LunarG, Inc.
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
 * - Christophe Riccio <christophe@lunarg.com>
 * - Daniel Rakos <daniel.rakos@rastergrid.com>
 */

#include "mock_vulkan_api.hpp"
#include "mock_debug_message_callback.hpp"
#include <vulkan/vulkan_android.h>

#ifdef WITH_DEBUG_MESSAGES
#include <vulkan/debug/vulkan_profiles.hpp>
#else
#include <vulkan/vulkan_profiles.hpp>
#endif

TEST(mocked_api_get_instance_profile_support, vulkan10_supported) {
    MockVulkanAPI mock;

    mock.SetInstanceAPIVersion(VK_API_VERSION_1_0);

    mock.SetInstanceExtensions(nullptr, {
        VK_EXT(VK_KHR_DISPLAY),
        VK_EXT(VK_KHR_SURFACE),
        VK_EXT(VK_KHR_ANDROID_SURFACE),
        VK_EXT(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2),
        VK_EXT(VK_KHR_SURFACE_PROTECTED_CAPABILITIES),
        VK_EXT(VK_KHR_GET_SURFACE_CAPABILITIES_2),
        VK_EXT(VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES),
        VK_EXT(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES),
        VK_EXT(VK_KHR_EXTERNAL_FENCE_CAPABILITIES),
        VK_EXT(VK_EXT_SWAPCHAIN_COLOR_SPACE),
    });

    VpProfileProperties profile{ VP_ANDROID_BASELINE_2021_NAME, VP_ANDROID_BASELINE_2021_SPEC_VERSION };

    VkBool32 supported = VK_FALSE;
    VkResult result = vpGetInstanceProfileSupport(nullptr, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_TRUE);
}

TEST(mocked_api_get_instance_profile_support, vulkan10_no_gpdp2) {
    MockVulkanAPI mock;

#ifdef WITH_DEBUG_MESSAGES
    MockDebugMessageCallback cb({
        "Unsupported mandatory extension VK_KHR_get_physical_device_properties2 on Vulkan 1.0",
        "Unsupported extension: VK_KHR_get_physical_device_properties2"
    });
#endif

    mock.SetInstanceAPIVersion(VK_API_VERSION_1_0);

    mock.SetInstanceExtensions(nullptr, {
        VK_EXT(VK_KHR_DISPLAY),
        VK_EXT(VK_KHR_SURFACE),
        VK_EXT(VK_KHR_ANDROID_SURFACE),
        VK_EXT(VK_KHR_SURFACE_PROTECTED_CAPABILITIES),
        VK_EXT(VK_KHR_GET_SURFACE_CAPABILITIES_2),
        VK_EXT(VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES),
        VK_EXT(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES),
        VK_EXT(VK_KHR_EXTERNAL_FENCE_CAPABILITIES),
        VK_EXT(VK_EXT_SWAPCHAIN_COLOR_SPACE),
    });

    VpProfileProperties profile{ VP_ANDROID_BASELINE_2021_NAME, VP_ANDROID_BASELINE_2021_SPEC_VERSION };

    VkBool32 supported = VK_TRUE;
    VkResult result = vpGetInstanceProfileSupport(nullptr, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_FALSE);
}

TEST(mocked_api_get_instance_profile_support, vulkan10_unsupported_version) {
    MockVulkanAPI mock;

#ifdef WITH_DEBUG_MESSAGES
    MockDebugMessageCallback cb({
        "Unsupported Profile API version 1.3.204 on a Vulkan system with version 1.0.0"
    });
#endif

    mock.SetInstanceAPIVersion(VK_API_VERSION_1_0);

    mock.SetInstanceExtensions(nullptr, {
        VK_EXT(VK_KHR_SURFACE),
        VK_EXT(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2),
        VK_EXT(VK_KHR_GET_SURFACE_CAPABILITIES_2),
        VK_EXT(VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES),
        VK_EXT(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES),
        VK_EXT(VK_KHR_EXTERNAL_FENCE_CAPABILITIES),
    });

    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION};

    VkBool32 supported = VK_TRUE;
    VkResult result = vpGetInstanceProfileSupport(nullptr, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_FALSE);
}

TEST(mocked_api_get_instance_profile_support, vulkan10_unsupported_extension) {
    MockVulkanAPI mock;

#ifdef WITH_DEBUG_MESSAGES
    MockDebugMessageCallback cb({
        "Unsupported extension: VK_KHR_get_surface_capabilities2"
    });
#endif

    mock.SetInstanceExtensions(nullptr, {
        VK_EXT(VK_KHR_DISPLAY),
        VK_EXT(VK_KHR_SURFACE),
        VK_EXT(VK_KHR_ANDROID_SURFACE),
        VK_EXT(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2),
        VK_EXT(VK_KHR_SURFACE_PROTECTED_CAPABILITIES),
        VK_EXT(VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES),
        VK_EXT(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES),
        VK_EXT(VK_KHR_EXTERNAL_FENCE_CAPABILITIES),
        VK_EXT(VK_EXT_SWAPCHAIN_COLOR_SPACE),
    });

    VpProfileProperties profile{ VP_ANDROID_BASELINE_2021_NAME, VP_ANDROID_BASELINE_2021_SPEC_VERSION };

    VkBool32 supported = VK_TRUE;
    VkResult result = vpGetInstanceProfileSupport(nullptr, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_FALSE);
}

TEST(mocked_api_get_instance_profile_support, vulkan11_supported) {
    MockVulkanAPI mock;

    mock.SetInstanceAPIVersion(VK_API_VERSION_1_1);

    mock.SetInstanceExtensions(nullptr, {
        VK_EXT(VK_KHR_DISPLAY),
        VK_EXT(VK_KHR_SURFACE),
        VK_EXT(VK_KHR_ANDROID_SURFACE),
        VK_EXT(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2),
        VK_EXT(VK_KHR_SURFACE_PROTECTED_CAPABILITIES),
        VK_EXT(VK_KHR_GET_SURFACE_CAPABILITIES_2),
        VK_EXT(VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES),
        VK_EXT(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES),
        VK_EXT(VK_KHR_EXTERNAL_FENCE_CAPABILITIES),
        VK_EXT(VK_EXT_SWAPCHAIN_COLOR_SPACE),
    });

    VpProfileProperties profile{ VP_ANDROID_BASELINE_2021_NAME, VP_ANDROID_BASELINE_2021_SPEC_VERSION };

    VkBool32 supported = VK_FALSE;
    VkResult result = vpGetInstanceProfileSupport(nullptr, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_TRUE);
}

TEST(mocked_api_get_instance_profile_support, vulkan11_unsupported_version) {
    MockVulkanAPI mock;

#ifdef WITH_DEBUG_MESSAGES
    MockDebugMessageCallback cb({
        "Unsupported Profile API version 1.3.204 on a Vulkan system with version 1.1.0"
    });
#endif

    mock.SetInstanceAPIVersion(VK_API_VERSION_1_1);

    mock.SetInstanceExtensions(nullptr, {
        VK_EXT(VK_KHR_SURFACE),
        VK_EXT(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2),
        VK_EXT(VK_KHR_GET_SURFACE_CAPABILITIES_2),
        VK_EXT(VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES),
        VK_EXT(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES),
        VK_EXT(VK_KHR_EXTERNAL_FENCE_CAPABILITIES),
    });

    VpProfileProperties profile{ VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION };

    VkBool32 supported = VK_TRUE;
    VkResult result = vpGetInstanceProfileSupport(nullptr, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_FALSE);
}

TEST(mocked_api_get_instance_profile_support, vulkan11_unsupported_extension) {
    MockVulkanAPI mock;

#ifdef WITH_DEBUG_MESSAGES
    MockDebugMessageCallback cb({
        "Unsupported extension: VK_KHR_android_surface"
    });
#endif

    mock.SetInstanceAPIVersion(VK_API_VERSION_1_1);

    mock.SetInstanceExtensions(nullptr, {
        VK_EXT(VK_KHR_DISPLAY),
        VK_EXT(VK_KHR_SURFACE),
        VK_EXT(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2),
        VK_EXT(VK_KHR_SURFACE_PROTECTED_CAPABILITIES),
        VK_EXT(VK_KHR_GET_SURFACE_CAPABILITIES_2),
        VK_EXT(VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES),
        VK_EXT(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES),
        VK_EXT(VK_KHR_EXTERNAL_FENCE_CAPABILITIES),
        VK_EXT(VK_EXT_SWAPCHAIN_COLOR_SPACE),
    });

    VpProfileProperties profile{ VP_ANDROID_BASELINE_2021_NAME, VP_ANDROID_BASELINE_2021_SPEC_VERSION };

    VkBool32 supported = VK_TRUE;
    VkResult result = vpGetInstanceProfileSupport(nullptr, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_FALSE);
}

TEST(mocked_api_get_instance_profile_support, layer_supported) {
    MockVulkanAPI mock;

    mock.SetInstanceAPIVersion(VK_API_VERSION_1_1);

    mock.SetInstanceExtensions("VK_DUMMY_layer1", {
        VK_EXT(VK_KHR_DISPLAY),
        VK_EXT(VK_KHR_SURFACE),
        VK_EXT(VK_KHR_ANDROID_SURFACE),
        VK_EXT(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2),
        VK_EXT(VK_KHR_SURFACE_PROTECTED_CAPABILITIES),
        VK_EXT(VK_KHR_GET_SURFACE_CAPABILITIES_2),
        VK_EXT(VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES),
        VK_EXT(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES),
        VK_EXT(VK_KHR_EXTERNAL_FENCE_CAPABILITIES),
        VK_EXT(VK_EXT_SWAPCHAIN_COLOR_SPACE),
    });

    VpProfileProperties profile{ VP_ANDROID_BASELINE_2021_NAME, VP_ANDROID_BASELINE_2021_SPEC_VERSION };

    VkBool32 supported = VK_FALSE;
    VkResult result = vpGetInstanceProfileSupport("VK_DUMMY_layer1", &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_TRUE);
}

TEST(mocked_api_get_instance_profile_support, layer_unsupported) {
    MockVulkanAPI mock;

#ifdef WITH_DEBUG_MESSAGES
    MockDebugMessageCallback cb({
        "Unsupported extension: VK_EXT_swapchain_colorspace",
        "Unsupported extension: VK_KHR_android_surface",
        "Unsupported extension: VK_KHR_external_fence_capabilities",
        "Unsupported extension: VK_KHR_external_memory_capabilities",
        "Unsupported extension: VK_KHR_external_semaphore_capabilities",
        "Unsupported extension: VK_KHR_get_physical_device_properties2",
        "Unsupported extension: VK_KHR_get_surface_capabilities2"
    });
#endif

    mock.SetInstanceAPIVersion(VK_API_VERSION_1_1);

    mock.SetInstanceExtensions("VK_DUMMY_layer1", {
        VK_EXT(VK_KHR_DISPLAY),
        VK_EXT(VK_KHR_SURFACE),
        VK_EXT(VK_KHR_ANDROID_SURFACE),
        VK_EXT(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2),
        VK_EXT(VK_KHR_SURFACE_PROTECTED_CAPABILITIES),
        VK_EXT(VK_KHR_GET_SURFACE_CAPABILITIES_2),
        VK_EXT(VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES),
        VK_EXT(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES),
        VK_EXT(VK_KHR_EXTERNAL_FENCE_CAPABILITIES),
        VK_EXT(VK_EXT_SWAPCHAIN_COLOR_SPACE),
    });
    mock.SetInstanceExtensions("VK_DUMMY_layer2", {
        VK_EXT(VK_KHR_SURFACE),
    });

    VpProfileProperties profile{ VP_ANDROID_BASELINE_2021_NAME, VP_ANDROID_BASELINE_2021_SPEC_VERSION };

    VkBool32 supported = VK_TRUE;
    VkResult result = vpGetInstanceProfileSupport("VK_DUMMY_layer2", &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_FALSE);
}
