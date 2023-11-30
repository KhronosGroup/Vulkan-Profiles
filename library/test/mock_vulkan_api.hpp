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

struct VulkanStructData {
    VkStructureType         sType;
    std::vector<uint8_t>    contents;

    VulkanStructData(VkStructureType sTypeIn, size_t size, const uint8_t* pData)
        : sType(sTypeIn)
        , contents(size - sizeof(VkBaseInStructure))
    {
        memcpy(contents.data(), pData + sizeof(VkBaseInStructure), size - sizeof(VkBaseInStructure));
    }

    void copyTo(VkBaseOutStructure* pStruct)
    {
        uint8_t* p = static_cast<uint8_t*>(static_cast<void*>(pStruct));
        memcpy(p + sizeof(VkBaseOutStructure), contents.data(), contents.size());
    }
};

#define VK_STRUCT(STRUCT) VulkanStructData(STRUCT.sType, sizeof(STRUCT), static_cast<uint8_t*>(static_cast<void*>(&STRUCT)))
#define VK_EXT(NAME) VkExtensionProperties{ NAME##_EXTENSION_NAME, NAME##_SPEC_VERSION }

enum {
    PROFILE_AREA_EXTENSIONS_BIT = 1 << 0,
    PROFILE_AREA_FEATURES_BIT = 1 << 1,
    PROFILE_AREA_PROPERTIES_BIT = 1 << 2,
    PROFILE_AREA_FORMATS_BIT = 1 << 3,
    PROFILE_AREA_ALL_BITS = PROFILE_AREA_EXTENSIONS_BIT | PROFILE_AREA_FEATURES_BIT | PROFILE_AREA_PROPERTIES_BIT |
                            PROFILE_AREA_FORMATS_BIT
};

class MockVulkanAPI final
{
private:
    std::unordered_map<VkInstance, std::unordered_map<std::string, PFN_vkVoidFunction>> m_instanceProcAddr;
    std::unordered_map<std::string, std::vector<VkExtensionProperties>>                 m_instanceExtensions;
    std::unordered_map<VkPhysicalDevice, std::vector<VkExtensionProperties>>            m_deviceExtensions;

    uint32_t                            m_instanceAPIVersion;
    const VkInstanceCreateInfo*         m_pInstanceCreateInfo;
    std::vector<VulkanStructData>       m_instanceCreateStructs;

    uint32_t                            m_deviceAPIVersion;
    const VkDeviceCreateInfo*           m_pDeviceCreateInfo;
    std::vector<VulkanStructData>       m_deviceCreateStructs;

    using MockedStructData = std::unordered_map<uint32_t, VulkanStructData>;

    MockedStructData                                m_mockedFeatures;
    MockedStructData                                m_mockedProperties;
    std::unordered_map<uint32_t, MockedStructData>  m_mockedFormats;
    std::vector<MockedStructData>                   m_mockedQueueFamilies;

    static MockVulkanAPI*   sInstance;

    const VkBaseOutStructure* GetStructure(const void* pNext, VkStructureType type)
    {
        const VkBaseOutStructure *p = static_cast<const VkBaseOutStructure*>(pNext);
        while (p != nullptr) {
            if (p->sType == type) return p;
            p = p->pNext;
        }
        return nullptr;
    }

