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
