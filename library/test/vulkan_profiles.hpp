
/*
 * Copyright (c) 2021-2022 LunarG, Inc.",
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
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

#include <vulkan/vulkan_core.h>

#define VP_MAX_PROFILE_NAME_SIZE 256U

#define VP_KHR_1_0_minimum_requirements 1
#define VP_KHR_1_0_MINIMUM_REQUIREMENTS_SPEC_VERSION 1
#define VP_KHR_1_0_MINIMUM_REQUIREMENTS_NAME "VP_KHR_1_0_minimum_requirements"

#define VP_KHR_1_2_roadmap_2022 1
#define VP_KHR_1_2_ROADMAP_2022_SPEC_VERSION 1
#define VP_KHR_1_2_ROADMAP_2022_NAME "VP_KHR_1_2_roadmap_2022"

#define VP_LUNARG_1_0_desktop_portability_2022 1
#define VP_LUNARG_1_0_DESKTOP_PORTABILITY_2022_SPEC_VERSION 1
#define VP_LUNARG_1_0_DESKTOP_PORTABILITY_2022_NAME "VP_LUNARG_1_0_desktop_portability_2022"

#define VP_LUNARG_1_2_desktop_portability_2022 1
#define VP_LUNARG_1_2_DESKTOP_PORTABILITY_2022_SPEC_VERSION 1
#define VP_LUNARG_1_2_DESKTOP_PORTABILITY_2022_NAME "VP_LUNARG_1_2_desktop_portability_2022"

#define VP_LUNARG_1_2_raytracing_2022 1
#define VP_LUNARG_1_2_RAYTRACING_2022_SPEC_VERSION 1
#define VP_LUNARG_1_2_RAYTRACING_2022_NAME "VP_LUNARG_1_2_raytracing_2022"

typedef struct VpProfileProperties {
    char profileName[VP_MAX_PROFILE_NAME_SIZE];
    uint32_t specVersion;
} VpProfileProperties;

VkResult vpCreateDevice(VkPhysicalDevice physicalDevice, const VpProfileProperties *pProfile, const VkDeviceCreateInfo *pCreateInfo,
                        const VkAllocationCallbacks *pAllocator, VkDevice *pDevice);

VkResult vpEnumerateDeviceProfiles(VkPhysicalDevice physicalDevice, const char *pLayerName, uint32_t *pPropertyCount,
                                   VpProfileProperties *pProperties);

// Implementation details:
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

inline bool isListed(const std::vector<const char *> &extensions, const char *extensionName) {
    for (std::size_t i = 0, n = extensions.size(); i < n; ++i) {
        if (extensions[i] == std::string(extensionName)) return true;
    }

    return false;
}

inline bool checkExtensionProperties(const std::vector<VkExtensionProperties> &requestedExts,
                                     const std::vector<VkExtensionProperties> &supportedExts) {
    std::map<std::string, uint32_t> propertiesMap;
    for (const auto &ext : supportedExts) {
        propertiesMap[std::string(ext.extensionName)] = ext.specVersion;
    }

    bool extsSupported = true;
    for (const auto &reqExt : requestedExts) {
        auto ext = std::string(reqExt.extensionName);
        if (propertiesMap.count(ext) == 0) {
            extsSupported = false;
        } else if (propertiesMap.at(ext) < reqExt.specVersion) {
            extsSupported = false;
        }
    }
    return extsSupported;
}

inline VkResult vpCreateDevice(VkPhysicalDevice physicalDevice, const VpProfileProperties *pProfile,
                               const VkDeviceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDevice *pDevice) {
    if (pProfile == nullptr) {
        return vkCreateDevice(physicalDevice, pCreateInfo, pAllocator, pDevice);
    } else if (strcmp(pProfile->profileName, VP_KHR_1_0_MINIMUM_REQUIREMENTS_NAME) == 0) {
        return vkCreateDevice(physicalDevice, pCreateInfo, pAllocator, pDevice);
    } else if (strcmp(pProfile->profileName, VP_KHR_1_2_ROADMAP_2022_NAME) == 0) {
        std::vector<const char *> extensions = {"VK_EXT_extended_dynamic_state2",
                                                "VK_EXT_texel_buffer_alignment",
                                                "VK_EXT_subgroup_size_control",
                                                "VK_EXT_shader_demote_to_helper_invocation",
                                                "VK_EXT_private_data",
                                                "VK_EXT_pipeline_creation_cache_control",
                                                "VK_EXT_inline_uniform_block",
                                                "VK_EXT_image_robustness",
                                                "VK_KHR_zero_initialize_workgroup_memory",
                                                "VK_KHR_synchronization2",
                                                "VK_KHR_shader_terminate_invocation",
                                                "VK_KHR_imageless_framebuffer"};

        for (uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; ++i) {
            if (isListed(extensions, pCreateInfo->ppEnabledExtensionNames[i])) {
                continue;
            }
            extensions.push_back(pCreateInfo->ppEnabledExtensionNames[i]);
        }

        VkPhysicalDeviceFeatures enabledFeatures = {};
        if (pCreateInfo->pEnabledFeatures != nullptr) {
            enabledFeatures = *pCreateInfo->pEnabledFeatures;
        }

        enabledFeatures.robustBufferAccess = VK_TRUE;
        enabledFeatures.imageCubeArray = VK_TRUE;
        enabledFeatures.independentBlend = VK_TRUE;
        enabledFeatures.sampleRateShading = VK_TRUE;
        enabledFeatures.drawIndirectFirstInstance = VK_TRUE;
        enabledFeatures.depthBiasClamp = VK_TRUE;
        enabledFeatures.samplerAnisotropy = VK_TRUE;
        enabledFeatures.occlusionQueryPrecise = VK_TRUE;
        enabledFeatures.fragmentStoresAndAtomics = VK_TRUE;
        enabledFeatures.shaderStorageImageExtendedFormats = VK_TRUE;
        enabledFeatures.shaderStorageImageReadWithoutFormat = VK_TRUE;
        enabledFeatures.shaderUniformBufferArrayDynamicIndexing = VK_TRUE;
        enabledFeatures.shaderSampledImageArrayDynamicIndexing = VK_TRUE;
        enabledFeatures.shaderStorageBufferArrayDynamicIndexing = VK_TRUE;
        enabledFeatures.shaderStorageImageArrayDynamicIndexing = VK_TRUE;

        VkPhysicalDeviceVulkan11Features pNext_0 = {};
        pNext_0.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
        pNext_0.pNext = const_cast<void *>(pCreateInfo->pNext);
        pNext_0.multiview = VK_TRUE;
        pNext_0.samplerYcbcrConversion = VK_TRUE;

        VkPhysicalDeviceVulkan12Features pNext_1 = {};
        pNext_1.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        pNext_1.pNext = &pNext_0;
        pNext_1.samplerMirrorClampToEdge = VK_TRUE;
        pNext_1.shaderUniformTexelBufferArrayDynamicIndexing = VK_TRUE;
        pNext_1.uniformBufferStandardLayout = VK_TRUE;
        pNext_1.shaderSubgroupExtendedTypes = VK_TRUE;
        pNext_1.separateDepthStencilLayouts = VK_TRUE;
        pNext_1.hostQueryReset = VK_TRUE;
        pNext_1.bufferDeviceAddress = VK_TRUE;
        pNext_1.vulkanMemoryModel = VK_TRUE;
        pNext_1.vulkanMemoryModelDeviceScope = VK_TRUE;
        pNext_1.vulkanMemoryModelAvailabilityVisibilityChains = VK_TRUE;
        pNext_1.subgroupBroadcastDynamicId = VK_TRUE;

        VkPhysicalDeviceImagelessFramebufferFeaturesKHR pNext_2 = {};
        pNext_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES;
        pNext_2.pNext = &pNext_1;
        pNext_2.imagelessFramebuffer = true;

        VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR pNext_3 = {};
        pNext_3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES_KHR;
        pNext_3.pNext = &pNext_2;
        pNext_3.shaderTerminateInvocation = true;

        VkPhysicalDeviceSynchronization2FeaturesKHR pNext_4 = {};
        pNext_4.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR;
        pNext_4.pNext = &pNext_3;
        pNext_4.synchronization2 = true;

        VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR pNext_5 = {};
        pNext_5.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_WORKGROUP_MEMORY_FEATURES_KHR;
        pNext_5.pNext = &pNext_4;
        pNext_5.shaderZeroInitializeWorkgroupMemory = VK_TRUE;

        VkPhysicalDeviceImageRobustnessFeaturesEXT pNext_6 = {};
        pNext_6.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES_EXT;
        pNext_6.pNext = &pNext_5;
        pNext_6.robustImageAccess = VK_TRUE;

        VkPhysicalDeviceInlineUniformBlockFeaturesEXT pNext_7 = {};
        pNext_7.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT;
        pNext_7.pNext = &pNext_6;
        pNext_7.inlineUniformBlock = VK_TRUE;

        VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT pNext_8 = {};
        pNext_8.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT;
        pNext_8.pNext = &pNext_7;
        pNext_8.pipelineCreationCacheControl = VK_TRUE;

        VkPhysicalDevicePrivateDataFeaturesEXT pNext_9 = {};
        pNext_9.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT;
        pNext_9.pNext = &pNext_8;
        pNext_9.privateData = VK_TRUE;

        VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT pNext_10 = {};
        pNext_10.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT;
        pNext_10.pNext = &pNext_9;
        pNext_10.shaderDemoteToHelperInvocation = VK_TRUE;

        VkPhysicalDeviceSubgroupSizeControlFeaturesEXT pNext_11 = {};
        pNext_11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT;
        pNext_11.pNext = &pNext_10;
        pNext_11.subgroupSizeControl = VK_TRUE;
        pNext_11.computeFullSubgroups = VK_TRUE;

        VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT pNext_12 = {};
        pNext_12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT;
        pNext_12.pNext = &pNext_11;
        pNext_12.texelBufferAlignment = VK_TRUE;

        VkPhysicalDeviceExtendedDynamicState2FeaturesEXT pNext_13 = {};
        pNext_13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT;
        pNext_13.pNext = &pNext_12;
        pNext_13.extendedDynamicState2 = VK_TRUE;

        void *pNext = &pNext_13;

        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pNext = pNext;
        deviceCreateInfo.queueCreateInfoCount = pCreateInfo->queueCreateInfoCount;
        deviceCreateInfo.pQueueCreateInfos = pCreateInfo->pQueueCreateInfos;
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = static_cast<const char *const *>(extensions.data());
        deviceCreateInfo.pEnabledFeatures = &enabledFeatures;
        return vkCreateDevice(physicalDevice, &deviceCreateInfo, pAllocator, pDevice);
    } else if (strcmp(pProfile->profileName, VP_LUNARG_1_0_DESKTOP_PORTABILITY_2022_NAME) == 0) {
        // TODO

        return vkCreateDevice(physicalDevice, pCreateInfo, pAllocator, pDevice);
    } else if (strcmp(pProfile->profileName, VP_LUNARG_1_2_DESKTOP_PORTABILITY_2022_NAME) == 0) {
        // TODO

        return vkCreateDevice(physicalDevice, pCreateInfo, pAllocator, pDevice);
    } else {
        return VK_ERROR_UNKNOWN;
    }
}

inline VkResult vpEnumerateDeviceProfiles(VkPhysicalDevice physicalDevice, const char *pLayerName, uint32_t *pPropertyCount,
                                          VpProfileProperties *pProperties) {
    VkResult result = VK_SUCCESS;

    uint32_t extensionPropertyCount;
    result = vkEnumerateInstanceExtensionProperties(pLayerName, &extensionPropertyCount, nullptr);
    std::vector<VkExtensionProperties> extensionProperties(extensionPropertyCount);
    result = vkEnumerateInstanceExtensionProperties(pLayerName, &extensionPropertyCount, extensionProperties.data());

    uint32_t deviceExtensionCount;
    result = vkEnumerateDeviceExtensionProperties(physicalDevice, pLayerName, &deviceExtensionCount, nullptr);
    std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionCount);
    result = vkEnumerateDeviceExtensionProperties(physicalDevice, pLayerName, &deviceExtensionCount, deviceExtensions.data());

    uint32_t supported_profile_count = 0;

    const VpProfileProperties supportedProfiles[] = {
        VpProfileProperties{VP_KHR_1_0_MINIMUM_REQUIREMENTS_NAME, VP_KHR_1_0_MINIMUM_REQUIREMENTS_SPEC_VERSION},
        VpProfileProperties{VP_KHR_1_2_ROADMAP_2022_NAME, VP_KHR_1_2_ROADMAP_2022_SPEC_VERSION},
        VpProfileProperties{VP_LUNARG_1_0_DESKTOP_PORTABILITY_2022_NAME, VP_LUNARG_1_0_DESKTOP_PORTABILITY_2022_SPEC_VERSION},
        VpProfileProperties{VP_LUNARG_1_2_DESKTOP_PORTABILITY_2022_NAME, VP_LUNARG_1_2_DESKTOP_PORTABILITY_2022_SPEC_VERSION}};

    for (int i = 0, n = sizeof(supportedProfiles) / sizeof(supportedProfiles[0]); i < n; ++i) {
        VkBool32 supported = VK_TRUE;

        if (strcmp(supportedProfiles[i].profileName, VP_KHR_1_0_MINIMUM_REQUIREMENTS_NAME) == 0) {

        } else if (strcmp(supportedProfiles[i].profileName, VP_KHR_1_2_ROADMAP_2022_NAME) == 0) {
            std::vector<VkExtensionProperties> profileExts = {VkExtensionProperties{"VK_EXT_extended_dynamic_state2", 1},
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
            if (checkExtensionProperties(profileExts, extensionProperties)) {
                supported = VK_FALSE;
            }

            VkPhysicalDeviceExtendedDynamicState2FeaturesEXT deviceExtendedDynamicState = {};
            deviceExtendedDynamicState.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT;
            deviceExtendedDynamicState.pNext = nullptr;

            VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT deviceTexelBufferAlign = {};
            deviceTexelBufferAlign.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT;
            deviceTexelBufferAlign.pNext = &deviceExtendedDynamicState;

            VkPhysicalDeviceSubgroupSizeControlFeaturesEXT deviceSubgroupSize = {};
            deviceSubgroupSize.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT;
            deviceSubgroupSize.pNext = &deviceTexelBufferAlign;

            VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT deviceShaderDemote = {};
            deviceShaderDemote.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT;
            deviceShaderDemote.pNext = &deviceSubgroupSize;

            VkPhysicalDevicePrivateDataFeaturesEXT devicePrivateData = {};
            devicePrivateData.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT;
            devicePrivateData.pNext = &deviceShaderDemote;

            VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT devicePipelineCreation = {};
            devicePipelineCreation.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT;
            devicePipelineCreation.pNext = &devicePrivateData;

            VkPhysicalDeviceInlineUniformBlockFeaturesEXT deviceInlineUniformBlock = {};
            deviceInlineUniformBlock.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT;
            deviceInlineUniformBlock.pNext = &devicePipelineCreation;

            VkPhysicalDeviceImageRobustnessFeaturesEXT deviceImageRobustness = {};
            deviceImageRobustness.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES_EXT;
            deviceImageRobustness.pNext = &deviceInlineUniformBlock;

            VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR deviceZeroInitialize = {};
            deviceZeroInitialize.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_WORKGROUP_MEMORY_FEATURES_KHR;
            deviceZeroInitialize.pNext = &deviceImageRobustness;

            VkPhysicalDeviceSynchronization2FeaturesKHR deviceSynchronization2 = {};
            deviceSynchronization2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR;
            deviceSynchronization2.pNext = &deviceZeroInitialize;

            VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR deviceShaderTerminateInvocation = {};
            deviceShaderTerminateInvocation.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES_KHR;
            deviceShaderTerminateInvocation.pNext = &deviceSynchronization2;

            VkPhysicalDeviceImagelessFramebufferFeaturesKHR deviceImagelessFramebufferFeatures = {};
            deviceImagelessFramebufferFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES;
            deviceImagelessFramebufferFeatures.pNext = &deviceShaderTerminateInvocation;

            VkPhysicalDeviceVulkan12Features deviceFeatures12 = {};
            deviceFeatures12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
            deviceFeatures12.pNext = &deviceImagelessFramebufferFeatures;

            VkPhysicalDeviceVulkan11Features deviceFeatures11 = {};
            deviceFeatures11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
            deviceFeatures11.pNext = &deviceFeatures12;

            VkPhysicalDeviceFeatures2 deviceFeatures = {};
            deviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            deviceFeatures.pNext = &deviceFeatures11;

            vkGetPhysicalDeviceFeatures2(physicalDevice, &deviceFeatures);

            if (deviceFeatures.features.robustBufferAccess != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceFeatures.features.imageCubeArray != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceFeatures.features.independentBlend != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceFeatures.features.sampleRateShading != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceFeatures.features.drawIndirectFirstInstance != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceFeatures.features.depthBiasClamp != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceFeatures.features.samplerAnisotropy != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceFeatures.features.occlusionQueryPrecise != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceFeatures.features.fragmentStoresAndAtomics != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceFeatures.features.shaderStorageImageExtendedFormats != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceFeatures.features.shaderStorageImageReadWithoutFormat != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceFeatures.features.shaderUniformBufferArrayDynamicIndexing != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceFeatures.features.shaderSampledImageArrayDynamicIndexing != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceFeatures.features.shaderStorageBufferArrayDynamicIndexing != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceFeatures.features.shaderStorageImageArrayDynamicIndexing != VK_TRUE) {
                supported = VK_FALSE;
            }

            if (deviceFeatures11.multiview != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceFeatures11.samplerYcbcrConversion != VK_TRUE) {
                supported = VK_FALSE;
            }

            if (deviceFeatures12.samplerMirrorClampToEdge != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceFeatures12.shaderUniformTexelBufferArrayDynamicIndexing != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceFeatures12.uniformBufferStandardLayout != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceFeatures12.shaderSubgroupExtendedTypes != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceFeatures12.separateDepthStencilLayouts != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceFeatures12.hostQueryReset != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceFeatures12.bufferDeviceAddress != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceFeatures12.vulkanMemoryModel != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceFeatures12.vulkanMemoryModelDeviceScope != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceFeatures12.vulkanMemoryModelAvailabilityVisibilityChains != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceFeatures12.subgroupBroadcastDynamicId != VK_TRUE) {
                supported = VK_FALSE;
            }

            if (deviceImagelessFramebufferFeatures.imagelessFramebuffer != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceShaderTerminateInvocation.shaderTerminateInvocation != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceSynchronization2.synchronization2 != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceZeroInitialize.shaderZeroInitializeWorkgroupMemory != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceImageRobustness.robustImageAccess != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceInlineUniformBlock.inlineUniformBlock != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (devicePipelineCreation.pipelineCreationCacheControl != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (devicePrivateData.privateData != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceShaderDemote.shaderDemoteToHelperInvocation != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceSubgroupSize.subgroupSizeControl != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceSubgroupSize.computeFullSubgroups != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceTexelBufferAlign.texelBufferAlignment != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (deviceExtendedDynamicState.extendedDynamicState2 != VK_TRUE) {
                supported = VK_FALSE;
            }

            VkPhysicalDeviceVulkan12Properties devicePropertiesVulkan12 = {};
            devicePropertiesVulkan12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;
            devicePropertiesVulkan12.pNext = nullptr;

            VkPhysicalDeviceVulkan11Properties devicePropertiesVulkan11 = {};
            devicePropertiesVulkan11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
            devicePropertiesVulkan11.pNext = &devicePropertiesVulkan12;

            VkPhysicalDeviceProperties2 deviceProperties{};
            deviceProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR;
            deviceProperties.pNext = &devicePropertiesVulkan11;

            vkGetPhysicalDeviceProperties2(physicalDevice, &deviceProperties);

            if (deviceProperties.properties.limits.maxImageDimension1D < 8192) {
                supported = VK_FALSE;
            }
            if (deviceProperties.properties.limits.maxImageDimension2D < 8192) {
                supported = VK_FALSE;
            }
            if (deviceProperties.properties.limits.maxImageDimensionCube < 8192) {
                supported = VK_FALSE;
            }
            if (deviceProperties.properties.limits.maxImageArrayLayers < 2048) {
                supported = VK_FALSE;
            }
            if (deviceProperties.properties.limits.maxUniformBufferRange < 65536) {
                supported = VK_FALSE;
            }
            if (deviceProperties.properties.limits.bufferImageGranularity > 4096) {
                supported = VK_FALSE;
            }

            if (deviceProperties.properties.limits.maxPerStageDescriptorSamplers < 64) {
                supported = VK_FALSE;
            }
            if (deviceProperties.properties.limits.maxPerStageDescriptorUniformBuffers < 15) {
                supported = VK_FALSE;
            }
            if (deviceProperties.properties.limits.maxPerStageDescriptorStorageBuffers < 30) {
                supported = VK_FALSE;
            }
            if (deviceProperties.properties.limits.maxPerStageDescriptorSampledImages < 200) {
                supported = VK_FALSE;
            }
            if (deviceProperties.properties.limits.maxPerStageDescriptorStorageImages < 16) {
                supported = VK_FALSE;
            }
            if (deviceProperties.properties.limits.maxPerStageResources < 200) {
                supported = VK_FALSE;
            }
            if (deviceProperties.properties.limits.maxDescriptorSetSamplers < 576) {
                supported = VK_FALSE;
            }
            if (deviceProperties.properties.limits.maxDescriptorSetUniformBuffers < 90) {
                supported = VK_FALSE;
            }
            if (deviceProperties.properties.limits.maxDescriptorSetStorageBuffers < 96) {
                supported = VK_FALSE;
            }
            if (deviceProperties.properties.limits.maxDescriptorSetSampledImages < 1800) {
                supported = VK_FALSE;
            }
            if (deviceProperties.properties.limits.maxDescriptorSetStorageImages < 144) {
                supported = VK_FALSE;
            }

            if (deviceProperties.properties.limits.maxFragmentCombinedOutputResources < 16) {
                supported = VK_FALSE;
            }
            if (deviceProperties.properties.limits.maxComputeWorkGroupInvocations < 256) {
                supported = VK_FALSE;
            }
            if (deviceProperties.properties.limits.maxComputeWorkGroupSize[0] < 256 ||
                deviceProperties.properties.limits.maxComputeWorkGroupSize[1] < 256 ||
                deviceProperties.properties.limits.maxComputeWorkGroupSize[2] < 64) {
                supported = VK_FALSE;
            }
            if (deviceProperties.properties.limits.subTexelPrecisionBits < 8) {
                supported = VK_FALSE;
            }
            if (deviceProperties.properties.limits.mipmapPrecisionBits < 6) {
                supported = VK_FALSE;
            }
            if (deviceProperties.properties.limits.maxSamplerLodBias < 14) {
                supported = VK_FALSE;
            }
            if (deviceProperties.properties.limits.pointSizeGranularity > 0.125) {
                supported = VK_FALSE;
            }
            if (deviceProperties.properties.limits.lineWidthGranularity > 0.5) {
                supported = VK_FALSE;
            }

            if (devicePropertiesVulkan11.subgroupSize < 4) {
                supported = VK_FALSE;
            }

            if (devicePropertiesVulkan12.shaderSignedZeroInfNanPreserveFloat16 != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (devicePropertiesVulkan12.shaderSignedZeroInfNanPreserveFloat32 != VK_TRUE) {
                supported = VK_FALSE;
            }
            if (devicePropertiesVulkan12.maxPerStageDescriptorUpdateAfterBindInputAttachments < 7) {
                supported = VK_FALSE;
            }

        } else if (strcmp(supportedProfiles[i].profileName, VP_LUNARG_1_0_DESKTOP_PORTABILITY_2022_NAME) == 0) {
            // TODO

        } else if (strcmp(supportedProfiles[i].profileName, VP_LUNARG_1_2_DESKTOP_PORTABILITY_2022_NAME) == 0) {
            // TODO

        } else if (strcmp(supportedProfiles[i].profileName, VP_LUNARG_1_2_RAYTRACING_2022_NAME) == 0) {
            // TODO

            break; // So unsupported
        }

        if (supported) {
            if (pProperties != nullptr && supported_profile_count < *pPropertyCount) {
                pProperties[supported_profile_count] = supportedProfiles[i];
            }
            supported_profile_count++;
        }
    }

    if (pProperties == nullptr) {
        *pPropertyCount = supported_profile_count;
    }

    return result;
}
