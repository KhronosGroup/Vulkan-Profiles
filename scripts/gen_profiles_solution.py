#!/usr/bin/python3
#
# Copyright (c) 2021-2026 LunarG, Inc.
# Copyright (c) 2023-2024 RasterGrid Kft.
#
# Licensed under the Apache License, Version 2.0 (the "License")
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Authors:
# - Daniel Rakos <daniel.rakos@rastergrid.com>
# - Christophe Riccio <christophe@lunarg.com>

import os
import re
import itertools
import functools
import argparse
from typing import OrderedDict
import xml.etree.ElementTree as etree
import json
from string import Template

from source.generate_schema import VulkanProfilesSchemaGenerator
from source.generate_profiles_schema import VulkanProfilesSchemaGenerator2
from source.vulkan_registry import VulkanRegistry, VulkanVersionNumber
from source.log import Log
from vulkan_object import VulkanObject
from source.vulkan_object_utils import initVulkanObject, VK_VERSION, gatherDependentExtensions

COPYRIGHT_HEADER = '''
/*
 * Copyright (C) 2021-2026 Valve Corporation
 * Copyright (C) 2021-2026 LunarG, Inc.
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
 * This file is ***GENERATED***.  Do Not Edit.
 * See scripts/gen_profiles_solution.py for modifications.
 */
'''

DEBUG_MSG_CB_DEFINE = '''
#include <cstdio>

#ifndef VP_DEBUG_MESSAGE_CALLBACK
#if defined(ANDROID) || defined(__ANDROID__)
#include <android/log.h>
#define VP_DEBUG_MESSAGE_CALLBACK(MSG) \\
    __android_log_print(ANDROID_LOG_ERROR, "Profiles ERROR", "%s", MSG)
#else
#define VP_DEBUG_MESSAGE_CALLBACK(MSG) fprintf(stderr, "%s\\n", MSG)
#endif
#else
void VP_DEBUG_MESSAGE_CALLBACK(const char*);
#endif

#define VP_DEBUG_MSG(MSG) VP_DEBUG_MESSAGE_CALLBACK((MSG))
#define VP_DEBUG_MSGF(MSGFMT, ...) { char msg[1024]; snprintf(msg, sizeof(msg) - 1, (MSGFMT), __VA_ARGS__); VP_DEBUG_MESSAGE_CALLBACK(msg); }
#define VP_DEBUG_COND_MSG(COND, MSG) if ((COND)) { VP_DEBUG_MSG((MSG)); }
#define VP_DEBUG_COND_MSGF(COND, MSGFMT, ...) if ((COND)) { VP_DEBUG_MSGF((MSGFMT), __VA_ARGS__); }
'''

H_HEADER = '''
#ifndef VULKAN_PROFILES_H_
#define VULKAN_PROFILES_H_ 1

#define VPAPI_ATTR

#ifdef __cplusplus
    extern "C" {
#endif

#include <vulkan/vulkan.h>
'''

H_FOOTER = '''
#ifdef __cplusplus
}
#endif

#endif // VULKAN_PROFILES_H_
'''

HPP_HEADER = '''
#pragma once

#define VPAPI_ATTR inline

#include <vulkan/vulkan.h>
'''

SHARED_INCLUDE = '''
#include <cstddef>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <map>
'''

API_DEFS = '''
#define VP_HEADER_VERSION_COMPLETE VK_MAKE_API_VERSION(0, 2, 0, VK_HEADER_VERSION)

#define VP_MAX_PROFILE_NAME_SIZE 256U

typedef struct VpProfileProperties {
    char        profileName[VP_MAX_PROFILE_NAME_SIZE];
    uint32_t    specVersion;
} VpProfileProperties;

typedef struct VpBlockProperties {
    VpProfileProperties profiles;
    uint32_t apiVersion;
    char blockName[VP_MAX_PROFILE_NAME_SIZE];
} VpBlockProperties;

typedef struct VpVideoProfileProperties {
    char name[VP_MAX_PROFILE_NAME_SIZE];
} VpVideoProfileProperties;

typedef enum VpInstanceCreateFlagBits {
    VP_INSTANCE_CREATE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
} VpInstanceCreateFlagBits;
typedef VkFlags VpInstanceCreateFlags;

typedef struct VpInstanceCreateInfo {
    const VkInstanceCreateInfo* pCreateInfo;
    VpInstanceCreateFlags       flags;
    uint32_t                    enabledFullProfileCount;
    const VpProfileProperties*  pEnabledFullProfiles;
    uint32_t                    enabledProfileBlockCount;
    const VpBlockProperties*    pEnabledProfileBlocks;
} VpInstanceCreateInfo;

typedef enum VpDeviceCreateFlagBits {
    VP_DEVICE_CREATE_DISABLE_ROBUST_BUFFER_ACCESS_BIT = 0x0000001,
    VP_DEVICE_CREATE_DISABLE_ROBUST_IMAGE_ACCESS_BIT = 0x0000002,
    VP_DEVICE_CREATE_DISABLE_ROBUST_ACCESS =
        VP_DEVICE_CREATE_DISABLE_ROBUST_BUFFER_ACCESS_BIT | VP_DEVICE_CREATE_DISABLE_ROBUST_IMAGE_ACCESS_BIT,

    VP_DEVICE_CREATE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
} VpDeviceCreateFlagBits;
typedef VkFlags VpDeviceCreateFlags;

typedef struct VpDeviceCreateInfo {
    const VkDeviceCreateInfo*   pCreateInfo;
    VpDeviceCreateFlags         flags;
    uint32_t                    enabledFullProfileCount;
    const VpProfileProperties*  pEnabledFullProfiles;
    uint32_t                    enabledProfileBlockCount;
    const VpBlockProperties*    pEnabledProfileBlocks;
} VpDeviceCreateInfo;

VK_DEFINE_HANDLE(VpFunctions)

typedef VpFunctions VpCapabilities;

typedef enum VpFunctionsCreateFlagBits {
    VP_FUNCTIONS_CREATE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
} VpFunctionsCreateFlagBits;

typedef VkFlags VpFunctionsCreateFlags;

typedef enum VpInstanceFunctionsLoadFlagBits {
    VP_INSTANCE_FUNCTIONS_LOAD_KHR_GET_PHYSICAL_DEVICE_PROPERTIES2_BIT = (1 << 0),
    VP_INSTANCE_FUNCTIONS_LOAD_MISSING_ONLY_BIT = (1 << 1),
    VP_INSTANCE_FUNCTIONS_LOAD_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
} VpInstanceFunctionsLoadFlagBits;

typedef VkFlags VpInstanceFunctionsLoadFlags;

struct VpFunctions_T {
    static VpFunctions_T& Get() {
        static VpFunctions_T instance;
        return instance;
    }

    VpFunctions_T() {
#ifndef VK_NO_PROTOTYPES
        ImportVulkanFunctions_Static();
#endif//VK_NO_PROTOTYPES
    }

    VkResult validate(bool full_init = false) const {
        // Validate global vulkan function initialization
        // vkEnumerateInstanceVersion is omitted from validation on purpose.
        // It is not available in Vulkan 1.0, and nullptr is a valid state indicating Vulkan 1.0.

        if (this->EnumerateInstanceExtensionProperties == nullptr ||
            this->CreateInstance == nullptr ||
            this->GetInstanceProcAddr == nullptr) {
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        if (full_init) {
            if (this->GetPhysicalDeviceFeatures2 == nullptr ||
                this->GetPhysicalDeviceProperties2 == nullptr ||
                this->GetPhysicalDeviceFormatProperties2 == nullptr ||
                this->GetPhysicalDeviceQueueFamilyProperties2 == nullptr) {
                return VK_ERROR_INITIALIZATION_FAILED;
            }
        }

        return VK_SUCCESS;
    }

    PFN_vkGetInstanceProcAddr GetInstanceProcAddr = nullptr;
    PFN_vkEnumerateInstanceVersion EnumerateInstanceVersion;
    PFN_vkEnumerateInstanceExtensionProperties EnumerateInstanceExtensionProperties = nullptr;
    PFN_vkEnumerateDeviceExtensionProperties EnumerateDeviceExtensionProperties = nullptr;
    PFN_vkGetPhysicalDeviceFeatures2 GetPhysicalDeviceFeatures2 = nullptr;
    PFN_vkGetPhysicalDeviceProperties2 GetPhysicalDeviceProperties2 = nullptr;
    PFN_vkGetPhysicalDeviceFormatProperties2 GetPhysicalDeviceFormatProperties2 = nullptr;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties2 GetPhysicalDeviceQueueFamilyProperties2 = nullptr;
    PFN_vkCreateInstance CreateInstance = nullptr;
    PFN_vkCreateDevice CreateDevice = nullptr;

private:
#ifndef VK_NO_PROTOTYPES
    void ImportVulkanFunctions_Static() {
        #define VP_SET_STATIC(memberName, staticFuncName) this->memberName = (PFN_vk##memberName)staticFuncName

        VP_SET_STATIC(GetInstanceProcAddr, vkGetInstanceProcAddr);

        VP_SET_STATIC(EnumerateInstanceVersion, vkEnumerateInstanceVersion);
        VP_SET_STATIC(EnumerateInstanceExtensionProperties, vkEnumerateInstanceExtensionProperties);
        VP_SET_STATIC(EnumerateDeviceExtensionProperties, vkEnumerateDeviceExtensionProperties);

        VP_SET_STATIC(GetPhysicalDeviceFeatures2, vkGetPhysicalDeviceFeatures2);
        VP_SET_STATIC(GetPhysicalDeviceProperties2, vkGetPhysicalDeviceProperties2);
        VP_SET_STATIC(GetPhysicalDeviceFormatProperties2, vkGetPhysicalDeviceFormatProperties2);
        VP_SET_STATIC(GetPhysicalDeviceQueueFamilyProperties2, vkGetPhysicalDeviceQueueFamilyProperties2);

        VP_SET_STATIC(CreateInstance, vkCreateInstance);
        VP_SET_STATIC(CreateDevice, vkCreateDevice);

        #undef VP_SET_STATIC
    }
#endif//VK_NO_PROTOTYPES
};

/// Description of a Allocator to be created.
typedef struct VpFunctionsCreateInfo
{
    /// Flags for created allocator. Use #VpInstanceCreateFlagBits enum.
    VpFunctionsCreateFlags          flags;

    PFN_vkGetInstanceProcAddr GetInstanceProcAddr = nullptr;
    PFN_vkEnumerateInstanceVersion EnumerateInstanceVersion;
    PFN_vkEnumerateInstanceExtensionProperties EnumerateInstanceExtensionProperties = nullptr;
    PFN_vkEnumerateDeviceExtensionProperties EnumerateDeviceExtensionProperties = nullptr;
    PFN_vkCreateInstance CreateInstance = nullptr;
    PFN_vkCreateDevice CreateDevice = nullptr;
    PFN_vkGetPhysicalDeviceFeatures2 GetPhysicalDeviceFeatures2 = nullptr;
    PFN_vkGetPhysicalDeviceProperties2 GetPhysicalDeviceProperties2 = nullptr;
    PFN_vkGetPhysicalDeviceFormatProperties2 GetPhysicalDeviceFormatProperties2 = nullptr;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties2 GetPhysicalDeviceQueueFamilyProperties2 = nullptr;
} VpFunctionsCreateInfo;

#ifdef VP_USE_OBJECT

VPAPI_ATTR VkResult vpCreateFunctions(
    const VpFunctionsCreateInfo*                pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VpFunctions*                                pFunctions
);

/// Destroys allocator object.
VPAPI_ATTR void vpDestroyFunctions(
    VpFunctions                                 functions,
    const VkAllocationCallbacks*                pAllocator);

#endif//VP_USE_OBJECT

/// Helper function to initialize a VpFunctions instance with global Vulkan functions loaded with GetInstanceProcAddr
VPAPI_ATTR VkResult vpInitializeGlobalFunctions(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    PFN_vkGetInstanceProcAddr                   GetInstanceProcAddr);

/// Initializes capabilities with instance functions
VPAPI_ATTR VkResult vpInitializeInstanceFunctions(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    VkInstance                                  instance,
    VpInstanceFunctionsLoadFlags                flags);

// Query the list of available profiles in the library
VPAPI_ATTR VkResult vpGetProfiles(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    uint32_t*                                   pPropertyCount,
    VpProfileProperties*                        pProperties);

// List the required profiles of a profile
VPAPI_ATTR VkResult vpGetProfileRequiredProfiles(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    uint32_t*                                   pPropertyCount,
    VpProfileProperties*                        pProperties);

// Query the profile required Vulkan API version
VPAPI_ATTR uint32_t vpGetProfileAPIVersion(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile);

// List the recommended fallback profiles of a profile
VPAPI_ATTR VkResult vpGetProfileFallbacks(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    uint32_t*                                   pPropertyCount,
    VpProfileProperties*                        pProperties);

// Query whether the profile has multiple variants. Profiles with multiple variants can only use vpGetInstanceProfileSupport and vpGetPhysicalDeviceProfileSupport capabilities of the library. Other function will return a VK_ERROR_UNKNOWN error
VPAPI_ATTR VkResult vpHasMultipleVariantsProfile(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    VkBool32*                                   pHasMultipleVariants);

// Check whether a profile is supported at the instance level
VPAPI_ATTR VkResult vpGetInstanceProfileSupport(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const char*                                 pLayerName,
    const VpProfileProperties*                  pProfile,
    VkBool32*                                   pSupported);

// Check whether a variant of a profile is supported at the instance level and report this list of blocks used to validate the profiles
VPAPI_ATTR VkResult vpGetInstanceProfileVariantsSupport(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const char*                                 pLayerName,
    const VpProfileProperties*                  pProfile,
    VkBool32*                                   pSupported,
    uint32_t*                                   pPropertyCount,
    VpBlockProperties*                          pProperties);

// Create a VkInstance with the profile instance extensions enabled
VPAPI_ATTR VkResult vpCreateInstance(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpInstanceCreateInfo*                 pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkInstance*                                 pInstance);

// Check whether a profile is supported by the physical device
VPAPI_ATTR VkResult vpGetPhysicalDeviceProfileSupport(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    VkInstance                                  instance,
    VkPhysicalDevice                            physicalDevice,
    const VpProfileProperties*                  pProfile,
    VkBool32*                                   pSupported);

// Check whether a variant of a profile is supported by the physical device and report this list of blocks used to validate the profiles
VPAPI_ATTR VkResult vpGetPhysicalDeviceProfileVariantsSupport(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    VkInstance                                  instance,
    VkPhysicalDevice                            physicalDevice,
    const VpProfileProperties*                  pProfile,
    VkBool32*                                   pSupported,
    uint32_t*                                   pPropertyCount,
    VpBlockProperties*                          pProperties);

// Create a VkDevice with the profile features and device extensions enabled
VPAPI_ATTR VkResult vpCreateDevice(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    VkPhysicalDevice                            physicalDevice,
    const VpDeviceCreateInfo*                   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDevice*                                   pDevice);

// Query the list of instance extensions of a profile
VPAPI_ATTR VkResult vpGetProfileInstanceExtensionProperties(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t*                                   pPropertyCount,
    VkExtensionProperties*                      pProperties);

// Query the list of device extensions of a profile
VPAPI_ATTR VkResult vpGetProfileDeviceExtensionProperties(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t*                                   pPropertyCount,
    VkExtensionProperties*                      pProperties);

// Fill the feature structures with the requirements of a profile
VPAPI_ATTR VkResult vpGetProfileFeatures(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    void*                                       pNext);

// Query the list of feature structure types specified by the profile
VPAPI_ATTR VkResult vpGetProfileFeatureStructureTypes(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t*                                   pStructureTypeCount,
    VkStructureType*                            pStructureTypes);

// Fill the property structures with the requirements of a profile
VPAPI_ATTR VkResult vpGetProfileProperties(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    void*                                       pNext);

// Query the list of property structure types specified by the profile
VPAPI_ATTR VkResult vpGetProfilePropertyStructureTypes(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t*                                   pStructureTypeCount,
    VkStructureType*                            pStructureTypes);

// Fill the queue family property structures with the requirements of a profile
VPAPI_ATTR VkResult vpGetProfileQueueFamilyProperties(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t*                                   pPropertyCount,
    VkQueueFamilyProperties2KHR*                pProperties);

// Query the list of queue family property structure types specified by the profile
VPAPI_ATTR VkResult vpGetProfileQueueFamilyStructureTypes(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t*                                   pStructureTypeCount,
    VkStructureType*                            pStructureTypes);

// Query the list of formats with specified requirements by a profile
VPAPI_ATTR VkResult vpGetProfileFormats(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t*                                   pFormatCount,
    VkFormat*                                   pFormats);

// Query the requirements of a format for a profile
VPAPI_ATTR VkResult vpGetProfileFormatProperties(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    VkFormat                                    format,
    void*                                       pNext);

// Query the list of format structure types specified by the profile
VPAPI_ATTR VkResult vpGetProfileFormatStructureTypes(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t*                                   pStructureTypeCount,
    VkStructureType*                            pStructureTypes);

#ifdef VK_KHR_video_queue
// Query the list of video profiles specified by the profile
VPAPI_ATTR VkResult vpGetProfileVideoProfiles(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t*                                   pVideoProfileCount,
    VpVideoProfileProperties*                   pVideoProfiles);

// Query the video profile info structures for a video profile defined by a profile
VPAPI_ATTR VkResult vpGetProfileVideoProfileInfo(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t                                    videoProfileIndex,
    VkVideoProfileInfoKHR*                      pVideoProfileInfo);

// Query the list of video profile info structure types specified by the profile for a video profile
VPAPI_ATTR VkResult vpGetProfileVideoProfileInfoStructureTypes(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t                                    videoProfileIndex,
    uint32_t*                                   pStructureTypeCount,
    VkStructureType*                            pStructureTypes);

// Query the video capabilities requirements for a video profile defined by a profile
VPAPI_ATTR VkResult vpGetProfileVideoCapabilities(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t                                    videoProfileIndex,
    void*                                       pNext);

// Query the list of video capability structure types specified by the profile for a video profile
VPAPI_ATTR VkResult vpGetProfileVideoCapabilityStructureTypes(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t                                    videoProfileIndex,
    uint32_t*                                   pStructureTypeCount,
    VkStructureType*                            pStructureTypes);

// Query the video format property requirements for a video profile defined by a profile
VPAPI_ATTR VkResult vpGetProfileVideoFormatProperties(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t                                    videoProfileIndex,
    uint32_t*                                   pPropertyCount,
    VkVideoFormatPropertiesKHR*                 pProperties);

// Query the list of video format property structure types specified by the profile for a video profile
VPAPI_ATTR VkResult vpGetProfileVideoFormatStructureTypes(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t                                    videoProfileIndex,
    uint32_t*                                   pStructureTypeCount,
    VkStructureType*                            pStructureTypes);
#endif  // VK_KHR_video_queue
'''

PRIVATE_DEFS = '''
VPAPI_ATTR std::string FormatString(const char* message, ...) {
    std::size_t const STRING_BUFFER(4096);

    assert(message != nullptr);
    assert(strlen(message) >= 1 && strlen(message) < STRING_BUFFER);

    char buffer[STRING_BUFFER];
    va_list list;

    va_start(list, message);
    vsnprintf(buffer, STRING_BUFFER, message, list);
    va_end(list);

    return buffer;
}

VPAPI_ATTR const void* vpGetStructure(const void* pNext, VkStructureType type) {
    const VkBaseOutStructure* p = static_cast<const VkBaseOutStructure*>(pNext);
    while (p != nullptr) {
        if (p->sType == type) return p;
        p = p->pNext;
    }
    return nullptr;
}

VPAPI_ATTR void* vpGetStructure(void* pNext, VkStructureType type) {
    VkBaseOutStructure* p = static_cast<VkBaseOutStructure*>(pNext);
    while (p != nullptr) {
        if (p->sType == type) return p;
        p = p->pNext;
    }
    return nullptr;
}

VPAPI_ATTR VkBaseOutStructure* vpExtractStructure(VkPhysicalDeviceFeatures2KHR* pFeatures, VkStructureType structureType) {
    if (structureType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR) {
        return nullptr;
    }

    VkBaseOutStructure* current = reinterpret_cast<VkBaseOutStructure*>(pFeatures);
    VkBaseOutStructure* previous = nullptr;
    VkBaseOutStructure* found = nullptr;

    while (current != nullptr) {
        if (structureType == current->sType) {
            found = current;
            if (previous != nullptr) {
                previous->pNext = current->pNext;
            }
            current = nullptr;
        } else {
            previous = current;
            current = current->pNext;
        }
    }

    if (found != nullptr) {
        found->pNext = nullptr;
        return found;
    } else {
        return nullptr;
    }
}

VPAPI_ATTR void GatherStructureTypes(std::vector<VkStructureType>& structureTypes, VkBaseOutStructure* pNext) {
    while (pNext) {
        if (std::find(structureTypes.begin(), structureTypes.end(), pNext->sType) == structureTypes.end()) {
            structureTypes.push_back(pNext->sType);
        }

        pNext = pNext->pNext;
    }
}

VPAPI_ATTR bool isMultiple(double source, double multiple) {
    double mod = std::fmod(source, multiple);
    return std::abs(mod) < 0.0001;
}

VPAPI_ATTR bool isPowerOfTwo(double source) {
    double mod = std::fmod(source, 1.0);
    if (std::abs(mod) >= 0.0001) return false;

    std::uint64_t value = static_cast<std::uint64_t>(std::abs(source));
    return !(value & (value - static_cast<std::uint64_t>(1)));
}

using PFN_vpStructFiller = void(*)(VkBaseOutStructure* p);
using PFN_vpStructComparator = bool(*)(VkBaseOutStructure* p);
using PFN_vpStructChainerCb = void(*)(VkBaseOutStructure* p, void* pUser);
using PFN_vpStructChainer = void(*)(VkBaseOutStructure* p, void* pUser, PFN_vpStructChainerCb pfnCb);
using PFN_vpStructArrayChainerCb = void(*)(uint32_t count, VkBaseOutStructure* p, void* pUser);
using PFN_vpStructArrayChainer = void(*)(uint32_t count, VkBaseOutStructure* p, void* pUser, PFN_vpStructArrayChainerCb pfnCb);

struct VpFeatureDesc {
    PFN_vpStructFiller              pfnFiller;
    PFN_vpStructComparator          pfnComparator;
};

struct VpPropertyDesc {
    PFN_vpStructFiller              pfnFiller;
    PFN_vpStructComparator          pfnComparator;
};

struct VpQueueFamilyDesc {
    PFN_vpStructFiller              pfnFiller;
    PFN_vpStructComparator          pfnComparator;
};

struct VpFormatDesc {
    VkFormat                        format;
    PFN_vpStructFiller              pfnFiller;
    PFN_vpStructComparator          pfnComparator;
};

struct VpStructChainerDesc {
    PFN_vpStructChainer             pfnFeature;
    PFN_vpStructChainer             pfnProperty;
    PFN_vpStructArrayChainer        pfnQueueFamily;
    PFN_vpStructChainer             pfnFormat;
};

struct VpVideoProfileInfoDesc {
    PFN_vpStructFiller              pfnFiller;
    PFN_vpStructComparator          pfnComparator;
};

struct VpVideoCapabilityDesc {
    PFN_vpStructFiller              pfnFiller;
    PFN_vpStructComparator          pfnComparator;
};

struct VpVideoFormatDesc {
    PFN_vpStructFiller              pfnFiller;
    PFN_vpStructComparator          pfnComparator;
};

struct VpVideoProfileStructChainerDesc {
    PFN_vpStructChainer             pfnInfo;
    PFN_vpStructChainer             pfnCapability;
    PFN_vpStructArrayChainer        pfnFormat;
};

struct VpVideoProfileDesc {
    VpVideoProfileProperties properties;

    uint32_t infoStructTypeCount;
    const VkStructureType* pInfoStructTypes;
    VpVideoProfileInfoDesc info;

    uint32_t capabilityStructTypeCount;
    const VkStructureType* pCapabilityStructTypes;
    VpVideoCapabilityDesc capability;

    uint32_t formatStructTypeCount;
    const VkStructureType* pFormatStructTypes;
    uint32_t formatCount;
    const VpVideoFormatDesc* pFormats;

    VpVideoProfileStructChainerDesc chainers;
};

struct VpVariantDesc {
    char blockName[VP_MAX_PROFILE_NAME_SIZE];

    uint32_t instanceExtensionCount;
    const VkExtensionProperties* pInstanceExtensions;

    uint32_t deviceExtensionCount;
    const VkExtensionProperties* pDeviceExtensions;

    uint32_t featureStructTypeCount;
    const VkStructureType* pFeatureStructTypes;
    VpFeatureDesc feature;

    uint32_t propertyStructTypeCount;
    const VkStructureType* pPropertyStructTypes;
    VpPropertyDesc property;

    uint32_t queueFamilyStructTypeCount;
    const VkStructureType* pQueueFamilyStructTypes;
    uint32_t queueFamilyCount;
    const VpQueueFamilyDesc* pQueueFamilies;

    uint32_t formatStructTypeCount;
    const VkStructureType* pFormatStructTypes;
    uint32_t formatCount;
    const VpFormatDesc* pFormats;

    VpStructChainerDesc chainers;

    uint32_t videoProfileCount;
    const VpVideoProfileDesc* pVideoProfiles;
};

struct VpCapabilitiesDesc {
    uint32_t variantCount;
    const VpVariantDesc* pVariants;
};

struct VpProfileDesc {
    VpProfileProperties             props;
    uint32_t                        minApiVersion;

    const detail::VpVariantDesc*    pMergedCapabilities;

    uint32_t                        requiredProfileCount;
    const VpProfileProperties*      pRequiredProfiles;

    uint32_t                        requiredCapabilityCount;
    const VpCapabilitiesDesc*       pRequiredCapabilities;

    uint32_t                        fallbackCount;
    const VpProfileProperties*      pFallbacks;
};

template <typename T>
VPAPI_ATTR bool vpCheckFlags(const T& actual, const uint64_t expected) {
    return (actual & expected) == expected;
}
'''

