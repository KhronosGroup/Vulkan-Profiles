
/*
 * Copyright (c) 2021-2022 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License")
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
 * - Christophe Riccio <christophe@lunarg.com>
 */

#ifndef VULKAN_PROFILES_
#define VULKAN_PROFILES_ 1

#define VP_INLINE

#ifdef __cplusplus
    extern "C" {
#endif
#include <vulkan/vulkan_core.h>
#ifdef VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan_beta.h>
#endif

#define VK_KHR_roadmap_2022 1
#define VP_KHR_ROADMAP_2022_NAME "VP_KHR_roadmap_2022"
#define VP_KHR_ROADMAP_2022_SPEC_VERSION 1
#define VP_KHR_ROADMAP_2022_MIN_VERSION VK_MAKE_VERSION(1, 3, 200)

#define VP_ANDROID_baseline_2022 1
#define VP_ANDROID_BASELINE_2022_NAME "VP_ANDROID_baseline_2022"
#define VP_ANDROID_BASELINE_2022_SPEC_VERSION 1
#define VP_ANDROID_BASELINE_2022_MIN_API_VERSION VK_MAKE_VERSION(1, 0, 68)

#define VP_ANDROID_angle_es31 1
#define VP_ANDROID_ANGLE_ES31_2022_NAME "VP_ANDROID_angle_es31"
#define VP_ANDROID_ANGLE_ES31_2022_SPEC_VERSION 1
#define VP_ANDROID_ANGLE_ES31_2022_MIN_API_VERSION VK_MAKE_VERSION(1, 1, 131)

#define VP_LUNARG_desktop_portability_2022 1
#define VP_LUNARG_DESKTOP_PORTABILITY_2022_NAME "VP_LUNARG_desktop_portability_2022"
#define VP_LUNARG_DESKTOP_PORTABILITY_2022_SPEC_VERSION 1
#define VP_LUNARG_DESKTOP_PORTABILITY_2022_MIN_API_VERSION VK_MAKE_VERSION(1, 1, 142)

#ifdef VK_ENABLE_BETA_EXTENSIONS
#define VP_LUNARG_desktop_portability_2022_subset 1
#define VP_LUNARG_DESKTOP_PORTABILITY_2022_SUBSET_NAME "VP_LUNARG_desktop_portability_2022_subset"
#define VP_LUNARG_DESKTOP_PORTABILITY_2022_SUBSET_SPEC_VERSION 1
#define VP_LUNARG_DESKTOP_PORTABILITY_2022_SUBSET_MIN_API_VERSION VK_MAKE_VERSION(1, 1, 142)
#endif

#define VP_MAX_PROFILE_NAME_SIZE 256U

typedef struct VpProfileProperties {
    char profileName[VP_MAX_PROFILE_NAME_SIZE];
    uint32_t specVersion;
} VpProfileProperties;

typedef enum VpDeviceCreateFlagBits {
    VP_DEVICE_CREATE_DISABLE_ROBUST_BUFFER_ACCESS_BIT = 0x00000001,
    VP_DEVICE_CREATE_DISABLE_ROBUST_IMAGE_ACCESS_BIT = 0x00000002,
    VP_DEVICE_CREATE_MERGE_EXTENSIONS_BIT = 0x00000004,
    VP_DEVICE_CREATE_OVERRIDE_EXTENSIONS_BIT = 0x00000008,

    VP_DEVICE_CREATE_DISABLE_ROBUST_ACCESS_BIT =
        VP_DEVICE_CREATE_DISABLE_ROBUST_BUFFER_ACCESS_BIT | VP_DEVICE_CREATE_DISABLE_ROBUST_IMAGE_ACCESS_BIT,
    VP_DEVICE_CREATE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
} VpDeviceCreateFlagBits;
typedef VkFlags VpDeviceCreateFlags;

typedef struct VpDeviceCreateInfo {
    const VkDeviceCreateInfo *pCreateInfo;
    const VpProfileProperties *pProfile;
    VpDeviceCreateFlags flags;
} VpDeviceCreateInfo;

// Query the list of available profiles in the library
void vpGetProfiles(uint32_t *pPropertyCount, VpProfileProperties *pProperties);

// List the recommand fallback profiles of a profile
void vpGetProfileFallbacks(const VpProfileProperties *pProfile, uint32_t *pPropertyCount, VpProfileProperties *pProperties);

// Check whether a profile is supported by the physical device
VkResult vpGetDeviceProfileSupport(VkPhysicalDevice physicalDevice, const char *pLayerName, const VpProfileProperties *pProfile,
                                   VkBool32 *pSupported);

// Create a VkDevice with the profile features and extensions enabled
VkResult vpCreateDevice(VkPhysicalDevice physicalDevice, const VpDeviceCreateInfo *pCreateInfo,
                        const VkAllocationCallbacks *pAllocator, VkDevice *pDevice);

// Query the list of extension of a profile
void vpGetProfileExtensionProperties(const VpProfileProperties *pProfile, uint32_t *pPropertyCount,
                                     VkExtensionProperties *pProperties);

// Fill the pNext Vulkan structures with the requirements of a profile
void vpGetProfileStructures(const VpProfileProperties *pProfile, void *pNext);

typedef enum VpStructureArea {
    VP_STRUCTURE_FEATURES = 0,  // A Vulkan structure specified to expose features
    VP_STRUCTURE_PROPERTIES     // A Vulkan structure specified to expose properties
} VpStructureArea;

typedef struct VpStructureProperties {
    VkStructureType type;
    VpStructureArea area;
} VpStructureProperties;

// Query the list of structures used to specify requirements of a profile
void vpGetProfileStructureProperties(const VpProfileProperties *pProfile, uint32_t *pPropertyCount,
                                     VpStructureProperties *pProperties);

// Query the list of formats with specified requirements by a profile
void vpGetProfileFormats(const VpProfileProperties *pProfile, uint32_t *pFormatCount, VkFormat *pFormats);

// Query the requirements of a format for a profile
void vpGetProfileFormatProperties(const VpProfileProperties *pProfile, VkFormat format, void *pNext);

// Query the requirements of memory types by a profile
void vpGetProfileMemoryTypes(const VpProfileProperties *pProfile, uint32_t *pMemoryTypeCount, VkMemoryPropertyFlags *pMemoryTypes);

// Query the requirements of queue families by a profile
void vpGetProfileQueueFamilies(const VpProfileProperties *pProfile, uint32_t *pPropertyCount, VkQueueFamilyProperties *pProperties);


#ifdef __cplusplus
}
#endif

#endif//VULKAN_PROFILES_
