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
 * - Christophe Riccio <christophe@lunarg.com>
 */

#define VK_ENABLE_BETA_EXTENSIONS 1

#include "test.hpp"
#include <vulkan/vulkan_profiles.hpp>

struct FormatFeatureFlagBits {
    int flag;
    std::string label;
};

std::vector<std::string> GetFormatFeatures(int flags) {
    static const FormatFeatureFlagBits format_features[] = {
        {VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT, "VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT"},
        {VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT, "VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT"},
        {VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT, "VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT"},
        {VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT, "VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT"},
        {VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT, "VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT"},
        {VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT, "VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT"},
        {VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT, "VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT"},
        {VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT, "VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT"},
        {VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, "VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT"},
        {VK_FORMAT_FEATURE_BLIT_SRC_BIT, "VK_FORMAT_FEATURE_BLIT_SRC_BIT"},
        {VK_FORMAT_FEATURE_BLIT_DST_BIT, "VK_FORMAT_FEATURE_BLIT_DST_BIT"},
        {VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT, "VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT"},
        {VK_FORMAT_FEATURE_TRANSFER_SRC_BIT, "VK_FORMAT_FEATURE_TRANSFER_SRC_BIT"},
        {VK_FORMAT_FEATURE_TRANSFER_DST_BIT, "VK_FORMAT_FEATURE_TRANSFER_DST_BIT"},
        {VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT, "VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT"}};

    std::vector<std::string> results;

    for (std::size_t i = 0, n = _vpCountOf(format_features); i < n; ++i) {
        if (flags & format_features[i].flag) results.push_back(format_features[i].label);
    }

    return results;
}

TEST(test_library_util, GetPropertiesDesktopPortability1_0) {
    std::vector<std::string> VK_FORMAT_B4G4R4A4_UNORM_PACK16 = GetFormatFeatures(5121);
    // VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT

    std::vector<std::string> VK_FORMAT_R5G6B5_UNORM_PACK16 = GetFormatFeatures(7553);
    // VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT

    std::vector<std::string> VK_FORMAT_R8_UINT = GetFormatFeatures(3201);
    // VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
    // VK_FORMAT_FEATURE_BLIT_DST_BIT

    std::vector<std::string> VK_FORMAT_R8_UINT_buffer = GetFormatFeatures(72);
    // VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT

    std::vector<std::string> VK_FORMAT_R8G8B8A8_UNORM_buffer = GetFormatFeatures(88);
    // VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT

    std::vector<std::string> VK_FORMAT_R32G32_SFLOAT = GetFormatFeatures(52355);
    // VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
    // VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
    // VK_FORMAT_FEATURE_TRANSFER_DST_BIT

    std::vector<std::string> VK_FORMAT_R32_SINT = GetFormatFeatures(52359);
    // VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
    // VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
    // VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT

    std::vector<std::string> VK_FORMAT_R16G16B16A16_SFLOAT = GetFormatFeatures(56707);
    // VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
    // VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
    // VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT

    std::vector<std::string> VK_FORMAT_R8G8B8A8_SNORM = GetFormatFeatures(54275);
    // VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
    // VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT

    EXPECT_TRUE(true);
}

TEST(test_library_util, GetStructure_override) {
    VkPhysicalDeviceVulkan12Features deviceVulkan12Features = {};
    deviceVulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    deviceVulkan12Features.pNext = nullptr;

    const VkPhysicalDeviceVulkan12Features *requestedVulkan12Features = (const VkPhysicalDeviceVulkan12Features *)_vpGetStructure(
        &deviceVulkan12Features, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES);

    EXPECT_EQ(requestedVulkan12Features, &deviceVulkan12Features);

    EXPECT_EQ(VK_FALSE, requestedVulkan12Features->samplerMirrorClampToEdge);
    EXPECT_EQ(VK_FALSE, requestedVulkan12Features->shaderUniformTexelBufferArrayDynamicIndexing);
    EXPECT_EQ(VK_FALSE, requestedVulkan12Features->imagelessFramebuffer);

    EXPECT_EQ(VK_FALSE, requestedVulkan12Features->drawIndirectCount);
    EXPECT_EQ(VK_FALSE, requestedVulkan12Features->descriptorIndexing);

    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.samplerMirrorClampToEdge);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.shaderUniformTexelBufferArrayDynamicIndexing);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.imagelessFramebuffer);

    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.drawIndirectCount);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.descriptorIndexing);
}