PRIVATE_IMPL_BODY = '''
VPAPI_ATTR const VpProfileDesc* vpGetProfileDesc(const char profileName[VP_MAX_PROFILE_NAME_SIZE]) {
    for (uint32_t profileIndex = 0; profileIndex < profileCount; ++profileIndex) {
        if (strncmp(profiles[profileIndex].props.profileName, profileName, VP_MAX_PROFILE_NAME_SIZE) == 0) {
            return &profiles[profileIndex];
        }
    }
    return nullptr;
}

VPAPI_ATTR std::vector<VpProfileProperties> GatherProfiles(const VpProfileProperties& profile, const char* pBlockName = nullptr) {
    std::vector<VpProfileProperties> gatheredProfiles;

    if (pBlockName == nullptr) {
        const detail::VpProfileDesc* profileDesc = detail::vpGetProfileDesc(profile.profileName);
        if (profileDesc != nullptr) {
            for (uint32_t profileIndex = 0; profileIndex < profileDesc->requiredProfileCount; ++profileIndex) {
                gatheredProfiles.push_back(profileDesc->pRequiredProfiles[profileIndex]);
            }
        }
    }

    gatheredProfiles.push_back(profile);

    return gatheredProfiles;
}

VPAPI_ATTR bool vpCheckVersion(uint32_t actual, uint32_t expected) {
    uint32_t actualMajor = VK_API_VERSION_MAJOR(actual);
    uint32_t actualMinor = VK_API_VERSION_MINOR(actual);
    uint32_t expectedMajor = VK_API_VERSION_MAJOR(expected);
    uint32_t expectedMinor = VK_API_VERSION_MINOR(expected);
    return actualMajor > expectedMajor || (actualMajor == expectedMajor && actualMinor >= expectedMinor);
}

VPAPI_ATTR bool HasExtension(const std::vector<VkExtensionProperties>& list, const VkExtensionProperties& element) {
    for (std::size_t i = 0, n = list.size(); i < n; ++i) {
        if (strcmp(list[i].extensionName, element.extensionName) == 0) {
            return true;
        }
    }

    return false;
}

VPAPI_ATTR bool CheckExtension(const VkExtensionProperties* supportedProperties, size_t supportedSize, const char *requestedExtension) {
    bool found = false;
    for (size_t i = 0, n = supportedSize; i < n; ++i) {
        if (strcmp(supportedProperties[i].extensionName, requestedExtension) == 0) {
            found = true;
            break;
            // Drivers don't actually update their spec version, so we cannot rely on this
            // if (supportedProperties[i].specVersion >= expectedVersion) found = true;
        }
    }
    VP_DEBUG_COND_MSGF(!found, "Unsupported extension: %s", requestedExtension);
    return found;
}

VPAPI_ATTR bool CheckExtension(const std::vector<const char*>& extensions, const char* extension) {
    for (const char* c : extensions) {
        if (strcmp(c, extension) == 0) {
            return true;
        }
    }
    return false;
}

VPAPI_ATTR void GetExtensions(uint32_t extensionCount, const VkExtensionProperties *pExtensions, std::vector<const char *> &extensions) {
    for (uint32_t ext_index = 0; ext_index < extensionCount; ++ext_index) {
        if (CheckExtension(extensions, pExtensions[ext_index].extensionName)) {
            continue;
        }
        extensions.push_back(pExtensions[ext_index].extensionName);
    }
}

VPAPI_ATTR std::vector<VpBlockProperties> GatherBlocks(
    uint32_t enabledFullProfileCount, const VpProfileProperties* pEnabledFullProfiles,
    uint32_t enabledProfileBlockCount, const VpBlockProperties* pEnabledProfileBlocks) {
    std::vector<VpBlockProperties> results;

    for (std::size_t profile_index = 0; profile_index < enabledFullProfileCount; ++profile_index) {
        const std::vector<VpProfileProperties>& gathered_profiles = GatherProfiles(pEnabledFullProfiles[profile_index]);

        for (std::size_t gathered_index = 0; gathered_index < gathered_profiles.size(); ++gathered_index) {
            VpBlockProperties block{gathered_profiles[gathered_index], 0, ""};
            results.push_back(block);
        }
    }

    for (std::size_t block_index = 0; block_index < enabledProfileBlockCount; ++block_index) {
        results.push_back(pEnabledProfileBlocks[block_index]);
    }

    return results;
}

VPAPI_ATTR VkResult vpGetInstanceProfileSupportSingleProfile(
    uint32_t                                    api_version,
    const std::vector<VkExtensionProperties>&   supported_extensions,
    const VpProfileProperties*                  pProfile,
    VkBool32*                                   pSupported,
    std::vector<VpBlockProperties>&             supportedBlocks,
    std::vector<VpBlockProperties>&             unsupportedBlocks) {
    assert(pProfile != nullptr);

    const detail::VpProfileDesc* pProfileDesc = vpGetProfileDesc(pProfile->profileName);
    if (pProfileDesc == nullptr) {
        *pSupported = VK_FALSE;
        return VK_ERROR_UNKNOWN;
    }

    VpBlockProperties block{*pProfile, api_version};

    if (pProfileDesc->props.specVersion < pProfile->specVersion) {
        *pSupported = VK_FALSE;
        unsupportedBlocks.push_back(block);
    }

    // Required API version is built in root profile, not need to check dependent profile API versions
    if (api_version != 0) {
        if (!vpCheckVersion(api_version, pProfileDesc->minApiVersion)) {
            VP_DEBUG_MSGF("Unsupported Profile API version %u.%u.%u on a Vulkan system with version %u.%u.%u", VK_API_VERSION_MAJOR(pProfileDesc->minApiVersion), VK_API_VERSION_MINOR(pProfileDesc->minApiVersion), VK_API_VERSION_PATCH(pProfileDesc->minApiVersion), VK_API_VERSION_MAJOR(api_version), VK_API_VERSION_MINOR(api_version), VK_API_VERSION_PATCH(api_version));

            *pSupported = VK_FALSE;
            unsupportedBlocks.push_back(block);
        }
    }

    for (uint32_t capability_index = 0; capability_index < pProfileDesc->requiredCapabilityCount; ++capability_index) {
        const VpCapabilitiesDesc& capabilities_desc = pProfileDesc->pRequiredCapabilities[capability_index];

        VkBool32 supported_capabilities = VK_FALSE;
        for (uint32_t variant_index = 0; variant_index < capabilities_desc.variantCount; ++variant_index) {
            const VpVariantDesc& variant_desc = capabilities_desc.pVariants[variant_index];

            VkBool32 supported_variant = VK_TRUE;
            for (uint32_t i = 0; i < variant_desc.instanceExtensionCount; ++i) {
                if (!detail::CheckExtension(supported_extensions.data(), supported_extensions.size(),
                                              variant_desc.pInstanceExtensions[i].extensionName)) {
                    supported_variant = VK_FALSE;
                    memcpy(block.blockName, variant_desc.blockName, VP_MAX_PROFILE_NAME_SIZE * sizeof(char));
                    unsupportedBlocks.push_back(block);
                }
            }

            if (supported_variant == VK_TRUE) {
                supported_capabilities = VK_TRUE;
                memcpy(block.blockName, variant_desc.blockName, VP_MAX_PROFILE_NAME_SIZE * sizeof(char));
                supportedBlocks.push_back(block);
            }
        }

        if (supported_capabilities == VK_FALSE) {
            *pSupported = VK_FALSE;
            return VK_SUCCESS;
        }
    }

    return VK_SUCCESS;
}

enum structure_type {
    STRUCTURE_FEATURE = 0,
    STRUCTURE_PROPERTY,
    STRUCTURE_QUEUE_FAMILY,
    STRUCTURE_FORMAT
};

VPAPI_ATTR VkResult vpGetProfileStructureTypes(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    structure_type                              type,
    uint32_t*                                   pStructureTypeCount,
    VkStructureType*                            pStructureTypes) {
#ifdef VP_USE_OBJECT
    const VpFunctions_T& vp = functions == nullptr ? VpFunctions_T::Get() : *functions;
#else
    const VpFunctions_T& vp = VpFunctions_T::Get();
#endif//VP_USE_OBJECT

    VkResult result_validate = vp.validate(false);
    if (result_validate != VK_SUCCESS) {
        return result_validate;
    }

    VkResult result = pBlockName == nullptr ? VK_SUCCESS : VK_INCOMPLETE;

    std::vector<VkStructureType> results;

    const std::vector<VpProfileProperties>& gathered_profiles = detail::GatherProfiles(*pProfile);

    for (std::size_t profile_index = 0, profile_count = gathered_profiles.size(); profile_index < profile_count; ++profile_index) {
        const detail::VpProfileDesc* profile_desc = detail::vpGetProfileDesc(gathered_profiles[profile_index].profileName);
        if (profile_desc == nullptr) return VK_ERROR_UNKNOWN;

        for (uint32_t capability_index = 0; capability_index < profile_desc->requiredCapabilityCount; ++capability_index) {
            const detail::VpCapabilitiesDesc& cap_desc = profile_desc->pRequiredCapabilities[capability_index];

            for (uint32_t variant_index = 0; variant_index < cap_desc.variantCount; ++variant_index) {
                const detail::VpVariantDesc& variant = cap_desc.pVariants[variant_index];
                if (pBlockName != nullptr) {
                    if (strcmp(variant.blockName, pBlockName) != 0) {
                        continue;
                    }
                    result = VK_SUCCESS;
                }

                uint32_t count = 0;
                const VkStructureType* data = nullptr;

                switch (type) {
                    default:
                    case STRUCTURE_FEATURE:
                        count = variant.featureStructTypeCount;
                        data = variant.pFeatureStructTypes;
                        break;
                    case STRUCTURE_PROPERTY:
                        count = variant.propertyStructTypeCount;
                        data = variant.pPropertyStructTypes;
                        break;
                    case STRUCTURE_QUEUE_FAMILY:
                        count = variant.queueFamilyStructTypeCount;
                        data = variant.pQueueFamilyStructTypes;
                        break;
                    case STRUCTURE_FORMAT:
                        count = variant.formatStructTypeCount;
                        data = variant.pFormatStructTypes;
                        break;
                }

                for (uint32_t type_index = 0; type_index < count; ++type_index) {
                    const VkStructureType dataType = data[type_index];
                    if (std::find(results.begin(), results.end(), dataType) == std::end(results)) {
                        results.push_back(dataType);
                    }
                }
            }
        }
    }

    const uint32_t count = static_cast<uint32_t>(results.size());
    std::sort(results.begin(), results.end());

    if (pStructureTypes == nullptr) {
        *pStructureTypeCount = count;
    } else {
        if (*pStructureTypeCount < count) {
            result = VK_INCOMPLETE;
        } else {
            *pStructureTypeCount = count;
        }

        if (*pStructureTypeCount > 0) {
            memcpy(pStructureTypes, &results[0], *pStructureTypeCount * sizeof(VkStructureType));
        }
    }

    return result;
}

enum ExtensionType {
    EXTENSION_INSTANCE,
    EXTENSION_DEVICE,
};

VPAPI_ATTR VkResult vpGetProfileExtensionProperties(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    ExtensionType                               type,
    uint32_t*                                   pPropertyCount,
    VkExtensionProperties*                      pProperties) {
#ifdef VP_USE_OBJECT
    const VpFunctions_T& vp = functions == nullptr ? VpFunctions_T::Get() : *functions;
#else
    const VpFunctions_T& vp = VpFunctions_T::Get();
#endif//VP_USE_OBJECT

    VkResult result_validate = vp.validate(false);
    if (result_validate != VK_SUCCESS) {
        return result_validate;
    }

    VkResult result = pBlockName == nullptr ? VK_SUCCESS : VK_INCOMPLETE;

    std::vector<VkExtensionProperties> results;

    const std::vector<VpProfileProperties>& gathered_profiles = detail::GatherProfiles(*pProfile, pBlockName);

    for (std::size_t profile_index = 0, profile_count = gathered_profiles.size(); profile_index < profile_count; ++profile_index) {
        const detail::VpProfileDesc* profile_desc = detail::vpGetProfileDesc(gathered_profiles[profile_index].profileName);
        if (profile_desc == nullptr) {
            return VK_ERROR_UNKNOWN;
        }

        for (uint32_t capability_index = 0; capability_index < profile_desc->requiredCapabilityCount; ++capability_index) {
            const detail::VpCapabilitiesDesc& cap_desc = profile_desc->pRequiredCapabilities[capability_index];

            for (uint32_t variant_index = 0; variant_index < cap_desc.variantCount; ++variant_index) {
                const detail::VpVariantDesc& variant = cap_desc.pVariants[variant_index];
                if (pBlockName != nullptr) {
                    if (strcmp(variant.blockName, pBlockName) != 0) {
                        continue;
                    }
                    result = VK_SUCCESS;
                }

                switch (type) {
                    default:
                    case EXTENSION_INSTANCE:
                        for (uint32_t ext_index = 0; ext_index < variant.instanceExtensionCount; ++ext_index) {
                            if (detail::HasExtension(results, variant.pInstanceExtensions[ext_index])) {
                                continue;
                            }
                            results.push_back(variant.pInstanceExtensions[ext_index]);
                        }
                        break;
                    case EXTENSION_DEVICE:
                        for (uint32_t ext_index = 0; ext_index < variant.deviceExtensionCount; ++ext_index) {
                            if (detail::HasExtension(results, variant.pDeviceExtensions[ext_index])) {
                                continue;
                            }
                            results.push_back(variant.pDeviceExtensions[ext_index]);
                        }
                        break;
                }
            }
        }
    }

    const uint32_t count = static_cast<uint32_t>(results.size());

    if (pProperties == nullptr) {
        *pPropertyCount = count;
    } else {
        if (*pPropertyCount < count) {
            result = VK_INCOMPLETE;
        } else {
            *pPropertyCount = count;
        }
        if (*pPropertyCount > 0) {
            memcpy(pProperties, &results[0], *pPropertyCount * sizeof(VkExtensionProperties));
        }
    }

    return result;
}

VPAPI_ATTR VkResult vpGetProfileVideoProfileDesc(
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t                                    videoProfileIndex,
    const detail::VpVideoProfileDesc**          ppVideoProfileDesc) {
    VkResult result = pBlockName == nullptr ? VK_SUCCESS : VK_INCOMPLETE;

    uint32_t curr_base_video_profile_index = 0;

    const std::vector<VpProfileProperties>& gathered_profiles = detail::GatherProfiles(*pProfile);

    for (std::size_t profile_index = 0, profile_count = gathered_profiles.size(); profile_index < profile_count; ++profile_index) {
        const detail::VpProfileDesc* profile_desc = detail::vpGetProfileDesc(gathered_profiles[profile_index].profileName);
        if (profile_desc == nullptr) return VK_ERROR_UNKNOWN;

        for (uint32_t capability_index = 0; capability_index < profile_desc->requiredCapabilityCount; ++capability_index) {
            const detail::VpCapabilitiesDesc& cap_desc = profile_desc->pRequiredCapabilities[capability_index];

            for (uint32_t variant_index = 0; variant_index < cap_desc.variantCount; ++variant_index) {
                const detail::VpVariantDesc& variant = cap_desc.pVariants[variant_index];
                if (pBlockName != nullptr) {
                    if (strcmp(variant.blockName, pBlockName) != 0) {
                        continue;
                    }
                    result = VK_SUCCESS;
                }

                if (videoProfileIndex < curr_base_video_profile_index + variant.videoProfileCount) {
                    *ppVideoProfileDesc = &variant.pVideoProfiles[videoProfileIndex - curr_base_video_profile_index];
                    return result;
                } else {
                    curr_base_video_profile_index += variant.videoProfileCount;
                }
            }
        }
    }

    *ppVideoProfileDesc = nullptr;
    return VK_ERROR_UNKNOWN;
}
'''

