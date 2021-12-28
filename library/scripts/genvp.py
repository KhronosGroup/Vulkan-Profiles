#!/usr/bin/python3
#
# Copyright (c) 2021-2022 LunarG, Inc.
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
# Author: Daniel Rakos, RasterGrid

import os
import re
import argparse
import xml.etree.ElementTree as etree
import json

COPYRIGHT_HEADER = '''/**
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
 * DO NOT EDIT: This file is generated.
 */
'''

H_HEADER = '''
#ifndef VULKAN_PROFILES_
#define VULKAN_PROFILES_ 1

#define VPAPI_ATTR

#ifdef __cplusplus
    extern "C" {
#endif

#include <vulkan/vulkan_core.h>
#ifdef VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan_beta.h>

#endif
'''

H_FOOTER = '''
#ifdef __cplusplus
}
#endif

#endif //VULKAN_PROFILES_
'''

CPP_HEADER = '''
#include <vulkan/vulkan_profiles.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <vector>
#include <algorithm>

#define _vpArraySize(arr) static_cast<uint32_t>(sizeof(arr) / sizeof(arr[0]))
'''

HPP_HEADER = '''
#ifndef VULKAN_PROFILES_
#define VULKAN_PROFILES_ 1

#define VPAPI_ATTR inline

#include <vulkan/vulkan_core.h>
#ifdef VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan_beta.h>
#endif
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <vector>
#include <algorithm>

#define _vpArraySize(arr) static_cast<uint32_t>(sizeof(arr) / sizeof(arr[0]))
'''

HPP_FOOTER = '''
#endif //VULKAN_PROFILES_
'''

API_DEFS = '''
#define VP_MAX_PROFILE_NAME_SIZE 256U

typedef struct VpProfileProperties {
    char        profileName[VP_MAX_PROFILE_NAME_SIZE];
    uint32_t    specVersion;
} VpProfileProperties;

typedef enum VpInstanceCreateFlagBits {
    VP_INSTANCE_CREATE_MERGE_EXTENSIONS_BIT = 0x00000001,
    VP_INSTANCE_CREATE_OVERRIDE_EXTENSIONS_BIT = 0x00000002,

    VP_INSTANCE_CREATE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
} VpInstanceCreateFlagBits;
typedef VkFlags VpInstanceCreateFlags;

typedef struct VpInstanceCreateInfo {
    const VkInstanceCreateInfo* pCreateInfo;
    const VpProfileProperties*  pProfile;
    VpInstanceCreateFlags       flags;
} VpInstanceCreateInfo;

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
    const VkDeviceCreateInfo*   pCreateInfo;
    const VpProfileProperties*  pProfile;
    VpDeviceCreateFlags         flags;
} VpDeviceCreateInfo;

// Query the list of available profiles in the library
VPAPI_ATTR VkResult vpGetProfiles(uint32_t *pPropertyCount, VpProfileProperties *pProperties);

// List the recommended fallback profiles of a profile
VPAPI_ATTR VkResult vpGetProfileFallbacks(const VpProfileProperties *pProfile, uint32_t *pPropertyCount, VpProfileProperties *pProperties);

// Check whether a profile is supported at the instance level
VPAPI_ATTR VkResult vpGetInstanceProfileSupport(const char *pLayerName, const VpProfileProperties *pProfile, VkBool32 *pSupported);

// Create a VkInstance with the profile instance extensions enabled
VPAPI_ATTR VkResult vpCreateInstance(const VpInstanceCreateInfo *pCreateInfo,
                                     const VkAllocationCallbacks *pAllocator, VkInstance *pInstance);

// Check whether a profile is supported by the physical device
VPAPI_ATTR VkResult vpGetPhysicalDeviceProfileSupport(VkPhysicalDevice physicalDevice, const VpProfileProperties *pProfile, VkBool32 *pSupported);

// Create a VkDevice with the profile features and device extensions enabled
VPAPI_ATTR VkResult vpCreateDevice(VkPhysicalDevice physicalDevice, const VpDeviceCreateInfo *pCreateInfo,
                                   const VkAllocationCallbacks *pAllocator, VkDevice *pDevice);

// Query the list of instance extensions of a profile
VPAPI_ATTR VkResult vpGetProfileInstanceExtensionProperties(const VpProfileProperties *pProfile, uint32_t *pPropertyCount,
                                                            VkExtensionProperties *pProperties);

// Query the list of device extensions of a profile
VPAPI_ATTR VkResult vpGetProfileDeviceExtensionProperties(const VpProfileProperties *pProfile, uint32_t *pPropertyCount,
                                                          VkExtensionProperties *pProperties);

// Fill the feature structures with the requirements of a profile
VPAPI_ATTR void vpGetProfileFeatures(const VpProfileProperties *pProfile, void *pNext);

// Query the list of feature structure types specified by the profile
VPAPI_ATTR VkResult vpGetProfileFeatureStructureTypes(const VpProfileProperties *pProfile, uint32_t *pPropertyCount,
                                                      VkStructureType *pProperties);

// Fill the property structures with the requirements of a profile
VPAPI_ATTR void vpGetProfileProperties(const VpProfileProperties *pProfile, void *pNext);

// Query the list of property structure types specified by the profile
VPAPI_ATTR VkResult vpGetProfilePropertyStructureTypes(const VpProfileProperties *pProfile, uint32_t *pPropertyCount,
                                                       VkStructureType *pProperties);

// Query the requirements of queue families by a profile
VPAPI_ATTR VkResult vpGetProfileQueueFamilyProperties(const VpProfileProperties *pProfile, uint32_t *pPropertyCount,
                                                      VkQueueFamilyProperties2KHR *pProperties);

// Query the list of query family structure types specified by the profile
VPAPI_ATTR VkResult vpGetProfileQueueFamilyStructureTypes(const VpProfileProperties *pProfile, uint32_t *pPropertyCount,
                                                          VkStructureType *pProperties);

// Query the list of formats with specified requirements by a profile
VPAPI_ATTR VkResult vpGetProfileFormats(const VpProfileProperties *pProfile, uint32_t *pFormatCount, VkFormat *pFormats);

// Query the requirements of a format for a profile
VPAPI_ATTR void vpGetProfileFormatProperties(const VpProfileProperties *pProfile, VkFormat format, void *pNext);

// Query the list of format structure types specified by the profile
VPAPI_ATTR VkResult vpGetProfileFormatStructureTypes(const VpProfileProperties *pProfile, VkFormat format, uint32_t *pPropertyCount,
                                                     VkStructureType *pProperties);
'''

