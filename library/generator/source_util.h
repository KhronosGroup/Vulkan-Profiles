// Implementation details:
#include <cstring>
#include <cassert>
#include <vector>

#define _vpCountOf(arr) sizeof(arr) / sizeof(arr[0])

VP_INLINE bool _vpCheckExtension(const VkExtensionProperties *supportedProperties, std::size_t supportedSize,
                              const char *requestedExtension) {
    for (size_t i = 0, n = supportedSize; i < n; ++i) {
        if (strcmp(supportedProperties[i].extensionName, requestedExtension) == 0) {
            return true;
        }
    }

    return false;
}

VP_INLINE bool _vpCheckMemoryProperty(const VkPhysicalDeviceMemoryProperties &memoryProperties,
                                   const VkMemoryPropertyFlags &memoryPropertyFlags) {
    assert(&memoryProperties != nullptr);

    for (size_t i = 0, n = memoryProperties.memoryTypeCount; i < n; ++i) {
        if ((memoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags) return true;
    }

    return false;
}

VP_INLINE bool _vpCheckFormatProperty(const VkFormatProperties2 *deviceProps, const VpFormatProperties &profileProps) {
    if ((deviceProps->formatProperties.linearTilingFeatures & profileProps.linearTilingFeatures) !=
        profileProps.linearTilingFeatures) {
        return false;
    } else if ((deviceProps->formatProperties.optimalTilingFeatures & profileProps.optimalTilingFeatures) !=
               profileProps.optimalTilingFeatures) {
        return false;
    } else if ((deviceProps->formatProperties.bufferFeatures & profileProps.bufferFeatures) != profileProps.bufferFeatures) {
        return false;
    }

    return true;
}

VP_INLINE bool _vpCheckQueueFamilyProperty(const VkQueueFamilyProperties *queueFamilyProperties,
                                        std::size_t queueFamilyPropertiesCount,
                                        const VkQueueFamilyProperties &profileQueueFamilyPropertie) {
    assert(queueFamilyProperties != nullptr);

    for (size_t i = 0, n = queueFamilyPropertiesCount; i < n; ++i) {
        if (queueFamilyProperties[i].queueCount < profileQueueFamilyPropertie.queueCount) {
            continue;
        } else if (queueFamilyProperties[i].timestampValidBits < profileQueueFamilyPropertie.timestampValidBits) {
            continue;
        } else if (queueFamilyProperties[i].minImageTransferGranularity.width >
                   profileQueueFamilyPropertie.minImageTransferGranularity.width) {
            continue;
        } else if (queueFamilyProperties[i].minImageTransferGranularity.height >
                   profileQueueFamilyPropertie.minImageTransferGranularity.height) {
            continue;
        } else if (queueFamilyProperties[i].minImageTransferGranularity.depth >
                   profileQueueFamilyPropertie.minImageTransferGranularity.depth) {
            continue;
        } else if ((queueFamilyProperties[i].queueFlags & profileQueueFamilyPropertie.queueFlags) !=
                   profileQueueFamilyPropertie.queueFlags) {
            continue;
        }

        return true;
    }

    return false;
}

VP_INLINE void _vpGetExtensions(const VpDeviceCreateInfo *pCreateInfo, uint32_t propertyCount,
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

VP_INLINE const void *_vpGetStructure(const void *pNext, VkStructureType type) {
    if (pNext == nullptr) {
        return nullptr;
    }

    struct VkStruct {
        VkStructureType sType;
        void *pNext;
    };

    const VkStruct *p = static_cast<const VkStruct *>(pNext);

    if (p->sType == type) {
        return pNext;
    } else {
        return _vpGetStructure(p->pNext, type);
    }
}