PUBLIC_IMPL_BODY = '''
#ifdef VP_USE_OBJECT

VPAPI_ATTR VkResult vpCreateFunctions(
    const VpFunctionsCreateInfo*                pFunctionsCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VpFunctions*                                pFunctions) {
    (void)pAllocator;
    (void)pFunctionsCreateInfo;
    VpFunctions_T* functions = new (std::nothrow) VpFunctions_T();
    *pFunctions = functions;
    if (!functions) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

#define VP_COPY_IF_NOT_NULL(funcName) \
    if(pFunctionsCreateInfo->funcName != nullptr) \
        functions->funcName = pFunctionsCreateInfo->funcName;

    VP_COPY_IF_NOT_NULL(GetInstanceProcAddr);

    VP_COPY_IF_NOT_NULL(EnumerateInstanceVersion);
    VP_COPY_IF_NOT_NULL(EnumerateInstanceExtensionProperties);
    VP_COPY_IF_NOT_NULL(EnumerateDeviceExtensionProperties);

    VP_COPY_IF_NOT_NULL(GetPhysicalDeviceFeatures2);
    VP_COPY_IF_NOT_NULL(GetPhysicalDeviceProperties2);
    VP_COPY_IF_NOT_NULL(GetPhysicalDeviceFormatProperties2);
    VP_COPY_IF_NOT_NULL(GetPhysicalDeviceQueueFamilyProperties2);

    VP_COPY_IF_NOT_NULL(CreateInstance);
    VP_COPY_IF_NOT_NULL(CreateDevice);

#undef VP_COPY_IF_NOT_NULL

    return functions->validate();
}

/// Destroys allocator object.
VPAPI_ATTR void vpDestroyFunctions(
    VpFunctions                                 functions,
    const VkAllocationCallbacks*                pAllocator) {
    (void)pAllocator;
    
    delete functions;
}

#endif//VP_USE_OBJECT

/// Helper function to initialize VpFunctions instance with global Vulkan functions
VPAPI_ATTR VkResult vpInitializeGlobalFunctions(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    PFN_vkGetInstanceProcAddr                   GetInstanceProcAddr) {

#ifdef VP_USE_OBJECT
    VpFunctions_T& vp = functions == nullptr ? VpFunctions_T::Get() : *functions;
#else
    VpFunctions_T& vp = VpFunctions_T::Get();
#endif//VP_USE_OBJECT

    if (GetInstanceProcAddr == nullptr) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    vp.GetInstanceProcAddr = GetInstanceProcAddr;

#define VP_FETCH_FUNC(memberName, functionNameString) \
    vp.memberName = (PFN_vk##memberName)vp.GetInstanceProcAddr(nullptr, functionNameString)

    VP_FETCH_FUNC(EnumerateInstanceVersion, "vkEnumerateInstanceVersion");
    VP_FETCH_FUNC(EnumerateInstanceExtensionProperties, "vkEnumerateInstanceExtensionProperties");
    VP_FETCH_FUNC(CreateInstance, "vkCreateInstance");

#undef VP_FETCH_FUNC

    return vp.validate();
}

/// Initializes capabilities with instance functions
VPAPI_ATTR VkResult vpInitializeInstanceFunctions(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    VkInstance                                  instance,
    VpInstanceFunctionsLoadFlags                flags) {
#ifdef VP_USE_OBJECT
    VpFunctions_T& vp = functions == nullptr ? VpFunctions_T::Get() : *functions;
#else
    VpFunctions_T& vp = VpFunctions_T::Get();
#endif//VP_USE_OBJECT

#define VP_FETCH_FUNC(memberName, functionNameString) \
    if (((flags & VP_INSTANCE_FUNCTIONS_LOAD_MISSING_ONLY_BIT) && vp.memberName == nullptr) || !(flags & VP_INSTANCE_FUNCTIONS_LOAD_MISSING_ONLY_BIT)) \
        vp.memberName = (PFN_vk##memberName)vp.GetInstanceProcAddr(instance, functionNameString);

    VP_FETCH_FUNC(EnumerateDeviceExtensionProperties, "vkEnumerateDeviceExtensionProperties");
    VP_FETCH_FUNC(GetPhysicalDeviceFeatures2, "vkGetPhysicalDeviceFeatures2");
    if (!vp.GetPhysicalDeviceFeatures2 && (flags & VP_INSTANCE_FUNCTIONS_LOAD_KHR_GET_PHYSICAL_DEVICE_PROPERTIES2_BIT)) {
        VP_FETCH_FUNC(GetPhysicalDeviceFeatures2, "vkGetPhysicalDeviceFeatures2KHR");
    }

    VP_FETCH_FUNC(GetPhysicalDeviceProperties2, "vkGetPhysicalDeviceProperties2");
    if (!vp.GetPhysicalDeviceProperties2 && (flags & VP_INSTANCE_FUNCTIONS_LOAD_KHR_GET_PHYSICAL_DEVICE_PROPERTIES2_BIT)) {
        VP_FETCH_FUNC(GetPhysicalDeviceProperties2, "vkGetPhysicalDeviceProperties2KHR");
    }

    VP_FETCH_FUNC(GetPhysicalDeviceFormatProperties2, "vkGetPhysicalDeviceFormatProperties2");
    if (!vp.GetPhysicalDeviceFormatProperties2 && (flags & VP_INSTANCE_FUNCTIONS_LOAD_KHR_GET_PHYSICAL_DEVICE_PROPERTIES2_BIT)) {
        VP_FETCH_FUNC(GetPhysicalDeviceFormatProperties2, "vkGetPhysicalDeviceFormatProperties2KHR");
    }

    VP_FETCH_FUNC(GetPhysicalDeviceQueueFamilyProperties2, "vkGetPhysicalDeviceQueueFamilyProperties2");
    if (!vp.GetPhysicalDeviceQueueFamilyProperties2 && (flags & VP_INSTANCE_FUNCTIONS_LOAD_KHR_GET_PHYSICAL_DEVICE_PROPERTIES2_BIT)) {
        VP_FETCH_FUNC(GetPhysicalDeviceQueueFamilyProperties2, "vkGetPhysicalDeviceQueueFamilyProperties2KHR");
    }

    VP_FETCH_FUNC(CreateDevice, "vkCreateDevice");
#undef VP_FETCH_FUNC

    // Validate the instance functions are loaded correctly
    if (vp.EnumerateDeviceExtensionProperties == nullptr ||
        vp.CreateDevice == nullptr) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    bool requiresProperties2 = (flags & VP_INSTANCE_FUNCTIONS_LOAD_KHR_GET_PHYSICAL_DEVICE_PROPERTIES2_BIT);

    if (vp.GetPhysicalDeviceFeatures2 == nullptr ||
        vp.GetPhysicalDeviceProperties2 == nullptr ||
        vp.GetPhysicalDeviceFormatProperties2 == nullptr ||
        vp.GetPhysicalDeviceQueueFamilyProperties2 == nullptr) {
        return requiresProperties2 ? VK_ERROR_INITIALIZATION_FAILED : VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    return VK_SUCCESS;
}

VPAPI_ATTR VkResult vpGetProfiles(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    uint32_t*                                   pPropertyCount,
    VpProfileProperties*                        pProperties) {
#ifdef VP_USE_OBJECT
    const VpFunctions_T& vp = functions == nullptr ? VpFunctions_T::Get() : *functions;
#else
    const VpFunctions_T& vp = VpFunctions_T::Get();
#endif//VP_USE_OBJECT

    VkResult result_validate = vp.validate(false);
    if (result_validate != VK_SUCCESS) {
        return result_validate;
    }

    VkResult result = VK_SUCCESS;

    if (pProperties == nullptr) {
        *pPropertyCount = detail::profileCount;
    } else {
        if (*pPropertyCount < detail::profileCount) {
            result = VK_INCOMPLETE;
        } else {
            *pPropertyCount = detail::profileCount;
        }
        for (uint32_t property_index = 0; property_index < *pPropertyCount; ++property_index) {
            pProperties[property_index] = detail::profiles[property_index].props;
        }
    }
    return result;
}

VPAPI_ATTR VkResult vpGetProfileRequiredProfiles(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    uint32_t*                                   pPropertyCount,
    VpProfileProperties*                        pProperties) {
#ifdef VP_USE_OBJECT
    const VpFunctions_T& vp = functions == nullptr ? VpFunctions_T::Get() : *functions;
#else
    const VpFunctions_T& vp = VpFunctions_T::Get();
#endif//VP_USE_OBJECT

    VkResult result_validate = vp.validate(false);
    if (result_validate != VK_SUCCESS) {
        return result_validate;
    }

    VkResult result = VK_SUCCESS;

    const detail::VpProfileDesc* desc = detail::vpGetProfileDesc(pProfile->profileName);
    if (desc == nullptr) {
        return VK_ERROR_UNKNOWN;
    }

    if (pProperties == nullptr) {
        *pPropertyCount = desc->requiredProfileCount;
    } else {
        if (*pPropertyCount < desc->requiredProfileCount) {
            result = VK_INCOMPLETE;
        } else {
            *pPropertyCount = desc->requiredProfileCount;
        }
        for (uint32_t property_index = 0; property_index < *pPropertyCount; ++property_index) {
            pProperties[property_index] = desc->pRequiredProfiles[property_index];
        }
    }
    return result;
}

VPAPI_ATTR uint32_t vpGetProfileAPIVersion(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile) {
#ifdef VP_USE_OBJECT
    const VpFunctions_T& vp = functions == nullptr ? VpFunctions_T::Get() : *functions;
#else
    const VpFunctions_T& vp = VpFunctions_T::Get();
#endif//VP_USE_OBJECT

    VkResult result_validate = vp.validate(false);
    if (result_validate != VK_SUCCESS) {
        return result_validate;
    }

    const std::vector<VpProfileProperties>& gathered_profiles = detail::GatherProfiles(*pProfile, nullptr);

    uint32_t major = 0;
    uint32_t minor = 0;
    uint32_t patch = 0;

    for (std::size_t profile_index = 0, profile_count = gathered_profiles.size(); profile_index < profile_count; ++profile_index) {
        const detail::VpProfileDesc* desc = detail::vpGetProfileDesc(gathered_profiles[profile_index].profileName);
        if (desc == nullptr) {
            return 0;
        }

        major = std::max<uint32_t>(major, VK_API_VERSION_MAJOR(desc->minApiVersion));
        minor = std::max<uint32_t>(minor, VK_API_VERSION_MINOR(desc->minApiVersion));
        patch = std::max<uint32_t>(patch, VK_API_VERSION_PATCH(desc->minApiVersion));
    }

    return VK_MAKE_API_VERSION(0, major, minor, patch);
}

VPAPI_ATTR VkResult vpGetProfileFallbacks(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    uint32_t*                                   pPropertyCount,
    VpProfileProperties*                        pProperties) {
#ifdef VP_USE_OBJECT
    const VpFunctions_T& vp = functions == nullptr ? VpFunctions_T::Get() : *functions;
#else
    const VpFunctions_T& vp = VpFunctions_T::Get();
#endif//VP_USE_OBJECT

    VkResult result_validate = vp.validate(true);
    if (result_validate != VK_SUCCESS) {
        return result_validate;
    }

    VkResult result = VK_SUCCESS;

    const detail::VpProfileDesc* desc = detail::vpGetProfileDesc(pProfile->profileName);
    if (desc == nullptr) {
        return VK_ERROR_UNKNOWN;
    }

    if (pProperties == nullptr) {
        *pPropertyCount = desc->fallbackCount;
    } else {
        if (*pPropertyCount < desc->fallbackCount) {
            result = VK_INCOMPLETE;
        } else {
            *pPropertyCount = desc->fallbackCount;
        }
        for (uint32_t i = 0; i < *pPropertyCount; ++i) {
            pProperties[i] = desc->pFallbacks[i];
        }
    }
    return result;
}

VPAPI_ATTR VkResult vpHasMultipleVariantsProfile(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    VkBool32*                                   pHasMultipleVariants) {
#ifdef VP_USE_OBJECT
    const VpFunctions_T& vp = functions == nullptr ? VpFunctions_T::Get() : *functions;
#else
    const VpFunctions_T& vp = VpFunctions_T::Get();
#endif//VP_USE_OBJECT

    VkResult result_validate = vp.validate(true);
    if (result_validate != VK_SUCCESS) {
        return result_validate;
    }

    const std::vector<VpProfileProperties>& gathered_profiles = detail::GatherProfiles(*pProfile, nullptr);

    for (std::size_t profile_index = 0, profile_count = gathered_profiles.size(); profile_index < profile_count; ++profile_index) {
        const detail::VpProfileDesc* desc = detail::vpGetProfileDesc(gathered_profiles[profile_index].profileName);
        if (desc == nullptr) {
            return VK_ERROR_UNKNOWN;
        }

        for (uint32_t caps_index = 0, caps_count = desc->requiredCapabilityCount; caps_index < caps_count; ++caps_index) {
            if (desc->pRequiredCapabilities[caps_index].variantCount > 1) {
                *pHasMultipleVariants = VK_TRUE;
                return VK_SUCCESS;
            }
        }
    }

    *pHasMultipleVariants = VK_FALSE;
    return VK_SUCCESS;
}

VPAPI_ATTR VkResult vpGetInstanceProfileVariantsSupport(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const char*                         pLayerName,
    const VpProfileProperties*          pProfile,
    VkBool32*                           pSupported,
    uint32_t*                           pPropertyCount,
    VpBlockProperties*                  pProperties) {
#ifdef VP_USE_OBJECT
    const VpFunctions_T& vp = functions == nullptr ? VpFunctions_T::Get() : *functions;
#else
    const VpFunctions_T& vp = VpFunctions_T::Get();
#endif//VP_USE_OBJECT

    VkResult result_validate = vp.validate(false);
    if (result_validate != VK_SUCCESS) {
        return result_validate;
    }

    VkResult result = VK_SUCCESS;

    uint32_t api_version = VK_API_VERSION_1_0;
    PFN_vkEnumerateInstanceVersion pfnEnumerateInstanceVersion = vp.EnumerateInstanceVersion;
    if (pfnEnumerateInstanceVersion != nullptr) {
        result = pfnEnumerateInstanceVersion(&api_version);
        if (result != VK_SUCCESS) {
            *pSupported = VK_FALSE;
            return result;
        } /* else {
            VP_DEBUG_MSGF("Instance API version %u.%u.%u", VK_API_VERSION_MAJOR(api_version), VK_API_VERSION_MINOR(api_version), VK_API_VERSION_PATCH(api_version));
        } */
    }

    uint32_t supported_instance_extension_count = 0;
    result = vp.EnumerateInstanceExtensionProperties(pLayerName, &supported_instance_extension_count, nullptr);
    if (result != VK_SUCCESS) {
        *pSupported = VK_FALSE;
        return result;
    }
    std::vector<VkExtensionProperties> supported_instance_extensions;
    if (supported_instance_extension_count > 0) {
        supported_instance_extensions.resize(supported_instance_extension_count);
    }
    result = vp.EnumerateInstanceExtensionProperties(pLayerName, &supported_instance_extension_count, supported_instance_extensions.data());
    if (result != VK_SUCCESS) {
        *pSupported = VK_FALSE;
        return result;
    }

    VkBool32 supported = VK_TRUE;

    // We require VK_KHR_get_physical_device_properties2 if we are on Vulkan 1.0
    if (api_version < VK_API_VERSION_1_1) {
        bool foundGPDP2 = false;
        for (size_t ext_index = 0, ext_count = supported_instance_extensions.size(); ext_index < ext_count; ++ext_index) {
            if (strcmp(supported_instance_extensions[ext_index].extensionName, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) == 0) {
                foundGPDP2 = true;
                break;
            }
        }
        if (!foundGPDP2) {
            VP_DEBUG_MSG("Unsupported mandatory extension VK_KHR_get_physical_device_properties2 on Vulkan 1.0");
            supported = VK_FALSE;
        }
    }

    const detail::VpProfileDesc* pProfileDesc = detail::vpGetProfileDesc(pProfile->profileName);
    if (pProfileDesc == nullptr) return VK_ERROR_UNKNOWN;

    std::vector<VpBlockProperties> supported_blocks;
    std::vector<VpBlockProperties> unsupported_blocks;

    result = detail::vpGetInstanceProfileSupportSingleProfile(api_version, supported_instance_extensions, pProfile, &supported, supported_blocks, unsupported_blocks);
    if (result != VK_SUCCESS) {
        *pSupported = supported;
        return result;
    }

    for (std::size_t required_profile_index = 0; required_profile_index < pProfileDesc->requiredProfileCount; ++required_profile_index) {
        result = detail::vpGetInstanceProfileSupportSingleProfile(0, supported_instance_extensions, &pProfileDesc->pRequiredProfiles[required_profile_index], &supported, supported_blocks, unsupported_blocks);
        if (result != VK_SUCCESS) {
            *pSupported = supported;
            return result;
        }
    }

    const std::vector<VpBlockProperties>& blocks = supported ? supported_blocks : unsupported_blocks;

    if (pProperties == nullptr) {
        *pPropertyCount = static_cast<uint32_t>(blocks.size());
    } else {
        if (*pPropertyCount < static_cast<uint32_t>(blocks.size())) {
            result = VK_INCOMPLETE;
        } else {
            *pPropertyCount = static_cast<uint32_t>(blocks.size());
        }
        for (uint32_t block_index = 0, block_count = static_cast<uint32_t>(blocks.size()); block_index < block_count; ++block_index) {
            pProperties[block_index] = blocks[block_index];
        }
    }

    *pSupported = supported;
    return result;
}

VPAPI_ATTR VkResult vpGetInstanceProfileSupport(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const char*                                 pLayerName,
    const VpProfileProperties*                  pProfile,
    VkBool32*                                   pSupported) {
#ifdef VP_USE_OBJECT
    const VpFunctions_T& vp = functions == nullptr ? VpFunctions_T::Get() : *functions;
#else
    const VpFunctions_T& vp = VpFunctions_T::Get();
#endif//VP_USE_OBJECT

    VkResult result_validate = vp.validate(false);
    if (result_validate != VK_SUCCESS) {
        return result_validate;
    }

    uint32_t count = 0;
    return vpGetInstanceProfileVariantsSupport(
#ifdef VP_USE_OBJECT
        functions,
#endif//VP_USE_OBJECT
        pLayerName, pProfile, pSupported, &count, nullptr);
}

VPAPI_ATTR VkResult vpCreateInstance(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpInstanceCreateInfo*                 pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkInstance*                                 pInstance) {
#ifdef VP_USE_OBJECT
    const VpFunctions_T& vp = functions == nullptr ? VpFunctions_T::Get() : *functions;
#else
    const VpFunctions_T& vp = VpFunctions_T::Get();
#endif//VP_USE_OBJECT

    VkResult result_validate = vp.validate(false);
    if (result_validate != VK_SUCCESS) {
        return result_validate;
    }

    if (pCreateInfo == nullptr || pInstance == nullptr) {
        return vp.CreateInstance(pCreateInfo == nullptr ? nullptr : pCreateInfo->pCreateInfo, pAllocator, pInstance);
    }

    const std::vector<VpBlockProperties>& blocks = detail::GatherBlocks(
        pCreateInfo->enabledFullProfileCount, pCreateInfo->pEnabledFullProfiles,
        pCreateInfo->enabledProfileBlockCount, pCreateInfo->pEnabledProfileBlocks);

    std::vector<const char*> extensions;
    for (std::uint32_t ext_index = 0, ext_count = pCreateInfo->pCreateInfo->enabledExtensionCount; ext_index < ext_count; ++ext_index) {
        extensions.push_back(pCreateInfo->pCreateInfo->ppEnabledExtensionNames[ext_index]);
    }

    for (std::size_t block_index = 0, block_count = blocks.size(); block_index < block_count; ++block_index) {
        const detail::VpProfileDesc* profile_desc = detail::vpGetProfileDesc(blocks[block_index].profiles.profileName);
        if (profile_desc == nullptr) {
            return VK_ERROR_UNKNOWN;
        }

        for (std::size_t caps_index = 0, caps_count = profile_desc->requiredCapabilityCount; caps_index < caps_count; ++caps_index) {
            const detail::VpCapabilitiesDesc* caps_desc = &profile_desc->pRequiredCapabilities[caps_index];

            for (std::size_t variant_index = 0, variant_count = caps_desc->variantCount; variant_index < variant_count; ++variant_index) {
                const detail::VpVariantDesc* variant = &caps_desc->pVariants[variant_index];

                if (strcmp(blocks[block_index].blockName, "") != 0) {
                    if (strcmp(variant->blockName, blocks[block_index].blockName) != 0) {
                        continue;
                    }
                }

                detail::GetExtensions(variant->instanceExtensionCount, variant->pInstanceExtensions, extensions);
            }
        }
    }

    VkApplicationInfo appInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO};
    if (pCreateInfo->pCreateInfo->pApplicationInfo != nullptr) {
        appInfo = *pCreateInfo->pCreateInfo->pApplicationInfo;
    } else if (!blocks.empty()) {
        appInfo.apiVersion = vpGetProfileAPIVersion(
#ifdef VP_USE_OBJECT
            functions,
#endif//VP_USE_OBJECT
            &blocks[0].profiles);
    }

    VkInstanceCreateInfo createInfo = *pCreateInfo->pCreateInfo;
    createInfo.pApplicationInfo = &appInfo;

    bool use_gpdp2 = false;

    // Need to include VK_KHR_get_physical_device_properties2 if we are on Vulkan 1.0
    if (createInfo.pApplicationInfo->apiVersion < VK_API_VERSION_1_1) {
        bool foundGPDP2 = false;
        for (size_t ext_index = 0, ext_count = extensions.size(); ext_index < ext_count; ++ext_index) {
            if (strcmp(extensions[ext_index], VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) == 0) {
                foundGPDP2 = true;
                break;
            }
        }
        if (!foundGPDP2) {
            extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
            use_gpdp2 = true;
        }
    }

#ifdef __APPLE__
    bool has_portability_ext = false;
    for (std::size_t ext_index = 0, ext_count = extensions.size(); ext_index < ext_count; ++ext_index) {
        if (strcmp(extensions[ext_index], VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0) {
            has_portability_ext = true;
            break;
        }
    }

    if (!has_portability_ext) {
        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    }

    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

    if (!extensions.empty()) {
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
    }

    VkResult result = vp.CreateInstance(&createInfo, pAllocator, pInstance);

    if (result == VK_SUCCESS) {
        VpInstanceFunctionsLoadFlags flags = VP_INSTANCE_FUNCTIONS_LOAD_MISSING_ONLY_BIT;
        if (use_gpdp2) {
            flags |= VP_INSTANCE_FUNCTIONS_LOAD_KHR_GET_PHYSICAL_DEVICE_PROPERTIES2_BIT;
        }

        result = vpInitializeInstanceFunctions(
#ifdef VP_USE_OBJECT
            functions,
#endif//VP_USE_OBJECT
            *pInstance, flags);
    }

    return result;
}

VPAPI_ATTR VkResult vpGetPhysicalDeviceProfileVariantsSupport(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    VkInstance                                  instance,
    VkPhysicalDevice                            physicalDevice,
    const VpProfileProperties*                  pProfile,
    VkBool32*                                   pSupported,
    uint32_t*                                   pPropertyCount,
    VpBlockProperties*                          pProperties) {
#ifdef VP_USE_OBJECT
    const VpFunctions_T& vp = functions == nullptr ? VpFunctions_T::Get() : *functions;
#else
    const VpFunctions_T& vp = VpFunctions_T::Get();
#endif//VP_USE_OBJECT

    VkResult result_validate = vp.validate(true);
    if (result_validate != VK_SUCCESS) {
        return result_validate;
    }

    VkResult result = VK_SUCCESS;

    uint32_t supported_device_extension_count = 0;
    result = vp.EnumerateDeviceExtensionProperties(physicalDevice, nullptr, &supported_device_extension_count, nullptr);
    if (result != VK_SUCCESS) {
        return result;
    }
    std::vector<VkExtensionProperties> supported_device_extensions;
    if (supported_device_extension_count > 0) {
        supported_device_extensions.resize(supported_device_extension_count);
    }
    result = vp.EnumerateDeviceExtensionProperties(physicalDevice, nullptr, &supported_device_extension_count, supported_device_extensions.data());
    if (result != VK_SUCCESS) {
        return result;
    }

    // Workaround old loader bug where count could be smaller on the second call to vkEnumerateDeviceExtensionProperties
    if (supported_device_extension_count > 0) {
        supported_device_extensions.resize(supported_device_extension_count);
    }

    {
        const detail::VpProfileDesc* pProfileDesc = detail::vpGetProfileDesc(pProfile->profileName);
        if (pProfileDesc == nullptr) {
            return VK_ERROR_UNKNOWN;
        }
    }

    struct GPDP2EntryPoints {
        PFN_vkGetPhysicalDeviceFeatures2KHR                 pfnGetPhysicalDeviceFeatures2;
        PFN_vkGetPhysicalDeviceProperties2KHR               pfnGetPhysicalDeviceProperties2;
        PFN_vkGetPhysicalDeviceFormatProperties2KHR         pfnGetPhysicalDeviceFormatProperties2;
        PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR    pfnGetPhysicalDeviceQueueFamilyProperties2;
    };

#ifdef VK_KHR_video_queue
    struct VideoInfo {
        PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR         pfnGetPhysicalDeviceVideoCapabilitiesKHR;
        PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR     pfnGetPhysicalDeviceVideoFormatPropertiesKHR;
        const detail::VpVideoProfileDesc*                   pProfileDesc;
        VkVideoProfileInfoKHR                               profileInfo;
        VkPhysicalDeviceVideoFormatInfoKHR                  formatInfo;
        bool                                                supportedProfile;
        uint32_t                                            matchingProfiles;
    };
#endif  // VK_KHR_video_queue

    std::vector<VpBlockProperties> supported_blocks;
    std::vector<VpBlockProperties> unsupported_blocks;

    struct UserData {
        VkPhysicalDevice physicalDevice;
        std::vector<VpBlockProperties>& supported_blocks;
        std::vector<VpBlockProperties>& unsupported_blocks;
        const detail::VpVariantDesc* variant;
        GPDP2EntryPoints gpdp2;
#ifdef VK_KHR_video_queue
        VideoInfo video;
#endif  // VK_KHR_video_queue
        uint32_t index;
        detail::PFN_vpStructChainerCb pfnCb;
        bool supported;
    } userData{physicalDevice, supported_blocks, unsupported_blocks};

    userData.gpdp2.pfnGetPhysicalDeviceFeatures2 = vp.GetPhysicalDeviceFeatures2;
    userData.gpdp2.pfnGetPhysicalDeviceProperties2 = vp.GetPhysicalDeviceProperties2;
    userData.gpdp2.pfnGetPhysicalDeviceFormatProperties2 = vp.GetPhysicalDeviceFormatProperties2;
    userData.gpdp2.pfnGetPhysicalDeviceQueueFamilyProperties2 = vp.GetPhysicalDeviceQueueFamilyProperties2;

    if (userData.gpdp2.pfnGetPhysicalDeviceFeatures2 == nullptr ||
        userData.gpdp2.pfnGetPhysicalDeviceProperties2 == nullptr ||
        userData.gpdp2.pfnGetPhysicalDeviceFormatProperties2 == nullptr ||
        userData.gpdp2.pfnGetPhysicalDeviceQueueFamilyProperties2 == nullptr) {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

#ifdef VK_KHR_video_queue
    PFN_vkGetInstanceProcAddr gipa = vp.GetInstanceProcAddr;
    userData.video.pfnGetPhysicalDeviceVideoCapabilitiesKHR =
        (PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR)gipa(instance, "vkGetPhysicalDeviceVideoCapabilitiesKHR");
    userData.video.pfnGetPhysicalDeviceVideoFormatPropertiesKHR =
        (PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR)gipa(instance, "vkGetPhysicalDeviceVideoFormatPropertiesKHR");
#endif  // VK_KHR_video_queue

    bool supported = true;

    const std::vector<VpProfileProperties>& gathered_profiles = detail::GatherProfiles(*pProfile);

    for (std::size_t profile_index = 0, profile_count = gathered_profiles.size(); profile_index < profile_count; ++profile_index) {
        const char* profile_name = gathered_profiles[profile_index].profileName;

        const detail::VpProfileDesc* profile_desc = detail::vpGetProfileDesc(profile_name);
        if (profile_desc == nullptr) {
            return VK_ERROR_UNKNOWN;
        }

        bool supported_profile = true;

        if (profile_desc->props.specVersion < gathered_profiles[profile_index].specVersion) {
            VP_DEBUG_MSGF("Unsupported requested %s profile version: %u, profile supported at version %u", profile_name, profile_desc->props.specVersion, pProfile->specVersion);
            supported_profile = false;
        }

        VpBlockProperties block{gathered_profiles[profile_index], profile_desc->minApiVersion};

        {
            VkPhysicalDeviceProperties2KHR properties2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR };
            userData.gpdp2.pfnGetPhysicalDeviceProperties2(physicalDevice, &properties2);
            if (!detail::vpCheckVersion(properties2.properties.apiVersion, profile_desc->minApiVersion)) {
                VP_DEBUG_MSGF("Unsupported API version: %u.%u.%u", VK_API_VERSION_MAJOR(profile_desc->minApiVersion), VK_API_VERSION_MINOR(profile_desc->minApiVersion), VK_API_VERSION_PATCH(profile_desc->minApiVersion));
                supported_profile = false;
            }
        }

        for (uint32_t required_capability_index = 0; required_capability_index < profile_desc->requiredCapabilityCount; ++required_capability_index) {
            const detail::VpCapabilitiesDesc* required_capabilities = &profile_desc->pRequiredCapabilities[required_capability_index];

            bool supported_block = false;

            for (uint32_t variant_index = 0; variant_index < required_capabilities->variantCount; ++variant_index) {
                const detail::VpVariantDesc& variant_desc = required_capabilities->pVariants[variant_index];

                bool supported_variant = true;

                for (uint32_t ext_index = 0; ext_index < variant_desc.deviceExtensionCount; ++ext_index) {
                    const char *requested_extension = variant_desc.pDeviceExtensions[ext_index].extensionName;
                    if (!detail::CheckExtension(supported_device_extensions.data(), supported_device_extensions.size(), requested_extension)) {
                        supported_variant = false;
                    }
                }

                userData.variant = &variant_desc;

                if (supported_variant) {
                    VkPhysicalDeviceFeatures2KHR features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR };
                    userData.variant->chainers.pfnFeature(
                        static_cast<VkBaseOutStructure*>(static_cast<void*>(&features)), &userData,
                        [](VkBaseOutStructure* p, void* pUser) {
                            UserData* pUserData = static_cast<UserData*>(pUser);
                            pUserData->gpdp2.pfnGetPhysicalDeviceFeatures2(
                                pUserData->physicalDevice,
                                static_cast<VkPhysicalDeviceFeatures2KHR*>(static_cast<void*>(p)));

                            pUserData->supported = true;
                            while (p != nullptr) {
                                if (!pUserData->variant->feature.pfnComparator(p)) {
                                    pUserData->supported = false;
                                }
                                p = p->pNext;
                            }
                        }
                    );
                    if (!userData.supported) {
                        supported_variant = false;
                    }
                }

                if (supported_variant) {
                    VkPhysicalDeviceProperties2KHR device_properties2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR };
                    userData.variant->chainers.pfnProperty(
                        static_cast<VkBaseOutStructure*>(static_cast<void*>(&device_properties2)), &userData,
                        [](VkBaseOutStructure* p, void* pUser) {
                            UserData* pUserData = static_cast<UserData*>(pUser);
                            pUserData->gpdp2.pfnGetPhysicalDeviceProperties2(
                                pUserData->physicalDevice,
                                static_cast<VkPhysicalDeviceProperties2KHR*>(static_cast<void*>(p)));

                            pUserData->supported = true;
                            while (p != nullptr) {
                                if (!pUserData->variant->property.pfnComparator(p)) {
                                    pUserData->supported = false;
                                }
                                p = p->pNext;
                            }
                        }
                    );
                    if (!userData.supported) {
                        supported_variant = false;
                    }
                }

                if (supported_variant && userData.variant->queueFamilyCount > 0) {
                    uint32_t queue_family_count = 0;
                    userData.gpdp2.pfnGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &queue_family_count, nullptr);
                    std::vector<VkQueueFamilyProperties2KHR> queueFamilyProps(queue_family_count, { VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2_KHR });
                    userData.variant->chainers.pfnQueueFamily(
                        queue_family_count, static_cast<VkBaseOutStructure*>(static_cast<void*>(queueFamilyProps.data())), &userData,
                        [](uint32_t queue_family_count, VkBaseOutStructure* pBaseArray, void* pUser) {
                            UserData* pUserData = static_cast<UserData*>(pUser);
                            VkQueueFamilyProperties2KHR* pArray = static_cast<VkQueueFamilyProperties2KHR*>(static_cast<void*>(pBaseArray));
                            pUserData->gpdp2.pfnGetPhysicalDeviceQueueFamilyProperties2(pUserData->physicalDevice, &queue_family_count, pArray);
                            pUserData->supported = true;
                            for (uint32_t profile_qf_idx = 0; profile_qf_idx < pUserData->variant->queueFamilyCount; ++profile_qf_idx) {
                                bool found_matching = false;
                                for (uint32_t queue_family_index = 0; queue_family_index < queue_family_count; ++queue_family_index) {
                                    bool this_matches = true;
                                    VkBaseOutStructure* p = static_cast<VkBaseOutStructure*>(static_cast<void*>(&pArray[queue_family_index]));
                                    while (p != nullptr) {
                                        if (!pUserData->variant->pQueueFamilies[profile_qf_idx].pfnComparator(p)) {
                                            this_matches = false;
                                        }
                                        p = p->pNext;
                                    }
                                    if (this_matches) {
                                        found_matching = true;
                                        break;
                                    }
                                }
                                if (!found_matching) {
                                    pUserData->supported = false;
                                    break;
                                }
                            }
                        }
                    );
                    if (!userData.supported) {
                        supported_variant = false;
                    }
                }

                for (uint32_t format_index = 0; supported_variant && (format_index < userData.variant->formatCount); ++format_index) {
                    userData.index = format_index;
                    VkFormatProperties2KHR format_properties2{ VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR };
                    userData.variant->chainers.pfnFormat(
                        static_cast<VkBaseOutStructure*>(static_cast<void*>(&format_properties2)), &userData,
                        [](VkBaseOutStructure* p, void* pUser) {
                            UserData* pUserData = static_cast<UserData*>(pUser);
                            pUserData->gpdp2.pfnGetPhysicalDeviceFormatProperties2(
                                pUserData->physicalDevice,
                                pUserData->variant->pFormats[pUserData->index].format,
                                static_cast<VkFormatProperties2KHR*>(static_cast<void*>(p)));
                            pUserData->supported = true;
                            while (p != nullptr) {
                                if (!pUserData->variant->pFormats[pUserData->index].pfnComparator(p)) {
                                    pUserData->supported = false;
                                }
                                p = p->pNext;
                            }
                        }
                    );
                    if (!userData.supported) {
                        supported_variant = false;
                    }
                }

#ifdef VK_KHR_video_queue
                if (supported_variant && (userData.variant->videoProfileCount > 0)) {
                    VkVideoProfileListInfoKHR profile_list{ VK_STRUCTURE_TYPE_VIDEO_PROFILE_LIST_INFO_KHR };
                    profile_list.profileCount = 1;
                    profile_list.pProfiles = &userData.video.profileInfo;
                    userData.video.formatInfo.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_FORMAT_INFO_KHR;
                    userData.video.formatInfo.pNext = &profile_list;

                    if (userData.video.pfnGetPhysicalDeviceVideoCapabilitiesKHR != nullptr &&
                        userData.video.pfnGetPhysicalDeviceVideoFormatPropertiesKHR != nullptr) {
                        for (uint32_t video_profile_index = 0; video_profile_index < userData.variant->videoProfileCount; ++video_profile_index) {
                            userData.video.profileInfo = VkVideoProfileInfoKHR{ VK_STRUCTURE_TYPE_VIDEO_PROFILE_INFO_KHR };
                            userData.video.pProfileDesc = &userData.variant->pVideoProfiles[video_profile_index];
                            userData.supported = true;
                            userData.video.matchingProfiles = 0;

                            detail::vpForEachMatchingVideoProfiles(&userData.video.profileInfo, &userData,
                                [](VkBaseOutStructure* p, void* pUser) {
                                    UserData* pUserData = static_cast<UserData*>(pUser);
                                    while (p != nullptr) {
                                        if (!pUserData->video.pProfileDesc->info.pfnComparator(p)) {
                                            return;
                                        }
                                        p = p->pNext;
                                    }

                                    pUserData->video.supportedProfile = true;

                                    VkVideoCapabilitiesKHR capabilities{ VK_STRUCTURE_TYPE_VIDEO_CAPABILITIES_KHR };
                                    pUserData->video.pProfileDesc->chainers.pfnCapability(
                                        static_cast<VkBaseOutStructure*>(static_cast<void*>(&capabilities)), pUserData,
                                        [](VkBaseOutStructure* p, void* pUser) {
                                            UserData* pUserData = static_cast<UserData*>(pUser);
                                            VkResult result = pUserData->video.pfnGetPhysicalDeviceVideoCapabilitiesKHR(
                                                pUserData->physicalDevice,
                                                &pUserData->video.profileInfo,
                                                static_cast<VkVideoCapabilitiesKHR*>(static_cast<void*>(p)));
                                            if (result != VK_SUCCESS) {
                                                pUserData->video.supportedProfile = false;
                                                return;
                                            }
                                            while (p != nullptr) {
                                                if (!pUserData->video.pProfileDesc->capability.pfnComparator(p)) {
                                                    pUserData->supported = false;
                                                }
                                                p = p->pNext;
                                            }
                                        }
                                    );

                                    if (pUserData->video.supportedProfile) {
                                        pUserData->video.matchingProfiles++;
                                    } else {
                                        return;
                                    }

                                    std::vector<VkVideoFormatPropertiesKHR> format_props;
                                    for (uint32_t format_index = 0; format_index < pUserData->video.pProfileDesc->formatCount; ++format_index) {
                                        pUserData->index = format_index;
                                        {
                                            VkVideoFormatPropertiesKHR tmp_props{ VK_STRUCTURE_TYPE_VIDEO_FORMAT_PROPERTIES_KHR };
                                            pUserData->video.pProfileDesc->pFormats[format_index].pfnFiller(static_cast<VkBaseOutStructure*>(static_cast<void*>(&tmp_props)));
                                            pUserData->video.formatInfo.imageUsage = tmp_props.imageUsageFlags;
                                        }

                                        uint32_t format_count = 0;
                                        pUserData->video.pfnGetPhysicalDeviceVideoFormatPropertiesKHR(pUserData->physicalDevice, &pUserData->video.formatInfo, &format_count, nullptr);
                                        format_props.resize(format_count, { VK_STRUCTURE_TYPE_VIDEO_FORMAT_PROPERTIES_KHR });
                                        pUserData->video.pProfileDesc->chainers.pfnFormat(
                                            format_count, static_cast<VkBaseOutStructure*>(static_cast<void*>(format_props.data())), pUserData,
                                            [](uint32_t format_count, VkBaseOutStructure* pBaseArray, void* pUser) {
                                                UserData* pUserData = static_cast<UserData*>(pUser);
                                                VkVideoFormatPropertiesKHR* pArray = static_cast<VkVideoFormatPropertiesKHR*>(static_cast<void*>(pBaseArray));
                                                pUserData->video.pfnGetPhysicalDeviceVideoFormatPropertiesKHR(pUserData->physicalDevice, &pUserData->video.formatInfo, &format_count, pArray);
                                                bool found_matching = false;
                                                for (uint32_t i = 0; i < format_count; ++i) {
                                                    bool this_matches = true;
                                                    VkBaseOutStructure* p = static_cast<VkBaseOutStructure*>(static_cast<void*>(&pArray[i]));
                                                    while (p != nullptr) {
                                                        if (!pUserData->video.pProfileDesc->pFormats[pUserData->index].pfnComparator(p)) {
                                                            this_matches = false;
                                                        }
                                                        p = p->pNext;
                                                    }
                                                    if (this_matches) {
                                                        found_matching = true;
                                                        break;
                                                    }
                                                }
                                                if (!found_matching) {
                                                    pUserData->supported = false;
                                                }
                                            }
                                        );
                                    }
                                }
                            );
                            if (!userData.supported || userData.video.matchingProfiles == 0) {
                                supported_variant = false;
                            }
                        }
                    } else {
                        supported_variant = false;
                    }
                }
#endif  // VK_KHR_video_queue

                memcpy(block.blockName, variant_desc.blockName, VP_MAX_PROFILE_NAME_SIZE * sizeof(char));
                if (supported_variant) {
                    supported_blocks.push_back(block);
                    supported_block = true;
                    break;
                } else {
                    unsupported_blocks.push_back(block);
                }
            }

            if (!supported_block) {
                supported_profile = false;
            }
        }

        if (!supported_profile) {
            supported = false;
        }
    }

    const std::vector<VpBlockProperties>& blocks = supported ? supported_blocks : unsupported_blocks;

    if (pProperties == nullptr) {
        *pPropertyCount = static_cast<uint32_t>(blocks.size());
    } else {
        if (*pPropertyCount < static_cast<uint32_t>(blocks.size())) {
            result = VK_INCOMPLETE;
        } else {
            *pPropertyCount = static_cast<uint32_t>(blocks.size());
        }
        for (uint32_t i = 0, n = static_cast<uint32_t>(blocks.size()); i < n; ++i) {
            pProperties[i] = blocks[i];
        }
    }

    *pSupported = supported ? VK_TRUE : VK_FALSE;
    return VK_SUCCESS;
}

VPAPI_ATTR VkResult vpGetPhysicalDeviceProfileSupport(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    VkInstance                                  instance,
    VkPhysicalDevice                            physicalDevice,
    const VpProfileProperties*                  pProfile,
    VkBool32 *pSupported) {
    uint32_t count = 0;

    return vpGetPhysicalDeviceProfileVariantsSupport(
#ifdef VP_USE_OBJECT
        functions,
#endif//VP_USE_OBJECT
        instance, physicalDevice, pProfile, pSupported, &count, nullptr);
}

VPAPI_ATTR VkResult vpCreateDevice(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    VkPhysicalDevice                            physicalDevice,
    const VpDeviceCreateInfo*                   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDevice*                                   pDevice) {
#ifdef VP_USE_OBJECT
    const VpFunctions_T& vp = functions == nullptr ? VpFunctions_T::Get() : *functions;
#else
    const VpFunctions_T& vp = VpFunctions_T::Get();
#endif//VP_USE_OBJECT

    if (physicalDevice == VK_NULL_HANDLE || pCreateInfo == nullptr || pDevice == nullptr) {
        return vp.CreateDevice(physicalDevice, pCreateInfo == nullptr ? nullptr : pCreateInfo->pCreateInfo, pAllocator, pDevice);
    }

    const std::vector<VpBlockProperties>& blocks = detail::GatherBlocks(
        pCreateInfo->enabledFullProfileCount, pCreateInfo->pEnabledFullProfiles,
        pCreateInfo->enabledProfileBlockCount, pCreateInfo->pEnabledProfileBlocks);

    std::unique_ptr<detail::FeaturesChain> chain = std::make_unique<detail::FeaturesChain>();
    std::vector<VkStructureType> structureTypes;

    std::vector<const char*> extensions;
    for (std::uint32_t ext_index = 0, ext_count = pCreateInfo->pCreateInfo->enabledExtensionCount; ext_index < ext_count; ++ext_index) {
        extensions.push_back(pCreateInfo->pCreateInfo->ppEnabledExtensionNames[ext_index]);
    }

    for (std::size_t block_index = 0, block_count = blocks.size(); block_index < block_count; ++block_index) {
        const detail::VpProfileDesc* pProfileDesc = detail::vpGetProfileDesc(blocks[block_index].profiles.profileName);
        if (pProfileDesc == nullptr) {
            return VK_ERROR_UNKNOWN;
        }

        for (std::size_t caps_index = 0, caps_count = pProfileDesc->requiredCapabilityCount; caps_index < caps_count; ++caps_index) {
            const detail::VpCapabilitiesDesc* pCapsDesc = &pProfileDesc->pRequiredCapabilities[caps_index];

            for (std::size_t variant_index = 0, variant_count = pCapsDesc->variantCount; variant_index < variant_count; ++variant_index) {
                const detail::VpVariantDesc* variant = &pCapsDesc->pVariants[variant_index];

                if (strcmp(blocks[block_index].blockName, "") != 0) {
                    if (strcmp(variant->blockName, blocks[block_index].blockName) != 0) {
                        continue;
                    }
                }

                for (uint32_t type_index = 0; type_index < variant->featureStructTypeCount; ++type_index) {
                    const VkStructureType type = variant->pFeatureStructTypes[type_index];
                    if (std::find(structureTypes.begin(), structureTypes.end(), type) == std::end(structureTypes)) {
                        structureTypes.push_back(type);
                    }
                }

                detail::GetExtensions(variant->deviceExtensionCount, variant->pDeviceExtensions, extensions);
            }
        }
    }

    VkBaseOutStructure* pNext = static_cast<VkBaseOutStructure*>(const_cast<void*>(pCreateInfo->pCreateInfo->pNext));
    detail::GatherStructureTypes(structureTypes, pNext);

    chain->Build(structureTypes);

    VkPhysicalDeviceFeatures2KHR* pFeatures = &chain->requiredFeaturesChain;
    if (pCreateInfo->pCreateInfo->pEnabledFeatures) {
        pFeatures->features = *pCreateInfo->pCreateInfo->pEnabledFeatures;
    }

    for (std::size_t block_index = 0, block_count = blocks.size(); block_index < block_count; ++block_index) {
        const detail::VpProfileDesc* pProfileDesc = detail::vpGetProfileDesc(blocks[block_index].profiles.profileName);
        if (pProfileDesc == nullptr) {
            return VK_ERROR_UNKNOWN;
        }

        for (std::size_t caps_index = 0, caps_count = pProfileDesc->requiredCapabilityCount; caps_index < caps_count; ++caps_index) {
            const detail::VpCapabilitiesDesc* pCapsDesc = &pProfileDesc->pRequiredCapabilities[caps_index];

            if (pCapsDesc->variantCount > 1) {
                continue; // Multiple variants are not supported by this version of vpCreateDevice
            }

            for (std::size_t variant_index = 0, variant_count = pCapsDesc->variantCount; variant_index < variant_count; ++variant_index) {
                const detail::VpVariantDesc* variant = &pCapsDesc->pVariants[variant_index];

                VkBaseOutStructure* base_ptr = reinterpret_cast<VkBaseOutStructure*>(pFeatures);
                if (variant->feature.pfnFiller != nullptr) {
                    while (base_ptr != nullptr) {
                        variant->feature.pfnFiller(base_ptr);
                        base_ptr = base_ptr->pNext;
                    }
                }
            }
        }
    }

    chain->ApplyFeatures(pCreateInfo);

    if (pCreateInfo->flags & VP_DEVICE_CREATE_DISABLE_ROBUST_BUFFER_ACCESS_BIT) {
        pFeatures->features.robustBufferAccess = VK_FALSE;
    }

    VkDeviceCreateInfo createInfo{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    createInfo.pNext = &chain->requiredFeaturesChain;
    createInfo.queueCreateInfoCount = pCreateInfo->pCreateInfo->queueCreateInfoCount;
    createInfo.pQueueCreateInfos = pCreateInfo->pCreateInfo->pQueueCreateInfos;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    return vp.CreateDevice(physicalDevice, &createInfo, pAllocator, pDevice);
}

VPAPI_ATTR VkResult vpGetProfileInstanceExtensionProperties(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t*                                   pPropertyCount,
    VkExtensionProperties*                      pProperties) {
    return detail::vpGetProfileExtensionProperties(
#ifdef VP_USE_OBJECT
        functions,
#endif//VP_USE_OBJECT
        pProfile, pBlockName, detail::EXTENSION_INSTANCE, pPropertyCount, pProperties);
}

VPAPI_ATTR VkResult vpGetProfileDeviceExtensionProperties(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t*                                   pPropertyCount,
    VkExtensionProperties*                      pProperties) {
    return detail::vpGetProfileExtensionProperties(
#ifdef VP_USE_OBJECT
        functions,
#endif//VP_USE_OBJECT
        pProfile, pBlockName, detail::EXTENSION_DEVICE, pPropertyCount, pProperties);
}

VPAPI_ATTR VkResult vpGetProfileFeatures(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    void*                                       pNext) {
#ifdef VP_USE_OBJECT
    (void)functions;
#endif//VP_USE_OBJECT

    VkResult result = pBlockName == nullptr ? VK_SUCCESS : VK_INCOMPLETE;

    const std::vector<VpProfileProperties>& gathered_profiles = detail::GatherProfiles(*pProfile);

    for (std::size_t profile_index = 0, profile_count = gathered_profiles.size(); profile_index < profile_count; ++profile_index) {
        const detail::VpProfileDesc* profile_desc = detail::vpGetProfileDesc(gathered_profiles[profile_index].profileName);
        if (profile_desc == nullptr) return VK_ERROR_UNKNOWN;

        for (uint32_t capability_index = 0; capability_index < profile_desc->requiredCapabilityCount; ++capability_index) {
            const detail::VpCapabilitiesDesc& cap_desc = profile_desc->pRequiredCapabilities[capability_index];

            for (uint32_t variant_index = 0; variant_index < cap_desc.variantCount; ++variant_index) {
                const detail::VpVariantDesc& variant = cap_desc.pVariants[variant_index];
                if (pBlockName != nullptr) {
                    if (strcmp(variant.blockName, pBlockName) != 0) {
                        continue;
                    }
                    result = VK_SUCCESS;
                }

                if (variant.feature.pfnFiller == nullptr) continue;

                VkBaseOutStructure* p = static_cast<VkBaseOutStructure*>(pNext);
                while (p != nullptr) {
                    variant.feature.pfnFiller(p);
                    p = p->pNext;
                }
            }
        }
    }

    return result;
}

VPAPI_ATTR VkResult vpGetProfileProperties(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    void*                                       pNext) {
    VkResult result = pBlockName == nullptr ? VK_SUCCESS : VK_INCOMPLETE;

    VkBool32 multiple_variants = VK_FALSE;
    if (vpHasMultipleVariantsProfile(
#ifdef VP_USE_OBJECT
        functions,
#endif//VP_USE_OBJECT
        pProfile,
        &multiple_variants) == VK_ERROR_UNKNOWN) {
        return VK_ERROR_UNKNOWN;
    }
    if (multiple_variants == VK_TRUE && pBlockName == nullptr) {
        return VK_ERROR_UNKNOWN;
    }

    const std::vector<VpProfileProperties>& gathered_profiles = detail::GatherProfiles(*pProfile);

    for (std::size_t profile_index = 0, profile_count = gathered_profiles.size(); profile_index < profile_count; ++profile_index) {
        const detail::VpProfileDesc* profile_desc = detail::vpGetProfileDesc(gathered_profiles[profile_index].profileName);
        if (profile_desc == nullptr) return VK_ERROR_UNKNOWN;

        for (uint32_t capability_index = 0; capability_index < profile_desc->requiredCapabilityCount; ++capability_index) {
            const detail::VpCapabilitiesDesc& cap_desc = profile_desc->pRequiredCapabilities[capability_index];

            for (uint32_t variant_index = 0; variant_index < cap_desc.variantCount; ++variant_index) {
                const detail::VpVariantDesc& variant = cap_desc.pVariants[variant_index];
                if (pBlockName != nullptr) {
                    if (strcmp(variant.blockName, pBlockName) != 0) {
                        continue;
                    }
                    result = VK_SUCCESS;
                }

                if (variant.property.pfnFiller == nullptr) continue;

                VkBaseOutStructure* p = static_cast<VkBaseOutStructure*>(pNext);
                while (p != nullptr) {
                    variant.property.pfnFiller(p);
                    p = p->pNext;
                }
            }
        }
    }

    return result;
}

VPAPI_ATTR VkResult vpGetProfileQueueFamilyProperties(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t*                                   pPropertyCount,
    VkQueueFamilyProperties2KHR*                pProperties) {
#ifdef VP_USE_OBJECT
    (void)functions;
#endif//VP_USE_OBJECT

    if (pPropertyCount == nullptr) return VK_ERROR_UNKNOWN;

    VkResult result = pBlockName == nullptr ? VK_SUCCESS : VK_INCOMPLETE;

    const std::vector<VpProfileProperties>& gathered_profiles = detail::GatherProfiles(*pProfile);

    uint32_t total_queue_family_count = 0;

    for (std::size_t profile_index = 0, profile_count = gathered_profiles.size(); profile_index < profile_count; ++profile_index) {
        const detail::VpProfileDesc* profile_desc = detail::vpGetProfileDesc(gathered_profiles[profile_index].profileName);
        if (profile_desc == nullptr) return VK_ERROR_UNKNOWN;

        for (uint32_t capability_index = 0; capability_index < profile_desc->requiredCapabilityCount; ++capability_index) {
            const detail::VpCapabilitiesDesc& cap_desc = profile_desc->pRequiredCapabilities[capability_index];

            for (uint32_t variant_index = 0; variant_index < cap_desc.variantCount; ++variant_index) {
                const detail::VpVariantDesc& variant = cap_desc.pVariants[variant_index];
                if (pBlockName != nullptr) {
                    if (strcmp(variant.blockName, pBlockName) != 0) {
                        continue;
                    }
                    result = VK_SUCCESS;
                }

                if (pProperties != nullptr) {
                    for (uint32_t i = 0; i < variant.queueFamilyCount; ++i) {
                        if (total_queue_family_count < *pPropertyCount) {
                            if (variant.pQueueFamilies[i].pfnFiller == nullptr) continue;

                            VkBaseOutStructure* p = reinterpret_cast<VkBaseOutStructure*>(pProperties);
                            while (p != nullptr) {
                                variant.pQueueFamilies[i].pfnFiller(p);
                                p = p->pNext;
                            }

                            total_queue_family_count++;
                            pProperties++;
                        } else {
                            result = VK_INCOMPLETE;
                            break;
                        }
                    }
                } else {
                    total_queue_family_count += variant.queueFamilyCount;
                }
            }
        }
    }

    *pPropertyCount = total_queue_family_count;
    return result;
}

VPAPI_ATTR VkResult vpGetProfileFormats(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t*                                   pFormatCount,
    VkFormat*                                   pFormats) {
#ifdef VP_USE_OBJECT
    (void)functions;
#endif//VP_USE_OBJECT

    VkResult result = pBlockName == nullptr ? VK_SUCCESS : VK_INCOMPLETE;

    std::vector<VkFormat> results;

    const std::vector<VpProfileProperties>& gathered_profiles = detail::GatherProfiles(*pProfile);

    for (std::size_t profile_index = 0, profile_count = gathered_profiles.size(); profile_index < profile_count; ++profile_index) {
        const detail::VpProfileDesc* profile_desc = detail::vpGetProfileDesc(gathered_profiles[profile_index].profileName);
        if (profile_desc == nullptr) return VK_ERROR_UNKNOWN;

        for (uint32_t capability_index = 0; capability_index < profile_desc->requiredCapabilityCount; ++capability_index) {
            const detail::VpCapabilitiesDesc& cap_desc = profile_desc->pRequiredCapabilities[capability_index];

            for (uint32_t variant_index = 0; variant_index < cap_desc.variantCount; ++variant_index) {
                const detail::VpVariantDesc& variant = cap_desc.pVariants[variant_index];
                if (pBlockName != nullptr) {
                    if (strcmp(variant.blockName, pBlockName) != 0) {
                        continue;
                    }
                    result = VK_SUCCESS;
                }

                for (uint32_t format_index = 0; format_index < variant.formatCount; ++format_index) {
                    if (std::find(results.begin(), results.end(), variant.pFormats[format_index].format) == std::end(results)) {
                        results.push_back(variant.pFormats[format_index].format);
                    }
                }
            }
        }
    }

    const uint32_t count = static_cast<uint32_t>(results.size());

    if (pFormats == nullptr) {
        *pFormatCount = count;
    } else {
        if (*pFormatCount < count) {
            result = VK_INCOMPLETE;
        } else {
            *pFormatCount = count;
        }

        if (*pFormatCount > 0) {
            memcpy(pFormats, &results[0], *pFormatCount * sizeof(VkFormat));
        }
    }
    return result;
}

VPAPI_ATTR VkResult vpGetProfileFormatProperties(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    VkFormat                                    format,
    void*                                       pNext) {
#ifdef VP_USE_OBJECT
    (void)functions;
#endif//VP_USE_OBJECT

    VkResult result = pBlockName == nullptr ? VK_SUCCESS : VK_INCOMPLETE;

    const std::vector<VpProfileProperties>& gathered_profiles = detail::GatherProfiles(*pProfile);

    for (std::size_t profile_index = 0, profile_count = gathered_profiles.size(); profile_index < profile_count; ++profile_index) {
        const char* profile_name = gathered_profiles[profile_index].profileName;

        const detail::VpProfileDesc* pProfileDesc = detail::vpGetProfileDesc(profile_name);
        if (pProfileDesc == nullptr) return VK_ERROR_UNKNOWN;

        for (uint32_t required_capability_index = 0; required_capability_index < pProfileDesc->requiredCapabilityCount;
                ++required_capability_index) {
            const detail::VpCapabilitiesDesc& required_capabilities = pProfileDesc->pRequiredCapabilities[required_capability_index];

            for (uint32_t required_variant_index = 0; required_variant_index < required_capabilities.variantCount; ++required_variant_index) {
                const detail::VpVariantDesc& variant = required_capabilities.pVariants[required_variant_index];
                if (pBlockName != nullptr) {
                    if (strcmp(variant.blockName, pBlockName) != 0) {
                        continue;
                    }
                    result = VK_SUCCESS;
                }

                for (uint32_t format_index = 0; format_index < variant.formatCount; ++format_index) {
                    if (variant.pFormats[format_index].format != format) {
                        continue;
                    }

                    VkBaseOutStructure* base_ptr = static_cast<VkBaseOutStructure*>(static_cast<void*>(pNext));
                    while (base_ptr != nullptr) {
                        variant.pFormats[format_index].pfnFiller(base_ptr);
                        base_ptr = base_ptr->pNext;
                    }
#if defined(VK_VERSION_1_3) || defined(VK_KHR_format_feature_flags2)
                    VkFormatProperties2KHR* fp2 = static_cast<VkFormatProperties2KHR*>(
                        detail::vpGetStructure(pNext, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR));
                    VkFormatProperties3KHR* fp3 = static_cast<VkFormatProperties3KHR*>(
                        detail::vpGetStructure(pNext, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3_KHR));
                    if (fp3 != nullptr) {
                        VkFormatProperties2KHR fp{ VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR };
                        variant.pFormats[format_index].pfnFiller(static_cast<VkBaseOutStructure*>(static_cast<void*>(&fp)));
                        fp3->linearTilingFeatures |= static_cast<VkFormatFeatureFlags2KHR>(fp3->linearTilingFeatures | fp.formatProperties.linearTilingFeatures);
                        fp3->optimalTilingFeatures |= static_cast<VkFormatFeatureFlags2KHR>(fp3->optimalTilingFeatures | fp.formatProperties.optimalTilingFeatures);
                        fp3->bufferFeatures |= static_cast<VkFormatFeatureFlags2KHR>(fp3->bufferFeatures | fp.formatProperties.bufferFeatures);
                    }
                    if (fp2 != nullptr) {
                        VkFormatProperties3KHR fp{ VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3_KHR };
                        variant.pFormats[format_index].pfnFiller(static_cast<VkBaseOutStructure*>(static_cast<void*>(&fp)));
                        fp2->formatProperties.linearTilingFeatures |= static_cast<VkFormatFeatureFlags>(fp2->formatProperties.linearTilingFeatures | fp.linearTilingFeatures);
                        fp2->formatProperties.optimalTilingFeatures |= static_cast<VkFormatFeatureFlags>(fp2->formatProperties.optimalTilingFeatures | fp.optimalTilingFeatures);
                        fp2->formatProperties.bufferFeatures |= static_cast<VkFormatFeatureFlags>(fp2->formatProperties.bufferFeatures | fp.bufferFeatures);
                    }
#endif
                }
            }
        }
    }

    return result;
}

VPAPI_ATTR VkResult vpGetProfileFeatureStructureTypes(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t*                                   pStructureTypeCount,
    VkStructureType*                            pStructureTypes) {
    return detail::vpGetProfileStructureTypes(
#ifdef VP_USE_OBJECT
        functions,
#endif//VP_USE_OBJECT
        pProfile, pBlockName, detail::STRUCTURE_FEATURE, pStructureTypeCount, pStructureTypes);
}

VPAPI_ATTR VkResult vpGetProfilePropertyStructureTypes(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t*                                   pStructureTypeCount,
    VkStructureType*                            pStructureTypes) {
    return detail::vpGetProfileStructureTypes(
#ifdef VP_USE_OBJECT
        functions,
#endif//VP_USE_OBJECT
        pProfile, pBlockName, detail::STRUCTURE_PROPERTY, pStructureTypeCount, pStructureTypes);
}

VPAPI_ATTR VkResult vpGetProfileQueueFamilyStructureTypes(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t*                                   pStructureTypeCount,
    VkStructureType*                            pStructureTypes) {
    return detail::vpGetProfileStructureTypes(
#ifdef VP_USE_OBJECT
        functions,
#endif//VP_USE_OBJECT
        pProfile, pBlockName, detail::STRUCTURE_QUEUE_FAMILY, pStructureTypeCount, pStructureTypes);
}

VPAPI_ATTR VkResult vpGetProfileFormatStructureTypes(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t*                                   pStructureTypeCount,
    VkStructureType*                            pStructureTypes) {
    return detail::vpGetProfileStructureTypes(
#ifdef VP_USE_OBJECT
        functions,
#endif//VP_USE_OBJECT
        pProfile, pBlockName, detail::STRUCTURE_FORMAT, pStructureTypeCount, pStructureTypes);
}

#ifdef VK_KHR_video_queue
// Query the list of video profiles specified by the profile
VPAPI_ATTR VkResult vpGetProfileVideoProfiles(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t*                                   pVideoProfileCount,
    VpVideoProfileProperties*                   pVideoProfiles) {
#ifdef VP_USE_OBJECT
    (void)functions;
#endif//VP_USE_OBJECT
    if (pVideoProfileCount == nullptr) return VK_ERROR_UNKNOWN;

    VkResult result = pBlockName == nullptr ? VK_SUCCESS : VK_INCOMPLETE;

    uint32_t total_video_profile_count = 0;

    const std::vector<VpProfileProperties>& gathered_profiles = detail::GatherProfiles(*pProfile);

    for (std::size_t profile_index = 0, profile_count = gathered_profiles.size(); profile_index < profile_count; ++profile_index) {
        const detail::VpProfileDesc* profile_desc = detail::vpGetProfileDesc(gathered_profiles[profile_index].profileName);
        if (profile_desc == nullptr) return VK_ERROR_UNKNOWN;

        for (uint32_t capability_index = 0; capability_index < profile_desc->requiredCapabilityCount; ++capability_index) {
            const detail::VpCapabilitiesDesc& cap_desc = profile_desc->pRequiredCapabilities[capability_index];

            for (uint32_t variant_index = 0; variant_index < cap_desc.variantCount; ++variant_index) {
                const detail::VpVariantDesc& variant = cap_desc.pVariants[variant_index];
                if (pBlockName != nullptr) {
                    if (strcmp(variant.blockName, pBlockName) != 0) {
                        continue;
                    }
                    result = VK_SUCCESS;
                }

                if (pVideoProfiles != nullptr) {
                    for (uint32_t i = 0; i < variant.videoProfileCount; ++i) {
                        if (total_video_profile_count < *pVideoProfileCount) {
                            *pVideoProfiles = variant.pVideoProfiles[i].properties;
                            total_video_profile_count++;
                            pVideoProfiles++;
                        } else {
                            result = VK_INCOMPLETE;
                            break;
                        }
                    }
                } else {
                    total_video_profile_count += variant.videoProfileCount;
                }
            }
        }
    }

    *pVideoProfileCount = total_video_profile_count;
    return result;
}

VPAPI_ATTR VkResult vpGetProfileVideoProfileInfo(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t                                    videoProfileIndex,
    VkVideoProfileInfoKHR*                      pVideoProfileInfo) {
#ifdef VP_USE_OBJECT
    (void)functions;
#endif//VP_USE_OBJECT

    const detail::VpVideoProfileDesc* pVideoProfileDesc = nullptr;
    VkResult result = detail::vpGetProfileVideoProfileDesc(pProfile, pBlockName, videoProfileIndex, &pVideoProfileDesc);

    if (pVideoProfileDesc != nullptr) {
        VkBaseOutStructure* p = reinterpret_cast<VkBaseOutStructure*>(pVideoProfileInfo);
        while (p != nullptr) {
            pVideoProfileDesc->info.pfnFiller(p);
            p = p->pNext;
        }
    }

    return result;
}

VPAPI_ATTR VkResult vpGetProfileVideoCapabilities(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t                                    videoProfileIndex,
    void*                                       pNext) {
#ifdef VP_USE_OBJECT
    (void)functions;
#endif//VP_USE_OBJECT

    const detail::VpVideoProfileDesc* pVideoProfileDesc = nullptr;
    VkResult result = detail::vpGetProfileVideoProfileDesc(pProfile, pBlockName, videoProfileIndex, &pVideoProfileDesc);

    if (pVideoProfileDesc != nullptr) {
        VkBaseOutStructure* p = reinterpret_cast<VkBaseOutStructure*>(pNext);
        while (p != nullptr) {
            pVideoProfileDesc->capability.pfnFiller(p);
            p = p->pNext;
        }
    }

    return result;
}

VPAPI_ATTR VkResult vpGetProfileVideoFormatProperties(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t                                    videoProfileIndex,
    uint32_t*                                   pPropertyCount,
    VkVideoFormatPropertiesKHR*                 pProperties) {
#ifdef VP_USE_OBJECT
    (void)functions;
#endif//VP_USE_OBJECT

    const detail::VpVideoProfileDesc* pVideoProfileDesc = nullptr;
    VkResult result = detail::vpGetProfileVideoProfileDesc(pProfile, pBlockName, videoProfileIndex, &pVideoProfileDesc);

    uint32_t property_count = 0;
    if (pVideoProfileDesc != nullptr) {
        if (pProperties != nullptr) {
            for (; property_count < pVideoProfileDesc->formatCount; ++property_count) {
                if (property_count < *pPropertyCount) {
                    VkBaseOutStructure* p = reinterpret_cast<VkBaseOutStructure*>(&pProperties[property_count]);
                    while (p != nullptr) {
                        pVideoProfileDesc->pFormats[property_count].pfnFiller(p);
                        p = p->pNext;
                    }
                } else {
                    result = VK_INCOMPLETE;
                    break;
                }
            }
        } else {
            property_count = pVideoProfileDesc->formatCount;
        }
    }

    *pPropertyCount = property_count;
    return result;
}

VPAPI_ATTR VkResult vpGetProfileVideoProfileInfoStructureTypes(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t                                    videoProfileIndex,
    uint32_t*                                   pStructureTypeCount,
    VkStructureType*                            pStructureTypes) {
#ifdef VP_USE_OBJECT
    (void)functions;
#endif//VP_USE_OBJECT

    const detail::VpVideoProfileDesc* pVideoProfileDesc = nullptr;
    VkResult result = detail::vpGetProfileVideoProfileDesc(pProfile, pBlockName, videoProfileIndex, &pVideoProfileDesc);

    if (pVideoProfileDesc != nullptr) {
        if (pStructureTypes != nullptr) {
            if (*pStructureTypeCount < pVideoProfileDesc->infoStructTypeCount) {
                result = VK_INCOMPLETE;
            } else {
                *pStructureTypeCount = pVideoProfileDesc->infoStructTypeCount;
            }
            if (*pStructureTypeCount > 0) {
                memcpy(pStructureTypes, pVideoProfileDesc->pInfoStructTypes, *pStructureTypeCount * sizeof(VkStructureType));
            }
        } else {
            *pStructureTypeCount = pVideoProfileDesc->infoStructTypeCount;
        }
    }

    return result;
}

VPAPI_ATTR VkResult vpGetProfileVideoCapabilityStructureTypes(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t                                    videoProfileIndex,
    uint32_t*                                   pStructureTypeCount,
    VkStructureType*                            pStructureTypes) {
#ifdef VP_USE_OBJECT
    (void)functions;
#endif//VP_USE_OBJECT

    const detail::VpVideoProfileDesc* pVideoProfileDesc = nullptr;
    VkResult result = detail::vpGetProfileVideoProfileDesc(pProfile, pBlockName, videoProfileIndex, &pVideoProfileDesc);

    if (pVideoProfileDesc != nullptr) {
        if (pStructureTypes != nullptr) {
            if (*pStructureTypeCount < pVideoProfileDesc->capabilityStructTypeCount) {
                result = VK_INCOMPLETE;
            } else {
                *pStructureTypeCount = pVideoProfileDesc->capabilityStructTypeCount;
            }
            if (*pStructureTypeCount > 0) {
                memcpy(pStructureTypes, pVideoProfileDesc->pCapabilityStructTypes, *pStructureTypeCount * sizeof(VkStructureType));
            }
        } else {
            *pStructureTypeCount = pVideoProfileDesc->capabilityStructTypeCount;
        }
    }

    return result;
}

VPAPI_ATTR VkResult vpGetProfileVideoFormatStructureTypes(
#ifdef VP_USE_OBJECT
    VpFunctions                                 functions,
#endif//VP_USE_OBJECT
    const VpProfileProperties*                  pProfile,
    const char*                                 pBlockName,
    uint32_t                                    videoProfileIndex,
    uint32_t*                                   pStructureTypeCount,
    VkStructureType*                            pStructureTypes) {
#ifdef VP_USE_OBJECT
    (void)functions;
#endif//VP_USE_OBJECT

    const detail::VpVideoProfileDesc* pVideoProfileDesc = nullptr;
    VkResult result = detail::vpGetProfileVideoProfileDesc(pProfile, pBlockName, videoProfileIndex, &pVideoProfileDesc);

    if (pVideoProfileDesc != nullptr) {
        if (pStructureTypes != nullptr) {
            if (*pStructureTypeCount < pVideoProfileDesc->formatStructTypeCount) {
                result = VK_INCOMPLETE;
            } else {
                *pStructureTypeCount = pVideoProfileDesc->formatStructTypeCount;
            }
            if (*pStructureTypeCount > 0) {
                memcpy(pStructureTypes, pVideoProfileDesc->pFormatStructTypes, *pStructureTypeCount * sizeof(VkStructureType));
            }
        } else {
            *pStructureTypeCount = pVideoProfileDesc->formatStructTypeCount;
        }
    }

    return result;
}
#endif  // VK_KHR_video_queue
'''

