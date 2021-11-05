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

#include "vulkan_profiles.hpp"

#include <cstdio>
#include <memory>
#include <vector>

VkResult vpEnumerateDeviceProfiles(VkPhysicalDevice physicalDevice, const char* pLayerName, uint32_t* pPropertyCount,
                                   VpProfileProperties* pProperties) {
    assert(physicalDevice != VK_NULL_HANDLE);
    assert(pPropertyCount != nullptr);

    uint32_t availablePropertyCount = 0;
    vpGetProfiles(&availablePropertyCount, nullptr);

    std::vector<VpProfileProperties> availableProperties(availablePropertyCount);
    vpGetProfiles(&availablePropertyCount, availableProperties.data());

    *pPropertyCount = 0;
    std::size_t outputIndex = 0;
    for (uint32_t i = 0, n = availablePropertyCount; i < n; ++i) {
        VkBool32 supported = VK_FALSE;
        VkResult result = vpGetDeviceProfileSupport(physicalDevice, pLayerName, &availableProperties[i], &supported);
        if (result != VK_SUCCESS) {
            *pPropertyCount = 0;
            return result;
        }

        if (supported == VK_TRUE) {
            ++(*pPropertyCount);

            if (pProperties != nullptr) {
                pProperties[outputIndex] = availableProperties[i];
                ++outputIndex;
            }
        }
    }

    return VK_SUCCESS;
}

bool IsFound(std::vector<VpProfileProperties>& profiles, const char* profileName) {
    for (std::size_t i = 0, n = profiles.size(); i < n; ++i) {
        if (strcmp(profiles[i].profileName, profileName) == 0) return true;
    }

    return false;
}

TEST(test_profile, enumerate) {
    TestScaffold scaffold;

    uint32_t profileCount = 0;

    VkResult result_count = vpEnumerateDeviceProfiles(scaffold.physicalDevice, nullptr, &profileCount, nullptr);
    EXPECT_TRUE(result_count == VK_SUCCESS);
    EXPECT_TRUE(profileCount > 0);

    std::vector<VpProfileProperties> profiles;
    if (profileCount > 0) {
        profiles.resize(profileCount);
        VkResult result_profile = vpEnumerateDeviceProfiles(scaffold.physicalDevice, nullptr, &profileCount, &profiles[0]);
        EXPECT_TRUE(result_profile == VK_SUCCESS);
    }

    for (VpProfileProperties profile : profiles) {
        std::printf("Profile supported: %s, version %d\n", profile.profileName, profile.specVersion);
    }

    EXPECT_TRUE(IsFound(profiles, VP_LUNARG_DESKTOP_PORTABILITY_2022_NAME));
#ifdef VK_ENABLE_BETA_EXTENSIONS
    EXPECT_FALSE(IsFound(profiles, VP_LUNARG_DESKTOP_PORTABILITY_2022_SUBSET_NAME));
#endif
}

TEST(test_profile, create_profile) {
    TestScaffold scaffold;

    uint32_t profileCount = 0;

    VkResult result_count = vpEnumerateDeviceProfiles(scaffold.physicalDevice, nullptr, &profileCount, nullptr);
    EXPECT_TRUE(result_count == VK_SUCCESS);
    EXPECT_TRUE(profileCount > 0);

    std::vector<VpProfileProperties> profiles;
    if (profileCount > 0) {
        profiles.resize(profileCount);
        VkResult result_profile = vpEnumerateDeviceProfiles(scaffold.physicalDevice, nullptr, &profileCount, &profiles[0]);
        EXPECT_TRUE(result_profile == VK_SUCCESS);
    }

    for (const VpProfileProperties& profile : profiles) {
        std::printf("Profile supported: %s, version %d\n", profile.profileName, profile.specVersion);
    }

    int error = 0;

    for (const VpProfileProperties& profile : profiles) {
        std::printf("Creating a Vulkan device using profile %s, version %d: ", profile.profileName, profile.specVersion);

        VkDeviceCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        info.pNext = nullptr;
        info.queueCreateInfoCount = 1;
        info.pQueueCreateInfos = &scaffold.queueCreateInfo;
        info.enabledExtensionCount = 0;
        info.ppEnabledExtensionNames = nullptr;
        info.pEnabledFeatures = nullptr;

        VpDeviceCreateInfo profileInfo = {};
        profileInfo.pCreateInfo = &info;
        profileInfo.pProfile = &profile;

        VkDevice device = VK_NULL_HANDLE;
        VkResult res = vpCreateDevice(scaffold.physicalDevice, &profileInfo, nullptr, &device);
        if (res != VK_SUCCESS) {
            ++error;
            EXPECT_TRUE(device == VK_NULL_HANDLE);
            std::printf("FAILURE: %d\n", res);
        } else {
            std::printf("SUCCESS!\n");
        }
    }

    EXPECT_EQ(0, error);
}