PRIVATE_IMPL_BODY = '''
using _pfnvpStructFiller = void(*)(VkBaseOutStructure* p);
using _pfnvpStructComparator = bool(*)(VkBaseOutStructure* p);

struct _vpFeatureDesc {
    _pfnvpStructFiller              pfnFiller;
    _pfnvpStructComparator          pfnComparator;
};

struct _vpPropertyDesc {
    _pfnvpStructFiller              pfnFiller;
    _pfnvpStructComparator          pfnComparator;
};

struct _vpQueueFamilyDesc {
    _pfnvpStructFiller              pfnFiller;
    _pfnvpStructComparator          pfnComparator;
};

struct _vpFormatDesc {
    VkFormat                        format;
    _pfnvpStructFiller              pfnFiller;
    _pfnvpStructComparator          pfnComparator;
};

struct _vpProfileDesc {
    VpProfileProperties             props;
    uint32_t                        minApiVersion;

    const VkExtensionProperties*    pInstanceExtensions;
    uint32_t                        instanceExtensionCount;

    const VkExtensionProperties*    pDeviceExtensions;
    uint32_t                        deviceExtensionCount;

    const VpProfileProperties*      pFallbacks;
    uint32_t                        fallbackCount;

    const VkStructureType*          pFeatureStructTypes;
    uint32_t                        featureStructTypeCount;
    _vpFeatureDesc                  feature;

    const VkStructureType*          pPropertyStructTypes;
    uint32_t                        propertyStructTypeCount;
    _vpPropertyDesc                 property;

    const VkStructureType*          pQueueFamilyStructTypes;
    uint32_t                        queueFamilyStructTypeCount;
    const _vpQueueFamilyDesc*       pQueueFamilies;
    uint32_t                        queueFamilyCount;

    const VkStructureType*          pFormatStructTypes;
    uint32_t                        formatStructTypeCount;
    const _vpFormatDesc*            pFormats;
    uint32_t                        formatCount;
};

VPAPI_ATTR const _vpProfileDesc* _vpGetProfileDesc(const _vpProfileDesc* pProfiles, uint32_t profileCount,
                                                   const char profileName[VP_MAX_PROFILE_NAME_SIZE]) {
    for (uint32_t i = 0; i < profileCount; ++i) {
        if (strncmp(pProfiles[i].props.profileName, profileName, VP_MAX_PROFILE_NAME_SIZE) == 0) return &pProfiles[i];
    }
    return nullptr;
}

VPAPI_ATTR bool _vpCheckExtension(const VkExtensionProperties *supportedProperties, size_t supportedSize,
                                  const char *requestedExtension, uint32_t expectedVersion = 0) {
    for (size_t i = 0, n = supportedSize; i < n; ++i) {
        if (strcmp(supportedProperties[i].extensionName, requestedExtension) == 0) {
            return supportedProperties[i].specVersion > expectedVersion;
        }
    }
    return false;
}

VPAPI_ATTR void _vpGetInstanceExtensions(const VpInstanceCreateInfo *pCreateInfo, uint32_t propertyCount,
                                         const VkExtensionProperties *pProperties, std::vector<const char *> &extensions) {
    if (pCreateInfo->flags & VP_INSTANCE_CREATE_MERGE_EXTENSIONS_BIT) {
        for (int i = 0, n = propertyCount; i < n; ++i) {
            extensions.push_back(pProperties[i].extensionName);
        }

        for (uint32_t i = 0; i < pCreateInfo->pCreateInfo->enabledExtensionCount; ++i) {
            if (_vpCheckExtension(pProperties, propertyCount, pCreateInfo->pCreateInfo->ppEnabledExtensionNames[i])) {
                continue;
            }
            extensions.push_back(pCreateInfo->pCreateInfo->ppEnabledExtensionNames[i]);
        }
    } else {  // or VP_INSTANCE_CREATE_OVERRIDE_EXTENSIONS_BIT
        for (int i = 0, n = pCreateInfo->pCreateInfo->enabledExtensionCount; i < n; ++i) {
            extensions.push_back(pCreateInfo->pCreateInfo->ppEnabledExtensionNames[i]);
        }
    }
}

VPAPI_ATTR void _vpGetDeviceExtensions(const VpDeviceCreateInfo *pCreateInfo, uint32_t propertyCount,
                                       const VkExtensionProperties *pProperties, std::vector<const char *> &extensions) {
    if (pCreateInfo->flags & VP_DEVICE_CREATE_MERGE_EXTENSIONS_BIT) {
        for (int i = 0, n = propertyCount; i < n; ++i) {
            extensions.push_back(pProperties[i].extensionName);
        }

        for (uint32_t i = 0; i < pCreateInfo->pCreateInfo->enabledExtensionCount; ++i) {
            if (_vpCheckExtension(pProperties, propertyCount, pCreateInfo->pCreateInfo->ppEnabledExtensionNames[i])) {
                continue;
            }
            extensions.push_back(pCreateInfo->pCreateInfo->ppEnabledExtensionNames[i]);
        }
    } else {  // or VP_DEVICE_CREATE_OVERRIDE_EXTENSIONS_BIT
        for (int i = 0, n = pCreateInfo->pCreateInfo->enabledExtensionCount; i < n; ++i) {
            extensions.push_back(pCreateInfo->pCreateInfo->ppEnabledExtensionNames[i]);
        }
    }
}

VPAPI_ATTR const void* _vpGetStructure(const void* pNext, VkStructureType type) {
    const VkBaseOutStructure *p = static_cast<const VkBaseOutStructure*>(pNext);
    while (p != nullptr) {
        if (p->sType == type) return p;
        p = p->pNext;
    }
    return nullptr;
}

VPAPI_ATTR void* _vpGetStructure(void* pNext, VkStructureType type) {
    VkBaseOutStructure *p = static_cast<VkBaseOutStructure*>(pNext);
    while (p != nullptr) {
        if (p->sType == type) return p;
        p = p->pNext;
    }
    return nullptr;
}
'''