PRIVATE_IMPL_FEATURES_CHAIN_IMPL = '''
    VkPhysicalDeviceFeatures2KHR requiredFeaturesChain{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR, nullptr};
    VkBaseOutStructure* current = nullptr;

    void ApplyRobustness(const VpDeviceCreateInfo* pCreateInfo) {
#ifdef VK_VERSION_1_1
        VkPhysicalDeviceFeatures2KHR* pFeatures2 = static_cast<VkPhysicalDeviceFeatures2KHR*>(
            vpGetStructure(&this->requiredFeaturesChain, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR));
        if (pFeatures2 != nullptr && (pCreateInfo->flags & VP_DEVICE_CREATE_DISABLE_ROBUST_BUFFER_ACCESS_BIT)) {
            pFeatures2->features.robustBufferAccess = VK_FALSE;
        }
#endif

#ifdef VK_EXT_robustness2
        VkPhysicalDeviceRobustness2FeaturesEXT* pRobustness2FeaturesEXT = static_cast<VkPhysicalDeviceRobustness2FeaturesEXT*>(
            vpGetStructure(&this->requiredFeaturesChain, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT));
        if (pRobustness2FeaturesEXT != nullptr) {
            if (pCreateInfo->flags & VP_DEVICE_CREATE_DISABLE_ROBUST_BUFFER_ACCESS_BIT) {
                pRobustness2FeaturesEXT->robustBufferAccess2 = VK_FALSE;
            }
            if (pCreateInfo->flags & VP_DEVICE_CREATE_DISABLE_ROBUST_IMAGE_ACCESS_BIT) {
                pRobustness2FeaturesEXT->robustImageAccess2 = VK_FALSE;
            }
        }
#endif
#ifdef VK_EXT_image_robustness
        VkPhysicalDeviceImageRobustnessFeaturesEXT* pImageRobustnessFeaturesEXT =
            static_cast<VkPhysicalDeviceImageRobustnessFeaturesEXT*>(vpGetStructure(
                &this->requiredFeaturesChain, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES_EXT));
        if (pImageRobustnessFeaturesEXT != nullptr && (pCreateInfo->flags & VP_DEVICE_CREATE_DISABLE_ROBUST_IMAGE_ACCESS_BIT)) {
            pImageRobustnessFeaturesEXT->robustImageAccess = VK_FALSE;
        }
#endif
#ifdef VK_VERSION_1_3
        VkPhysicalDeviceVulkan13Features* pVulkan13Features = static_cast<VkPhysicalDeviceVulkan13Features*>(
            vpGetStructure(&this->requiredFeaturesChain, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES));
        if (pVulkan13Features != nullptr && (pCreateInfo->flags & VP_DEVICE_CREATE_DISABLE_ROBUST_IMAGE_ACCESS_BIT)) {
            pVulkan13Features->robustImageAccess = VK_FALSE;
        }
#endif
    }

    void ApplyFeatures(const VpDeviceCreateInfo* pCreateInfo) {
        const std::size_t offset = sizeof(VkBaseOutStructure);
        const VkBaseOutStructure* q = reinterpret_cast<const VkBaseOutStructure*>(pCreateInfo->pCreateInfo->pNext);
        while (q) {
            const std::size_t count = this->structureSize[q->sType];
            for (std::size_t index = 0; index < count; ++index) {
                const VkBaseOutStructure* pInputStruct = reinterpret_cast<const VkBaseOutStructure*>(q);
                VkBaseOutStructure* pOutputStruct = reinterpret_cast<VkBaseOutStructure*>(detail::vpGetStructure(&this->requiredFeaturesChain, q->sType));
                const uint8_t* pInputData = reinterpret_cast<const uint8_t*>(pInputStruct) + offset;
                uint8_t* pOutputData = reinterpret_cast<uint8_t*>(pOutputStruct) + offset;
                const VkBool32* input = reinterpret_cast<const VkBool32*>(pInputData);
                VkBool32* output = reinterpret_cast<VkBool32*>(pOutputData);

                output[index] = (output[index] == VK_TRUE || input[index] == VK_TRUE) ? VK_TRUE : VK_FALSE;
            }
            q = q->pNext;
        }

        this->ApplyRobustness(pCreateInfo);
    }

    void PushBack(VkBaseOutStructure* found) {
        VkBaseOutStructure* last = reinterpret_cast<VkBaseOutStructure*>(&requiredFeaturesChain);
        while (last->pNext != nullptr) {
            last = last->pNext;
        }
        last->pNext = found;
    }

    void Build(const std::vector<VkStructureType>& requiredList) {
        for (std::size_t i = 0, n = requiredList.size(); i < n; ++i) {
            const VkStructureType sType = requiredList[i];
            if (sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR) {
                continue;
            }

            VkBaseOutStructure* found = vpExtractStructure(&physicalDeviceFeatures2KHR, sType);
            if (found == nullptr) {
                continue;
            }

            PushBack(found);
        }
    }
'''

# Generates a C/C++ condition verifying flags
# e.g.:
#   condition = '(A+B),C'
#   variable = 'myFlags'
#   generates '((myFlags & A && myFlags & B) || myFlags & C)"
def genCConditionForFlags(condition, variable):
    c_cond = ""
    value = ""

    def genExpressionFromValue(variable, value):
        return value if value == "" else "{0} & {1}".format(variable, value)

    for char in condition:
        if char in ['(', ')', '+', ',']:
            c_cond += genExpressionFromValue(variable, value)
            value = ""
            if char == '+':
                # '+' means AND
                c_cond += ' && '
            elif char == ',':
                # ',' means OR
                c_cond += ' || '
            else:
                c_cond += char
        else:
            value += char
    c_cond += genExpressionFromValue(variable, value)

    return c_cond

class VulkanProfileCapabilities():
    def __init__(self, registry, json_profile_key, json_profile_value, json_capability_key, json_capabilities_list, merge_mode, doc_mode):
        self.blockName = json_capability_key
        self.extensions = dict()
        self.instanceExtensions = dict()
        self.deviceExtensions = dict()
        self.features = dict()
        self.properties = dict()
        self.formats = dict()
        self.queueFamiliesProperties = []
        self.videoProfiles = []
        self.videoProfilesByName = OrderedDict()
        if merge_mode:
            for json_capabilities in json_capabilities_list:
                self.mergeCaps(registry, json_capabilities, True)
        elif doc_mode:
            for json_capabilities in json_capabilities_list:
                self.mergeCaps(registry, json_capabilities, False)
        else:
            self.mergeCaps(registry, json_capabilities_list, merge_mode)

    def mergeCaps(self, registry, caps, merge_mode):
        self.mergeProfileExtensions(registry, caps)
        self.mergeProfileFeatures(caps)
        if not merge_mode:
            self.mergeProfileProperties(caps)
            self.mergeProfileFormats(caps)
            self.mergeProfileQueueFamiliesProperties(caps)
            self.mergeProfileVideoProfiles(registry, caps)

    def mergeProfileCapData(self, dst, src):
        if type(src) != type(dst):
            Log.f("Data type confict during profile capability data merge (src is '{0}', dst is '{1}')".format(type(src), type(dst)))
        elif type(src) == dict:
            for key, val in src.items():
                if type(val) == dict:
                    if not key in dst:
                        dst[key] = dict()
                    self.mergeProfileCapData(dst[key], val)

                elif type(val) == list:
                    if not key in dst:
                        dst[key] = []
                    dst[key].extend(val)

                else:
                    if key in dst and type(dst[key]) != type(val):
                        # For some cases where float value are written as integer in JSON files, eg: pointSizeGranularity and lineWidthGranularity
                        if type(val) is int and type(dst[key]) is float:
                            dst[key] = float(val)
                        elif type(val) is float and type(dst[key]) is int:
                            dst[key] = float(val)
                        else:
                            Log.f("'{0}' data type conflict during profile capability data merge (src is '{1}', dst is '{2}')".format(key, type(val), type(dst[key])))
                    else:
                        dst[key] = val
        else:
            Log.f("Unexpected data type during profile capability data merge (src is '{0}', dst is '{1}')".format(type(src), type(dst)))

    def mergeProfileExtensions(self, registry, data):
        if data.get('extensions') != None:
            for extName, specVer in data['extensions'].items():
                extInfo = registry.extensions.get(extName)
                if extInfo != None:
                    self.extensions[extName] = specVer
                    if extInfo.type == 'instance':
                        self.instanceExtensions[extName] = specVer
                    elif extInfo.type == 'device':
                        self.deviceExtensions[extName] = specVer
                    else:
                        Log.f("Extension '{0}' has invalid type '{1}'".format(extName, extInfo.type))
                else:
                    Log.f("Extension '{0}' does not exist".format(extName))

    def mergeProfileFeatures(self, data):
        if data.get('features') != None:
            self.mergeProfileCapData(self.features, data['features'])

    def mergeProfileProperties(self, data):
        if data.get('properties') != None:
            self.mergeProfileCapData(self.properties, data['properties'])

    def mergeProfileFormats(self, data):
        if data.get('formats') != None:
            self.mergeProfileCapData(self.formats, data['formats'])

    def mergeProfileQueueFamiliesProperties(self, data):
        if data.get('queueFamiliesProperties') != None:
            self.queueFamiliesProperties.extend(data['queueFamiliesProperties'])

    def mergeProfileVideoProfiles(self, registry, data):
        if data.get('videoProfiles') != None:
            for videoProfile in data.get('videoProfiles'):
                videoProfileName = registry.getVideoProfileNameFromVideoProfile(videoProfile)
                if not videoProfileName in self.videoProfilesByName:
                    self.videoProfilesByName[videoProfileName] = {
                        'profile': videoProfile['profile'] if 'profile' in videoProfile else dict(),
                        'capabilities': dict(),
                        'formats': list()
                    }
                    self.videoProfiles.append(self.videoProfilesByName[videoProfileName])
                if 'capabilities' in videoProfile:
                    self.mergeProfileCapData(self.videoProfilesByName[videoProfileName]['capabilities'], videoProfile['capabilities'])
                if 'formats' in videoProfile:
                    self.videoProfilesByName[videoProfileName]['formats'].extend(videoProfile['formats'])