TEST(test_library_util, GetStructure) {
    VkPhysicalDeviceVulkan11Features pNext_1 = {};
    pNext_1.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    pNext_1.pNext = nullptr;

    VkPhysicalDeviceVulkan12Features pNext_2 = {};
    pNext_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    pNext_2.pNext = &pNext_1;

    VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR pNext_3 = {};
    pNext_3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES_KHR;
    pNext_3.pNext = &pNext_2;

    VkPhysicalDeviceSynchronization2FeaturesKHR pNext_4 = {};
    pNext_4.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR;
    pNext_4.pNext = &pNext_3;

    VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR pNext_5 = {};
    pNext_5.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_WORKGROUP_MEMORY_FEATURES_KHR;
    pNext_5.pNext = &pNext_4;

    VkPhysicalDeviceImageRobustnessFeaturesEXT pNext_6 = {};
    pNext_6.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES_EXT;
    pNext_6.pNext = &pNext_5;

    VkPhysicalDeviceInlineUniformBlockFeaturesEXT pNext_7 = {};
    pNext_7.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT;
    pNext_7.pNext = &pNext_6;

    VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT pNext_8 = {};
    pNext_8.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES_EXT;
    pNext_8.pNext = &pNext_7;

    VkPhysicalDevicePrivateDataFeaturesEXT pNext_9 = {};
    pNext_9.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES_EXT;
    pNext_9.pNext = &pNext_8;

    EXPECT_EQ(&pNext_9, _vpGetStructure(&pNext_9, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES_EXT));
    EXPECT_EQ(&pNext_2, _vpGetStructure(&pNext_9, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES));
    EXPECT_EQ(nullptr, _vpGetStructure(&pNext_9, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT));
}

TEST(test_library_util, CheckExtension) {
    static const VkExtensionProperties test_data[] = {VkExtensionProperties{"VK_EXT_extended_dynamic_state2", 1},
                                                      VkExtensionProperties{"VK_EXT_texel_buffer_alignment", 1},
                                                      VkExtensionProperties{"VK_EXT_subgroup_size_control", 2},
                                                      VkExtensionProperties{"VK_EXT_shader_demote_to_helper_invocation", 1},
                                                      VkExtensionProperties{"VK_EXT_private_data", 1},
                                                      VkExtensionProperties{"VK_EXT_pipeline_creation_cache_control", 1},
                                                      VkExtensionProperties{"VK_EXT_inline_uniform_block", 1},
                                                      VkExtensionProperties{"VK_EXT_image_robustness", 1},
                                                      VkExtensionProperties{"VK_KHR_zero_initialize_workgroup_memory", 1},
                                                      VkExtensionProperties{"VK_KHR_synchronization2", 1},
                                                      VkExtensionProperties{"VK_KHR_shader_terminate_invocation", 1},
                                                      VkExtensionProperties{"VK_KHR_imageless_framebuffer", 1}};

    EXPECT_TRUE(_vpCheckExtension(test_data, _vpCountOf(test_data), "VK_KHR_synchronization2"));

    EXPECT_TRUE(!_vpCheckExtension(test_data, _vpCountOf(test_data), "VK_KHR_synchronization"));
    EXPECT_TRUE(!_vpCheckExtension(test_data, _vpCountOf(test_data), "KHR_synchronization2"));
    EXPECT_TRUE(!_vpCheckExtension(test_data, _vpCountOf(test_data), "VK_EXT_synchronization2"));
}

