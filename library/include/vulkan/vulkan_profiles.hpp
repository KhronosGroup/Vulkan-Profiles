/**
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

#if defined(VK_VERSION_1_0) && \
    defined(VK_KHR_get_physical_device_properties2) && \
    defined(VK_KHR_surface) && \
    defined(VK_KHR_get_surface_capabilities2) && \
    defined(VK_KHR_android_surface) && \
    defined(VK_EXT_swapchain_colorspace) && \
    defined(VK_KHR_external_semaphore_capabilities) && \
    defined(VK_KHR_external_memory_capabilities) && \
    defined(VK_KHR_external_fence_capabilities) && \
    defined(VK_KHR_swapchain) && \
    defined(VK_KHR_maintenance1) && \
    defined(VK_KHR_incremental_present) && \
    defined(VK_GOOGLE_display_timing) && \
    defined(VK_KHR_shader_draw_parameters) && \
    defined(VK_KHR_descriptor_update_template) && \
    defined(VK_KHR_get_memory_requirements2) && \
    defined(VK_KHR_dedicated_allocation) && \
    defined(VK_KHR_storage_buffer_storage_class) && \
    defined(VK_KHR_external_semaphore) && \
    defined(VK_KHR_external_memory) && \
    defined(VK_KHR_external_semaphore_fd) && \
    defined(VK_KHR_external_fence) && \
    defined(VK_KHR_external_fence_fd) && \
    defined(VK_KHR_variable_pointers) && \
    defined(VK_KHR_maintenance2)
#define VP_ANDROID_baseline_2021 1
#define VP_ANDROID_BASELINE_2021_NAME "VP_ANDROID_baseline_2021"
#define VP_ANDROID_BASELINE_2021_SPEC_VERSION 1
#define VP_ANDROID_BASELINE_2021_MIN_API_VERSION VK_MAKE_VERSION(1, 0, 68)
#endif

#if defined(VK_VERSION_1_3) && \
    defined(VK_EXT_global_priority)
#define VP_KHR_roadmap_2022 1
#define VP_KHR_ROADMAP_2022_NAME "VP_KHR_roadmap_2022"
#define VP_KHR_ROADMAP_2022_SPEC_VERSION 1
#define VP_KHR_ROADMAP_2022_MIN_API_VERSION VK_MAKE_VERSION(1, 3, 201)
#endif

#if defined(VK_VERSION_1_1) && \
    defined(VK_KHR_8bit_storage) && \
    defined(VK_KHR_create_renderpass2) && \
    defined(VK_KHR_depth_stencil_resolve) && \
    defined(VK_KHR_driver_properties) && \
    defined(VK_KHR_image_format_list) && \
    defined(VK_KHR_imageless_framebuffer) && \
    defined(VK_KHR_sampler_mirror_clamp_to_edge) && \
    defined(VK_KHR_shader_float16_int8) && \
    defined(VK_KHR_shader_subgroup_extended_types) && \
    defined(VK_KHR_timeline_semaphore) && \
    defined(VK_KHR_uniform_buffer_standard_layout) && \
    defined(VK_EXT_descriptor_indexing) && \
    defined(VK_EXT_host_query_reset) && \
    defined(VK_EXT_scalar_block_layout) && \
    defined(VK_KHR_sampler_ycbcr_conversion) && \
    defined(VK_EXT_robustness2) && \
    defined(VK_EXT_subgroup_size_control) && \
    defined(VK_EXT_texel_buffer_alignment) && \
    defined(VK_EXT_vertex_attribute_divisor) && \
    defined(VK_KHR_swapchain) && \
    defined(VK_KHR_swapchain_mutable_format) && \
    defined(VK_KHR_portability_subset)
#define VP_LUNARG_desktop_portability_2021_subset 1
#define VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME "VP_LUNARG_desktop_portability_2021_subset"
#define VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_SPEC_VERSION 1
#define VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_MIN_API_VERSION VK_MAKE_VERSION(1, 1, 142)
#endif

#if defined(VK_VERSION_1_1) && \
    defined(VK_KHR_8bit_storage) && \
    defined(VK_KHR_create_renderpass2) && \
    defined(VK_KHR_depth_stencil_resolve) && \
    defined(VK_KHR_driver_properties) && \
    defined(VK_KHR_image_format_list) && \
    defined(VK_KHR_imageless_framebuffer) && \
    defined(VK_KHR_sampler_mirror_clamp_to_edge) && \
    defined(VK_KHR_shader_float16_int8) && \
    defined(VK_KHR_shader_subgroup_extended_types) && \
    defined(VK_KHR_timeline_semaphore) && \
    defined(VK_KHR_uniform_buffer_standard_layout) && \
    defined(VK_EXT_descriptor_indexing) && \
    defined(VK_EXT_host_query_reset) && \
    defined(VK_EXT_scalar_block_layout) && \
    defined(VK_KHR_sampler_ycbcr_conversion) && \
    defined(VK_EXT_robustness2) && \
    defined(VK_EXT_subgroup_size_control) && \
    defined(VK_EXT_texel_buffer_alignment) && \
    defined(VK_EXT_vertex_attribute_divisor) && \
    defined(VK_KHR_swapchain) && \
    defined(VK_KHR_swapchain_mutable_format)
#define VP_LUNARG_desktop_portability_2021 1
#define VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME "VP_LUNARG_desktop_portability_2021"
#define VP_LUNARG_DESKTOP_PORTABILITY_2021_SPEC_VERSION 1
#define VP_LUNARG_DESKTOP_PORTABILITY_2021_MIN_API_VERSION VK_MAKE_VERSION(1, 1, 142)
#endif

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

#ifdef VP_ANDROID_baseline_2021
namespace VP_ANDROID_BASELINE_2021 {

static const VkExtensionProperties _instanceExtensions[] = {
    VkExtensionProperties{ VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_SURFACE_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_ANDROID_SURFACE_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME, 1 },
};

static const VkExtensionProperties _deviceExtensions[] = {
    VkExtensionProperties{ VK_KHR_SWAPCHAIN_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_MAINTENANCE_1_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_INCREMENTAL_PRESENT_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_GOOGLE_DISPLAY_TIMING_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_EXTERNAL_SEMAPHORE_FD_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_EXTERNAL_FENCE_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_EXTERNAL_FENCE_FD_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_MAINTENANCE_2_EXTENSION_NAME, 1 },
};

static const VkStructureType _featureStructTypes[] = {
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 },
};

static const VkStructureType _propertyStructTypes[] = {
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 },
};

static const VkStructureType _formatStructTypes[] = {
    { VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2 },
    { VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3 },
};

static const _vpFeatureDesc _featureDesc = {
    [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2: {
                    VkPhysicalDeviceFeatures2* s = reinterpret_cast<VkPhysicalDeviceFeatures2*>(p);
                    s->features.robustBufferAccess = VK_TRUE;
                    s->features.depthBiasClamp = VK_TRUE;
                    s->features.fragmentStoresAndAtomics = VK_TRUE;
                    s->features.fullDrawIndexUint32 = VK_TRUE;
                    s->features.imageCubeArray = VK_TRUE;
                    s->features.independentBlend = VK_TRUE;
                    s->features.largePoints = VK_TRUE;
                    s->features.shaderImageGatherExtended = VK_TRUE;
                    s->features.shaderInt16 = VK_TRUE;
                    s->features.shaderSampledImageArrayDynamicIndexing = VK_TRUE;
                    s->features.shaderStorageBufferArrayDynamicIndexing = VK_TRUE;
                    s->features.shaderStorageImageArrayDynamicIndexing = VK_TRUE;
                    s->features.shaderStorageImageWriteWithoutFormat = VK_TRUE;
                    s->features.shaderUniformBufferArrayDynamicIndexing = VK_TRUE;
                    s->features.textureCompressionASTC_LDR = VK_TRUE;
                    s->features.textureCompressionETC2 = VK_TRUE;
                } break;
                default: break;
            }
    },
    [](VkBaseOutStructure* p) -> bool {
        bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2: {
                    VkPhysicalDeviceFeatures2* s = reinterpret_cast<VkPhysicalDeviceFeatures2*>(p);
                    ret = ret && (s->features.robustBufferAccess == VK_TRUE);
                    ret = ret && (s->features.depthBiasClamp == VK_TRUE);
                    ret = ret && (s->features.fragmentStoresAndAtomics == VK_TRUE);
                    ret = ret && (s->features.fullDrawIndexUint32 == VK_TRUE);
                    ret = ret && (s->features.imageCubeArray == VK_TRUE);
                    ret = ret && (s->features.independentBlend == VK_TRUE);
                    ret = ret && (s->features.largePoints == VK_TRUE);
                    ret = ret && (s->features.shaderImageGatherExtended == VK_TRUE);
                    ret = ret && (s->features.shaderInt16 == VK_TRUE);
                    ret = ret && (s->features.shaderSampledImageArrayDynamicIndexing == VK_TRUE);
                    ret = ret && (s->features.shaderStorageBufferArrayDynamicIndexing == VK_TRUE);
                    ret = ret && (s->features.shaderStorageImageArrayDynamicIndexing == VK_TRUE);
                    ret = ret && (s->features.shaderStorageImageWriteWithoutFormat == VK_TRUE);
                    ret = ret && (s->features.shaderUniformBufferArrayDynamicIndexing == VK_TRUE);
                    ret = ret && (s->features.textureCompressionASTC_LDR == VK_TRUE);
                    ret = ret && (s->features.textureCompressionETC2 == VK_TRUE);
                } break;
                default: break;
            }
        return ret;
    }
};

static const _vpPropertyDesc _propertyDesc = {
    [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2: {
                    VkPhysicalDeviceProperties2* s = reinterpret_cast<VkPhysicalDeviceProperties2*>(p);
                    s->properties.limits.maxImageDimension1D = 16384;
                    s->properties.limits.maxImageDimension2D = 8192;
                    s->properties.limits.maxImageDimension3D = 4096;
                    s->properties.limits.maxImageDimensionCube = 8192;
                    s->properties.limits.maxImageArrayLayers = 256;
                    s->properties.limits.maxTexelBufferElements = 65536;
                    s->properties.limits.maxUniformBufferRange = 65536;
                    s->properties.limits.maxStorageBufferRange = 268435456;
                    s->properties.limits.maxPushConstantsSize = 128;
                    s->properties.limits.maxMemoryAllocationCount = 4294967295;
                    s->properties.limits.maxSamplerAllocationCount = 16384;
                    s->properties.limits.bufferImageGranularity = 4096;
                    s->properties.limits.maxBoundDescriptorSets = 4;
                    s->properties.limits.maxPerStageDescriptorSamplers = 128;
                    s->properties.limits.maxPerStageDescriptorUniformBuffers = 12;
                    s->properties.limits.maxPerStageDescriptorStorageBuffers = 35;
                    s->properties.limits.maxPerStageDescriptorSampledImages = 16;
                    s->properties.limits.maxPerStageDescriptorStorageImages = 8;
                    s->properties.limits.maxPerStageDescriptorInputAttachments = 4;
                    s->properties.limits.maxPerStageResources = 79;
                    s->properties.limits.maxDescriptorSetSamplers = 768;
                    s->properties.limits.maxDescriptorSetUniformBuffers = 72;
                    s->properties.limits.maxDescriptorSetUniformBuffersDynamic = 8;
                    s->properties.limits.maxDescriptorSetStorageBuffers = 72;
                    s->properties.limits.maxDescriptorSetStorageBuffersDynamic = 4;
                    s->properties.limits.maxDescriptorSetSampledImages = 96;
                    s->properties.limits.maxDescriptorSetStorageImages = 48;
                    s->properties.limits.maxDescriptorSetInputAttachments = 4;
                    s->properties.limits.maxVertexInputAttributes = 16;
                    s->properties.limits.maxVertexInputBindings = 16;
                    s->properties.limits.maxVertexInputAttributeOffset = 2047;
                    s->properties.limits.maxVertexInputBindingStride = 2048;
                    s->properties.limits.maxVertexOutputComponents = 128;
                    s->properties.limits.maxTessellationGenerationLevel = 64;
                    s->properties.limits.maxTessellationPatchSize = 32;
                    s->properties.limits.maxTessellationControlPerVertexInputComponents = 128;
                    s->properties.limits.maxTessellationControlPerVertexOutputComponents = 128;
                    s->properties.limits.maxTessellationControlPerPatchOutputComponents = 120;
                    s->properties.limits.maxTessellationControlTotalOutputComponents = 4096;
                    s->properties.limits.maxTessellationEvaluationInputComponents = 128;
                    s->properties.limits.maxTessellationEvaluationOutputComponents = 128;
                    s->properties.limits.maxFragmentInputComponents = 128;
                    s->properties.limits.maxFragmentOutputAttachments = 4;
                    s->properties.limits.maxFragmentDualSrcAttachments = 0;
                    s->properties.limits.maxFragmentCombinedOutputResources = 47;
                    s->properties.limits.maxComputeSharedMemorySize = 32768;
                    s->properties.limits.maxComputeWorkGroupCount[0] = 65535;
                    s->properties.limits.maxComputeWorkGroupCount[1] = 65535;
                    s->properties.limits.maxComputeWorkGroupCount[2] = 65535;
                    s->properties.limits.maxComputeWorkGroupInvocations = 128;
                    s->properties.limits.maxComputeWorkGroupSize[0] = 128;
                    s->properties.limits.maxComputeWorkGroupSize[1] = 128;
                    s->properties.limits.maxComputeWorkGroupSize[2] = 128;
                    s->properties.limits.subPixelPrecisionBits = 8;
                    s->properties.limits.subTexelPrecisionBits = 8;
                    s->properties.limits.mipmapPrecisionBits = 8;
                    s->properties.limits.maxDrawIndexedIndexValue = 4294967295;
                    s->properties.limits.maxDrawIndirectCount = 1;
                    s->properties.limits.maxSamplerLodBias = 15.9961;
                    s->properties.limits.maxSamplerAnisotropy = 1.0;
                    s->properties.limits.maxViewports = 1;
                    s->properties.limits.maxViewportDimensions[0] = 8192;
                    s->properties.limits.maxViewportDimensions[1] = 8192;
                    s->properties.limits.viewportBoundsRange[0] = -32768;
                    s->properties.limits.viewportBoundsRange[1] = 32767;
                    s->properties.limits.viewportSubPixelBits = 0;
                    s->properties.limits.minMemoryMapAlignment = 64;
                    s->properties.limits.minTexelBufferOffsetAlignment = 256;
                    s->properties.limits.minUniformBufferOffsetAlignment = 64;
                    s->properties.limits.minStorageBufferOffsetAlignment = 256;
                    s->properties.limits.minTexelOffset = -8;
                    s->properties.limits.maxTexelOffset = 7;
                    s->properties.limits.minTexelGatherOffset = -8;
                    s->properties.limits.maxTexelGatherOffset = 7;
                    s->properties.limits.minInterpolationOffset = -0.5;
                    s->properties.limits.maxInterpolationOffset = 0.5;
                    s->properties.limits.subPixelInterpolationOffsetBits = 4;
                    s->properties.limits.maxFramebufferWidth = 8192;
                    s->properties.limits.maxFramebufferHeight = 8192;
                    s->properties.limits.maxFramebufferLayers = 256;
                    s->properties.limits.framebufferColorSampleCounts = (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT);
                    s->properties.limits.framebufferDepthSampleCounts = (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT);
                    s->properties.limits.framebufferStencilSampleCounts = (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT);
                    s->properties.limits.framebufferNoAttachmentsSampleCounts = (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT);
                    s->properties.limits.maxColorAttachments = 4;
                    s->properties.limits.sampledImageColorSampleCounts = (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT);
                    s->properties.limits.sampledImageIntegerSampleCounts = (VK_SAMPLE_COUNT_1_BIT);
                    s->properties.limits.sampledImageDepthSampleCounts = (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT);
                    s->properties.limits.sampledImageStencilSampleCounts = (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT);
                    s->properties.limits.storageImageSampleCounts = (VK_SAMPLE_COUNT_1_BIT);
                    s->properties.limits.maxSampleMaskWords = 1;
                    s->properties.limits.timestampComputeAndGraphics = VK_TRUE;
                    s->properties.limits.timestampPeriod = 38.4615;
                    s->properties.limits.maxClipDistances = 0;
                    s->properties.limits.maxCullDistances = 0;
                    s->properties.limits.maxCombinedClipAndCullDistances = 0;
                    s->properties.limits.discreteQueuePriorities = 2;
                    s->properties.limits.pointSizeRange[0] = 1.0;
                    s->properties.limits.pointSizeRange[1] = 1024.0;
                    s->properties.limits.lineWidthRange[0] = 1.0;
                    s->properties.limits.lineWidthRange[1] = 1.0;
                    s->properties.limits.pointSizeGranularity = 0.0625;
                    s->properties.limits.strictLines = VK_TRUE;
                    s->properties.limits.nonCoherentAtomSize = 64;
                } break;
                default: break;
            }
    },
    [](VkBaseOutStructure* p) -> bool {
        bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2: {
                    VkPhysicalDeviceProperties2* s = reinterpret_cast<VkPhysicalDeviceProperties2*>(p);
                    ret = ret && (s->properties.limits.maxImageDimension1D >= 16384);
                    ret = ret && (s->properties.limits.maxImageDimension2D >= 8192);
                    ret = ret && (s->properties.limits.maxImageDimension3D >= 4096);
                    ret = ret && (s->properties.limits.maxImageDimensionCube >= 8192);
                    ret = ret && (s->properties.limits.maxImageArrayLayers >= 256);
                    ret = ret && (s->properties.limits.maxTexelBufferElements >= 65536);
                    ret = ret && (s->properties.limits.maxUniformBufferRange >= 65536);
                    ret = ret && (s->properties.limits.maxStorageBufferRange >= 268435456);
                    ret = ret && (s->properties.limits.maxPushConstantsSize >= 128);
                    ret = ret && (s->properties.limits.maxMemoryAllocationCount >= 4294967295);
                    ret = ret && (s->properties.limits.maxSamplerAllocationCount >= 16384);
                    ret = ret && (s->properties.limits.bufferImageGranularity <= 4096);
                    ret = ret && (s->properties.limits.maxBoundDescriptorSets >= 4);
                    ret = ret && (s->properties.limits.maxPerStageDescriptorSamplers >= 128);
                    ret = ret && (s->properties.limits.maxPerStageDescriptorUniformBuffers >= 12);
                    ret = ret && (s->properties.limits.maxPerStageDescriptorStorageBuffers >= 35);
                    ret = ret && (s->properties.limits.maxPerStageDescriptorSampledImages >= 16);
                    ret = ret && (s->properties.limits.maxPerStageDescriptorStorageImages >= 8);
                    ret = ret && (s->properties.limits.maxPerStageDescriptorInputAttachments >= 4);
                    ret = ret && (s->properties.limits.maxPerStageResources >= 79);
                    ret = ret && (s->properties.limits.maxDescriptorSetSamplers >= 768);
                    ret = ret && (s->properties.limits.maxDescriptorSetUniformBuffers >= 72);
                    ret = ret && (s->properties.limits.maxDescriptorSetUniformBuffersDynamic >= 8);
                    ret = ret && (s->properties.limits.maxDescriptorSetStorageBuffers >= 72);
                    ret = ret && (s->properties.limits.maxDescriptorSetStorageBuffersDynamic >= 4);
                    ret = ret && (s->properties.limits.maxDescriptorSetSampledImages >= 96);
                    ret = ret && (s->properties.limits.maxDescriptorSetStorageImages >= 48);
                    ret = ret && (s->properties.limits.maxDescriptorSetInputAttachments >= 4);
                    ret = ret && (s->properties.limits.maxVertexInputAttributes >= 16);
                    ret = ret && (s->properties.limits.maxVertexInputBindings >= 16);
                    ret = ret && (s->properties.limits.maxVertexInputAttributeOffset >= 2047);
                    ret = ret && (s->properties.limits.maxVertexInputBindingStride >= 2048);
                    ret = ret && (s->properties.limits.maxVertexOutputComponents >= 128);
                    ret = ret && (s->properties.limits.maxTessellationGenerationLevel >= 64);
                    ret = ret && (s->properties.limits.maxTessellationPatchSize >= 32);
                    ret = ret && (s->properties.limits.maxTessellationControlPerVertexInputComponents >= 128);
                    ret = ret && (s->properties.limits.maxTessellationControlPerVertexOutputComponents >= 128);
                    ret = ret && (s->properties.limits.maxTessellationControlPerPatchOutputComponents >= 120);
                    ret = ret && (s->properties.limits.maxTessellationControlTotalOutputComponents >= 4096);
                    ret = ret && (s->properties.limits.maxTessellationEvaluationInputComponents >= 128);
                    ret = ret && (s->properties.limits.maxTessellationEvaluationOutputComponents >= 128);
                    ret = ret && (s->properties.limits.maxFragmentInputComponents >= 128);
                    ret = ret && (s->properties.limits.maxFragmentOutputAttachments >= 4);
                    ret = ret && (s->properties.limits.maxFragmentDualSrcAttachments >= 0);
                    ret = ret && (s->properties.limits.maxFragmentCombinedOutputResources >= 47);
                    ret = ret && (s->properties.limits.maxComputeSharedMemorySize >= 32768);
                    ret = ret && (s->properties.limits.maxComputeWorkGroupCount[0] >= 65535);
                    ret = ret && (s->properties.limits.maxComputeWorkGroupCount[1] >= 65535);
                    ret = ret && (s->properties.limits.maxComputeWorkGroupCount[2] >= 65535);
                    ret = ret && (s->properties.limits.maxComputeWorkGroupInvocations >= 128);
                    ret = ret && (s->properties.limits.maxComputeWorkGroupSize[0] >= 128);
                    ret = ret && (s->properties.limits.maxComputeWorkGroupSize[1] >= 128);
                    ret = ret && (s->properties.limits.maxComputeWorkGroupSize[2] >= 128);
                    ret = ret && (s->properties.limits.subPixelPrecisionBits >= 8);
                    ret = ret && (s->properties.limits.subTexelPrecisionBits >= 8);
                    ret = ret && (s->properties.limits.mipmapPrecisionBits >= 8);
                    ret = ret && (s->properties.limits.maxDrawIndexedIndexValue >= 4294967295);
                    ret = ret && (s->properties.limits.maxDrawIndirectCount >= 1);
                    ret = ret && (s->properties.limits.maxSamplerLodBias >= 15.9961);
                    ret = ret && (s->properties.limits.maxSamplerAnisotropy >= 1.0);
                    ret = ret && (s->properties.limits.maxViewports >= 1);
                    ret = ret && (s->properties.limits.maxViewportDimensions[0] >= 8192);
                    ret = ret && (s->properties.limits.maxViewportDimensions[1] >= 8192);
                    ret = ret && (s->properties.limits.viewportBoundsRange[0] <= -32768);
                    ret = ret && (s->properties.limits.viewportBoundsRange[1] >= 32767);
                    ret = ret && (s->properties.limits.viewportSubPixelBits >= 0);
                    ret = ret && (s->properties.limits.minMemoryMapAlignment >= 64);
                    ret = ret && (s->properties.limits.minTexelBufferOffsetAlignment <= 256);
                    ret = ret && (s->properties.limits.minUniformBufferOffsetAlignment <= 64);
                    ret = ret && (s->properties.limits.minStorageBufferOffsetAlignment <= 256);
                    ret = ret && (s->properties.limits.minTexelOffset <= -8);
                    ret = ret && (s->properties.limits.maxTexelOffset >= 7);
                    ret = ret && (s->properties.limits.minTexelGatherOffset <= -8);
                    ret = ret && (s->properties.limits.maxTexelGatherOffset >= 7);
                    ret = ret && (s->properties.limits.minInterpolationOffset <= -0.5);
                    ret = ret && (s->properties.limits.maxInterpolationOffset >= 0.5);
                    ret = ret && (s->properties.limits.subPixelInterpolationOffsetBits >= 4);
                    ret = ret && (s->properties.limits.maxFramebufferWidth >= 8192);
                    ret = ret && (s->properties.limits.maxFramebufferHeight >= 8192);
                    ret = ret && (s->properties.limits.maxFramebufferLayers >= 256);
                    ret = ret && ((s->properties.limits.framebufferColorSampleCounts & (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT)) == (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT));
                    ret = ret && ((s->properties.limits.framebufferDepthSampleCounts & (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT)) == (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT));
                    ret = ret && ((s->properties.limits.framebufferStencilSampleCounts & (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT)) == (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT));
                    ret = ret && ((s->properties.limits.framebufferNoAttachmentsSampleCounts & (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT)) == (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT));
                    ret = ret && (s->properties.limits.maxColorAttachments >= 4);
                    ret = ret && ((s->properties.limits.sampledImageColorSampleCounts & (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT)) == (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT));
                    ret = ret && ((s->properties.limits.sampledImageIntegerSampleCounts & (VK_SAMPLE_COUNT_1_BIT)) == (VK_SAMPLE_COUNT_1_BIT));
                    ret = ret && ((s->properties.limits.sampledImageDepthSampleCounts & (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT)) == (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT));
                    ret = ret && ((s->properties.limits.sampledImageStencilSampleCounts & (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT)) == (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT));
                    ret = ret && ((s->properties.limits.storageImageSampleCounts & (VK_SAMPLE_COUNT_1_BIT)) == (VK_SAMPLE_COUNT_1_BIT));
                    ret = ret && (s->properties.limits.maxSampleMaskWords >= 1);
                    ret = ret && (s->properties.limits.timestampComputeAndGraphics == VK_TRUE);
                    ret = ret && (s->properties.limits.maxClipDistances >= 0);
                    ret = ret && (s->properties.limits.maxCullDistances >= 0);
                    ret = ret && (s->properties.limits.maxCombinedClipAndCullDistances >= 0);
                    ret = ret && (s->properties.limits.discreteQueuePriorities >= 2);
                    ret = ret && (s->properties.limits.pointSizeRange[0] <= 1.0);
                    ret = ret && (s->properties.limits.pointSizeRange[1] >= 1024.0);
                    ret = ret && (s->properties.limits.lineWidthRange[0] <= 1.0);
                    ret = ret && (s->properties.limits.lineWidthRange[1] >= 1.0);
                    ret = ret && (s->properties.limits.pointSizeGranularity <= 0.0625);
                    ret = ret && (s->properties.limits.strictLines == VK_TRUE);
                    ret = ret && (s->properties.limits.nonCoherentAtomSize <= 64);
                } break;
                default: break;
            }
        return ret;
    }
};

static const _vpFormatDesc _formatDesc[] = {
    {
        VK_FORMAT_R8G8B8_UNORM,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_X8_D24_UNORM_PACK32,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_4x4_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_4x4_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_5x4_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_5x4_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_5x5_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_5x5_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_6x5_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_6x5_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_6x6_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_6x6_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_8x5_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_8x5_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_8x6_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_8x6_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_8x8_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_8x8_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_10x5_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_10x5_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_10x6_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_10x6_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_10x8_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_10x8_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_10x10_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_10x10_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_12x10_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_12x10_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_12x12_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ASTC_12x12_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_D24_UNORM_S8_UINT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_B4G4R4A4_UNORM_PACK16,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R5G6B5_UNORM_PACK16,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_A1R5G5B5_UNORM_PACK16,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R8_UNORM,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R8_SNORM,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R8_UINT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R8_SINT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8_UNORM,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8_SNORM,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8_UINT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8_SINT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8B8A8_UNORM,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8B8A8_SNORM,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8B8A8_UINT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8B8A8_SINT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8B8A8_SRGB,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_B8G8R8A8_UNORM,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_B8G8R8A8_SRGB,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_A8B8G8R8_UNORM_PACK32,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_A8B8G8R8_SNORM_PACK32,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_A8B8G8R8_UINT_PACK32,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_A8B8G8R8_SINT_PACK32,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_A8B8G8R8_SRGB_PACK32,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_A2B10G10R10_UNORM_PACK32,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_A2B10G10R10_UINT_PACK32,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R16_UNORM,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R16_SNORM,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R16_UINT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R16_SINT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R16_SFLOAT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16_UNORM,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16_SNORM,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16_UINT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16_SINT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16_SFLOAT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16B16A16_UNORM,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16B16A16_SNORM,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16B16A16_UINT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16B16A16_SINT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16B16A16_SFLOAT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R32_UINT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R32_SINT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R32_SFLOAT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R32G32_UINT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R32G32_SINT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R32G32_SFLOAT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R32G32B32_UINT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R32G32B32_SINT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R32G32B32_SFLOAT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R32G32B32A32_UINT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R32G32B32A32_SINT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_R32G32B32A32_SFLOAT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_B10G11R11_UFLOAT_PACK32,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_D16_UNORM,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_D32_SFLOAT,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_EAC_R11_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_EAC_R11_SNORM_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_EAC_R11G11_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
    {
        VK_FORMAT_EAC_R11G11_SNORM_BLOCK,
        [](VkBaseOutStructure* p) {
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            return ret;
        }
    },
};

} // namespace VP_ANDROID_BASELINE_2021
#endif

#ifdef VP_KHR_roadmap_2022
namespace VP_KHR_ROADMAP_2022 {

static const VkExtensionProperties _deviceExtensions[] = {
    VkExtensionProperties{ VK_EXT_GLOBAL_PRIORITY_EXTENSION_NAME, 1 },
};

static const VkStructureType _featureStructTypes[] = {
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES },
};

static const VkStructureType _propertyStructTypes[] = {
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 },
};

static const _vpFeatureDesc _featureDesc = {
    [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2: {
                    VkPhysicalDeviceFeatures2* s = reinterpret_cast<VkPhysicalDeviceFeatures2*>(p);
                    s->features.robustBufferAccess = VK_TRUE;
                    s->features.fullDrawIndexUint32 = VK_TRUE;
                    s->features.imageCubeArray = VK_TRUE;
                    s->features.independentBlend = VK_TRUE;
                    s->features.sampleRateShading = VK_TRUE;
                    s->features.drawIndirectFirstInstance = VK_TRUE;
                    s->features.depthClamp = VK_TRUE;
                    s->features.depthBiasClamp = VK_TRUE;
                    s->features.samplerAnisotropy = VK_TRUE;
                    s->features.occlusionQueryPrecise = VK_TRUE;
                    s->features.fragmentStoresAndAtomics = VK_TRUE;
                    s->features.shaderStorageImageExtendedFormats = VK_TRUE;
                    s->features.shaderUniformBufferArrayDynamicIndexing = VK_TRUE;
                    s->features.shaderSampledImageArrayDynamicIndexing = VK_TRUE;
                    s->features.shaderStorageBufferArrayDynamicIndexing = VK_TRUE;
                    s->features.shaderStorageImageArrayDynamicIndexing = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES: {
                    VkPhysicalDeviceVulkan11Features* s = reinterpret_cast<VkPhysicalDeviceVulkan11Features*>(p);
                    s->multiview = VK_TRUE;
                    s->samplerYcbcrConversion = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES: {
                    VkPhysicalDeviceVulkan12Features* s = reinterpret_cast<VkPhysicalDeviceVulkan12Features*>(p);
                    s->uniformBufferStandardLayout = VK_TRUE;
                    s->subgroupBroadcastDynamicId = VK_TRUE;
                    s->imagelessFramebuffer = VK_TRUE;
                    s->separateDepthStencilLayouts = VK_TRUE;
                    s->hostQueryReset = VK_TRUE;
                    s->timelineSemaphore = VK_TRUE;
                    s->shaderSubgroupExtendedTypes = VK_TRUE;
                    s->vulkanMemoryModel = VK_TRUE;
                    s->vulkanMemoryModelDeviceScope = VK_TRUE;
                    s->vulkanMemoryModelAvailabilityVisibilityChains = VK_TRUE;
                    s->bufferDeviceAddress = VK_TRUE;
                    s->samplerMirrorClampToEdge = VK_TRUE;
                    s->descriptorIndexing = VK_TRUE;
                    s->shaderUniformTexelBufferArrayDynamicIndexing = VK_TRUE;
                    s->shaderStorageTexelBufferArrayDynamicIndexing = VK_TRUE;
                    s->shaderUniformBufferArrayNonUniformIndexing = VK_TRUE;
                    s->shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
                    s->shaderStorageBufferArrayNonUniformIndexing = VK_TRUE;
                    s->shaderStorageImageArrayNonUniformIndexing = VK_TRUE;
                    s->shaderUniformTexelBufferArrayNonUniformIndexing = VK_TRUE;
                    s->shaderStorageTexelBufferArrayNonUniformIndexing = VK_TRUE;
                    s->descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE;
                    s->descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
                    s->descriptorBindingStorageImageUpdateAfterBind = VK_TRUE;
                    s->descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE;
                    s->descriptorBindingUniformTexelBufferUpdateAfterBind = VK_TRUE;
                    s->descriptorBindingStorageTexelBufferUpdateAfterBind = VK_TRUE;
                    s->descriptorBindingUpdateUnusedWhilePending = VK_TRUE;
                    s->descriptorBindingPartiallyBound = VK_TRUE;
                    s->descriptorBindingVariableDescriptorCount = VK_TRUE;
                    s->runtimeDescriptorArray = VK_TRUE;
                    s->scalarBlockLayout = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES: {
                    VkPhysicalDeviceVulkan13Features* s = reinterpret_cast<VkPhysicalDeviceVulkan13Features*>(p);
                    s->robustImageAccess = VK_TRUE;
                    s->shaderTerminateInvocation = VK_TRUE;
                    s->shaderZeroInitializeWorkgroupMemory = VK_TRUE;
                    s->synchronization2 = VK_TRUE;
                    s->shaderIntegerDotProduct = VK_TRUE;
                    s->maintenance4 = VK_TRUE;
                    s->pipelineCreationCacheControl = VK_TRUE;
                    s->subgroupSizeControl = VK_TRUE;
                    s->computeFullSubgroups = VK_TRUE;
                    s->shaderDemoteToHelperInvocation = VK_TRUE;
                    s->inlineUniformBlock = VK_TRUE;
                    s->descriptorBindingInlineUniformBlockUpdateAfterBind = VK_TRUE;
                } break;
                default: break;
            }
    },
    [](VkBaseOutStructure* p) -> bool {
        bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2: {
                    VkPhysicalDeviceFeatures2* s = reinterpret_cast<VkPhysicalDeviceFeatures2*>(p);
                    ret = ret && (s->features.robustBufferAccess == VK_TRUE);
                    ret = ret && (s->features.fullDrawIndexUint32 == VK_TRUE);
                    ret = ret && (s->features.imageCubeArray == VK_TRUE);
                    ret = ret && (s->features.independentBlend == VK_TRUE);
                    ret = ret && (s->features.sampleRateShading == VK_TRUE);
                    ret = ret && (s->features.drawIndirectFirstInstance == VK_TRUE);
                    ret = ret && (s->features.depthClamp == VK_TRUE);
                    ret = ret && (s->features.depthBiasClamp == VK_TRUE);
                    ret = ret && (s->features.samplerAnisotropy == VK_TRUE);
                    ret = ret && (s->features.occlusionQueryPrecise == VK_TRUE);
                    ret = ret && (s->features.fragmentStoresAndAtomics == VK_TRUE);
                    ret = ret && (s->features.shaderStorageImageExtendedFormats == VK_TRUE);
                    ret = ret && (s->features.shaderUniformBufferArrayDynamicIndexing == VK_TRUE);
                    ret = ret && (s->features.shaderSampledImageArrayDynamicIndexing == VK_TRUE);
                    ret = ret && (s->features.shaderStorageBufferArrayDynamicIndexing == VK_TRUE);
                    ret = ret && (s->features.shaderStorageImageArrayDynamicIndexing == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES: {
                    VkPhysicalDeviceVulkan11Features* s = reinterpret_cast<VkPhysicalDeviceVulkan11Features*>(p);
                    ret = ret && (s->multiview == VK_TRUE);
                    ret = ret && (s->samplerYcbcrConversion == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES: {
                    VkPhysicalDeviceVulkan12Features* s = reinterpret_cast<VkPhysicalDeviceVulkan12Features*>(p);
                    ret = ret && (s->uniformBufferStandardLayout == VK_TRUE);
                    ret = ret && (s->subgroupBroadcastDynamicId == VK_TRUE);
                    ret = ret && (s->imagelessFramebuffer == VK_TRUE);
                    ret = ret && (s->separateDepthStencilLayouts == VK_TRUE);
                    ret = ret && (s->hostQueryReset == VK_TRUE);
                    ret = ret && (s->timelineSemaphore == VK_TRUE);
                    ret = ret && (s->shaderSubgroupExtendedTypes == VK_TRUE);
                    ret = ret && (s->vulkanMemoryModel == VK_TRUE);
                    ret = ret && (s->vulkanMemoryModelDeviceScope == VK_TRUE);
                    ret = ret && (s->vulkanMemoryModelAvailabilityVisibilityChains == VK_TRUE);
                    ret = ret && (s->bufferDeviceAddress == VK_TRUE);
                    ret = ret && (s->samplerMirrorClampToEdge == VK_TRUE);
                    ret = ret && (s->descriptorIndexing == VK_TRUE);
                    ret = ret && (s->shaderUniformTexelBufferArrayDynamicIndexing == VK_TRUE);
                    ret = ret && (s->shaderStorageTexelBufferArrayDynamicIndexing == VK_TRUE);
                    ret = ret && (s->shaderUniformBufferArrayNonUniformIndexing == VK_TRUE);
                    ret = ret && (s->shaderSampledImageArrayNonUniformIndexing == VK_TRUE);
                    ret = ret && (s->shaderStorageBufferArrayNonUniformIndexing == VK_TRUE);
                    ret = ret && (s->shaderStorageImageArrayNonUniformIndexing == VK_TRUE);
                    ret = ret && (s->shaderUniformTexelBufferArrayNonUniformIndexing == VK_TRUE);
                    ret = ret && (s->shaderStorageTexelBufferArrayNonUniformIndexing == VK_TRUE);
                    ret = ret && (s->descriptorBindingUniformBufferUpdateAfterBind == VK_TRUE);
                    ret = ret && (s->descriptorBindingSampledImageUpdateAfterBind == VK_TRUE);
                    ret = ret && (s->descriptorBindingStorageImageUpdateAfterBind == VK_TRUE);
                    ret = ret && (s->descriptorBindingStorageBufferUpdateAfterBind == VK_TRUE);
                    ret = ret && (s->descriptorBindingUniformTexelBufferUpdateAfterBind == VK_TRUE);
                    ret = ret && (s->descriptorBindingStorageTexelBufferUpdateAfterBind == VK_TRUE);
                    ret = ret && (s->descriptorBindingUpdateUnusedWhilePending == VK_TRUE);
                    ret = ret && (s->descriptorBindingPartiallyBound == VK_TRUE);
                    ret = ret && (s->descriptorBindingVariableDescriptorCount == VK_TRUE);
                    ret = ret && (s->runtimeDescriptorArray == VK_TRUE);
                    ret = ret && (s->scalarBlockLayout == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES: {
                    VkPhysicalDeviceVulkan13Features* s = reinterpret_cast<VkPhysicalDeviceVulkan13Features*>(p);
                    ret = ret && (s->robustImageAccess == VK_TRUE);
                    ret = ret && (s->shaderTerminateInvocation == VK_TRUE);
                    ret = ret && (s->shaderZeroInitializeWorkgroupMemory == VK_TRUE);
                    ret = ret && (s->synchronization2 == VK_TRUE);
                    ret = ret && (s->shaderIntegerDotProduct == VK_TRUE);
                    ret = ret && (s->maintenance4 == VK_TRUE);
                    ret = ret && (s->pipelineCreationCacheControl == VK_TRUE);
                    ret = ret && (s->subgroupSizeControl == VK_TRUE);
                    ret = ret && (s->computeFullSubgroups == VK_TRUE);
                    ret = ret && (s->shaderDemoteToHelperInvocation == VK_TRUE);
                    ret = ret && (s->inlineUniformBlock == VK_TRUE);
                    ret = ret && (s->descriptorBindingInlineUniformBlockUpdateAfterBind == VK_TRUE);
                } break;
                default: break;
            }
        return ret;
    }
};

static const _vpPropertyDesc _propertyDesc = {
    [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES: {
                    VkPhysicalDeviceVulkan11Properties* s = reinterpret_cast<VkPhysicalDeviceVulkan11Properties*>(p);
                    s->maxMultiviewViewCount = 6;
                    s->maxMultiviewInstanceIndex = 134217727;
                    s->subgroupSize = 4;
                    s->subgroupSupportedStages = (VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
                    s->subgroupSupportedOperations = (VK_SUBGROUP_FEATURE_BASIC_BIT | VK_SUBGROUP_FEATURE_VOTE_BIT | VK_SUBGROUP_FEATURE_ARITHMETIC_BIT | VK_SUBGROUP_FEATURE_BALLOT_BIT | VK_SUBGROUP_FEATURE_SHUFFLE_BIT | VK_SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT | VK_SUBGROUP_FEATURE_QUAD_BIT);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES: {
                    VkPhysicalDeviceVulkan12Properties* s = reinterpret_cast<VkPhysicalDeviceVulkan12Properties*>(p);
                    s->maxTimelineSemaphoreValueDifference = 2147483647;
                    s->shaderSignedZeroInfNanPreserveFloat16 = VK_TRUE;
                    s->shaderSignedZeroInfNanPreserveFloat32 = VK_TRUE;
                    s->maxPerStageDescriptorUpdateAfterBindSamplers = 500000;
                    s->maxPerStageDescriptorUpdateAfterBindUniformBuffers = 12;
                    s->maxPerStageDescriptorUpdateAfterBindStorageBuffers = 500000;
                    s->maxPerStageDescriptorUpdateAfterBindSampledImages = 500000;
                    s->maxPerStageDescriptorUpdateAfterBindStorageImages = 500000;
                    s->maxPerStageDescriptorUpdateAfterBindInputAttachments = 7;
                    s->maxPerStageUpdateAfterBindResources = 500000;
                    s->maxDescriptorSetUpdateAfterBindSamplers = 500000;
                    s->maxDescriptorSetUpdateAfterBindUniformBuffers = 72;
                    s->maxDescriptorSetUpdateAfterBindUniformBuffersDynamic = 8;
                    s->maxDescriptorSetUpdateAfterBindStorageBuffers = 500000;
                    s->maxDescriptorSetUpdateAfterBindStorageBuffersDynamic = 4;
                    s->maxDescriptorSetUpdateAfterBindSampledImages = 500000;
                    s->maxDescriptorSetUpdateAfterBindStorageImages = 500000;
                    s->maxDescriptorSetUpdateAfterBindInputAttachments = 7;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES: {
                    VkPhysicalDeviceVulkan13Properties* s = reinterpret_cast<VkPhysicalDeviceVulkan13Properties*>(p);
                    s->maxBufferSize = 1073741824;
                    s->maxInlineUniformBlockSize = 256;
                    s->maxPerStageDescriptorInlineUniformBlocks = 4;
                    s->maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks = 4;
                    s->maxDescriptorSetInlineUniformBlocks = 4;
                    s->maxDescriptorSetUpdateAfterBindInlineUniformBlocks = 4;
                    s->maxInlineUniformTotalSize = 4;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2: {
                    VkPhysicalDeviceProperties2* s = reinterpret_cast<VkPhysicalDeviceProperties2*>(p);
                    s->properties.limits.maxImageDimension1D = 8192;
                    s->properties.limits.maxImageDimension2D = 8192;
                    s->properties.limits.maxImageDimensionCube = 8192;
                    s->properties.limits.maxImageArrayLayers = 2048;
                    s->properties.limits.maxUniformBufferRange = 65536;
                    s->properties.limits.bufferImageGranularity = 4096;
                    s->properties.limits.maxPerStageDescriptorSamplers = 64;
                    s->properties.limits.maxPerStageDescriptorUniformBuffers = 15;
                    s->properties.limits.maxPerStageDescriptorStorageBuffers = 30;
                    s->properties.limits.maxPerStageDescriptorSampledImages = 200;
                    s->properties.limits.maxPerStageDescriptorStorageImages = 16;
                    s->properties.limits.maxPerStageResources = 200;
                    s->properties.limits.maxDescriptorSetSamplers = 576;
                    s->properties.limits.maxDescriptorSetUniformBuffers = 90;
                    s->properties.limits.maxDescriptorSetStorageBuffers = 96;
                    s->properties.limits.maxDescriptorSetSampledImages = 1800;
                    s->properties.limits.maxDescriptorSetStorageImages = 144;
                    s->properties.limits.maxFragmentCombinedOutputResources = 16;
                    s->properties.limits.maxComputeWorkGroupInvocations = 256;
                    s->properties.limits.maxComputeWorkGroupSize[0] = 1024;
                    s->properties.limits.maxComputeWorkGroupSize[1] = 1024;
                    s->properties.limits.maxComputeWorkGroupSize[2] = 64;
                    s->properties.limits.subTexelPrecisionBits = 8;
                    s->properties.limits.mipmapPrecisionBits = 6;
                    s->properties.limits.maxSamplerLodBias = 14;
                    s->properties.limits.pointSizeGranularity = 0.125;
                    s->properties.limits.lineWidthGranularity = 0.5;
                    s->properties.limits.standardSampleLocations = VK_TRUE;
                    s->properties.limits.maxColorAttachments = 7;
                } break;
                default: break;
            }
    },
    [](VkBaseOutStructure* p) -> bool {
        bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES: {
                    VkPhysicalDeviceVulkan11Properties* s = reinterpret_cast<VkPhysicalDeviceVulkan11Properties*>(p);
                    ret = ret && (s->maxMultiviewViewCount >= 6);
                    ret = ret && (s->maxMultiviewInstanceIndex >= 134217727);
                    ret = ret && ((s->subgroupSupportedStages & (VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)) == (VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_FRAGMENT_BIT));
                    ret = ret && ((s->subgroupSupportedOperations & (VK_SUBGROUP_FEATURE_BASIC_BIT | VK_SUBGROUP_FEATURE_VOTE_BIT | VK_SUBGROUP_FEATURE_ARITHMETIC_BIT | VK_SUBGROUP_FEATURE_BALLOT_BIT | VK_SUBGROUP_FEATURE_SHUFFLE_BIT | VK_SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT | VK_SUBGROUP_FEATURE_QUAD_BIT)) == (VK_SUBGROUP_FEATURE_BASIC_BIT | VK_SUBGROUP_FEATURE_VOTE_BIT | VK_SUBGROUP_FEATURE_ARITHMETIC_BIT | VK_SUBGROUP_FEATURE_BALLOT_BIT | VK_SUBGROUP_FEATURE_SHUFFLE_BIT | VK_SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT | VK_SUBGROUP_FEATURE_QUAD_BIT));
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES: {
                    VkPhysicalDeviceVulkan12Properties* s = reinterpret_cast<VkPhysicalDeviceVulkan12Properties*>(p);
                    ret = ret && (s->maxTimelineSemaphoreValueDifference >= 2147483647);
                    ret = ret && (s->shaderSignedZeroInfNanPreserveFloat16 == VK_TRUE);
                    ret = ret && (s->shaderSignedZeroInfNanPreserveFloat32 == VK_TRUE);
                    ret = ret && (s->maxPerStageDescriptorUpdateAfterBindSamplers >= 500000);
                    ret = ret && (s->maxPerStageDescriptorUpdateAfterBindUniformBuffers >= 12);
                    ret = ret && (s->maxPerStageDescriptorUpdateAfterBindStorageBuffers >= 500000);
                    ret = ret && (s->maxPerStageDescriptorUpdateAfterBindSampledImages >= 500000);
                    ret = ret && (s->maxPerStageDescriptorUpdateAfterBindStorageImages >= 500000);
                    ret = ret && (s->maxPerStageDescriptorUpdateAfterBindInputAttachments >= 7);
                    ret = ret && (s->maxPerStageUpdateAfterBindResources >= 500000);
                    ret = ret && (s->maxDescriptorSetUpdateAfterBindSamplers >= 500000);
                    ret = ret && (s->maxDescriptorSetUpdateAfterBindUniformBuffers >= 72);
                    ret = ret && (s->maxDescriptorSetUpdateAfterBindUniformBuffersDynamic >= 8);
                    ret = ret && (s->maxDescriptorSetUpdateAfterBindStorageBuffers >= 500000);
                    ret = ret && (s->maxDescriptorSetUpdateAfterBindStorageBuffersDynamic >= 4);
                    ret = ret && (s->maxDescriptorSetUpdateAfterBindSampledImages >= 500000);
                    ret = ret && (s->maxDescriptorSetUpdateAfterBindStorageImages >= 500000);
                    ret = ret && (s->maxDescriptorSetUpdateAfterBindInputAttachments >= 7);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES: {
                    VkPhysicalDeviceVulkan13Properties* s = reinterpret_cast<VkPhysicalDeviceVulkan13Properties*>(p);
                    ret = ret && (s->maxBufferSize >= 1073741824);
                    ret = ret && (s->maxInlineUniformBlockSize >= 256);
                    ret = ret && (s->maxPerStageDescriptorInlineUniformBlocks >= 4);
                    ret = ret && (s->maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks >= 4);
                    ret = ret && (s->maxDescriptorSetInlineUniformBlocks >= 4);
                    ret = ret && (s->maxDescriptorSetUpdateAfterBindInlineUniformBlocks >= 4);
                    ret = ret && (s->maxInlineUniformTotalSize >= 4);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2: {
                    VkPhysicalDeviceProperties2* s = reinterpret_cast<VkPhysicalDeviceProperties2*>(p);
                    ret = ret && (s->properties.limits.maxImageDimension1D >= 8192);
                    ret = ret && (s->properties.limits.maxImageDimension2D >= 8192);
                    ret = ret && (s->properties.limits.maxImageDimensionCube >= 8192);
                    ret = ret && (s->properties.limits.maxImageArrayLayers >= 2048);
                    ret = ret && (s->properties.limits.maxUniformBufferRange >= 65536);
                    ret = ret && (s->properties.limits.bufferImageGranularity <= 4096);
                    ret = ret && (s->properties.limits.maxPerStageDescriptorSamplers >= 64);
                    ret = ret && (s->properties.limits.maxPerStageDescriptorUniformBuffers >= 15);
                    ret = ret && (s->properties.limits.maxPerStageDescriptorStorageBuffers >= 30);
                    ret = ret && (s->properties.limits.maxPerStageDescriptorSampledImages >= 200);
                    ret = ret && (s->properties.limits.maxPerStageDescriptorStorageImages >= 16);
                    ret = ret && (s->properties.limits.maxPerStageResources >= 200);
                    ret = ret && (s->properties.limits.maxDescriptorSetSamplers >= 576);
                    ret = ret && (s->properties.limits.maxDescriptorSetUniformBuffers >= 90);
                    ret = ret && (s->properties.limits.maxDescriptorSetStorageBuffers >= 96);
                    ret = ret && (s->properties.limits.maxDescriptorSetSampledImages >= 1800);
                    ret = ret && (s->properties.limits.maxDescriptorSetStorageImages >= 144);
                    ret = ret && (s->properties.limits.maxFragmentCombinedOutputResources >= 16);
                    ret = ret && (s->properties.limits.maxComputeWorkGroupInvocations >= 256);
                    ret = ret && (s->properties.limits.maxComputeWorkGroupSize[0] >= 1024);
                    ret = ret && (s->properties.limits.maxComputeWorkGroupSize[1] >= 1024);
                    ret = ret && (s->properties.limits.maxComputeWorkGroupSize[2] >= 64);
                    ret = ret && (s->properties.limits.subTexelPrecisionBits >= 8);
                    ret = ret && (s->properties.limits.mipmapPrecisionBits >= 6);
                    ret = ret && (s->properties.limits.maxSamplerLodBias >= 14);
                    ret = ret && (s->properties.limits.pointSizeGranularity <= 0.125);
                    ret = ret && (s->properties.limits.lineWidthGranularity <= 0.5);
                    ret = ret && (s->properties.limits.standardSampleLocations == VK_TRUE);
                    ret = ret && (s->properties.limits.maxColorAttachments >= 7);
                } break;
                default: break;
            }
        return ret;
    }
};

} // namespace VP_KHR_ROADMAP_2022
#endif

#ifdef VP_LUNARG_desktop_portability_2021_subset
namespace VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET {

static const VkExtensionProperties _deviceExtensions[] = {
    VkExtensionProperties{ VK_KHR_8BIT_STORAGE_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_IMAGE_FORMAT_LIST_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME, 4 },
    VkExtensionProperties{ VK_KHR_SAMPLER_MIRROR_CLAMP_TO_EDGE_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_SHADER_SUBGROUP_EXTENDED_TYPES_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_EXT_ROBUSTNESS_2_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_SWAPCHAIN_EXTENSION_NAME, 70 },
    VkExtensionProperties{ VK_KHR_SWAPCHAIN_MUTABLE_FORMAT_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME, 1 },
};

static const VpProfileProperties _fallbacks[] = {
    { VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, VP_LUNARG_DESKTOP_PORTABILITY_2021_SPEC_VERSION },
};

static const VkStructureType _featureStructTypes[] = {
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT16_INT8_FEATURES_KHR },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR },
};

static const VkStructureType _propertyStructTypes[] = {
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES_KHR },
};

static const VkStructureType _queueFamilyStructTypes[] = {
    { VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2 },
};

static const VkStructureType _formatStructTypes[] = {
    { VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2 },
    { VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3 },
};

static const _vpFeatureDesc _featureDesc = {
    [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES: {
                    VkPhysicalDeviceMultiviewFeatures* s = reinterpret_cast<VkPhysicalDeviceMultiviewFeatures*>(p);
                    s->multiview = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2: {
                    VkPhysicalDeviceFeatures2* s = reinterpret_cast<VkPhysicalDeviceFeatures2*>(p);
                    s->features.depthBiasClamp = VK_TRUE;
                    s->features.depthClamp = VK_TRUE;
                    s->features.drawIndirectFirstInstance = VK_TRUE;
                    s->features.dualSrcBlend = VK_TRUE;
                    s->features.fillModeNonSolid = VK_TRUE;
                    s->features.fragmentStoresAndAtomics = VK_TRUE;
                    s->features.fullDrawIndexUint32 = VK_TRUE;
                    s->features.imageCubeArray = VK_TRUE;
                    s->features.independentBlend = VK_TRUE;
                    s->features.inheritedQueries = VK_TRUE;
                    s->features.largePoints = VK_TRUE;
                    s->features.multiDrawIndirect = VK_TRUE;
                    s->features.multiViewport = VK_TRUE;
                    s->features.occlusionQueryPrecise = VK_TRUE;
                    s->features.robustBufferAccess = VK_TRUE;
                    s->features.sampleRateShading = VK_TRUE;
                    s->features.samplerAnisotropy = VK_TRUE;
                    s->features.shaderClipDistance = VK_TRUE;
                    s->features.shaderImageGatherExtended = VK_TRUE;
                    s->features.shaderSampledImageArrayDynamicIndexing = VK_TRUE;
                    s->features.shaderStorageBufferArrayDynamicIndexing = VK_TRUE;
                    s->features.shaderStorageImageArrayDynamicIndexing = VK_TRUE;
                    s->features.shaderStorageImageExtendedFormats = VK_TRUE;
                    s->features.shaderStorageImageWriteWithoutFormat = VK_TRUE;
                    s->features.shaderTessellationAndGeometryPointSize = VK_TRUE;
                    s->features.shaderUniformBufferArrayDynamicIndexing = VK_TRUE;
                    s->features.tessellationShader = VK_TRUE;
                    s->features.textureCompressionBC = VK_TRUE;
                    s->features.vertexPipelineStoresAndAtomics = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR: {
                    VkPhysicalDeviceImagelessFramebufferFeaturesKHR* s = reinterpret_cast<VkPhysicalDeviceImagelessFramebufferFeaturesKHR*>(p);
                    s->imagelessFramebuffer = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES: {
                    VkPhysicalDevice16BitStorageFeatures* s = reinterpret_cast<VkPhysicalDevice16BitStorageFeatures*>(p);
                    s->storageBuffer16BitAccess = VK_TRUE;
                    s->uniformAndStorageBuffer16BitAccess = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR: {
                    VkPhysicalDevice8BitStorageFeaturesKHR* s = reinterpret_cast<VkPhysicalDevice8BitStorageFeaturesKHR*>(p);
                    s->storageBuffer8BitAccess = VK_TRUE;
                    s->uniformAndStorageBuffer8BitAccess = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT: {
                    VkPhysicalDeviceDescriptorIndexingFeaturesEXT* s = reinterpret_cast<VkPhysicalDeviceDescriptorIndexingFeaturesEXT*>(p);
                    s->shaderUniformTexelBufferArrayDynamicIndexing = VK_TRUE;
                    s->shaderStorageTexelBufferArrayDynamicIndexing = VK_TRUE;
                    s->shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
                    s->shaderStorageBufferArrayNonUniformIndexing = VK_TRUE;
                    s->shaderUniformTexelBufferArrayNonUniformIndexing = VK_TRUE;
                    s->descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
                    s->descriptorBindingStorageImageUpdateAfterBind = VK_TRUE;
                    s->descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE;
                    s->descriptorBindingUniformTexelBufferUpdateAfterBind = VK_TRUE;
                    s->descriptorBindingStorageTexelBufferUpdateAfterBind = VK_TRUE;
                    s->descriptorBindingUpdateUnusedWhilePending = VK_TRUE;
                    s->descriptorBindingPartiallyBound = VK_TRUE;
                    s->descriptorBindingVariableDescriptorCount = VK_TRUE;
                    s->runtimeDescriptorArray = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT: {
                    VkPhysicalDeviceHostQueryResetFeaturesEXT* s = reinterpret_cast<VkPhysicalDeviceHostQueryResetFeaturesEXT*>(p);
                    s->hostQueryReset = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR: {
                    VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR* s = reinterpret_cast<VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR*>(p);
                    s->shaderSubgroupExtendedTypes = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR: {
                    VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR* s = reinterpret_cast<VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR*>(p);
                    s->uniformBufferStandardLayout = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES: {
                    VkPhysicalDeviceShaderDrawParametersFeatures* s = reinterpret_cast<VkPhysicalDeviceShaderDrawParametersFeatures*>(p);
                    s->shaderDrawParameters = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT16_INT8_FEATURES_KHR: {
                    VkPhysicalDeviceShaderFloat16Int8FeaturesKHR* s = reinterpret_cast<VkPhysicalDeviceShaderFloat16Int8FeaturesKHR*>(p);
                    s->shaderInt8 = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR: {
                    VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR* s = reinterpret_cast<VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR*>(p);
                    s->samplerYcbcrConversion = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES: {
                    VkPhysicalDeviceVariablePointersFeatures* s = reinterpret_cast<VkPhysicalDeviceVariablePointersFeatures*>(p);
                    s->variablePointersStorageBuffer = VK_TRUE;
                    s->variablePointers = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR: {
                    VkPhysicalDevicePortabilitySubsetFeaturesKHR* s = reinterpret_cast<VkPhysicalDevicePortabilitySubsetFeaturesKHR*>(p);
                    s->vertexAttributeAccessBeyondStride = VK_TRUE;
                    s->separateStencilMaskRef = VK_TRUE;
                    s->mutableComparisonSamplers = VK_TRUE;
                    s->multisampleArrayImage = VK_TRUE;
                    s->imageViewFormatSwizzle = VK_TRUE;
                    s->imageViewFormatReinterpretation = VK_TRUE;
                    s->events = VK_TRUE;
                    s->constantAlphaColorBlendFactors = VK_TRUE;
                } break;
                default: break;
            }
    },
    [](VkBaseOutStructure* p) -> bool {
        bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES: {
                    VkPhysicalDeviceMultiviewFeatures* s = reinterpret_cast<VkPhysicalDeviceMultiviewFeatures*>(p);
                    ret = ret && (s->multiview == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2: {
                    VkPhysicalDeviceFeatures2* s = reinterpret_cast<VkPhysicalDeviceFeatures2*>(p);
                    ret = ret && (s->features.depthBiasClamp == VK_TRUE);
                    ret = ret && (s->features.depthClamp == VK_TRUE);
                    ret = ret && (s->features.drawIndirectFirstInstance == VK_TRUE);
                    ret = ret && (s->features.dualSrcBlend == VK_TRUE);
                    ret = ret && (s->features.fillModeNonSolid == VK_TRUE);
                    ret = ret && (s->features.fragmentStoresAndAtomics == VK_TRUE);
                    ret = ret && (s->features.fullDrawIndexUint32 == VK_TRUE);
                    ret = ret && (s->features.imageCubeArray == VK_TRUE);
                    ret = ret && (s->features.independentBlend == VK_TRUE);
                    ret = ret && (s->features.inheritedQueries == VK_TRUE);
                    ret = ret && (s->features.largePoints == VK_TRUE);
                    ret = ret && (s->features.multiDrawIndirect == VK_TRUE);
                    ret = ret && (s->features.multiViewport == VK_TRUE);
                    ret = ret && (s->features.occlusionQueryPrecise == VK_TRUE);
                    ret = ret && (s->features.robustBufferAccess == VK_TRUE);
                    ret = ret && (s->features.sampleRateShading == VK_TRUE);
                    ret = ret && (s->features.samplerAnisotropy == VK_TRUE);
                    ret = ret && (s->features.shaderClipDistance == VK_TRUE);
                    ret = ret && (s->features.shaderImageGatherExtended == VK_TRUE);
                    ret = ret && (s->features.shaderSampledImageArrayDynamicIndexing == VK_TRUE);
                    ret = ret && (s->features.shaderStorageBufferArrayDynamicIndexing == VK_TRUE);
                    ret = ret && (s->features.shaderStorageImageArrayDynamicIndexing == VK_TRUE);
                    ret = ret && (s->features.shaderStorageImageExtendedFormats == VK_TRUE);
                    ret = ret && (s->features.shaderStorageImageWriteWithoutFormat == VK_TRUE);
                    ret = ret && (s->features.shaderTessellationAndGeometryPointSize == VK_TRUE);
                    ret = ret && (s->features.shaderUniformBufferArrayDynamicIndexing == VK_TRUE);
                    ret = ret && (s->features.tessellationShader == VK_TRUE);
                    ret = ret && (s->features.textureCompressionBC == VK_TRUE);
                    ret = ret && (s->features.vertexPipelineStoresAndAtomics == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR: {
                    VkPhysicalDeviceImagelessFramebufferFeaturesKHR* s = reinterpret_cast<VkPhysicalDeviceImagelessFramebufferFeaturesKHR*>(p);
                    ret = ret && (s->imagelessFramebuffer == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES: {
                    VkPhysicalDevice16BitStorageFeatures* s = reinterpret_cast<VkPhysicalDevice16BitStorageFeatures*>(p);
                    ret = ret && (s->storageBuffer16BitAccess == VK_TRUE);
                    ret = ret && (s->uniformAndStorageBuffer16BitAccess == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR: {
                    VkPhysicalDevice8BitStorageFeaturesKHR* s = reinterpret_cast<VkPhysicalDevice8BitStorageFeaturesKHR*>(p);
                    ret = ret && (s->storageBuffer8BitAccess == VK_TRUE);
                    ret = ret && (s->uniformAndStorageBuffer8BitAccess == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT: {
                    VkPhysicalDeviceDescriptorIndexingFeaturesEXT* s = reinterpret_cast<VkPhysicalDeviceDescriptorIndexingFeaturesEXT*>(p);
                    ret = ret && (s->shaderUniformTexelBufferArrayDynamicIndexing == VK_TRUE);
                    ret = ret && (s->shaderStorageTexelBufferArrayDynamicIndexing == VK_TRUE);
                    ret = ret && (s->shaderSampledImageArrayNonUniformIndexing == VK_TRUE);
                    ret = ret && (s->shaderStorageBufferArrayNonUniformIndexing == VK_TRUE);
                    ret = ret && (s->shaderUniformTexelBufferArrayNonUniformIndexing == VK_TRUE);
                    ret = ret && (s->descriptorBindingSampledImageUpdateAfterBind == VK_TRUE);
                    ret = ret && (s->descriptorBindingStorageImageUpdateAfterBind == VK_TRUE);
                    ret = ret && (s->descriptorBindingStorageBufferUpdateAfterBind == VK_TRUE);
                    ret = ret && (s->descriptorBindingUniformTexelBufferUpdateAfterBind == VK_TRUE);
                    ret = ret && (s->descriptorBindingStorageTexelBufferUpdateAfterBind == VK_TRUE);
                    ret = ret && (s->descriptorBindingUpdateUnusedWhilePending == VK_TRUE);
                    ret = ret && (s->descriptorBindingPartiallyBound == VK_TRUE);
                    ret = ret && (s->descriptorBindingVariableDescriptorCount == VK_TRUE);
                    ret = ret && (s->runtimeDescriptorArray == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT: {
                    VkPhysicalDeviceHostQueryResetFeaturesEXT* s = reinterpret_cast<VkPhysicalDeviceHostQueryResetFeaturesEXT*>(p);
                    ret = ret && (s->hostQueryReset == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR: {
                    VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR* s = reinterpret_cast<VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR*>(p);
                    ret = ret && (s->shaderSubgroupExtendedTypes == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR: {
                    VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR* s = reinterpret_cast<VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR*>(p);
                    ret = ret && (s->uniformBufferStandardLayout == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES: {
                    VkPhysicalDeviceShaderDrawParametersFeatures* s = reinterpret_cast<VkPhysicalDeviceShaderDrawParametersFeatures*>(p);
                    ret = ret && (s->shaderDrawParameters == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT16_INT8_FEATURES_KHR: {
                    VkPhysicalDeviceShaderFloat16Int8FeaturesKHR* s = reinterpret_cast<VkPhysicalDeviceShaderFloat16Int8FeaturesKHR*>(p);
                    ret = ret && (s->shaderInt8 == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR: {
                    VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR* s = reinterpret_cast<VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR*>(p);
                    ret = ret && (s->samplerYcbcrConversion == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES: {
                    VkPhysicalDeviceVariablePointersFeatures* s = reinterpret_cast<VkPhysicalDeviceVariablePointersFeatures*>(p);
                    ret = ret && (s->variablePointersStorageBuffer == VK_TRUE);
                    ret = ret && (s->variablePointers == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR: {
                    VkPhysicalDevicePortabilitySubsetFeaturesKHR* s = reinterpret_cast<VkPhysicalDevicePortabilitySubsetFeaturesKHR*>(p);
                    ret = ret && (s->vertexAttributeAccessBeyondStride == VK_TRUE);
                    ret = ret && (s->separateStencilMaskRef == VK_TRUE);
                    ret = ret && (s->mutableComparisonSamplers == VK_TRUE);
                    ret = ret && (s->multisampleArrayImage == VK_TRUE);
                    ret = ret && (s->imageViewFormatSwizzle == VK_TRUE);
                    ret = ret && (s->imageViewFormatReinterpretation == VK_TRUE);
                    ret = ret && (s->events == VK_TRUE);
                    ret = ret && (s->constantAlphaColorBlendFactors == VK_TRUE);
                } break;
                default: break;
            }
        return ret;
    }
};

static const _vpPropertyDesc _propertyDesc = {
    [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES: {
                    VkPhysicalDeviceMultiviewProperties* s = reinterpret_cast<VkPhysicalDeviceMultiviewProperties*>(p);
                    s->maxMultiviewViewCount = 6;
                    s->maxMultiviewInstanceIndex = 134217727;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2: {
                    VkPhysicalDeviceProperties2* s = reinterpret_cast<VkPhysicalDeviceProperties2*>(p);
                    s->properties.limits.maxImageDimension1D = 16384;
                    s->properties.limits.maxImageDimension2D = 16384;
                    s->properties.limits.maxImageDimension3D = 2048;
                    s->properties.limits.maxImageDimensionCube = 16384;
                    s->properties.limits.maxImageArrayLayers = 2048;
                    s->properties.limits.maxTexelBufferElements = 67108900;
                    s->properties.limits.maxUniformBufferRange = 65536;
                    s->properties.limits.maxStorageBufferRange = 134217728;
                    s->properties.limits.maxPushConstantsSize = 128;
                    s->properties.limits.maxMemoryAllocationCount = 4096;
                    s->properties.limits.maxSamplerAllocationCount = 1024;
                    s->properties.limits.bufferImageGranularity = 1024;
                    s->properties.limits.sparseAddressSpaceSize = 1073741824;
                    s->properties.limits.maxBoundDescriptorSets = 8;
                    s->properties.limits.maxPerStageDescriptorSamplers = 16;
                    s->properties.limits.maxPerStageDescriptorUniformBuffers = 15;
                    s->properties.limits.maxPerStageDescriptorStorageBuffers = 16;
                    s->properties.limits.maxPerStageDescriptorSampledImages = 128;
                    s->properties.limits.maxPerStageDescriptorStorageImages = 8;
                    s->properties.limits.maxPerStageDescriptorInputAttachments = 8;
                    s->properties.limits.maxPerStageResources = 128;
                    s->properties.limits.maxDescriptorSetSamplers = 80;
                    s->properties.limits.maxDescriptorSetUniformBuffers = 90;
                    s->properties.limits.maxDescriptorSetUniformBuffersDynamic = 8;
                    s->properties.limits.maxDescriptorSetStorageBuffers = 155;
                    s->properties.limits.maxDescriptorSetStorageBuffersDynamic = 8;
                    s->properties.limits.maxDescriptorSetSampledImages = 256;
                    s->properties.limits.maxDescriptorSetStorageImages = 40;
                    s->properties.limits.maxDescriptorSetInputAttachments = 8;
                    s->properties.limits.maxVertexInputAttributes = 28;
                    s->properties.limits.maxVertexInputBindings = 28;
                    s->properties.limits.maxVertexInputAttributeOffset = 2047;
                    s->properties.limits.maxVertexInputBindingStride = 2048;
                    s->properties.limits.maxVertexOutputComponents = 124;
                    s->properties.limits.maxTessellationGenerationLevel = 64;
                    s->properties.limits.maxTessellationPatchSize = 32;
                    s->properties.limits.maxTessellationControlPerVertexInputComponents = 124;
                    s->properties.limits.maxTessellationControlPerVertexOutputComponents = 124;
                    s->properties.limits.maxTessellationControlPerPatchOutputComponents = 120;
                    s->properties.limits.maxTessellationControlTotalOutputComponents = 2048;
                    s->properties.limits.maxTessellationEvaluationInputComponents = 124;
                    s->properties.limits.maxTessellationEvaluationOutputComponents = 124;
                    s->properties.limits.maxFragmentInputComponents = 116;
                    s->properties.limits.maxFragmentOutputAttachments = 8;
                    s->properties.limits.maxFragmentDualSrcAttachments = 1;
                    s->properties.limits.maxFragmentCombinedOutputResources = 8;
                    s->properties.limits.maxComputeSharedMemorySize = 32768;
                    s->properties.limits.maxComputeWorkGroupCount[0] = 65535;
                    s->properties.limits.maxComputeWorkGroupCount[1] = 65535;
                    s->properties.limits.maxComputeWorkGroupCount[2] = 65535;
                    s->properties.limits.maxComputeWorkGroupInvocations = 1024;
                    s->properties.limits.maxComputeWorkGroupSize[0] = 1024;
                    s->properties.limits.maxComputeWorkGroupSize[1] = 1024;
                    s->properties.limits.maxComputeWorkGroupSize[2] = 64;
                    s->properties.limits.subPixelPrecisionBits = 4;
                    s->properties.limits.subTexelPrecisionBits = 4;
                    s->properties.limits.mipmapPrecisionBits = 4;
                    s->properties.limits.maxDrawIndexedIndexValue = 4294967295;
                    s->properties.limits.maxDrawIndirectCount = 1073740000;
                    s->properties.limits.maxSamplerLodBias = 14.0;
                    s->properties.limits.maxSamplerAnisotropy = 16.0;
                    s->properties.limits.maxViewports = 16;
                    s->properties.limits.maxViewportDimensions[0] = 16384;
                    s->properties.limits.maxViewportDimensions[1] = 16384;
                    s->properties.limits.viewportBoundsRange[0] = -32768;
                    s->properties.limits.viewportBoundsRange[1] = 32767;
                    s->properties.limits.minMemoryMapAlignment = 64;
                    s->properties.limits.minTexelBufferOffsetAlignment = 64;
                    s->properties.limits.minUniformBufferOffsetAlignment = 256;
                    s->properties.limits.minStorageBufferOffsetAlignment = 64;
                    s->properties.limits.minTexelOffset = -8;
                    s->properties.limits.maxTexelOffset = 7;
                    s->properties.limits.minTexelGatherOffset = -8;
                    s->properties.limits.maxTexelGatherOffset = 7;
                    s->properties.limits.minInterpolationOffset = -0.5;
                    s->properties.limits.maxInterpolationOffset = 0.4375;
                    s->properties.limits.subPixelInterpolationOffsetBits = 4;
                    s->properties.limits.maxFramebufferWidth = 16384;
                    s->properties.limits.maxFramebufferHeight = 16384;
                    s->properties.limits.maxFramebufferLayers = 1024;
                    s->properties.limits.framebufferColorSampleCounts = (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT);
                    s->properties.limits.framebufferDepthSampleCounts = (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT);
                    s->properties.limits.framebufferStencilSampleCounts = (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT);
                    s->properties.limits.maxColorAttachments = 8;
                    s->properties.limits.sampledImageColorSampleCounts = (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT);
                    s->properties.limits.sampledImageIntegerSampleCounts = (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT);
                    s->properties.limits.sampledImageDepthSampleCounts = (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT);
                    s->properties.limits.sampledImageStencilSampleCounts = (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT);
                    s->properties.limits.storageImageSampleCounts = (VK_SAMPLE_COUNT_1_BIT);
                    s->properties.limits.maxSampleMaskWords = 1;
                    s->properties.limits.maxClipDistances = 8;
                    s->properties.limits.maxCullDistances = 8;
                    s->properties.limits.maxCombinedClipAndCullDistances = 8;
                    s->properties.limits.discreteQueuePriorities = 2;
                    s->properties.limits.pointSizeRange[0] = 1.0;
                    s->properties.limits.pointSizeRange[1] = 64.0;
                    s->properties.limits.lineWidthRange[0] = 1.0;
                    s->properties.limits.lineWidthRange[1] = 1.0;
                    s->properties.limits.pointSizeGranularity = 0.125;
                    s->properties.limits.lineWidthGranularity = 0.5;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT: {
                    VkPhysicalDeviceDescriptorIndexingPropertiesEXT* s = reinterpret_cast<VkPhysicalDeviceDescriptorIndexingPropertiesEXT*>(p);
                    s->maxUpdateAfterBindDescriptorsInAllPools = 1048576;
                    s->maxPerStageDescriptorUpdateAfterBindSamplers = 16;
                    s->maxPerStageDescriptorUpdateAfterBindUniformBuffers = 15;
                    s->maxPerStageDescriptorUpdateAfterBindStorageBuffers = 31;
                    s->maxPerStageDescriptorUpdateAfterBindSampledImages = 128;
                    s->maxPerStageDescriptorUpdateAfterBindStorageImages = 8;
                    s->maxPerStageDescriptorUpdateAfterBindInputAttachments = 128;
                    s->maxPerStageUpdateAfterBindResources = 159;
                    s->maxDescriptorSetUpdateAfterBindSamplers = 80;
                    s->maxDescriptorSetUpdateAfterBindUniformBuffers = 90;
                    s->maxDescriptorSetUpdateAfterBindUniformBuffersDynamic = 8;
                    s->maxDescriptorSetUpdateAfterBindStorageBuffers = 155;
                    s->maxDescriptorSetUpdateAfterBindStorageBuffersDynamic = 8;
                    s->maxDescriptorSetUpdateAfterBindSampledImages = 640;
                    s->maxDescriptorSetUpdateAfterBindStorageImages = 40;
                    s->maxDescriptorSetUpdateAfterBindInputAttachments = 8;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR: {
                    VkPhysicalDeviceDepthStencilResolvePropertiesKHR* s = reinterpret_cast<VkPhysicalDeviceDepthStencilResolvePropertiesKHR*>(p);
                    s->independentResolve = VK_TRUE;
                    s->independentResolveNone = VK_TRUE;
                    s->supportedDepthResolveModes = (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT);
                    s->supportedStencilResolveModes = (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES: {
                    VkPhysicalDeviceMaintenance3Properties* s = reinterpret_cast<VkPhysicalDeviceMaintenance3Properties*>(p);
                    s->maxPerSetDescriptors = 700;
                    s->maxMemoryAllocationSize = 2147483648;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES_KHR: {
                    VkPhysicalDevicePortabilitySubsetPropertiesKHR* s = reinterpret_cast<VkPhysicalDevicePortabilitySubsetPropertiesKHR*>(p);
                    s->minVertexInputBindingStrideAlignment = 4;
                } break;
                default: break;
            }
    },
    [](VkBaseOutStructure* p) -> bool {
        bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES: {
                    VkPhysicalDeviceMultiviewProperties* s = reinterpret_cast<VkPhysicalDeviceMultiviewProperties*>(p);
                    ret = ret && (s->maxMultiviewViewCount >= 6);
                    ret = ret && (s->maxMultiviewInstanceIndex >= 134217727);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2: {
                    VkPhysicalDeviceProperties2* s = reinterpret_cast<VkPhysicalDeviceProperties2*>(p);
                    ret = ret && (s->properties.limits.maxImageDimension1D >= 16384);
                    ret = ret && (s->properties.limits.maxImageDimension2D >= 16384);
                    ret = ret && (s->properties.limits.maxImageDimension3D >= 2048);
                    ret = ret && (s->properties.limits.maxImageDimensionCube >= 16384);
                    ret = ret && (s->properties.limits.maxImageArrayLayers >= 2048);
                    ret = ret && (s->properties.limits.maxTexelBufferElements >= 67108900);
                    ret = ret && (s->properties.limits.maxUniformBufferRange >= 65536);
                    ret = ret && (s->properties.limits.maxStorageBufferRange >= 134217728);
                    ret = ret && (s->properties.limits.maxPushConstantsSize >= 128);
                    ret = ret && (s->properties.limits.maxMemoryAllocationCount >= 4096);
                    ret = ret && (s->properties.limits.maxSamplerAllocationCount >= 1024);
                    ret = ret && (s->properties.limits.bufferImageGranularity <= 1024);
                    ret = ret && (s->properties.limits.sparseAddressSpaceSize >= 1073741824);
                    ret = ret && (s->properties.limits.maxBoundDescriptorSets >= 8);
                    ret = ret && (s->properties.limits.maxPerStageDescriptorSamplers >= 16);
                    ret = ret && (s->properties.limits.maxPerStageDescriptorUniformBuffers >= 15);
                    ret = ret && (s->properties.limits.maxPerStageDescriptorStorageBuffers >= 16);
                    ret = ret && (s->properties.limits.maxPerStageDescriptorSampledImages >= 128);
                    ret = ret && (s->properties.limits.maxPerStageDescriptorStorageImages >= 8);
                    ret = ret && (s->properties.limits.maxPerStageDescriptorInputAttachments >= 8);
                    ret = ret && (s->properties.limits.maxPerStageResources >= 128);
                    ret = ret && (s->properties.limits.maxDescriptorSetSamplers >= 80);
                    ret = ret && (s->properties.limits.maxDescriptorSetUniformBuffers >= 90);
                    ret = ret && (s->properties.limits.maxDescriptorSetUniformBuffersDynamic >= 8);
                    ret = ret && (s->properties.limits.maxDescriptorSetStorageBuffers >= 155);
                    ret = ret && (s->properties.limits.maxDescriptorSetStorageBuffersDynamic >= 8);
                    ret = ret && (s->properties.limits.maxDescriptorSetSampledImages >= 256);
                    ret = ret && (s->properties.limits.maxDescriptorSetStorageImages >= 40);
                    ret = ret && (s->properties.limits.maxDescriptorSetInputAttachments >= 8);
                    ret = ret && (s->properties.limits.maxVertexInputAttributes >= 28);
                    ret = ret && (s->properties.limits.maxVertexInputBindings >= 28);
                    ret = ret && (s->properties.limits.maxVertexInputAttributeOffset >= 2047);
                    ret = ret && (s->properties.limits.maxVertexInputBindingStride >= 2048);
                    ret = ret && (s->properties.limits.maxVertexOutputComponents >= 124);
                    ret = ret && (s->properties.limits.maxTessellationGenerationLevel >= 64);
                    ret = ret && (s->properties.limits.maxTessellationPatchSize >= 32);
                    ret = ret && (s->properties.limits.maxTessellationControlPerVertexInputComponents >= 124);
                    ret = ret && (s->properties.limits.maxTessellationControlPerVertexOutputComponents >= 124);
                    ret = ret && (s->properties.limits.maxTessellationControlPerPatchOutputComponents >= 120);
                    ret = ret && (s->properties.limits.maxTessellationControlTotalOutputComponents >= 2048);
                    ret = ret && (s->properties.limits.maxTessellationEvaluationInputComponents >= 124);
                    ret = ret && (s->properties.limits.maxTessellationEvaluationOutputComponents >= 124);
                    ret = ret && (s->properties.limits.maxFragmentInputComponents >= 116);
                    ret = ret && (s->properties.limits.maxFragmentOutputAttachments >= 8);
                    ret = ret && (s->properties.limits.maxFragmentDualSrcAttachments >= 1);
                    ret = ret && (s->properties.limits.maxFragmentCombinedOutputResources >= 8);
                    ret = ret && (s->properties.limits.maxComputeSharedMemorySize >= 32768);
                    ret = ret && (s->properties.limits.maxComputeWorkGroupCount[0] >= 65535);
                    ret = ret && (s->properties.limits.maxComputeWorkGroupCount[1] >= 65535);
                    ret = ret && (s->properties.limits.maxComputeWorkGroupCount[2] >= 65535);
                    ret = ret && (s->properties.limits.maxComputeWorkGroupInvocations >= 1024);
                    ret = ret && (s->properties.limits.maxComputeWorkGroupSize[0] >= 1024);
                    ret = ret && (s->properties.limits.maxComputeWorkGroupSize[1] >= 1024);
                    ret = ret && (s->properties.limits.maxComputeWorkGroupSize[2] >= 64);
                    ret = ret && (s->properties.limits.subPixelPrecisionBits >= 4);
                    ret = ret && (s->properties.limits.subTexelPrecisionBits >= 4);
                    ret = ret && (s->properties.limits.mipmapPrecisionBits >= 4);
                    ret = ret && (s->properties.limits.maxDrawIndexedIndexValue >= 4294967295);
                    ret = ret && (s->properties.limits.maxDrawIndirectCount >= 1073740000);
                    ret = ret && (s->properties.limits.maxSamplerLodBias >= 14.0);
                    ret = ret && (s->properties.limits.maxSamplerAnisotropy >= 16.0);
                    ret = ret && (s->properties.limits.maxViewports >= 16);
                    ret = ret && (s->properties.limits.maxViewportDimensions[0] >= 16384);
                    ret = ret && (s->properties.limits.maxViewportDimensions[1] >= 16384);
                    ret = ret && (s->properties.limits.viewportBoundsRange[0] <= -32768);
                    ret = ret && (s->properties.limits.viewportBoundsRange[1] >= 32767);
                    ret = ret && (s->properties.limits.minMemoryMapAlignment >= 64);
                    ret = ret && (s->properties.limits.minTexelBufferOffsetAlignment <= 64);
                    ret = ret && (s->properties.limits.minUniformBufferOffsetAlignment <= 256);
                    ret = ret && (s->properties.limits.minStorageBufferOffsetAlignment <= 64);
                    ret = ret && (s->properties.limits.minTexelOffset <= -8);
                    ret = ret && (s->properties.limits.maxTexelOffset >= 7);
                    ret = ret && (s->properties.limits.minTexelGatherOffset <= -8);
                    ret = ret && (s->properties.limits.maxTexelGatherOffset >= 7);
                    ret = ret && (s->properties.limits.minInterpolationOffset <= -0.5);
                    ret = ret && (s->properties.limits.maxInterpolationOffset >= 0.4375);
                    ret = ret && (s->properties.limits.subPixelInterpolationOffsetBits >= 4);
                    ret = ret && (s->properties.limits.maxFramebufferWidth >= 16384);
                    ret = ret && (s->properties.limits.maxFramebufferHeight >= 16384);
                    ret = ret && (s->properties.limits.maxFramebufferLayers >= 1024);
                    ret = ret && ((s->properties.limits.framebufferColorSampleCounts & (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT)) == (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT));
                    ret = ret && ((s->properties.limits.framebufferDepthSampleCounts & (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT)) == (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT));
                    ret = ret && ((s->properties.limits.framebufferStencilSampleCounts & (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT)) == (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT));
                    ret = ret && (s->properties.limits.maxColorAttachments >= 8);
                    ret = ret && ((s->properties.limits.sampledImageColorSampleCounts & (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT)) == (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT));
                    ret = ret && ((s->properties.limits.sampledImageIntegerSampleCounts & (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT)) == (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT));
                    ret = ret && ((s->properties.limits.sampledImageDepthSampleCounts & (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT)) == (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT));
                    ret = ret && ((s->properties.limits.sampledImageStencilSampleCounts & (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT)) == (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT));
                    ret = ret && ((s->properties.limits.storageImageSampleCounts & (VK_SAMPLE_COUNT_1_BIT)) == (VK_SAMPLE_COUNT_1_BIT));
                    ret = ret && (s->properties.limits.maxSampleMaskWords >= 1);
                    ret = ret && (s->properties.limits.maxClipDistances >= 8);
                    ret = ret && (s->properties.limits.maxCullDistances >= 8);
                    ret = ret && (s->properties.limits.maxCombinedClipAndCullDistances >= 8);
                    ret = ret && (s->properties.limits.discreteQueuePriorities >= 2);
                    ret = ret && (s->properties.limits.pointSizeRange[0] <= 1.0);
                    ret = ret && (s->properties.limits.pointSizeRange[1] >= 64.0);
                    ret = ret && (s->properties.limits.lineWidthRange[0] <= 1.0);
                    ret = ret && (s->properties.limits.lineWidthRange[1] >= 1.0);
                    ret = ret && (s->properties.limits.pointSizeGranularity <= 0.125);
                    ret = ret && (s->properties.limits.lineWidthGranularity <= 0.5);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT: {
                    VkPhysicalDeviceDescriptorIndexingPropertiesEXT* s = reinterpret_cast<VkPhysicalDeviceDescriptorIndexingPropertiesEXT*>(p);
                    ret = ret && (s->maxUpdateAfterBindDescriptorsInAllPools >= 1048576);
                    ret = ret && (s->maxPerStageDescriptorUpdateAfterBindSamplers >= 16);
                    ret = ret && (s->maxPerStageDescriptorUpdateAfterBindUniformBuffers >= 15);
                    ret = ret && (s->maxPerStageDescriptorUpdateAfterBindStorageBuffers >= 31);
                    ret = ret && (s->maxPerStageDescriptorUpdateAfterBindSampledImages >= 128);
                    ret = ret && (s->maxPerStageDescriptorUpdateAfterBindStorageImages >= 8);
                    ret = ret && (s->maxPerStageDescriptorUpdateAfterBindInputAttachments >= 128);
                    ret = ret && (s->maxPerStageUpdateAfterBindResources >= 159);
                    ret = ret && (s->maxDescriptorSetUpdateAfterBindSamplers >= 80);
                    ret = ret && (s->maxDescriptorSetUpdateAfterBindUniformBuffers >= 90);
                    ret = ret && (s->maxDescriptorSetUpdateAfterBindUniformBuffersDynamic >= 8);
                    ret = ret && (s->maxDescriptorSetUpdateAfterBindStorageBuffers >= 155);
                    ret = ret && (s->maxDescriptorSetUpdateAfterBindStorageBuffersDynamic >= 8);
                    ret = ret && (s->maxDescriptorSetUpdateAfterBindSampledImages >= 640);
                    ret = ret && (s->maxDescriptorSetUpdateAfterBindStorageImages >= 40);
                    ret = ret && (s->maxDescriptorSetUpdateAfterBindInputAttachments >= 8);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR: {
                    VkPhysicalDeviceDepthStencilResolvePropertiesKHR* s = reinterpret_cast<VkPhysicalDeviceDepthStencilResolvePropertiesKHR*>(p);
                    ret = ret && (s->independentResolve == VK_TRUE);
                    ret = ret && (s->independentResolveNone == VK_TRUE);
                    ret = ret && ((s->supportedDepthResolveModes & (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT)) == (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT));
                    ret = ret && ((s->supportedStencilResolveModes & (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT)) == (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT));
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES: {
                    VkPhysicalDeviceMaintenance3Properties* s = reinterpret_cast<VkPhysicalDeviceMaintenance3Properties*>(p);
                    ret = ret && (s->maxPerSetDescriptors >= 700);
                    ret = ret && (s->maxMemoryAllocationSize >= 2147483648);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES_KHR: {
                    VkPhysicalDevicePortabilitySubsetPropertiesKHR* s = reinterpret_cast<VkPhysicalDevicePortabilitySubsetPropertiesKHR*>(p);
                    ret = ret && (s->minVertexInputBindingStrideAlignment <= 4);
                } break;
                default: break;
            }
        return ret;
    }
};

static const _vpQueueFamilyDesc _queueFamilyDesc[] = {
    {
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2: {
                    VkQueueFamilyProperties2* s = reinterpret_cast<VkQueueFamilyProperties2*>(p);
                    s->queueFamilyProperties.queueFlags = (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT);
                    s->queueFamilyProperties.queueCount = 1;
                    s->queueFamilyProperties.timestampValidBits = 36;
                    s->queueFamilyProperties.minImageTransferGranularity.width = 1;
                    s->queueFamilyProperties.minImageTransferGranularity.height = 1;
                    s->queueFamilyProperties.minImageTransferGranularity.depth = 1;
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2: {
                    VkQueueFamilyProperties2* s = reinterpret_cast<VkQueueFamilyProperties2*>(p);
                    ret = ret && (s->queueFamilyProperties.queueFlags == (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT));
                    ret = ret && (s->queueFamilyProperties.queueCount == 1);
                    ret = ret && (s->queueFamilyProperties.timestampValidBits == 36);
                    ret = ret && (s->queueFamilyProperties.minImageTransferGranularity.width == 1);
                    ret = ret && (s->queueFamilyProperties.minImageTransferGranularity.height == 1);
                    ret = ret && (s->queueFamilyProperties.minImageTransferGranularity.depth == 1);
                } break;
                default: break;
            }
            return ret;
        }
    },
};

static const _vpFormatDesc _formatDesc[] = {
    {
        VK_FORMAT_R8_UNORM,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R8_SNORM,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R8_UINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R8_SINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8_UNORM,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8_SNORM,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8_UINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8_SINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8B8A8_UNORM,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8B8A8_SNORM,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8B8A8_UINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8B8A8_SINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8B8A8_SRGB,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_A8B8G8R8_UNORM_PACK32,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_A8B8G8R8_SNORM_PACK32,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_A8B8G8R8_UINT_PACK32,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_A8B8G8R8_SINT_PACK32,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_A8B8G8R8_SRGB_PACK32,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_A2B10G10R10_UNORM_PACK32,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_A2B10G10R10_UINT_PACK32,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16_UNORM,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16_SNORM,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16_UINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16_SINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16_SFLOAT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16_UNORM,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16_SNORM,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16_UINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16_SINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16_SFLOAT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16B16A16_UNORM,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16B16A16_SNORM,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16B16A16_UINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16B16A16_SINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16B16A16_SFLOAT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R32_UINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R32_SINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R32_SFLOAT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R32G32_UINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R32G32_SINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R32G32_SFLOAT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R32G32B32A32_UINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R32G32B32A32_SINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R32G32B32A32_SFLOAT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_B10G11R11_UFLOAT_PACK32,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_D16_UNORM,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_D32_SFLOAT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC1_RGB_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC1_RGB_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC1_RGBA_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC1_RGBA_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC2_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC2_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC3_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC3_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC4_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC4_SNORM_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC5_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC5_SNORM_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC6H_UFLOAT_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC6H_SFLOAT_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC7_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC7_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
};

} // namespace VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET
#endif

#ifdef VP_LUNARG_desktop_portability_2021
namespace VP_LUNARG_DESKTOP_PORTABILITY_2021 {

static const VkExtensionProperties _deviceExtensions[] = {
    VkExtensionProperties{ VK_KHR_8BIT_STORAGE_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_IMAGE_FORMAT_LIST_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME, 4 },
    VkExtensionProperties{ VK_KHR_SAMPLER_MIRROR_CLAMP_TO_EDGE_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_SHADER_SUBGROUP_EXTENDED_TYPES_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_EXT_ROBUSTNESS_2_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_SWAPCHAIN_EXTENSION_NAME, 70 },
    VkExtensionProperties{ VK_KHR_SWAPCHAIN_MUTABLE_FORMAT_EXTENSION_NAME, 1 },
};

static const VkStructureType _featureStructTypes[] = {
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT16_INT8_FEATURES_KHR },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES },
};

static const VkStructureType _propertyStructTypes[] = {
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR },
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES },
};

static const VkStructureType _queueFamilyStructTypes[] = {
    { VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2 },
};

static const VkStructureType _formatStructTypes[] = {
    { VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2 },
    { VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3 },
};

static const _vpFeatureDesc _featureDesc = {
    [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES: {
                    VkPhysicalDeviceMultiviewFeatures* s = reinterpret_cast<VkPhysicalDeviceMultiviewFeatures*>(p);
                    s->multiview = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2: {
                    VkPhysicalDeviceFeatures2* s = reinterpret_cast<VkPhysicalDeviceFeatures2*>(p);
                    s->features.depthBiasClamp = VK_TRUE;
                    s->features.depthClamp = VK_TRUE;
                    s->features.drawIndirectFirstInstance = VK_TRUE;
                    s->features.dualSrcBlend = VK_TRUE;
                    s->features.fillModeNonSolid = VK_TRUE;
                    s->features.fragmentStoresAndAtomics = VK_TRUE;
                    s->features.fullDrawIndexUint32 = VK_TRUE;
                    s->features.imageCubeArray = VK_TRUE;
                    s->features.independentBlend = VK_TRUE;
                    s->features.inheritedQueries = VK_TRUE;
                    s->features.largePoints = VK_TRUE;
                    s->features.multiDrawIndirect = VK_TRUE;
                    s->features.multiViewport = VK_TRUE;
                    s->features.occlusionQueryPrecise = VK_TRUE;
                    s->features.robustBufferAccess = VK_TRUE;
                    s->features.sampleRateShading = VK_TRUE;
                    s->features.samplerAnisotropy = VK_TRUE;
                    s->features.shaderClipDistance = VK_TRUE;
                    s->features.shaderImageGatherExtended = VK_TRUE;
                    s->features.shaderSampledImageArrayDynamicIndexing = VK_TRUE;
                    s->features.shaderStorageBufferArrayDynamicIndexing = VK_TRUE;
                    s->features.shaderStorageImageArrayDynamicIndexing = VK_TRUE;
                    s->features.shaderStorageImageExtendedFormats = VK_TRUE;
                    s->features.shaderStorageImageWriteWithoutFormat = VK_TRUE;
                    s->features.shaderTessellationAndGeometryPointSize = VK_TRUE;
                    s->features.shaderUniformBufferArrayDynamicIndexing = VK_TRUE;
                    s->features.tessellationShader = VK_TRUE;
                    s->features.textureCompressionBC = VK_TRUE;
                    s->features.vertexPipelineStoresAndAtomics = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR: {
                    VkPhysicalDeviceImagelessFramebufferFeaturesKHR* s = reinterpret_cast<VkPhysicalDeviceImagelessFramebufferFeaturesKHR*>(p);
                    s->imagelessFramebuffer = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES: {
                    VkPhysicalDevice16BitStorageFeatures* s = reinterpret_cast<VkPhysicalDevice16BitStorageFeatures*>(p);
                    s->storageBuffer16BitAccess = VK_TRUE;
                    s->uniformAndStorageBuffer16BitAccess = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR: {
                    VkPhysicalDevice8BitStorageFeaturesKHR* s = reinterpret_cast<VkPhysicalDevice8BitStorageFeaturesKHR*>(p);
                    s->storageBuffer8BitAccess = VK_TRUE;
                    s->uniformAndStorageBuffer8BitAccess = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT: {
                    VkPhysicalDeviceDescriptorIndexingFeaturesEXT* s = reinterpret_cast<VkPhysicalDeviceDescriptorIndexingFeaturesEXT*>(p);
                    s->shaderUniformTexelBufferArrayDynamicIndexing = VK_TRUE;
                    s->shaderStorageTexelBufferArrayDynamicIndexing = VK_TRUE;
                    s->shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
                    s->shaderStorageBufferArrayNonUniformIndexing = VK_TRUE;
                    s->shaderUniformTexelBufferArrayNonUniformIndexing = VK_TRUE;
                    s->descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
                    s->descriptorBindingStorageImageUpdateAfterBind = VK_TRUE;
                    s->descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE;
                    s->descriptorBindingUniformTexelBufferUpdateAfterBind = VK_TRUE;
                    s->descriptorBindingStorageTexelBufferUpdateAfterBind = VK_TRUE;
                    s->descriptorBindingUpdateUnusedWhilePending = VK_TRUE;
                    s->descriptorBindingPartiallyBound = VK_TRUE;
                    s->descriptorBindingVariableDescriptorCount = VK_TRUE;
                    s->runtimeDescriptorArray = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT: {
                    VkPhysicalDeviceHostQueryResetFeaturesEXT* s = reinterpret_cast<VkPhysicalDeviceHostQueryResetFeaturesEXT*>(p);
                    s->hostQueryReset = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR: {
                    VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR* s = reinterpret_cast<VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR*>(p);
                    s->shaderSubgroupExtendedTypes = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR: {
                    VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR* s = reinterpret_cast<VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR*>(p);
                    s->uniformBufferStandardLayout = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES: {
                    VkPhysicalDeviceShaderDrawParametersFeatures* s = reinterpret_cast<VkPhysicalDeviceShaderDrawParametersFeatures*>(p);
                    s->shaderDrawParameters = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT16_INT8_FEATURES_KHR: {
                    VkPhysicalDeviceShaderFloat16Int8FeaturesKHR* s = reinterpret_cast<VkPhysicalDeviceShaderFloat16Int8FeaturesKHR*>(p);
                    s->shaderInt8 = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR: {
                    VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR* s = reinterpret_cast<VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR*>(p);
                    s->samplerYcbcrConversion = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES: {
                    VkPhysicalDeviceVariablePointersFeatures* s = reinterpret_cast<VkPhysicalDeviceVariablePointersFeatures*>(p);
                    s->variablePointersStorageBuffer = VK_TRUE;
                    s->variablePointers = VK_TRUE;
                } break;
                default: break;
            }
    },
    [](VkBaseOutStructure* p) -> bool {
        bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES: {
                    VkPhysicalDeviceMultiviewFeatures* s = reinterpret_cast<VkPhysicalDeviceMultiviewFeatures*>(p);
                    ret = ret && (s->multiview == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2: {
                    VkPhysicalDeviceFeatures2* s = reinterpret_cast<VkPhysicalDeviceFeatures2*>(p);
                    ret = ret && (s->features.depthBiasClamp == VK_TRUE);
                    ret = ret && (s->features.depthClamp == VK_TRUE);
                    ret = ret && (s->features.drawIndirectFirstInstance == VK_TRUE);
                    ret = ret && (s->features.dualSrcBlend == VK_TRUE);
                    ret = ret && (s->features.fillModeNonSolid == VK_TRUE);
                    ret = ret && (s->features.fragmentStoresAndAtomics == VK_TRUE);
                    ret = ret && (s->features.fullDrawIndexUint32 == VK_TRUE);
                    ret = ret && (s->features.imageCubeArray == VK_TRUE);
                    ret = ret && (s->features.independentBlend == VK_TRUE);
                    ret = ret && (s->features.inheritedQueries == VK_TRUE);
                    ret = ret && (s->features.largePoints == VK_TRUE);
                    ret = ret && (s->features.multiDrawIndirect == VK_TRUE);
                    ret = ret && (s->features.multiViewport == VK_TRUE);
                    ret = ret && (s->features.occlusionQueryPrecise == VK_TRUE);
                    ret = ret && (s->features.robustBufferAccess == VK_TRUE);
                    ret = ret && (s->features.sampleRateShading == VK_TRUE);
                    ret = ret && (s->features.samplerAnisotropy == VK_TRUE);
                    ret = ret && (s->features.shaderClipDistance == VK_TRUE);
                    ret = ret && (s->features.shaderImageGatherExtended == VK_TRUE);
                    ret = ret && (s->features.shaderSampledImageArrayDynamicIndexing == VK_TRUE);
                    ret = ret && (s->features.shaderStorageBufferArrayDynamicIndexing == VK_TRUE);
                    ret = ret && (s->features.shaderStorageImageArrayDynamicIndexing == VK_TRUE);
                    ret = ret && (s->features.shaderStorageImageExtendedFormats == VK_TRUE);
                    ret = ret && (s->features.shaderStorageImageWriteWithoutFormat == VK_TRUE);
                    ret = ret && (s->features.shaderTessellationAndGeometryPointSize == VK_TRUE);
                    ret = ret && (s->features.shaderUniformBufferArrayDynamicIndexing == VK_TRUE);
                    ret = ret && (s->features.tessellationShader == VK_TRUE);
                    ret = ret && (s->features.textureCompressionBC == VK_TRUE);
                    ret = ret && (s->features.vertexPipelineStoresAndAtomics == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR: {
                    VkPhysicalDeviceImagelessFramebufferFeaturesKHR* s = reinterpret_cast<VkPhysicalDeviceImagelessFramebufferFeaturesKHR*>(p);
                    ret = ret && (s->imagelessFramebuffer == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES: {
                    VkPhysicalDevice16BitStorageFeatures* s = reinterpret_cast<VkPhysicalDevice16BitStorageFeatures*>(p);
                    ret = ret && (s->storageBuffer16BitAccess == VK_TRUE);
                    ret = ret && (s->uniformAndStorageBuffer16BitAccess == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR: {
                    VkPhysicalDevice8BitStorageFeaturesKHR* s = reinterpret_cast<VkPhysicalDevice8BitStorageFeaturesKHR*>(p);
                    ret = ret && (s->storageBuffer8BitAccess == VK_TRUE);
                    ret = ret && (s->uniformAndStorageBuffer8BitAccess == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT: {
                    VkPhysicalDeviceDescriptorIndexingFeaturesEXT* s = reinterpret_cast<VkPhysicalDeviceDescriptorIndexingFeaturesEXT*>(p);
                    ret = ret && (s->shaderUniformTexelBufferArrayDynamicIndexing == VK_TRUE);
                    ret = ret && (s->shaderStorageTexelBufferArrayDynamicIndexing == VK_TRUE);
                    ret = ret && (s->shaderSampledImageArrayNonUniformIndexing == VK_TRUE);
                    ret = ret && (s->shaderStorageBufferArrayNonUniformIndexing == VK_TRUE);
                    ret = ret && (s->shaderUniformTexelBufferArrayNonUniformIndexing == VK_TRUE);
                    ret = ret && (s->descriptorBindingSampledImageUpdateAfterBind == VK_TRUE);
                    ret = ret && (s->descriptorBindingStorageImageUpdateAfterBind == VK_TRUE);
                    ret = ret && (s->descriptorBindingStorageBufferUpdateAfterBind == VK_TRUE);
                    ret = ret && (s->descriptorBindingUniformTexelBufferUpdateAfterBind == VK_TRUE);
                    ret = ret && (s->descriptorBindingStorageTexelBufferUpdateAfterBind == VK_TRUE);
                    ret = ret && (s->descriptorBindingUpdateUnusedWhilePending == VK_TRUE);
                    ret = ret && (s->descriptorBindingPartiallyBound == VK_TRUE);
                    ret = ret && (s->descriptorBindingVariableDescriptorCount == VK_TRUE);
                    ret = ret && (s->runtimeDescriptorArray == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT: {
                    VkPhysicalDeviceHostQueryResetFeaturesEXT* s = reinterpret_cast<VkPhysicalDeviceHostQueryResetFeaturesEXT*>(p);
                    ret = ret && (s->hostQueryReset == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR: {
                    VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR* s = reinterpret_cast<VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR*>(p);
                    ret = ret && (s->shaderSubgroupExtendedTypes == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR: {
                    VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR* s = reinterpret_cast<VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR*>(p);
                    ret = ret && (s->uniformBufferStandardLayout == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES: {
                    VkPhysicalDeviceShaderDrawParametersFeatures* s = reinterpret_cast<VkPhysicalDeviceShaderDrawParametersFeatures*>(p);
                    ret = ret && (s->shaderDrawParameters == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT16_INT8_FEATURES_KHR: {
                    VkPhysicalDeviceShaderFloat16Int8FeaturesKHR* s = reinterpret_cast<VkPhysicalDeviceShaderFloat16Int8FeaturesKHR*>(p);
                    ret = ret && (s->shaderInt8 == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR: {
                    VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR* s = reinterpret_cast<VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR*>(p);
                    ret = ret && (s->samplerYcbcrConversion == VK_TRUE);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES: {
                    VkPhysicalDeviceVariablePointersFeatures* s = reinterpret_cast<VkPhysicalDeviceVariablePointersFeatures*>(p);
                    ret = ret && (s->variablePointersStorageBuffer == VK_TRUE);
                    ret = ret && (s->variablePointers == VK_TRUE);
                } break;
                default: break;
            }
        return ret;
    }
};

static const _vpPropertyDesc _propertyDesc = {
    [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES: {
                    VkPhysicalDeviceMultiviewProperties* s = reinterpret_cast<VkPhysicalDeviceMultiviewProperties*>(p);
                    s->maxMultiviewViewCount = 6;
                    s->maxMultiviewInstanceIndex = 134217727;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2: {
                    VkPhysicalDeviceProperties2* s = reinterpret_cast<VkPhysicalDeviceProperties2*>(p);
                    s->properties.limits.maxImageDimension1D = 16384;
                    s->properties.limits.maxImageDimension2D = 16384;
                    s->properties.limits.maxImageDimension3D = 2048;
                    s->properties.limits.maxImageDimensionCube = 16384;
                    s->properties.limits.maxImageArrayLayers = 2048;
                    s->properties.limits.maxTexelBufferElements = 67108900;
                    s->properties.limits.maxUniformBufferRange = 65536;
                    s->properties.limits.maxStorageBufferRange = 134217728;
                    s->properties.limits.maxPushConstantsSize = 128;
                    s->properties.limits.maxMemoryAllocationCount = 4096;
                    s->properties.limits.maxSamplerAllocationCount = 1024;
                    s->properties.limits.bufferImageGranularity = 1024;
                    s->properties.limits.sparseAddressSpaceSize = 1073741824;
                    s->properties.limits.maxBoundDescriptorSets = 8;
                    s->properties.limits.maxPerStageDescriptorSamplers = 16;
                    s->properties.limits.maxPerStageDescriptorUniformBuffers = 15;
                    s->properties.limits.maxPerStageDescriptorStorageBuffers = 16;
                    s->properties.limits.maxPerStageDescriptorSampledImages = 128;
                    s->properties.limits.maxPerStageDescriptorStorageImages = 8;
                    s->properties.limits.maxPerStageDescriptorInputAttachments = 8;
                    s->properties.limits.maxPerStageResources = 128;
                    s->properties.limits.maxDescriptorSetSamplers = 80;
                    s->properties.limits.maxDescriptorSetUniformBuffers = 90;
                    s->properties.limits.maxDescriptorSetUniformBuffersDynamic = 8;
                    s->properties.limits.maxDescriptorSetStorageBuffers = 155;
                    s->properties.limits.maxDescriptorSetStorageBuffersDynamic = 8;
                    s->properties.limits.maxDescriptorSetSampledImages = 256;
                    s->properties.limits.maxDescriptorSetStorageImages = 40;
                    s->properties.limits.maxDescriptorSetInputAttachments = 8;
                    s->properties.limits.maxVertexInputAttributes = 28;
                    s->properties.limits.maxVertexInputBindings = 28;
                    s->properties.limits.maxVertexInputAttributeOffset = 2047;
                    s->properties.limits.maxVertexInputBindingStride = 2048;
                    s->properties.limits.maxVertexOutputComponents = 124;
                    s->properties.limits.maxTessellationGenerationLevel = 64;
                    s->properties.limits.maxTessellationPatchSize = 32;
                    s->properties.limits.maxTessellationControlPerVertexInputComponents = 124;
                    s->properties.limits.maxTessellationControlPerVertexOutputComponents = 124;
                    s->properties.limits.maxTessellationControlPerPatchOutputComponents = 120;
                    s->properties.limits.maxTessellationControlTotalOutputComponents = 2048;
                    s->properties.limits.maxTessellationEvaluationInputComponents = 124;
                    s->properties.limits.maxTessellationEvaluationOutputComponents = 124;
                    s->properties.limits.maxFragmentInputComponents = 116;
                    s->properties.limits.maxFragmentOutputAttachments = 8;
                    s->properties.limits.maxFragmentDualSrcAttachments = 1;
                    s->properties.limits.maxFragmentCombinedOutputResources = 8;
                    s->properties.limits.maxComputeSharedMemorySize = 32768;
                    s->properties.limits.maxComputeWorkGroupCount[0] = 65535;
                    s->properties.limits.maxComputeWorkGroupCount[1] = 65535;
                    s->properties.limits.maxComputeWorkGroupCount[2] = 65535;
                    s->properties.limits.maxComputeWorkGroupInvocations = 1024;
                    s->properties.limits.maxComputeWorkGroupSize[0] = 1024;
                    s->properties.limits.maxComputeWorkGroupSize[1] = 1024;
                    s->properties.limits.maxComputeWorkGroupSize[2] = 64;
                    s->properties.limits.subPixelPrecisionBits = 4;
                    s->properties.limits.subTexelPrecisionBits = 4;
                    s->properties.limits.mipmapPrecisionBits = 4;
                    s->properties.limits.maxDrawIndexedIndexValue = 4294967295;
                    s->properties.limits.maxDrawIndirectCount = 1073740000;
                    s->properties.limits.maxSamplerLodBias = 14.0;
                    s->properties.limits.maxSamplerAnisotropy = 16.0;
                    s->properties.limits.maxViewports = 16;
                    s->properties.limits.maxViewportDimensions[0] = 16384;
                    s->properties.limits.maxViewportDimensions[1] = 16384;
                    s->properties.limits.viewportBoundsRange[0] = -32768;
                    s->properties.limits.viewportBoundsRange[1] = 32767;
                    s->properties.limits.minMemoryMapAlignment = 64;
                    s->properties.limits.minTexelBufferOffsetAlignment = 64;
                    s->properties.limits.minUniformBufferOffsetAlignment = 256;
                    s->properties.limits.minStorageBufferOffsetAlignment = 64;
                    s->properties.limits.minTexelOffset = -8;
                    s->properties.limits.maxTexelOffset = 7;
                    s->properties.limits.minTexelGatherOffset = -8;
                    s->properties.limits.maxTexelGatherOffset = 7;
                    s->properties.limits.minInterpolationOffset = -0.5;
                    s->properties.limits.maxInterpolationOffset = 0.4375;
                    s->properties.limits.subPixelInterpolationOffsetBits = 4;
                    s->properties.limits.maxFramebufferWidth = 16384;
                    s->properties.limits.maxFramebufferHeight = 16384;
                    s->properties.limits.maxFramebufferLayers = 1024;
                    s->properties.limits.framebufferColorSampleCounts = (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT);
                    s->properties.limits.framebufferDepthSampleCounts = (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT);
                    s->properties.limits.framebufferStencilSampleCounts = (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT);
                    s->properties.limits.maxColorAttachments = 8;
                    s->properties.limits.sampledImageColorSampleCounts = (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT);
                    s->properties.limits.sampledImageIntegerSampleCounts = (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT);
                    s->properties.limits.sampledImageDepthSampleCounts = (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT);
                    s->properties.limits.sampledImageStencilSampleCounts = (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT);
                    s->properties.limits.storageImageSampleCounts = (VK_SAMPLE_COUNT_1_BIT);
                    s->properties.limits.maxSampleMaskWords = 1;
                    s->properties.limits.maxClipDistances = 8;
                    s->properties.limits.maxCullDistances = 8;
                    s->properties.limits.maxCombinedClipAndCullDistances = 8;
                    s->properties.limits.discreteQueuePriorities = 2;
                    s->properties.limits.pointSizeRange[0] = 1.0;
                    s->properties.limits.pointSizeRange[1] = 64.0;
                    s->properties.limits.lineWidthRange[0] = 1.0;
                    s->properties.limits.lineWidthRange[1] = 1.0;
                    s->properties.limits.pointSizeGranularity = 0.125;
                    s->properties.limits.lineWidthGranularity = 0.5;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT: {
                    VkPhysicalDeviceDescriptorIndexingPropertiesEXT* s = reinterpret_cast<VkPhysicalDeviceDescriptorIndexingPropertiesEXT*>(p);
                    s->maxUpdateAfterBindDescriptorsInAllPools = 1048576;
                    s->maxPerStageDescriptorUpdateAfterBindSamplers = 16;
                    s->maxPerStageDescriptorUpdateAfterBindUniformBuffers = 15;
                    s->maxPerStageDescriptorUpdateAfterBindStorageBuffers = 31;
                    s->maxPerStageDescriptorUpdateAfterBindSampledImages = 128;
                    s->maxPerStageDescriptorUpdateAfterBindStorageImages = 8;
                    s->maxPerStageDescriptorUpdateAfterBindInputAttachments = 128;
                    s->maxPerStageUpdateAfterBindResources = 159;
                    s->maxDescriptorSetUpdateAfterBindSamplers = 80;
                    s->maxDescriptorSetUpdateAfterBindUniformBuffers = 90;
                    s->maxDescriptorSetUpdateAfterBindUniformBuffersDynamic = 8;
                    s->maxDescriptorSetUpdateAfterBindStorageBuffers = 155;
                    s->maxDescriptorSetUpdateAfterBindStorageBuffersDynamic = 8;
                    s->maxDescriptorSetUpdateAfterBindSampledImages = 640;
                    s->maxDescriptorSetUpdateAfterBindStorageImages = 40;
                    s->maxDescriptorSetUpdateAfterBindInputAttachments = 8;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR: {
                    VkPhysicalDeviceDepthStencilResolvePropertiesKHR* s = reinterpret_cast<VkPhysicalDeviceDepthStencilResolvePropertiesKHR*>(p);
                    s->independentResolve = VK_TRUE;
                    s->independentResolveNone = VK_TRUE;
                    s->supportedDepthResolveModes = (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT);
                    s->supportedStencilResolveModes = (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES: {
                    VkPhysicalDeviceMaintenance3Properties* s = reinterpret_cast<VkPhysicalDeviceMaintenance3Properties*>(p);
                    s->maxPerSetDescriptors = 700;
                    s->maxMemoryAllocationSize = 2147483648;
                } break;
                default: break;
            }
    },
    [](VkBaseOutStructure* p) -> bool {
        bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES: {
                    VkPhysicalDeviceMultiviewProperties* s = reinterpret_cast<VkPhysicalDeviceMultiviewProperties*>(p);
                    ret = ret && (s->maxMultiviewViewCount >= 6);
                    ret = ret && (s->maxMultiviewInstanceIndex >= 134217727);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2: {
                    VkPhysicalDeviceProperties2* s = reinterpret_cast<VkPhysicalDeviceProperties2*>(p);
                    ret = ret && (s->properties.limits.maxImageDimension1D >= 16384);
                    ret = ret && (s->properties.limits.maxImageDimension2D >= 16384);
                    ret = ret && (s->properties.limits.maxImageDimension3D >= 2048);
                    ret = ret && (s->properties.limits.maxImageDimensionCube >= 16384);
                    ret = ret && (s->properties.limits.maxImageArrayLayers >= 2048);
                    ret = ret && (s->properties.limits.maxTexelBufferElements >= 67108900);
                    ret = ret && (s->properties.limits.maxUniformBufferRange >= 65536);
                    ret = ret && (s->properties.limits.maxStorageBufferRange >= 134217728);
                    ret = ret && (s->properties.limits.maxPushConstantsSize >= 128);
                    ret = ret && (s->properties.limits.maxMemoryAllocationCount >= 4096);
                    ret = ret && (s->properties.limits.maxSamplerAllocationCount >= 1024);
                    ret = ret && (s->properties.limits.bufferImageGranularity <= 1024);
                    ret = ret && (s->properties.limits.sparseAddressSpaceSize >= 1073741824);
                    ret = ret && (s->properties.limits.maxBoundDescriptorSets >= 8);
                    ret = ret && (s->properties.limits.maxPerStageDescriptorSamplers >= 16);
                    ret = ret && (s->properties.limits.maxPerStageDescriptorUniformBuffers >= 15);
                    ret = ret && (s->properties.limits.maxPerStageDescriptorStorageBuffers >= 16);
                    ret = ret && (s->properties.limits.maxPerStageDescriptorSampledImages >= 128);
                    ret = ret && (s->properties.limits.maxPerStageDescriptorStorageImages >= 8);
                    ret = ret && (s->properties.limits.maxPerStageDescriptorInputAttachments >= 8);
                    ret = ret && (s->properties.limits.maxPerStageResources >= 128);
                    ret = ret && (s->properties.limits.maxDescriptorSetSamplers >= 80);
                    ret = ret && (s->properties.limits.maxDescriptorSetUniformBuffers >= 90);
                    ret = ret && (s->properties.limits.maxDescriptorSetUniformBuffersDynamic >= 8);
                    ret = ret && (s->properties.limits.maxDescriptorSetStorageBuffers >= 155);
                    ret = ret && (s->properties.limits.maxDescriptorSetStorageBuffersDynamic >= 8);
                    ret = ret && (s->properties.limits.maxDescriptorSetSampledImages >= 256);
                    ret = ret && (s->properties.limits.maxDescriptorSetStorageImages >= 40);
                    ret = ret && (s->properties.limits.maxDescriptorSetInputAttachments >= 8);
                    ret = ret && (s->properties.limits.maxVertexInputAttributes >= 28);
                    ret = ret && (s->properties.limits.maxVertexInputBindings >= 28);
                    ret = ret && (s->properties.limits.maxVertexInputAttributeOffset >= 2047);
                    ret = ret && (s->properties.limits.maxVertexInputBindingStride >= 2048);
                    ret = ret && (s->properties.limits.maxVertexOutputComponents >= 124);
                    ret = ret && (s->properties.limits.maxTessellationGenerationLevel >= 64);
                    ret = ret && (s->properties.limits.maxTessellationPatchSize >= 32);
                    ret = ret && (s->properties.limits.maxTessellationControlPerVertexInputComponents >= 124);
                    ret = ret && (s->properties.limits.maxTessellationControlPerVertexOutputComponents >= 124);
                    ret = ret && (s->properties.limits.maxTessellationControlPerPatchOutputComponents >= 120);
                    ret = ret && (s->properties.limits.maxTessellationControlTotalOutputComponents >= 2048);
                    ret = ret && (s->properties.limits.maxTessellationEvaluationInputComponents >= 124);
                    ret = ret && (s->properties.limits.maxTessellationEvaluationOutputComponents >= 124);
                    ret = ret && (s->properties.limits.maxFragmentInputComponents >= 116);
                    ret = ret && (s->properties.limits.maxFragmentOutputAttachments >= 8);
                    ret = ret && (s->properties.limits.maxFragmentDualSrcAttachments >= 1);
                    ret = ret && (s->properties.limits.maxFragmentCombinedOutputResources >= 8);
                    ret = ret && (s->properties.limits.maxComputeSharedMemorySize >= 32768);
                    ret = ret && (s->properties.limits.maxComputeWorkGroupCount[0] >= 65535);
                    ret = ret && (s->properties.limits.maxComputeWorkGroupCount[1] >= 65535);
                    ret = ret && (s->properties.limits.maxComputeWorkGroupCount[2] >= 65535);
                    ret = ret && (s->properties.limits.maxComputeWorkGroupInvocations >= 1024);
                    ret = ret && (s->properties.limits.maxComputeWorkGroupSize[0] >= 1024);
                    ret = ret && (s->properties.limits.maxComputeWorkGroupSize[1] >= 1024);
                    ret = ret && (s->properties.limits.maxComputeWorkGroupSize[2] >= 64);
                    ret = ret && (s->properties.limits.subPixelPrecisionBits >= 4);
                    ret = ret && (s->properties.limits.subTexelPrecisionBits >= 4);
                    ret = ret && (s->properties.limits.mipmapPrecisionBits >= 4);
                    ret = ret && (s->properties.limits.maxDrawIndexedIndexValue >= 4294967295);
                    ret = ret && (s->properties.limits.maxDrawIndirectCount >= 1073740000);
                    ret = ret && (s->properties.limits.maxSamplerLodBias >= 14.0);
                    ret = ret && (s->properties.limits.maxSamplerAnisotropy >= 16.0);
                    ret = ret && (s->properties.limits.maxViewports >= 16);
                    ret = ret && (s->properties.limits.maxViewportDimensions[0] >= 16384);
                    ret = ret && (s->properties.limits.maxViewportDimensions[1] >= 16384);
                    ret = ret && (s->properties.limits.viewportBoundsRange[0] <= -32768);
                    ret = ret && (s->properties.limits.viewportBoundsRange[1] >= 32767);
                    ret = ret && (s->properties.limits.minMemoryMapAlignment >= 64);
                    ret = ret && (s->properties.limits.minTexelBufferOffsetAlignment <= 64);
                    ret = ret && (s->properties.limits.minUniformBufferOffsetAlignment <= 256);
                    ret = ret && (s->properties.limits.minStorageBufferOffsetAlignment <= 64);
                    ret = ret && (s->properties.limits.minTexelOffset <= -8);
                    ret = ret && (s->properties.limits.maxTexelOffset >= 7);
                    ret = ret && (s->properties.limits.minTexelGatherOffset <= -8);
                    ret = ret && (s->properties.limits.maxTexelGatherOffset >= 7);
                    ret = ret && (s->properties.limits.minInterpolationOffset <= -0.5);
                    ret = ret && (s->properties.limits.maxInterpolationOffset >= 0.4375);
                    ret = ret && (s->properties.limits.subPixelInterpolationOffsetBits >= 4);
                    ret = ret && (s->properties.limits.maxFramebufferWidth >= 16384);
                    ret = ret && (s->properties.limits.maxFramebufferHeight >= 16384);
                    ret = ret && (s->properties.limits.maxFramebufferLayers >= 1024);
                    ret = ret && ((s->properties.limits.framebufferColorSampleCounts & (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT)) == (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT));
                    ret = ret && ((s->properties.limits.framebufferDepthSampleCounts & (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT)) == (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT));
                    ret = ret && ((s->properties.limits.framebufferStencilSampleCounts & (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT)) == (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT));
                    ret = ret && (s->properties.limits.maxColorAttachments >= 8);
                    ret = ret && ((s->properties.limits.sampledImageColorSampleCounts & (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT)) == (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT));
                    ret = ret && ((s->properties.limits.sampledImageIntegerSampleCounts & (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT)) == (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT));
                    ret = ret && ((s->properties.limits.sampledImageDepthSampleCounts & (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT)) == (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT));
                    ret = ret && ((s->properties.limits.sampledImageStencilSampleCounts & (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT)) == (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT));
                    ret = ret && ((s->properties.limits.storageImageSampleCounts & (VK_SAMPLE_COUNT_1_BIT)) == (VK_SAMPLE_COUNT_1_BIT));
                    ret = ret && (s->properties.limits.maxSampleMaskWords >= 1);
                    ret = ret && (s->properties.limits.maxClipDistances >= 8);
                    ret = ret && (s->properties.limits.maxCullDistances >= 8);
                    ret = ret && (s->properties.limits.maxCombinedClipAndCullDistances >= 8);
                    ret = ret && (s->properties.limits.discreteQueuePriorities >= 2);
                    ret = ret && (s->properties.limits.pointSizeRange[0] <= 1.0);
                    ret = ret && (s->properties.limits.pointSizeRange[1] >= 64.0);
                    ret = ret && (s->properties.limits.lineWidthRange[0] <= 1.0);
                    ret = ret && (s->properties.limits.lineWidthRange[1] >= 1.0);
                    ret = ret && (s->properties.limits.pointSizeGranularity <= 0.125);
                    ret = ret && (s->properties.limits.lineWidthGranularity <= 0.5);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT: {
                    VkPhysicalDeviceDescriptorIndexingPropertiesEXT* s = reinterpret_cast<VkPhysicalDeviceDescriptorIndexingPropertiesEXT*>(p);
                    ret = ret && (s->maxUpdateAfterBindDescriptorsInAllPools >= 1048576);
                    ret = ret && (s->maxPerStageDescriptorUpdateAfterBindSamplers >= 16);
                    ret = ret && (s->maxPerStageDescriptorUpdateAfterBindUniformBuffers >= 15);
                    ret = ret && (s->maxPerStageDescriptorUpdateAfterBindStorageBuffers >= 31);
                    ret = ret && (s->maxPerStageDescriptorUpdateAfterBindSampledImages >= 128);
                    ret = ret && (s->maxPerStageDescriptorUpdateAfterBindStorageImages >= 8);
                    ret = ret && (s->maxPerStageDescriptorUpdateAfterBindInputAttachments >= 128);
                    ret = ret && (s->maxPerStageUpdateAfterBindResources >= 159);
                    ret = ret && (s->maxDescriptorSetUpdateAfterBindSamplers >= 80);
                    ret = ret && (s->maxDescriptorSetUpdateAfterBindUniformBuffers >= 90);
                    ret = ret && (s->maxDescriptorSetUpdateAfterBindUniformBuffersDynamic >= 8);
                    ret = ret && (s->maxDescriptorSetUpdateAfterBindStorageBuffers >= 155);
                    ret = ret && (s->maxDescriptorSetUpdateAfterBindStorageBuffersDynamic >= 8);
                    ret = ret && (s->maxDescriptorSetUpdateAfterBindSampledImages >= 640);
                    ret = ret && (s->maxDescriptorSetUpdateAfterBindStorageImages >= 40);
                    ret = ret && (s->maxDescriptorSetUpdateAfterBindInputAttachments >= 8);
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR: {
                    VkPhysicalDeviceDepthStencilResolvePropertiesKHR* s = reinterpret_cast<VkPhysicalDeviceDepthStencilResolvePropertiesKHR*>(p);
                    ret = ret && (s->independentResolve == VK_TRUE);
                    ret = ret && (s->independentResolveNone == VK_TRUE);
                    ret = ret && ((s->supportedDepthResolveModes & (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT)) == (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT));
                    ret = ret && ((s->supportedStencilResolveModes & (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT)) == (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT));
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES: {
                    VkPhysicalDeviceMaintenance3Properties* s = reinterpret_cast<VkPhysicalDeviceMaintenance3Properties*>(p);
                    ret = ret && (s->maxPerSetDescriptors >= 700);
                    ret = ret && (s->maxMemoryAllocationSize >= 2147483648);
                } break;
                default: break;
            }
        return ret;
    }
};

static const _vpQueueFamilyDesc _queueFamilyDesc[] = {
    {
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2: {
                    VkQueueFamilyProperties2* s = reinterpret_cast<VkQueueFamilyProperties2*>(p);
                    s->queueFamilyProperties.queueFlags = (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT);
                    s->queueFamilyProperties.queueCount = 1;
                    s->queueFamilyProperties.timestampValidBits = 36;
                    s->queueFamilyProperties.minImageTransferGranularity.width = 1;
                    s->queueFamilyProperties.minImageTransferGranularity.height = 1;
                    s->queueFamilyProperties.minImageTransferGranularity.depth = 1;
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2: {
                    VkQueueFamilyProperties2* s = reinterpret_cast<VkQueueFamilyProperties2*>(p);
                    ret = ret && (s->queueFamilyProperties.queueFlags == (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT));
                    ret = ret && (s->queueFamilyProperties.queueCount == 1);
                    ret = ret && (s->queueFamilyProperties.timestampValidBits == 36);
                    ret = ret && (s->queueFamilyProperties.minImageTransferGranularity.width == 1);
                    ret = ret && (s->queueFamilyProperties.minImageTransferGranularity.height == 1);
                    ret = ret && (s->queueFamilyProperties.minImageTransferGranularity.depth == 1);
                } break;
                default: break;
            }
            return ret;
        }
    },
};

static const _vpFormatDesc _formatDesc[] = {
    {
        VK_FORMAT_R8_UNORM,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R8_SNORM,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R8_UINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R8_SINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8_UNORM,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8_SNORM,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8_UINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8_SINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8B8A8_UNORM,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8B8A8_SNORM,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8B8A8_UINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8B8A8_SINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R8G8B8A8_SRGB,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_A8B8G8R8_UNORM_PACK32,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_A8B8G8R8_SNORM_PACK32,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_A8B8G8R8_UINT_PACK32,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_A8B8G8R8_SINT_PACK32,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_A8B8G8R8_SRGB_PACK32,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_A2B10G10R10_UNORM_PACK32,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_A2B10G10R10_UINT_PACK32,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16_UNORM,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16_SNORM,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16_UINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16_SINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16_SFLOAT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16_UNORM,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16_SNORM,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16_UINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16_SINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16_SFLOAT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16B16A16_UNORM,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16B16A16_SNORM,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16B16A16_UINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16B16A16_SINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R16G16B16A16_SFLOAT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R32_UINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R32_SINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R32_SFLOAT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R32G32_UINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R32G32_SINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R32G32_SFLOAT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R32G32B32A32_UINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R32G32B32A32_SINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_R32G32B32A32_SFLOAT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_B10G11R11_UFLOAT_PACK32,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.linearTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                    s->formatProperties.bufferFeatures = (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.linearTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                    ret = ret && (s->formatProperties.bufferFeatures == (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_D16_UNORM,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_D32_SFLOAT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC1_RGB_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC1_RGB_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC1_RGBA_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC1_RGBA_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC2_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC2_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC3_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC3_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC4_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC4_SNORM_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC5_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC5_SNORM_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC6H_UFLOAT_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC6H_SFLOAT_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC7_UNORM_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
    {
        VK_FORMAT_BC7_SRGB_BLOCK,
        [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    s->formatProperties.optimalTilingFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT);
                } break;
                default: break;
            }
        },
        [](VkBaseOutStructure* p) -> bool {
            bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2: {
                    VkFormatProperties2* s = reinterpret_cast<VkFormatProperties2*>(p);
                    ret = ret && (s->formatProperties.optimalTilingFeatures == (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT));
                } break;
                default: break;
            }
            return ret;
        }
    },
};

} // namespace VP_LUNARG_DESKTOP_PORTABILITY_2021
#endif

static const _vpProfileDesc _vpProfiles[] = {
#ifdef VP_ANDROID_baseline_2021
    _vpProfileDesc{
        VpProfileProperties{ VP_ANDROID_BASELINE_2021_NAME, VP_ANDROID_BASELINE_2021_SPEC_VERSION },
        VP_ANDROID_BASELINE_2021_MIN_API_VERSION,
        &VP_ANDROID_BASELINE_2021::_instanceExtensions[0], _vpArraySize(VP_ANDROID_BASELINE_2021::_instanceExtensions),
        &VP_ANDROID_BASELINE_2021::_deviceExtensions[0], _vpArraySize(VP_ANDROID_BASELINE_2021::_deviceExtensions),
        nullptr, 0,
        &VP_ANDROID_BASELINE_2021::_featureStructTypes[0], _vpArraySize(VP_ANDROID_BASELINE_2021::_featureStructTypes),
        VP_ANDROID_BASELINE_2021::_featureDesc,
        &VP_ANDROID_BASELINE_2021::_propertyStructTypes[0], _vpArraySize(VP_ANDROID_BASELINE_2021::_propertyStructTypes),
        VP_ANDROID_BASELINE_2021::_propertyDesc,
        nullptr, 0,
        nullptr, 0,
        &VP_ANDROID_BASELINE_2021::_formatStructTypes[0], _vpArraySize(VP_ANDROID_BASELINE_2021::_formatStructTypes),
        &VP_ANDROID_BASELINE_2021::_formatDesc[0], _vpArraySize(VP_ANDROID_BASELINE_2021::_formatDesc),
    },
#endif
#ifdef VP_KHR_roadmap_2022
    _vpProfileDesc{
        VpProfileProperties{ VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION },
        VP_KHR_ROADMAP_2022_MIN_API_VERSION,
        nullptr, 0,
        &VP_KHR_ROADMAP_2022::_deviceExtensions[0], _vpArraySize(VP_KHR_ROADMAP_2022::_deviceExtensions),
        nullptr, 0,
        &VP_KHR_ROADMAP_2022::_featureStructTypes[0], _vpArraySize(VP_KHR_ROADMAP_2022::_featureStructTypes),
        VP_KHR_ROADMAP_2022::_featureDesc,
        &VP_KHR_ROADMAP_2022::_propertyStructTypes[0], _vpArraySize(VP_KHR_ROADMAP_2022::_propertyStructTypes),
        VP_KHR_ROADMAP_2022::_propertyDesc,
        nullptr, 0,
        nullptr, 0,
        nullptr, 0,
        nullptr, 0,
    },
#endif
#ifdef VP_LUNARG_desktop_portability_2021_subset
    _vpProfileDesc{
        VpProfileProperties{ VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME, VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_SPEC_VERSION },
        VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_MIN_API_VERSION,
        nullptr, 0,
        &VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET::_deviceExtensions[0], _vpArraySize(VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET::_deviceExtensions),
        &VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET::_fallbacks[0], _vpArraySize(VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET::_fallbacks),
        &VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET::_featureStructTypes[0], _vpArraySize(VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET::_featureStructTypes),
        VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET::_featureDesc,
        &VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET::_propertyStructTypes[0], _vpArraySize(VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET::_propertyStructTypes),
        VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET::_propertyDesc,
        &VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET::_queueFamilyStructTypes[0], _vpArraySize(VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET::_queueFamilyStructTypes),
        &VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET::_queueFamilyDesc[0], _vpArraySize(VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET::_queueFamilyDesc),
        &VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET::_formatStructTypes[0], _vpArraySize(VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET::_formatStructTypes),
        &VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET::_formatDesc[0], _vpArraySize(VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET::_formatDesc),
    },
#endif
#ifdef VP_LUNARG_desktop_portability_2021
    _vpProfileDesc{
        VpProfileProperties{ VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, VP_LUNARG_DESKTOP_PORTABILITY_2021_SPEC_VERSION },
        VP_LUNARG_DESKTOP_PORTABILITY_2021_MIN_API_VERSION,
        nullptr, 0,
        &VP_LUNARG_DESKTOP_PORTABILITY_2021::_deviceExtensions[0], _vpArraySize(VP_LUNARG_DESKTOP_PORTABILITY_2021::_deviceExtensions),
        nullptr, 0,
        &VP_LUNARG_DESKTOP_PORTABILITY_2021::_featureStructTypes[0], _vpArraySize(VP_LUNARG_DESKTOP_PORTABILITY_2021::_featureStructTypes),
        VP_LUNARG_DESKTOP_PORTABILITY_2021::_featureDesc,
        &VP_LUNARG_DESKTOP_PORTABILITY_2021::_propertyStructTypes[0], _vpArraySize(VP_LUNARG_DESKTOP_PORTABILITY_2021::_propertyStructTypes),
        VP_LUNARG_DESKTOP_PORTABILITY_2021::_propertyDesc,
        &VP_LUNARG_DESKTOP_PORTABILITY_2021::_queueFamilyStructTypes[0], _vpArraySize(VP_LUNARG_DESKTOP_PORTABILITY_2021::_queueFamilyStructTypes),
        &VP_LUNARG_DESKTOP_PORTABILITY_2021::_queueFamilyDesc[0], _vpArraySize(VP_LUNARG_DESKTOP_PORTABILITY_2021::_queueFamilyDesc),
        &VP_LUNARG_DESKTOP_PORTABILITY_2021::_formatStructTypes[0], _vpArraySize(VP_LUNARG_DESKTOP_PORTABILITY_2021::_formatStructTypes),
        &VP_LUNARG_DESKTOP_PORTABILITY_2021::_formatDesc[0], _vpArraySize(VP_LUNARG_DESKTOP_PORTABILITY_2021::_formatDesc),
    },
#endif
};

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
        VkBaseOutStructure* p = reinterpret_cast<VkBaseOutStructure*>(pNext);
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
        VkBaseOutStructure* p = reinterpret_cast<VkBaseOutStructure*>(pNext);
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
            VkBaseOutStructure* p = reinterpret_cast<VkBaseOutStructure*>(&pProperties[i]);
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
            VkBaseOutStructure* p = reinterpret_cast<VkBaseOutStructure*>(pNext);
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
                pDesc->pFormats[i].pfnFiller(reinterpret_cast<VkBaseOutStructure*>(&fp));
                fp3->linearTilingFeatures = static_cast<VkFormatFeatureFlags2KHR>(fp3->linearTilingFeatures | fp.formatProperties.linearTilingFeatures);
                fp3->optimalTilingFeatures = static_cast<VkFormatFeatureFlags2KHR>(fp3->optimalTilingFeatures | fp.formatProperties.optimalTilingFeatures);
                fp3->bufferFeatures = static_cast<VkFormatFeatureFlags2KHR>(fp3->bufferFeatures | fp.formatProperties.bufferFeatures);
            }
            if (fp2 != nullptr) {
                VkFormatProperties3KHR fp{ VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3_KHR };
                pDesc->pFormats[i].pfnFiller(reinterpret_cast<VkBaseOutStructure*>(&fp));
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

VPAPI_ATTR VkResult vpGetPhysicalDeviceProfileSupport(VkPhysicalDevice physicalDevice, const VpProfileProperties *pProfile, VkBool32 *pSupported) {
    assert(pProfile != nullptr);
    assert(pSupported != nullptr);
    assert(physicalDevice != VK_NULL_HANDLE);

    VkResult result = VK_SUCCESS;

    uint32_t extCount;
    result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, nullptr);
    if (result != VK_SUCCESS) return result;
    std::vector<VkExtensionProperties> ext(extCount);
    result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, ext.data());
    if (result != VK_SUCCESS) return result;

    VkPhysicalDeviceProperties devProps;
    vkGetPhysicalDeviceProperties(physicalDevice, &devProps);

    *pSupported = VK_FALSE;

#ifdef VP_ANDROID_baseline_2021
    if (strcmp(pProfile->profileName, VP_ANDROID_BASELINE_2021_NAME) == 0) {
        if (VP_ANDROID_BASELINE_2021_SPEC_VERSION < pProfile->specVersion) return result;

        if (VK_VERSION_PATCH(devProps.apiVersion) < VK_VERSION_PATCH(VP_ANDROID_BASELINE_2021_MIN_API_VERSION)) return result;

        for (uint32_t i = 0; i < _vpArraySize(VP_ANDROID_BASELINE_2021::_deviceExtensions); ++i) {
            if (!_vpCheckExtension(ext.data(), ext.size(), VP_ANDROID_BASELINE_2021::_deviceExtensions[i].extensionName)) return result;
        }

        VkPhysicalDeviceFeatures2 physicalDeviceFeatures2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
        vkGetPhysicalDeviceFeatures2(physicalDevice, &physicalDeviceFeatures2);
        if (!VP_ANDROID_BASELINE_2021::_featureDesc.pfnComparator(reinterpret_cast<VkBaseOutStructure*>(&physicalDeviceFeatures2))) return result;

        VkPhysicalDeviceProperties2 physicalDeviceProperties2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
        vkGetPhysicalDeviceProperties2(physicalDevice, &physicalDeviceProperties2);
        if (!VP_ANDROID_BASELINE_2021::_propertyDesc.pfnComparator(reinterpret_cast<VkBaseOutStructure*>(&physicalDeviceProperties2))) return result;
    } else
#endif
#ifdef VP_KHR_roadmap_2022
    if (strcmp(pProfile->profileName, VP_KHR_ROADMAP_2022_NAME) == 0) {
        if (VP_KHR_ROADMAP_2022_SPEC_VERSION < pProfile->specVersion) return result;

        if (VK_VERSION_PATCH(devProps.apiVersion) < VK_VERSION_PATCH(VP_KHR_ROADMAP_2022_MIN_API_VERSION)) return result;

        for (uint32_t i = 0; i < _vpArraySize(VP_KHR_ROADMAP_2022::_deviceExtensions); ++i) {
            if (!_vpCheckExtension(ext.data(), ext.size(), VP_KHR_ROADMAP_2022::_deviceExtensions[i].extensionName)) return result;
        }

        VkPhysicalDeviceFeatures2 physicalDeviceFeatures2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
        VkPhysicalDeviceVulkan11Features physicalDeviceVulkan11Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES };
        VkPhysicalDeviceVulkan12Features physicalDeviceVulkan12Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
        physicalDeviceVulkan12Features.pNext = &physicalDeviceVulkan11Features;
        VkPhysicalDeviceVulkan13Features physicalDeviceVulkan13Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
        physicalDeviceVulkan13Features.pNext = &physicalDeviceVulkan12Features;
        physicalDeviceFeatures2.pNext = &physicalDeviceVulkan13Features;
        vkGetPhysicalDeviceFeatures2(physicalDevice, &physicalDeviceFeatures2);
        if (!VP_KHR_ROADMAP_2022::_featureDesc.pfnComparator(reinterpret_cast<VkBaseOutStructure*>(&physicalDeviceFeatures2))) return result;

        VkPhysicalDeviceVulkan11Properties physicalDeviceVulkan11Properties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES };
        VkPhysicalDeviceVulkan12Properties physicalDeviceVulkan12Properties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES };
        physicalDeviceVulkan12Properties.pNext = &physicalDeviceVulkan11Properties;
        VkPhysicalDeviceVulkan13Properties physicalDeviceVulkan13Properties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES };
        physicalDeviceVulkan13Properties.pNext = &physicalDeviceVulkan12Properties;
        VkPhysicalDeviceProperties2 physicalDeviceProperties2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
        physicalDeviceProperties2.pNext = &physicalDeviceVulkan13Properties;
        vkGetPhysicalDeviceProperties2(physicalDevice, &physicalDeviceProperties2);
        if (!VP_KHR_ROADMAP_2022::_propertyDesc.pfnComparator(reinterpret_cast<VkBaseOutStructure*>(&physicalDeviceProperties2))) return result;
    } else
#endif
#ifdef VP_LUNARG_desktop_portability_2021_subset
    if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME) == 0) {
        if (VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_SPEC_VERSION < pProfile->specVersion) return result;

        if (VK_VERSION_PATCH(devProps.apiVersion) < VK_VERSION_PATCH(VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_MIN_API_VERSION)) return result;

        for (uint32_t i = 0; i < _vpArraySize(VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET::_deviceExtensions); ++i) {
            if (!_vpCheckExtension(ext.data(), ext.size(), VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET::_deviceExtensions[i].extensionName)) return result;
        }

        VkPhysicalDeviceMultiviewFeatures physicalDeviceMultiviewFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES };
        VkPhysicalDeviceFeatures2 physicalDeviceFeatures2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
        VkPhysicalDeviceImagelessFramebufferFeaturesKHR physicalDeviceImagelessFramebufferFeaturesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR };
        physicalDeviceImagelessFramebufferFeaturesKHR.pNext = &physicalDeviceMultiviewFeatures;
        VkPhysicalDevice16BitStorageFeatures physicalDevice16BitStorageFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES };
        physicalDevice16BitStorageFeatures.pNext = &physicalDeviceImagelessFramebufferFeaturesKHR;
        VkPhysicalDevice8BitStorageFeaturesKHR physicalDevice8BitStorageFeaturesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR };
        physicalDevice8BitStorageFeaturesKHR.pNext = &physicalDevice16BitStorageFeatures;
        VkPhysicalDeviceDescriptorIndexingFeaturesEXT physicalDeviceDescriptorIndexingFeaturesEXT{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT };
        physicalDeviceDescriptorIndexingFeaturesEXT.pNext = &physicalDevice8BitStorageFeaturesKHR;
        VkPhysicalDeviceHostQueryResetFeaturesEXT physicalDeviceHostQueryResetFeaturesEXT{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT };
        physicalDeviceHostQueryResetFeaturesEXT.pNext = &physicalDeviceDescriptorIndexingFeaturesEXT;
        VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR physicalDeviceShaderSubgroupExtendedTypesFeaturesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR };
        physicalDeviceShaderSubgroupExtendedTypesFeaturesKHR.pNext = &physicalDeviceHostQueryResetFeaturesEXT;
        VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR physicalDeviceUniformBufferStandardLayoutFeaturesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR };
        physicalDeviceUniformBufferStandardLayoutFeaturesKHR.pNext = &physicalDeviceShaderSubgroupExtendedTypesFeaturesKHR;
        VkPhysicalDeviceShaderDrawParametersFeatures physicalDeviceShaderDrawParametersFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES };
        physicalDeviceShaderDrawParametersFeatures.pNext = &physicalDeviceUniformBufferStandardLayoutFeaturesKHR;
        VkPhysicalDeviceShaderFloat16Int8FeaturesKHR physicalDeviceShaderFloat16Int8FeaturesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT16_INT8_FEATURES_KHR };
        physicalDeviceShaderFloat16Int8FeaturesKHR.pNext = &physicalDeviceShaderDrawParametersFeatures;
        VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR physicalDeviceSamplerYcbcrConversionFeaturesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR };
        physicalDeviceSamplerYcbcrConversionFeaturesKHR.pNext = &physicalDeviceShaderFloat16Int8FeaturesKHR;
        VkPhysicalDeviceVariablePointersFeatures physicalDeviceVariablePointersFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES };
        physicalDeviceVariablePointersFeatures.pNext = &physicalDeviceSamplerYcbcrConversionFeaturesKHR;
        VkPhysicalDevicePortabilitySubsetFeaturesKHR physicalDevicePortabilitySubsetFeaturesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR };
        physicalDevicePortabilitySubsetFeaturesKHR.pNext = &physicalDeviceVariablePointersFeatures;
        physicalDeviceFeatures2.pNext = &physicalDevicePortabilitySubsetFeaturesKHR;
        vkGetPhysicalDeviceFeatures2(physicalDevice, &physicalDeviceFeatures2);
        if (!VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET::_featureDesc.pfnComparator(reinterpret_cast<VkBaseOutStructure*>(&physicalDeviceFeatures2))) return result;

        VkPhysicalDeviceMultiviewProperties physicalDeviceMultiviewProperties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES };
        VkPhysicalDeviceProperties2 physicalDeviceProperties2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
        VkPhysicalDeviceDescriptorIndexingPropertiesEXT physicalDeviceDescriptorIndexingPropertiesEXT{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT };
        physicalDeviceDescriptorIndexingPropertiesEXT.pNext = &physicalDeviceMultiviewProperties;
        VkPhysicalDeviceDepthStencilResolvePropertiesKHR physicalDeviceDepthStencilResolvePropertiesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR };
        physicalDeviceDepthStencilResolvePropertiesKHR.pNext = &physicalDeviceDescriptorIndexingPropertiesEXT;
        VkPhysicalDeviceMaintenance3Properties physicalDeviceMaintenance3Properties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES };
        physicalDeviceMaintenance3Properties.pNext = &physicalDeviceDepthStencilResolvePropertiesKHR;
        VkPhysicalDevicePortabilitySubsetPropertiesKHR physicalDevicePortabilitySubsetPropertiesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES_KHR };
        physicalDevicePortabilitySubsetPropertiesKHR.pNext = &physicalDeviceMaintenance3Properties;
        physicalDeviceProperties2.pNext = &physicalDevicePortabilitySubsetPropertiesKHR;
        vkGetPhysicalDeviceProperties2(physicalDevice, &physicalDeviceProperties2);
        if (!VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET::_propertyDesc.pfnComparator(reinterpret_cast<VkBaseOutStructure*>(&physicalDeviceProperties2))) return result;
    } else
#endif
#ifdef VP_LUNARG_desktop_portability_2021
    if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME) == 0) {
        if (VP_LUNARG_DESKTOP_PORTABILITY_2021_SPEC_VERSION < pProfile->specVersion) return result;

        if (VK_VERSION_PATCH(devProps.apiVersion) < VK_VERSION_PATCH(VP_LUNARG_DESKTOP_PORTABILITY_2021_MIN_API_VERSION)) return result;

        for (uint32_t i = 0; i < _vpArraySize(VP_LUNARG_DESKTOP_PORTABILITY_2021::_deviceExtensions); ++i) {
            if (!_vpCheckExtension(ext.data(), ext.size(), VP_LUNARG_DESKTOP_PORTABILITY_2021::_deviceExtensions[i].extensionName)) return result;
        }

        VkPhysicalDeviceMultiviewFeatures physicalDeviceMultiviewFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES };
        VkPhysicalDeviceFeatures2 physicalDeviceFeatures2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
        VkPhysicalDeviceImagelessFramebufferFeaturesKHR physicalDeviceImagelessFramebufferFeaturesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR };
        physicalDeviceImagelessFramebufferFeaturesKHR.pNext = &physicalDeviceMultiviewFeatures;
        VkPhysicalDevice16BitStorageFeatures physicalDevice16BitStorageFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES };
        physicalDevice16BitStorageFeatures.pNext = &physicalDeviceImagelessFramebufferFeaturesKHR;
        VkPhysicalDevice8BitStorageFeaturesKHR physicalDevice8BitStorageFeaturesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR };
        physicalDevice8BitStorageFeaturesKHR.pNext = &physicalDevice16BitStorageFeatures;
        VkPhysicalDeviceDescriptorIndexingFeaturesEXT physicalDeviceDescriptorIndexingFeaturesEXT{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT };
        physicalDeviceDescriptorIndexingFeaturesEXT.pNext = &physicalDevice8BitStorageFeaturesKHR;
        VkPhysicalDeviceHostQueryResetFeaturesEXT physicalDeviceHostQueryResetFeaturesEXT{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT };
        physicalDeviceHostQueryResetFeaturesEXT.pNext = &physicalDeviceDescriptorIndexingFeaturesEXT;
        VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR physicalDeviceShaderSubgroupExtendedTypesFeaturesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR };
        physicalDeviceShaderSubgroupExtendedTypesFeaturesKHR.pNext = &physicalDeviceHostQueryResetFeaturesEXT;
        VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR physicalDeviceUniformBufferStandardLayoutFeaturesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR };
        physicalDeviceUniformBufferStandardLayoutFeaturesKHR.pNext = &physicalDeviceShaderSubgroupExtendedTypesFeaturesKHR;
        VkPhysicalDeviceShaderDrawParametersFeatures physicalDeviceShaderDrawParametersFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES };
        physicalDeviceShaderDrawParametersFeatures.pNext = &physicalDeviceUniformBufferStandardLayoutFeaturesKHR;
        VkPhysicalDeviceShaderFloat16Int8FeaturesKHR physicalDeviceShaderFloat16Int8FeaturesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT16_INT8_FEATURES_KHR };
        physicalDeviceShaderFloat16Int8FeaturesKHR.pNext = &physicalDeviceShaderDrawParametersFeatures;
        VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR physicalDeviceSamplerYcbcrConversionFeaturesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR };
        physicalDeviceSamplerYcbcrConversionFeaturesKHR.pNext = &physicalDeviceShaderFloat16Int8FeaturesKHR;
        VkPhysicalDeviceVariablePointersFeatures physicalDeviceVariablePointersFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES };
        physicalDeviceVariablePointersFeatures.pNext = &physicalDeviceSamplerYcbcrConversionFeaturesKHR;
        physicalDeviceFeatures2.pNext = &physicalDeviceVariablePointersFeatures;
        vkGetPhysicalDeviceFeatures2(physicalDevice, &physicalDeviceFeatures2);
        if (!VP_LUNARG_DESKTOP_PORTABILITY_2021::_featureDesc.pfnComparator(reinterpret_cast<VkBaseOutStructure*>(&physicalDeviceFeatures2))) return result;

        VkPhysicalDeviceMultiviewProperties physicalDeviceMultiviewProperties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES };
        VkPhysicalDeviceProperties2 physicalDeviceProperties2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
        VkPhysicalDeviceDescriptorIndexingPropertiesEXT physicalDeviceDescriptorIndexingPropertiesEXT{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT };
        physicalDeviceDescriptorIndexingPropertiesEXT.pNext = &physicalDeviceMultiviewProperties;
        VkPhysicalDeviceDepthStencilResolvePropertiesKHR physicalDeviceDepthStencilResolvePropertiesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR };
        physicalDeviceDepthStencilResolvePropertiesKHR.pNext = &physicalDeviceDescriptorIndexingPropertiesEXT;
        VkPhysicalDeviceMaintenance3Properties physicalDeviceMaintenance3Properties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES };
        physicalDeviceMaintenance3Properties.pNext = &physicalDeviceDepthStencilResolvePropertiesKHR;
        physicalDeviceProperties2.pNext = &physicalDeviceMaintenance3Properties;
        vkGetPhysicalDeviceProperties2(physicalDevice, &physicalDeviceProperties2);
        if (!VP_LUNARG_DESKTOP_PORTABILITY_2021::_propertyDesc.pfnComparator(reinterpret_cast<VkBaseOutStructure*>(&physicalDeviceProperties2))) return result;
    } else
#endif
    {
        return result;
    }

    *pSupported = VK_TRUE;
    return result;
}

VPAPI_ATTR VkResult vpCreateDevice(VkPhysicalDevice physicalDevice, const VpDeviceCreateInfo *pCreateInfo,
                                   const VkAllocationCallbacks *pAllocator, VkDevice *pDevice) {
    assert(pCreateInfo != nullptr);

    if (physicalDevice == VK_NULL_HANDLE || pCreateInfo == nullptr || pDevice == nullptr) {
        return vkCreateDevice(physicalDevice, pCreateInfo == nullptr ? nullptr : pCreateInfo->pCreateInfo, pAllocator, pDevice);
    } else if (pCreateInfo->pProfile == nullptr || strcmp(pCreateInfo->pProfile->profileName, "") == 0) {
        return vkCreateDevice(physicalDevice, pCreateInfo->pCreateInfo, pAllocator, pDevice);
    } else
#ifdef VP_ANDROID_baseline_2021
    if (strcmp(pCreateInfo->pProfile->profileName, VP_ANDROID_BASELINE_2021_NAME) == 0) {
        std::vector<const char*> extensions;
        _vpGetDeviceExtensions(pCreateInfo, _vpArraySize(VP_ANDROID_BASELINE_2021::_deviceExtensions), &VP_ANDROID_BASELINE_2021::_deviceExtensions[0], extensions);

        void *pNext = const_cast<void*>(pCreateInfo->pCreateInfo->pNext);
        VkPhysicalDeviceFeatures2 profilePhysicalDeviceFeatures2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
        vpGetProfileFeatures(pCreateInfo->pProfile, &profilePhysicalDeviceFeatures2);
        if (pCreateInfo->pCreateInfo->pEnabledFeatures != nullptr) {
            profilePhysicalDeviceFeatures2.features = *pCreateInfo->pCreateInfo->pEnabledFeatures;
        }
        if (pCreateInfo->flags & VP_DEVICE_CREATE_DISABLE_ROBUST_BUFFER_ACCESS_BIT) {
            profilePhysicalDeviceFeatures2.features.robustBufferAccess = VK_FALSE;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2) == nullptr && pCreateInfo->pCreateInfo->pEnabledFeatures == nullptr) {
            profilePhysicalDeviceFeatures2.pNext = pNext;
            pNext = &profilePhysicalDeviceFeatures2;
        }

        VkDeviceCreateInfo deviceCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        deviceCreateInfo.pNext = pNext;
        deviceCreateInfo.flags = pCreateInfo->pCreateInfo->flags;
        deviceCreateInfo.queueCreateInfoCount = pCreateInfo->pCreateInfo->queueCreateInfoCount;
        deviceCreateInfo.pQueueCreateInfos = pCreateInfo->pCreateInfo->pQueueCreateInfos;
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = extensions.data();
        return vkCreateDevice(physicalDevice, &deviceCreateInfo, pAllocator, pDevice);
    } else
#endif
#ifdef VP_KHR_roadmap_2022
    if (strcmp(pCreateInfo->pProfile->profileName, VP_KHR_ROADMAP_2022_NAME) == 0) {
        std::vector<const char*> extensions;
        _vpGetDeviceExtensions(pCreateInfo, _vpArraySize(VP_KHR_ROADMAP_2022::_deviceExtensions), &VP_KHR_ROADMAP_2022::_deviceExtensions[0], extensions);

        void *pNext = const_cast<void*>(pCreateInfo->pCreateInfo->pNext);
        VkPhysicalDeviceFeatures2 profilePhysicalDeviceFeatures2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
        VkPhysicalDeviceVulkan11Features profilePhysicalDeviceVulkan11Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES };
        profilePhysicalDeviceVulkan11Features.pNext = &profilePhysicalDeviceFeatures2;
        VkPhysicalDeviceVulkan12Features profilePhysicalDeviceVulkan12Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
        profilePhysicalDeviceVulkan12Features.pNext = &profilePhysicalDeviceVulkan11Features;
        VkPhysicalDeviceVulkan13Features profilePhysicalDeviceVulkan13Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
        profilePhysicalDeviceVulkan13Features.pNext = &profilePhysicalDeviceVulkan12Features;
        vpGetProfileFeatures(pCreateInfo->pProfile, &profilePhysicalDeviceVulkan13Features);
        if (pCreateInfo->pCreateInfo->pEnabledFeatures != nullptr) {
            profilePhysicalDeviceFeatures2.features = *pCreateInfo->pCreateInfo->pEnabledFeatures;
        }
        if (pCreateInfo->flags & VP_DEVICE_CREATE_DISABLE_ROBUST_BUFFER_ACCESS_BIT) {
            profilePhysicalDeviceFeatures2.features.robustBufferAccess = VK_FALSE;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2) == nullptr && pCreateInfo->pCreateInfo->pEnabledFeatures == nullptr) {
            profilePhysicalDeviceFeatures2.pNext = pNext;
            pNext = &profilePhysicalDeviceFeatures2;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES) == nullptr) {
            profilePhysicalDeviceVulkan11Features.pNext = pNext;
            pNext = &profilePhysicalDeviceVulkan11Features;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES) == nullptr) {
            profilePhysicalDeviceVulkan12Features.pNext = pNext;
            pNext = &profilePhysicalDeviceVulkan12Features;
        }
        if (pCreateInfo->flags & VP_DEVICE_CREATE_DISABLE_ROBUST_IMAGE_ACCESS_BIT) {
            profilePhysicalDeviceVulkan13Features.robustImageAccess = VK_FALSE;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES) == nullptr) {
            profilePhysicalDeviceVulkan13Features.pNext = pNext;
            pNext = &profilePhysicalDeviceVulkan13Features;
        }

        VkDeviceCreateInfo deviceCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        deviceCreateInfo.pNext = pNext;
        deviceCreateInfo.flags = pCreateInfo->pCreateInfo->flags;
        deviceCreateInfo.queueCreateInfoCount = pCreateInfo->pCreateInfo->queueCreateInfoCount;
        deviceCreateInfo.pQueueCreateInfos = pCreateInfo->pCreateInfo->pQueueCreateInfos;
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = extensions.data();
        return vkCreateDevice(physicalDevice, &deviceCreateInfo, pAllocator, pDevice);
    } else
#endif
#ifdef VP_LUNARG_desktop_portability_2021_subset
    if (strcmp(pCreateInfo->pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME) == 0) {
        std::vector<const char*> extensions;
        _vpGetDeviceExtensions(pCreateInfo, _vpArraySize(VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET::_deviceExtensions), &VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET::_deviceExtensions[0], extensions);

        void *pNext = const_cast<void*>(pCreateInfo->pCreateInfo->pNext);
        VkPhysicalDeviceMultiviewFeatures profilePhysicalDeviceMultiviewFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES };
        VkPhysicalDeviceFeatures2 profilePhysicalDeviceFeatures2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
        profilePhysicalDeviceFeatures2.pNext = &profilePhysicalDeviceMultiviewFeatures;
        VkPhysicalDeviceImagelessFramebufferFeaturesKHR profilePhysicalDeviceImagelessFramebufferFeaturesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR };
        profilePhysicalDeviceImagelessFramebufferFeaturesKHR.pNext = &profilePhysicalDeviceFeatures2;
        VkPhysicalDevice16BitStorageFeatures profilePhysicalDevice16BitStorageFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES };
        profilePhysicalDevice16BitStorageFeatures.pNext = &profilePhysicalDeviceImagelessFramebufferFeaturesKHR;
        VkPhysicalDevice8BitStorageFeaturesKHR profilePhysicalDevice8BitStorageFeaturesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR };
        profilePhysicalDevice8BitStorageFeaturesKHR.pNext = &profilePhysicalDevice16BitStorageFeatures;
        VkPhysicalDeviceDescriptorIndexingFeaturesEXT profilePhysicalDeviceDescriptorIndexingFeaturesEXT{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT };
        profilePhysicalDeviceDescriptorIndexingFeaturesEXT.pNext = &profilePhysicalDevice8BitStorageFeaturesKHR;
        VkPhysicalDeviceHostQueryResetFeaturesEXT profilePhysicalDeviceHostQueryResetFeaturesEXT{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT };
        profilePhysicalDeviceHostQueryResetFeaturesEXT.pNext = &profilePhysicalDeviceDescriptorIndexingFeaturesEXT;
        VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR profilePhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR };
        profilePhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR.pNext = &profilePhysicalDeviceHostQueryResetFeaturesEXT;
        VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR profilePhysicalDeviceUniformBufferStandardLayoutFeaturesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR };
        profilePhysicalDeviceUniformBufferStandardLayoutFeaturesKHR.pNext = &profilePhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR;
        VkPhysicalDeviceShaderDrawParametersFeatures profilePhysicalDeviceShaderDrawParametersFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES };
        profilePhysicalDeviceShaderDrawParametersFeatures.pNext = &profilePhysicalDeviceUniformBufferStandardLayoutFeaturesKHR;
        VkPhysicalDeviceShaderFloat16Int8FeaturesKHR profilePhysicalDeviceShaderFloat16Int8FeaturesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT16_INT8_FEATURES_KHR };
        profilePhysicalDeviceShaderFloat16Int8FeaturesKHR.pNext = &profilePhysicalDeviceShaderDrawParametersFeatures;
        VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR profilePhysicalDeviceSamplerYcbcrConversionFeaturesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR };
        profilePhysicalDeviceSamplerYcbcrConversionFeaturesKHR.pNext = &profilePhysicalDeviceShaderFloat16Int8FeaturesKHR;
        VkPhysicalDeviceVariablePointersFeatures profilePhysicalDeviceVariablePointersFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES };
        profilePhysicalDeviceVariablePointersFeatures.pNext = &profilePhysicalDeviceSamplerYcbcrConversionFeaturesKHR;
        VkPhysicalDevicePortabilitySubsetFeaturesKHR profilePhysicalDevicePortabilitySubsetFeaturesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR };
        profilePhysicalDevicePortabilitySubsetFeaturesKHR.pNext = &profilePhysicalDeviceVariablePointersFeatures;
        vpGetProfileFeatures(pCreateInfo->pProfile, &profilePhysicalDevicePortabilitySubsetFeaturesKHR);
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES) == nullptr) {
            profilePhysicalDeviceMultiviewFeatures.pNext = pNext;
            pNext = &profilePhysicalDeviceMultiviewFeatures;
        }
        if (pCreateInfo->pCreateInfo->pEnabledFeatures != nullptr) {
            profilePhysicalDeviceFeatures2.features = *pCreateInfo->pCreateInfo->pEnabledFeatures;
        }
        if (pCreateInfo->flags & VP_DEVICE_CREATE_DISABLE_ROBUST_BUFFER_ACCESS_BIT) {
            profilePhysicalDeviceFeatures2.features.robustBufferAccess = VK_FALSE;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2) == nullptr && pCreateInfo->pCreateInfo->pEnabledFeatures == nullptr) {
            profilePhysicalDeviceFeatures2.pNext = pNext;
            pNext = &profilePhysicalDeviceFeatures2;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR) == nullptr) {
            profilePhysicalDeviceImagelessFramebufferFeaturesKHR.pNext = pNext;
            pNext = &profilePhysicalDeviceImagelessFramebufferFeaturesKHR;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES) == nullptr) {
            profilePhysicalDevice16BitStorageFeatures.pNext = pNext;
            pNext = &profilePhysicalDevice16BitStorageFeatures;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR) == nullptr) {
            profilePhysicalDevice8BitStorageFeaturesKHR.pNext = pNext;
            pNext = &profilePhysicalDevice8BitStorageFeaturesKHR;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT) == nullptr) {
            profilePhysicalDeviceDescriptorIndexingFeaturesEXT.pNext = pNext;
            pNext = &profilePhysicalDeviceDescriptorIndexingFeaturesEXT;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT) == nullptr) {
            profilePhysicalDeviceHostQueryResetFeaturesEXT.pNext = pNext;
            pNext = &profilePhysicalDeviceHostQueryResetFeaturesEXT;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR) == nullptr) {
            profilePhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR.pNext = pNext;
            pNext = &profilePhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR) == nullptr) {
            profilePhysicalDeviceUniformBufferStandardLayoutFeaturesKHR.pNext = pNext;
            pNext = &profilePhysicalDeviceUniformBufferStandardLayoutFeaturesKHR;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES) == nullptr) {
            profilePhysicalDeviceShaderDrawParametersFeatures.pNext = pNext;
            pNext = &profilePhysicalDeviceShaderDrawParametersFeatures;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT16_INT8_FEATURES_KHR) == nullptr) {
            profilePhysicalDeviceShaderFloat16Int8FeaturesKHR.pNext = pNext;
            pNext = &profilePhysicalDeviceShaderFloat16Int8FeaturesKHR;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR) == nullptr) {
            profilePhysicalDeviceSamplerYcbcrConversionFeaturesKHR.pNext = pNext;
            pNext = &profilePhysicalDeviceSamplerYcbcrConversionFeaturesKHR;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES) == nullptr) {
            profilePhysicalDeviceVariablePointersFeatures.pNext = pNext;
            pNext = &profilePhysicalDeviceVariablePointersFeatures;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR) == nullptr) {
            profilePhysicalDevicePortabilitySubsetFeaturesKHR.pNext = pNext;
            pNext = &profilePhysicalDevicePortabilitySubsetFeaturesKHR;
        }

        VkDeviceCreateInfo deviceCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        deviceCreateInfo.pNext = pNext;
        deviceCreateInfo.flags = pCreateInfo->pCreateInfo->flags;
        deviceCreateInfo.queueCreateInfoCount = pCreateInfo->pCreateInfo->queueCreateInfoCount;
        deviceCreateInfo.pQueueCreateInfos = pCreateInfo->pCreateInfo->pQueueCreateInfos;
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = extensions.data();
        return vkCreateDevice(physicalDevice, &deviceCreateInfo, pAllocator, pDevice);
    } else
#endif
#ifdef VP_LUNARG_desktop_portability_2021
    if (strcmp(pCreateInfo->pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME) == 0) {
        std::vector<const char*> extensions;
        _vpGetDeviceExtensions(pCreateInfo, _vpArraySize(VP_LUNARG_DESKTOP_PORTABILITY_2021::_deviceExtensions), &VP_LUNARG_DESKTOP_PORTABILITY_2021::_deviceExtensions[0], extensions);

        void *pNext = const_cast<void*>(pCreateInfo->pCreateInfo->pNext);
        VkPhysicalDeviceMultiviewFeatures profilePhysicalDeviceMultiviewFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES };
        VkPhysicalDeviceFeatures2 profilePhysicalDeviceFeatures2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
        profilePhysicalDeviceFeatures2.pNext = &profilePhysicalDeviceMultiviewFeatures;
        VkPhysicalDeviceImagelessFramebufferFeaturesKHR profilePhysicalDeviceImagelessFramebufferFeaturesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR };
        profilePhysicalDeviceImagelessFramebufferFeaturesKHR.pNext = &profilePhysicalDeviceFeatures2;
        VkPhysicalDevice16BitStorageFeatures profilePhysicalDevice16BitStorageFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES };
        profilePhysicalDevice16BitStorageFeatures.pNext = &profilePhysicalDeviceImagelessFramebufferFeaturesKHR;
        VkPhysicalDevice8BitStorageFeaturesKHR profilePhysicalDevice8BitStorageFeaturesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR };
        profilePhysicalDevice8BitStorageFeaturesKHR.pNext = &profilePhysicalDevice16BitStorageFeatures;
        VkPhysicalDeviceDescriptorIndexingFeaturesEXT profilePhysicalDeviceDescriptorIndexingFeaturesEXT{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT };
        profilePhysicalDeviceDescriptorIndexingFeaturesEXT.pNext = &profilePhysicalDevice8BitStorageFeaturesKHR;
        VkPhysicalDeviceHostQueryResetFeaturesEXT profilePhysicalDeviceHostQueryResetFeaturesEXT{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT };
        profilePhysicalDeviceHostQueryResetFeaturesEXT.pNext = &profilePhysicalDeviceDescriptorIndexingFeaturesEXT;
        VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR profilePhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR };
        profilePhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR.pNext = &profilePhysicalDeviceHostQueryResetFeaturesEXT;
        VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR profilePhysicalDeviceUniformBufferStandardLayoutFeaturesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR };
        profilePhysicalDeviceUniformBufferStandardLayoutFeaturesKHR.pNext = &profilePhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR;
        VkPhysicalDeviceShaderDrawParametersFeatures profilePhysicalDeviceShaderDrawParametersFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES };
        profilePhysicalDeviceShaderDrawParametersFeatures.pNext = &profilePhysicalDeviceUniformBufferStandardLayoutFeaturesKHR;
        VkPhysicalDeviceShaderFloat16Int8FeaturesKHR profilePhysicalDeviceShaderFloat16Int8FeaturesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT16_INT8_FEATURES_KHR };
        profilePhysicalDeviceShaderFloat16Int8FeaturesKHR.pNext = &profilePhysicalDeviceShaderDrawParametersFeatures;
        VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR profilePhysicalDeviceSamplerYcbcrConversionFeaturesKHR{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR };
        profilePhysicalDeviceSamplerYcbcrConversionFeaturesKHR.pNext = &profilePhysicalDeviceShaderFloat16Int8FeaturesKHR;
        VkPhysicalDeviceVariablePointersFeatures profilePhysicalDeviceVariablePointersFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES };
        profilePhysicalDeviceVariablePointersFeatures.pNext = &profilePhysicalDeviceSamplerYcbcrConversionFeaturesKHR;
        vpGetProfileFeatures(pCreateInfo->pProfile, &profilePhysicalDeviceVariablePointersFeatures);
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES) == nullptr) {
            profilePhysicalDeviceMultiviewFeatures.pNext = pNext;
            pNext = &profilePhysicalDeviceMultiviewFeatures;
        }
        if (pCreateInfo->pCreateInfo->pEnabledFeatures != nullptr) {
            profilePhysicalDeviceFeatures2.features = *pCreateInfo->pCreateInfo->pEnabledFeatures;
        }
        if (pCreateInfo->flags & VP_DEVICE_CREATE_DISABLE_ROBUST_BUFFER_ACCESS_BIT) {
            profilePhysicalDeviceFeatures2.features.robustBufferAccess = VK_FALSE;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2) == nullptr && pCreateInfo->pCreateInfo->pEnabledFeatures == nullptr) {
            profilePhysicalDeviceFeatures2.pNext = pNext;
            pNext = &profilePhysicalDeviceFeatures2;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR) == nullptr) {
            profilePhysicalDeviceImagelessFramebufferFeaturesKHR.pNext = pNext;
            pNext = &profilePhysicalDeviceImagelessFramebufferFeaturesKHR;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES) == nullptr) {
            profilePhysicalDevice16BitStorageFeatures.pNext = pNext;
            pNext = &profilePhysicalDevice16BitStorageFeatures;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR) == nullptr) {
            profilePhysicalDevice8BitStorageFeaturesKHR.pNext = pNext;
            pNext = &profilePhysicalDevice8BitStorageFeaturesKHR;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT) == nullptr) {
            profilePhysicalDeviceDescriptorIndexingFeaturesEXT.pNext = pNext;
            pNext = &profilePhysicalDeviceDescriptorIndexingFeaturesEXT;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT) == nullptr) {
            profilePhysicalDeviceHostQueryResetFeaturesEXT.pNext = pNext;
            pNext = &profilePhysicalDeviceHostQueryResetFeaturesEXT;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR) == nullptr) {
            profilePhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR.pNext = pNext;
            pNext = &profilePhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR) == nullptr) {
            profilePhysicalDeviceUniformBufferStandardLayoutFeaturesKHR.pNext = pNext;
            pNext = &profilePhysicalDeviceUniformBufferStandardLayoutFeaturesKHR;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES) == nullptr) {
            profilePhysicalDeviceShaderDrawParametersFeatures.pNext = pNext;
            pNext = &profilePhysicalDeviceShaderDrawParametersFeatures;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT16_INT8_FEATURES_KHR) == nullptr) {
            profilePhysicalDeviceShaderFloat16Int8FeaturesKHR.pNext = pNext;
            pNext = &profilePhysicalDeviceShaderFloat16Int8FeaturesKHR;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR) == nullptr) {
            profilePhysicalDeviceSamplerYcbcrConversionFeaturesKHR.pNext = pNext;
            pNext = &profilePhysicalDeviceSamplerYcbcrConversionFeaturesKHR;
        }
        if (_vpGetStructure(pNext, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES) == nullptr) {
            profilePhysicalDeviceVariablePointersFeatures.pNext = pNext;
            pNext = &profilePhysicalDeviceVariablePointersFeatures;
        }

        VkDeviceCreateInfo deviceCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        deviceCreateInfo.pNext = pNext;
        deviceCreateInfo.flags = pCreateInfo->pCreateInfo->flags;
        deviceCreateInfo.queueCreateInfoCount = pCreateInfo->pCreateInfo->queueCreateInfoCount;
        deviceCreateInfo.pQueueCreateInfos = pCreateInfo->pCreateInfo->pQueueCreateInfos;
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = extensions.data();
        return vkCreateDevice(physicalDevice, &deviceCreateInfo, pAllocator, pDevice);
    } else
#endif
    {
        return VK_ERROR_UNKNOWN;
    }
}

#endif //VULKAN_PROFILES_