TEST(test_profile, create_extensions_supported) {
    TestScaffold scaffold;

    uint32_t profileCount = 0;

    VkResult result_count = vpEnumerateDeviceProfiles(scaffold.physicalDevice, nullptr, &profileCount, nullptr);
    EXPECT_TRUE(result_count == VK_SUCCESS);
    EXPECT_TRUE(profileCount > 0);

    std::vector<VpProfileProperties> profiles;
    if (profileCount > 0) {
        profiles.resize(profileCount);
        VkResult result_profile = vpEnumerateDeviceProfiles(scaffold.physicalDevice, nullptr, &profileCount, &profiles[0]);
        EXPECT_TRUE(result_profile == VK_SUCCESS);
    }

    for (const VpProfileProperties& profile : profiles) {
        std::printf("Profile supported: %s, version %d\n", profile.profileName, profile.specVersion);
    }

    int error = 0;

    static const char* extensions[] = {"VK_KHR_image_format_list", "VK_KHR_maintenance3", "VK_KHR_imageless_framebuffer"};

    for (const VpProfileProperties& profile : profiles) {
        std::printf("Creating a Vulkan device using profile %s, version %d: ", profile.profileName, profile.specVersion);

        VkDeviceCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        info.pNext = nullptr;
        info.queueCreateInfoCount = 1;
        info.pQueueCreateInfos = &scaffold.queueCreateInfo;
        info.enabledExtensionCount = _vpCountOf(extensions);
        info.ppEnabledExtensionNames = extensions;
        info.pEnabledFeatures = nullptr;

        VpDeviceCreateInfo profileInfo = {};
        profileInfo.pCreateInfo = &info;
        profileInfo.pProfile = &profile;

        VkDevice device = VK_NULL_HANDLE;
        VkResult res = vpCreateDevice(scaffold.physicalDevice, &profileInfo, nullptr, &device);
        if (res != VK_SUCCESS) {
            ++error;
            EXPECT_TRUE(device == VK_NULL_HANDLE);
            std::printf("FAILURE: %d\n", res);
        } else {
            vkDestroyDevice(device, nullptr);
            std::printf("SUCCESS!\n");
        }
    }

    EXPECT_EQ(0, error);
}

TEST(test_profile, create_extensions_unsupported) {
    TestScaffold scaffold;

    uint32_t profileCount = 0;

    VkResult result_count = vpEnumerateDeviceProfiles(scaffold.physicalDevice, nullptr, &profileCount, nullptr);
    EXPECT_TRUE(result_count == VK_SUCCESS);
    EXPECT_TRUE(profileCount > 0);

    std::vector<VpProfileProperties> profiles;
    if (profileCount > 0) {
        profiles.resize(profileCount);
        VkResult result_profile = vpEnumerateDeviceProfiles(scaffold.physicalDevice, nullptr, &profileCount, &profiles[0]);
        EXPECT_TRUE(result_profile == VK_SUCCESS);
    }

    for (const VpProfileProperties& profile : profiles) {
        std::printf("Profile supported: %s, version %d\n", profile.profileName, profile.specVersion);
    }

    int error = 0;

    static const char* extensions[] = {"VK_LUNARG_doesnot_exist", "VK_GTRUC_automagic_rendering",
                                       "VK_GTRUC_portability_everywhere"};

    for (const VpProfileProperties& profile : profiles) {
        std::printf("Creating a Vulkan device using profile %s, version %d: ", profile.profileName, profile.specVersion);

        VkDeviceCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        info.pNext = nullptr;
        info.queueCreateInfoCount = 1;
        info.pQueueCreateInfos = &scaffold.queueCreateInfo;
        info.enabledExtensionCount = _vpCountOf(extensions);
        info.ppEnabledExtensionNames = extensions;
        info.pEnabledFeatures = nullptr;

        VpDeviceCreateInfo profileInfo = {};
        profileInfo.pCreateInfo = &info;
        profileInfo.pProfile = &profile;

        VkDevice device = VK_NULL_HANDLE;
        VkResult res = vpCreateDevice(scaffold.physicalDevice, &profileInfo, nullptr, &device);
        if (res != VK_SUCCESS) {
            EXPECT_TRUE(device == VK_NULL_HANDLE);
            std::printf("EXPECTED FAILURE: %d\n", res);
        } else {
            ++error;
            vkDestroyDevice(device, nullptr);
            std::printf("UNEXPECTED SUCCESS\n");
        }
    }

    EXPECT_EQ(0, error);
}