class VulkanProfileStructs():
    def __init__(self, registry, caps):
        # Feature struct types
        self.feature = []

        for capabilities_key, capabilities_value in caps.items():
            for name in capabilities_value.features:
                if name in [ 'VkPhysicalDeviceFeatures', 'VkPhysicalDeviceFeatures2KHR' ]:
                    # Special case, add both as VkPhysicalDeviceFeatures2[KHR]
                    if 'VkPhysicalDeviceFeatures2KHR' in registry.structs:
                        if registry.structs['VkPhysicalDeviceFeatures2KHR'] not in self.feature:
                            self.feature.append(registry.structs['VkPhysicalDeviceFeatures2KHR'])
                    else:
                        if registry.structs['VkPhysicalDeviceFeatures2'] not in self.feature:
                            self.feature.append(registry.structs['VkPhysicalDeviceFeatures2'])
                else:
                    if registry.getChainableStructDef(name, 'VkPhysicalDeviceFeatures2') not in self.feature:
                        self.feature.append(registry.getChainableStructDef(name, 'VkPhysicalDeviceFeatures2'))
        self.eliminateAliases(self.feature)

        # Property struct types
        self.property = []

        for key, value in caps.items():
            for name in value.properties:
                if name in [ 'VkPhysicalDeviceProperties', 'VkPhysicalDeviceProperties2KHR' ]:
                    # Special case, add both as VkPhysicalDeviceProperties2[KHR]
                    if 'VkPhysicalDeviceProperties2KHR' in registry.structs:
                        if registry.structs['VkPhysicalDeviceProperties2KHR'] not in self.property:
                            self.property.append(registry.structs['VkPhysicalDeviceProperties2KHR'])
                    else:
                        if registry.structs['VkPhysicalDeviceProperties2'] not in self.property:
                            self.property.append(registry.structs['VkPhysicalDeviceProperties2'])
                else:
                    if registry.getChainableStructDef(name, 'VkPhysicalDeviceProperties2') not in self.property:
                        self.property.append(registry.getChainableStructDef(name, 'VkPhysicalDeviceProperties2'))
        self.eliminateAliases(self.property)

        # Queue family struct types
        self.queueFamily = []

        for key, value in caps.items():
            queueFamilyStructs = dict()
            for queueFamilyProps in value.queueFamiliesProperties:
                queueFamilyStructs.update(queueFamilyProps)
            for name in queueFamilyStructs:
                if name in [ 'VkQueueFamilyProperties', 'VkQueueFamilyProperties2KHR' ]:
                    # Special case, add both as VkQueueFamilyProperties2[KHR]
                    if 'VkQueueFamilyProperties2KHR' in registry.structs:
                        if registry.structs['VkQueueFamilyProperties2KHR'] not in self.queueFamily:
                            self.queueFamily.append(registry.structs['VkQueueFamilyProperties2KHR'])
                    else:
                        if registry.structs['VkQueueFamilyProperties2'] not in self.queueFamily:
                            self.queueFamily.append(registry.structs['VkQueueFamilyProperties2'])
                else:
                    if registry.getChainableStructDef(name, 'VkQueueFamilyProperties2') not in self.queueFamily:
                        self.queueFamily.append(registry.getChainableStructDef(name, 'VkQueueFamilyProperties2'))
        self.eliminateAliases(self.queueFamily)

        # Format struct types
        self.format = []

        for key, value in caps.items():
            formatStructs = dict()
            for formatProps in value.formats.values():
                formatStructs.update(formatProps)
            for name in formatStructs:
                if name in [ 'VkFormatProperties', 'VkFormatProperties2KHR', 'VkFormatProperties3KHR' ]:
                    # Special case, add all as VkFormatProperties2[KHR] and VkFormatProperties3KHR
                    if 'VkFormatProperties2KHR' in registry.structs:
                        if registry.structs['VkFormatProperties2KHR'] not in self.format:
                            self.format.append(registry.structs['VkFormatProperties2KHR'])
                    else:
                        if registry.structs['VkFormatProperties2'] not in self.format:
                            self.format.append(registry.structs['VkFormatProperties2'])
                    if 'VkFormatProperties3KHR' in registry.structs:
                        if registry.structs['VkFormatProperties3KHR'] not in self.format:
                            self.format.append(registry.structs['VkFormatProperties3KHR'])
                else:
                    if registry.getChainableStructDef(name, 'VkFormatProperties2') not in self.format:
                        self.format.append(registry.getChainableStructDef(name, 'VkFormatProperties2'))
        self.eliminateAliases(self.format)

        # Video profile struct types
        self.videoProfileInfo = OrderedDict()
        self.videoCapability = OrderedDict()
        self.videoFormat = OrderedDict()

        for value in caps.values():
            for videoProfileName, videoProfile in value.videoProfilesByName.items():
                # Video profile info struct types
                videoProfileInfo = self.videoProfileInfo[videoProfileName] = []
                for name in videoProfile['profile']:
                    structDef = registry.getChainableStructDef(name, 'VkVideoProfileInfoKHR')
                    if structDef not in videoProfileInfo:
                        videoProfileInfo.append(structDef)
                self.eliminateAliases(videoProfileInfo)

                # Video capability struct types
                videoCapability = self.videoCapability[videoProfileName] = []
                if 'capabilities' in videoProfile:
                    for name in videoProfile['capabilities']:
                        structDef = registry.getChainableStructDef(name, 'VkVideoCapabilitiesKHR')
                        if structDef not in videoCapability:
                            videoCapability.append(structDef)
                self.eliminateAliases(videoCapability)

                # Video format property struct types
                videoFormat = self.videoFormat[videoProfileName] = []
                if 'formats' in videoProfile:
                    for format in videoProfile['formats']:
                        for name in format:
                            structDef = registry.getChainableStructDef(name, 'VkVideoFormatPropertiesKHR')
                            if structDef not in videoFormat:
                                videoFormat.append(structDef)
                self.eliminateAliases(videoFormat)


    def eliminateAliases(self, structs):
        structNames = []
        duplicates = []
        # Collect duplicates
        for structDef in structs:
            if structDef.name in structNames:
                duplicates.append(structDef)
            structNames.append(structDef.aliases)
        # Remove duplicates
        for duplicate in duplicates:
            structs.remove(duplicate)


class VulkanProfile():
    def __init__(self, registry, json_profiles_database, json_profile_key, json_profile_value, json_capabilities):
        self.registry = registry
        self.key = json_profile_key
        self.label = json_profile_value['label']
        self.description = json_profile_value['description']
        self.version = json_profile_value['version']
        self.apiVersion = json_profile_value['api-version']
        self.apiVersionNumber = VulkanVersionNumber(self.apiVersion, registry.api)
        self.fallbacks = json_profile_value.get('fallback')
        self.versionRequirements = []
        self.referencedCapabilities = json_profile_value['capabilities']

        profile_list = json_profiles_database.collectRequiredProfiles(json_profile_key)

        self.profileRequirements = []
        for profile in profile_list:
            if profile != json_profile_key:
                self.profileRequirements.append(profile)
        self.extensionRequirements = []

        collected_json_capabilities = []
        collected_json_capabilities.extend(json_profiles_database.collectProfileCapabilities(profile_list))

        self.merge_capabilities = VulkanProfileCapabilities(registry, json_profile_key, json_profile_value, '"MERGED"', collected_json_capabilities, True, False)
        self.doc_capabilities = VulkanProfileCapabilities(registry, json_profile_key, json_profile_value, '"DOC"', collected_json_capabilities, False, True)
        self.split_capabilities = dict()
        for referenced_capability in json_profile_value['capabilities']:
            # When we have multiple possible capabilities blocks, we load them all but effectively the API library can't effectively implement this behavior.
            if type(referenced_capability).__name__ == 'list':
                for capability_key in referenced_capability:
                    self.split_capabilities[capability_key] = VulkanProfileCapabilities(registry, json_profile_key, json_profile_value, capability_key, json_capabilities[capability_key], False, False)
            elif referenced_capability in json_capabilities:
                self.split_capabilities[referenced_capability] = VulkanProfileCapabilities(registry, json_profile_key, json_profile_value, referenced_capability, json_capabilities[referenced_capability], False, False)

        self.structs = VulkanProfileStructs(registry, self.split_capabilities)
        self.multiple_variants = self.checkMultipleVariants(json_profile_value)
        self.collectCompileTimeRequirements()
        self.validate()

    def checkMultipleVariants(self, json_profile_value):
        for cap_key in json_profile_value['capabilities']:
            if type(cap_key).__name__ == 'list':
                return True
        return False

    def collectCompileTimeRequirements(self):
        # Add API version to the list of requirements
        versionName = self.apiVersionNumber.versionName
        if versionName in self.registry.versions:
            self.versionRequirements.append(versionName)
        else:
            Log.f("No version '{0}' found in registry required by profile '{1}'".format(str(self.apiVersionNumber), self.key))

        # Add any required extension to the list of requirements
        for key, value in self.split_capabilities.items():
            for extName in value.extensions:
                if extName in self.registry.extensions:
                    self.extensionRequirements.append(extName)
                else:
                    Log.f("Extension '{0}' required by profile '{1}' does not exist".format(extName, self.key))


    def validate(self):
        self.validateStructDependencies('MERGE', self.merge_capabilities)
        for capabilities_key, capabilities_value in self.split_capabilities.items():
            self.validateStructDependencies(capabilities_key, capabilities_value)
            self.validateVideoProfiles(capabilities_key, capabilities_value)


    def validateStructDependencies(self, capabilities_key, capabilities_value):
        for feature in capabilities_value.features:
            self.validateStructDependency(capabilities_key, capabilities_value, feature)

        for prop in capabilities_value.properties:
            self.validateStructDependency(capabilities_key, capabilities_value, prop)

        for queueFamilyData in capabilities_value.queueFamiliesProperties:
            for queueFamilyProp in queueFamilyData:
                self.validateStructDependency(capabilities_key, capabilities_value, queueFamilyProp)

        for videoProfile in capabilities_value.videoProfiles:
            for videoProfileInfoStruct in videoProfile['profile']:
                self.validateStructDependency(capabilities_key, capabilities_value, videoProfileInfoStruct)
            if 'capabilities' in videoProfile:
                for videoCapabilityStruct in videoProfile['capabilities']:
                    self.validateStructDependency(capabilities_key, capabilities_value, videoCapabilityStruct)
            if 'formats' in videoProfile:
                for videoFormat in videoProfile['formats']:
                    for videoFormatPropStruct in videoFormat:
                        self.validateStructDependency(capabilities_key, capabilities_value, videoFormatPropStruct)


    def validateStructDependency(self, capabilities_key, capabilities_value, structName):
        if structName in self.registry.structs:
            structDef = self.registry.structs[structName]
            depFound = False

            # Check if the required API version defines this struct
            if structDef.definedByVersion != None and structDef.definedByVersion <= self.apiVersionNumber:
                depFound = True

            # Check if any required extension defines this struct
            for definedByExtension in structDef.definedByExtensions:
                if definedByExtension in capabilities_value.extensions:
                    depFound = True
                    break

            if not depFound:
                if structDef.definedByExtensions and structDef.definedByVersion:
                    Log.e("Unexpected required struct '{0}' in profile '{1}', this struct requires API version '{2}' or an extension '{3}' which are not required in the capabilities block '{4}'.\n".format(structName, self.key, structDef.definedByVersion, ', '.join(structDef.definedByExtensions), capabilities_key))
                elif structDef.definedByExtensions:
                    Log.e("Unexpected required struct '{0}' in profile '{1}', this struct requires an extension '{2}' which is not required in the capabilities block '{3}'.\n".format(structName, self.key, ', '.join(structDef.definedByExtensions), capabilities_key))
                elif structDef.definedByVersion:
                    Log.e("Unexpected required struct '{0}' in profile '{1}', this struct requires API version '{2}' which is not required in the capabilities block '{3}'.\n".format(structName, self.key, structDef.definedByVersion, capabilities_key))
                else:
                    Log.e("Unexpected required struct '{0}' in capabilities block '{1}' of profile '{2}'.\n".format(structName, capabilities_key, self.key))
        else:
            Log.f("Struct '{0}' in profile '{1}' does not exist in the registry".format(structName, self.key))


    def validateVideoProfiles(self, capabilities_key, capabilities_value):
        def isStructInList(structName, structList):
            if structName in structList:
                return True
            else:
                # Check also for possible aliases
                for alias in self.registry.structs[structName].aliases:
                    if alias in structList:
                        return True
            return False

        for videoProfile in capabilities_value.videoProfiles:
            # This already validates that the video profile description is valid
            videoCodec = self.registry.getVideoCodecFromVideoProfile(videoProfile)
            videoProfileName = self.registry.getVideoProfileNameFromVideoProfile(videoProfile)

            # Validate that the video profile description contains only video profile info structures allowed by the video codec
            if 'profile' in videoProfile:
                for videoProfileInfoStruct in videoProfile['profile']:
                    if not isStructInList(videoProfileInfoStruct, ['VkVideoProfileInfoKHR'] + list(videoCodec.profileStructs.keys())):
                        Log.e("Unexpected video profile info structure '{0}' in video profile '{1}' in profile '{2}'.".format(videoProfileInfoStruct, videoProfileName, self.key))

            # Validate that the video capabilities contain only video capability structures allowed by the video codec
            if 'capabilities' in videoProfile:
                for videoCapabilityStruct in videoProfile['capabilities']:
                    if not isStructInList(videoCapabilityStruct, ['VkVideoCapabilitiesKHR'] + list(videoCodec.capabilities.keys())):
                        Log.e("Unexpected video capability structure '{0}' in video profile '{1}' in profile '{2}'.".format(videoCapabilityStruct, videoProfileName, self.key))

            # Validate that the video format properties of all video formats contain only video format properties structures allowed by
            # the video format categories of the video codec that the video format falls into
            if 'formats' in videoProfile:
                for videoFormat in videoProfile['formats']:
                    videoFormatCategories = videoCodec.getVideoFormatCategoriesForFormat(videoFormat, videoProfile['capabilities'] if 'capabilities' in videoProfile else {}, self.registry)
                    for videoFormatCategory in videoFormatCategories:
                        for videoFormatPropStruct in videoFormat:
                            if not isStructInList(videoFormatPropStruct, ['VkVideoFormatPropertiesKHR'] + list(videoFormatCategory.properties.keys())):
                                Log.e("Unexpected video format properties structure '{0}' for video format category '{1}' in video profile '{2}' in profile '{3}'.".format(videoFormatPropStruct, videoFormatCategory.name, videoProfileName, self.key))


    def generatePrivateImpl(self, debugMessages):
        uname = self.key.upper()
        gen = ('#ifdef {0}\n'
               'namespace {1} {{\n').format(self.key, uname)

        gen += self.gen_structTypeData()

        if not self.multiple_variants:
            gen += self.gen_extensionData(self.merge_capabilities, 'instance')
            gen += self.gen_extensionData(self.merge_capabilities, 'device')
            gen += self.gen_structDesc(self.merge_capabilities, debugMessages)
            gen += self.gen_videoProfileStructDesc(self.merge_capabilities, debugMessages)
        gen += '\n'

        gen += 'namespace blocks {\n'
        for key, value in self.split_capabilities.items():
            gen += ('namespace {0} {{\n').format(key)
            gen += self.gen_extensionData(value, 'instance')
            gen += self.gen_extensionData(value, 'device')
            gen += self.gen_structDesc(value, debugMessages)
            gen += self.gen_videoProfileStructDesc(value, debugMessages)
            gen += ('}} // namespace {0}\n').format(key)
        gen += '} // namespace blocks\n'

        gen += ('}} // namespace {1}\n'
                '#endif // {0}\n\n').format(self.key, uname)
        return gen

    def gen_extensionData(self, capabilities, type):
        foundExt = False
        gen = '\n'
        gen += 'static const VkExtensionProperties {0}Extensions[] = {{\n'.format(type)
        for extName, specVer in sorted(capabilities.extensions.items()):
            extInfo = self.registry.extensions[extName]
            if extInfo.type == type:
                gen += '    VkExtensionProperties{{ {0}_EXTENSION_NAME, {1} }},\n'.format(extInfo.upperCaseName, specVer)
                foundExt = True
        gen += '};\n'
        return gen if foundExt else ''

    def gen_structTypeData(self, structDefs = None, name = None):
        gen = ''
        if structDefs == None:
            gen += self.gen_structTypeData(self.structs.feature, 'feature')
            gen += self.gen_structTypeData(self.structs.property, 'property')
            gen += self.gen_structTypeData(self.structs.queueFamily, 'queueFamily')
            gen += self.gen_structTypeData(self.structs.format, 'format')
        else:
            if structDefs:
                gen += ('\n'
                        'static const VkStructureType {0}StructTypes[] = {{\n').format(name)
                for structDef in structDefs:
                    gen += '    {0},\n'.format(structDef.sType)
                gen += '};\n'
        return gen


    def gen_listValue(self, values, isEnum = True):
        gen = ''
        if isEnum:
            gen += '('
        else:
            gen += '{ '

        separator = ''
        if values != None and len(values) > 0:
            for value in values:
                gen += separator + str(value)
                if isEnum:
                    separator = ' | '
                else:
                    separator = ', '
        elif isEnum:
            gen += '0'

        if isEnum:
            gen += ')'
        else:
            gen += ' }'
        return gen


    def gen_structFill(self, fmt, structDef, var, values):
        gen = ''
        for member, value in sorted(values.items()):
            if member in structDef.members:
                if type(value) == dict:
                    # Nested structure
                    memberDef = self.registry.structs.get(structDef.members[member].type)
                    if memberDef != None:
                        gen += self.gen_structFill(fmt, memberDef, var + member + '.', value)
                    else:
                        Log.f("Member '{0}' in structure '{1}' is not a struct".format(member, structDef.name))

                elif type(value) == list:
                    # Some sort of list (enums or integer/float list for structure initialization)
                    if len(value) == 0:
                        # If list is empty then ignore
                        continue
                    if structDef.members[member].isArray:
                        if not isinstance(self.registry.evalArraySize(structDef.members[member].arraySize), int):
                            Log.f("Unsupported array member '{0}' in structure '{1}'".format(member, structDef.name) +
                                  "(currently only 1D non-dynamic arrays are supported in this context)")
                        # If it's an array we have to generate per-element assignment code
                        for i, v in enumerate(value):
                            if type(v) == float:
                                if structDef.members[member].type == 'double':
                                    gen += fmt.format('{0}{1}[{2}] = {3}'.format(var, member, i, v))
                                else:
                                    gen += fmt.format('{0}{1}[{2}] = {3}f'.format(var, member, i, v))
                            else:
                                gen += fmt.format('{0}{1}[{2}] = {3}'.format(var, member, i, v))
                    else:
                        # For enums and struct initialization, most of the code can be shared
                        isEnum = isinstance(value[0], str)
                        if isEnum:
                            # For enums we only add bits
                            genAssign = '{0}{1} |= '.format(var, member)
                        else:
                            genAssign = '{0}{1} = '.format(var, member)
                        genAssign += '{0}'.format(self.gen_listValue(value, isEnum))
                        gen += fmt.format(genAssign)
                elif type(value) == float:
                    if structDef.members[member].type == 'double':
                        gen += fmt.format('{0}{1} = {2}'.format(var, member, value))
                    else:
                        gen += fmt.format('{0}{1} = {2}f'.format(var, member, value))
                elif type(value) == bool:
                    # Boolean
                    gen += fmt.format('{0}{1} = {2}'.format(var, member, 'VK_TRUE' if value else 'VK_FALSE'))

                else:
                    # Everything else
                    gen += fmt.format('{0}{1} = {2}'.format(var, member, value))
            else:
                Log.f("No member '{0}' in structure '{1}'".format(member, structDef.name))
        return gen


    def gen_structCompare(self, fmt, structDef, var, values, parentLimittype = None):
        gen = ''
        for member, value in sorted(values.items()):
            if member in structDef.members:
                limittype = structDef.members[member].limittype
                membertype = structDef.members[member].type
                if limittype == None:
                    # Use parent's limit type
                    limittype = parentLimittype

                if limittype == 'bitmask':
                    # Compare bitmask by checking if device value contains every bit of profile value
                    comparePredFmt = 'vpCheckFlags({0}, {1})'
                elif limittype == 'bits':
                    # Compare max limit by checking if device value is greater than or equal to profile value
                    comparePredFmt = '{0} >= {1}'
                elif limittype == 'max':
                    # Compare max limit by checking if device value is greater than or equal to profile value
                    comparePredFmt = '{0} >= {1}'
                elif limittype == 'max,pot' or limittype == 'pot,max':
                    # Compare max limit by checking if device value is greater than or equal to profile value
                    if (membertype == 'float' or membertype == 'double'):
                        comparePredFmt = [ '{0} >= {1}' ]
                    else:
                        comparePredFmt = [ '{0} >= {1}', '({0} & ({0} - 1)) == 0' ]
                elif limittype == 'bits':
                    # Behaves like max, but smaller values are allowed
                    comparePredFmt = '{0} >= {1}'
                elif limittype == 'min':
                    # Compare min limit by checking if device value is less than or equal to profile value
                    comparePredFmt = '{0} <= {1}'
                elif limittype == 'pot':
                    if (membertype == 'float' or membertype == 'double'):
                        comparePredFmt = [ 'isPowerOfTwo({0})' ]
                    else:
                        comparePredFmt = [ '({0} & ({0} - 1)) == 0' ]
                elif limittype == 'min,pot' or limittype == 'pot,min':
                    # Compare min limit by checking if device value is less than or equal to profile value and if the value is a power of two
                    if (membertype == 'float' or membertype == 'double'):
                        comparePredFmt = [ '{0} <= {1}', 'isPowerOfTwo({0})' ]
                    else:
                        comparePredFmt = [ '{0} <= {1}', '({0} & ({0} - 1)) == 0' ]
                elif limittype == 'min,mul' or limittype == 'mul,min':
                    # Compare min limit by checking if device value is less than or equal to profile value and a multiple of profile value
                    if (membertype == 'float' or membertype == 'double'):
                        comparePredFmt = [ '{0} <= {1}', 'isMultiple({1}, {0})' ]
                    else:
                        comparePredFmt = [ '{0} <= {1}', '({1} % {0}) == 0' ]
                elif limittype == 'range':
                    # Compare range limit by checking if device range is larger than or equal to profile range
                    comparePredFmt = [ '{0} <= {1}', '{0} >= {1}' ]
                elif limittype == 'exact' or limittype == 'struct':
                    # Compare exact and struct values with equality
                    comparePredFmt = '{0} == {1}'
                elif limittype is None or limittype == 'noauto':
                    # Compare everything else with equality
                    comparePredFmt = '{0} == {1}'
                else:
                    Log.f("Unsupported limittype '{0}' in member '{1}' of structure '{2}'".format(limittype, member, structDef.name))

                if type(value) == dict:
                    # Nested structure
                    memberDef = self.registry.structs.get(structDef.members[member].type)
                    if memberDef != None:
                        gen += self.gen_structCompare(fmt, memberDef, var + member + '.', value, limittype)
                    else:
                        Log.f("Member '{0}' in structure '{1}' is not a struct".format(member, structDef.name))

                elif type(value) == list:
                    # Some sort of list (enums or integer/float list for structure initialization)
                    if len(value) == 0:
                        # If list is empty then ignore
                        continue
                    if structDef.members[member].isArray:
                        if not isinstance(self.registry.evalArraySize(structDef.members[member].arraySize), int):
                            Log.f("Unsupported array member '{0}' in structure '{1}'".format(member, structDef.name) +
                                  "(currently only 1D non-dynamic arrays are supported in this context)")
                        # If it's an array we have to generate per-element comparison code
                        for i in range(len(value)):
                            if limittype == 'range':
                                gen += fmt.format(comparePredFmt[i].format('{0}{1}[{2}]'.format(var, member, i), value[i]))
                            else:
                                gen += fmt.format(comparePredFmt.format('{0}{1}[{2}]'.format(var, member, i), value[i]))
                    else:
                        # Enum flags and basic structs can be compared directly
                        isEnum = isinstance(value[0], str)
                        gen += fmt.format(comparePredFmt.format('{0}{1}'.format(var, member), self.gen_listValue(value, isEnum)))

                elif type(value) == bool:
                    # Boolean
                    gen += fmt.format(comparePredFmt.format('{0}{1}'.format(var, member), 'VK_TRUE' if value else 'VK_FALSE'))

                else:
                    # Everything else
                    if type(comparePredFmt) == list:
                        for i in range(len(comparePredFmt)):
                            gen += fmt.format(comparePredFmt[i].format('{0}{1}'.format(var, member), value))
                    elif comparePredFmt is not None:
                        gen += fmt.format(comparePredFmt.format('{0}{1}'.format(var, member), value))
            else:
                Log.f("No member '{0}' in structure '{1}'".format(member, structDef.name))
        return gen


    def gen_structFunc(self, structDefs, caps, func, fmt, debugMessages = False):
        gen = ''

        hasData = False

        gen += ('            switch (p->sType) {\n')

        for structDef in structDefs:
            paramList = []

            # Fill VkPhysicalDeviceFeatures into VkPhysicalDeviceFeatures2[KHR]
            if structDef.name in ['VkPhysicalDeviceFeatures2', 'VkPhysicalDeviceFeatures2KHR']:
                innerCap = caps.get('VkPhysicalDeviceFeatures')
                if innerCap:
                    paramList.append((self.registry.structs['VkPhysicalDeviceFeatures'], '->features.', innerCap))

            # Fill VkPhysicalDeviceProperties into VkPhysicalDeviceProperties2[KHR]
            if structDef.name in ['VkPhysicalDeviceProperties2', 'VkPhysicalDeviceProperties2KHR']:
                innerCap = caps.get('VkPhysicalDeviceProperties')
                if innerCap:
                    paramList.append((self.registry.structs['VkPhysicalDeviceProperties'], '->properties.', innerCap))

            # Fill VkQueueFamilyProperties into VkQueueFamilyProperties2[KHR]
            if structDef.name in ['VkQueueFamilyProperties2', 'VkQueueFamilyProperties2KHR']:
                innerCap = caps.get('VkQueueFamilyProperties')
                if innerCap:
                    paramList.append((self.registry.structs['VkQueueFamilyProperties'], '->queueFamilyProperties.', innerCap))

            # Fill VkFormatProperties into VkFormatProperties2[KHR]
            if structDef.name in ['VkFormatProperties2', 'VkFormatProperties2KHR']:
                innerCap = caps.get('VkFormatProperties')
                if innerCap:
                    paramList.append((self.registry.structs['VkFormatProperties'], '->formatProperties.', innerCap))

            # Fill all other structures directly
            if structDef.name in caps:
                paramList.append((structDef, '->', caps[structDef.name]))

            # Use variable names in the debug version of the library that can be later prettified
            if debugMessages:
                varName = 'prettify_' + structDef.name
            else:
                varName = 's'

            if paramList:
                hasLocalCastPtr = False # track if we have defined local pointer yet
                gen += '                case {0}: {{\n'.format(structDef.sType)
                for params in paramList:
                    genAssign = func('                    ' + fmt, params[0], varName + params[1], params[2])
                    if genAssign != '':
                        if hasLocalCastPtr == False: 
                            # only define pointer in the event that it has data
                            gen += '                    {0}* {1} = static_cast<{0}*>(static_cast<void*>(p));\n'.format(structDef.name, varName)
                            hasLocalCastPtr = True
                        hasData = True
                        gen += genAssign
                gen += '                } break;\n'

        gen += ('                default: break;\n'
                '            }\n')
        return gen if hasData else ''


    def gen_structChainerFunc(self, structDefs, baseStruct):
        gen = '    [](VkBaseOutStructure* p, void* pUser, PFN_vpStructChainerCb pfnCb) {\n'
        if structDefs:
            pNext = 'nullptr'
            for structDef in structDefs:
                if structDef.name != baseStruct:
                    varName = structDef.name[2].lower() + structDef.name[3:]
                    gen += '        {0} {1}{{ {2}, {3} }};\n'.format(structDef.name, varName, structDef.sType, pNext)
                    pNext = '&' + varName
            gen += '        p->pNext = static_cast<VkBaseOutStructure*>(static_cast<void*>({0}));\n'.format(pNext)

        gen += ('        pfnCb(p, pUser);\n'
                '    },\n')
        return gen

    def gen_structFeaturesChainerFunc(self, structDefs, caps, baseStruct):
        gen = '    [](VkBaseOutStructure* p, void* pUser, PFN_vpStructChainerCb pfnCb) {\n'
        if structDefs:
            pNext = 'nullptr'
            for structDef in structDefs:
                if structDef.name in caps:
                    if structDef.name != baseStruct:
                        varName = structDef.name[2].lower() + structDef.name[3:]
                        gen += '        {0} {1}{{ {2}, {3} }};\n'.format(structDef.name, varName, structDef.sType, pNext)
                        pNext = '&' + varName
            gen += '        p->pNext = static_cast<VkBaseOutStructure*>(static_cast<void*>({0}));\n'.format(pNext)

        gen += ('        pfnCb(p, pUser);\n'
                '    },\n')
        return gen


    def gen_structArrayChainerFunc(self, structDefs, baseStruct):
        gen = '    [](uint32_t count, VkBaseOutStructure* p, void* pUser, PFN_vpStructArrayChainerCb pfnCb) {\n'
        if len(structDefs) > 0:
            gen += '        struct ExtStructs {\n'
            for structDef in structDefs:
                if structDef.name != baseStruct:
                    varName = structDef.name[2].lower() + structDef.name[3:]
                    gen += '            {0} {1};\n'.format(structDef.name, varName)
            gen += '        };\n'
            gen += '        std::vector<ExtStructs> ext_structs{};\n'
            gen += '        if (count > 0) {\n'
            gen += '            ext_structs.resize(count);\n'
            gen += '            {0}* pArray = static_cast<{0}*>(static_cast<void*>(p));\n'.format(baseStruct)
            gen += '            for (uint32_t i = 0; i < count; ++i) {\n'
            pNext = 'nullptr'
            for structDef in structDefs:
                if structDef.name != baseStruct:
                    varName = structDef.name[2].lower() + structDef.name[3:]
                    gen += '                ext_structs[i].{0} = {1}{{ {2}, {3} }};\n'.format(varName, structDef.name, structDef.sType, pNext)
                    pNext = '&ext_structs[i].' + varName
            gen += '                pArray[i].pNext = static_cast<VkBaseOutStructure*>(static_cast<void*>({0}));\n'.format(pNext)
            gen += '            }\n'
            gen += '        }\n'
        gen += ('        pfnCb(count, p, pUser);\n'
                '    },\n')
        return gen


    def gen_structDesc(self, capabilities, debugMessages):
        gen = ''

        fillFmt = '{0};\n'
        cmpFmt = 'ret = ret && ({0});\n'

        # Feature descriptor
        if debugMessages:
            cmpFmtFeatures = 'ret = ret && ({0}); VP_DEBUG_COND_MSG(!({0}), "Unsupported feature condition: {0}");\n'
        else:
            cmpFmtFeatures = cmpFmt

        gen += ('\n'
                'static const VpFeatureDesc featureDesc = {\n'
                '    [](VkBaseOutStructure* p) { (void)p;\n')
        gen += self.gen_structFunc(self.structs.feature, capabilities.features, self.gen_structFill, fillFmt)
        gen += ('    },\n'
                '    [](VkBaseOutStructure* p) -> bool { (void)p;\n'
                '        bool ret = true;\n')
        gen += self.gen_structFunc(self.structs.feature, capabilities.features, self.gen_structCompare, cmpFmtFeatures, debugMessages)
        gen += ('        return ret;\n'
                '    }\n'
                '};\n')

        # Property descriptor
        if debugMessages:
            cmpFmtProperties = 'ret = ret && ({0}); VP_DEBUG_COND_MSG(!({0}), "Unsupported properties condition: {0}");\n'
        else:
            cmpFmtProperties = cmpFmt

        gen += ('\n'
                'static const VpPropertyDesc propertyDesc = {\n'
                '    [](VkBaseOutStructure* p) { (void)p;\n')
        gen += self.gen_structFunc(self.structs.property, capabilities.properties, self.gen_structFill, fillFmt)
        gen += ('    },\n'
                '    [](VkBaseOutStructure* p) -> bool { (void)p;\n'
                '        bool ret = true;\n')
        gen += self.gen_structFunc(self.structs.property, capabilities.properties, self.gen_structCompare, cmpFmtProperties, debugMessages)
        gen += ('        return ret;\n'
                '    }\n'
                '};\n')

        # Queue family descriptor
        if self.structs.queueFamily and capabilities.queueFamiliesProperties:
            gen += ('\n'
                    'static const VpQueueFamilyDesc queueFamilyDesc[] = {\n')
            for queueFamilyCaps in capabilities.queueFamiliesProperties:
                gen += ('    {\n'
                        '        [](VkBaseOutStructure* p) { (void)p;\n')
                gen += self.gen_structFunc(self.structs.queueFamily, queueFamilyCaps, self.gen_structFill, fillFmt)
                gen += ('        },\n'
                        '        [](VkBaseOutStructure* p) -> bool { (void)p;\n'
                        '            bool ret = true;\n')
                gen += self.gen_structFunc(self.structs.queueFamily, queueFamilyCaps, self.gen_structCompare, cmpFmt)
                gen += ('            return ret;\n'
                        '        }\n'
                        '    },\n')
            gen += ('};\n')

        # Format descriptor
        if capabilities.formats:
            gen += ('\n'
                    'static const VpFormatDesc formatDesc[] = {\n')
            for formatName, formatCaps in sorted(capabilities.formats.items()):
                if debugMessages:
                    cmpFmtFormat = 'ret = ret && ({0}); VP_DEBUG_COND_MSG(!({0}), "Unsupported format condition for ' + formatName + ': {0}");\n'
                else:
                    cmpFmtFormat = cmpFmt

                gen += ('    {{\n'
                        '        {0},\n'
                        '        [](VkBaseOutStructure* p) {{ (void)p;\n').format(formatName)
                gen += self.gen_structFunc(self.structs.format, formatCaps, self.gen_structFill, fillFmt)
                gen += ('        },\n'
                        '        [](VkBaseOutStructure* p) -> bool { (void)p;\n'
                        '            bool ret = true;\n')
                gen += self.gen_structFunc(self.structs.format, formatCaps, self.gen_structCompare, cmpFmtFormat, debugMessages)
                gen += ('            return ret;\n'
                        '        }\n'
                        '    },\n')
            gen += '};\n'

        # Structure chaining descriptors
        gen += ('\n'
                'static const VpStructChainerDesc chainerDesc = {\n')
        gen += self.gen_structFeaturesChainerFunc(self.structs.feature, capabilities.features, 'VkPhysicalDeviceFeatures2KHR')
        gen += self.gen_structChainerFunc(self.structs.property, 'VkPhysicalDeviceProperties2KHR')
        gen += self.gen_structArrayChainerFunc(self.structs.queueFamily, 'VkQueueFamilyProperties2KHR')
        gen += self.gen_structChainerFunc(self.structs.format, 'VkFormatProperties2KHR')
        gen += '};\n'

        # If debug messages are needed do further prettifying (warning: obscure regular expressions follow)
        if debugMessages:
            # Prettify structure references in non-bitmask comparisons
            gen = re.sub(r"(VP_DEBUG_COND_MSG\([^,]+[^:]+: )prettify_Vk([^\-]+)\->([^\)]+\))", r"\1Vk\2::\3", gen)
            # Prettify bitmask comparisons
            gen = re.sub(r"(VP_DEBUG_COND_MSG\([^,]+[^:]+: )vpCheckFlags\(prettify_Vk([^\-]+)\->([^,]+), ([^\)]+)\)", r"\1Vk\2::\3 contains \4", gen)

        return gen

    def gen_videoProfileStructDesc(self, capabilities, debugMessages):
        if len(capabilities.videoProfiles) == 0:
            return ''

        gen = ''

        fillFmt = '{0};\n'
        cmpFmt = 'ret = ret && ({0});\n'

        videoProfileIndex = 0
        for videoProfileName, videoProfile in capabilities.videoProfilesByName.items():
            videoProfileIndex += 1

            gen += '\nnamespace video_profile_{0} {{\n'.format(videoProfileIndex)

            gen += self.gen_structTypeData(self.structs.videoProfileInfo[videoProfileName], 'info')
            gen += self.gen_structTypeData(self.structs.videoCapability[videoProfileName], 'capability')
            gen += self.gen_structTypeData(self.structs.videoFormat[videoProfileName], 'format')

            # Video profile info descriptor
            gen += ('\n'
                    'static const VpVideoProfileInfoDesc infoDesc = {\n'
                    '    [](VkBaseOutStructure* p) { (void)p;\n')
            gen += self.gen_structFunc(self.structs.videoProfileInfo[videoProfileName], videoProfile['profile'], self.gen_structFill, fillFmt)
            gen += ('    },\n'
                    '    [](VkBaseOutStructure* p) -> bool { (void)p;\n'
                    '        bool ret = true;\n')
            gen += self.gen_structFunc(self.structs.videoProfileInfo[videoProfileName], videoProfile['profile'], self.gen_structCompare, cmpFmt)
            gen += ('        return ret;\n'
                    '    }\n'
                    '};\n')

            # Video capability descriptor
            if debugMessages:
                cmpFmtCapabilities = 'ret = ret && ({0}); VP_DEBUG_COND_MSG(!({0}), "Unsupported video capability condition: {0}");\n'
            else:
                cmpFmtCapabilities = cmpFmt

            gen += ('\n'
                    'static const VpVideoCapabilityDesc capabilityDesc = {\n'
                    '    [](VkBaseOutStructure* p) { (void)p;\n')
            gen += self.gen_structFunc(self.structs.videoCapability[videoProfileName], videoProfile['capabilities'], self.gen_structFill, fillFmt)
            gen += ('    },\n'
                    '    [](VkBaseOutStructure* p) -> bool { (void)p;\n'
                    '        bool ret = true;\n')
            gen += self.gen_structFunc(self.structs.videoCapability[videoProfileName], videoProfile['capabilities'], self.gen_structCompare, cmpFmtCapabilities, debugMessages)
            gen += ('        return ret;\n'
                    '    }\n'
                    '};\n')

            # Video format descriptor
            if 'formats' in videoProfile and len(videoProfile['formats']) > 0:
                gen += ('\n'
                        'static const VpVideoFormatDesc formatDesc[] = {\n')
                for format in videoProfile['formats']:
                    gen += ('    {\n'
                            '        [](VkBaseOutStructure* p) { (void)p;\n')
                    gen += self.gen_structFunc(self.structs.videoFormat[videoProfileName], format, self.gen_structFill, fillFmt)
                    gen += ('        },\n'
                            '        [](VkBaseOutStructure* p) -> bool { (void)p;\n'
                            '            bool ret = true;\n')
                    gen += self.gen_structFunc(self.structs.videoFormat[videoProfileName], format, self.gen_structCompare, cmpFmt)
                    gen += ('            return ret;\n'
                            '        }\n'
                            '    },\n')
                gen += '};\n'

            # Structure chaining descriptors
            gen += ('\n'
                    'static const VpVideoProfileStructChainerDesc chainerDesc = {\n')
            gen += self.gen_structChainerFunc(self.structs.videoProfileInfo[videoProfileName], 'VkVideoProfileInfoKHR')
            gen += self.gen_structChainerFunc(self.structs.videoCapability[videoProfileName], 'VkVideoCapabilitiesKHR')
            gen += self.gen_structArrayChainerFunc(self.structs.videoFormat[videoProfileName], 'VkVideoFormatPropertiesKHR')
            gen += '};\n'

            gen += '}} // namespace video_profile_{0}\n'.format(videoProfileIndex)

        # Video profile descriptor
        gen += ('\n'
                'static const VpVideoProfileDesc videoProfileDesc[] = {\n')

        def gen_dataArrayInfo(condition, namespace, name):
            if condition:
                return 'static_cast<uint32_t>(std::size({0}::{1})), {0}::{1},\n'.format(namespace, name)
            else:
                return '0, nullptr,\n'

        videoProfileIndex = 0
        for videoProfileName, videoProfile in capabilities.videoProfilesByName.items():
            videoProfileIndex += 1
            namespace = 'video_profile_{0}'.format(videoProfileIndex)

            gen += ('    {{\n'
                    '        {{ "{0}" }},\n').format(videoProfileName)
            gen += '        ' + gen_dataArrayInfo(self.structs.videoProfileInfo[videoProfileName], namespace, 'infoStructTypes')
            gen += '        {0}::infoDesc,\n'.format(namespace)
            gen += '        ' + gen_dataArrayInfo(self.structs.videoCapability[videoProfileName], namespace, 'capabilityStructTypes')
            gen += '        {0}::capabilityDesc,\n'.format(namespace)
            gen += '        ' + gen_dataArrayInfo(self.structs.videoFormat[videoProfileName], namespace, 'formatStructTypes')
            gen += '        ' + gen_dataArrayInfo('formats' in videoProfile and len(videoProfile['formats']) > 0, namespace, 'formatDesc')
            gen += '        {0}::chainerDesc,\n'.format(namespace)
            gen += '    },\n'

        gen += '};\n'

        return gen

