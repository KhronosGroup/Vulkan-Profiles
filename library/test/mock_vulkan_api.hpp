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

#pragma once

#include <gtest/gtest.h>

#include <vulkan/vulkan_core.h>

#include <string.h>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <algorithm>

struct ExpectedStructData {
    VkStructureType         sType;
    std::vector<uint8_t>    contents;

    ExpectedStructData(VkStructureType sTypeIn, size_t size, const uint8_t* pData)
        : sType(sTypeIn)
        , contents(size - sizeof(VkBaseInStructure))
    {
        memcpy(contents.data(), pData + sizeof(VkBaseInStructure), size - sizeof(VkBaseInStructure));
    }
};

#define EXPECT_STRUCT(STRUCT) ExpectedStructData(STRUCT.sType, sizeof(STRUCT), static_cast<uint8_t*>(static_cast<void*>(&STRUCT)))
#define VK_EXT(NAME) VkExtensionProperties{ NAME##_EXTENSION_NAME, NAME##_SPEC_VERSION }

class MockVulkanAPI final
{
private:
    std::unordered_map<VkInstance, std::unordered_map<std::string, PFN_vkVoidFunction>> m_instanceProcAddr;
    std::unordered_map<std::string, std::vector<VkExtensionProperties>>                 m_instanceExtensions;
    std::unordered_map<VkPhysicalDevice, std::vector<VkExtensionProperties>>            m_deviceExtensions;

    uint32_t                            m_instanceAPIVersion;
    const VkInstanceCreateInfo*         m_pInstanceCreateInfo;
    std::vector<ExpectedStructData>     m_instanceCreateStructs;

    uint32_t                            m_deviceAPIVersion;
    const VkDeviceCreateInfo*           m_pDeviceCreateInfo;
    std::vector<ExpectedStructData>     m_deviceCreateStructs;

    static MockVulkanAPI*   sInstance;

    const void* GetStructure(const void* pNext, VkStructureType type)
    {
        const VkBaseOutStructure *p = static_cast<const VkBaseOutStructure*>(pNext);
        while (p != nullptr) {
            if (p->sType == type) return p;
            p = p->pNext;
        }
        return nullptr;
    }

    void CheckChainedStructs(const void* pNext, const std::vector<ExpectedStructData>& expectedStructs)
    {
        for (auto& expectedStructData : expectedStructs) {
            const void* pActualStructData = GetStructure(pNext, expectedStructData.sType);
            EXPECT_NE(pActualStructData, nullptr) << "Chained struct is missing";
            if (pActualStructData != nullptr) {
                EXPECT_TRUE(memcmp(expectedStructData.contents.data(),
                                   static_cast<const uint8_t*>(pActualStructData) + sizeof(VkBaseInStructure),
                                   expectedStructData.contents.size()) == 0) << "Chained struct data mismatch";
            }
        }
    }

    template <typename T>
    void AddInstanceProc(VkInstance instance, const char* pName, T proc)
    {
        m_instanceProcAddr[instance][std::string(pName)] = (PFN_vkVoidFunction)proc;
    }

    void RemoveInstanceProc(VkInstance instance, const char* pName)
    {
        m_instanceProcAddr[instance].erase(std::string(pName));
    }

public:
    VkInstance              vkInstance;
    VkPhysicalDevice        vkPhysicalDevice;
    VkAllocationCallbacks   vkAllocator;

    MockVulkanAPI()
        : m_instanceProcAddr{}
        , m_instanceExtensions{}
        , m_deviceExtensions{}
        , m_instanceAPIVersion{ VK_API_VERSION_1_0 }
        , m_pInstanceCreateInfo{}
        , m_instanceCreateStructs{}
        , m_deviceAPIVersion{ VK_API_VERSION_1_0 }
        , m_pDeviceCreateInfo{}
        , m_deviceCreateStructs{}
        , vkInstance{ VkInstance(0x11D00D00) }
        , vkPhysicalDevice{ VkPhysicalDevice(0x42D00D00) }
        , vkAllocator{}
    {
        sInstance = this;
    }

    ~MockVulkanAPI()
    {
        sInstance = nullptr;
    }

    static PFN_vkVoidFunction vkGetInstanceProcAddr(
        VkInstance                                  instance,
        const char*                                 pName)
    {
        EXPECT_NE(sInstance, nullptr) << "No Vulkan API mock is configured";
        if (sInstance != nullptr) {
            auto it = sInstance->m_instanceProcAddr.find(instance);
            if (it != sInstance->m_instanceProcAddr.end()) {
                auto it2 = it->second.find(pName);
                if (it2 != it->second.end()) {
                    return it2->second;
                }
            }
        }
        return nullptr;
    }