PUBLIC_IMPL_BODY = '''
VPAPI_ATTR VkResult vpGetProfiles(uint32_t *pPropertyCount, VpProfileProperties *pProperties) {
    VkResult result = VK_SUCCESS;
    const uint32_t profileCount = _vpArraySize(_vpProfiles);

    if (pProperties == nullptr) {
        *pPropertyCount = profileCount;
    } else {
        if (*pPropertyCount < profileCount) {
            result = VK_INCOMPLETE;
        } else {
            *pPropertyCount = profileCount;
        }
        for (uint32_t i = 0; i < *pPropertyCount; ++i) {
            pProperties[i] = _vpProfiles[i].props;
        }
    }
    return result;
}

VPAPI_ATTR VkResult vpGetProfileFallbacks(const VpProfileProperties *pProfile, uint32_t *pPropertyCount, VpProfileProperties *pProperties) {
    VkResult result = VK_SUCCESS;

    const _vpProfileDesc* pDesc = _vpGetProfileDesc(_vpProfiles, _vpArraySize(_vpProfiles), pProfile->profileName);
    if (pDesc == nullptr) return VK_ERROR_UNKNOWN;

    if (pProperties == nullptr) {
        *pPropertyCount = pDesc->fallbackCount;
    } else {
        if (*pPropertyCount < pDesc->fallbackCount) {
            result = VK_INCOMPLETE;
        } else {
            *pPropertyCount = pDesc->fallbackCount;
        }
        for (uint32_t i = 0; i < *pPropertyCount; ++i) {
            pProperties[i] = pDesc->pFallbacks[i];
        }
    }
    return result;
}

VPAPI_ATTR VkResult vpGetInstanceProfileSupport(const char *pLayerName, const VpProfileProperties *pProfile, VkBool32 *pSupported) {
    assert(pProfile != nullptr);
    assert(pSupported != nullptr);
    VkResult result = VK_SUCCESS;

    uint32_t apiVersion;
    vkEnumerateInstanceVersion(&apiVersion);

    uint32_t extCount;
    result = vkEnumerateInstanceExtensionProperties(pLayerName, &extCount, nullptr);
    if (result != VK_SUCCESS) return result;
    std::vector<VkExtensionProperties> ext(extCount);
    result = vkEnumerateInstanceExtensionProperties(pLayerName, &extCount, ext.data());
    if (result != VK_SUCCESS) return result;

    *pSupported = VK_FALSE;

    const _vpProfileDesc* pDesc = _vpGetProfileDesc(_vpProfiles, _vpArraySize(_vpProfiles), pProfile->profileName);
    if (pDesc == nullptr) return VK_ERROR_UNKNOWN;

    if (pDesc->props.specVersion < pProfile->specVersion) return result;

    if (VK_VERSION_PATCH(apiVersion) < VK_VERSION_PATCH(pDesc->minApiVersion)) return result;

    for (uint32_t i = 0; i < pDesc->instanceExtensionCount; ++i) {
        if (!_vpCheckExtension(ext.data(), ext.size(),
            pDesc->pInstanceExtensions[i].extensionName,
            pDesc->pInstanceExtensions[i].specVersion)) return result;
    }

    *pSupported = VK_TRUE;
    return result;
}

VPAPI_ATTR VkResult vpCreateInstance(const VpInstanceCreateInfo *pCreateInfo,
                                     const VkAllocationCallbacks *pAllocator, VkInstance *pInstance) {
    VkInstanceCreateInfo createInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
    std::vector<const char*> extensions;
    VkInstanceCreateInfo* pInstanceCreateInfo = nullptr;
    VkExtensionProperties* pProfileExtensions = nullptr;
    uint32_t profileExtensionCount = 0;

    if (pCreateInfo != nullptr && pCreateInfo->pCreateInfo != nullptr) {
        createInfo = *pCreateInfo->pCreateInfo;
        pInstanceCreateInfo = &createInfo;

        const _vpProfileDesc* pDesc = nullptr;
        if (pCreateInfo->pProfile != nullptr) {
            pDesc = _vpGetProfileDesc(_vpProfiles, _vpArraySize(_vpProfiles), pCreateInfo->pProfile->profileName);
            if (pDesc == nullptr) return VK_ERROR_UNKNOWN;
        }

        if (pDesc != nullptr && pDesc->pInstanceExtensions != nullptr) {
            _vpGetInstanceExtensions(pCreateInfo, pDesc->instanceExtensionCount, pDesc->pInstanceExtensions, extensions);
            createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
            createInfo.ppEnabledExtensionNames = extensions.data();
        }

        if (createInfo.pApplicationInfo == nullptr) {
            appInfo.apiVersion = pDesc->minApiVersion;
            createInfo.pApplicationInfo = &appInfo;
        }
    }

    return vkCreateInstance(pInstanceCreateInfo, pAllocator, pInstance);
}

VPAPI_ATTR VkResult vpGetProfileInstanceExtensionProperties(const VpProfileProperties *pProfile, uint32_t *pPropertyCount,
                                                            VkExtensionProperties *pProperties) {
    VkResult result = VK_SUCCESS;

    const _vpProfileDesc* pDesc = _vpGetProfileDesc(_vpProfiles, _vpArraySize(_vpProfiles), pProfile->profileName);
    if (pDesc == nullptr) return VK_ERROR_UNKNOWN;

    if (pProperties == nullptr) {
        *pPropertyCount = pDesc->instanceExtensionCount;
    } else {
        if (*pPropertyCount < pDesc->instanceExtensionCount) {
            result = VK_INCOMPLETE;
        } else {
            *pPropertyCount = pDesc->instanceExtensionCount;
        }
        for (uint32_t i = 0; i < *pPropertyCount; ++i) {
            pProperties[i] = pDesc->pInstanceExtensions[i];
        }
    }
    return result;
}

VPAPI_ATTR VkResult vpGetProfileDeviceExtensionProperties(const VpProfileProperties *pProfile, uint32_t *pPropertyCount,
                                                          VkExtensionProperties *pProperties) {
    VkResult result = VK_SUCCESS;

    const _vpProfileDesc* pDesc = _vpGetProfileDesc(_vpProfiles, _vpArraySize(_vpProfiles), pProfile->profileName);
    if (pDesc == nullptr) return VK_ERROR_UNKNOWN;

    if (pProperties == nullptr) {
        *pPropertyCount = pDesc->deviceExtensionCount;
    } else {
        if (*pPropertyCount < pDesc->deviceExtensionCount) {
            result = VK_INCOMPLETE;
        } else {
            *pPropertyCount = pDesc->deviceExtensionCount;
        }
        for (uint32_t i = 0; i < *pPropertyCount; ++i) {
            pProperties[i] = pDesc->pDeviceExtensions[i];
        }
    }
    return result;
}

VPAPI_ATTR void vpGetProfileFeatures(const VpProfileProperties *pProfile, void *pNext)
{
    const _vpProfileDesc* pDesc = _vpGetProfileDesc(_vpProfiles, _vpArraySize(_vpProfiles), pProfile->profileName);
    if (pDesc != nullptr && pDesc->feature.pfnFiller != nullptr) {
        VkBaseOutStructure* p = static_cast<VkBaseOutStructure*>(pNext);
        while (p != nullptr) {
            pDesc->feature.pfnFiller(p);
            p = p->pNext;
        }
    }
}

VPAPI_ATTR VkResult vpGetProfileFeatureStructureTypes(const VpProfileProperties *pProfile, uint32_t *pPropertyCount,
                                                      VkStructureType *pProperties)
{
    VkResult result = VK_SUCCESS;

    const _vpProfileDesc* pDesc = _vpGetProfileDesc(_vpProfiles, _vpArraySize(_vpProfiles), pProfile->profileName);
    if (pDesc == nullptr) return VK_ERROR_UNKNOWN;

    if (pProperties == nullptr) {
        *pPropertyCount = pDesc->featureStructTypeCount;
    } else {
        if (*pPropertyCount < pDesc->featureStructTypeCount) {
            result = VK_INCOMPLETE;
        } else {
            *pPropertyCount = pDesc->featureStructTypeCount;
        }
        for (uint32_t i = 0; i < *pPropertyCount; ++i) {
            pProperties[i] = pDesc->pFeatureStructTypes[i];
        }
    }
    return result;
}

VPAPI_ATTR void vpGetProfileProperties(const VpProfileProperties *pProfile, void *pNext)
{
    const _vpProfileDesc* pDesc = _vpGetProfileDesc(_vpProfiles, _vpArraySize(_vpProfiles), pProfile->profileName);
    if (pDesc != nullptr && pDesc->property.pfnFiller != nullptr) {
        VkBaseOutStructure* p = static_cast<VkBaseOutStructure*>(pNext);
        while (p != nullptr) {
            pDesc->property.pfnFiller(p);
            p = p->pNext;
        }
    }
}

VPAPI_ATTR VkResult vpGetProfilePropertyStructureTypes(const VpProfileProperties *pProfile, uint32_t *pPropertyCount,
                                                       VkStructureType *pProperties)
{
    VkResult result = VK_SUCCESS;

    const _vpProfileDesc* pDesc = _vpGetProfileDesc(_vpProfiles, _vpArraySize(_vpProfiles), pProfile->profileName);
    if (pDesc == nullptr) return VK_ERROR_UNKNOWN;

    if (pProperties == nullptr) {
        *pPropertyCount = pDesc->propertyStructTypeCount;
    } else {
        if (*pPropertyCount < pDesc->propertyStructTypeCount) {
            result = VK_INCOMPLETE;
        } else {
            *pPropertyCount = pDesc->propertyStructTypeCount;
        }
        for (uint32_t i = 0; i < *pPropertyCount; ++i) {
            pProperties[i] = pDesc->pPropertyStructTypes[i];
        }
    }
    return result;
}

VPAPI_ATTR VkResult vpGetProfileQueueFamilyProperties(const VpProfileProperties *pProfile, uint32_t *pPropertyCount,
                                                      VkQueueFamilyProperties2KHR *pProperties)
{
    VkResult result = VK_SUCCESS;

    const _vpProfileDesc* pDesc = _vpGetProfileDesc(_vpProfiles, _vpArraySize(_vpProfiles), pProfile->profileName);
    if (pDesc == nullptr) return VK_ERROR_UNKNOWN;

    if (pProperties == nullptr) {
        *pPropertyCount = pDesc->queueFamilyCount;
    } else {
        if (*pPropertyCount < pDesc->queueFamilyCount) {
            result = VK_INCOMPLETE;
        } else {
            *pPropertyCount = pDesc->queueFamilyCount;
        }
        for (uint32_t i = 0; i < *pPropertyCount; ++i) {
            VkBaseOutStructure* p = static_cast<VkBaseOutStructure*>(static_cast<void*>(&pProperties[i]));
            while (p != nullptr) {
                pDesc->pQueueFamilies[i].pfnFiller(p);
                p = p->pNext;
            }
        }
    }
    return result;
}

VPAPI_ATTR VkResult vpGetProfileQueueFamilyStructureTypes(const VpProfileProperties *pProfile, uint32_t *pPropertyCount,
                                                          VkStructureType *pProperties)
{
    VkResult result = VK_SUCCESS;

    const _vpProfileDesc* pDesc = _vpGetProfileDesc(_vpProfiles, _vpArraySize(_vpProfiles), pProfile->profileName);
    if (pDesc == nullptr) return VK_ERROR_UNKNOWN;

    if (pProperties == nullptr) {
        *pPropertyCount = pDesc->queueFamilyStructTypeCount;
    } else {
        if (*pPropertyCount < pDesc->queueFamilyStructTypeCount) {
            result = VK_INCOMPLETE;
        } else {
            *pPropertyCount = pDesc->queueFamilyStructTypeCount;
        }
        for (uint32_t i = 0; i < *pPropertyCount; ++i) {
            pProperties[i] = pDesc->pQueueFamilyStructTypes[i];
        }
    }
    return result;
}

VPAPI_ATTR VkResult vpGetProfileFormats(const VpProfileProperties *pProfile, uint32_t *pFormatCount, VkFormat *pFormats)
{
    VkResult result = VK_SUCCESS;

    const _vpProfileDesc* pDesc = _vpGetProfileDesc(_vpProfiles, _vpArraySize(_vpProfiles), pProfile->profileName);
    if (pDesc == nullptr) return VK_ERROR_UNKNOWN;

    if (pFormats == nullptr) {
        *pFormatCount = pDesc->formatCount;
    } else {
        if (*pFormatCount < pDesc->formatCount) {
            result = VK_INCOMPLETE;
        } else {
            *pFormatCount = pDesc->formatCount;
        }
        for (uint32_t i = 0; i < *pFormatCount; ++i) {
            pFormats[i] = pDesc->pFormats[i].format;
        }
    }
    return result;
}

VPAPI_ATTR void vpGetProfileFormatProperties(const VpProfileProperties *pProfile, VkFormat format, void *pNext)
{
    VkResult result = VK_SUCCESS;

    const _vpProfileDesc* pDesc = _vpGetProfileDesc(_vpProfiles, _vpArraySize(_vpProfiles), pProfile->profileName);
    if (pDesc == nullptr) return;

    for (uint32_t i = 0; i < pDesc->formatCount; ++i) {
        if (pDesc->pFormats[i].format == format) {
            VkBaseOutStructure* p = static_cast<VkBaseOutStructure*>(static_cast<void*>(pNext));
            while (p != nullptr) {
                pDesc->pFormats[i].pfnFiller(p);
                p = p->pNext;
            }
#if defined(VK_VERSION_1_3) || defined(VK_KHR_format_feature_flags2)
            VkFormatProperties2KHR* fp2 = static_cast<VkFormatProperties2KHR*>(
                _vpGetStructure(pNext, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR));
            VkFormatProperties3KHR* fp3 = static_cast<VkFormatProperties3KHR*>(
                _vpGetStructure(pNext, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3_KHR));
            if (fp3 != nullptr) {
                VkFormatProperties2KHR fp{ VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR };
                pDesc->pFormats[i].pfnFiller(static_cast<VkBaseOutStructure*>(static_cast<void*>(&fp)));
                fp3->linearTilingFeatures = static_cast<VkFormatFeatureFlags2KHR>(fp3->linearTilingFeatures | fp.formatProperties.linearTilingFeatures);
                fp3->optimalTilingFeatures = static_cast<VkFormatFeatureFlags2KHR>(fp3->optimalTilingFeatures | fp.formatProperties.optimalTilingFeatures);
                fp3->bufferFeatures = static_cast<VkFormatFeatureFlags2KHR>(fp3->bufferFeatures | fp.formatProperties.bufferFeatures);
            }
            if (fp2 != nullptr) {
                VkFormatProperties3KHR fp{ VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3_KHR };
                pDesc->pFormats[i].pfnFiller(static_cast<VkBaseOutStructure*>(static_cast<void*>(&fp)));
                fp2->formatProperties.linearTilingFeatures = static_cast<VkFormatFeatureFlags>(fp2->formatProperties.linearTilingFeatures | fp.linearTilingFeatures);
                fp2->formatProperties.optimalTilingFeatures = static_cast<VkFormatFeatureFlags>(fp2->formatProperties.optimalTilingFeatures | fp.optimalTilingFeatures);
                fp2->formatProperties.bufferFeatures = static_cast<VkFormatFeatureFlags>(fp2->formatProperties.bufferFeatures | fp.bufferFeatures);
            }
#endif
        }
    }
}

VPAPI_ATTR VkResult vpGetProfileFormatStructureTypes(const VpProfileProperties *pProfile, VkFormat format, uint32_t *pPropertyCount,
                                                     VkStructureType *pProperties)
{
    VkResult result = VK_SUCCESS;

    const _vpProfileDesc* pDesc = _vpGetProfileDesc(_vpProfiles, _vpArraySize(_vpProfiles), pProfile->profileName);
    if (pDesc == nullptr) return VK_ERROR_UNKNOWN;

    if (pProperties == nullptr) {
        *pPropertyCount = pDesc->formatStructTypeCount;
    } else {
        if (*pPropertyCount < pDesc->formatStructTypeCount) {
            result = VK_INCOMPLETE;
        } else {
            *pPropertyCount = pDesc->formatStructTypeCount;
        }
        for (uint32_t i = 0; i < *pPropertyCount; ++i) {
            pProperties[i] = pDesc->pFormatStructTypes[i];
        }
    }
    return result;
}
'''