TEST(test_library_util, CheckQueueFamilyProperty) {
    TestScaffold scaffold;

    std::uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(scaffold.physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamily(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(scaffold.physicalDevice, &queueFamilyCount, queueFamily.data());

    {
        static const VkQueueFamilyProperties test_data[] = {
            {VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT, 1, 36, {1, 1, 1}}};
        EXPECT_TRUE(_vpCheckQueueFamilyProperty(&queueFamily[0], queueFamilyCount, test_data[0]));
    }

    // Check unsupported flag
    {
        static const VkQueueFamilyProperties test_data[] = {
            {VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | (1 << 29), 1, 36, {1, 1, 1}}};
        EXPECT_TRUE(!_vpCheckQueueFamilyProperty(&queueFamily[0], queueFamilyCount, test_data[0]));
    }

    // Check unsupported timestampValidBits
    {
        static const VkQueueFamilyProperties test_data[] = {{VK_QUEUE_GRAPHICS_BIT, 1, 4894042, {1, 1, 1}}};
        EXPECT_TRUE(!_vpCheckQueueFamilyProperty(&queueFamily[0], queueFamilyCount, test_data[0]));
    }

    // Check unsupported queue count
    {
        static const VkQueueFamilyProperties test_data[] = {{VK_QUEUE_GRAPHICS_BIT, 46848646, 32, {1, 1, 1}}};
        EXPECT_TRUE(!_vpCheckQueueFamilyProperty(&queueFamily[0], queueFamilyCount, test_data[0]));
    }
}

TEST(test_library_util, CheckMemoryProperty) {
    TestScaffold scaffold;

    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(scaffold.physicalDevice, &memoryProperties);

    EXPECT_TRUE(_vpCheckMemoryProperty(memoryProperties, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
    EXPECT_TRUE(_vpCheckMemoryProperty(memoryProperties, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

    EXPECT_TRUE(!_vpCheckMemoryProperty(memoryProperties, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | (1 << 29)));
}

TEST(test_library_util, CheckFormatProperty) {
    TestScaffold scaffold;

    VpFormatProperties profileProps;
    profileProps.format = VK_FORMAT_R8G8B8A8_UNORM;
    profileProps.linearTilingFeatures = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                        VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    profileProps.optimalTilingFeatures =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    profileProps.bufferFeatures = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
                                  VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    {
        VkFormatProperties2 deviceProps = {};
        deviceProps.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
        vkGetPhysicalDeviceFormatProperties2(scaffold.physicalDevice, VK_FORMAT_R8G8B8A8_UNORM, &deviceProps);

        EXPECT_TRUE(_vpCheckFormatProperty(&deviceProps, profileProps));
    }

    {
        VkFormatProperties2 deviceProps = {};
        deviceProps.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
        vkGetPhysicalDeviceFormatProperties2(scaffold.physicalDevice, VK_FORMAT_R8G8B8A8_UNORM, &deviceProps);
        deviceProps.formatProperties.linearTilingFeatures = 0;

        EXPECT_TRUE(!_vpCheckFormatProperty(&deviceProps, profileProps));
    }

    {
        VkFormatProperties2 deviceProps = {};
        deviceProps.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
        vkGetPhysicalDeviceFormatProperties2(scaffold.physicalDevice, VK_FORMAT_R8G8B8A8_UNORM, &deviceProps);
        deviceProps.formatProperties.optimalTilingFeatures = 0;

        EXPECT_TRUE(!_vpCheckFormatProperty(&deviceProps, profileProps));
    }

    {
        VkFormatProperties2 deviceProps = {};
        deviceProps.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
        vkGetPhysicalDeviceFormatProperties2(scaffold.physicalDevice, VK_FORMAT_R8G8B8A8_UNORM, &deviceProps);
        deviceProps.formatProperties.bufferFeatures = 0;

        EXPECT_TRUE(!_vpCheckFormatProperty(&deviceProps, profileProps));
    }
}

TEST(test_library_util, _vpGetExtensions) {
    static const char *EXTENSIONS[] = {
        VK_KHR_MAINTENANCE_3_EXTENSION_NAME,                    // Not in VP_LUNARG_1_1_DESKTOP_PORTABILITY_2022_NAME
        VK_EXT_PIPELINE_CREATION_CACHE_CONTROL_EXTENSION_NAME,  // Not in VP_LUNARG_1_1_DESKTOP_PORTABILITY_2022_NAME
        VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,              // In VP_LUNARG_1_1_DESKTOP_PORTABILITY_2022_NAME
        VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME                 // In VP_LUNARG_1_1_DESKTOP_PORTABILITY_2022_NAME
    };

    VkDeviceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pNext = nullptr;
    info.pEnabledFeatures = nullptr;

    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2022_NAME, VP_LUNARG_DESKTOP_PORTABILITY_2022_SPEC_VERSION};

    VpDeviceCreateInfo profileInfo = {};
    profileInfo.pCreateInfo = &info;
    profileInfo.pProfile = &profile;

    {
        info.enabledExtensionCount = 0;
        info.ppEnabledExtensionNames = nullptr;
        profileInfo.flags = VP_DEVICE_CREATE_MERGE_EXTENSIONS_BIT;

        std::vector<const char *> extensions;
        _vpGetExtensions(&profileInfo, _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2022_EXTENSIONS),
                         &_VP_LUNARG_DESKTOP_PORTABILITY_2022_EXTENSIONS[0], extensions);
        EXPECT_EQ(_vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2022_EXTENSIONS), extensions.size());
    }

    {
        info.enabledExtensionCount = _vpCountOf(EXTENSIONS);
        info.ppEnabledExtensionNames = EXTENSIONS;
        profileInfo.flags = VP_DEVICE_CREATE_MERGE_EXTENSIONS_BIT;

        std::vector<const char *> extensions;
        _vpGetExtensions(&profileInfo, _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2022_EXTENSIONS),
                         &_VP_LUNARG_DESKTOP_PORTABILITY_2022_EXTENSIONS[0], extensions);
        EXPECT_EQ(_vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2022_EXTENSIONS) + 2, extensions.size());
    }

    {
        info.enabledExtensionCount = _vpCountOf(EXTENSIONS);
        info.ppEnabledExtensionNames = EXTENSIONS;
        profileInfo.flags = VP_DEVICE_CREATE_OVERRIDE_EXTENSIONS_BIT;

        std::vector<const char *> extensions;
        _vpGetExtensions(&profileInfo, _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2022_EXTENSIONS),
                         &_VP_LUNARG_DESKTOP_PORTABILITY_2022_EXTENSIONS[0], extensions);
        EXPECT_EQ(_vpCountOf(EXTENSIONS), extensions.size());
    }

    {
        info.enabledExtensionCount = 0;
        info.ppEnabledExtensionNames = nullptr;
        profileInfo.flags = VP_DEVICE_CREATE_OVERRIDE_EXTENSIONS_BIT;

        std::vector<const char *> extensions;
        _vpGetExtensions(&profileInfo, _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2022_EXTENSIONS),
                         &_VP_LUNARG_DESKTOP_PORTABILITY_2022_EXTENSIONS[0], extensions);
        EXPECT_EQ(0, extensions.size());
    }
}