TEST(test_profile, check_extension_not_found) {
    static const char* EXTENSIONS[] = {VK_KHR_MAINTENANCE_3_EXTENSION_NAME, VK_EXT_PIPELINE_CREATION_CACHE_CONTROL_EXTENSION_NAME};

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

TEST(test_profile, check_extension_found) {
    static const char* EXTENSIONS[] = {VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME, VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME};

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

TEST(test_profile, get_extensions) {
    static const char* EXTENSIONS[] = {
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

        std::vector<const char*> extensions;
        _vpGetExtensions(&profileInfo, _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2022_EXTENSIONS),
                         &_VP_LUNARG_DESKTOP_PORTABILITY_2022_EXTENSIONS[0], extensions);
        EXPECT_EQ(_vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2022_EXTENSIONS), extensions.size());
    }

    {
        info.enabledExtensionCount = _vpCountOf(EXTENSIONS);
        info.ppEnabledExtensionNames = EXTENSIONS;
        profileInfo.flags = VP_DEVICE_CREATE_MERGE_EXTENSIONS_BIT;

        std::vector<const char*> extensions;
        _vpGetExtensions(&profileInfo, _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2022_EXTENSIONS),
                         &_VP_LUNARG_DESKTOP_PORTABILITY_2022_EXTENSIONS[0], extensions);
        EXPECT_EQ(_vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2022_EXTENSIONS) + 2, extensions.size());
    }

    {
        info.enabledExtensionCount = _vpCountOf(EXTENSIONS);
        info.ppEnabledExtensionNames = EXTENSIONS;
        profileInfo.flags = VP_DEVICE_CREATE_OVERRIDE_EXTENSIONS_BIT;

        std::vector<const char*> extensions;
        _vpGetExtensions(&profileInfo, _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2022_EXTENSIONS),
                         &_VP_LUNARG_DESKTOP_PORTABILITY_2022_EXTENSIONS[0], extensions);
        EXPECT_EQ(_vpCountOf(EXTENSIONS), extensions.size());
    }

    {
        info.enabledExtensionCount = 0;
        info.ppEnabledExtensionNames = nullptr;
        profileInfo.flags = VP_DEVICE_CREATE_OVERRIDE_EXTENSIONS_BIT;

        std::vector<const char*> extensions;
        _vpGetExtensions(&profileInfo, _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2022_EXTENSIONS),
                         &_VP_LUNARG_DESKTOP_PORTABILITY_2022_EXTENSIONS[0], extensions);
        EXPECT_EQ(0, extensions.size());
    }
}

TEST(test_profile, create_extensions_flag) {
    TestScaffold scaffold;

    VpProfileProperties profile{VP_LUNARG_DESKTOP_PORTABILITY_2022_NAME, VP_LUNARG_DESKTOP_PORTABILITY_2022_SPEC_VERSION};

    VkBool32 supported = VK_FALSE;
    vpGetDeviceProfileSupport(scaffold.physicalDevice, nullptr, &profile, &supported);
    EXPECT_EQ(VK_TRUE, supported);

    int error = 0;

    std::printf("Creating a Vulkan device using profile %s, version %d: ", profile.profileName, profile.specVersion);

    static const char* extensions[] = {VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME, VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME};

    VkDeviceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pNext = nullptr;
    info.queueCreateInfoCount = 1;
    info.pQueueCreateInfos = &scaffold.queueCreateInfo;
    info.pEnabledFeatures = nullptr;

    VpDeviceCreateInfo profileInfo = {};
    profileInfo.pCreateInfo = &info;
    profileInfo.pProfile = &profile;

    // override profile extensions
    {
        info.enabledExtensionCount = _vpCountOf(extensions);
        info.ppEnabledExtensionNames = extensions;
        profileInfo.flags = VP_DEVICE_CREATE_OVERRIDE_EXTENSIONS_BIT;

        VkDevice device = VK_NULL_HANDLE;
        VkResult res = vpCreateDevice(scaffold.physicalDevice, &profileInfo, nullptr, &device);
        if (res != VK_SUCCESS) {
            ++error;
            EXPECT_TRUE(device == VK_NULL_HANDLE);
            std::printf("FAILURE: %d\n", res);
        } else {
            vkDestroyDevice(device, nullptr);
            std::printf("SUCCESS!\n");
        }
    }

    // add extensions to the profile extension list
    {
        info.enabledExtensionCount = _vpCountOf(extensions);
        info.ppEnabledExtensionNames = extensions;
        profileInfo.flags = 0;

        VkDevice device = VK_NULL_HANDLE;
        VkResult res = vpCreateDevice(scaffold.physicalDevice, &profileInfo, nullptr, &device);
        if (res != VK_SUCCESS) {
            ++error;
            EXPECT_TRUE(device == VK_NULL_HANDLE);
            std::printf("FAILURE: %d\n", res);
        } else {
            vkDestroyDevice(device, nullptr);
            std::printf("SUCCESS!\n");
        }
    }

    // add extensions to the profile extension list but the list is empty
    {
        info.enabledExtensionCount = 0;
        info.ppEnabledExtensionNames = nullptr;
        profileInfo.flags = 0;

        VkDevice device = VK_NULL_HANDLE;
        VkResult res = vpCreateDevice(scaffold.physicalDevice, &profileInfo, nullptr, &device);
        if (res != VK_SUCCESS) {
            ++error;
            EXPECT_TRUE(device == VK_NULL_HANDLE);
            std::printf("FAILURE: %d\n", res);
        } else {
            vkDestroyDevice(device, nullptr);
            std::printf("SUCCESS!\n");
        }
    }

    EXPECT_EQ(0, error);
}