class VulkanProfilesDatabase():
    def __init__(self):
        self.json_files = [] # json_root[]

    def recurseRequiredProfiles(self, json_files, results, profile_key):
        for json_file in json_files:
            for json_profile_key, json_profile_value in json_file['profiles'].items():
                if profile_key == json_profile_key:
                    json_profiles_required = json_profile_value.get('profiles')
                    if json_profiles_required is None:
                        break # This profile doesn't have required profiles
                    for json_profile_required in json_profiles_required:
                        self.recurseRequiredProfiles(json_files, results, json_profile_required)
                    break
        results.append(profile_key)

    def collectRequiredProfiles(self, profile_key):
        results = []
        self.recurseRequiredProfiles(self.json_files, results, profile_key)
        if len(results) > 1:
            Log.i('Required profiles by the {0} profile:'.format(profile_key))
            for result in results:
                if result != profile_key:
                    Log.i('- {0}'.format(result))

        else:
            Log.i('Required profiles by the {0} profile: None'.format(profile_key))
        return results

    def gatherProfileCapabilities(self, json_profile_key, json_profile_value, json_capabilities_value):
        capabilities_list = []

        for cap_key in json_profile_value['capabilities']:
            # When we have multiple possible capabilities blocks, we load them all but effectively the API library can't effectively implement this behavior.
            if type(cap_key).__name__ == 'list':
                for cap_key_case in cap_key:
                    Log.i('- {0}::{1}'.format(json_profile_key, cap_key_case))
                    capabilities_list.append(json_capabilities_value[cap_key_case])
            elif cap_key in json_capabilities_value:
                capabilities_list.append(json_capabilities_value[cap_key])
                Log.i('- {0}::{1}'.format(json_profile_key, cap_key))

        return capabilities_list

    def collectProfileCapabilities(self, profile_requirements):
        Log.i('Required capabilities blocks by the {0} profile:'.format(profile_requirements[0]))
        
        capabilities_list = []
        for required_profile in profile_requirements:
            for json_file in self.json_files:
                found = False
                for json_profile_key, json_profile_value in json_file['profiles'].items():
                    if required_profile == json_profile_key:
                        capabilities = self.gatherProfileCapabilities(json_profile_key, json_profile_value, json_file['capabilities'])
                        capabilities_list.extend(capabilities)
                        found = True
                        break
                if found:
                    break

        return capabilities_list

class VulkanProfilesFiles():
    def __init__(self, registry, profiles_dir, profiles_files, validate, schema):
        self.profiles = dict()
        self.json_profiles_database = VulkanProfilesDatabase()

        dirAbsPath = os.path.abspath(profiles_dir)
        filenames = os.listdir(dirAbsPath)

        for filename in filenames:
            skip_file = False
            if profiles_files:
                if filename not in profiles_files:
                    skip_file = True
            if skip_file:
                continue
            fileAbsPath = os.path.join(dirAbsPath, filename)
            if os.path.isfile(fileAbsPath) and os.path.splitext(filename)[-1] == '.json':
                Log.i("Loading profile file: '{0}'".format(filename))
                with open(fileAbsPath, 'r') as f:
                    json_root = json.load(f)
                    if validate:
                        try:
                            import jsonschema
                            Log.i("Validating profile file: '{0}'".format(filename))
                            jsonschema.validate(json_root, schema)
                        except ModuleNotFoundError:
                            Log.w("`jsonschema` module is not installed, schema validation skip")
                    self.json_profiles_database.json_files.append(json_root)

        for json_file_data in self.json_profiles_database.json_files:
            self.parseProfiles(registry, json_file_data['profiles'], json_file_data['capabilities'])

    def parseProfiles(self, registry, json_profiles, json_caps):
        for json_profile_key, json_profile_value in json_profiles.items():
            Log.i("Registering profile '{0}'".format(json_profile_key))
            if json_profile_key not in self.profiles:
                self.profiles[json_profile_key] = VulkanProfile(registry, self.json_profiles_database, json_profile_key, json_profile_value, json_caps)

class VulkanProfilesLibraryGenerator():
    def __init__(self, registry, input_profiles_files, output_filename, debugMessages = False):
        self.registry = registry
        self.profiles_files = input_profiles_files
        self.debugMessages = debugMessages
        self.outputFilename = output_filename


    def patch_code(self, code):
        # Removes lines with debug messages if they aren't needed
        if self.debugMessages:
            return code
        else:
            lines = code.split('\n')
            patched_lines = []
            for line in lines:
                if not 'VP_DEBUG' in line:
                    patched_lines.append(line)
            return '\n'.join(patched_lines)


    def generate(self, outIncDir, outSrcDir):
        if outSrcDir != None:
            self.generate_h(outIncDir)
            self.generate_cpp(outSrcDir)
        self.generate_hpp(outIncDir)


    def generate_h(self, outDir):
        fileAbsPath = os.path.join(os.path.abspath(outDir), "{0}.h".format(self.outputFilename))
        Log.i("Generating '{0}'...".format(fileAbsPath))
        with open(fileAbsPath, 'w') as f:
            f.write(COPYRIGHT_HEADER)
            f.write(H_HEADER)
            f.write(self.gen_profileDefs())
            f.write(API_DEFS)
            f.write(H_FOOTER)


    def generate_cpp(self, outDir):
        fileAbsPath = os.path.join(os.path.abspath(outDir), "{0}.cpp".format(self.outputFilename))
        Log.i("Generating '{0}'...".format(fileAbsPath))
        with open(fileAbsPath, 'w') as f:
            f.write(COPYRIGHT_HEADER)
            f.write(SHARED_INCLUDE)
            if self.debugMessages:
                f.write('#include <vulkan/debug/{0}.h>\n'.format(self.outputFilename))
                f.write(DEBUG_MSG_CB_DEFINE)
            else:
                f.write('#include <vulkan/{0}.h>\n'.format(self.outputFilename))
            f.write(self.gen_privateImpl())
            f.write(self.gen_publicImpl())


    def generate_hpp(self, outDir):
        fileAbsPath = os.path.join(os.path.abspath(outDir), '{0}.hpp'.format(self.outputFilename))
        Log.i("Generating '{0}'...".format(fileAbsPath))
        with open(fileAbsPath, 'w') as f:
            f.write(COPYRIGHT_HEADER)
            f.write(HPP_HEADER)
            f.write(SHARED_INCLUDE)
            f.write(self.gen_profileDefs())
            f.write(API_DEFS)
            if self.debugMessages:
                f.write(DEBUG_MSG_CB_DEFINE)
            f.write(self.gen_privateImpl())
            f.write(self.gen_publicImpl())


    def gen_profileDefs(self):
        gen = ''
        profiles_ordered = []

        for profile_key, profile_data in sorted(self.profiles_files.profiles.items()):
            for required_profile in profile_data.profileRequirements:
                if required_profile not in profiles_ordered:
                    profiles_ordered.append(required_profile)
            if profile_key not in profiles_ordered:
                profiles_ordered.append(profile_key)

        for profile_key in profiles_ordered:
            profile = self.profiles_files.profiles[profile_key]

            profile_ukey = profile_key.upper()
            gen += '\n'

            # Add prerequisites
            allRequirements = sorted(profile.versionRequirements) + sorted(profile.profileRequirements) + sorted(profile.extensionRequirements)
            if allRequirements:
                for i, requirement in enumerate(allRequirements):
                    if i == 0:
                        gen += '#if '
                    else:
                        gen += '    '

                    gen += 'defined({0})'.format(requirement)

                    if i < len(allRequirements) - 1:
                        gen += ' && \\\n'
                    else:
                        gen += '\n'

            version = profile.apiVersion.split('.')
            major = int(version[0])
            minor = int(version[1])
            patch = int(version[2])
            for required_profile in profile.profileRequirements:
                version = self.profiles_files.profiles[required_profile].apiVersion.split('.')
                major = max(major, int(version[0]))
                minor = max(minor, int(version[1]))
                patch = max(patch, int(version[2]))

            gen += '#define {0} 1\n'.format(profile_key)
            gen += '#define {0}_NAME "{1}"\n'.format(profile_ukey, profile_key)
            gen += '#define {0}_SPEC_VERSION {1}\n'.format(profile_ukey, profile.version)
            gen += '#define {0}_MIN_API_VERSION VK_MAKE_VERSION({1}, {2}, {3})\n'.format(profile_ukey, major, minor, patch)

            if allRequirements:
                gen += '#endif\n'

        return gen


    def gen_privateImpl(self):
        gen = '\n'
        gen += 'namespace detail {\n\n'
        gen += PRIVATE_DEFS
        gen += self.gen_videoProfileEnumerator()
        gen += self.gen_profilePrivateImpl()
        gen += self.gen_profileDescTable()
        gen += self.gen_profileFeatureChain()
        gen += PRIVATE_IMPL_BODY
        gen += '\n} // namespace detail\n'
        return self.patch_code(gen)


    def gen_profilePrivateImpl(self):
        gen = ''
        for _, profile in sorted(self.profiles_files.profiles.items()):
            gen += profile.generatePrivateImpl(self.debugMessages)
        return gen


    def gen_dataArrayInfo(self, condition, name):
        if condition:
            return '        static_cast<uint32_t>(std::size({0})), {0},\n'.format(name)
        else:
            return '        0, nullptr,\n'

    def gen_variants(self, capabilities_key, capabilities_value):
        gen = '                {\n'
        gen += '            ' + ('        "{0}",\n').format(capabilities_value.blockName)
        gen += '            ' + self.gen_dataArrayInfo(capabilities_value.instanceExtensions, 'blocks::{0}::instanceExtensions'.format(capabilities_key))
        gen += '            ' + self.gen_dataArrayInfo(capabilities_value.deviceExtensions, 'blocks::{0}::deviceExtensions'.format(capabilities_key))
        gen += '            ' + self.gen_dataArrayInfo(capabilities_value.features, 'featureStructTypes')
        gen += '                    blocks::{0}::featureDesc,\n'.format(capabilities_key)
        gen += '            ' + self.gen_dataArrayInfo(capabilities_value.properties, 'propertyStructTypes')
        gen += '                    blocks::{0}::propertyDesc,\n'.format(capabilities_key)
        gen += '            ' + self.gen_dataArrayInfo(capabilities_value.queueFamiliesProperties, 'queueFamilyStructTypes')
        gen += '            ' + self.gen_dataArrayInfo(capabilities_value.queueFamiliesProperties, 'blocks::{0}::queueFamilyDesc'.format(capabilities_key))
        gen += '            ' + self.gen_dataArrayInfo(capabilities_value.formats, 'formatStructTypes')
        gen += '            ' + self.gen_dataArrayInfo(capabilities_value.formats, 'blocks::{0}::formatDesc'.format(capabilities_key))
        gen += '                    blocks::{0}::chainerDesc,\n'.format(capabilities_key)
        gen += '            ' + self.gen_dataArrayInfo(capabilities_value.videoProfiles, 'blocks::{0}::videoProfileDesc'.format(capabilities_key))
        gen += '                },\n'
        return gen

    def get_blockName(self, capability_keys):
        blockName = ""
        if type(capability_keys).__name__ == 'list':
            for capability_key in capability_keys:
                blockName += capability_key
                blockName += "_"
        else:
            blockName = capability_keys
        return blockName

    def gen_profileDescTable(self):
        gen = '\n'
        for profile_key, profile_value in sorted(self.profiles_files.profiles.items()):
            profile_ukey = profile_key.upper()
        
            gen += ('#ifdef {0}\n').format(profile_key)
            gen += ('namespace {0} {{\n').format(profile_ukey)

            if not profile_value.multiple_variants:
                gen += '    static const VpVariantDesc mergedCapabilities[] = {\n'
                gen += '        {\n'  # <- new open curly
                gen += ('        {0},\n').format(profile_value.merge_capabilities.blockName)
                gen += self.gen_dataArrayInfo(profile_value.merge_capabilities.instanceExtensions, 'instanceExtensions')
                gen += self.gen_dataArrayInfo(profile_value.merge_capabilities.deviceExtensions, 'deviceExtensions')
                gen += self.gen_dataArrayInfo(profile_value.merge_capabilities.features, 'featureStructTypes')
                gen += '            featureDesc,\n'
                gen += self.gen_dataArrayInfo(profile_value.merge_capabilities.properties, 'propertyStructTypes')
                gen += '            propertyDesc,\n'
                gen += self.gen_dataArrayInfo(profile_value.merge_capabilities.queueFamiliesProperties, 'queueFamilyStructTypes')
                gen += self.gen_dataArrayInfo(profile_value.merge_capabilities.queueFamiliesProperties, 'queueFamilyDesc')
                gen += self.gen_dataArrayInfo(profile_value.merge_capabilities.formats, 'formatStructTypes')
                gen += self.gen_dataArrayInfo(profile_value.merge_capabilities.formats, 'formatDesc')
                gen += '        chainerDesc,\n'
                gen += self.gen_dataArrayInfo(profile_value.merge_capabilities.videoProfiles, 'videoProfileDesc')
                gen += '        },\n' # <- new closing curly
                gen += '    };\n\n'

            gen += '    namespace blocks {'
            for capability_keys in profile_value.referencedCapabilities:
                blockName = self.get_blockName(capability_keys)
                gen += ('\n        namespace {0} {{\n').format(blockName)
                if type(capability_keys).__name__ == 'list':
                    gen += '            static const VpVariantDesc variants[] = {\n'
                    for capability_key in capability_keys:
                        gen += self.gen_variants(capability_key, profile_value.split_capabilities[capability_key])
                    gen += '            };\n'
                    gen += '            static const uint32_t variantCount = static_cast<uint32_t>(std::size(variants));\n'
                else:
                    gen += '            static const VpVariantDesc variants[] = {\n'
                    gen += self.gen_variants(capability_keys, profile_value.split_capabilities[capability_keys])
                    gen += '            };\n'
                    gen += '            static const uint32_t variantCount = static_cast<uint32_t>(std::size(variants));\n'
                gen += ('        }} // namespace {0}\n').format(blockName)
            gen += '    } // namespace blocks\n\n'

            gen += '    static const VpCapabilitiesDesc capabilities[] = {\n'
            for capability_keys in profile_value.referencedCapabilities:
                gen += ('        {{ blocks::{0}::variantCount, blocks::{0}::variants }},\n').format(self.get_blockName(capability_keys))
            gen += '    };\n'
            gen += '    static const uint32_t capabilityCount = static_cast<uint32_t>(std::size(capabilities));\n'

            if profile_value.fallbacks:
                gen += ('\n'
                    '    static const VpProfileProperties fallbacks[] = {\n')
                for fallback in profile_value.fallbacks:
                    gen += '        {{{0}_NAME, {0}_SPEC_VERSION}},\n'.format(fallback.upper())
                gen += ('    };\n'
                    '    static const uint32_t fallbackCount = static_cast<uint32_t>(std::size(fallbacks));\n')

            if profile_value.profileRequirements:
                gen += ('\n'
                    '    static const VpProfileProperties profiles[] = {\n')
                for profile in profile_value.profileRequirements:
                    gen += '        {{{0}_NAME, {0}_SPEC_VERSION}},\n'.format(profile.upper())
                gen += ('    };\n'
                    '    static const uint32_t profileCount = static_cast<uint32_t>(std::size(profiles));\n')

            gen += ('}} // namespace {0}\n').format(profile_ukey)
            gen += ('#endif //{0}\n\n').format(profile_key)

        gen += 'static const VpProfileDesc profiles[] = {\n'
        for profile_key, profile_value in sorted(self.profiles_files.profiles.items()):
            profile_ukey = profile_key.upper()
            gen += ('#ifdef {0}\n'
                    '    VpProfileDesc{{\n'
                    '        VpProfileProperties{{ {1}_NAME, {1}_SPEC_VERSION }},\n'
                    '        {1}_MIN_API_VERSION,\n').format(profile_key, profile_ukey)
            if profile_value.multiple_variants:
                gen += '        nullptr,\n'
            else:
                gen += ('        {0}::mergedCapabilities,\n').format(profile_ukey)
            if profile_value.profileRequirements:
                gen += ('        {0}::profileCount, {0}::profiles,\n').format(profile_ukey)
            else:
                gen += ('        0, nullptr,\n')
            gen += ('        {0}::capabilityCount, {0}::capabilities,\n').format(profile_ukey)
            if profile_value.fallbacks:
                gen += ('        {1}::fallbackCount, {1}::fallbacks,\n').format(profile_key, profile_ukey)
            else:
                gen += ('        0, nullptr,\n')
            gen += ('    }},\n'
                    '#endif // {0}\n').format(profile_ukey)

        gen += ('};\n'
                'static const uint32_t profileCount = static_cast<uint32_t>(std::size(profiles));\n')
        return gen

    def gen_StructureSizeImpl(self):
        gen = '\n'
        for struct_key, struct_data in self.registry.structs.items():
            if 'VkPhysicalDeviceFeatures2' not in struct_data.extends or 'VkDeviceCreateInfo' not in struct_data.extends:
                continue

            struct_non_alias = self.registry.getNonAliasTypeName(struct_key, self.registry.structs)
            if struct_non_alias != struct_key:
                continue

            platform_protection = False
            for extension in struct_data.definedByExtensions:
                platform = self.registry.extensions[extension].platform
                if platform:
                    gen += '#ifdef {0}\n'.format(self.registry.platforms[platform].protect)
                    platform_protection = True

            gen += '        this->structureSize.insert({{ {0}, size<{1}>() }});\n'.format(struct_data.sType, struct_key)

            if platform_protection:
                gen += '#endif\n'

        gen += '        this->structureSize.insert({ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR, size<VkPhysicalDeviceFeatures2KHR>() });'
        return gen

    def gen_StructureFeatureChain(self):
        gen = '\n        //Initializing the full list of available structure features\n'
        gen += '        void* pNext = nullptr;\n'

        for struct_key, struct_data in self.registry.structs.items():
            if 'VkPhysicalDeviceFeatures2' not in struct_data.extends or 'VkDeviceCreateInfo' not in struct_data.extends:
                continue

            struct_non_alias = self.registry.getNonAliasTypeName(struct_key, self.registry.structs)
            if struct_non_alias != struct_key:
                continue

            currentVarName = struct_key[2].lower() + struct_key[3:]

            platform_protection = False
            for extension in struct_data.definedByExtensions:
                platform = self.registry.extensions[extension].platform
                if platform:
                    gen += '#ifdef {0}\n'.format(self.registry.platforms[platform].protect)
                    platform_protection = True

            gen += '        {0}.pNext = pNext;\n'.format(currentVarName)
            gen += '        pNext = &{0};\n'.format(currentVarName)
 
            if platform_protection:
                gen += '#endif\n'

        gen += "        physicalDeviceFeatures2KHR.pNext = pNext;\n"

        return gen

    def gen_StructureFeatureImpl(self):
        gen = '\n'

        for struct_key, struct_data in self.registry.structs.items():
            if 'VkPhysicalDeviceFeatures2' not in struct_data.extends or 'VkDeviceCreateInfo' not in struct_data.extends:
                continue

            struct_non_alias = self.registry.getNonAliasTypeName(struct_key, self.registry.structs)
            if struct_non_alias != struct_key:
                continue

            currentVarName = struct_key[2].lower() + struct_key[3:]

            platform_protection = False
            for extension in struct_data.definedByExtensions:
                platform = self.registry.extensions[extension].platform
                if platform:
                    gen += '#ifdef {0}\n'.format(self.registry.platforms[platform].protect)
                    platform_protection = True

            gen += '    {0} {1}{{ {2}, nullptr }};\n'.format(struct_key, currentVarName, struct_data.sType)
 
            if platform_protection:
                gen += '#endif\n'

        gen += "    VkPhysicalDeviceFeatures2KHR physicalDeviceFeatures2KHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR, nullptr };\n"

        return gen

    def gen_profileFeatureChain(self):
        genStructureSize = self.gen_StructureSizeImpl()
        genStructureFeatures = self.gen_StructureFeatureImpl()
        gen_StructureFeatureChain = self.gen_StructureFeatureChain()

        gen = '\n'
        gen += '''
struct FeaturesChain {
    std::map<VkStructureType, std::size_t> structureSize;

    template<typename T>
    constexpr std::size_t size() const {
        return (sizeof(T) - sizeof(VkBaseOutStructure)) / sizeof(VkBool32);
    }\n'''

        gen += '''\n\t// Chain with all Vulkan Features structures{0}'''.format(genStructureFeatures)

        gen += '''
    FeaturesChain() {{
        // Initializing all feature structures, number of Features (VkBool32) per structure.{0}\n{1}
    }}\n\n'''.format(genStructureSize, gen_StructureFeatureChain)

        gen += PRIVATE_IMPL_FEATURES_CHAIN_IMPL

        gen += '}; // struct FeaturesChain\n'

        Template(gen).substitute(genStructureSize=genStructureSize, genStructureFeatures=genStructureFeatures)
        return gen

    def gen_videoProfileEnumerator(self):
        # Generates an enumerator function that goes through all supportable video profiles
        # Used to handle "wildcard" video profiles where only partial video profile info is specified
        # in the JSON and the defined capabilities and video format properties apply to all video profiles
        # that match the "wildcard".
        gen = '\n'
        gen += '''
#ifdef VK_KHR_video_queue
VPAPI_ATTR void vpForEachMatchingVideoProfiles(
    VkVideoProfileInfoKHR*                      pVideoProfileInfo,
    void*                                       pUser,
    PFN_vpStructChainerCb                       pfnCb) {
    const VkVideoChromaSubsamplingFlagsKHR chroma_subsampling_list[] = {
        VK_VIDEO_CHROMA_SUBSAMPLING_420_BIT_KHR,
        VK_VIDEO_CHROMA_SUBSAMPLING_422_BIT_KHR,
        VK_VIDEO_CHROMA_SUBSAMPLING_444_BIT_KHR,
        VK_VIDEO_CHROMA_SUBSAMPLING_MONOCHROME_BIT_KHR
    };
    const VkVideoComponentBitDepthFlagsKHR bit_depth_list[] = {
        VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR,
        VK_VIDEO_COMPONENT_BIT_DEPTH_10_BIT_KHR,
        VK_VIDEO_COMPONENT_BIT_DEPTH_12_BIT_KHR
    };
    for (size_t chromaSubsampling_idx = 0; chromaSubsampling_idx < std::size(chroma_subsampling_list); ++chromaSubsampling_idx) {
        pVideoProfileInfo->chromaSubsampling = chroma_subsampling_list[chromaSubsampling_idx];
        for (size_t lumaBitDepth_idx = 0; lumaBitDepth_idx < std::size(bit_depth_list); ++lumaBitDepth_idx) {
            pVideoProfileInfo->lumaBitDepth = bit_depth_list[lumaBitDepth_idx];
            for (size_t chromaBitDepth_idx = 0; chromaBitDepth_idx < std::size(bit_depth_list); ++chromaBitDepth_idx) {
                pVideoProfileInfo->chromaBitDepth = bit_depth_list[chromaBitDepth_idx];
'''

        for videoCodecOp, videoCodec in self.registry.videoCodecsByValue.items():
            gen += '{0}{{\n'.format(' ' * 16)
            indent = ' ' * 20
            gen += '{0}pVideoProfileInfo->pNext = nullptr;\n'.format(indent)
            gen += '{0}pVideoProfileInfo->videoCodecOperation = {1};\n'.format(indent, videoCodecOp)
            for profileStruct in videoCodec.profileStructs:
                profileStructDef = self.registry.structs[profileStruct]
                gen += '{0}{1} var_{2} = {{ {3} }};\n'.format(indent, profileStruct, profileStruct[2:], profileStructDef.sType)
                gen += '{0}var_{1}.pNext = pVideoProfileInfo->pNext;\n'.format(indent, profileStruct[2:])
                gen += '{0}pVideoProfileInfo->pNext = &var_{1};\n'.format(indent, profileStruct[2:])

            # Permute profiles for each profile struct member value
            profiles = OrderedDict({'': []})
            lastValue = dict()
            for profileStruct in videoCodec.profileStructs.values():
                lastValue[profileStruct.struct] = dict()
                for profileStructMember in profileStruct.members.values():
                    lastValue[profileStruct.struct][profileStructMember.name] = None
                    newProfiles = {}
                    for profileStructMemberValue, profileStructMemberName in profileStructMember.values.items():
                        for profileName, profile in profiles.items():
                            # Only add video profile name suffix to the full descriptive name if not empty to avoid excess whitespace
                            newProfileName = profileName if profileStructMemberName == '' else f'{profileName} {profileStructMemberName}'
                            newProfiles[newProfileName] = profile + [{
                                "struct": profileStruct.struct,
                                "member": profileStructMember.name,
                                "value": profileStructMemberValue
                            }]
                    profiles = newProfiles

            for profile in profiles.values():
                for profileStruct in videoCodec.profileStructs:
                    for elem in profile:
                        if elem['struct'] == profileStruct:
                            if lastValue[elem['struct']][elem['member']] != elem['value']:
                                gen += '{0}var_{1}.{2} = {3};\n'.format(indent, elem['struct'][2:], elem['member'], elem['value'])
                                lastValue[elem['struct']][elem['member']] = elem['value']
                gen += '{0}pfnCb(reinterpret_cast<VkBaseOutStructure*>(pVideoProfileInfo), pUser);\n'.format(indent)

            gen += '{0}}}\n'.format(' ' * 16)

        gen += '''            }
        }
    }
}
#endif  // VK_KHR_video_queue
'''
        return gen

    def gen_publicImpl(self):
        gen = PUBLIC_IMPL_BODY
        return self.patch_code(gen)

