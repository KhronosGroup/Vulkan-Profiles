/*
 * Copyright (c) 2021-2024 Valve Corporation
 * Copyright (c) 2021-2024 LunarG, Inc.
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
#ifndef VULKAN_PROFILES_HEADER_ONLY
#include <vulkan/vulkan_profiles.hpp>
#else
#include <vulkan/debug/vulkan_profiles.h>
#endif

#include <cstdio>
#include <memory>
#include <vector>

#define countof(arr) sizeof(arr) / sizeof(arr[0])

static TestScaffold* scaffold = nullptr;

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    ::scaffold = new TestScaffold;

    int result = RUN_ALL_TESTS();

    delete ::scaffold;
    ::scaffold = nullptr;

    return result;
}

struct Capabilities {
    VpCapabilities handle = VK_NULL_HANDLE;

    Capabilities() {
        VpVulkanFunctions vulkanFunctions;
        vulkanFunctions.GetInstanceProcAddr = vkGetInstanceProcAddr;
        vulkanFunctions.GetDeviceProcAddr = vkGetDeviceProcAddr;
        vulkanFunctions.EnumerateInstanceVersion = vkEnumerateInstanceVersion;
        vulkanFunctions.EnumerateInstanceExtensionProperties = vkEnumerateInstanceExtensionProperties;
        vulkanFunctions.EnumerateDeviceExtensionProperties = vkEnumerateDeviceExtensionProperties;
        vulkanFunctions.GetPhysicalDeviceFeatures2 = vkGetPhysicalDeviceFeatures2;
        vulkanFunctions.GetPhysicalDeviceProperties2 = vkGetPhysicalDeviceProperties2;
        vulkanFunctions.GetPhysicalDeviceFormatProperties2 = vkGetPhysicalDeviceFormatProperties2;
        vulkanFunctions.GetPhysicalDeviceQueueFamilyProperties2 = vkGetPhysicalDeviceQueueFamilyProperties2;
        vulkanFunctions.CreateInstance = vkCreateInstance;
        vulkanFunctions.CreateDevice = vkCreateDevice;

        VpCapabilitiesCreateInfo createInfo;
        createInfo.apiVersion = VK_API_VERSION_1_1;
        createInfo.flags = VP_PROFILE_CREATE_STATIC_BIT;
        createInfo.pVulkanFunctions = &vulkanFunctions;

        vpCreateCapabilities(&createInfo, nullptr, &handle);
    }

    ~Capabilities() {
        vpDestroyCapabilities(handle, nullptr);
    }
};

TEST(api_capabilities_object, check_support_vulkan_1_1) {
    Capabilities object;

    const VpProfileProperties profileProperties = {VP_LUNARG_MINIMUM_REQUIREMENTS_1_1_NAME, VP_LUNARG_MINIMUM_REQUIREMENTS_1_1_SPEC_VERSION};

    VkBool32 supported = VK_FALSE;
    VkResult result = vpGetPhysicalDeviceProfileSupport(
        object.handle, scaffold->instance, scaffold->physicalDevice, &profileProperties, &supported);
    EXPECT_TRUE(result == VK_SUCCESS);
    EXPECT_TRUE(supported == VK_TRUE);
}

TEST(api_capabilities_object, overrite_with_unsupported_extensions) {
    Capabilities object;

    const VpProfileProperties profileProperties = {VP_LUNARG_DESKTOP_BASELINE_2023_NAME, VP_LUNARG_DESKTOP_BASELINE_2023_SPEC_VERSION};

    static const char* extensions[] = {"VK_LUNARG_doesnot_exist", "VK_GTRUC_automagic_rendering",
                                       "VK_GTRUC_portability_everywhere"};

    VkDeviceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pNext = nullptr;
    info.queueCreateInfoCount = 1;
    info.pQueueCreateInfos = &scaffold->queueCreateInfo;
    info.enabledExtensionCount = countof(extensions);
    info.ppEnabledExtensionNames = extensions;
    info.pEnabledFeatures = nullptr;

    VpDeviceCreateInfo profileInfo = {};
    profileInfo.pCreateInfo = &info;
    profileInfo.enabledFullProfileCount = 1;
    profileInfo.pEnabledFullProfiles = &profileProperties;
    profileInfo.flags = 0;

    VkDevice device = VK_NULL_HANDLE;
    VkResult res = vpCreateDevice(
        object.handle, scaffold->physicalDevice, &profileInfo, nullptr, &device);
    EXPECT_TRUE(res != VK_SUCCESS);
    EXPECT_TRUE(device == VK_NULL_HANDLE);
}

TEST(api_capabilities_object, get_profiles) {
    Capabilities object;

    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfiles(object.handle, &propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_TRUE(propertyCount > 1);

    std::vector<VpProfileProperties> properties(propertyCount);
    VkResult result1 = vpGetProfiles(object.handle, &propertyCount, &properties[0]);
    EXPECT_EQ(VK_SUCCESS, result1);
    EXPECT_TRUE(propertyCount > 1);

    EXPECT_STREQ(VP_KHR_ROADMAP_2022_NAME, properties[0].profileName);
    EXPECT_EQ(VP_KHR_ROADMAP_2022_SPEC_VERSION, properties[0].specVersion);
}

TEST(api_capabilities_object, get_api_version) {
    Capabilities object;

    const VpProfileProperties profileProperties = {VP_LUNARG_DESKTOP_BASELINE_2023_NAME, 1};

    uint32_t version = vpGetProfileAPIVersion(object.handle, &profileProperties);

    EXPECT_EQ(VK_MAKE_API_VERSION(0, 1, 2, 148), version);
}

TEST(api_capabilities_object, get_required_profiles) {
    Capabilities object;

    const VpProfileProperties profileProperties = {VP_KHR_ROADMAP_2024_NAME, VP_KHR_ROADMAP_2024_SPEC_VERSION};

    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfileRequiredProfiles(object.handle, &profileProperties, &propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_TRUE(propertyCount == 1);
}

TEST(api_capabilities_object, get_profile_fallback) {
    Capabilities object;

    const VpProfileProperties profileProperties = {VP_LUNARG_DESKTOP_BASELINE_2023_NAME, 1};

    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfileFallbacks(object.handle, &profileProperties, &propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_TRUE(propertyCount == 0);
}

TEST(api_capabilities_object, has_multiple_variants_profile) {
    Capabilities object;

    const VpProfileProperties profileProperties = {VP_LUNARG_DESKTOP_BASELINE_2023_NAME, 1};

    VkBool32 has_multiple_variants = VK_TRUE;
    VkResult result0 = vpHasMultipleVariantsProfile(object.handle, &profileProperties, &has_multiple_variants);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(VK_FALSE, has_multiple_variants);
}

TEST(api_capabilities_object, get_profile_device_extension_properties) {
    Capabilities object;

    const VpProfileProperties profileProperties = {VP_KHR_ROADMAP_2022_NAME, 1};

    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfileDeviceExtensionProperties(
        object.handle, &profileProperties, nullptr, &propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(1, propertyCount);

    propertyCount = 2;

    std::vector<VkExtensionProperties> properties(propertyCount);
    VkResult result1 = vpGetProfileDeviceExtensionProperties(
        object.handle, &profileProperties, nullptr, &propertyCount, &properties[0]);
    EXPECT_EQ(VK_SUCCESS, result1);
    EXPECT_EQ(1, propertyCount);

    EXPECT_STREQ(VK_KHR_GLOBAL_PRIORITY_EXTENSION_NAME, properties[0].extensionName);
}

TEST(api_capabilities_object, get_profile_instance_extension_properties) {
    Capabilities object;

    const VpProfileProperties profileProperties = {VP_LUNARG_DESKTOP_BASELINE_2023_NAME, 1};

    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfileInstanceExtensionProperties(
        object.handle, &profileProperties, nullptr, &propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(0, propertyCount);

    propertyCount = 2;

    std::vector<VkExtensionProperties> properties(propertyCount);
    VkResult result1 = vpGetProfileInstanceExtensionProperties(
        object.handle, &profileProperties, nullptr, &propertyCount, &properties[0]);
    EXPECT_EQ(VK_SUCCESS, result1);
    EXPECT_EQ(0, propertyCount);
}

TEST(api_capabilities_object, get_profile_fallbacks) {
    Capabilities object;

    const VpProfileProperties profileProperties = {VP_LUNARG_DESKTOP_BASELINE_2023_NAME, 1};

    uint32_t count = 0;
    VkResult result0 = vpGetProfileFallbacks(object.handle, &profileProperties, &count, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(0, count);

    count = 1;

    std::vector<VpProfileProperties> data(count);
    VkResult result1 = vpGetProfileFallbacks(object.handle, &profileProperties, &count, &data[0]);
    EXPECT_EQ(VK_SUCCESS, result1);
    EXPECT_EQ(0, count);
}

TEST(api_capabilities_object, get_profile_formats) {
    Capabilities object;

    const VpProfileProperties profileProperties = {VP_KHR_ROADMAP_2022_NAME, 1};

    uint32_t formatCount = 0;
    VkResult result0 = vpGetProfileFormats(object.handle, &profileProperties, nullptr, &formatCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(0, formatCount);
}

TEST(api_capabilities_object, get_profile_properties) {
    Capabilities object;

    VkPhysicalDeviceProperties2 profileProperties2{};
    profileProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    profileProperties2.pNext = nullptr;

    const VpProfileProperties profileProperties = {VP_LUNARG_DESKTOP_BASELINE_2023_NAME, 1};

    vpGetProfileProperties(object.handle, &profileProperties, nullptr, &profileProperties2);

    EXPECT_EQ(16384, profileProperties2.properties.limits.maxImageDimension1D);
    EXPECT_EQ(16384, profileProperties2.properties.limits.maxImageDimension2D);
    EXPECT_EQ(2048, profileProperties2.properties.limits.maxImageDimension3D);
    EXPECT_EQ(16384, profileProperties2.properties.limits.maxImageDimensionCube);
    EXPECT_EQ(2048, profileProperties2.properties.limits.maxImageArrayLayers);
    EXPECT_EQ(8, profileProperties2.properties.limits.maxColorAttachments);
}

TEST(api_capabilities_object, get_profile_features) {
    Capabilities object;

    VkPhysicalDeviceVulkan12Features deviceVulkan12Features = {};
    deviceVulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    deviceVulkan12Features.pNext = nullptr;

    VkPhysicalDeviceFeatures2 deviceFeatures2 = {};
    deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    deviceFeatures2.pNext = &deviceVulkan12Features;

    const VpProfileProperties profileProperties = {VP_KHR_ROADMAP_2022_NAME, 1};

    vpGetProfileFeatures(object.handle, &profileProperties, nullptr, &deviceFeatures2);

    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.samplerMirrorClampToEdge);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.drawIndirectCount);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.storageBuffer8BitAccess);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.uniformAndStorageBuffer8BitAccess);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.storagePushConstant8);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.shaderBufferInt64Atomics);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.shaderSharedInt64Atomics);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.shaderFloat16);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.shaderInt8);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.descriptorIndexing);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.shaderUniformTexelBufferArrayDynamicIndexing);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.uniformBufferStandardLayout);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.shaderSubgroupExtendedTypes);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.separateDepthStencilLayouts);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.hostQueryReset);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.bufferDeviceAddress);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.vulkanMemoryModel);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.vulkanMemoryModelDeviceScope);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.subgroupBroadcastDynamicId);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.imagelessFramebuffer);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.shaderOutputLayer);
}

TEST(api_capabilities_object, get_profile_feature_structure_types) {
    Capabilities object;

    const VpProfileProperties profileProperties = {VP_KHR_ROADMAP_2022_NAME, 1};

    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfileFeatureStructureTypes(
        object.handle, &profileProperties, nullptr, &propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(4, propertyCount);

    propertyCount = 5;

    std::vector<VkStructureType> properties(propertyCount);
    VkResult result1 = vpGetProfileFeatureStructureTypes(
        object.handle, &profileProperties, nullptr, &propertyCount, &properties[0]);
    EXPECT_EQ(VK_SUCCESS, result1);
    EXPECT_EQ(4, propertyCount);

    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, properties[0]);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, properties[1]);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES, properties[2]);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, properties[3]);
}

TEST(api_capabilities_object, get_profile_Property_structure_types) {
    Capabilities object;

    const VpProfileProperties profileProperties = {VP_KHR_ROADMAP_2022_NAME, 1};

    uint32_t propertyCount = 0;
    VkResult result0 = vpGetProfilePropertyStructureTypes(
        object.handle, &profileProperties, nullptr, &propertyCount, nullptr);
    EXPECT_EQ(VK_SUCCESS, result0);
    EXPECT_EQ(4, propertyCount);

    propertyCount = 5;

    std::vector<VkStructureType> properties(propertyCount);
    VkResult result1 = vpGetProfilePropertyStructureTypes(
        object.handle, &profileProperties, nullptr, &propertyCount, &properties[0]);
    EXPECT_EQ(VK_SUCCESS, result1);
    EXPECT_EQ(4, propertyCount);

    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES, properties[0]);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES, properties[1]);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES, properties[2]);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, properties[3]);
}