TEST(test_profile, create_features) {
    TestScaffold scaffold;

    uint32_t profileCount = 0;

    VkResult result_count = vpEnumerateDeviceProfiles(scaffold.physicalDevice, nullptr, &profileCount, nullptr);
    EXPECT_TRUE(result_count == VK_SUCCESS);
    EXPECT_TRUE(profileCount > 0);

    std::vector<VpProfileProperties> profiles;
    if (profileCount > 0) {
        profiles.resize(profileCount);
        VkResult result_profile = vpEnumerateDeviceProfiles(scaffold.physicalDevice, nullptr, &profileCount, &profiles[0]);
        EXPECT_TRUE(result_profile == VK_SUCCESS);
    }

    for (const VpProfileProperties& profile : profiles) {
        std::printf("Profile supported: %s, version %d\n", profile.profileName, profile.specVersion);
    }

    int error = 0;

    for (const VpProfileProperties& profile : profiles) {
        std::printf("Creating a Vulkan device using profile %s, version %d: ", profile.profileName, profile.specVersion);

        VkPhysicalDeviceFeatures enabledFeatures = {};
        enabledFeatures.robustBufferAccess = VK_TRUE;

        VkDeviceCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        info.pNext = nullptr;
        info.queueCreateInfoCount = 1;
        info.pQueueCreateInfos = &scaffold.queueCreateInfo;
        info.enabledExtensionCount = 0;
        info.ppEnabledExtensionNames = nullptr;
        info.pEnabledFeatures = &enabledFeatures;

        VpDeviceCreateInfo profileInfo = {};
        profileInfo.pCreateInfo = &info;
        profileInfo.pProfile = &profile;

        VkDevice device = VK_NULL_HANDLE;
        VkResult res = vpCreateDevice(scaffold.physicalDevice, &profileInfo, nullptr, &device);
        if (res != VK_SUCCESS) {
            ++error;
            EXPECT_TRUE(device == VK_NULL_HANDLE);
            std::printf("FAILURE: %d\n", res);
        } else {
            std::printf("SUCCESS!\n");
        }
    }

    EXPECT_EQ(0, error);
}

TEST(test_profile, create_pnext) {
    TestScaffold scaffold;

    uint32_t profileCount = 0;

    VkResult result_count = vpEnumerateDeviceProfiles(scaffold.physicalDevice, nullptr, &profileCount, nullptr);
    EXPECT_TRUE(result_count == VK_SUCCESS);
    EXPECT_TRUE(profileCount > 0);

    std::vector<VpProfileProperties> profiles;
    if (profileCount > 0) {
        profiles.resize(profileCount);
        VkResult result_profile = vpEnumerateDeviceProfiles(scaffold.physicalDevice, nullptr, &profileCount, &profiles[0]);
        EXPECT_TRUE(result_profile == VK_SUCCESS);
    }

    for (const VpProfileProperties& profile : profiles) {
        std::printf("Profile supported: %s, version %d\n", profile.profileName, profile.specVersion);
    }

    int error = 0;

    for (const VpProfileProperties& profile : profiles) {
        std::printf("Creating a Vulkan device using profile %s, version %d: ", profile.profileName, profile.specVersion);

        VkPhysicalDeviceSubgroupSizeControlFeaturesEXT deviceFeatures = {};
        deviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES_EXT;
        deviceFeatures.pNext = nullptr;
        deviceFeatures.subgroupSizeControl = VK_FALSE;
        deviceFeatures.computeFullSubgroups = VK_TRUE;

        VkDeviceCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        info.pNext = &deviceFeatures;
        info.queueCreateInfoCount = 1;
        info.pQueueCreateInfos = &scaffold.queueCreateInfo;
        info.enabledExtensionCount = 0;
        info.ppEnabledExtensionNames = nullptr;
        info.pEnabledFeatures = nullptr;

        VpDeviceCreateInfo profileInfo = {};
        profileInfo.pCreateInfo = &info;
        profileInfo.pProfile = &profile;

        VkDevice device = VK_NULL_HANDLE;
        VkResult res = vpCreateDevice(scaffold.physicalDevice, &profileInfo, nullptr, &device);
        if (res != VK_SUCCESS) {
            ++error;
            EXPECT_TRUE(device == VK_NULL_HANDLE);
            std::printf("FAILURE: %d\n", res);
        } else {
            std::printf("SUCCESS!\n");
        }
    }

    EXPECT_EQ(0, error);
}