DOC_MD_HEADER = '''
<!-- markdownlint-disable MD041 -->
<p align="left"><img src="https://vulkan.lunarg.com/img/NewLunarGLogoBlack.png" alt="LunarG" width=263 height=113 /></p>
<p align="left">Copyright (c) 2021-2026 LunarG, Inc.</p>

<p align="center"><img src="./images/logo.png" width=400 /></p>

[![Creative Commons][3]][4]

[3]: https://i.creativecommons.org/l/by-nd/4.0/88x31.png "Creative Commons License"
[4]: https://creativecommons.org/licenses/by-nd/4.0/
'''

class VulkanProfilesDocGenerator():
    def __init__(self, registry, profiles_files):
        self.registry = registry
        sorted_profiles = sorted(profiles_files.profiles.values(), key = self.sort_KHR_EXT_first)

        self.profiles = []
        for profile in sorted_profiles:
            if (not profile.multiple_variants):
                self.profiles.append(profile)

        # Determine maximum core version required across all profiles
        self.maxRequiredCoreVersion = max(profile.apiVersionNumber for profile in self.profiles)

        # Collect extensions required by core versions up to the maximum core version required
        # across all profiles so that we can include related data in the relevant tables
        self.coreInstanceExtensions = []
        self.coreDeviceExtensions = []
        for extension in self.registry.extensions.values():
            version = self.registry.getExtensionPromotedToVersion(extension.name)
            if version != None and version.number <= self.maxRequiredCoreVersion:
                if extension.type == 'instance':
                    self.coreInstanceExtensions.append(extension.name)
                elif extension.type == 'device':
                    self.coreDeviceExtensions.append(extension.name)


    def sort_KHR_EXT_first(self, profileOrExtName):
        # Make sure KHR profiles and extensions come first and EXT extensions come next
        key = profileOrExtName.key if isinstance(profileOrExtName, VulkanProfile) else profileOrExtName
        if key[2:7] == '_KHR_':
            return 'A' + key
        elif key[2:7] == '_KHX_':
            return 'B' + key
        elif key[2:7] == '_EXT_':
            return 'C' + key
        else:
            return key


    def generate(self, outDoc):
        Log.i("Generating '{0}'...".format(outDoc))
        with open(outDoc, 'w') as f:
            f.write(self.gen_doc())


    def gen_doc(self):
        gen = DOC_MD_HEADER
        gen += '\n# Vulkan Profiles Definitions\n'
        gen += self.gen_profilesList()
        gen += self.gen_extensions()
        gen += self.gen_features()
        gen += self.gen_limits()
        gen += self.gen_queueFamilies()
        gen += self.gen_formats()
        gen += self.gen_videoProfiles()
        return gen


    def gen_manPageLink(self, entry, text):
        # The version is irrelevant currently in the man page base link as it gets redirected to
        # the latest version's corresponding page, so we simply use version 1.1 as convention
        return '[{0}](https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/{1}.html)'.format(text, entry)


    def gen_table(self, rowHandlers):
        gen = '| Profiles |'
        cellFmt = ' {0} |'
        for profile in self.profiles:
            gen += cellFmt.format(profile.key)
        gen += '\n{0}'.format(re.sub(r"[^|]", '-', gen))
        for row, rowHandler in rowHandlers.items():
            gen += '\n| {0} |'.format(row)
            for profile in self.profiles:
                gen += cellFmt.format(rowHandler(row, profile))
        return gen


    def gen_sectionedTable(self, rowHandlers):
        gen = '| Profiles |'
        cellFmt = ' {0} |'
        for profile in self.profiles:
            gen += cellFmt.format(profile.key)
        gen += '\n{0}'.format(re.sub(r"[^|]", '-', gen))
        for section, sectionRowHandlers in rowHandlers.items():
            gen += '\n| **{0}** |'.format(section)
            for row, rowHandler in sectionRowHandlers.items():
                gen += '\n| {0} |'.format(rowHandler(section, row))
                for profile in self.profiles:
                    gen += cellFmt.format(rowHandler(section, row, profile))
        return gen


    def gen_profilesList(self):
        return '\n## Vulkan Profiles List\n\n{0}\n'.format(self.gen_table(OrderedDict({
            'Label': lambda _, profile : profile.label,
            'Description': lambda _, profile : profile.description,
            'Version': lambda _, profile : profile.version,
            'Required API version': lambda _, profile : profile.apiVersion,
            'Required profiles': lambda _, profile : ', '.join(profile.profileRequirements) if profile.profileRequirements != None else '-',
            'Fallback profiles': lambda _, profile : ', '.join(profile.fallbacks) if profile.fallbacks != None else '-'
        })))

    def gen_extension(self, section, extension, profile = None):
        # If no profile was specified then this is the first column so return the extension name
        # with a link to the extension's manual page
        if profile is None:
            return self.gen_manPageLink(extension, extension)

        # If it's an extension explicitly required by the profile then this is a supported extension
        if extension in profile.merge_capabilities.extensions:
            return ':heavy_check_mark:'

        # Otherwise check if this extension has been promoted to a core API version that the profile requires
        version = self.registry.getExtensionPromotedToVersion(extension)
        # If core API version found and is required by the profile then this extension is supported as being core
        if version != None and version.number <= profile.apiVersionNumber:
            return str(version.number) + ' Core'

        # Otherwise it's unsupported
        return ':x:'


    def gen_extensions(self):
        # Collect instance extensions defined by the profiles
        instanceExtensions = self.coreInstanceExtensions + list(itertools.chain(*[
            profile.merge_capabilities.instanceExtensions.keys() for profile in self.profiles
        ]))
        instanceExtensions.sort(key = self.sort_KHR_EXT_first)

        # Collect device extensions defined by the profiles
        deviceExtensions = self.coreDeviceExtensions + list(itertools.chain(*[
            profile.merge_capabilities.deviceExtensions.keys() for profile in self.profiles
        ]))
        deviceExtensions.sort(key = self.sort_KHR_EXT_first)

        # Generate table legend
        legend = (
            '* :heavy_check_mark: indicates that the extension is defined in the profile\n'
            '* "X.X Core" indicates that the extension is not defined in the profile but '
            'the extension is promoted to the specified core API version that is smaller than '
            'or equal to the minimum required API version of the profile\n'
            '* :x: indicates that the extension is neither defined in the profile nor it is '
            'promoted to a core API version that is smaller than or equal to the minimum '
            'required API version of the profile\n'
        )

        # Generate table
        table = self.gen_sectionedTable(OrderedDict({
            'Instance extensions': OrderedDict({ row: self.gen_extension for row in instanceExtensions }),
            'Device extensions': OrderedDict({ row: self.gen_extension for row in deviceExtensions })
        }))
        return '\n## Vulkan Profiles Extensions\n\n{0}\n{1}\n'.format(legend, table)


    def has_nestedFeatureData(self, data):
        for key in data:
            if not isinstance(data[key], bool):
                return True
        return None


    def formatFeatureSupport(self, supported, struct, section):
        structDef = self.registry.structs[struct]
        # VkPhysicalDeviceVulkan11Features is defined in Vulkan 1.2, but actually it defines Vulkan 1.1 features
        if struct == 'VkPhysicalDeviceVulkan11Features':
            where = 'Vulkan 1.1'
            isExactMatch = (section == where)
        elif structDef.definedByVersion != None:
            where = 'Vulkan {0}'.format(str(structDef.definedByVersion))
            isExactMatch = (section == where)
        elif len(structDef.definedByExtensions) > 0:
            where = '/'.join(structDef.definedByExtensions)
            isExactMatch = (section in structDef.definedByExtensions)
        else:
            where = 'Vulkan 1.0'
            isExactMatch = (section == where)
        if supported:
            if isExactMatch:
                return '<span title="defined in {0} ({1})">:heavy_check_mark:</span>'.format(struct, where)
            else:
                return '<span title="equivalent defined in {0} ({1})">:warning:</span>'.format(struct, where)
        else:
            return ':x:'


    def getFeatureStructSynonyms(self, struct, member):
        structDef = self.registry.structs[struct]
        if structDef.definedByVersion != None:
            # For 1.1+ core features we always have two structures defining the feature, one is
            # the feature specific structure, the other is VkPhysicalDeviceVulkanXXFeatures
            if struct == 'VkPhysicalDeviceVulkan11Features':
                # VkPhysicalDeviceVulkan11Features is defined in Vulkan 1.2, but actually it
                # defines Vulkan 1.1 features
                version = self.registry.versions['VK_VERSION_1_1']
            else:
                # For other structures find the version defining the structure
                for version in self.registry.versions.values():
                    if version.number == structDef.definedByVersion:
                        break
            # Return all the structures defining this feature member
            return version.features[member].structs
        else:
            # In all other cases we're talking about a non-promoted extension, as the structure
            # we receive here is always a non-alias structure, so we can simply return the
            # aliases of the structure
            return structDef.aliases


    def getFeatureStructForManPageLink(self, struct, member):
        # We don't want to link to the man page VkPhysicalDeviceVulkanXXFeatures structures,
        # instead we prefer to use the more specific non-alias structure if possible
        for alias in self.getFeatureStructSynonyms(struct, member):
            if re.match(r"^VkPhysicalDeviceVulkan[0-9]+Features$", alias) is None:
                structDef = self.registry.structs[alias]
                if not structDef.isAlias:
                    struct = alias
        return struct


    def gen_feature(self, struct, section, member, profile = None):
        # If no profile was specified then this is the first column so return the member name
        # with a link to the encompassing structure's manual page
        if profile is None:
            return self.gen_manPageLink(self.getFeatureStructForManPageLink(struct, member),
                                        member)

        # If this feature struct member is defined in the profile as is, consider it supported
        if struct in profile.doc_capabilities.features:
            featureStruct = profile.doc_capabilities.features[struct]
            if member in featureStruct:
                return self.formatFeatureSupport(featureStruct[member], struct, section)

        # If the struct is VkPhysicalDeviceFeatures then check if the feature is defined in
        # VkPhysicalDeviceFeatures2 or VkPhysicalDeviceFeatures2KHR for the profile and then
        # consider it supported
        if struct == 'VkPhysicalDeviceFeatures':
            for wrapperStruct in [ 'VkPhysicalDeviceFeatures2', 'VkPhysicalDeviceFeatures2KHR' ]:
                if wrapperStruct in profile.doc_capabilities.features:
                    featureStruct = profile.doc_capabilities.features[wrapperStruct]['features']
                    if member in featureStruct:
                        return self.formatFeatureSupport(featureStruct[member], struct, section)

        # If the struct has aliases and the feature struct member is defined in the profile in
        # one of those, consider it supported
        for alias in self.getFeatureStructSynonyms(struct, member):
            if alias in profile.doc_capabilities.features:
                featureStruct = profile.doc_capabilities.features[alias]
                if member in featureStruct:
                    return self.formatFeatureSupport(featureStruct[member], alias, section)

        return self.formatFeatureSupport(False, struct, section)


    def gen_featuresSection(self, features, definedFeatures, sectionHeader, tableData):
        # Go through defined feature structures
        for definedFeatureStructName, definedFeatureList in definedFeatures.items():
            # Go through defined features within those structures
            for definedFeature in definedFeatureList:
                # Check if there's a feature with a matching name in the features to consider
                if definedFeature in features.keys():
                    feature = features[definedFeature]
                    # Check that the feature structure actually matches one of the structures
                    # this feature is defined in (this is needed because the registry xml doesn't
                    # prevent multiple structures defining features with identical names so we
                    # have to check whether we actually talk about a synonym or a completely
                    # different feature with the same name)
                    if definedFeatureStructName in feature.structs:
                        if not sectionHeader in tableData:
                            tableData[sectionHeader] = OrderedDict()
                        # Feature is defined, add it to the table
                        tableData[sectionHeader][definedFeature] = functools.partial(self.gen_feature, definedFeatureStructName)


    def gen_features(self):
        # Merge all feature references across the profiles to collect the relevant features to look at
        definedFeatures = dict()
        for profile in self.profiles:
            for featureStructName, features in profile.doc_capabilities.features.items():
                # VkPhysicalDeviceFeatures2 is an exception, as it contains a nested structure
                # No other structure is allowed to have this
                if featureStructName in [ 'VkPhysicalDeviceFeatures2', 'VkPhysicalDeviceFeatures2KHR' ]:
                    featureStructName = 'VkPhysicalDeviceFeatures'
                    features = features['features']
                elif self.has_nestedFeatureData(features):
                    Log.f("Unexpected nested feature data in profile '{0}' structure '{1}'".format(profile.name, featureStructName))
                # If this is an alias structure then find the non-alias one and use that
                featureStructName = self.registry.getNonAliasTypeName(featureStructName, self.registry.structs)
                # Copy defined feature structure data
                if not featureStructName in definedFeatures:
                    definedFeatures[featureStructName] = []
                definedFeatures[featureStructName].extend(features.keys())

        tableData = OrderedDict()

        # First, go through core features
        for version in sorted(self.registry.versions.values(), key = lambda version: version.number):
            self.gen_featuresSection(version.features, definedFeatures, 'Vulkan ' + str(version.number), tableData)

        # Then, go through extensions
        for extension in sorted(self.registry.extensions.values(), key = lambda extension: self.sort_KHR_EXT_first(extension.name)):
            self.gen_featuresSection(extension.features, definedFeatures, extension.name, tableData)

        # Sort individual features within the sections by name
        for sectionName in tableData.keys():
            tableData[sectionName] = OrderedDict(sorted(tableData[sectionName].items()))

        # TODO: Currently we don't include features that are required by the minimum required API
        # version of a profile, or features required by extensions required by the profile, as
        # that would necessitate the inclusion of the information currently only available
        # textually in the "Feature Requirements" section of the Vulkan Specification
        disclaimer = (
            '> **NOTE**: The table below only contains features explicitly defined by the '
            'corresponding profile. Further features may be supported by the profiles in '
            'accordance to the requirements defined in the "Feature Requirements" section '
            'of the appropriate version of the Vulkan API Specification.'
        )

        # Generate table legend
        legend = (
            '* :heavy_check_mark: indicates that the feature is defined in the profile (hover '
            'over the symbol to view the structure and corresponding extension or core API '
            'version where the feature is defined in the profile)\n'
            '* :warning: indicates that the feature is not defined in the profile but an '
            'equivalent feature is (hover over the symbol to view the structure and '
            'corresponding extension or core API version where the feature is defined in the '
            'profile)\n'
            '* :x: indicates that neither the feature nor an equivalent feature is defined in '
            'the profile\n'
        )

        # Generate table
        table = self.gen_sectionedTable(tableData)
        return '\n## Vulkan Profile Features\n\n{0}\n\n{1}\n{2}\n'.format(disclaimer, legend, table)


    def formatValue(self, value):
        if type(value) == bool:
            # Boolean
            return 'VK_TRUE' if value else 'VK_FALSE'
        elif type(value) == dict:
            # Structure, match the Vulkan Specification's formatting
            return '({0})'.format(','.join(str(el) for el in value.values()))
        elif type(value) == list:
            if len(value) == 0:
                # Empty array, not much to return
                return '-'
            elif type(value[0]) == str:
                # Bitmask, match the Vulkan Specification's formatting
                return '({0})'.format(' \\| '.join(value))
            else:
                # Array, match the Vulkan Specification's formatting
                return '({0})'.format(','.join(str(el) for el in value))
        else:
            return str(value)


    def formatProperty(self, value, struct, section = None):
        structDef = self.registry.structs[struct]
        # VkPhysicalDeviceVulkan11Properties is defined in Vulkan 1.2, but actually it defines Vulkan 1.1 features
        if struct == 'VkPhysicalDeviceVulkan11Properties':
            where = 'Vulkan 1.1'
            isExactMatch = (section == where)
        elif structDef.definedByVersion != None:
            where = 'Vulkan {0}'.format(str(structDef.definedByVersion))
            isExactMatch = (section == where)
        elif len(structDef.definedByExtensions) > 0:
            where = '/'.join(structDef.definedByExtensions)
            isExactMatch = (section in structDef.definedByExtensions)
        else:
            where = 'Vulkan 1.0'
            isExactMatch = (section == where)
        if isExactMatch or section == None:
            return '<span title="defined in {0} ({1})">{2}</span>'.format(struct, where, self.formatValue(value))
        else:
            return '<span title="equivalent defined in {0} ({1})">_{2}_</span>'.format(struct, where, self.formatValue(value))


    def formatLimitName(self, struct, member):
        structDef = self.registry.structs[struct]
        memberDef = structDef.members[member]
        limittype = memberDef.limittype

        if limittype in [ None, 'noauto', 'bitmask' ]:
            return member
        elif limittype == 'exact':
            return member + ' (exact)'
        elif limittype == 'max':
            return member + ' (max)'
        elif limittype == 'max,pot' or limittype == 'pot,max':
            return member + ' (max,pot)'
        elif limittype in [ 'min' ]:
            return member + ' (min)'
        elif limittype == 'min,pot' or limittype == 'pot,min':
            return member + ' (min,pot)'
        elif limittype == 'min,mul' or limittype == 'mul,min':
            return member + ' (min,mul)'
        elif limittype == 'bits':
            return member + ' (bits)'
        elif limittype == 'range':
            return member + ' (min-max)'
        else:
            Log.f("Unexpected limittype '{0}'".format(limittype))


    def getLimitStructSynonyms(self, struct, member):
        structDef = self.registry.structs[struct]
        if structDef.definedByVersion != None:
            # For 1.1+ core limits we always have two structures defining the limit, one is
            # the limit specific structure, the other is VkPhysicalDeviceVulkanXXProperties
            if struct == 'VkPhysicalDeviceVulkan11Properties':
                # VkPhysicalDeviceVulkan11Properties is defined in Vulkan 1.2, but actually it
                # defines Vulkan 1.1 limits
                version = self.registry.versions['VK_VERSION_1_1']
            else:
                # For other structures find the version defining the structure
                for version in self.registry.versions.values():
                    if version.number == structDef.definedByVersion and member in version.limits:
                        break
            # Return all the structures defining this limit member
            return version.limits[member].structs
        else:
            # In all other cases we're talking about a non-promoted extension, as the structure
            # we receive here is always a non-alias structure, so we can simply return the
            # aliases of the structure
            return structDef.aliases


    def getLimitStructForManPageLink(self, struct, member):
        # If the structure at hand is VkPhysicalDeviceProperties then we should rather link
        # to the underlying nested structure that actually defines the limit
        if struct == 'VkPhysicalDeviceProperties':
            structs = self.registry.versions['VK_VERSION_1_0'].limits[member].structs
            for nestedStruct in [ 'VkPhysicalDeviceLimits', 'VkPhysicalDeviceSparseProperties' ]:
                if nestedStruct in structs:
                    return nestedStruct

        # We don't want to link to the man page VkPhysicalDeviceVulkanXXProperties structures,
        # instead we prefer to use the more specific non-alias structure if possible
        for alias in self.getLimitStructSynonyms(struct, member):
            if re.match(r"^VkPhysicalDeviceVulkan[0-9]+Properties$", alias) is None:
                structDef = self.registry.structs[alias]
                if not structDef.isAlias:
                    struct = alias
        return struct


    def gen_limit(self, struct, section, member, profile = None):
        # If no profile was specified then this is the first column so return the member name
        # decorated with the corresponding limittype specific info and a link to the
        # encompassing structure's manual page
        if profile is None:
            return self.gen_manPageLink(self.getLimitStructForManPageLink(struct, member),
                                        self.formatLimitName(struct, member))

        # If this limit/property struct member is defined in the profile as is, include it
        if struct in profile.doc_capabilities.properties:
            limitStruct = profile.doc_capabilities.properties[struct]
            if member in limitStruct:
                return self.formatProperty(limitStruct[member], struct, section)

        # If the struct is VkPhysicalDeviceLimits or VkPhysicalDeviceSparseProperties then check
        # if the limit/property is defined somewhere nested in VkPhysicalDeviceProperties,
        # VkPhysicalDeviceProperties2, or VkPhysicalDeviceProperties2KHR for the profile then
        # include it
        if struct == 'VkPhysicalDeviceLimits' or struct == 'VkPhysicalDeviceSparseProperties':
            if struct == 'VkPhysicalDeviceLimits':
                memberStruct = 'limits'
            else:
                memberStruct = 'sparseProperties'
            propertyStruct = None
            if 'VkPhysicalDeviceProperties' in profile.doc_capabilities.properties:
                propertyStructName = 'VkPhysicalDeviceProperties'
                propertyStruct = profile.doc_capabilities.properties[propertyStructName]
            for wrapperStruct in [ 'VkPhysicalDeviceProperties2', 'VkPhysicalDeviceProperties2KHR' ]:
                if wrapperStruct in profile.doc_capabilities.properties:
                    propertyStructName = wrapperStruct
                    propertyStruct = profile.doc_capabilities.properties[wrapperStruct]['properties']
            if propertyStruct != None: # and memberStruct != 'sparseProperties':
                if memberStruct in propertyStruct:
                    limitStruct = propertyStruct[memberStruct]
                    if member in limitStruct:
                        return self.formatProperty(limitStruct[member], propertyStructName, section)

        # If the struct has aliases and the limit/property struct member is defined in the profile
        # in one of those then include it
        for alias in self.getLimitStructSynonyms(struct, member):
            if alias in profile.doc_capabilities.properties:
                limitStruct = profile.doc_capabilities.properties[alias]
                if member in limitStruct and limitStruct[member]:
                    return self.formatProperty(limitStruct[member], alias, section)

        return '-'


    def gen_limitsSection(self, limits, definedLimits, sectionHeader, tableData):
        # Go through defined limit/property structures
        for definedLimitStructName, definedLimitList in definedLimits.items():
            # Go through defined limits within those structures
            for definedLimit in definedLimitList:
                # Check if there's a limit with a matching name in the limits to consider
                if definedLimit in limits.keys():
                    limit = limits[definedLimit]
                    # Check that the limit/property structure actually matches one of the
                    # structures this limit is defined in (this is needed because the registry xml
                    # doesn't prevent multiple structures defining limits/properties with
                    # identical names so we have to check whether we actually talk about a synonym
                    # or a completely different limit/property with the same name)
                    if definedLimitStructName in limit.structs:
                        if not sectionHeader in tableData:
                            tableData[sectionHeader] = OrderedDict()
                        # Limit/property is defined, add it to the table
                        tableData[sectionHeader][definedLimit] = functools.partial(self.gen_limit, definedLimitStructName)


    def gen_limits(self):
        # Merge all limit/property references across the profiles to collect the relevant limits to look at
        definedLimits = dict()
        for profile in self.profiles:
            for propertyStructName, properties in profile.doc_capabilities.properties.items():
                # VkPhysicalDeviceProperties and VkPhysicalDeviceProperties2 are exceptions,
                # need custom handling due to only using their nested structures
                if propertyStructName in [ 'VkPhysicalDeviceProperties2', 'VkPhysicalDeviceProperties2KHR' ]:
                    propertyStructName = 'VkPhysicalDeviceProperties'
                    properties = properties['properties']
                if propertyStructName == 'VkPhysicalDeviceProperties':
                    for member, struct in { 'limits': 'VkPhysicalDeviceLimits', 'sparseProperties': 'VkPhysicalDeviceSparseProperties' }.items():
                        if member in properties:
                            if not struct in definedLimits:
                                definedLimits[struct] = []
                            definedLimits[struct].extend(properties[member].keys())
                    continue

                # If this is an alias structure then find the non-alias one and use that
                propertyStructName = self.registry.getNonAliasTypeName(propertyStructName, self.registry.structs)
                # Copy defined limit/property structure data
                if not propertyStructName in definedLimits:
                    definedLimits[propertyStructName] = []
                definedLimits[propertyStructName].extend(properties.keys())

        tableData = OrderedDict()

        # First, go through core limits/properties
        for version in sorted(self.registry.versions.values(), key = lambda version: version.number):
            self.gen_limitsSection(version.limits, definedLimits, 'Vulkan ' + str(version.number), tableData)

        # Then, go through extensions
        for extension in sorted(self.registry.extensions.values(), key = lambda extension: self.sort_KHR_EXT_first(extension.name)):
            self.gen_limitsSection(extension.limits, definedLimits, extension.name, tableData)

        # Sort individual limits within the sections by name
        for sectionName in tableData.keys():
            tableData[sectionName] = OrderedDict(sorted(tableData[sectionName].items()))

        # TODO: Currently we don't include limits/properties that are required by the minimum
        # required API version of a profile, or limits/properties required by extensions required
        # by the profile, as that would necessitate the inclusion of information currently only
        # available textually in the "Limit Requirements" section of the Vulkan Specification
        disclaimer = (
            '> **NOTE**: The table below only contains properties/limits explicitly defined '
            'by the corresponding profile. Further properties/limits may be supported by the '
            'profiles in accordance to the requirements defined in the "Limit Requirements" '
            'section of the appropriate version of the Vulkan API Specification.'
        )

        # Generate table legend
        legend = (
            '* "valueWithRegularFont" indicates that the limit/property is defined in the profile '
            '(hover over the value to view the structure and corresponding extension or core API '
            'version where the limit/property is defined in the profile)\n'
            '* "_valueWithItalicFont_" indicates that the limit/property is not defined in the profile '
            'but an equivalent limit/property is (hover over the symbol to view the structure '
            'and corresponding extension or core API version where the limit/property is defined '
            'in the profile)\n'
            '* "-" indicates that neither the limit/property nor an equivalent limit/property is '
            'defined in the profile\n'
        )

        # Generate table
        table = self.gen_sectionedTable(tableData)
        return '\n## Vulkan Profile Limits (Properties)\n\n{0}\n\n{1}\n{2}\n'.format(disclaimer, legend, table)


    def gen_queueFamily(self, index, struct, section, member, profile = None):
        # If no profile was specified then this is the first column so return the member name
        # decorated with the corresponding limittype specific info and a link to the
        # encompassing structure's manual page
        if profile is None:
            return self.gen_manPageLink(struct, self.formatLimitName(struct, member))

        # If this profile doesn't even define this queue family index then early out
        if len(profile.doc_capabilities.queueFamiliesProperties) <= index:
            return ''

        # If this queue family property struct member is defined in the profile as is, include it
        if struct in profile.doc_capabilities.queueFamiliesProperties[index]:
            propertyStruct = profile.doc_capabilities.queueFamiliesProperties[index][struct]
            if member in propertyStruct:
                return self.formatProperty(propertyStruct[member], struct)

        # If the struct is VkPhysicalDeviceQueueFamilyProperties then check if the feature is
        # defined in VkPhysicalDeviceQueueFamilyProperties2 or VkPhysicalDeviceQueueFamilyProperties2KHR
        # for the profile and then include it
        if struct == 'VkPhysicalDeviceQueueFamilyProperties':
            for wrapperStruct in [ 'VkPhysicalDeviceQueueFamilyProperties2', 'VkPhysicalDeviceQueueFamilyProperties2KHR' ]:
                if wrapperStruct in profile.doc_capabilities.queueFamiliesProperties[index]:
                    propertyStruct = profile.doc_capabilities.queueFamiliesProperties[index][wrapperStruct]['queueFamilyProperties']
                    if member in propertyStruct and propertyStruct[member]:
                        return self.formatProperty(propertyStruct[member], wrapperStruct)

        # If the struct has aliases and the property struct member is defined in the profile
        # in one of those then include it
        structDef = self.registry.structs[struct]
        for alias in structDef.aliases:
            if alias in profile.doc_capabilities.queueFamiliesProperties[index]:
                propertyStruct = profile.doc_capabilities.queueFamiliesProperties[index][alias]
                if member in propertyStruct and propertyStruct[member]:
                    return self.formatProperty(propertyStruct[member], alias)

        return '-'


    def gen_queueFamilies(self):
        # Merge all queue family property references across the profiles to collect the relevant
        # properties to look at for each queue family definition index
        definedQueueFamilies = []
        for profile in self.profiles:
            for index, queueFamily in enumerate(profile.doc_capabilities.queueFamiliesProperties):
                definedQueueFamilyProperties = OrderedDict()
                for structName, properties in queueFamily.items():
                    # VkPhysicalDeviceQueueFamilies2 is an exception, as it contains a nested structure
                    # No other structure is allowed to have this
                    if structName in [ 'VkPhysicalDeviceQueueFamilyProperties2', 'VkPhysicalDeviceQueueFamilyProperties2KHR']:
                        structName = 'VkPhysicalDeviceQueueFamilyProperties'
                        properties = properties['queueFamilyProperties']
                    # If this is an alias structure then find the non-alias one and use that
                    structName = self.registry.getNonAliasTypeName(structName, self.registry.structs)
                    # Copy defined limit/property structure data
                    if not structName in definedQueueFamilyProperties:
                        definedQueueFamilyProperties[structName] = []
                    definedQueueFamilyProperties[structName].extend(sorted(properties.keys()))
                # Add queue family to the list
                if len(definedQueueFamilies) <= index:
                    definedQueueFamilies.append(dict())
                definedQueueFamilies[index].update(definedQueueFamilyProperties)

        # Construct table data
        tableData = OrderedDict()
        for index, queueFamilyProperties in enumerate(definedQueueFamilies):
            section = tableData['Queue family #' + str(index)] = OrderedDict()
            for structName, members in queueFamilyProperties.items():
                section.update({ row: functools.partial(self.gen_queueFamily, index, structName) for row in members })

        # Generate table legend
        legend = (
            '* "valueWithRegularFont" indicates that the queue family property is defined in the '
            'profile (hover over the value to view the structure and corresponding extension or '
            'core API version where the queue family property is defined in the profile)\n'
            '* "_valueWithItalicFont_" indicates that the queue family property is not defined in the '
            'profile but an equivalent queue family property is (hover over the symbol to view '
            'the structure and corresponding extension or core API version where the queue family '
            'property is defined in the profile)\n'
            '* "-" indicates that neither the queue family property nor an equivalent queue '
            'family property is defined in the profile\n'
            '* Empty cells next to the properties of a particular queue family definition section '
            'indicate that the profile does not have a corresponding queue family definition\n'
        )

        # Generate table
        table = self.gen_sectionedTable(tableData)
        return '\n## Vulkan Profile Queue Families\n\n{0}\n{1}\n'.format(legend, table)


    def getFormatStructForManPageLink(self, struct):
        # We prefer returning VkFormatProperties3 instead of VkFormatProperties as even though
        # they are technically not strictly aliases, the former is the one that should be used
        # going forward and the feature flags are anyway defined to be usable as synonyms for
        # the legacy 32-bit flags
        return 'VkFormatProperties3' if struct == 'VkFormatProperties' else struct


    def gen_format(self, format, struct, section, member, profile = None):
        # If no profile was specified then this is the first column so return the member name
        # decorated with the corresponding limittype specific info and a link to the
        # encompassing structure's manual page
        if profile is None:
            return self.gen_manPageLink(self.getFormatStructForManPageLink(struct),
                                        self.formatLimitName(struct, member))

        # If this profile doesn't even define this format then early out
        if not format in profile.doc_capabilities.formats:
            # Before doing so, though, we have to check whether any of the aliases of the format
            # are defined by the profile
            formatAliases = self.registry.enums['VkFormat'].aliasValues
            if not format in formatAliases or not formatAliases[format] in profile.doc_capabilities.formats:
                return ''

        # If this format property struct member is defined in the profile as is, include it
        if struct in profile.doc_capabilities.formats[format]:
            propertyStruct = profile.doc_capabilities.formats[format][struct]
            if member in propertyStruct:
                return self.formatProperty(propertyStruct[member], struct)

        # If the struct is VkFormatProperties then 'member' also contains the trimmed name of
        # the flag bit to check for, so we check for that, or any of its aliases
        if struct == 'VkFormatProperties':
            for alternative in [ 'VkFormatProperties', 'VkFormatProperties2', 'VkFormatProperties2KHR', 'VkFormatProperties3', 'VkFormatProperties3KHR' ]:
                if alternative in profile.doc_capabilities.formats[format]:
                    propertyStruct = profile.doc_capabilities.formats[format][alternative]
                    # VkFormatProperties2[KHR] wrap the real structure in a member
                    if 'formatProperties' in propertyStruct:
                        propertyStruct = propertyStruct['formatProperties']
                    if member in propertyStruct:
                        return self.formatProperty(propertyStruct[member], alternative)

        # If the struct has aliases and the property struct member is defined in the profile
        # in one of those then include it
        structDef = self.registry.structs[struct]
        for alias in structDef.aliases:
            if alias in profile.doc_capabilities.formats[format]:
                propertyStruct = profile.doc_capabilities.formats[format][alias]
                if member in propertyStruct and propertyStruct[member]:
                    return self.formatProperty(propertyStruct[member], alias)

        return '-'


    def gen_formats(self):
        # Merge all format property references across the profiles to collect the relevant
        # properties to look at for each format
        definedFormats = dict()
        for profile in self.profiles:
            for format, formatProperties in profile.doc_capabilities.formats.items():
                # This may be an alias of a format name, so get the real name
                formatAliases = self.registry.enums['VkFormat'].aliasValues
                format = formatAliases[format] if format in formatAliases else format

                definedFormatProperties = OrderedDict()
                for structName, properties in formatProperties.items():
                    # VkFormatProperties, VkFormatProperties2, and VkFormatProperties3 are special
                    if structName in [ 'VkFormatProperties2', 'VkFormatProperties2KHR' ]:
                        structName = 'VkFormatProperties'
                        properties = properties['formatProperties']
                    if structName in [ 'VkFormatProperties3', 'VkFormatProperties3KHR' ]:
                        structName = 'VkFormatProperties'
                    # If this is an alias structure then find the non-alias one and use that
                    structName = self.registry.getNonAliasTypeName(structName, self.registry.structs)
                    # Copy defined format property structure data
                    if not structName in definedFormatProperties:
                        definedFormatProperties[structName] = []
                    definedFormatProperties[structName].extend(sorted(properties.keys()))

                # Add format information
                if not format in definedFormats:
                    definedFormats[format] = OrderedDict()
                definedFormats[format].update(definedFormatProperties)


        # Construct table data
        tableData = OrderedDict()
        for format in sorted(definedFormats.keys()):
            section = tableData[format] = OrderedDict()
            for structName, members in definedFormats[format].items():
                section.update({ row: functools.partial(self.gen_format, format, structName) for row in members })

        # TODO: Currently we don't include format properties that are required by the minimum
        # required API version of a profile, or those required by extensions required by the
        # profile, as that would necessitate the inclusion of information currently only
        # available textually in the "Required Format Support" section of the Vulkan Specification
        disclaimer = (
            '> **NOTE**: The table below only contains formats and properties explicitly defined '
            'by the corresponding profile. Further formats and properties may be supported by the '
            'profiles in accordance to the requirements defined in the "Required Format Support" '
            'section of the appropriate version of the Vulkan API Specification.'
        )

        # Generate table legend
        legend = (
            '* "valueWithRegularFont" indicates that the format property is defined in the '
            'profile (hover over the value to view the structure and corresponding extension or '
            'core API version where the format property is defined in the profile)\n'
            '* "_valueWithItalicFont_" indicates that the format property is not defined in the '
            'profile but an equivalent format property is (hover over the symbol to view the '
            'structure and corresponding extension or core API version where the format property '
            'is defined in the profile)\n'
            '* "-" indicates that neither the format property nor an equivalent format property '
            'is defined in the profile\n'
            '* Empty cells next to the properties of a particular format definition section '
            'indicate that the profile does not have a corresponding format definition\n'
        )

        # Generate table
        table = self.gen_sectionedTable(tableData)
        return '\n## Vulkan Profile Formats\n\n{0}\n\n{1}\n{2}\n'.format(disclaimer, legend, table)


    def gen_videoProfile(self, videoProfilesPerProfileKey, videoProfileName, profile = None):
        if videoProfileName in videoProfilesPerProfileKey[profile.key]:
            # Video profile is supported by the given profile
            return ':heavy_check_mark:'
        else:
            # Video profile is NOT supported by the given profile
            return ':x:'


    def gen_videoProfiles(self):
        if self.registry.videoCodecs is None:
            return ""

        definedVideoProfiles = OrderedDict()
        videoProfilesPerProfileKey = OrderedDict()

        # Merge all defined video profile names
        for profile in self.profiles:
            videoProfilesPerProfileKey[profile.key] = OrderedDict()
            for videoProfileName, videoProfile in profile.doc_capabilities.videoProfilesByName.items():
                videoProfilesPerProfileKey[profile.key][videoProfileName] = videoProfile
                if not videoProfileName in definedVideoProfiles:
                    definedVideoProfiles[videoProfileName] = videoProfile

        # Construct table data
        tableData = OrderedDict()
        for videoProfileName in sorted(definedVideoProfiles.keys()):
            tableData[videoProfileName] = functools.partial(self.gen_videoProfile, videoProfilesPerProfileKey)

        # Generate table legend
        legend = (
            'The table below lists the video profiles (or class of video profiles) that any profile defines requirements for.\n'
            'Video profile names take the following form:\n'
            '> `"(General|[<codec>] <category>) [([<subsampling>] [<bits>])] [<codecSpecificProfileInfo>]"`\n'
            '* "General" refers to general requirements for all video profiles.\n'
            '* `<category>` is the category of the video profile requirements (e.g. "Decode" or "Encode").\n'
            '* `<codec>` identifies the specific codec (e.g. H.264, H.265, AV1) the requirements apply to. If not indicated, '
            'the requirements apply to all supported video profiles falling in the specified category.\n'
            '* `<subsampling>` indicates the chroma subsampling (e.g. 4:2:0, 4:2:2, 4:4:4, or monochrome). If not indicated, '
            'the requirements apply to all supported chroma subsampling modes.\n'
            '* `<bits>` indicates the luma and chroma bit depth (e.g. 8-bit, 10-bit, or e.g. 10:8-bit for mixed bit depths). '
            'If not indicated, the requirements apply to all supported bit depths. Partial, luma-only or chroma-only "wildcard" '
            'requirements are indicated with an asterisk in place of the corresponding bit depth (e.g. 10:\\*-bit or \\*:8-bit).\n'
            '* `<codecSpecificProfileInfo>` indicates additional codec-specific video profile information (e.g. "Main", "High"). '
            'If not indicated, the requirements apply to all video profiles of the codec.\n'
            '* :heavy_check_mark: indicates that the profile defines requirements for the video profile (or class of video profiles)\n'
            '* :x: indicates that that the profile does not define requirements for the video profile (or class of video profiles)\n'
        )

        # Generate per video profile sections
        videoProfileSections = ""
        for videoProfileName in sorted(definedVideoProfiles.keys()):
            videoProfile = definedVideoProfiles[videoProfileName]
            videoCodec = self.registry.getVideoCodecFromVideoProfile(videoProfile)

            videoProfileSections += "\n### {0}\n\n".format(videoProfileName)
            videoProfileSections += self.gen_videoProfileDefinition(videoProfile, videoCodec)
            videoProfileSections += self.gen_videoCapabilities(videoProfileName, videoCodec)
            videoProfileSections += self.gen_videoFormats(videoProfileName, videoCodec)

        # Generate table
        table = self.gen_table(tableData)
        return '\n## Vulkan Profile Video Profiles\n\n{0}\n{1}\n{2}'.format(legend, table, videoProfileSections)


    def gen_videoProfileDefinition(self, videoProfile, videoCodec):
        base = self.registry.getBaseVideoProfileInfoFromVideoProfile(videoProfile)
        baseStructName = self.registry.getNonAliasTypeName('VkVideoProfileInfoKHR', self.registry.structs)

        # Construct table data
        table = '| Profile member | Value |\n'
        table += '|----------------|-------|\n'
        table += '| **{0}** |\n'.format(baseStructName)
        for member in self.registry.structs[baseStructName].members:
            # Asterisk marks unspecified members (wildcard members)
            value = base[member] if base is not None and member in base else "*"
            table += '| {0} | {1} |\n'.format(self.gen_manPageLink(baseStructName, member), value)

        if 'profile' in videoProfile:
            videoProfileDesc = videoProfile['profile']
        else:
            videoProfileDesc = dict()

        for profileStruct in videoCodec.profileStructs.values():
            extStructName = self.registry.getNonAliasTypeName(profileStruct.struct, self.registry.structs)
            table += '| **{0}** |\n'.format(extStructName)

            videoProfileStruct = None
            if extStructName in videoProfileDesc:
                videoProfileStruct = videoProfileDesc[extStructName]
            else:
                # Check also for possible aliases
                for alias in self.registry.structs[extStructName].aliases:
                    if alias in videoProfileDesc:
                        videoProfileStruct = videoProfileDesc[alias]

            for profileStructMember in profileStruct.members.values():
                # Asterisk marks unspecified members (wildcard members)
                value = videoProfileStruct[profileStructMember.name] if videoProfileStruct is not None and profileStructMember.name in videoProfileStruct else "*"
                table += '| {0} | {1} |\n'.format(self.gen_manPageLink(extStructName, profileStructMember.name), value)

        return '\n#### Video Profile Definition\n\n{0}'.format(table)


    def formatVideoProfileProperty(self, value, struct):
        structDef = self.registry.structs[struct]
        if structDef.definedByVersion != None:
            where = 'Vulkan {0}'.format(str(structDef.definedByVersion))
        elif len(structDef.definedByExtensions) > 0:
            where = '/'.join(structDef.definedByExtensions)
        return '<span title="defined in {0} ({1})">{2}</span>'.format(struct, where, self.formatValue(value))


    def gen_videoCapability(self, videoProfileName, struct, member, profile = None):
        # If no profile was specified then this is the first column so return the member name
        # decorated with the corresponding limittype specific info and a link to the
        # encompassing capability structure's manual page
        if profile is None:
            return self.gen_manPageLink(struct, self.formatLimitName(struct, member))

        # Check if the capability is defined in the profile
        if videoProfileName in profile.doc_capabilities.videoProfilesByName:
            videoProfile = profile.doc_capabilities.videoProfilesByName[videoProfileName]
            capabilityStruct = None
            capabilityStructName = None
            if struct in videoProfile['capabilities']:
                capabilityStruct = videoProfile['capabilities'][struct]
                capabilityStructName = struct
            else:
                # Check also for possible aliases
                for alias in self.registry.structs[struct].aliases:
                    if alias in videoProfile['capabilities']:
                        capabilityStruct = videoProfile['capabilities'][alias]
                        capabilityStructName = alias
                        break

            if capabilityStruct is not None and member in capabilityStruct:
                return self.formatVideoProfileProperty(capabilityStruct[member], capabilityStructName)

        return '-'


    def gen_videoCapabilities(self, videoProfileName, videoCodec):
        # Merge all capability references across the profiles to collect the relevant capabilities to look at
        definedCapabilities = dict()
        for profile in self.profiles:
            for videoProfile in profile.doc_capabilities.videoProfiles:
                if videoProfileName != self.registry.getVideoProfileNameFromVideoProfile(videoProfile):
                    continue
                if not 'capabilities' in videoProfile:
                    continue
                for capabilityStructName, capabilities in videoProfile['capabilities'].items():
                    # If this is an alias structure then find the non-alias one and use that
                    capabilityStructName = self.registry.getNonAliasTypeName(capabilityStructName, self.registry.structs)
                    # Copy defined capability structure data
                    if not capabilityStructName in definedCapabilities:
                        definedCapabilities[capabilityStructName] = []
                    definedCapabilities[capabilityStructName].extend(capabilities.keys())

        # Construct table
        tableData = OrderedDict()
        for capabilityStructName in ['VkVideoCapabilitiesKHR'] + list(videoCodec.capabilities.keys()):
            capabilityStruct = self.registry.structs[self.registry.getNonAliasTypeName(capabilityStructName, self.registry.structs)]
            if not capabilityStruct.name in definedCapabilities:
                continue
            tableData[capabilityStruct.name] = OrderedDict()
            for capability in capabilityStruct.members:
                if not capability in definedCapabilities[capabilityStruct.name]:
                    continue
                # Capability is defined, add it to the table
                tableData[capabilityStruct.name][capability] = functools.partial(self.gen_videoCapability, videoProfileName)

        # Generate table
        table = self.gen_sectionedTable(tableData)
        return '\n#### Video Capabilities\n\n{0}'.format(table)


    def gen_videoFormat(self, definedVideoFormatsPerProfile, videoProfileName, videoFormatCategory, index, struct, section, member, profile = None):
        # If no profile was specified then this is the first column so return the member name
        # decorated with the corresponding limittype specific info and a link to the
        # encompassing structure's manual page
        if profile is None:
            return self.gen_manPageLink(struct, self.formatLimitName(struct, member))

        # If this profile doesn't even define this video format category or this video format index then early out
        if not profile.key in definedVideoFormatsPerProfile:
            return ''
        if not videoFormatCategory.name in definedVideoFormatsPerProfile[profile.key]:
            return ''
        if len(definedVideoFormatsPerProfile[profile.key][videoFormatCategory.name]) <= index:
            return ''
        
        videoFormatProps = definedVideoFormatsPerProfile[profile.key][videoFormatCategory.name][index]

        # Find the video format properties structure data in the profile, if it exists
        videoFormatPropsStruct = None
        videoFormatPropsStructName = None
        if struct in videoFormatProps:
            videoFormatPropsStruct = videoFormatProps[struct]
            videoFormatPropsStructName = struct
        else:
            # Check also for alises
            for alias in self.registry.structs[struct].aliases:
                if alias in videoFormatProps:
                    videoFormatPropsStruct = videoFormatProps[alias]
                    videoFormatPropsStructName = alias

        if videoFormatPropsStruct is not None and member in videoFormatPropsStruct:
            return self.formatVideoProfileProperty(videoFormatPropsStruct[member], videoFormatPropsStructName)

        return '-'


    def gen_videoFormats(self, videoProfileName, videoCodec):
        # Merge all video format categories across the profiles to collect the relevant video format categories to look at
        # Also collect the video format properties that have the necessary prerequisites at least in one Vulkan profile
        # Finally, Collect for each video format category the maximum number of video formats per Vulkan profile
        definedVideoFormats = dict()
        definedVideoFormatProps = set()
        definedVideoFormatsPerProfile = dict()
        for profile in self.profiles:
            definedVideoFormatsCount = dict()
            definedVideoFormatsPerProfile[profile.key] = dict()
            for videoProfile in profile.doc_capabilities.videoProfiles:
                if videoProfileName != self.registry.getVideoProfileNameFromVideoProfile(videoProfile):
                    continue
                if not 'formats' in videoProfile:
                    continue
                for videoFormat in videoProfile['formats']:
                    # Collect the video format properties structures that have their prerequisites met
                    for videoFormatProps in videoFormat:
                        nonAliasStructName = self.registry.getNonAliasTypeName(videoFormatProps, self.registry.structs)
                        if nonAliasStructName in definedVideoFormatProps:
                            continue
                        # Check also for aliases
                        for structName in [ videoFormatProps ] + self.registry.structs[videoFormatProps].aliases:
                            structDef = self.registry.structs[structName]
                            if structDef.definedByVersion is not None and structDef.definedByVersion < profile.apiVersionNumber:
                                definedVideoFormatProps.add(nonAliasStructName)
                            for definedByExtension in structDef.definedByExtensions:
                                if definedByExtension in profile.doc_capabilities.extensions:
                                    definedVideoFormatProps.add(nonAliasStructName)

                    for videoFormatCategory in videoCodec.getVideoFormatCategoriesForFormat(videoFormat, videoProfile['capabilities'] if 'capabilities' in videoProfile else {}, self.registry):
                        # Include the video format in the defined list for the video format category
                        if not videoFormatCategory.name in definedVideoFormatsCount:
                            definedVideoFormatsCount[videoFormatCategory.name] = 0
                            definedVideoFormatsPerProfile[profile.key][videoFormatCategory.name] = []
                        definedVideoFormatsCount[videoFormatCategory.name] += 1
                        definedVideoFormatsPerProfile[profile.key][videoFormatCategory.name].append(videoFormat)

            # Check if this profile increased the maximum format counts per video format category
            for videoFormatCategoryName, count in definedVideoFormatsCount.items():
                if not videoFormatCategoryName in definedVideoFormats or count > definedVideoFormats[videoFormatCategoryName]:
                    definedVideoFormats[videoFormatCategoryName] = count

        # Construct table
        tableData = OrderedDict()
        for videoFormatCategory in videoCodec.formats.values():
            if videoFormatCategory.name in definedVideoFormats:
                # There are formats falling in this video format category
                for index in range(definedVideoFormats[videoFormatCategory.name]):
                    section = tableData["{0} Format #{1}".format(videoFormatCategory.name, index + 1)] = OrderedDict()
                    # Include all video format properties that apply to this video format category
                    for videoFormatProps in [ 'VkVideoFormatPropertiesKHR' ] + list(videoFormatCategory.properties.keys()):
                        propsStruct = self.registry.structs[self.registry.getNonAliasTypeName(videoFormatProps, self.registry.structs)]
                        # Skip video format properties not defined by profile prequisites
                        if not propsStruct.name in definedVideoFormatProps:
                            continue

                        # Setup row handler function and add rows for each property
                        rowHandler = functools.partial(self.gen_videoFormat, definedVideoFormatsPerProfile, videoProfileName, videoFormatCategory, index, propsStruct.name)
                        section.update({ row: rowHandler for row in propsStruct.members.keys()})

        # Generate table
        table = self.gen_sectionedTable(tableData)
        return '\n#### Video Formats\n\n{0}'.format(table)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument('--registry', '-r', action='store', required=True,
                        help='Use specified registry file instead of vk.xml (video.xml must be present in the same directory for video support).')
    parser.add_argument('--input', '-i', action='store',
                        help='Path to directory with profiles.')
    parser.add_argument('--input-filenames', action='store',
                        help='The optional filenames of the profiles files in the directory. If this parameter is not set, all profiles files are loaded.')
    parser.add_argument('--api', action='store',
                        default='vulkan',
                        choices=['vulkan'],
                        help="Target API")
    parser.add_argument('--output-library-inc', action='store',
                        help='Output include directory for profile library')
    parser.add_argument('--output-library-src', action='store',
                        help='Output source directory for profile library')
    parser.add_argument('--output-library-filename', action='store',
                        default='vulkan_profiles',
                        help='Output filename for profile library, default "vulkan_profiles"')
    parser.add_argument('--output-schema', action='store',
                        help='Output file for JSON profile schema')
    parser.add_argument('--output-doc', action='store',
                        help='Output file for profiles markdown documentation')
    parser.add_argument('--validate', '-v', action='store_true',
                        help='Validate generated JSON profile schema and JSON profiles against the schema')
    parser.add_argument('--debug', '-d', action='store_true',
                        help='Also generate library variant with debug messages')
    parser.add_argument('--config', '-c', action='store',
                        default='release',
                        choices=['release', 'debug'],
                        help='Select the build configuration, either "Release" or "Debug" for the API Library to generate debug messages')

    parser.set_defaults(config='Release')

    args = parser.parse_args()

    if args.output_library_inc is None and args.output_schema is None and args.output_doc is None and not args.validate:
        parser.print_help()
        exit()

    if args.output_library_inc != None:
        if args.registry is None or args.input is None or args.output_library_inc is None:
            Log.e("Generating the profile library requires specifying --registry, --input and --output-library-inc arguments")
            parser.print_help()
            exit()

    if args.output_schema != None:
        if args.registry is None:
            Log.e("Generating the profile schema requires specifying --registry and ---output-schema arguments")
            parser.print_help()
            exit()

    if args.output_doc != None:
        if args.registry is None or args.input is None:
            Log.e("Generating the profile schema requires specifying --registry, --input and --output-doc arguments")
            parser.print_help()
            exit()

    schema = None

    if args.registry != None:
        registry = VulkanRegistry(args.registry, args.api)
        vk: VulkanObject = initVulkanObject(args.registry, True)

    if args.output_schema != None or args.validate:
        generator = VulkanProfilesSchemaGenerator(registry)
        generator2 = VulkanProfilesSchemaGenerator2(vk)
        if args.output_schema is not None:
            #generator.generate(args.output_schema)
            generator2.generate(args.output_schema)
        if args.validate:
            generator.validate()
            schema = generator.schema

    profiles_filenames = []
    if args.input_filenames:
        profiles_filenames = args.input_filenames.split(',')

    if args.input != None:
        input_profiles_files = VulkanProfilesFiles(registry, args.input, profiles_filenames, args.validate, schema)

    if args.output_library_inc != None:
        generator = VulkanProfilesLibraryGenerator(registry, input_profiles_files, args.output_library_filename, str.lower(args.config) == 'debug')
        generator.generate(args.output_library_inc, args.output_library_src)
        if args.debug:
            generator = VulkanProfilesLibraryGenerator(registry, input_profiles_files, args.output_library_filename, True)
            generator.generate(args.output_library_inc + '/debug', args.output_library_src + '/debug')

    if args.output_doc != None:
        generator = VulkanProfilesDocGenerator(registry, input_profiles_files)
        generator.generate(args.output_doc)
