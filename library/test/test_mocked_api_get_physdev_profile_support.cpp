/*
 * Copyright (c) 2021-2022 Valve Corporation
 * Copyright (c) 2021-2022 LunarG, Inc.
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
#include "mock_debug_message_callback.hpp"
#include <vulkan/vulkan_android.h>

#if WITH_DEBUG_MESSAGES
#include <vulkan/debug/vulkan_profiles.hpp>
#else
#include <vulkan/vulkan_profiles.hpp>
#endif

TEST(mocked_api_get_physdev_profile_support, vulkan10_supported) {
    MockVulkanAPI mock;

#if WITH_DEBUG_MESSAGES
    MockDebugMessageCallback cb({
        "Checking device support for profile VP_ANDROID_baseline_2021 "
            "(deviceName=, driverName=, driverInfo=). You may find the details "
            "of the capabilities of this device on https://vulkan.gpuinfo.org/"
    });
#endif

    mock.SetInstanceAPIVersion(VK_API_VERSION_1_0);
    mock.SetDeviceAPIVersion(VK_API_VERSION_1_0);

    mock.SetDeviceExtensions(mock.vkPhysicalDevice, {
        VK_EXT(VK_KHR_SWAPCHAIN),
        VK_EXT(VK_KHR_MAINTENANCE_1),
        VK_EXT(VK_KHR_MAINTENANCE_2),
        VK_EXT(VK_KHR_INCREMENTAL_PRESENT),
        VK_EXT(VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE),
        VK_EXT(VK_KHR_GET_MEMORY_REQUIREMENTS_2),
        VK_EXT(VK_KHR_DEDICATED_ALLOCATION),
        VK_EXT(VK_KHR_STORAGE_BUFFER_STORAGE_CLASS),
        VK_EXT(VK_KHR_VARIABLE_POINTERS),
        VK_EXT(VK_KHR_EXTERNAL_SEMAPHORE),
        VK_EXT(VK_KHR_EXTERNAL_SEMAPHORE_FD),
        VK_EXT(VK_KHR_EXTERNAL_MEMORY),
        VK_EXT(VK_KHR_EXTERNAL_MEMORY_FD),
        VK_EXT(VK_KHR_EXTERNAL_FENCE),
        VK_EXT(VK_KHR_EXTERNAL_FENCE_FD),
        VK_EXT(VK_EXT_DESCRIPTOR_INDEXING),
        VK_EXT(VK_GOOGLE_DISPLAY_TIMING),
    });

    VpProfileProperties profile{ VP_ANDROID_BASELINE_2021_NAME, VP_ANDROID_BASELINE_2021_SPEC_VERSION };

    VkPhysicalDeviceMultiviewFeaturesKHR multiviewFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR };
    VkPhysicalDeviceFeatures2KHR features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR, &multiviewFeatures };
    vpGetProfileFeatures(&profile, &features);
    features.features.dualSrcBlend = VK_TRUE;
    features.features.drawIndirectFirstInstance = VK_TRUE;
    multiviewFeatures.multiview = VK_TRUE;
    mock.SetFeatures({
        VK_STRUCT(features),
        VK_STRUCT(multiviewFeatures)
    });

    VkPhysicalDeviceMultiviewPropertiesKHR multiviewProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR };
    VkPhysicalDeviceProperties2KHR props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR, &multiviewProps };
    vpGetProfileProperties(&profile, &props);
    props.properties.limits.maxImageDimension2D = 16384;
    props.properties.limits.maxBoundDescriptorSets = 8;
    props.properties.limits.subPixelPrecisionBits = 8;
    props.properties.limits.maxViewports = 4;
    props.properties.limits.viewportBoundsRange[0] = -16384;
    props.properties.limits.viewportBoundsRange[1] = 16384;
    props.properties.limits.framebufferColorSampleCounts |= VK_SAMPLE_COUNT_2_BIT;
    props.properties.limits.pointSizeRange[0] = 1.f;
    props.properties.limits.pointSizeRange[1] = 32.f;
    props.properties.limits.pointSizeGranularity = 0.125f;
    multiviewProps.maxMultiviewViewCount = 6;
    multiviewProps.maxMultiviewInstanceIndex = 65536;
    mock.SetProperties({
        VK_STRUCT(props),
        VK_STRUCT(multiviewProps)
    });

    uint32_t formatCount;
    vpGetProfileFormats(&profile, &formatCount, nullptr);
    std::vector<VkFormat> formats(formatCount);
    vpGetProfileFormats(&profile, &formatCount, formats.data());
    for (size_t i = 0; i < formatCount; ++i) {
        VkFormatProperties2KHR formatProps{ VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR };
        vpGetProfileFormatProperties(&profile, formats[i], &formatProps);
        formatProps.formatProperties.optimalTilingFeatures |= VK_FORMAT_FEATURE_BLIT_SRC_BIT;
        formatProps.formatProperties.bufferFeatures |= VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
        mock.AddFormat(formats[i], { VK_STRUCT(formatProps) });
    }

    VkQueueFamilyProperties2KHR queueFamilyProps{ VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2_KHR };
    queueFamilyProps.queueFamilyProperties.queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;
    queueFamilyProps.queueFamilyProperties.queueCount = 2;
    queueFamilyProps.queueFamilyProperties.timestampValidBits = 63;
    queueFamilyProps.queueFamilyProperties.minImageTransferGranularity = { 1, 1, 1 };
    mock.AddQueueFamily({ VK_STRUCT(queueFamilyProps) });

    VkBool32 supported = VK_FALSE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_TRUE);
}

TEST(mocked_api_get_physdev_profile_support, vulkan10_no_gpdp2) {
    MockVulkanAPI mock;

    // We don't set an instance version which will also result in no GPDP2 entry points
    mock.SetDeviceAPIVersion(VK_API_VERSION_1_0);

    mock.SetDeviceExtensions(mock.vkPhysicalDevice, {
        VK_EXT(VK_KHR_SWAPCHAIN),
        VK_EXT(VK_KHR_MAINTENANCE_1),
        VK_EXT(VK_KHR_MAINTENANCE_2),
        VK_EXT(VK_KHR_INCREMENTAL_PRESENT),
        VK_EXT(VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE),
        VK_EXT(VK_KHR_GET_MEMORY_REQUIREMENTS_2),
        VK_EXT(VK_KHR_DEDICATED_ALLOCATION),
        VK_EXT(VK_KHR_STORAGE_BUFFER_STORAGE_CLASS),
        VK_EXT(VK_KHR_VARIABLE_POINTERS),
        VK_EXT(VK_KHR_EXTERNAL_SEMAPHORE),
        VK_EXT(VK_KHR_EXTERNAL_SEMAPHORE_FD),
        VK_EXT(VK_KHR_EXTERNAL_MEMORY),
        VK_EXT(VK_KHR_EXTERNAL_MEMORY_FD),
        VK_EXT(VK_KHR_EXTERNAL_FENCE),
        VK_EXT(VK_KHR_EXTERNAL_FENCE_FD),
        VK_EXT(VK_EXT_DESCRIPTOR_INDEXING),
        VK_EXT(VK_GOOGLE_DISPLAY_TIMING),
    });

    VpProfileProperties profile{ VP_ANDROID_BASELINE_2021_NAME, VP_ANDROID_BASELINE_2021_SPEC_VERSION };

    VkPhysicalDeviceMultiviewFeaturesKHR multiviewFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR };
    VkPhysicalDeviceFeatures2KHR features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR, &multiviewFeatures };
    vpGetProfileFeatures(&profile, &features);
    features.features.dualSrcBlend = VK_TRUE;
    features.features.drawIndirectFirstInstance = VK_TRUE;
    multiviewFeatures.multiview = VK_TRUE;
    mock.SetFeatures({
        VK_STRUCT(features),
        VK_STRUCT(multiviewFeatures)
    });

    VkPhysicalDeviceMultiviewPropertiesKHR multiviewProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR };
    VkPhysicalDeviceProperties2KHR props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR, &multiviewProps };
    vpGetProfileProperties(&profile, &props);
    props.properties.limits.maxImageDimension2D = 16384;
    props.properties.limits.maxBoundDescriptorSets = 8;
    props.properties.limits.subPixelPrecisionBits = 8;
    props.properties.limits.maxViewports = 4;
    props.properties.limits.viewportBoundsRange[0] = -16384;
    props.properties.limits.viewportBoundsRange[1] = 16384;
    props.properties.limits.framebufferColorSampleCounts |= VK_SAMPLE_COUNT_2_BIT;
    props.properties.limits.pointSizeRange[0] = 1.f;
    props.properties.limits.pointSizeRange[1] = 32.f;
    props.properties.limits.pointSizeGranularity = 0.125f;
    multiviewProps.maxMultiviewViewCount = 6;
    multiviewProps.maxMultiviewInstanceIndex = 65536;
    mock.SetProperties({
        VK_STRUCT(props),
        VK_STRUCT(multiviewProps)
    });

    uint32_t formatCount;
    vpGetProfileFormats(&profile, &formatCount, nullptr);
    std::vector<VkFormat> formats(formatCount);
    vpGetProfileFormats(&profile, &formatCount, formats.data());
    for (size_t i = 0; i < formatCount; ++i) {
        VkFormatProperties2KHR formatProps{ VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR };
        vpGetProfileFormatProperties(&profile, formats[i], &formatProps);
        formatProps.formatProperties.optimalTilingFeatures |= VK_FORMAT_FEATURE_BLIT_SRC_BIT;
        formatProps.formatProperties.bufferFeatures |= VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
        mock.AddFormat(formats[i], { VK_STRUCT(formatProps) });
    }

    VkQueueFamilyProperties2KHR queueFamilyProps{ VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2_KHR };
    queueFamilyProps.queueFamilyProperties.queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;
    queueFamilyProps.queueFamilyProperties.queueCount = 2;
    queueFamilyProps.queueFamilyProperties.timestampValidBits = 63;
    queueFamilyProps.queueFamilyProperties.minImageTransferGranularity = { 1, 1, 1 };
    mock.AddQueueFamily({ VK_STRUCT(queueFamilyProps) });

    VkBool32 supported = VK_TRUE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_ERROR_EXTENSION_NOT_PRESENT);
    EXPECT_EQ(supported, VK_TRUE);
}

TEST(mocked_api_get_physdev_profile_support, vulkan10_unsupported_version) {
    MockVulkanAPI mock;

#if WITH_DEBUG_MESSAGES
    MockDebugMessageCallback cb({
        "Checking device support for profile VP_KHR_roadmap_2022 "
            "(deviceName=, driverName=, driverInfo=). You may find the details "
            "of the capabilities of this device on https://vulkan.gpuinfo.org/",
        "Unsupported API version: 1.3.204"
    });
#endif

    // Unsupported version
    mock.SetInstanceAPIVersion(VK_API_VERSION_1_0);
    mock.SetDeviceAPIVersion(VK_API_VERSION_1_0);

    mock.SetDeviceExtensions(mock.vkPhysicalDevice, {
        VK_EXT(VK_KHR_GLOBAL_PRIORITY),
    });

    VpProfileProperties profile{VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION};

    VkPhysicalDeviceVulkan13Features vulkan13Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
    VkPhysicalDeviceVulkan12Features vulkan12Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, &vulkan13Features};
    VkPhysicalDeviceVulkan11Features vulkan11Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, &vulkan12Features};
    VkPhysicalDeviceFeatures2 features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &vulkan11Features};
    vpGetProfileFeatures(&profile, &features);

    mock.SetFeatures({
        VK_STRUCT(features), VK_STRUCT(vulkan11Features), VK_STRUCT(vulkan12Features), VK_STRUCT(vulkan13Features)
    });

    VkPhysicalDeviceVulkan13Properties vulkan13Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES};
    VkPhysicalDeviceVulkan12Properties vulkan12Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES, &vulkan13Properties};
    VkPhysicalDeviceVulkan11Properties vulkan11Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES, &vulkan12Properties};
    VkPhysicalDeviceProperties2 props{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, &vulkan11Properties};
    vpGetProfileProperties(&profile, &props);

    mock.SetProperties(
        {VK_STRUCT(props), VK_STRUCT(vulkan11Properties), VK_STRUCT(vulkan12Properties), VK_STRUCT(vulkan13Properties)
    });

    uint32_t formatCount;
    vpGetProfileFormats(&profile, &formatCount, nullptr);
    std::vector<VkFormat> formats(formatCount);
    vpGetProfileFormats(&profile, &formatCount, formats.data());
    for (size_t i = 0; i < formatCount; ++i) {
        VkFormatProperties2KHR formatProps{ VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR };
        vpGetProfileFormatProperties(&profile, formats[i], &formatProps);
        formatProps.formatProperties.optimalTilingFeatures |= VK_FORMAT_FEATURE_BLIT_SRC_BIT;
        formatProps.formatProperties.bufferFeatures |= VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
        mock.AddFormat(formats[i], { VK_STRUCT(formatProps) });
    }

    VkQueueFamilyProperties2KHR queueFamilyProps{ VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2_KHR };
    queueFamilyProps.queueFamilyProperties.queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT;
    queueFamilyProps.queueFamilyProperties.queueCount = 1;
    queueFamilyProps.queueFamilyProperties.timestampValidBits = 63;
    queueFamilyProps.queueFamilyProperties.minImageTransferGranularity = { 1, 1, 1 };
    mock.AddQueueFamily({ VK_STRUCT(queueFamilyProps) });

    queueFamilyProps.queueFamilyProperties.queueFlags = VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT;
    queueFamilyProps.queueFamilyProperties.queueCount = 4;
    queueFamilyProps.queueFamilyProperties.timestampValidBits = 63;
    queueFamilyProps.queueFamilyProperties.minImageTransferGranularity = { 1, 1, 1 };
    mock.AddQueueFamily({ VK_STRUCT(queueFamilyProps) });

    queueFamilyProps.queueFamilyProperties.queueFlags = VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT;
    queueFamilyProps.queueFamilyProperties.queueCount = 2;
    queueFamilyProps.queueFamilyProperties.timestampValidBits = 63;
    queueFamilyProps.queueFamilyProperties.minImageTransferGranularity = { 8, 8, 8 };
    mock.AddQueueFamily({ VK_STRUCT(queueFamilyProps) });

    VkBool32 supported = VK_TRUE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_FALSE);
}

TEST(mocked_api_get_physdev_profile_support, vulkan10_unsupported_extension) {
    MockVulkanAPI mock;

#if WITH_DEBUG_MESSAGES
    MockDebugMessageCallback cb({
        "Checking device support for profile VP_ANDROID_baseline_2021 "
            "(deviceName=, driverName=, driverInfo=). You may find the details "
            "of the capabilities of this device on https://vulkan.gpuinfo.org/",
        "Unsupported extension: VK_GOOGLE_display_timing"
    });
#endif

    mock.SetInstanceAPIVersion(VK_API_VERSION_1_0);
    mock.SetDeviceAPIVersion(VK_API_VERSION_1_0);

    mock.SetDeviceExtensions(mock.vkPhysicalDevice, {
        VK_EXT(VK_KHR_SWAPCHAIN),
        VK_EXT(VK_KHR_MAINTENANCE_1),
        VK_EXT(VK_KHR_MAINTENANCE_2),
        VK_EXT(VK_KHR_INCREMENTAL_PRESENT),
        VK_EXT(VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE),
        VK_EXT(VK_KHR_GET_MEMORY_REQUIREMENTS_2),
        VK_EXT(VK_KHR_DEDICATED_ALLOCATION),
        VK_EXT(VK_KHR_STORAGE_BUFFER_STORAGE_CLASS),
        VK_EXT(VK_KHR_VARIABLE_POINTERS),
        VK_EXT(VK_KHR_EXTERNAL_SEMAPHORE),
        VK_EXT(VK_KHR_EXTERNAL_SEMAPHORE_FD),
        VK_EXT(VK_KHR_EXTERNAL_MEMORY),
        VK_EXT(VK_KHR_EXTERNAL_MEMORY_FD),
        VK_EXT(VK_KHR_EXTERNAL_FENCE),
        VK_EXT(VK_KHR_EXTERNAL_FENCE_FD),
        VK_EXT(VK_EXT_DESCRIPTOR_INDEXING),
        // Unsupported extension: VK_GOOGLE_display_timing
    });

    VpProfileProperties profile{ VP_ANDROID_BASELINE_2021_NAME, VP_ANDROID_BASELINE_2021_SPEC_VERSION };

    VkPhysicalDeviceMultiviewFeaturesKHR multiviewFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR };
    VkPhysicalDeviceFeatures2KHR features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR, &multiviewFeatures };
    vpGetProfileFeatures(&profile, &features);
    features.features.dualSrcBlend = VK_TRUE;
    features.features.drawIndirectFirstInstance = VK_TRUE;
    multiviewFeatures.multiview = VK_TRUE;
    mock.SetFeatures({
        VK_STRUCT(features),
        VK_STRUCT(multiviewFeatures)
    });

    VkPhysicalDeviceMultiviewPropertiesKHR multiviewProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR };
    VkPhysicalDeviceProperties2KHR props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR, &multiviewProps };
    vpGetProfileProperties(&profile, &props);
    props.properties.limits.maxImageDimension2D = 16384;
    props.properties.limits.maxBoundDescriptorSets = 8;
    props.properties.limits.subPixelPrecisionBits = 8;
    props.properties.limits.maxViewports = 4;
    props.properties.limits.viewportBoundsRange[0] = -16384;
    props.properties.limits.viewportBoundsRange[1] = 16384;
    props.properties.limits.framebufferColorSampleCounts |= VK_SAMPLE_COUNT_2_BIT;
    props.properties.limits.pointSizeRange[0] = 1.f;
    props.properties.limits.pointSizeRange[1] = 32.f;
    props.properties.limits.pointSizeGranularity = 0.125f;
    multiviewProps.maxMultiviewViewCount = 6;
    multiviewProps.maxMultiviewInstanceIndex = 65536;
    mock.SetProperties({
        VK_STRUCT(props),
        VK_STRUCT(multiviewProps)
    });

    uint32_t formatCount;
    vpGetProfileFormats(&profile, &formatCount, nullptr);
    std::vector<VkFormat> formats(formatCount);
    vpGetProfileFormats(&profile, &formatCount, formats.data());
    for (size_t i = 0; i < formatCount; ++i) {
        VkFormatProperties2KHR formatProps{ VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR };
        vpGetProfileFormatProperties(&profile, formats[i], &formatProps);
        formatProps.formatProperties.optimalTilingFeatures |= VK_FORMAT_FEATURE_BLIT_SRC_BIT;
        formatProps.formatProperties.bufferFeatures |= VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
        mock.AddFormat(formats[i], { VK_STRUCT(formatProps) });
    }

    VkQueueFamilyProperties2KHR queueFamilyProps{ VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2_KHR };
    queueFamilyProps.queueFamilyProperties.queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;
    queueFamilyProps.queueFamilyProperties.queueCount = 2;
    queueFamilyProps.queueFamilyProperties.timestampValidBits = 63;
    queueFamilyProps.queueFamilyProperties.minImageTransferGranularity = { 1, 1, 1 };
    mock.AddQueueFamily({ VK_STRUCT(queueFamilyProps) });

    VkBool32 supported = VK_TRUE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_FALSE);
}

TEST(mocked_api_get_physdev_profile_support, vulkan10_unsupported_feature) {
    MockVulkanAPI mock;

#if WITH_DEBUG_MESSAGES
    MockDebugMessageCallback cb({
        "Checking device support for profile VP_ANDROID_baseline_2021 "
            "(deviceName=, driverName=, driverInfo=). You may find the details "
            "of the capabilities of this device on https://vulkan.gpuinfo.org/",
        "Unsupported feature condition: VkPhysicalDeviceFeatures2KHR::features.sampleRateShading == VK_TRUE"
    });
#endif

    mock.SetInstanceAPIVersion(VK_API_VERSION_1_0);
    mock.SetDeviceAPIVersion(VK_API_VERSION_1_0);

    mock.SetDeviceExtensions(mock.vkPhysicalDevice, {
        VK_EXT(VK_KHR_SWAPCHAIN),
        VK_EXT(VK_KHR_MAINTENANCE_1),
        VK_EXT(VK_KHR_MAINTENANCE_2),
        VK_EXT(VK_KHR_INCREMENTAL_PRESENT),
        VK_EXT(VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE),
        VK_EXT(VK_KHR_GET_MEMORY_REQUIREMENTS_2),
        VK_EXT(VK_KHR_DEDICATED_ALLOCATION),
        VK_EXT(VK_KHR_STORAGE_BUFFER_STORAGE_CLASS),
        VK_EXT(VK_KHR_VARIABLE_POINTERS),
        VK_EXT(VK_KHR_EXTERNAL_SEMAPHORE),
        VK_EXT(VK_KHR_EXTERNAL_SEMAPHORE_FD),
        VK_EXT(VK_KHR_EXTERNAL_MEMORY),
        VK_EXT(VK_KHR_EXTERNAL_MEMORY_FD),
        VK_EXT(VK_KHR_EXTERNAL_FENCE),
        VK_EXT(VK_KHR_EXTERNAL_FENCE_FD),
        VK_EXT(VK_EXT_DESCRIPTOR_INDEXING),
        VK_EXT(VK_GOOGLE_DISPLAY_TIMING),
    });

    VpProfileProperties profile{ VP_ANDROID_BASELINE_2021_NAME, VP_ANDROID_BASELINE_2021_SPEC_VERSION };

    VkPhysicalDeviceMultiviewFeaturesKHR multiviewFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR };
    VkPhysicalDeviceFeatures2KHR features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR, &multiviewFeatures };
    vpGetProfileFeatures(&profile, &features);
    features.features.sampleRateShading = VK_FALSE; // Unsupported feature
    features.features.dualSrcBlend = VK_TRUE;
    features.features.drawIndirectFirstInstance = VK_TRUE;
    multiviewFeatures.multiview = VK_TRUE;
    mock.SetFeatures({
        VK_STRUCT(features),
        VK_STRUCT(multiviewFeatures)
    });

    VkPhysicalDeviceMultiviewPropertiesKHR multiviewProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR };
    VkPhysicalDeviceProperties2KHR props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR, &multiviewProps };
    vpGetProfileProperties(&profile, &props);
    props.properties.limits.maxImageDimension2D = 16384;
    props.properties.limits.maxBoundDescriptorSets = 8;
    props.properties.limits.subPixelPrecisionBits = 8;
    props.properties.limits.maxViewports = 4;
    props.properties.limits.viewportBoundsRange[0] = -16384;
    props.properties.limits.viewportBoundsRange[1] = 16384;
    props.properties.limits.framebufferColorSampleCounts |= VK_SAMPLE_COUNT_2_BIT;
    props.properties.limits.pointSizeRange[0] = 1.f;
    props.properties.limits.pointSizeRange[1] = 32.f;
    props.properties.limits.pointSizeGranularity = 0.125f;
    multiviewProps.maxMultiviewViewCount = 6;
    multiviewProps.maxMultiviewInstanceIndex = 65536;
    mock.SetProperties({
        VK_STRUCT(props),
        VK_STRUCT(multiviewProps)
    });

    uint32_t formatCount;
    vpGetProfileFormats(&profile, &formatCount, nullptr);
    std::vector<VkFormat> formats(formatCount);
    vpGetProfileFormats(&profile, &formatCount, formats.data());
    for (size_t i = 0; i < formatCount; ++i) {
        VkFormatProperties2KHR formatProps{ VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR };
        vpGetProfileFormatProperties(&profile, formats[i], &formatProps);
        formatProps.formatProperties.optimalTilingFeatures |= VK_FORMAT_FEATURE_BLIT_SRC_BIT;
        formatProps.formatProperties.bufferFeatures |= VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
        mock.AddFormat(formats[i], { VK_STRUCT(formatProps) });
    }

    VkQueueFamilyProperties2KHR queueFamilyProps{ VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2_KHR };
    queueFamilyProps.queueFamilyProperties.queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;
    queueFamilyProps.queueFamilyProperties.queueCount = 2;
    queueFamilyProps.queueFamilyProperties.timestampValidBits = 63;
    queueFamilyProps.queueFamilyProperties.minImageTransferGranularity = { 1, 1, 1 };
    mock.AddQueueFamily({ VK_STRUCT(queueFamilyProps) });

    VkBool32 supported = VK_TRUE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_FALSE);
}

TEST(mocked_api_get_physdev_profile_support, vulkan10_unsupported_property) {
    MockVulkanAPI mock;

#if WITH_DEBUG_MESSAGES
    MockDebugMessageCallback cb({
        "Checking device support for profile VP_ANDROID_baseline_2021 "
            "(deviceName=, driverName=, driverInfo=). You may find the details "
            "of the capabilities of this device on https://vulkan.gpuinfo.org/",
        "Unsupported properties condition: VkPhysicalDeviceProperties2KHR::properties.limits.maxImageDimension2D >= 4096"
    });
#endif

    mock.SetInstanceAPIVersion(VK_API_VERSION_1_0);
    mock.SetDeviceAPIVersion(VK_API_VERSION_1_0);

    mock.SetDeviceExtensions(mock.vkPhysicalDevice, {
        VK_EXT(VK_KHR_SWAPCHAIN),
        VK_EXT(VK_KHR_MAINTENANCE_1),
        VK_EXT(VK_KHR_MAINTENANCE_2),
        VK_EXT(VK_KHR_INCREMENTAL_PRESENT),
        VK_EXT(VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE),
        VK_EXT(VK_KHR_GET_MEMORY_REQUIREMENTS_2),
        VK_EXT(VK_KHR_DEDICATED_ALLOCATION),
        VK_EXT(VK_KHR_STORAGE_BUFFER_STORAGE_CLASS),
        VK_EXT(VK_KHR_VARIABLE_POINTERS),
        VK_EXT(VK_KHR_EXTERNAL_SEMAPHORE),
        VK_EXT(VK_KHR_EXTERNAL_SEMAPHORE_FD),
        VK_EXT(VK_KHR_EXTERNAL_MEMORY),
        VK_EXT(VK_KHR_EXTERNAL_MEMORY_FD),
        VK_EXT(VK_KHR_EXTERNAL_FENCE),
        VK_EXT(VK_KHR_EXTERNAL_FENCE_FD),
        VK_EXT(VK_EXT_DESCRIPTOR_INDEXING),
        VK_EXT(VK_GOOGLE_DISPLAY_TIMING),
    });

    VpProfileProperties profile{ VP_ANDROID_BASELINE_2021_NAME, VP_ANDROID_BASELINE_2021_SPEC_VERSION };

    VkPhysicalDeviceMultiviewFeaturesKHR multiviewFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR };
    VkPhysicalDeviceFeatures2KHR features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR, &multiviewFeatures };
    vpGetProfileFeatures(&profile, &features);
    features.features.dualSrcBlend = VK_TRUE;
    features.features.drawIndirectFirstInstance = VK_TRUE;
    multiviewFeatures.multiview = VK_TRUE;
    mock.SetFeatures({
        VK_STRUCT(features),
        VK_STRUCT(multiviewFeatures)
    });

    VkPhysicalDeviceMultiviewPropertiesKHR multiviewProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR };
    VkPhysicalDeviceProperties2KHR props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR, &multiviewProps };
    vpGetProfileProperties(&profile, &props);
    props.properties.limits.maxImageDimension2D = 2048; // Unsupported property
    props.properties.limits.maxBoundDescriptorSets = 8;
    props.properties.limits.subPixelPrecisionBits = 8;
    props.properties.limits.maxViewports = 4;
    props.properties.limits.viewportBoundsRange[0] = -16384;
    props.properties.limits.viewportBoundsRange[1] = 16384;
    props.properties.limits.framebufferColorSampleCounts |= VK_SAMPLE_COUNT_2_BIT;
    props.properties.limits.pointSizeRange[0] = 1.f;
    props.properties.limits.pointSizeRange[1] = 32.f;
    props.properties.limits.pointSizeGranularity = 0.125f;
    multiviewProps.maxMultiviewViewCount = 6;
    multiviewProps.maxMultiviewInstanceIndex = 65536;
    mock.SetProperties({
        VK_STRUCT(props),
        VK_STRUCT(multiviewProps)
    });

    uint32_t formatCount;
    vpGetProfileFormats(&profile, &formatCount, nullptr);
    std::vector<VkFormat> formats(formatCount);
    vpGetProfileFormats(&profile, &formatCount, formats.data());
    for (size_t i = 0; i < formatCount; ++i) {
        VkFormatProperties2KHR formatProps{ VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR };
        vpGetProfileFormatProperties(&profile, formats[i], &formatProps);
        formatProps.formatProperties.optimalTilingFeatures |= VK_FORMAT_FEATURE_BLIT_SRC_BIT;
        formatProps.formatProperties.bufferFeatures |= VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
        mock.AddFormat(formats[i], { VK_STRUCT(formatProps) });
    }

    VkQueueFamilyProperties2KHR queueFamilyProps{ VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2_KHR };
    queueFamilyProps.queueFamilyProperties.queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;
    queueFamilyProps.queueFamilyProperties.queueCount = 2;
    queueFamilyProps.queueFamilyProperties.timestampValidBits = 63;
    queueFamilyProps.queueFamilyProperties.minImageTransferGranularity = { 1, 1, 1 };
    mock.AddQueueFamily({ VK_STRUCT(queueFamilyProps) });

    VkBool32 supported = VK_TRUE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_FALSE);
}

TEST(mocked_api_get_physdev_profile_support, vulkan10_unsupported_format) {
    MockVulkanAPI mock;

#if WITH_DEBUG_MESSAGES
    MockDebugMessageCallback cb({
        "Checking device support for profile VP_ANDROID_baseline_2021 "
            "(deviceName=, driverName=, driverInfo=). You may find the details "
            "of the capabilities of this device on https://vulkan.gpuinfo.org/",
        "Unsupported format condition for VK_FORMAT_E5B9G9R9_UFLOAT_PACK32: "
            "VkFormatProperties2KHR::formatProperties.optimalTilingFeatures contains "
            "(VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | "
            "VK_FORMAT_FEATURE_BLIT_SRC_BIT | "
            "VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | "
            "VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | "
            "VK_FORMAT_FEATURE_TRANSFER_DST_BIT)"
    });
#endif

    mock.SetInstanceAPIVersion(VK_API_VERSION_1_0);
    mock.SetDeviceAPIVersion(VK_API_VERSION_1_0);

    mock.SetDeviceExtensions(mock.vkPhysicalDevice, {
        VK_EXT(VK_KHR_SWAPCHAIN),
        VK_EXT(VK_KHR_MAINTENANCE_1),
        VK_EXT(VK_KHR_MAINTENANCE_2),
        VK_EXT(VK_KHR_INCREMENTAL_PRESENT),
        VK_EXT(VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE),
        VK_EXT(VK_KHR_GET_MEMORY_REQUIREMENTS_2),
        VK_EXT(VK_KHR_DEDICATED_ALLOCATION),
        VK_EXT(VK_KHR_STORAGE_BUFFER_STORAGE_CLASS),
        VK_EXT(VK_KHR_VARIABLE_POINTERS),
        VK_EXT(VK_KHR_EXTERNAL_SEMAPHORE),
        VK_EXT(VK_KHR_EXTERNAL_SEMAPHORE_FD),
        VK_EXT(VK_KHR_EXTERNAL_MEMORY),
        VK_EXT(VK_KHR_EXTERNAL_MEMORY_FD),
        VK_EXT(VK_KHR_EXTERNAL_FENCE),
        VK_EXT(VK_KHR_EXTERNAL_FENCE_FD),
        VK_EXT(VK_EXT_DESCRIPTOR_INDEXING),
        VK_EXT(VK_GOOGLE_DISPLAY_TIMING),
    });

    VpProfileProperties profile{ VP_ANDROID_BASELINE_2021_NAME, VP_ANDROID_BASELINE_2021_SPEC_VERSION };

    VkPhysicalDeviceMultiviewFeaturesKHR multiviewFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR };
    VkPhysicalDeviceFeatures2KHR features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR, &multiviewFeatures };
    vpGetProfileFeatures(&profile, &features);
    mock.SetFeatures({
        VK_STRUCT(features),
        VK_STRUCT(multiviewFeatures)
    });

    VkPhysicalDeviceMultiviewPropertiesKHR multiviewProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR };
    VkPhysicalDeviceProperties2KHR props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR, &multiviewProps };
    vpGetProfileProperties(&profile, &props);
    mock.SetProperties({
        VK_STRUCT(props),
        VK_STRUCT(multiviewProps)
    });

    uint32_t formatCount;
    vpGetProfileFormats(&profile, &formatCount, nullptr);
    std::vector<VkFormat> formats(formatCount);
    vpGetProfileFormats(&profile, &formatCount, formats.data());
    for (size_t i = 0; i < formatCount; ++i) {
        VkFormatProperties2KHR formatProps{ VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR };
        vpGetProfileFormatProperties(&profile, formats[i], &formatProps);
        formatProps.formatProperties.optimalTilingFeatures |= VK_FORMAT_FEATURE_BLIT_SRC_BIT;
        formatProps.formatProperties.bufferFeatures |= VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
        if (formats[i] == VK_FORMAT_E5B9G9R9_UFLOAT_PACK32) {
            formatProps.formatProperties.optimalTilingFeatures = 0; // Unsupported format
        }
        mock.AddFormat(formats[i], { VK_STRUCT(formatProps) });
    }

    VkQueueFamilyProperties2KHR queueFamilyProps{ VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2_KHR };
    queueFamilyProps.queueFamilyProperties.queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;
    queueFamilyProps.queueFamilyProperties.queueCount = 2;
    queueFamilyProps.queueFamilyProperties.timestampValidBits = 63;
    queueFamilyProps.queueFamilyProperties.minImageTransferGranularity = { 1, 1, 1 };
    mock.AddQueueFamily({ VK_STRUCT(queueFamilyProps) });

    VkBool32 supported = VK_TRUE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_FALSE);
}

TEST(mocked_api_get_physdev_profile_support, vulkan11_unsupported_version) {
    MockVulkanAPI mock;

#if WITH_DEBUG_MESSAGES
    MockDebugMessageCallback cb(
        {"Checking device support for profile VP_KHR_roadmap_2022 "
         "(deviceName=, driverName=, driverInfo=). You may find the details "
         "of the capabilities of this device on https://vulkan.gpuinfo.org/",
         "Unsupported API version: 1.3.204"});
#endif

    // Unsupported version
    mock.SetInstanceAPIVersion(VK_API_VERSION_1_1);
    mock.SetDeviceAPIVersion(VK_API_VERSION_1_1);

    mock.SetDeviceExtensions(mock.vkPhysicalDevice, {
        VK_EXT(VK_KHR_GLOBAL_PRIORITY),
    });

    VpProfileProperties profile{VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION};

    VkPhysicalDeviceVulkan13Features vulkan13Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
    VkPhysicalDeviceVulkan12Features vulkan12Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, &vulkan13Features};
    VkPhysicalDeviceVulkan11Features vulkan11Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, &vulkan12Features};
    VkPhysicalDeviceFeatures2 features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &vulkan11Features};
    vpGetProfileFeatures(&profile, &features);

    mock.SetFeatures({VK_STRUCT(features), VK_STRUCT(vulkan11Features), VK_STRUCT(vulkan12Features), VK_STRUCT(vulkan13Features)});

    VkPhysicalDeviceVulkan13Properties vulkan13Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES};
    VkPhysicalDeviceVulkan12Properties vulkan12Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES, &vulkan13Properties};
    VkPhysicalDeviceVulkan11Properties vulkan11Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES, &vulkan12Properties};
    VkPhysicalDeviceProperties2 props{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, &vulkan11Properties};
    vpGetProfileProperties(&profile, &props);

    mock.SetProperties(
        {VK_STRUCT(props), VK_STRUCT(vulkan11Properties), VK_STRUCT(vulkan12Properties), VK_STRUCT(vulkan13Properties)});

    uint32_t formatCount;
    vpGetProfileFormats(&profile, &formatCount, nullptr);
    std::vector<VkFormat> formats(formatCount);
    vpGetProfileFormats(&profile, &formatCount, formats.data());
    for (size_t i = 0; i < formatCount; ++i) {
        VkFormatProperties2KHR formatProps{VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR};
        vpGetProfileFormatProperties(&profile, formats[i], &formatProps);
        mock.AddFormat(formats[i], {VK_STRUCT(formatProps)});
    }

    VkQueueFamilyProperties2KHR queueFamilyProps{VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2_KHR};
    queueFamilyProps.queueFamilyProperties.queueFlags =
        VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT;
    queueFamilyProps.queueFamilyProperties.queueCount = 1;
    queueFamilyProps.queueFamilyProperties.timestampValidBits = 63;
    queueFamilyProps.queueFamilyProperties.minImageTransferGranularity = {1, 1, 1};
    mock.AddQueueFamily({VK_STRUCT(queueFamilyProps)});

    queueFamilyProps.queueFamilyProperties.queueFlags = VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT;
    queueFamilyProps.queueFamilyProperties.queueCount = 4;
    queueFamilyProps.queueFamilyProperties.timestampValidBits = 63;
    queueFamilyProps.queueFamilyProperties.minImageTransferGranularity = {1, 1, 1};
    mock.AddQueueFamily({VK_STRUCT(queueFamilyProps)});

    queueFamilyProps.queueFamilyProperties.queueFlags = VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT;
    queueFamilyProps.queueFamilyProperties.queueCount = 2;
    queueFamilyProps.queueFamilyProperties.timestampValidBits = 63;
    queueFamilyProps.queueFamilyProperties.minImageTransferGranularity = {8, 8, 8};
    mock.AddQueueFamily({VK_STRUCT(queueFamilyProps)});

    VkBool32 supported = VK_TRUE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_FALSE);
}

TEST(mocked_api_get_physdev_profile_support, vulkan13_unsupported_extension) {
    MockVulkanAPI mock;

#if WITH_DEBUG_MESSAGES
    MockDebugMessageCallback cb({
        "Checking device support for profile VP_KHR_roadmap_2022 "
            "(deviceName=, driverName=, driverInfo=). You may find the details "
            "of the capabilities of this device on https://vulkan.gpuinfo.org/",
        "Unsupported extension: VK_KHR_global_priority"
    });
#endif

    mock.SetInstanceAPIVersion(VK_API_VERSION_1_3);
    mock.SetDeviceAPIVersion(VK_API_VERSION_1_3);

    mock.SetDeviceExtensions(mock.vkPhysicalDevice, {
        // Unsupported extension: VK_EXT(VK_KHR_GLOBAL_PRIORITY),
    });

    VpProfileProperties profile{VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION};

    VkPhysicalDeviceVulkan13Features vulkan13Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
    VkPhysicalDeviceVulkan12Features vulkan12Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, &vulkan13Features};
    VkPhysicalDeviceVulkan11Features vulkan11Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, &vulkan12Features};
    VkPhysicalDeviceFeatures2 features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &vulkan11Features};
    vpGetProfileFeatures(&profile, &features);

    mock.SetFeatures({VK_STRUCT(features), VK_STRUCT(vulkan11Features), VK_STRUCT(vulkan12Features), VK_STRUCT(vulkan13Features)});

    VkPhysicalDeviceVulkan13Properties vulkan13Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES};
    VkPhysicalDeviceVulkan12Properties vulkan12Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES, &vulkan13Properties};
    VkPhysicalDeviceVulkan11Properties vulkan11Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES, &vulkan12Properties};
    VkPhysicalDeviceProperties2 props{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, &vulkan11Properties};
    vpGetProfileProperties(&profile, &props);

    mock.SetProperties({VK_STRUCT(props), VK_STRUCT(vulkan11Properties), VK_STRUCT(vulkan12Properties), VK_STRUCT(vulkan13Properties)});

    VkBool32 supported = VK_TRUE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_FALSE);
}

TEST(mocked_api_get_physdev_profile_support, vulkan13_unsupported_feature) {
    MockVulkanAPI mock;

#if WITH_DEBUG_MESSAGES
    MockDebugMessageCallback cb({
        "Checking device support for profile VP_KHR_roadmap_2022 "
            "(deviceName=, driverName=, driverInfo=). You may find the details "
            "of the capabilities of this device on https://vulkan.gpuinfo.org/",
        "Unsupported feature condition: VkPhysicalDeviceFeatures2KHR::features.fullDrawIndexUint32 == VK_TRUE"
    });
#endif

    mock.SetInstanceAPIVersion(VK_API_VERSION_1_3);
    mock.SetDeviceAPIVersion(VK_API_VERSION_1_3);

    mock.SetDeviceExtensions(mock.vkPhysicalDevice, {
        VK_EXT(VK_KHR_GLOBAL_PRIORITY),
    });

    VpProfileProperties profile{VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION};

    VkPhysicalDeviceVulkan13Features vulkan13Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
    VkPhysicalDeviceVulkan12Features vulkan12Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, &vulkan13Features};
    VkPhysicalDeviceVulkan11Features vulkan11Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, &vulkan12Features};
    VkPhysicalDeviceFeatures2 features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &vulkan11Features};
    vpGetProfileFeatures(&profile, &features);

    features.features.fullDrawIndexUint32 = VK_FALSE;

    mock.SetFeatures({VK_STRUCT(features), VK_STRUCT(vulkan11Features), VK_STRUCT(vulkan12Features), VK_STRUCT(vulkan13Features)});

    VkPhysicalDeviceVulkan13Properties vulkan13Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES};
    VkPhysicalDeviceVulkan12Properties vulkan12Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES, &vulkan13Properties};
    VkPhysicalDeviceVulkan11Properties vulkan11Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES, &vulkan12Properties};
    VkPhysicalDeviceProperties2 props{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, &vulkan11Properties};
    vpGetProfileProperties(&profile, &props);

    mock.SetProperties({VK_STRUCT(props), VK_STRUCT(vulkan11Properties), VK_STRUCT(vulkan12Properties), VK_STRUCT(vulkan13Properties)});

    VkBool32 supported = VK_TRUE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_FALSE);
}

TEST(mocked_api_get_physdev_profile_support, vulkan13_unsupported_property) {
    MockVulkanAPI mock;

#if WITH_DEBUG_MESSAGES
    MockDebugMessageCallback cb(
        {"Checking device support for profile VP_KHR_roadmap_2022 "
         "(deviceName=, driverName=, driverInfo=). You may find the details "
         "of the capabilities of this device on https://vulkan.gpuinfo.org/",
         "Unsupported properties condition: VkPhysicalDeviceProperties2KHR::properties.limits.maxImageDimensionCube >= 8192"});
#endif

    mock.SetInstanceAPIVersion(VK_API_VERSION_1_3);
    mock.SetDeviceAPIVersion(VK_API_VERSION_1_3);

    mock.SetDeviceExtensions(mock.vkPhysicalDevice, {
        VK_EXT(VK_KHR_GLOBAL_PRIORITY),
    });

    VpProfileProperties profile{VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION};

    VkPhysicalDeviceVulkan13Features vulkan13Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
    VkPhysicalDeviceVulkan12Features vulkan12Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, &vulkan13Features};
    VkPhysicalDeviceVulkan11Features vulkan11Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, &vulkan12Features};
    VkPhysicalDeviceFeatures2 features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &vulkan11Features};
    vpGetProfileFeatures(&profile, &features);


    mock.SetFeatures({VK_STRUCT(features), VK_STRUCT(vulkan11Features), VK_STRUCT(vulkan12Features), VK_STRUCT(vulkan13Features)});

    VkPhysicalDeviceVulkan13Properties vulkan13Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES};
    VkPhysicalDeviceVulkan12Properties vulkan12Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES, &vulkan13Properties};
    VkPhysicalDeviceVulkan11Properties vulkan11Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES, &vulkan12Properties};
    VkPhysicalDeviceProperties2 props{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, &vulkan11Properties};
    vpGetProfileProperties(&profile, &props);

    props.properties.limits.maxImageDimensionCube = 2048;

    mock.SetProperties(
        {VK_STRUCT(props), VK_STRUCT(vulkan11Properties), VK_STRUCT(vulkan12Properties), VK_STRUCT(vulkan13Properties)});

    VkBool32 supported = VK_TRUE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_FALSE);
}

TEST(mocked_api_get_physdev_profile_support, vulkan11_unsupported_format) {
    MockVulkanAPI mock;

#if WITH_DEBUG_MESSAGES
    MockDebugMessageCallback cb({
        "Checking device support for profile VP_ANDROID_baseline_2021 "
            "(deviceName=, driverName=, driverInfo=). You may find the details "
            "of the capabilities of this device on https://vulkan.gpuinfo.org/",
        "Unsupported format condition for VK_FORMAT_E5B9G9R9_UFLOAT_PACK32: "
            "VkFormatProperties2KHR::formatProperties.optimalTilingFeatures contains "
            "(VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | "
            "VK_FORMAT_FEATURE_BLIT_SRC_BIT | "
            "VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | "
            "VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | "
            "VK_FORMAT_FEATURE_TRANSFER_DST_BIT)"
    });
#endif

    const VpProfileProperties profile{VP_ANDROID_BASELINE_2021_NAME, VP_ANDROID_BASELINE_2021_SPEC_VERSION};

    mock.SetInstanceAPIVersion(VK_API_VERSION_1_1);
    mock.SetDeviceAPIVersion(VK_API_VERSION_1_1);

    uint32_t extensionsCount = 0;
    vpGetProfileDeviceExtensionProperties(&profile, &extensionsCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionsCount);
    vpGetProfileDeviceExtensionProperties(&profile, &extensionsCount, &extensions[0]);
    mock.SetDeviceExtensions(mock.vkPhysicalDevice, extensions);

    VkPhysicalDeviceFeatures2 features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
    vpGetProfileFeatures(&profile, &features);
    mock.SetFeatures({
        VK_STRUCT(features),
    });

    VkPhysicalDeviceProperties2 props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
    vpGetProfileProperties(&profile, &props);
    mock.SetProperties({
        VK_STRUCT(props),
    });

    uint32_t formatCount;
    vpGetProfileFormats(&profile, &formatCount, nullptr);
    std::vector<VkFormat> formats(formatCount);
    vpGetProfileFormats(&profile, &formatCount, formats.data());
    for (size_t i = 0; i < formatCount; ++i) {
        VkFormatProperties2KHR formatProps{ VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR };
        vpGetProfileFormatProperties(&profile, formats[i], &formatProps);
        if (formats[i] == VK_FORMAT_E5B9G9R9_UFLOAT_PACK32) {
            formatProps.formatProperties.optimalTilingFeatures = 0; // Unsupported format
        }
        mock.AddFormat(formats[i], { VK_STRUCT(formatProps) });
    }

    VkBool32 supported = VK_TRUE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_FALSE);
}


TEST(mocked_api_get_physdev_profile_support, vulkan13_supported_queue_family) {
    MockVulkanAPI mock;

#if WITH_DEBUG_MESSAGES
    MockDebugMessageCallback cb(
        {"Checking device support for profile VP_KHR_roadmap_2022 "
         "(deviceName=, driverName=, driverInfo=). You may find the details "
         "of the capabilities of this device on https://vulkan.gpuinfo.org/"});
#endif

    // Unsupported version
    mock.SetInstanceAPIVersion(VK_API_VERSION_1_3);
    mock.SetDeviceAPIVersion(VK_API_VERSION_1_3);

    mock.SetDeviceExtensions(mock.vkPhysicalDevice, {
        VK_EXT(VK_KHR_GLOBAL_PRIORITY),
    });

    const VpProfileProperties profile{VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION};

    VkPhysicalDeviceVulkan13Features vulkan13Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
    VkPhysicalDeviceVulkan12Features vulkan12Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, &vulkan13Features};
    VkPhysicalDeviceVulkan11Features vulkan11Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, &vulkan12Features};
    VkPhysicalDeviceFeatures2 features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &vulkan11Features};
    vpGetProfileFeatures(&profile, &features);

    mock.SetFeatures({VK_STRUCT(features), VK_STRUCT(vulkan11Features), VK_STRUCT(vulkan12Features), VK_STRUCT(vulkan13Features)});

    VkPhysicalDeviceVulkan13Properties vulkan13Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES};
    VkPhysicalDeviceVulkan12Properties vulkan12Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES, &vulkan13Properties};
    VkPhysicalDeviceVulkan11Properties vulkan11Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES, &vulkan12Properties};
    VkPhysicalDeviceProperties2 props{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, &vulkan11Properties};
    vpGetProfileProperties(&profile, &props);

    mock.SetProperties(
        {VK_STRUCT(props), VK_STRUCT(vulkan11Properties), VK_STRUCT(vulkan12Properties), VK_STRUCT(vulkan13Properties)});

    uint32_t formatCount;
    vpGetProfileFormats(&profile, &formatCount, nullptr);
    std::vector<VkFormat> formats(formatCount);
    vpGetProfileFormats(&profile, &formatCount, formats.data());
    for (size_t i = 0; i < formatCount; ++i) {
        VkFormatProperties2KHR formatProps{VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR};
        vpGetProfileFormatProperties(&profile, formats[i], &formatProps);
        mock.AddFormat(formats[i], {VK_STRUCT(formatProps)});
    }

    VkQueueFamilyProperties2KHR queueFamilyProps{VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2_KHR};
    queueFamilyProps.queueFamilyProperties.queueFlags =
        VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT;
    queueFamilyProps.queueFamilyProperties.queueCount = 1;
    queueFamilyProps.queueFamilyProperties.timestampValidBits = 63;
    queueFamilyProps.queueFamilyProperties.minImageTransferGranularity = {1, 1, 1};
    mock.AddQueueFamily({VK_STRUCT(queueFamilyProps)});

    queueFamilyProps.queueFamilyProperties.queueFlags = VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT;
    queueFamilyProps.queueFamilyProperties.queueCount = 4;
    queueFamilyProps.queueFamilyProperties.timestampValidBits = 63;
    queueFamilyProps.queueFamilyProperties.minImageTransferGranularity = {1, 1, 1};
    mock.AddQueueFamily({VK_STRUCT(queueFamilyProps)});

    queueFamilyProps.queueFamilyProperties.queueFlags = VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT;
    queueFamilyProps.queueFamilyProperties.queueCount = 2;
    queueFamilyProps.queueFamilyProperties.timestampValidBits = 63;
    queueFamilyProps.queueFamilyProperties.minImageTransferGranularity = {8, 8, 8};
    mock.AddQueueFamily({VK_STRUCT(queueFamilyProps)});

    VkBool32 supported = VK_TRUE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_TRUE);
}

TEST(mocked_api_get_physdev_profile_support, vulkan13_supported_version) {
    MockVulkanAPI mock;

#if WITH_DEBUG_MESSAGES
    MockDebugMessageCallback cb(
        {"Checking device support for profile VP_KHR_roadmap_2022 "
         "(deviceName=, driverName=, driverInfo=). You may find the details "
         "of the capabilities of this device on https://vulkan.gpuinfo.org/"});
#endif

    // Unsupported version
    mock.SetInstanceAPIVersion(VK_API_VERSION_1_3);
    mock.SetDeviceAPIVersion(VK_API_VERSION_1_3);

    mock.SetDeviceExtensions(mock.vkPhysicalDevice, {
        VK_EXT(VK_KHR_GLOBAL_PRIORITY),
    });

    const VpProfileProperties profile{VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION};

    VkPhysicalDeviceVulkan13Features vulkan13Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
    VkPhysicalDeviceVulkan12Features vulkan12Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, &vulkan13Features};
    VkPhysicalDeviceVulkan11Features vulkan11Features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, &vulkan12Features};
    VkPhysicalDeviceFeatures2 features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &vulkan11Features};
    vpGetProfileFeatures(&profile, &features);

    mock.SetFeatures({VK_STRUCT(features), VK_STRUCT(vulkan11Features), VK_STRUCT(vulkan12Features), VK_STRUCT(vulkan13Features)});

    VkPhysicalDeviceVulkan13Properties vulkan13Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES};
    VkPhysicalDeviceVulkan12Properties vulkan12Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES, &vulkan13Properties};
    VkPhysicalDeviceVulkan11Properties vulkan11Properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES, &vulkan12Properties};
    VkPhysicalDeviceProperties2 props{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, &vulkan11Properties};
    vpGetProfileProperties(&profile, &props);

    mock.SetProperties(
        {VK_STRUCT(props), VK_STRUCT(vulkan11Properties), VK_STRUCT(vulkan12Properties), VK_STRUCT(vulkan13Properties)});

    uint32_t formatCount;
    vpGetProfileFormats(&profile, &formatCount, nullptr);
    std::vector<VkFormat> formats(formatCount);
    vpGetProfileFormats(&profile, &formatCount, formats.data());
    for (size_t i = 0; i < formatCount; ++i) {
        VkFormatProperties2KHR formatProps{VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR};
        vpGetProfileFormatProperties(&profile, formats[i], &formatProps);
        mock.AddFormat(formats[i], {VK_STRUCT(formatProps)});
    }

    VkBool32 supported = VK_TRUE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_TRUE);
}