TEST(test_library_util, check_extension_not_found) {
    static const char *EXTENSIONS[] = {VK_KHR_MAINTENANCE_3_EXTENSION_NAME, VK_EXT_PIPELINE_CREATION_CACHE_CONTROL_EXTENSION_NAME};

    VkDeviceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pNext = nullptr;
    info.pEnabledFeatures = nullptr;
    info.enabledExtensionCount = _vpCountOf(EXTENSIONS);
    info.ppEnabledExtensionNames = EXTENSIONS;

    for (std::size_t i = 0, n = info.enabledExtensionCount; i < n; ++i) {
        EXPECT_FALSE(_vpCheckExtension(&_VP_LUNARG_DESKTOP_PORTABILITY_2022_EXTENSIONS[0],
                                       _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2022_EXTENSIONS),
                                       info.ppEnabledExtensionNames[i]));
    }
}

TEST(test_library_util, check_extension_found) {
    static const char *EXTENSIONS[] = {VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME, VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME};

    VkDeviceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pNext = nullptr;
    info.pEnabledFeatures = nullptr;
    info.enabledExtensionCount = _vpCountOf(EXTENSIONS);
    info.ppEnabledExtensionNames = EXTENSIONS;

    for (std::size_t i = 0, n = info.enabledExtensionCount; i < n; ++i) {
        EXPECT_TRUE(_vpCheckExtension(&_VP_LUNARG_DESKTOP_PORTABILITY_2022_EXTENSIONS[0],
                                      _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2022_EXTENSIONS), info.ppEnabledExtensionNames[i]));
    }
}
