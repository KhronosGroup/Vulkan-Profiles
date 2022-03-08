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
        "Checking device support for profile VP_LUNARG_desktop_portability_2021 "
            "(deviceName=, driverName=, driverInfo=). You may find the details "
            "of the capabilities of this device on https://vulkan.gpuinfo.org/",
        "Unsupported API version: 1.1.142"
    });
#endif

    // Unsupported version
    mock.SetInstanceAPIVersion(VK_API_VERSION_1_0);
    mock.SetDeviceAPIVersion(VK_API_VERSION_1_0);

    mock.SetDeviceExtensions(mock.vkPhysicalDevice, {
        VK_EXT(VK_KHR_8BIT_STORAGE),
        VK_EXT(VK_KHR_CREATE_RENDERPASS_2),
        VK_EXT(VK_KHR_DEPTH_STENCIL_RESOLVE),
        VK_EXT(VK_KHR_DRIVER_PROPERTIES),
        VK_EXT(VK_KHR_IMAGE_FORMAT_LIST),
        VK_EXT(VK_KHR_IMAGELESS_FRAMEBUFFER),
        VK_EXT(VK_KHR_SAMPLER_MIRROR_CLAMP_TO_EDGE),
        VK_EXT(VK_KHR_SHADER_FLOAT16_INT8),
        VK_EXT(VK_KHR_SHADER_SUBGROUP_EXTENDED_TYPES),
        VK_EXT(VK_KHR_TIMELINE_SEMAPHORE),
        VK_EXT(VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT),
        VK_EXT(VK_EXT_DESCRIPTOR_INDEXING),
        VK_EXT(VK_EXT_HOST_QUERY_RESET),
        VK_EXT(VK_EXT_SCALAR_BLOCK_LAYOUT),
        VK_EXT(VK_EXT_ROBUSTNESS_2),
        VK_EXT(VK_EXT_SUBGROUP_SIZE_CONTROL),
        VK_EXT(VK_EXT_TEXEL_BUFFER_ALIGNMENT),
        VK_EXT(VK_EXT_VERTEX_ATTRIBUTE_DIVISOR),
        VK_EXT(VK_KHR_SWAPCHAIN),
        VK_EXT(VK_KHR_SWAPCHAIN_MUTABLE_FORMAT),
    });

    VpProfileProperties profile{ VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, VP_LUNARG_DESKTOP_PORTABILITY_2021_SPEC_VERSION };

    VkPhysicalDeviceVariablePointerFeatures vpFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES };
    VkPhysicalDeviceShaderFloat16Int8FeaturesKHR sf16i8Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR, &vpFeatures };
    VkPhysicalDeviceShaderDrawParametersFeatures sdpFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETER_FEATURES, &sf16i8Features };
    VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR ubslFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR, &sdpFeatures };
    VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR ssgetFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR, &ubslFeatures };
    VkPhysicalDeviceHostQueryResetFeaturesEXT hqResetFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT, &ssgetFeatures };
    VkPhysicalDeviceDescriptorIndexingFeaturesEXT descIndFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT, &hqResetFeatures };
    VkPhysicalDevice8BitStorageFeatures storage8Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES, &descIndFeatures };
    VkPhysicalDevice16BitStorageFeatures storage16Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES, &storage8Features };
    VkPhysicalDeviceImagelessFramebufferFeaturesKHR imagelessFbFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES, &storage16Features };
    VkPhysicalDeviceMultiviewFeatures multiviewFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES, &imagelessFbFeatures };
    VkPhysicalDeviceFeatures2 features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &multiviewFeatures };
    vpGetProfileFeatures(&profile, &features);
    features.features.logicOp = VK_TRUE;
    storage8Features.storagePushConstant8 = VK_TRUE;
    mock.SetFeatures({
        VK_STRUCT(features),
        VK_STRUCT(multiviewFeatures),
        VK_STRUCT(imagelessFbFeatures),
        VK_STRUCT(storage16Features),
        VK_STRUCT(storage8Features),
        VK_STRUCT(descIndFeatures),
        VK_STRUCT(hqResetFeatures),
        VK_STRUCT(ssgetFeatures),
        VK_STRUCT(ubslFeatures),
        VK_STRUCT(sdpFeatures),
        VK_STRUCT(sf16i8Features),
        VK_STRUCT(vpFeatures)
    });

    VkPhysicalDeviceMaintenance3Properties maint3Props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES };
    VkPhysicalDeviceDepthStencilResolvePropertiesKHR dsResProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR, &maint3Props };
    VkPhysicalDeviceDescriptorIndexingPropertiesEXT descIndProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT, &dsResProps };
    VkPhysicalDeviceMultiviewProperties multiviewProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES, &descIndProps };
    VkPhysicalDeviceProperties2 props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, &multiviewProps };
    vpGetProfileProperties(&profile, &props);
    props.properties.limits.maxImageDimension3D = 8192;
    props.properties.limits.maxBoundDescriptorSets = 16;
    props.properties.limits.subPixelPrecisionBits = 8;
    props.properties.limits.framebufferColorSampleCounts |= VK_SAMPLE_COUNT_16_BIT;
    props.properties.limits.pointSizeRange[0] = 1.f;
    props.properties.limits.pointSizeRange[1] = 128.f;
    props.properties.limits.pointSizeGranularity = 0.0625f;
    multiviewProps.maxMultiviewViewCount = 8;
    mock.SetProperties({
        VK_STRUCT(props),
        VK_STRUCT(multiviewProps),
        VK_STRUCT(descIndProps),
        VK_STRUCT(dsResProps),
        VK_STRUCT(maint3Props)
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

TEST(mocked_api_get_physdev_profile_support, vulkan11_supported) {
    MockVulkanAPI mock;

#if WITH_DEBUG_MESSAGES
    MockDebugMessageCallback cb({
        "Checking device support for profile VP_LUNARG_desktop_portability_2021 "
            "(deviceName=, driverName=, driverInfo=). You may find the details "
            "of the capabilities of this device on https://vulkan.gpuinfo.org/"
    });
#endif

    mock.SetInstanceAPIVersion(VK_API_VERSION_1_1);
    mock.SetDeviceAPIVersion(VK_API_VERSION_1_1);

    mock.SetDeviceExtensions(mock.vkPhysicalDevice, {
        VK_EXT(VK_KHR_8BIT_STORAGE),
        VK_EXT(VK_KHR_CREATE_RENDERPASS_2),
        VK_EXT(VK_KHR_DEPTH_STENCIL_RESOLVE),
        VK_EXT(VK_KHR_DRIVER_PROPERTIES),
        VK_EXT(VK_KHR_IMAGE_FORMAT_LIST),
        VK_EXT(VK_KHR_IMAGELESS_FRAMEBUFFER),
        VK_EXT(VK_KHR_SAMPLER_MIRROR_CLAMP_TO_EDGE),
        VK_EXT(VK_KHR_SHADER_FLOAT16_INT8),
        VK_EXT(VK_KHR_SHADER_SUBGROUP_EXTENDED_TYPES),
        VK_EXT(VK_KHR_TIMELINE_SEMAPHORE),
        VK_EXT(VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT),
        VK_EXT(VK_EXT_DESCRIPTOR_INDEXING),
        VK_EXT(VK_EXT_HOST_QUERY_RESET),
        VK_EXT(VK_EXT_INLINE_UNIFORM_BLOCK),
        VK_EXT(VK_EXT_SCALAR_BLOCK_LAYOUT),
        VK_EXT(VK_EXT_ROBUSTNESS_2),
        VK_EXT(VK_EXT_SUBGROUP_SIZE_CONTROL),
        VK_EXT(VK_EXT_TEXEL_BUFFER_ALIGNMENT),
        VK_EXT(VK_EXT_VERTEX_ATTRIBUTE_DIVISOR),
        VK_EXT(VK_KHR_SWAPCHAIN),
        VK_EXT(VK_KHR_SWAPCHAIN_MUTABLE_FORMAT),
    });

    VpProfileProperties profile{ VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, VP_LUNARG_DESKTOP_PORTABILITY_2021_SPEC_VERSION };

    VkPhysicalDeviceVariablePointerFeatures vpFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES };
    VkPhysicalDeviceShaderFloat16Int8FeaturesKHR sf16i8Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR, &vpFeatures };
    VkPhysicalDeviceShaderDrawParametersFeatures sdpFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETER_FEATURES, &sf16i8Features };
    VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR ubslFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR, &sdpFeatures };
    VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR ssgetFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR, &ubslFeatures };
    VkPhysicalDeviceHostQueryResetFeaturesEXT hqResetFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT, &ssgetFeatures };
    VkPhysicalDeviceDescriptorIndexingFeaturesEXT descIndFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT, &hqResetFeatures };
    VkPhysicalDevice8BitStorageFeatures storage8Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES, &descIndFeatures };
    VkPhysicalDevice16BitStorageFeatures storage16Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES, &storage8Features };
    VkPhysicalDeviceImagelessFramebufferFeaturesKHR imagelessFbFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES, &storage16Features };
    VkPhysicalDeviceMultiviewFeatures multiviewFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES, &imagelessFbFeatures };
    VkPhysicalDeviceFeatures2 features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &multiviewFeatures };
    vpGetProfileFeatures(&profile, &features);
    features.features.logicOp = VK_TRUE;
    storage8Features.storagePushConstant8 = VK_TRUE;
    mock.SetFeatures({
        VK_STRUCT(features),
        VK_STRUCT(multiviewFeatures),
        VK_STRUCT(imagelessFbFeatures),
        VK_STRUCT(storage16Features),
        VK_STRUCT(storage8Features),
        VK_STRUCT(descIndFeatures),
        VK_STRUCT(hqResetFeatures),
        VK_STRUCT(ssgetFeatures),
        VK_STRUCT(ubslFeatures),
        VK_STRUCT(sdpFeatures),
        VK_STRUCT(sf16i8Features),
        VK_STRUCT(vpFeatures)
    });

    VkPhysicalDeviceMaintenance3Properties maint3Props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES };
    VkPhysicalDeviceDepthStencilResolvePropertiesKHR dsResProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR, &maint3Props };
    VkPhysicalDeviceDescriptorIndexingPropertiesEXT descIndProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT, &dsResProps };
    VkPhysicalDeviceMultiviewProperties multiviewProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES, &descIndProps };
    VkPhysicalDeviceProperties2 props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, &multiviewProps };
    vpGetProfileProperties(&profile, &props);
    props.properties.limits.maxImageDimension3D = 8192;
    props.properties.limits.maxBoundDescriptorSets = 16;
    props.properties.limits.subPixelPrecisionBits = 8;
    props.properties.limits.framebufferColorSampleCounts |= VK_SAMPLE_COUNT_16_BIT;
    props.properties.limits.pointSizeRange[0] = 1.f;
    props.properties.limits.pointSizeRange[1] = 128.f;
    props.properties.limits.pointSizeGranularity = 0.0625f;
    multiviewProps.maxMultiviewViewCount = 8;
    mock.SetProperties({
        VK_STRUCT(props),
        VK_STRUCT(multiviewProps),
        VK_STRUCT(descIndProps),
        VK_STRUCT(dsResProps),
        VK_STRUCT(maint3Props)
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

    VkBool32 supported = VK_FALSE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_TRUE);
}