TEST(test_profile, get_device_properties2) {
    VkPhysicalDeviceProperties2 profileProperties2{};
    profileProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    profileProperties2.pNext = nullptr;

    const VpProfileProperties Profile = {VP_LUNARG_DESKTOP_PORTABILITY_2022_NAME, 1};

    vpGetProfileStructures(&Profile, &profileProperties2);

    EXPECT_EQ(16384, profileProperties2.properties.limits.maxImageDimension1D);
    EXPECT_EQ(16384, profileProperties2.properties.limits.maxImageDimension2D);
    EXPECT_EQ(2048, profileProperties2.properties.limits.maxImageDimension3D);
    EXPECT_EQ(16384, profileProperties2.properties.limits.maxImageDimensionCube);
    EXPECT_EQ(2048, profileProperties2.properties.limits.maxImageArrayLayers);
    EXPECT_EQ(8, profileProperties2.properties.limits.maxColorAttachments);
}

TEST(test_profile, get_device_descriptor_indexing_properties) {
    VkPhysicalDeviceDescriptorIndexingProperties properties{};
    properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES;
    properties.pNext = nullptr;

    const VpProfileProperties Profile = {VP_LUNARG_DESKTOP_PORTABILITY_2022_NAME, 1};

    vpGetProfileStructures(&Profile, &properties);

    EXPECT_EQ(1048576, properties.maxUpdateAfterBindDescriptorsInAllPools);
    EXPECT_EQ(16, properties.maxPerStageDescriptorUpdateAfterBindSamplers);
}

TEST(test_profile, get_device_properties_chain) {
    VkPhysicalDeviceDescriptorIndexingProperties properties0{};
    properties0.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES;
    properties0.pNext = nullptr;

    VkPhysicalDeviceInlineUniformBlockPropertiesEXT properties1{};
    properties1.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_PROPERTIES_EXT;
    properties1.pNext = &properties0;

    const VpProfileProperties Profile = {VP_LUNARG_DESKTOP_PORTABILITY_2022_NAME, 1};

    vpGetProfileStructures(&Profile, &properties1);

    EXPECT_EQ(1048576, properties0.maxUpdateAfterBindDescriptorsInAllPools);
    EXPECT_EQ(16, properties0.maxPerStageDescriptorUpdateAfterBindSamplers);
    EXPECT_EQ(256, properties1.maxInlineUniformBlockSize);
}

TEST(test_profile, get_device_features) {
    VkPhysicalDeviceVulkan12Features deviceVulkan12Features = {};
    deviceVulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    deviceVulkan12Features.pNext = nullptr;

    const VpProfileProperties Profile = {VP_KHR_ROADMAP_2022_NAME, 1};

    vpGetProfileStructures(&Profile, &deviceVulkan12Features);

    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.samplerMirrorClampToEdge);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.drawIndirectCount);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.storageBuffer8BitAccess);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.uniformAndStorageBuffer8BitAccess);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.storagePushConstant8);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.shaderBufferInt64Atomics);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.shaderSharedInt64Atomics);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.shaderFloat16);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.shaderInt8);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.descriptorIndexing);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.shaderUniformTexelBufferArrayDynamicIndexing);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.uniformBufferStandardLayout);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.shaderSubgroupExtendedTypes);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.separateDepthStencilLayouts);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.hostQueryReset);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.bufferDeviceAddress);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.vulkanMemoryModel);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.vulkanMemoryModelDeviceScope);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.vulkanMemoryModelAvailabilityVisibilityChains);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.subgroupBroadcastDynamicId);
    EXPECT_EQ(VK_TRUE, deviceVulkan12Features.imagelessFramebuffer);
    EXPECT_EQ(VK_FALSE, deviceVulkan12Features.shaderOutputLayer);
}