    void CheckChainedStructs(const void* pNext, const std::vector<VulkanStructData>& expectedStructs)
    {
        for (auto& VulkanStructData : expectedStructs) {
            const VkBaseOutStructure* pActualStructData = GetStructure(pNext, VulkanStructData.sType);
            EXPECT_NE(pActualStructData, nullptr) << "Chained struct is missing";
            if (pActualStructData != nullptr) {
                EXPECT_TRUE(memcmp(VulkanStructData.contents.data(),
                                   reinterpret_cast<const uint8_t*>(pActualStructData) + sizeof(VkBaseInStructure),
                                   VulkanStructData.contents.size()) == 0) << "Chained struct data mismatch";
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
    VkDevice                vkDevice;
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
        , m_mockedFeatures{}
        , m_mockedProperties{}
        , m_mockedFormats{}
        , m_mockedQueueFamilies{}
        , vkInstance{ VkInstance(0x11D00D00) }
        , vkPhysicalDevice{ VkPhysicalDevice(0x42D00D00) }
        , vkDevice{ VkDevice(0x66D00D00) }
        , vkAllocator{}
    {
        sInstance = this;
    }

    ~MockVulkanAPI()
    {
        sInstance = nullptr;
    }

    void ClearProfileAreas(int profileAreas) {
        if (profileAreas & PROFILE_AREA_EXTENSIONS_BIT) {
            this->m_instanceExtensions.clear();
            this->m_deviceExtensions.clear();
        }

        if (profileAreas & PROFILE_AREA_FEATURES_BIT) {
            this->m_mockedFeatures.clear();
        }

        if (profileAreas & PROFILE_AREA_PROPERTIES_BIT) {
            this->m_mockedProperties.clear();
        }

        if (profileAreas & PROFILE_AREA_FORMATS_BIT) {
            this->m_mockedFormats.clear();
        }
    }

    static VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(
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

    static VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceVersion(
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

    static VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceExtensionProperties(
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

    static VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceExtensionProperties(
        VkPhysicalDevice                            physicalDevice,
        const char*                                 pLayerName,
        uint32_t*                                   pPropertyCount,
        VkExtensionProperties*                      pProperties)
    {
        (void)pLayerName;

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

    void SetExpectedInstanceCreateInfo(const VkInstanceCreateInfo* pCreateInfo, std::vector<VulkanStructData>&& structs)
    {
        m_pInstanceCreateInfo = pCreateInfo;
        m_instanceCreateStructs = std::move(structs);
    }

    static VKAPI_ATTR VkResult VKAPI_CALL vkCreateInstance(
        const VkInstanceCreateInfo*                 pCreateInfo,
        const VkAllocationCallbacks*                pAllocator,
        VkInstance*                                 pInstance)
    {
        EXPECT_NE(sInstance, nullptr) << "No Vulkan API mock is configured";
        EXPECT_NE(pCreateInfo, nullptr);
        if (sInstance != nullptr && pCreateInfo != nullptr) {
            EXPECT_EQ(pAllocator, &sInstance->vkAllocator) << "Unexpected allocator callbacks";

            EXPECT_EQ(pCreateInfo->sType, VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO);
            VkInstanceCreateFlags flags = sInstance->m_pInstanceCreateInfo->flags;
#ifdef __APPLE__
            flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
            EXPECT_EQ(pCreateInfo->flags, flags);

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
                        if (strcmp(sInstance->m_pInstanceCreateInfo->ppEnabledLayerNames[i], pCreateInfo->ppEnabledLayerNames[j]) ==
                            0) {
                            matches++;
                        }
                    }
                }
                EXPECT_EQ(matches, sInstance->m_pInstanceCreateInfo->enabledLayerCount) << "Layer list does not match";
            }

            std::vector<const char*> extensions;
            for (uint32_t i = 0; i < sInstance->m_pInstanceCreateInfo->enabledExtensionCount; ++i) {
                extensions.push_back(sInstance->m_pInstanceCreateInfo->ppEnabledExtensionNames[i]);
            }
#ifdef __APPLE__
            extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

            EXPECT_EQ(pCreateInfo->enabledExtensionCount, static_cast<uint32_t>(extensions.size()));
            {
                uint32_t matches = 0;
                for (uint32_t i = 0; i < static_cast<uint32_t>(extensions.size()); ++i) {
                    for (uint32_t j = 0; j < pCreateInfo->enabledExtensionCount; ++j) {
                        if (strcmp(extensions[i], pCreateInfo->ppEnabledExtensionNames[j]) == 0) {
                            matches++;
                        }
                    }
                }
                EXPECT_EQ(matches, static_cast<uint32_t>(extensions.size())) << "Extension list does not match";
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

    static VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceProperties(
        VkPhysicalDevice                            physicalDevice,
        VkPhysicalDeviceProperties*                 pProperties)
    {
        (void)physicalDevice;

        EXPECT_NE(sInstance, nullptr) << "No Vulkan API mock is configured";
        if (sInstance != nullptr) {
            pProperties->apiVersion = sInstance->m_deviceAPIVersion;
        }
    }

    void SetFeatures(std::vector<VulkanStructData>&& structs)
    {
        for (size_t i = 0; i < structs.size(); ++i) {
            m_mockedFeatures.emplace(structs[i].sType, std::move(structs[i]));
        }
    }

    static VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFeatures2(
        VkPhysicalDevice                            physicalDevice,
        VkPhysicalDeviceFeatures2*                  pFeatures)
    {
        (void)physicalDevice;

        EXPECT_NE(sInstance, nullptr) << "No Vulkan API mock is configured";
        if (sInstance != nullptr) {
            VkBaseOutStructure* p = static_cast<VkBaseOutStructure*>(static_cast<void*>(pFeatures));
            while (p != nullptr) {
                auto it = sInstance->m_mockedFeatures.find(p->sType);
                if (it != sInstance->m_mockedFeatures.end()) {
                    it->second.copyTo(p);
                }
                p = p->pNext;
            }
        }
    }

    void SetProperties(std::vector<VulkanStructData>&& structs)
    {
        for (size_t i = 0; i < structs.size(); ++i) {
            m_mockedProperties.emplace(structs[i].sType, std::move(structs[i]));
        }
    }

    static VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceProperties2(
        VkPhysicalDevice                            physicalDevice,
        VkPhysicalDeviceProperties2*                pProperties)
    {
        (void)physicalDevice;

        EXPECT_NE(sInstance, nullptr) << "No Vulkan API mock is configured";
        if (sInstance != nullptr) {
            VkBaseOutStructure* p = static_cast<VkBaseOutStructure*>(static_cast<void*>(pProperties));
            while (p != nullptr) {
                auto it = sInstance->m_mockedProperties.find(p->sType);
                if (it != sInstance->m_mockedProperties.end()) {
                    it->second.copyTo(p);
                }
                p = p->pNext;
            }
        }
    }

    void AddFormat(VkFormat format, std::vector<VulkanStructData>&& structs)
    {
        auto& formatProperties = m_mockedFormats[format];
        for (size_t i = 0; i < structs.size(); ++i) {
            formatProperties.emplace(structs[i].sType, std::move(structs[i]));
        }
    }

    static VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFormatProperties2(
        VkPhysicalDevice                            physicalDevice,
        VkFormat                                    format,
        VkFormatProperties2*                        pFormatProperties)
    {
        (void)physicalDevice;

        EXPECT_NE(sInstance, nullptr) << "No Vulkan API mock is configured";
        if (sInstance != nullptr) {
            auto fmtIt = sInstance->m_mockedFormats.find(format);
            if (fmtIt != sInstance->m_mockedFormats.end()) {
                auto& mockedFormat = fmtIt->second;
                VkBaseOutStructure* p = static_cast<VkBaseOutStructure*>(static_cast<void*>(pFormatProperties));
                while (p != nullptr) {
                    auto it = mockedFormat.find(p->sType);
                    if (it != mockedFormat.end()) {
                        it->second.copyTo(p);
                    }
                    p = p->pNext;
                }
            }
        }
    }

    void AddQueueFamily(std::vector<VulkanStructData>&& structs)
    {
        m_mockedQueueFamilies.push_back({});
        auto& queueFamilyProperties = m_mockedQueueFamilies.back();
        for (size_t i = 0; i < structs.size(); ++i) {
            queueFamilyProperties.emplace(structs[i].sType, std::move(structs[i]));
        }
    }

    static VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceQueueFamilyProperties2(
        VkPhysicalDevice                            physicalDevice,
        uint32_t*                                   pQueueFamilyPropertyCount,
        VkQueueFamilyProperties2*                   pQueueFamilyProperties)
    {
        (void)physicalDevice;

        EXPECT_NE(sInstance, nullptr) << "No Vulkan API mock is configured";
        if (sInstance != nullptr) {
            if (pQueueFamilyProperties == nullptr) {
                *pQueueFamilyPropertyCount = static_cast<uint32_t>(sInstance->m_mockedQueueFamilies.size());
            } else {
                if (*pQueueFamilyPropertyCount > sInstance->m_mockedQueueFamilies.size()) {
                    *pQueueFamilyPropertyCount = static_cast<uint32_t>(sInstance->m_mockedQueueFamilies.size());
                }
                for (uint32_t i = 0; i < *pQueueFamilyPropertyCount; ++i) {
                    VkBaseOutStructure* p = static_cast<VkBaseOutStructure*>(static_cast<void*>(&pQueueFamilyProperties[i]));
                    while (p != nullptr) {
                        auto it = sInstance->m_mockedQueueFamilies[i].find(p->sType);
                        if (it != sInstance->m_mockedQueueFamilies[i].end()) {
                            it->second.copyTo(p);
                        }
                        p = p->pNext;
                    }
                }
            }
        }
    }

    void SetExpectedDeviceCreateInfo(const VkDeviceCreateInfo* pCreateInfo, std::vector<VulkanStructData>&& structs)
    {
        m_pDeviceCreateInfo = pCreateInfo;
        m_deviceCreateStructs = std::move(structs);
    }

    static VKAPI_ATTR VkResult VKAPI_CALL vkCreateDevice(
        VkPhysicalDevice                            physicalDevice,
        const VkDeviceCreateInfo*                   pCreateInfo,
        const VkAllocationCallbacks*                pAllocator,
        VkDevice*                                   pDevice)
    {
        (void)physicalDevice;

        EXPECT_NE(sInstance, nullptr) << "No Vulkan API mock is configured";
        EXPECT_NE(pCreateInfo, nullptr);
        if (sInstance != nullptr && pCreateInfo != nullptr) {
            EXPECT_EQ(pAllocator, &sInstance->vkAllocator) << "Unexpected allocator callbacks";

            EXPECT_EQ(pCreateInfo->sType, VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO);
            EXPECT_EQ(pCreateInfo->flags, sInstance->m_pDeviceCreateInfo->flags);
            EXPECT_EQ(pCreateInfo->queueCreateInfoCount, sInstance->m_pDeviceCreateInfo->queueCreateInfoCount);
            EXPECT_EQ(pCreateInfo->pQueueCreateInfos, sInstance->m_pDeviceCreateInfo->pQueueCreateInfos);
            EXPECT_EQ(pCreateInfo->enabledLayerCount, sInstance->m_pDeviceCreateInfo->enabledLayerCount);
            EXPECT_EQ(pCreateInfo->ppEnabledLayerNames, sInstance->m_pDeviceCreateInfo->ppEnabledLayerNames);

            EXPECT_EQ(pCreateInfo->enabledExtensionCount, sInstance->m_pDeviceCreateInfo->enabledExtensionCount);
            {
                uint32_t matches = 0;
                for (uint32_t i = 0; i < sInstance->m_pDeviceCreateInfo->enabledExtensionCount; ++i) {
                    for (uint32_t j = 0; j < pCreateInfo->enabledExtensionCount; ++j) {
                        if (strcmp(sInstance->m_pDeviceCreateInfo->ppEnabledExtensionNames[i], pCreateInfo->ppEnabledExtensionNames[j]) == 0) {
                            matches++;
                        }
                    }
                }
                EXPECT_EQ(matches, sInstance->m_pDeviceCreateInfo->enabledExtensionCount) << "Extension list does not match";
            }

            EXPECT_EQ(pCreateInfo->pEnabledFeatures, sInstance->m_pDeviceCreateInfo->pEnabledFeatures);

            sInstance->CheckChainedStructs(pCreateInfo->pNext, sInstance->m_deviceCreateStructs);

            *pDevice = sInstance->vkDevice;
            return VK_SUCCESS;
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