class Log():
    def f(msg):
        print('FATAL: ' + msg)
        raise Exception(msg)

    def e(msg):
        print('ERROR: ' + msg)

    def w(msg):
        print('WARNING: ' + msg)

    def i(msg):
        print(msg)


class VulkanPlatform():
    def __init__(self, data):
        self.name = data.get('name')
        self.protect = data.get('protect')


class VulkanStructMember():
    def __init__(self, name, type, limittype, isArray = False):
        self.name = name
        self.type = type
        self.limittype = limittype
        self.isArray = isArray


class VulkanStruct():
    def __init__(self, name):
        self.name = name
        self.sType = None
        self.extends = []
        self.members = dict()
        self.aliases = [ name ]
        self.definedByVersion = None
        self.definedByExtensions = []


class VulkanDefinitionScope():
    def parseAliases(self, xml):
        self.sTypeAliases = dict()
        for sTypeAlias in xml.findall("./require/enum[@alias]"):
            if re.search(r'^VK_STRUCTURE_TYPE_.*', sTypeAlias.get('name')):
                self.sTypeAliases[sTypeAlias.get('alias')] = sTypeAlias.get('name')


class VulkanVersion(VulkanDefinitionScope):
    def __init__(self, xml):
        self.name = xml.get('name')
        self.number = xml.get('number')
        self.parseAliases(xml)


class VulkanExtension(VulkanDefinitionScope):
    def __init__(self, xml, upperCaseName):
        self.name = xml.get('name')
        self.upperCaseName = upperCaseName
        self.type = xml.get('type')
        self.platform = xml.get('platform')
        self.parseAliases(xml)


class VulkanRegistry():
    def __init__(self, registryFile):
        Log.i("Loading registry file: '{0}'".format(registryFile))
        xml = etree.parse(registryFile)
        self.parsePlatformInfo(xml)
        self.parseVersionInfo(xml)
        self.parseExtensionInfo(xml)
        self.parseStructInfo(xml)
        self.parsePrerequisites(xml)
        self.parseAliases(xml)
        self.applyWorkarounds()

    def parsePlatformInfo(self, xml):
        self.platforms = dict()
        for plat in xml.findall("./platforms/platform"):
            self.platforms[plat.get('name')] = VulkanPlatform(plat)


    def parseVersionInfo(self, xml):
        self.versions = dict()
        for feature in xml.findall('./feature'):
            if re.search(r"^[1-9][0-9]*\.[0-9]+$", feature.get('number')):
                self.versions[feature.get('number')] = VulkanVersion(feature)
            else:
                Log.f("Unsupported feature with number '{0}'", feature.get('number'))


    def parseExtensionInfo(self, xml):
        self.extensions = dict()
        for ext in xml.findall("./extensions/extension"):
            # Only care about enabled extensions
            if ext.get('supported') == "vulkan":
                name = ext.get('name')

                # Find name enum (due to inconsistencies in lower case and upper case names this is non-trivial)
                foundNameEnum = False
                matches = ext.findall("./require/enum[@value='\"" + name + "\"']")
                for match in matches:
                    if match.get('name').endswith("_EXTENSION_NAME"):
                        # Add extension definition
                        self.extensions[name] = VulkanExtension(ext, match.get('name')[:-len("_EXTENSION_NAME")])
                        foundNameEnum = True
                        break
                if not foundNameEnum:
                    Log.f("Cannot find name enum for extension '{0}'".format(name))


    def parseStructInfo(self, xml):
        self.structs = dict()
        for struct in xml.findall("./types/type[@category='struct']"):
            # Define base struct information
            structDef = VulkanStruct(struct.get('name'))

            # Find out whether it's an extension structure
            extends = struct.get('structextends')
            if extends != None:
                structDef.extends = extends.split(',')

            # Find sType value
            sType = struct.find("./member[name='sType']")
            if sType != None:
                structDef.sType = sType.get('values')

            # Parse struct members
            for member in struct.findall('./member'):
                name = member.find('./name').text
                tail = member.find('./name').tail
                type = member.find('./type').text

                # Only add real members (skip sType and pNext)
                if name != 'sType' and name != 'pNext':
                    # Define base member information
                    structDef.members[name] = VulkanStructMember(
                        name,
                        type,
                        member.get('limittype')
                    )

                    # Detect if it's an array
                    if tail != None and tail[0] == '[':
                        structDef.members[name].isArray = True

            # Store struct definition
            self.structs[struct.get('name')] = structDef


    def parsePrerequisites(self, xml):
        # Check features (i.e. API versions)
        for feature in xml.findall('./feature'):
            for requireType in feature.findall('./require/type'):
                # Add feature as the source of the definition of a struct
                if requireType.get('name') in self.structs:
                    self.structs[requireType.get('name')].definedByVersion = feature.get('number')

        # Check extensions
        for extension in xml.findall('./extensions/extension'):
            for requireType in extension.findall('./require/type'):
                # Add extension as the source of the definition of a struct
                if requireType.get('name') in self.structs:
                    self.structs[requireType.get('name')].definedByExtensions.append(extension.get('name'))


    def parseAliases(self, xml):
        # Find any struct aliases
        for struct in xml.findall("./types/type[@category='struct']"):
            alias = struct.get('alias')
            if alias != None:
                if alias in self.structs:
                    baseStructDef = self.structs[alias]
                    aliasStructDef = self.structs[struct.get('name')]

                    # Fill missing struct information for the alias
                    aliasStructDef.extends = baseStructDef.extends
                    aliasStructDef.members = baseStructDef.members
                    aliasStructDef.aliases = baseStructDef.aliases
                    aliasStructDef.aliases.append(struct.get('name'))

                    if baseStructDef.sType != None:
                        sTypeAlias = None

                        # First try to find sType alias in core versions
                        if aliasStructDef.definedByVersion != None:
                            for version in self.versions:
                                if version <= aliasStructDef.definedByVersion:
                                    sTypeAlias = self.versions[version].sTypeAliases.get(baseStructDef.sType)
                                    if sTypeAlias != None:
                                        break

                        # Otherwise need to find sType alias in extension
                        if sTypeAlias == None:
                            for extName in aliasStructDef.definedByExtensions:
                                sTypeAlias = self.extensions[extName].sTypeAliases.get(baseStructDef.sType)
                                if sTypeAlias != None:
                                    break

                        if sTypeAlias != None:
                            aliasStructDef.sType = sTypeAlias
                        else:
                            Log.f("Could not find sType enum of alias '{0}' of struct '{1}'".format(alias, struct.get('name')))
                else:
                    Log.f("Failed to find alias '{0}' of struct '{1}'".format(alias, struct.get('name')))


    def applyWorkarounds(self):
        # TODO: We currently have to apply workarounds due to "noauto" limittypes and other bugs related to limittypes in the vk.xml
        # These can only be solved permanently if we make modifications to the registry xml itself
        self.structs['VkPhysicalDeviceLimits'].members['bufferImageGranularity'].limittype = 'min' # should be maxalign
        self.structs['VkPhysicalDeviceLimits'].members['subPixelPrecisionBits'].limittype = 'max'
        self.structs['VkPhysicalDeviceLimits'].members['subTexelPrecisionBits'].limittype = 'max'
        self.structs['VkPhysicalDeviceLimits'].members['mipmapPrecisionBits'].limittype = 'max'
        self.structs['VkPhysicalDeviceLimits'].members['viewportSubPixelBits'].limittype = 'max'
        self.structs['VkPhysicalDeviceLimits'].members['minMemoryMapAlignment'].limittype = 'max' # should be minalign
        self.structs['VkPhysicalDeviceLimits'].members['minTexelBufferOffsetAlignment'].limittype = 'min' # should be maxalign
        self.structs['VkPhysicalDeviceLimits'].members['minUniformBufferOffsetAlignment'].limittype = 'min' # should be maxalign
        self.structs['VkPhysicalDeviceLimits'].members['minStorageBufferOffsetAlignment'].limittype = 'min' # should be maxalign
        self.structs['VkPhysicalDeviceLimits'].members['subPixelInterpolationOffsetBits'].limittype = 'max'
        self.structs['VkPhysicalDeviceLimits'].members['timestampPeriod'].limittype = 'IGNORE' # good question what this should be...
        self.structs['VkPhysicalDeviceLimits'].members['nonCoherentAtomSize'].limittype = 'min' # should be maxalign
        self.structs['VkPhysicalDeviceLimits'].members['maxColorAttachments'].limittype = 'max' # vk.xml declares this with 'bitmask' limittype for some reason
        self.structs['VkPhysicalDeviceLimits'].members['pointSizeGranularity'].limittype = 'min' # should be maxmul
        self.structs['VkPhysicalDeviceLimits'].members['lineWidthGranularity'].limittype = 'min' # should be maxmul
        self.structs['VkPhysicalDeviceVulkan11Properties'].members['subgroupSize'].limittype = 'IGNORE' # good question what this should be...
        self.structs['VkPhysicalDevicePortabilitySubsetPropertiesKHR'].members['minVertexInputBindingStrideAlignment'].limittype = 'min' # should be maxalign

        # TODO: There are also some bugs in the vk.xml, like parameters having "bitmask" limittype but actually VkBool32 type
        # This is non-sense, so we patch them
        for structName in self.structs:
            for memberName in self.structs[structName].members:
                memberDef = self.structs[structName].members[memberName]
                if memberDef.limittype == 'bitmask' and memberDef.type == 'VkBool32':
                    self.structs[structName].members[memberName].limittype = 'noauto'


    def getChainableStructDef(self, name, extends):
        structDef = self.structs.get(name)
        if structDef == None:
            Log.f("Structure '{0}' does not exist".format(name))
        if structDef.sType == None:
            Log.f("Structure '{0}' is not chainable".format(name))
        if not extends in structDef.extends + [ name ]:
            Log.f("Structure '{0}' does not extend '{1}'".format(name, extends))
        return structDef