TEST(test_profile, get_profile_extension_properties_full) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, 1};

    uint32_t propertyCount = 0;
    vpGetProfileExtensionProperties(&profile, &propertyCount, nullptr);
    EXPECT_EQ(12, propertyCount);

    std::vector<VkExtensionProperties> properties(propertyCount);
    vpGetProfileExtensionProperties(&profile, &propertyCount, &properties[0]);

    EXPECT_STREQ(VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME, properties[0].extensionName);
    EXPECT_STREQ(VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME, properties[1].extensionName);
    EXPECT_STREQ(VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME, properties[2].extensionName);
    EXPECT_STREQ(VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME, properties[3].extensionName);
    EXPECT_STREQ(VK_EXT_PRIVATE_DATA_EXTENSION_NAME, properties[4].extensionName);
    EXPECT_STREQ(VK_EXT_PIPELINE_CREATION_CACHE_CONTROL_EXTENSION_NAME, properties[5].extensionName);
    EXPECT_STREQ(VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME, properties[6].extensionName);
    EXPECT_STREQ(VK_EXT_IMAGE_ROBUSTNESS_EXTENSION_NAME, properties[7].extensionName);
    EXPECT_STREQ(VK_KHR_ZERO_INITIALIZE_WORKGROUP_MEMORY_EXTENSION_NAME, properties[8].extensionName);
    EXPECT_STREQ(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME, properties[9].extensionName);
    EXPECT_STREQ(VK_KHR_SHADER_TERMINATE_INVOCATION_EXTENSION_NAME, properties[10].extensionName);
    EXPECT_STREQ(VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME, properties[11].extensionName);
}

TEST(test_profile, get_profile_extension_properties_partial) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, 1};

    uint32_t propertyCount = 0;
    vpGetProfileExtensionProperties(&profile, &propertyCount, nullptr);
    EXPECT_EQ(12, propertyCount);

    propertyCount = 5;

    std::vector<VkExtensionProperties> properties(propertyCount);
    vpGetProfileExtensionProperties(&profile, &propertyCount, &properties[0]);
    EXPECT_EQ(5, propertyCount);

    EXPECT_STREQ(VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME, properties[0].extensionName);
    EXPECT_STREQ(VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME, properties[1].extensionName);
    EXPECT_STREQ(VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME, properties[2].extensionName);
    EXPECT_STREQ(VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME, properties[3].extensionName);
    EXPECT_STREQ(VK_EXT_PRIVATE_DATA_EXTENSION_NAME, properties[4].extensionName);
}

TEST(test_profile, get_profile_structure_properties_full) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, 1};

    uint32_t propertyCount = 0;
    vpGetProfileStructureProperties(&profile, &propertyCount, nullptr);
    EXPECT_EQ(17, propertyCount);

    std::vector<VpStructureProperties> properties(propertyCount);
    vpGetProfileStructureProperties(&profile, &propertyCount, &properties[0]);

    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, properties[0].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, properties[1].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, properties[2].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES_KHR, properties[3].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR, properties[4].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_WORKGROUP_MEMORY_FEATURES_KHR, properties[5].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES_EXT, properties[6].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT, properties[7].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES_EXT, properties[8].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES_EXT, properties[9].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES_EXT, properties[10].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES_EXT, properties[11].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT, properties[12].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT, properties[13].type);

    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES, properties[14].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES, properties[15].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, properties[16].type);
}

TEST(test_profile, get_profile_structure_properties_partial) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, 1};

    uint32_t structureTypesCount = 0;
    vpGetProfileStructureProperties(&profile, &structureTypesCount, nullptr);
    EXPECT_EQ(17, structureTypesCount);

    structureTypesCount = 5;

    std::vector<VpStructureProperties> structureTypes(structureTypesCount);
    vpGetProfileStructureProperties(&profile, &structureTypesCount, &structureTypes[0]);
    EXPECT_EQ(5, structureTypesCount);

    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, structureTypes[0].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, structureTypes[1].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, structureTypes[2].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES_KHR, structureTypes[3].type);
    EXPECT_EQ(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR, structureTypes[4].type);
}