TEST(mocked_api_get_physdev_profile_support, vulkan11_unsupported_version) {
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
    mock.SetInstanceAPIVersion(VK_API_VERSION_1_1);
    mock.SetDeviceAPIVersion(VK_API_VERSION_1_1);

    mock.SetDeviceExtensions(mock.vkPhysicalDevice, {
        VK_EXT(VK_KHR_GLOBAL_PRIORITY),
        VK_EXT(VK_EXT_HOST_QUERY_RESET),
        VK_EXT(VK_KHR_SWAPCHAIN),
    });

    VpProfileProperties profile{ VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION };

    VkPhysicalDeviceVulkan13Features features13{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
    VkPhysicalDeviceVulkan12Features features12{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, &features13 };
    VkPhysicalDeviceVulkan11Features features11{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, &features12 };
    VkPhysicalDeviceFeatures2 features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &features11 };
    vpGetProfileFeatures(&profile, &features);
    features.features.logicOp = VK_TRUE;
    features12.shaderInputAttachmentArrayNonUniformIndexing = VK_TRUE;
    mock.SetFeatures({
        VK_STRUCT(features),
        VK_STRUCT(features11),
        VK_STRUCT(features12),
        VK_STRUCT(features13),
    });

    VkPhysicalDeviceVulkan13Properties props13{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES };
    VkPhysicalDeviceVulkan12Properties props12{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES, &props13 };
    VkPhysicalDeviceVulkan11Properties props11{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES, &props12 };
    VkPhysicalDeviceProperties2 props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, &props11 };
    vpGetProfileProperties(&profile, &props);
    props.properties.limits.maxImageDimension3D = 16384;
    props.properties.limits.maxBoundDescriptorSets = 16;
    props.properties.limits.subPixelPrecisionBits = 8;
    props.properties.limits.framebufferColorSampleCounts |= VK_SAMPLE_COUNT_16_BIT;
    props.properties.limits.pointSizeRange[0] = 1.f;
    props.properties.limits.pointSizeRange[1] = 128.f;
    props.properties.limits.pointSizeGranularity = 0.0625f;
    props12.maxDescriptorSetUpdateAfterBindUniformBuffers = 96;
    mock.SetProperties({
        VK_STRUCT(props),
        VK_STRUCT(props11),
        VK_STRUCT(props12),
        VK_STRUCT(props13),
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

    VkBool32 supported = VK_TRUE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(mock.vkInstance, mock.vkPhysicalDevice, &profile, &supported);

    EXPECT_EQ(result, VK_SUCCESS);
    EXPECT_EQ(supported, VK_FALSE);
}

TEST(mocked_api_get_physdev_profile_support, vulkan11_unsupported_extension) {
    MockVulkanAPI mock;

#if WITH_DEBUG_MESSAGES
    MockDebugMessageCallback cb({
        "Checking device support for profile VP_LUNARG_desktop_portability_2021 "
            "(deviceName=, driverName=, driverInfo=). You may find the details "
            "of the capabilities of this device on https://vulkan.gpuinfo.org/",
        "Unsupported extension: VK_KHR_driver_properties"
    });
#endif

    mock.SetInstanceAPIVersion(VK_API_VERSION_1_1);
    mock.SetDeviceAPIVersion(VK_API_VERSION_1_1);

    mock.SetDeviceExtensions(mock.vkPhysicalDevice, {
        VK_EXT(VK_KHR_8BIT_STORAGE),
        VK_EXT(VK_KHR_CREATE_RENDERPASS_2),
        VK_EXT(VK_KHR_DEPTH_STENCIL_RESOLVE),
        // Unsupported extension: VK_EXT(VK_KHR_DRIVER_PROPERTIES),
        VK_EXT(VK_KHR_IMAGE_FORMAT_LIST),
        VK_EXT(VK_KHR_IMAGELESS_FRAMEBUFFER),
        VK_EXT(VK_KHR_SAMPLER_MIRROR_CLAMP_TO_EDGE),
        VK_EXT(VK_KHR_SHADER_FLOAT16_INT8),
        VK_EXT(VK_KHR_TIMELINE_SEMAPHORE),
        VK_EXT(VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT),
        VK_EXT(VK_EXT_DESCRIPTOR_INDEXING),
        VK_EXT(VK_EXT_HOST_QUERY_RESET),
        VK_EXT(VK_EXT_INLINE_UNIFORM_BLOCK),
        VK_EXT(VK_EXT_SCALAR_BLOCK_LAYOUT),
        VK_EXT(VK_EXT_ROBUSTNESS_2),
        VK_EXT(VK_EXT_SUBGROUP_SIZE_CONTROL),
        VK_EXT(VK_EXT_TEXEL_BUFFER_ALIGNMENT),
        VK_EXT(VK_EXT_VERTEX_ATTRIBUTE_DIVISOR),
        VK_EXT(VK_KHR_SWAPCHAIN),
        VK_EXT(VK_KHR_SWAPCHAIN_MUTABLE_FORMAT),
    });

    VpProfileProperties profile{ VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, VP_LUNARG_DESKTOP_PORTABILITY_2021_SPEC_VERSION };

    VkPhysicalDeviceVariablePointerFeatures vpFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES };
    VkPhysicalDeviceShaderFloat16Int8FeaturesKHR sf16i8Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR, &vpFeatures };
    VkPhysicalDeviceShaderDrawParametersFeatures sdpFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETER_FEATURES, &sf16i8Features };
    VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR ubslFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR, &sdpFeatures };
    VkPhysicalDeviceHostQueryResetFeaturesEXT hqResetFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT, &ubslFeatures};
    VkPhysicalDeviceDescriptorIndexingFeaturesEXT descIndFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT, &hqResetFeatures };
    VkPhysicalDevice8BitStorageFeatures storage8Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES, &descIndFeatures };
    VkPhysicalDevice16BitStorageFeatures storage16Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES, &storage8Features };
    VkPhysicalDeviceImagelessFramebufferFeaturesKHR imagelessFbFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES, &storage16Features };
    VkPhysicalDeviceMultiviewFeatures multiviewFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES, &imagelessFbFeatures };
    VkPhysicalDeviceFeatures2 features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &multiviewFeatures };
    vpGetProfileFeatures(&profile, &features);
    features.features.logicOp = VK_TRUE;
    storage8Features.storagePushConstant8 = VK_TRUE;
    mock.SetFeatures({
        VK_STRUCT(features),
        VK_STRUCT(multiviewFeatures),
        VK_STRUCT(imagelessFbFeatures),
        VK_STRUCT(storage16Features),
        VK_STRUCT(storage8Features),
        VK_STRUCT(descIndFeatures),
        VK_STRUCT(hqResetFeatures),
        VK_STRUCT(ubslFeatures),
        VK_STRUCT(sdpFeatures),
        VK_STRUCT(sf16i8Features),
        VK_STRUCT(vpFeatures)
    });

    VkPhysicalDeviceMaintenance3Properties maint3Props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES };
    VkPhysicalDeviceDepthStencilResolvePropertiesKHR dsResProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR, &maint3Props };
    VkPhysicalDeviceDescriptorIndexingPropertiesEXT descIndProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT, &dsResProps };
    VkPhysicalDeviceMultiviewProperties multiviewProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES, &descIndProps };
    VkPhysicalDeviceProperties2 props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, &multiviewProps };
    vpGetProfileProperties(&profile, &props);
    props.properties.limits.maxImageDimension3D = 8192;
    props.properties.limits.maxBoundDescriptorSets = 16;
    props.properties.limits.subPixelPrecisionBits = 8;
    props.properties.limits.framebufferColorSampleCounts |= VK_SAMPLE_COUNT_16_BIT;
    props.properties.limits.pointSizeRange[0] = 1.f;
    props.properties.limits.pointSizeRange[1] = 128.f;
    props.properties.limits.pointSizeGranularity = 0.0625f;
    multiviewProps.maxMultiviewViewCount = 8;
    mock.SetProperties({
        VK_STRUCT(props),
        VK_STRUCT(multiviewProps),
        VK_STRUCT(descIndProps),
        VK_STRUCT(dsResProps),
        VK_STRUCT(maint3Props)
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

TEST(mocked_api_get_physdev_profile_support, vulkan11_unsupported_feature) {
    MockVulkanAPI mock;

#if WITH_DEBUG_MESSAGES
    MockDebugMessageCallback cb({
        "Checking device support for profile VP_LUNARG_desktop_portability_2021 "
            "(deviceName=, driverName=, driverInfo=). You may find the details "
            "of the capabilities of this device on https://vulkan.gpuinfo.org/",
        "Unsupported feature condition: VkPhysicalDeviceDescriptorIndexingFeaturesEXT::shaderUniformTexelBufferArrayDynamicIndexing == VK_TRUE"
    });
#endif

    mock.SetInstanceAPIVersion(VK_API_VERSION_1_1);
    mock.SetDeviceAPIVersion(VK_API_VERSION_1_1);

    mock.SetDeviceExtensions(mock.vkPhysicalDevice, {
        VK_EXT(VK_KHR_8BIT_STORAGE),
        VK_EXT(VK_KHR_CREATE_RENDERPASS_2),
        VK_EXT(VK_KHR_DEPTH_STENCIL_RESOLVE),
        VK_EXT(VK_KHR_DRIVER_PROPERTIES),
        VK_EXT(VK_KHR_IMAGE_FORMAT_LIST),
        VK_EXT(VK_KHR_IMAGELESS_FRAMEBUFFER),
        VK_EXT(VK_KHR_SAMPLER_MIRROR_CLAMP_TO_EDGE),
        VK_EXT(VK_KHR_SHADER_FLOAT16_INT8),
        VK_EXT(VK_KHR_SHADER_SUBGROUP_EXTENDED_TYPES),
        VK_EXT(VK_KHR_TIMELINE_SEMAPHORE),
        VK_EXT(VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT),
        VK_EXT(VK_EXT_DESCRIPTOR_INDEXING),
        VK_EXT(VK_EXT_HOST_QUERY_RESET),
        VK_EXT(VK_EXT_INLINE_UNIFORM_BLOCK),
        VK_EXT(VK_EXT_SCALAR_BLOCK_LAYOUT),
        VK_EXT(VK_EXT_ROBUSTNESS_2),
        VK_EXT(VK_EXT_SUBGROUP_SIZE_CONTROL),
        VK_EXT(VK_EXT_TEXEL_BUFFER_ALIGNMENT),
        VK_EXT(VK_EXT_VERTEX_ATTRIBUTE_DIVISOR),
        VK_EXT(VK_KHR_SWAPCHAIN),
        VK_EXT(VK_KHR_SWAPCHAIN_MUTABLE_FORMAT),
    });

    VpProfileProperties profile{ VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, VP_LUNARG_DESKTOP_PORTABILITY_2021_SPEC_VERSION };

    VkPhysicalDeviceVariablePointerFeatures vpFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES };
    VkPhysicalDeviceShaderFloat16Int8FeaturesKHR sf16i8Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR, &vpFeatures };
    VkPhysicalDeviceShaderDrawParametersFeatures sdpFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETER_FEATURES, &sf16i8Features };
    VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR ubslFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR, &sdpFeatures };
    VkPhysicalDeviceHostQueryResetFeaturesEXT hqResetFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT, &ubslFeatures};
    VkPhysicalDeviceDescriptorIndexingFeaturesEXT descIndFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT, &hqResetFeatures };
    VkPhysicalDevice8BitStorageFeatures storage8Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES, &descIndFeatures };
    VkPhysicalDevice16BitStorageFeatures storage16Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES, &storage8Features };
    VkPhysicalDeviceImagelessFramebufferFeaturesKHR imagelessFbFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES, &storage16Features };
    VkPhysicalDeviceMultiviewFeatures multiviewFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES, &imagelessFbFeatures };
    VkPhysicalDeviceFeatures2 features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &multiviewFeatures };
    vpGetProfileFeatures(&profile, &features);
    features.features.logicOp = VK_TRUE;
    storage8Features.storagePushConstant8 = VK_TRUE;
    descIndFeatures.shaderUniformTexelBufferArrayDynamicIndexing = VK_FALSE; // Unsupported feature
    mock.SetFeatures({
        VK_STRUCT(features),
        VK_STRUCT(multiviewFeatures),
        VK_STRUCT(imagelessFbFeatures),
        VK_STRUCT(storage16Features),
        VK_STRUCT(storage8Features),
        VK_STRUCT(descIndFeatures),
        VK_STRUCT(hqResetFeatures),
        VK_STRUCT(ubslFeatures),
        VK_STRUCT(sdpFeatures),
        VK_STRUCT(sf16i8Features),
        VK_STRUCT(vpFeatures)
    });

    VkPhysicalDeviceMaintenance3Properties maint3Props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES };
    VkPhysicalDeviceDepthStencilResolvePropertiesKHR dsResProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR, &maint3Props };
    VkPhysicalDeviceDescriptorIndexingPropertiesEXT descIndProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT, &dsResProps };
    VkPhysicalDeviceMultiviewProperties multiviewProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES, &descIndProps };
    VkPhysicalDeviceProperties2 props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, &multiviewProps };
    vpGetProfileProperties(&profile, &props);
    props.properties.limits.maxImageDimension3D = 8192;
    props.properties.limits.maxBoundDescriptorSets = 16;
    props.properties.limits.subPixelPrecisionBits = 8;
    props.properties.limits.framebufferColorSampleCounts |= VK_SAMPLE_COUNT_16_BIT;
    props.properties.limits.pointSizeRange[0] = 1.f;
    props.properties.limits.pointSizeRange[1] = 128.f;
    props.properties.limits.pointSizeGranularity = 0.0625f;
    multiviewProps.maxMultiviewViewCount = 8;
    mock.SetProperties({
        VK_STRUCT(props),
        VK_STRUCT(multiviewProps),
        VK_STRUCT(descIndProps),
        VK_STRUCT(dsResProps),
        VK_STRUCT(maint3Props)
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

TEST(mocked_api_get_physdev_profile_support, vulkan11_unsupported_property) {
    MockVulkanAPI mock;

#if WITH_DEBUG_MESSAGES
    MockDebugMessageCallback cb({
        "Checking device support for profile VP_LUNARG_desktop_portability_2021 "
            "(deviceName=, driverName=, driverInfo=). You may find the details "
            "of the capabilities of this device on https://vulkan.gpuinfo.org/",
        "Unsupported properties condition: VkPhysicalDeviceDescriptorIndexingPropertiesEXT::maxDescriptorSetUpdateAfterBindInputAttachments >= 8"
    });
#endif

    mock.SetInstanceAPIVersion(VK_API_VERSION_1_1);
    mock.SetDeviceAPIVersion(VK_API_VERSION_1_1);

    mock.SetDeviceExtensions(mock.vkPhysicalDevice, {
        VK_EXT(VK_KHR_8BIT_STORAGE),
        VK_EXT(VK_KHR_CREATE_RENDERPASS_2),
        VK_EXT(VK_KHR_DEPTH_STENCIL_RESOLVE),
        VK_EXT(VK_KHR_DRIVER_PROPERTIES),
        VK_EXT(VK_KHR_IMAGE_FORMAT_LIST),
        VK_EXT(VK_KHR_IMAGELESS_FRAMEBUFFER),
        VK_EXT(VK_KHR_SAMPLER_MIRROR_CLAMP_TO_EDGE),
        VK_EXT(VK_KHR_SHADER_FLOAT16_INT8),
        VK_EXT(VK_KHR_TIMELINE_SEMAPHORE),
        VK_EXT(VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT),
        VK_EXT(VK_EXT_DESCRIPTOR_INDEXING),
        VK_EXT(VK_EXT_HOST_QUERY_RESET),
        VK_EXT(VK_EXT_INLINE_UNIFORM_BLOCK),
        VK_EXT(VK_EXT_SCALAR_BLOCK_LAYOUT),
        VK_EXT(VK_EXT_ROBUSTNESS_2),
        VK_EXT(VK_EXT_SUBGROUP_SIZE_CONTROL),
        VK_EXT(VK_EXT_TEXEL_BUFFER_ALIGNMENT),
        VK_EXT(VK_EXT_VERTEX_ATTRIBUTE_DIVISOR),
        VK_EXT(VK_KHR_SWAPCHAIN),
        VK_EXT(VK_KHR_SWAPCHAIN_MUTABLE_FORMAT),
    });

    VpProfileProperties profile{ VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, VP_LUNARG_DESKTOP_PORTABILITY_2021_SPEC_VERSION };

    VkPhysicalDeviceVariablePointerFeatures vpFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES };
    VkPhysicalDeviceShaderFloat16Int8FeaturesKHR sf16i8Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR, &vpFeatures };
    VkPhysicalDeviceShaderDrawParametersFeatures sdpFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETER_FEATURES, &sf16i8Features };
    VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR ubslFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR, &sdpFeatures };
    VkPhysicalDeviceHostQueryResetFeaturesEXT hqResetFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT, &ubslFeatures};
    VkPhysicalDeviceDescriptorIndexingFeaturesEXT descIndFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT, &hqResetFeatures };
    VkPhysicalDevice8BitStorageFeatures storage8Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES, &descIndFeatures };
    VkPhysicalDevice16BitStorageFeatures storage16Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES, &storage8Features };
    VkPhysicalDeviceImagelessFramebufferFeaturesKHR imagelessFbFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES, &storage16Features };
    VkPhysicalDeviceMultiviewFeatures multiviewFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES, &imagelessFbFeatures };
    VkPhysicalDeviceFeatures2 features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &multiviewFeatures };
    vpGetProfileFeatures(&profile, &features);
    features.features.logicOp = VK_TRUE;
    storage8Features.storagePushConstant8 = VK_TRUE;
    mock.SetFeatures({
        VK_STRUCT(features),
        VK_STRUCT(multiviewFeatures),
        VK_STRUCT(imagelessFbFeatures),
        VK_STRUCT(storage16Features),
        VK_STRUCT(storage8Features),
        VK_STRUCT(descIndFeatures),
        VK_STRUCT(hqResetFeatures),
        VK_STRUCT(ubslFeatures),
        VK_STRUCT(sdpFeatures),
        VK_STRUCT(sf16i8Features),
        VK_STRUCT(vpFeatures)
    });

    VkPhysicalDeviceMaintenance3Properties maint3Props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES };
    VkPhysicalDeviceDepthStencilResolvePropertiesKHR dsResProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR, &maint3Props };
    VkPhysicalDeviceDescriptorIndexingPropertiesEXT descIndProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT, &dsResProps };
    VkPhysicalDeviceMultiviewProperties multiviewProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES, &descIndProps };
    VkPhysicalDeviceProperties2 props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, &multiviewProps };
    vpGetProfileProperties(&profile, &props);
    props.properties.limits.maxImageDimension3D = 8192;
    props.properties.limits.maxBoundDescriptorSets = 16;
    props.properties.limits.subPixelPrecisionBits = 8;
    props.properties.limits.framebufferColorSampleCounts |= VK_SAMPLE_COUNT_16_BIT;
    props.properties.limits.pointSizeRange[0] = 1.f;
    props.properties.limits.pointSizeRange[1] = 128.f;
    props.properties.limits.pointSizeGranularity = 0.0625f;
    multiviewProps.maxMultiviewViewCount = 8;
    descIndProps.maxDescriptorSetUpdateAfterBindInputAttachments = 4; // Unsupported property
    mock.SetProperties({
        VK_STRUCT(props),
        VK_STRUCT(multiviewProps),
        VK_STRUCT(descIndProps),
        VK_STRUCT(dsResProps),
        VK_STRUCT(maint3Props)
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

TEST(mocked_api_get_physdev_profile_support, vulkan11_unsupported_format) {
    MockVulkanAPI mock;

#if WITH_DEBUG_MESSAGES
    MockDebugMessageCallback cb({
        "Checking device support for profile VP_LUNARG_desktop_portability_2021 "
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

    mock.SetInstanceAPIVersion(VK_API_VERSION_1_1);
    mock.SetDeviceAPIVersion(VK_API_VERSION_1_1);

    mock.SetDeviceExtensions(mock.vkPhysicalDevice, {
        VK_EXT(VK_KHR_8BIT_STORAGE),
        VK_EXT(VK_KHR_CREATE_RENDERPASS_2),
        VK_EXT(VK_KHR_DEPTH_STENCIL_RESOLVE),
        VK_EXT(VK_KHR_DRIVER_PROPERTIES),
        VK_EXT(VK_KHR_IMAGE_FORMAT_LIST),
        VK_EXT(VK_KHR_IMAGELESS_FRAMEBUFFER),
        VK_EXT(VK_KHR_SAMPLER_MIRROR_CLAMP_TO_EDGE),
        VK_EXT(VK_KHR_SHADER_FLOAT16_INT8),
        VK_EXT(VK_KHR_TIMELINE_SEMAPHORE),
        VK_EXT(VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT),
        VK_EXT(VK_EXT_DESCRIPTOR_INDEXING),
        VK_EXT(VK_EXT_HOST_QUERY_RESET),
        VK_EXT(VK_EXT_INLINE_UNIFORM_BLOCK),
        VK_EXT(VK_EXT_SCALAR_BLOCK_LAYOUT),
        VK_EXT(VK_EXT_ROBUSTNESS_2),
        VK_EXT(VK_EXT_SUBGROUP_SIZE_CONTROL),
        VK_EXT(VK_EXT_TEXEL_BUFFER_ALIGNMENT),
        VK_EXT(VK_EXT_VERTEX_ATTRIBUTE_DIVISOR),
        VK_EXT(VK_KHR_SWAPCHAIN),
        VK_EXT(VK_KHR_SWAPCHAIN_MUTABLE_FORMAT),
    });

    VpProfileProperties profile{ VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, VP_LUNARG_DESKTOP_PORTABILITY_2021_SPEC_VERSION };

    VkPhysicalDeviceVariablePointerFeatures vpFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES };
    VkPhysicalDeviceShaderFloat16Int8FeaturesKHR sf16i8Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR, &vpFeatures };
    VkPhysicalDeviceShaderDrawParametersFeatures sdpFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETER_FEATURES, &sf16i8Features };
    VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR ubslFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR, &sdpFeatures };
    VkPhysicalDeviceHostQueryResetFeaturesEXT hqResetFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT, &ubslFeatures};
    VkPhysicalDeviceDescriptorIndexingFeaturesEXT descIndFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT, &hqResetFeatures };
    VkPhysicalDevice8BitStorageFeatures storage8Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES, &descIndFeatures };
    VkPhysicalDevice16BitStorageFeatures storage16Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES, &storage8Features };
    VkPhysicalDeviceImagelessFramebufferFeaturesKHR imagelessFbFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES, &storage16Features };
    VkPhysicalDeviceMultiviewFeatures multiviewFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES, &imagelessFbFeatures };
    VkPhysicalDeviceFeatures2 features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &multiviewFeatures };
    vpGetProfileFeatures(&profile, &features);
    features.features.logicOp = VK_TRUE;
    storage8Features.storagePushConstant8 = VK_TRUE;
    mock.SetFeatures({
        VK_STRUCT(features),
        VK_STRUCT(multiviewFeatures),
        VK_STRUCT(imagelessFbFeatures),
        VK_STRUCT(storage16Features),
        VK_STRUCT(storage8Features),
        VK_STRUCT(descIndFeatures),
        VK_STRUCT(hqResetFeatures),
        VK_STRUCT(ubslFeatures),
        VK_STRUCT(sdpFeatures),
        VK_STRUCT(sf16i8Features),
        VK_STRUCT(vpFeatures)
    });

    VkPhysicalDeviceMaintenance3Properties maint3Props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES };
    VkPhysicalDeviceDepthStencilResolvePropertiesKHR dsResProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR, &maint3Props };
    VkPhysicalDeviceDescriptorIndexingPropertiesEXT descIndProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT, &dsResProps };
    VkPhysicalDeviceMultiviewProperties multiviewProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES, &descIndProps };
    VkPhysicalDeviceProperties2 props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, &multiviewProps };
    vpGetProfileProperties(&profile, &props);
    props.properties.limits.maxImageDimension3D = 8192;
    props.properties.limits.maxBoundDescriptorSets = 16;
    props.properties.limits.subPixelPrecisionBits = 8;
    props.properties.limits.framebufferColorSampleCounts |= VK_SAMPLE_COUNT_16_BIT;
    props.properties.limits.pointSizeRange[0] = 1.f;
    props.properties.limits.pointSizeRange[1] = 128.f;
    props.properties.limits.pointSizeGranularity = 0.0625f;
    multiviewProps.maxMultiviewViewCount = 8;
    mock.SetProperties({
        VK_STRUCT(props),
        VK_STRUCT(multiviewProps),
        VK_STRUCT(descIndProps),
        VK_STRUCT(dsResProps),
        VK_STRUCT(maint3Props)
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

TEST(mocked_api_get_physdev_profile_support, vulkan11_unsupported_queue_family) {
    MockVulkanAPI mock;

#if WITH_DEBUG_MESSAGES
    MockDebugMessageCallback cb({
        "Checking device support for profile VP_LUNARG_desktop_portability_2021 "
            "(deviceName=, driverName=, driverInfo=). You may find the details "
            "of the capabilities of this device on https://vulkan.gpuinfo.org/",
        "Unsupported queue family defined at profile data index #0"
    });
#endif

    mock.SetInstanceAPIVersion(VK_API_VERSION_1_1);
    mock.SetDeviceAPIVersion(VK_API_VERSION_1_1);

    mock.SetDeviceExtensions(mock.vkPhysicalDevice, {
        VK_EXT(VK_KHR_8BIT_STORAGE),
        VK_EXT(VK_KHR_CREATE_RENDERPASS_2),
        VK_EXT(VK_KHR_DEPTH_STENCIL_RESOLVE),
        VK_EXT(VK_KHR_DRIVER_PROPERTIES),
        VK_EXT(VK_KHR_IMAGE_FORMAT_LIST),
        VK_EXT(VK_KHR_IMAGELESS_FRAMEBUFFER),
        VK_EXT(VK_KHR_SAMPLER_MIRROR_CLAMP_TO_EDGE),
        VK_EXT(VK_KHR_SHADER_FLOAT16_INT8),
        VK_EXT(VK_KHR_TIMELINE_SEMAPHORE),
        VK_EXT(VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT),
        VK_EXT(VK_EXT_DESCRIPTOR_INDEXING),
        VK_EXT(VK_EXT_HOST_QUERY_RESET),
        VK_EXT(VK_EXT_INLINE_UNIFORM_BLOCK),
        VK_EXT(VK_EXT_SCALAR_BLOCK_LAYOUT),
        VK_EXT(VK_EXT_ROBUSTNESS_2),
        VK_EXT(VK_EXT_SUBGROUP_SIZE_CONTROL),
        VK_EXT(VK_EXT_TEXEL_BUFFER_ALIGNMENT),
        VK_EXT(VK_EXT_VERTEX_ATTRIBUTE_DIVISOR),
        VK_EXT(VK_KHR_SWAPCHAIN),
        VK_EXT(VK_KHR_SWAPCHAIN_MUTABLE_FORMAT),
    });

    VpProfileProperties profile{ VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, VP_LUNARG_DESKTOP_PORTABILITY_2021_SPEC_VERSION };

    VkPhysicalDeviceVariablePointerFeatures vpFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES };
    VkPhysicalDeviceShaderFloat16Int8FeaturesKHR sf16i8Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR, &vpFeatures };
    VkPhysicalDeviceShaderDrawParametersFeatures sdpFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETER_FEATURES, &sf16i8Features };
    VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR ubslFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR, &sdpFeatures };
    VkPhysicalDeviceHostQueryResetFeaturesEXT hqResetFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT, &ubslFeatures };
    VkPhysicalDeviceDescriptorIndexingFeaturesEXT descIndFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT, &hqResetFeatures };
    VkPhysicalDevice8BitStorageFeatures storage8Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES, &descIndFeatures };
    VkPhysicalDevice16BitStorageFeatures storage16Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES, &storage8Features };
    VkPhysicalDeviceImagelessFramebufferFeaturesKHR imagelessFbFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES, &storage16Features };
    VkPhysicalDeviceMultiviewFeatures multiviewFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES, &imagelessFbFeatures };
    VkPhysicalDeviceFeatures2 features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &multiviewFeatures };
    vpGetProfileFeatures(&profile, &features);
    features.features.logicOp = VK_TRUE;
    storage8Features.storagePushConstant8 = VK_TRUE;
    mock.SetFeatures({
        VK_STRUCT(features),
        VK_STRUCT(multiviewFeatures),
        VK_STRUCT(imagelessFbFeatures),
        VK_STRUCT(storage16Features),
        VK_STRUCT(storage8Features),
        VK_STRUCT(descIndFeatures),
        VK_STRUCT(hqResetFeatures),
        VK_STRUCT(ubslFeatures),
        VK_STRUCT(sdpFeatures),
        VK_STRUCT(sf16i8Features),
        VK_STRUCT(vpFeatures)
    });

    VkPhysicalDeviceMaintenance3Properties maint3Props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES };
    VkPhysicalDeviceDepthStencilResolvePropertiesKHR dsResProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR, &maint3Props };
    VkPhysicalDeviceDescriptorIndexingPropertiesEXT descIndProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT, &dsResProps };
    VkPhysicalDeviceMultiviewProperties multiviewProps{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES, &descIndProps };
    VkPhysicalDeviceProperties2 props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, &multiviewProps };
    vpGetProfileProperties(&profile, &props);
    props.properties.limits.maxImageDimension3D = 8192;
    props.properties.limits.maxBoundDescriptorSets = 16;
    props.properties.limits.subPixelPrecisionBits = 8;
    props.properties.limits.framebufferColorSampleCounts |= VK_SAMPLE_COUNT_16_BIT;
    props.properties.limits.pointSizeRange[0] = 1.f;
    props.properties.limits.pointSizeRange[1] = 128.f;
    props.properties.limits.pointSizeGranularity = 0.0625f;
    multiviewProps.maxMultiviewViewCount = 8;
    mock.SetProperties({
        VK_STRUCT(props),
        VK_STRUCT(multiviewProps),
        VK_STRUCT(descIndProps),
        VK_STRUCT(dsResProps),
        VK_STRUCT(maint3Props)
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
    // Unsupported queue family as it's missing VK_QUEUE_TRANSFER_BIT
    queueFamilyProps.queueFamilyProperties.queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_SPARSE_BINDING_BIT;
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