    void SetInstanceAPIVersion(uint32_t version)
    {
        m_instanceAPIVersion = version;
        if (version >= VK_API_VERSION_1_1) {
            AddInstanceProc(nullptr, "vkEnumerateInstanceVersion", &vkEnumerateInstanceVersion);
            AddInstanceProc(vkInstance, "vkGetPhysicalDeviceFeatures2", &vkGetPhysicalDeviceFeatures2);
            AddInstanceProc(vkInstance, "vkGetPhysicalDeviceProperties2", &vkGetPhysicalDeviceProperties2);
            AddInstanceProc(vkInstance, "vkGetPhysicalDeviceFormatProperties2", &vkGetPhysicalDeviceFormatProperties2);
            AddInstanceProc(vkInstance, "vkGetPhysicalDeviceQueueFamilyProperties2", &vkGetPhysicalDeviceQueueFamilyProperties2);
            RemoveInstanceProc(vkInstance, "vkGetPhysicalDeviceFeatures2KHR");
            RemoveInstanceProc(vkInstance, "vkGetPhysicalDeviceProperties2KHR");
            RemoveInstanceProc(vkInstance, "vkGetPhysicalDeviceFormatProperties2KHR");
            RemoveInstanceProc(vkInstance, "vkGetPhysicalDeviceQueueFamilyProperties2KHR");
        } else {
            AddInstanceProc(vkInstance, "vkGetPhysicalDeviceFeatures2KHR", &vkGetPhysicalDeviceFeatures2);
            AddInstanceProc(vkInstance, "vkGetPhysicalDeviceProperties2KHR", &vkGetPhysicalDeviceProperties2);
            AddInstanceProc(vkInstance, "vkGetPhysicalDeviceFormatProperties2KHR", &vkGetPhysicalDeviceFormatProperties2);
            AddInstanceProc(vkInstance, "vkGetPhysicalDeviceQueueFamilyProperties2KHR", &vkGetPhysicalDeviceQueueFamilyProperties2);
            RemoveInstanceProc(vkInstance, "vkGetPhysicalDeviceFeatures2");
            RemoveInstanceProc(vkInstance, "vkGetPhysicalDeviceProperties2");
            RemoveInstanceProc(vkInstance, "vkGetPhysicalDeviceFormatProperties2");
            RemoveInstanceProc(vkInstance, "vkGetPhysicalDeviceQueueFamilyProperties2");
        }
    }