TEST(test_profile, get_profile_formats_full) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2022_NAME, 1};

    uint32_t formatCount = 0;
    vpGetProfileFormats(&profile, &formatCount, nullptr);
    EXPECT_EQ(66, formatCount);

    std::vector<VkFormat> formats(formatCount);
    vpGetProfileFormats(&profile, &formatCount, &formats[0]);
    EXPECT_EQ(66, formatCount);

    EXPECT_EQ(VK_FORMAT_R8_UNORM, formats[0]);
    EXPECT_EQ(VK_FORMAT_R8_SNORM, formats[1]);
    EXPECT_EQ(VK_FORMAT_R8_UINT, formats[2]);
}

TEST(test_profile, get_profile_formats_partial) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2022_NAME, 1};

    uint32_t formatCount = 0;
    vpGetProfileFormats(&profile, &formatCount, nullptr);
    EXPECT_EQ(66, formatCount);

    formatCount = 3;

    std::vector<VkFormat> formats(formatCount);
    vpGetProfileFormats(&profile, &formatCount, &formats[0]);
    EXPECT_EQ(3, formatCount);

    EXPECT_EQ(VK_FORMAT_R8_UNORM, formats[0]);
    EXPECT_EQ(VK_FORMAT_R8_SNORM, formats[1]);
    EXPECT_EQ(VK_FORMAT_R8_UINT, formats[2]);
}

TEST(test_profile, get_profile_formats_unspecified) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, 1};

    uint32_t formatCount = 0;
    vpGetProfileFormats(&profile, &formatCount, nullptr);
    EXPECT_EQ(0, formatCount);
}

TEST(test_profile, get_profile_format_properties) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2022_NAME, 1};

    VkFormatProperties2 properties2 = {};
    properties2.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    vpGetProfileFormatProperties(&profile, VK_FORMAT_D16_UNORM, &properties2);
    EXPECT_EQ(0, properties2.formatProperties.bufferFeatures);
    EXPECT_EQ(VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
                  VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                  VK_FORMAT_FEATURE_TRANSFER_DST_BIT,
              properties2.formatProperties.optimalTilingFeatures);
    EXPECT_EQ(0, properties2.formatProperties.linearTilingFeatures);

    VkFormatProperties3KHR properties3 = {};
    properties3.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3_KHR;
    vpGetProfileFormatProperties(&profile, VK_FORMAT_D16_UNORM, &properties3);
    EXPECT_EQ(0, properties3.bufferFeatures);
    EXPECT_EQ(VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
                  VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                  VK_FORMAT_FEATURE_TRANSFER_DST_BIT,
              properties3.optimalTilingFeatures);
    EXPECT_EQ(0, properties3.linearTilingFeatures);
}

TEST(test_profile, get_profile_format_properties_chained) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2022_NAME, 1};

    VkFormatProperties2 properties2 = {};
    properties2.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    properties2.pNext = nullptr;

    VkFormatProperties3KHR properties3 = {};
    properties3.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3_KHR;
    properties3.pNext = &properties2;

    vpGetProfileFormatProperties(&profile, VK_FORMAT_D16_UNORM, &properties3);

    EXPECT_EQ(0, properties2.formatProperties.bufferFeatures);
    EXPECT_EQ(VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
                  VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                  VK_FORMAT_FEATURE_TRANSFER_DST_BIT,
              properties2.formatProperties.optimalTilingFeatures);
    EXPECT_EQ(0, properties2.formatProperties.linearTilingFeatures);

    EXPECT_EQ(0, properties3.bufferFeatures);
    EXPECT_EQ(VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
                  VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                  VK_FORMAT_FEATURE_TRANSFER_DST_BIT,
              properties3.optimalTilingFeatures);
    EXPECT_EQ(0, properties3.linearTilingFeatures);
}

TEST(test_profile, get_profile_memory_types_full) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2022_NAME, 1};

    uint32_t memoryPropertyFlagsCount = 0;
    vpGetProfileMemoryTypes(&profile, &memoryPropertyFlagsCount, nullptr);
    EXPECT_EQ(2, memoryPropertyFlagsCount);

    std::vector<VkMemoryPropertyFlags> memoryPropertyFlags(memoryPropertyFlagsCount);
    vpGetProfileMemoryTypes(&profile, &memoryPropertyFlagsCount, &memoryPropertyFlags[0]);
    EXPECT_EQ(2, memoryPropertyFlagsCount);

    EXPECT_EQ(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memoryPropertyFlags[0]);
    EXPECT_EQ(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
              memoryPropertyFlags[1]);
}