class VulkanProfileCapabilities():
    def __init__(self, registry, data, caps):
        self.extensions = dict()
        self.instanceExtensions = dict()
        self.deviceExtensions = dict()
        self.features = dict()
        self.properties = dict()
        self.formats = dict()
        self.queueFamiliesProperties = []
        self.memoryProperties = dict()
        for capName in data['capabilities']:
            if capName in caps:
                self.mergeCaps(registry, caps[capName])
            else:
                Log.f("Capability '{0}' needed by profile '{1}' is missing".format(capName, data['name']))


    def mergeCaps(self, registry, caps):
        self.mergeProfileExtensions(registry, caps)
        self.mergeProfileFeatures(caps)
        self.mergeProfileProperties(caps)
        self.mergeProfileFormats(caps)
        self.mergeProfileQueueFamiliesProperties(caps)
        self.mergeProfileMemoryProperties(caps)


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
                        Log.f("Data type confict during profile capability data merge (src is '{0}', dst is '{1}')".format(type(val), type(dst[key])))
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


    def mergeProfileMemoryProperties(self, data):
        if data.get('memoryProperties') != None:
            self.mergeProfileCapData(self.memoryProperties, data['memoryProperties'])


class VulkanProfileStructs():
    def __init__(self, registry, caps):
        # Feature struct types
        self.feature = []
        for name in caps.features:
            if name == 'VkPhysicalDeviceFeatures':
                # Special case, as it's wrapped in VkPhysicalDeviceFeatures2
                self.feature.append(registry.structs['VkPhysicalDeviceFeatures2'])
            else:
                self.feature.append(registry.getChainableStructDef(name, 'VkPhysicalDeviceFeatures2'))
        self.eliminateAliases(self.feature)

        # Property struct types
        self.property = []
        for name in caps.properties:
            if name == 'VkPhysicalDeviceProperties':
                # Special case, as it's wrapped in VkPhysicalDeviceProperties2
                self.property.append(registry.structs['VkPhysicalDeviceProperties2'])
            else:
                self.property.append(registry.getChainableStructDef(name, 'VkPhysicalDeviceProperties2'))
        self.eliminateAliases(self.property)

        # Queue family struct types
        self.queueFamily = []
        queueFamilyStructs = dict()
        for queueFamilyProps in caps.queueFamiliesProperties:
            queueFamilyStructs.update(queueFamilyProps)
        for name in queueFamilyStructs:
            if name == 'VkQueueFamilyProperties':
                # Special case, as it's wrapped in VkQueueFamilyProperties2
                self.queueFamily.append(registry.structs['VkQueueFamilyProperties2'])
            else:
                self.queueFamily.append(registry.getChainableStructDef(name, 'VkQueueFamilyProperties2'))
        self.eliminateAliases(self.queueFamily)

        # Format struct types
        self.format = []
        formatStructs = dict()
        for formatProps in caps.formats.values():
            formatStructs.update(formatProps)
        for name in formatStructs:
            if name == 'VkFormatProperties':
                # Special case, as it's wrapped in VkFormatProperties2 or VkFormatProperties3
                self.format.append(registry.structs['VkFormatProperties2'])
                self.format.append(registry.structs['VkFormatProperties3'])
            else:
                self.format.append(registry.getChainableStructDef(name, 'VkFormatProperties2'))
        self.eliminateAliases(self.format)


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
    def __init__(self, registry, name, data, caps):
        self.registry = registry
        self.name = name
        self.version = data['version']
        self.apiVersion = data['api-version']
        self.fallback = data.get('fallback')
        self.requirements = []
        self.capabilities = VulkanProfileCapabilities(registry, data, caps)
        self.structs = VulkanProfileStructs(registry, self.capabilities)
        self.collectCompileTimeRequirements()
        self.validate()


    def collectCompileTimeRequirements(self):
        # Add API version to the list of requirements
        match = re.search(r"^([1-9][0-9]*\.[0-9]+)[^0-9].*$", self.apiVersion)
        if match != None:
            versionNumber = match.group(1)
            if versionNumber in self.registry.versions:
                self.requirements.append(self.registry.versions[versionNumber].name)
            else:
                Log.f("No version '{0}' found in registry required by profile '{1}'".format(versionNumber, self.name))
        else:
            Log.f("Invalid version number '{0}' in profile '{1}'".format(self.apiVersion, self.name))

        # Add any required extension to the list of requirements
        for extName in self.capabilities.extensions:
            if extName in self.registry.extensions:
                self.requirements.append(extName)
            else:
                Log.f("Extension '{0}' required by profile '{1}' does not exist".format(extName, self.name))


    def validate(self):
        self.validateStructDependencies()


    def validateStructDependencies(self):
        for feature in self.capabilities.features:
            self.validateStructDependency(feature)

        for prop in self.capabilities.properties:
            self.validateStructDependency(prop)

        for queueFamilyData in self.capabilities.queueFamiliesProperties:
            for queueFamilyProp in queueFamilyData:
                self.validateStructDependency(queueFamilyProp)

        for memoryProp in self.capabilities.memoryProperties:
            self.validateStructDependency(memoryProp)


    def validateStructDependency(self, structName):
        if structName in self.registry.structs:
            structDef = self.registry.structs[structName]
            depFound = False

            # Check if the required API version defines this struct
            if structDef.definedByVersion != None and structDef.definedByVersion <= self.apiVersion:
                depFound = True

            # Check if any required extension defines this struct
            for definedByExtension in structDef.definedByExtensions:
                if definedByExtension in self.capabilities.extensions:
                    depFound = True
                    break

            if not depFound:
                Log.f("Unexpected required struct '{0}' in profile '{1}'".format(structName, self.name))
        else:
            Log.f("Struct '{0}' in profile '{1}' does not exist in the registry".format(structName, self.name))


    def generatePrivateImpl(self):
        uname = self.name.upper()
        gen = '\n'
        gen += ('#ifdef {0}\n'
                'namespace {1} {{\n').format(self.name, uname)
        gen += self.gen_extensionData('instance')
        gen += self.gen_extensionData('device')
        gen += self.gen_fallbackData()
        gen += self.gen_structTypeData()
        gen += self.gen_structDesc()
        gen += ('\n'
                '}} // namespace {0}\n'
                '#endif\n').format(uname)
        return gen

    def gen_extensionData(self, type):
        foundExt = False
        gen = '\n'
        gen += 'static const VkExtensionProperties _{0}Extensions[] = {{\n'.format(type)
        for extName, specVer in self.capabilities.extensions.items():
            extInfo = self.registry.extensions[extName]
            if extInfo.type == type:
                gen += '    VkExtensionProperties{{ {0}_EXTENSION_NAME, {1} }},\n'.format(extInfo.upperCaseName, specVer)
                foundExt = True
        gen += '};\n'
        return gen if foundExt else ''


    def gen_fallbackData(self):
        gen = ''
        if self.fallback:
            gen += ('\n'
                    'static const VpProfileProperties _fallbacks[] = {\n')
            for fallback in self.fallback:
                gen += '    {{ {0}_NAME, {0}_SPEC_VERSION }},\n'.format(fallback.upper())
            gen += '};\n'
        return gen


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
                        'static const VkStructureType _{0}StructTypes[] = {{\n').format(name)
                for structDef in structDefs:
                    gen += '    {{ {0} }},\n'.format(structDef.sType)
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
        for member, value in values.items():
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
                        # If it's an array we have to generate per-element assignment code
                        for i, v in enumerate(value):
                            gen += fmt.format('{0}{1}[{2}] = {3}'.format(var, member, i, v))
                    else:
                        # For enums and struct initialization, most of the code can be shared
                        genAssign = '{0}{1} = '.format(var, member)
                        isEnum = isinstance(value[0], str)
                        genAssign += '{0}'.format(self.gen_listValue(value, isEnum))
                        gen += fmt.format(genAssign)

                elif type(value) == bool:
                    # Boolean
                    gen += fmt.format('{0}{1} = {2}'.format(var, member, 'VK_TRUE' if value else 'VK_FALSE'))

                else:
                    # Everything else
                    gen += fmt.format('{0}{1} = {2}'.format(var, member, value))
            else:
                Log.f("No member '{0}' in structure '{1}'".format(member, structDef.name))
        return gen


    def gen_structCompare(self, fmt, structDef, var, values):
        gen = ''
        for member, value in values.items():
            if member in structDef.members:
                limittype = structDef.members[member].limittype
                if limittype == 'IGNORE':
                    # Skip this member as we don't know how to validate it
                    continue
                if limittype == 'bitmask':
                    # Compare bitmask by checking if device value contains every bit of profile value
                    comparePredFmt = '(({0} & {1}) == {1})'
                elif limittype == 'max':
                    # Compare max limit by checking if device value is greater than or equal to profile value
                    comparePredFmt = '({0} >= {1})'
                elif limittype == 'min':
                    # Compare min limit by checking if device value is less than or equal to profile value
                    comparePredFmt = '({0} <= {1})'
                elif limittype == 'range':
                    # Compare range limit by checking if device range is larger than or equal to profile range
                    comparePredFmt = [ '({0} <= {1})', '({0} >= {1})' ]
                elif limittype is None or limittype == 'noauto' or limittype == 'struct':
                    # Compare everything else with equality
                    comparePredFmt = '({0} == {1})'
                else:
                    Log.f("Unsupported limittype '{0}' in member '{1}' of structure '{2}'".format(limittype, member, structDef.name))

                if type(value) == dict:
                    # Nested structure
                    memberDef = self.registry.structs.get(structDef.members[member].type)
                    if memberDef != None:
                        gen += self.gen_structCompare(fmt, memberDef, var + member + '.', value)
                    else:
                        Log.f("Member '{0}' in structure '{1}' is not a struct".format(member, structDef.name))

                elif type(value) == list:
                    # Some sort of list (enums or integer/float list for structure initialization)
                    if len(value) == 0:
                        # If list is empty then ignore
                        continue
                    if structDef.members[member].isArray:
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
                    gen += fmt.format(comparePredFmt.format('{0}{1}'.format(var, member), value))
            else:
                Log.f("No member '{0}' in structure '{1}'".format(member, structDef.name))
        return gen


    def gen_structFunc(self, structDefs, caps, func, fmt):
        gen = ''

        hasData = False

        gen += ('            switch (p->sType) {\n')

        for structDef in structDefs:
            paramList = []

            # Fill VkPhysicalDeviceFeatures into VkPhysicalDeviceFeatures2
            if structDef.name == 'VkPhysicalDeviceFeatures2':
                innerCap = caps.get('VkPhysicalDeviceFeatures')
                if innerCap:
                    paramList.append((self.registry.structs['VkPhysicalDeviceFeatures'], 's->features.', innerCap))

            # Fill VkPhysicalDeviceProperties into VkPhysicalDeviceProperties2
            if structDef.name == 'VkPhysicalDeviceProperties2':
                innerCap = caps.get('VkPhysicalDeviceProperties')
                if innerCap:
                    paramList.append((self.registry.structs['VkPhysicalDeviceProperties'], 's->properties.', innerCap))

            # Fill VkQueueFamilyProperties into VkQueueFamilyProperties2
            if structDef.name == 'VkQueueFamilyProperties2':
                innerCap = caps.get('VkQueueFamilyProperties')
                if innerCap:
                    paramList.append((self.registry.structs['VkQueueFamilyProperties'], 's->queueFamilyProperties.', innerCap))

            # Fill VkFormatProperties into VkFormatProperties2
            if structDef.name == 'VkFormatProperties2':
                innerCap = caps.get('VkFormatProperties')
                if innerCap:
                    paramList.append((self.registry.structs['VkFormatProperties'], 's->formatProperties.', innerCap))

            # Fill all other structures directly
            if structDef.name in caps:
                paramList.append((structDef, 's->', caps[structDef.name]))

            if paramList:
                gen += '                case {0}: {{\n'.format(structDef.sType)
                gen += '                    {0}* s = static_cast<{0}*>(static_cast<void*>(p));\n'.format(structDef.name)
                for params in paramList:
                    genAssign = func('                    ' + fmt, params[0], params[1], params[2])
                    if genAssign != '':
                        hasData = True
                        gen += genAssign
                gen += '                } break;\n'

        gen += ('                default: break;\n'
                '            }\n')
        return gen if hasData else ''


    def gen_structDesc(self):
        gen = ''

        fillFmt = '{0};\n'
        cmpFmt = 'ret = ret && {0};\n'

        # Feature descriptor
        gen += ('\n'
                'static const _vpFeatureDesc _featureDesc = {\n'
                '    [](VkBaseOutStructure* p) {\n')
        gen += self.gen_structFunc(self.structs.feature, self.capabilities.features, self.gen_structFill, fillFmt)
        gen += ('    },\n'
                '    [](VkBaseOutStructure* p) -> bool {\n'
                '        bool ret = true;\n')
        gen += self.gen_structFunc(self.structs.feature, self.capabilities.features, self.gen_structCompare, cmpFmt)
        gen += ('        return ret;\n'
                '    }\n'
                '};\n')

        # Property descriptor
        gen += ('\n'
                'static const _vpPropertyDesc _propertyDesc = {\n'
                '    [](VkBaseOutStructure* p) {\n')
        gen += self.gen_structFunc(self.structs.property, self.capabilities.properties, self.gen_structFill, fillFmt)
        gen += ('    },\n'
                '    [](VkBaseOutStructure* p) -> bool {\n'
                '        bool ret = true;\n')
        gen += self.gen_structFunc(self.structs.property, self.capabilities.properties, self.gen_structCompare, cmpFmt)
        gen += ('        return ret;\n'
                '    }\n'
                '};\n')

        # Queue family descriptor
        if self.structs.queueFamily:
            gen += ('\n'
                    'static const _vpQueueFamilyDesc _queueFamilyDesc[] = {\n')
            for queueFamilyCaps in self.capabilities.queueFamiliesProperties:
                gen += ('    {\n'
                        '        [](VkBaseOutStructure* p) {\n')
                gen += self.gen_structFunc(self.structs.queueFamily, queueFamilyCaps, self.gen_structFill, fillFmt)
                gen += ('        },\n'
                        '        [](VkBaseOutStructure* p) -> bool {\n'
                        '            bool ret = true;\n')
                gen += self.gen_structFunc(self.structs.queueFamily, queueFamilyCaps, self.gen_structCompare, cmpFmt)
                gen += ('            return ret;\n'
                        '        }\n'
                        '    },\n')
            gen += ('};\n')

        # Format descriptor
        if self.structs.format:
            gen += ('\n'
                    'static const _vpFormatDesc _formatDesc[] = {\n')
            for formatName, formatCaps in self.capabilities.formats.items():
                gen += ('    {{\n'
                        '        {0},\n'
                        '        [](VkBaseOutStructure* p) {{\n').format(formatName)
                gen += self.gen_structFunc(self.structs.format, formatCaps, self.gen_structFill, fillFmt)
                gen += ('        },\n'
                        '        [](VkBaseOutStructure* p) -> bool {\n'
                        '            bool ret = true;\n')
                gen += self.gen_structFunc(self.structs.format, formatCaps, self.gen_structCompare, cmpFmt)
                gen += ('            return ret;\n'
                        '        }\n'
                        '    },\n')
            gen += '};\n'

        return gen