    static VkResult vkEnumerateInstanceVersion(
        uint32_t*                                   pApiVersion)
    {
        EXPECT_NE(sInstance, nullptr) << "No Vulkan API mock is configured";
        if (sInstance != nullptr) {
            *pApiVersion = sInstance->m_instanceAPIVersion;
            return VK_SUCCESS;
        }
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    void SetInstanceExtensions(const char* pLayerName, const std::vector<VkExtensionProperties>& extensions)
    {
        std::string layerName{ (pLayerName == nullptr) ? "" : pLayerName };
        m_instanceExtensions[layerName] = extensions;
    }

    static VkResult vkEnumerateInstanceExtensionProperties(
        const char*                                 pLayerName,
        uint32_t*                                   pPropertyCount,
        VkExtensionProperties*                      pProperties)
    {
        EXPECT_NE(sInstance, nullptr) << "No Vulkan API mock is configured";
        if (sInstance != nullptr) {
            VkResult result = VK_SUCCESS;
            std::string layerName{ (pLayerName == nullptr) ? "" : pLayerName };
            auto it = sInstance->m_instanceExtensions.find(layerName);
            if (it != sInstance->m_instanceExtensions.end()) {
                auto& extensions = it->second;
                if (pProperties == nullptr) {
                    *pPropertyCount = static_cast<uint32_t>(extensions.size());
                } else {
                    if (*pPropertyCount < extensions.size()) {
                        result = VK_INCOMPLETE;
                    } else {
                        *pPropertyCount = static_cast<uint32_t>(extensions.size());
                    }
                    for (uint32_t i = 0; i < *pPropertyCount; ++i) {
                        pProperties[i] = extensions[i];
                    }
                }
            } else {
                *pPropertyCount = 0;
                result = VK_ERROR_UNKNOWN;
            }
            return result;
        }
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    void SetDeviceExtensions(VkPhysicalDevice physicalDevice, const std::vector<VkExtensionProperties>& extensions)
    {
        m_deviceExtensions[physicalDevice] = extensions;
    }

    static VkResult vkEnumerateDeviceExtensionProperties(
        VkPhysicalDevice                            physicalDevice,
        const char*                                 pLayerName,
        uint32_t*                                   pPropertyCount,
        VkExtensionProperties*                      pProperties)
    {
        EXPECT_NE(sInstance, nullptr) << "No Vulkan API mock is configured";
        if (sInstance != nullptr) {
            VkResult result = VK_SUCCESS;
            auto it = sInstance->m_deviceExtensions.find(physicalDevice);
            if (it != sInstance->m_deviceExtensions.end()) {
                auto& extensions = it->second;
                if (pProperties == nullptr) {
                    *pPropertyCount = static_cast<uint32_t>(extensions.size());
                } else {
                    if (*pPropertyCount < extensions.size()) {
                        result = VK_INCOMPLETE;
                    } else {
                        *pPropertyCount = static_cast<uint32_t>(extensions.size());
                    }
                    for (uint32_t i = 0; i < *pPropertyCount; ++i) {
                        pProperties[i] = extensions[i];
                    }
                }
            } else {
                *pPropertyCount = 0;
                result = VK_ERROR_UNKNOWN;
            }
            return result;
        }
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    void SetExpectedInstanceCreateInfo(const VkInstanceCreateInfo* pCreateInfo, std::vector<ExpectedStructData>&& structs)
    {
        m_pInstanceCreateInfo = pCreateInfo;
        m_instanceCreateStructs = std::move(structs);
    }

    static VkResult vkCreateInstance(
        const VkInstanceCreateInfo*                 pCreateInfo,
        const VkAllocationCallbacks*                pAllocator,
        VkInstance*                                 pInstance)
    {
        EXPECT_NE(sInstance, nullptr) << "No Vulkan API mock is configured";
        EXPECT_NE(pCreateInfo, nullptr);
        if (sInstance != nullptr && pCreateInfo != nullptr) {
            EXPECT_EQ(pAllocator, &sInstance->vkAllocator) << "Unexpected allocator callbacks";

            EXPECT_EQ(pCreateInfo->sType, VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO);
            EXPECT_EQ(pCreateInfo->flags, sInstance->m_pInstanceCreateInfo->flags);

            EXPECT_EQ(pCreateInfo->pApplicationInfo != nullptr, sInstance->m_pInstanceCreateInfo->pApplicationInfo != nullptr) << "Presence of pApplicationInfo must match";
            if (pCreateInfo->pApplicationInfo != nullptr && sInstance->m_pInstanceCreateInfo->pApplicationInfo != nullptr) {
                EXPECT_EQ(pCreateInfo->pApplicationInfo->sType, VK_STRUCTURE_TYPE_APPLICATION_INFO);
                EXPECT_STREQ(pCreateInfo->pApplicationInfo->pApplicationName, sInstance->m_pInstanceCreateInfo->pApplicationInfo->pApplicationName);
                EXPECT_EQ(pCreateInfo->pApplicationInfo->applicationVersion, sInstance->m_pInstanceCreateInfo->pApplicationInfo->applicationVersion);
                EXPECT_STREQ(pCreateInfo->pApplicationInfo->pEngineName, sInstance->m_pInstanceCreateInfo->pApplicationInfo->pEngineName);
                EXPECT_EQ(pCreateInfo->pApplicationInfo->engineVersion, sInstance->m_pInstanceCreateInfo->pApplicationInfo->engineVersion);
                EXPECT_EQ(pCreateInfo->pApplicationInfo->apiVersion, sInstance->m_pInstanceCreateInfo->pApplicationInfo->apiVersion);
            }

            EXPECT_EQ(pCreateInfo->enabledLayerCount, sInstance->m_pInstanceCreateInfo->enabledLayerCount);
            {
                uint32_t matches = 0;
                for (uint32_t i = 0; i < sInstance->m_pInstanceCreateInfo->enabledLayerCount; ++i) {
                    for (uint32_t j = 0; j < pCreateInfo->enabledLayerCount; ++j) {
                        if (strcmp(sInstance->m_pInstanceCreateInfo->ppEnabledLayerNames[i], pCreateInfo->ppEnabledLayerNames[j]) == 0) {
                            matches++;
                        }
                    }
                }
                EXPECT_EQ(matches, sInstance->m_pInstanceCreateInfo->enabledLayerCount) << "Layer list does not match";
            }

            EXPECT_EQ(pCreateInfo->enabledExtensionCount, sInstance->m_pInstanceCreateInfo->enabledExtensionCount);
            {
                uint32_t matches = 0;
                for (uint32_t i = 0; i < sInstance->m_pInstanceCreateInfo->enabledExtensionCount; ++i) {
                    for (uint32_t j = 0; j < pCreateInfo->enabledExtensionCount; ++j) {
                        if (strcmp(sInstance->m_pInstanceCreateInfo->ppEnabledExtensionNames[i], pCreateInfo->ppEnabledExtensionNames[j]) == 0) {
                            matches++;
                        }
                    }
                }
                EXPECT_EQ(matches, sInstance->m_pInstanceCreateInfo->enabledExtensionCount) << "Extension list does not match";
            }

            sInstance->CheckChainedStructs(pCreateInfo->pNext, sInstance->m_instanceCreateStructs);

            *pInstance = sInstance->vkInstance;
            return VK_SUCCESS;
        }
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    void SetDeviceAPIVersion(uint32_t version)
    {
        m_deviceAPIVersion = version;
    }

    static void vkGetPhysicalDeviceProperties(
        VkPhysicalDevice                            physicalDevice,
        VkPhysicalDeviceProperties*                 pProperties)
    {
        EXPECT_NE(sInstance, nullptr) << "No Vulkan API mock is configured";
        if (sInstance != nullptr) {
            pProperties->apiVersion = sInstance->m_deviceAPIVersion;
        }
    }

    static void vkGetPhysicalDeviceFeatures2(
        VkPhysicalDevice                            physicalDevice,
        VkPhysicalDeviceFeatures2*                  pFeatures)
    {
        EXPECT_NE(sInstance, nullptr) << "No Vulkan API mock is configured";
        if (sInstance != nullptr) {
            // TODO
        }
    }

    static void vkGetPhysicalDeviceProperties2(
        VkPhysicalDevice                            physicalDevice,
        VkPhysicalDeviceProperties2*                pProperties)
    {
        EXPECT_NE(sInstance, nullptr) << "No Vulkan API mock is configured";
        if (sInstance != nullptr) {
            // TODO
        }
    }

    static void vkGetPhysicalDeviceFormatProperties2(
        VkPhysicalDevice                            physicalDevice,
        VkFormat                                    format,
        VkFormatProperties2*                        pFormatProperties)
    {
        EXPECT_NE(sInstance, nullptr) << "No Vulkan API mock is configured";
        if (sInstance != nullptr) {
            // TODO
        }
    }

    static void vkGetPhysicalDeviceQueueFamilyProperties2(
        VkPhysicalDevice                            physicalDevice,
        uint32_t*                                   pQueueFamilyPropertyCount,
        VkQueueFamilyProperties2*                   pQueueFamilyProperties)
    {
        EXPECT_NE(sInstance, nullptr) << "No Vulkan API mock is configured";
        if (sInstance != nullptr) {
            // TODO
        }
    }

    static VkResult vkCreateDevice(
        VkPhysicalDevice                            physicalDevice,
        const VkDeviceCreateInfo*                   pCreateInfo,
        const VkAllocationCallbacks*                pAllocator,
        VkDevice*                                   pDevice)
    {
        EXPECT_NE(sInstance, nullptr) << "No Vulkan API mock is configured";
        if (sInstance != nullptr) {
            // TODO
        }
        return VK_ERROR_INITIALIZATION_FAILED;
    }
};

MockVulkanAPI* MockVulkanAPI::sInstance = nullptr;

// Redirect Vulkan API calls to mock class
#define vkGetInstanceProcAddr MockVulkanAPI::vkGetInstanceProcAddr
#define vkEnumerateInstanceExtensionProperties MockVulkanAPI::vkEnumerateInstanceExtensionProperties
#define vkEnumerateDeviceExtensionProperties MockVulkanAPI::vkEnumerateDeviceExtensionProperties
#define vkCreateInstance MockVulkanAPI::vkCreateInstance
#define vkGetPhysicalDeviceProperties MockVulkanAPI::vkGetPhysicalDeviceProperties
#define vkCreateDevice MockVulkanAPI::vkCreateDevice