TEST(test_profile, get_profile_memory_types_partial) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2022_NAME, 1};

    uint32_t memoryPropertyFlagsCount = 0;
    vpGetProfileMemoryTypes(&profile, &memoryPropertyFlagsCount, nullptr);
    EXPECT_EQ(2, memoryPropertyFlagsCount);

    memoryPropertyFlagsCount = 1;

    std::vector<VkMemoryPropertyFlags> memoryPropertyFlags(memoryPropertyFlagsCount);
    vpGetProfileMemoryTypes(&profile, &memoryPropertyFlagsCount, &memoryPropertyFlags[0]);
    EXPECT_EQ(1, memoryPropertyFlagsCount);

    EXPECT_EQ(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memoryPropertyFlags[0]);
}

TEST(test_profile, get_profile_memory_types_unspecified) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, 1};

    uint32_t memoryPropertyFlagsCount = 0;
    vpGetProfileMemoryTypes(&profile, &memoryPropertyFlagsCount, nullptr);
    EXPECT_EQ(0, memoryPropertyFlagsCount);
}

TEST(test_profile, get_profile_queue_families_full) {
    const VpProfileProperties profile = {VP_LUNARG_DESKTOP_PORTABILITY_2022_NAME, 1};

    uint32_t count = 0;
    vpGetProfileQueueFamilies(&profile, &count, nullptr);
    EXPECT_EQ(1, count);

    std::vector<VkQueueFamilyProperties> data(count);
    vpGetProfileQueueFamilies(&profile, &count, &data[0]);
    EXPECT_EQ(1, count);

    EXPECT_EQ(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT,
              data[0].queueFlags);
    EXPECT_EQ(1, data[0].queueCount);
    EXPECT_EQ(36, data[0].timestampValidBits);
    EXPECT_EQ(1, data[0].minImageTransferGranularity.width);
    EXPECT_EQ(1, data[0].minImageTransferGranularity.height);
    EXPECT_EQ(1, data[0].minImageTransferGranularity.depth);
}

TEST(test_profile, get_profile_queue_families_unspecified) {
    const VpProfileProperties profile = {VP_KHR_ROADMAP_2022_NAME, 1};

    uint32_t count = 0;
    vpGetProfileQueueFamilies(&profile, &count, nullptr);
    EXPECT_EQ(0, count);
}

TEST(test_profile, get_profiles_full) {
    uint32_t pPropertyCount = 0;
    vpGetProfiles(&pPropertyCount, nullptr);

#ifdef VK_ENABLE_BETA_EXTENSIONS
    EXPECT_EQ(3, pPropertyCount);
#else
    EXPECT_EQ(2, pPropertyCount);
#endif

    std::vector<VpProfileProperties> pProperties(pPropertyCount);
    vpGetProfiles(&pPropertyCount, &pProperties[0]);
#ifdef VK_ENABLE_BETA_EXTENSIONS
    EXPECT_EQ(3, pPropertyCount);
#else
    EXPECT_EQ(2, pPropertyCount);
#endif

    EXPECT_STREQ(VP_KHR_ROADMAP_2022_NAME, pProperties[0].profileName);
    EXPECT_EQ(VP_KHR_ROADMAP_2022_SPEC_VERSION, pProperties[0].specVersion);

    EXPECT_STREQ(VP_LUNARG_DESKTOP_PORTABILITY_2022_NAME, pProperties[1].profileName);
    EXPECT_EQ(VP_LUNARG_DESKTOP_PORTABILITY_2022_SPEC_VERSION, pProperties[1].specVersion);

#ifdef VK_ENABLE_BETA_EXTENSIONS
    EXPECT_STREQ(VP_LUNARG_DESKTOP_PORTABILITY_2022_SUBSET_NAME, pProperties[2].profileName);
    EXPECT_EQ(VP_LUNARG_DESKTOP_PORTABILITY_2022_SUBSET_SPEC_VERSION, pProperties[2].specVersion);
#endif
}

TEST(test_profile, get_profiles_partial) {
    uint32_t pPropertyCount = 0;
    vpGetProfiles(&pPropertyCount, nullptr);

#ifdef VK_ENABLE_BETA_EXTENSIONS
    EXPECT_EQ(3, pPropertyCount);
#else
    EXPECT_EQ(2, pPropertyCount);
#endif

    pPropertyCount = 1;

    std::vector<VpProfileProperties> pProperties(pPropertyCount);
    vpGetProfiles(&pPropertyCount, &pProperties[0]);
    EXPECT_EQ(1, pPropertyCount);

    EXPECT_STREQ(VP_KHR_ROADMAP_2022_NAME, pProperties[0].profileName);
    EXPECT_EQ(VP_KHR_ROADMAP_2022_SPEC_VERSION, pProperties[0].specVersion);
}