class VulkanProfiles():
    def loadFromDir(registry, profilesDir):
        profiles = dict()
        dirAbsPath = os.path.abspath(profilesDir)
        filenames = os.listdir(dirAbsPath)
        for filename in filenames:
            fileAbsPath = os.path.join(dirAbsPath, filename)
            if os.path.isfile(fileAbsPath) and os.path.splitext(filename)[-1] == '.json':
                Log.i("Loading profile file: '{0}'".format(filename))
                with open(fileAbsPath, 'r') as f:
                    jsonData = json.load(f)
                    VulkanProfiles.parseProfiles(registry, profiles, jsonData['profiles'], jsonData['capabilities'])
        return profiles


    def parseProfiles(registry, profiles, json, caps):
        for name, data in json.items():
            Log.i("Registering profile '{0}'".format(name))
            profiles[name] = VulkanProfile(registry, name, data, caps)


class VulkanProfilesBuilder():
    def __init__(self, registry, profiles):
        self.registry = registry
        self.profiles = profiles


    def generate(self, outIncDir, outSrcDir):
        self.generate_h(outIncDir)
        self.generate_cpp(outSrcDir)
        self.generate_hpp(outIncDir)


    def generate_h(self, outDir):
        fileAbsPath = os.path.join(os.path.abspath(outDir), 'vulkan_profiles.h')
        Log.i("Generating '{0}'...".format(fileAbsPath))
        with open(fileAbsPath, 'w') as f:
            f.write(COPYRIGHT_HEADER)
            f.write(H_HEADER)
            f.write(self.gen_profileDefs())
            f.write(API_DEFS)
            f.write(H_FOOTER)


    def generate_cpp(self, outDir):
        fileAbsPath = os.path.join(os.path.abspath(outDir), 'vulkan_profiles.cpp')
        Log.i("Generating '{0}'...".format(fileAbsPath))
        with open(fileAbsPath, 'w') as f:
            f.write(COPYRIGHT_HEADER)
            f.write(CPP_HEADER)
            f.write(self.gen_privateImpl())
            f.write(self.gen_publicImpl())


    def generate_hpp(self, outDir):
        fileAbsPath = os.path.join(os.path.abspath(outDir), 'vulkan_profiles.hpp')
        Log.i("Generating '{0}'...".format(fileAbsPath))
        with open(fileAbsPath, 'w') as f:
            f.write(COPYRIGHT_HEADER)
            f.write(HPP_HEADER)
            f.write(self.gen_profileDefs())
            f.write(API_DEFS)
            f.write(self.gen_privateImpl())
            f.write(self.gen_publicImpl())
            f.write(HPP_FOOTER)


    def gen_profileDefs(self):
        gen = ''
        for name, profile in self.profiles.items():
            uname = name.upper()
            gen += '\n'

            # Add prerequisites
            if profile.requirements:
                for i, requirement in enumerate(profile.requirements):
                    if i == 0:
                        gen += '#if '
                    else:
                        gen += '    '

                    gen += 'defined({0})'.format(requirement)

                    if i < len(profile.requirements) - 1:
                        gen += ' && \\\n'
                    else:
                        gen += '\n'

            gen += '#define {0} 1\n'.format(name)
            gen += '#define {0}_NAME "{1}"\n'.format(uname, name)
            gen += '#define {0}_SPEC_VERSION {1}\n'.format(uname, profile.version)
            gen += '#define {0}_MIN_API_VERSION VK_MAKE_VERSION({1})\n'.format(uname, profile.apiVersion.replace(".", ", "))

            if profile.requirements:
                gen += '#endif\n'

        return gen


    def gen_privateImpl(self):
        gen = '\n'
        gen += PRIVATE_IMPL_BODY
        gen += self.gen_profilePrivateImpl()
        gen += self.gen_profileDescTable()
        return gen


    def gen_profilePrivateImpl(self):
        gen = ''
        for profile in self.profiles.values():
            gen += profile.generatePrivateImpl()
        return gen


    def gen_dataArrayInfo(self, condition, name):
        if condition:
            return '        &{0}[0], _vpArraySize({0}),\n'.format(name)
        else:
            return '        nullptr, 0,\n'


    def gen_profileDescTable(self):
        gen = '\n'
        gen += 'static const _vpProfileDesc _vpProfiles[] = {\n'

        for name, profile in self.profiles.items():
            uname = name.upper()
            gen += ('#ifdef {0}\n'
                    '    _vpProfileDesc{{\n'
                    '        VpProfileProperties{{ {1}_NAME, {1}_SPEC_VERSION }},\n'
                    '        {1}_MIN_API_VERSION,\n').format(name, uname)

            gen += self.gen_dataArrayInfo(profile.capabilities.instanceExtensions, '{0}::_instanceExtensions'.format(uname))
            gen += self.gen_dataArrayInfo(profile.capabilities.deviceExtensions, '{0}::_deviceExtensions'.format(uname))
            gen += self.gen_dataArrayInfo(profile.fallback, '{0}::_fallbacks'.format(uname))
            gen += self.gen_dataArrayInfo(profile.structs.feature, '{0}::_featureStructTypes'.format(uname))
            gen += '        {0}::_featureDesc,\n'.format(uname)
            gen += self.gen_dataArrayInfo(profile.structs.property, '{0}::_propertyStructTypes'.format(uname))
            gen += '        {0}::_propertyDesc,\n'.format(uname)
            gen += self.gen_dataArrayInfo(profile.structs.queueFamily, '{0}::_queueFamilyStructTypes'.format(uname))
            gen += self.gen_dataArrayInfo(profile.structs.queueFamily, '{0}::_queueFamilyDesc'.format(uname))
            gen += self.gen_dataArrayInfo(profile.structs.format, '{0}::_formatStructTypes'.format(uname))
            gen += self.gen_dataArrayInfo(profile.structs.format, '{0}::_formatDesc'.format(uname))

            gen += ('    },\n'
                    '#endif\n')

        gen += '};\n'
        return gen


    def gen_publicImpl(self):
        gen = PUBLIC_IMPL_BODY
        gen += self.gen_vpGetPhysicalDeviceProfileSupport()
        gen += self.gen_vpCreateDevice()
        return gen


    def gen_vpGetPhysicalDeviceProfileSupport(self):
        gen = '\n'
        gen += ('VPAPI_ATTR VkResult vpGetPhysicalDeviceProfileSupport(VkPhysicalDevice physicalDevice, const VpProfileProperties *pProfile, VkBool32 *pSupported) {\n'
                '    assert(pProfile != nullptr);\n'
                '    assert(pSupported != nullptr);\n'
                '    assert(physicalDevice != VK_NULL_HANDLE);\n'
                '\n'
                '    VkResult result = VK_SUCCESS;\n'
                '\n'
                '    uint32_t extCount;\n'
                '    result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, nullptr);\n'
                '    if (result != VK_SUCCESS) return result;\n'
                '    std::vector<VkExtensionProperties> ext(extCount);\n'
                '    result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, ext.data());\n'
                '    if (result != VK_SUCCESS) return result;\n'
                '\n'
                '    VkPhysicalDeviceProperties devProps;\n'
                '    vkGetPhysicalDeviceProperties(physicalDevice, &devProps);\n'
                '\n'
                '    *pSupported = VK_FALSE;\n'
                '\n')

        for name, profile in self.profiles.items():
            uname = name.upper()
            gen += ('#ifdef {0}\n'
                    '    if (strcmp(pProfile->profileName, {1}_NAME) == 0) {{\n'
                    '        if ({1}_SPEC_VERSION < pProfile->specVersion) return result;\n'
                    '\n'
                    '        if (VK_VERSION_PATCH(devProps.apiVersion) < VK_VERSION_PATCH({1}_MIN_API_VERSION)) return result;\n').format(name, uname)

            if profile.capabilities.deviceExtensions:
                gen += ('\n'
                        '        for (uint32_t i = 0; i < _vpArraySize({0}::_deviceExtensions); ++i) {{\n'
                        '            if (!_vpCheckExtension(ext.data(), ext.size(), {0}::_deviceExtensions[i].extensionName)) return result;\n'
                        '        }}\n').format(uname)

            # Check features
            features = profile.capabilities.features
            if features:
                pNext = None
                gen += '\n'
                for featureStructName, feature in features.items():
                    structDef = self.registry.structs.get(featureStructName)
                    if structDef == None:
                        Log.f("Feature structure '{0}' does not exist".format(featureStructName))

                    if featureStructName == 'VkPhysicalDeviceFeatures':
                        # Special case, as it's wrapped into VkPhysicalDeviceFeatures2
                        featureStructName = 'VkPhysicalDeviceFeatures2'
                        varAccessSuffix = '.features.'
                        sType = 'VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2'
                    else:
                        varAccessSuffix = '.'
                        sType = structDef.sType

                    varName = featureStructName[2].lower() + featureStructName[3:]
                    gen += '        {0} {1}{{ {2} }};\n'.format(featureStructName, varName, sType)
                    if featureStructName != 'VkPhysicalDeviceFeatures2':
                        if pNext != None:
                            gen += '        {0}.pNext = &{1};\n'.format(varName, pNext)
                        pNext = varName

                if not 'VkPhysicalDeviceFeatures' in features and not 'VkPhysicalDeviceFeatures2' in features:
                    # We have to manually add VkPhysicalDeviceFeatures2 as it's not used in the profile
                    gen += '        VkPhysicalDeviceFeatures2 physicalDeviceFeatures2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };\n'
                if pNext != None:
                    # For vkGetPhysicalDeviceFeatures2 everything has to be chained to VkPhysicalDeviceFeatures2
                    gen += '        physicalDeviceFeatures2.pNext = &{0};\n'.format(pNext)

                gen += '        vkGetPhysicalDeviceFeatures2(physicalDevice, &physicalDeviceFeatures2);\n'
                gen += '        if (!{0}::_featureDesc.pfnComparator(static_cast<VkBaseOutStructure*>(static_cast<void*>(&physicalDeviceFeatures2)))) return result;\n'.format(uname)

            # Check properties
            properties = profile.capabilities.properties
            if properties:
                pNext = None
                gen += '\n'
                for propertyStructName, property in properties.items():
                    structDef = self.registry.structs.get(propertyStructName)
                    if structDef == None:
                        Log.f("Properties structure '{0}' does not exist".format(propertyStructName))

                    if propertyStructName == 'VkPhysicalDeviceProperties':
                        # Special case, as it's wrapped into VkPhysicalDeviceProperties2
                        propertyStructName = 'VkPhysicalDeviceProperties2'
                        varAccessSuffix = '.properties.'
                        sType = 'VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2'
                    else:
                        varAccessSuffix = '.'
                        sType = structDef.sType

                    varName = propertyStructName[2].lower() + propertyStructName[3:]
                    gen += '        {0} {1}{{ {2} }};\n'.format(propertyStructName, varName, sType)
                    if propertyStructName != 'VkPhysicalDeviceProperties2':
                        if pNext != None:
                            gen += '        {0}.pNext = &{1};\n'.format(varName, pNext)
                        pNext = varName

                if not 'VkPhysicalDeviceProperties' in properties and not 'VkPhysicalDeviceProperties2' in properties:
                    # We have to manually add VkPhysicalDeviceProperies2 as it's not used in the profile
                    gen += '        VkPhysicalDeviceProperties2 physicalDeviceProperties2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };\n'
                if pNext != None:
                    # For vkGetPhysicalDeviceProperties2 everything has to be chained to VkPhysicalDeviceProperties2
                    gen += '        physicalDeviceProperties2.pNext = &{0};\n'.format(pNext)

                gen += '        vkGetPhysicalDeviceProperties2(physicalDevice, &physicalDeviceProperties2);\n'
                gen += '        if (!{0}::_propertyDesc.pfnComparator(static_cast<VkBaseOutStructure*>(static_cast<void*>(&physicalDeviceProperties2)))) return result;\n'.format(uname)

            # Check queue family properties
            if profile.capabilities.queueFamiliesProperties:
                # TODO: Complete this once this function is generalized
                pass

            # Check formats
            if profile.capabilities.formats:
                # TODO: Complete this once this function is generalized
                pass

            gen += ('    } else\n'
                    '#endif\n')

        gen += ('    {\n'
                '        return result;\n'
                '    }\n'
                '\n'
                '    *pSupported = VK_TRUE;\n'
                '    return result;\n'
                '}\n')
        return gen


    def gen_vpCreateDevice(self):
        gen = '\n'
        gen += ('VPAPI_ATTR VkResult vpCreateDevice(VkPhysicalDevice physicalDevice, const VpDeviceCreateInfo *pCreateInfo,\n'
                '                                   const VkAllocationCallbacks *pAllocator, VkDevice *pDevice) {\n'
                '    assert(pCreateInfo != nullptr);\n'
                '\n'
                '    if (physicalDevice == VK_NULL_HANDLE || pCreateInfo == nullptr || pDevice == nullptr) {\n'
                '        return vkCreateDevice(physicalDevice, pCreateInfo == nullptr ? nullptr : pCreateInfo->pCreateInfo, pAllocator, pDevice);\n'
                '    } else if (pCreateInfo->pProfile == nullptr || strcmp(pCreateInfo->pProfile->profileName, "") == 0) {\n'
                '        return vkCreateDevice(physicalDevice, pCreateInfo->pCreateInfo, pAllocator, pDevice);\n'
                '    } else\n')

        for name, profile in self.profiles.items():
            uname = name.upper()

            # TODO: Well, this is bogus now, as we add all extensions here even though some may be instance
            # and not device extensions, but we'll keep it as is for now to maintain existing behavior
            gen += ('#ifdef {0}\n'
                    '    if (strcmp(pCreateInfo->pProfile->profileName, {1}_NAME) == 0) {{\n'
                    '        std::vector<const char*> extensions;\n'
                    '        _vpGetDeviceExtensions(pCreateInfo, _vpArraySize({1}::_deviceExtensions), &{1}::_deviceExtensions[0], extensions);\n'
                    '\n'
                    '        void *pNext = const_cast<void*>(pCreateInfo->pCreateInfo->pNext);\n').format(name, uname)

            # Add profile feature structures if they aren't overridden by application
            features = profile.capabilities.features
            if features:
                genDef = ''
                genCheck = ''
                pNext = None
                for featureStructName, feature in features.items():
                    structDef = self.registry.structs.get(featureStructName)
                    if structDef == None:
                        Log.f("Feature structure '{0}' does not exist".format(featureStructName))

                    if featureStructName == 'VkPhysicalDeviceFeatures':
                        # Special case, as it's wrapped into VkPhysicalDeviceFeatures2
                        featureStructName = 'VkPhysicalDeviceFeatures2'
                        sType = 'VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2'
                    else:
                        sType = structDef.sType

                    profileVarName = 'profile' + featureStructName[2:]
                    genDef += '        {0} {1}{{ {2} }};\n'.format(featureStructName, profileVarName, sType)
                    if pNext != None:
                        genDef += '        {0}.pNext = &{1};\n'.format(profileVarName, pNext)
                    pNext = profileVarName

                    if featureStructName == 'VkPhysicalDeviceFeatures2':
                        genCheck += ('        if (pCreateInfo->pCreateInfo->pEnabledFeatures != nullptr) {{\n'
                                     '            profilePhysicalDeviceFeatures2.features = *pCreateInfo->pCreateInfo->pEnabledFeatures;\n'
                                     '        }}\n'
                                     '        if (pCreateInfo->flags & VP_DEVICE_CREATE_DISABLE_ROBUST_BUFFER_ACCESS_BIT) {{\n'
                                     '            profilePhysicalDeviceFeatures2.features.robustBufferAccess = VK_FALSE;\n'
                                     '        }}\n'
                                     '        if (_vpGetStructure(pNext, {0}) == nullptr && pCreateInfo->pCreateInfo->pEnabledFeatures == nullptr) {{\n').format(sType)
                    else:
                        if featureStructName == 'VkPhysicalDeviceVulkan13Features':
                            genCheck += ('        if (pCreateInfo->flags & VP_DEVICE_CREATE_DISABLE_ROBUST_IMAGE_ACCESS_BIT) {\n'
                                         '            profilePhysicalDeviceVulkan13Features.robustImageAccess = VK_FALSE;\n'
                                         '        }\n')
                        genCheck += '        if (_vpGetStructure(pNext, {0}) == nullptr) {{\n'.format(sType)
                    genCheck += ('            {0}.pNext = pNext;\n'
                                 '            pNext = &{0};\n'
                                 '        }}\n').format(profileVarName)

                gen += genDef
                gen += '        vpGetProfileFeatures(pCreateInfo->pProfile, &{0});\n'.format(pNext)
                gen += genCheck

            gen += ('\n'
                    '        VkDeviceCreateInfo deviceCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };\n'
                    '        deviceCreateInfo.pNext = pNext;\n'
                    '        deviceCreateInfo.flags = pCreateInfo->pCreateInfo->flags;\n'
                    '        deviceCreateInfo.queueCreateInfoCount = pCreateInfo->pCreateInfo->queueCreateInfoCount;\n'
                    '        deviceCreateInfo.pQueueCreateInfos = pCreateInfo->pCreateInfo->pQueueCreateInfos;\n'
                    '        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());\n'
                    '        deviceCreateInfo.ppEnabledExtensionNames = extensions.data();\n')

            if not 'VkPhysicalDeviceFeatures' in features and not 'VkPhysicalDeviceFeatures2' in features:
                gen += ('        VkPhysicalDeviceFeatures enabledFeatures;\n'
                        '        if (pCreateInfo->pCreateInfo->pEnabledFeatures != nullptr) {\n'
                        '            enabledFeatures = *pCreateInfo->pCreateInfo->pEnabledFeatures;\n'
                        '            if (pCreateInfo->flags & VP_DEVICE_CREATE_DISABLE_ROBUST_BUFFER_ACCESS_BIT) {\n'
                        '                enabledFeatures.robustBufferAccess = VK_FALSE;\n'
                        '            }\n'
                        '            deviceCreateInfo.pEnabledFeatures = &enabledFeatures;\n'
                        '        }\n')

            gen += ('        return vkCreateDevice(physicalDevice, &deviceCreateInfo, pAllocator, pDevice);\n'
                    '    } else\n'
                    '#endif\n')

        gen += ('    {\n'
                '        return VK_ERROR_UNKNOWN;\n'
                '    }\n'
                '}\n')
        return gen


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    # TODO: Remove temporary default values used for testing purposes only
    parser.add_argument('-registry', action='store',
                        default='./library/scripts/vk.xml',
                        help='Use specified registry file instead of vk.xml')
    parser.add_argument('-profiles', action='store',
                        default='./profiles',
                        help='Generate based on profiles in the specified directory')
    parser.add_argument('-outIncDir', action='store',
                        default='./library/include/vulkan',
                        help='Output include directory')
    parser.add_argument('-outSrcDir', action='store',
                        default='./library/source',
                        help='Output source directory')

    args = parser.parse_args()

    registry = VulkanRegistry(args.registry)
    profiles = VulkanProfiles.loadFromDir(registry, args.profiles)
    builder = VulkanProfilesBuilder(registry, profiles)
    builder.generate(args.outIncDir, args.outSrcDir)
