
/*
 * Copyright (C) 2021-2023 Valve Corporation
 * Copyright (C) 2021-2023 LunarG, Inc.
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

#pragma once

#define VPAPI_ATTR inline

#include <vulkan/vulkan.h>

#include <cstddef>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>

#if defined(VK_VERSION_1_2) && \
    defined(VK_KHR_get_memory_requirements2)
#define VP_LUNARG_test_profile_a 1
#define VP_LUNARG_TEST_PROFILE_A_NAME "VP_LUNARG_test_profile_a"
#define VP_LUNARG_TEST_PROFILE_A_SPEC_VERSION 1
#define VP_LUNARG_TEST_PROFILE_A_MIN_API_VERSION VK_MAKE_VERSION(1, 2, 224)
#endif

#if defined(VK_VERSION_1_3) && \
    defined(VP_LUNARG_test_profile_a) && \
    defined(VK_KHR_driver_properties)
#define VP_LUNARG_test_profile_b 1
#define VP_LUNARG_TEST_PROFILE_B_NAME "VP_LUNARG_test_profile_b"
#define VP_LUNARG_TEST_PROFILE_B_SPEC_VERSION 1
#define VP_LUNARG_TEST_PROFILE_B_MIN_API_VERSION VK_MAKE_VERSION(1, 3, 224)
#endif

#define VP_MAX_PROFILE_NAME_SIZE 256U

typedef struct VpProfileProperties {
    char        profileName[VP_MAX_PROFILE_NAME_SIZE];
    uint32_t    specVersion;
} VpProfileProperties;

typedef enum VpInstanceCreateFlagBits {
    // Default behavior:
    // - profile extensions are used (application must not specify extensions)

    // Merge application provided extension list and profile extension list
    VP_INSTANCE_CREATE_MERGE_EXTENSIONS_BIT = 0x00000001,

    // Use application provided extension list
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
    // Default behavior:
    // - profile extensions are used (application must not specify extensions)
    // - profile feature structures are used (application must not specify any of them) extended
    //   with any other application provided struct that isn't defined by the profile

    // Merge application provided extension list and profile extension list
    VP_DEVICE_CREATE_MERGE_EXTENSIONS_BIT = 0x00000001,

    // Use application provided extension list
    VP_DEVICE_CREATE_OVERRIDE_EXTENSIONS_BIT = 0x00000002,

    // Merge application provided versions of feature structures with the profile features
    // Currently unsupported, but is considered for future inclusion in which case the
    // default behavior could potentially be changed to merging as the currently defined
    // default behavior is forward-compatible with that
    // VP_DEVICE_CREATE_MERGE_FEATURES_BIT = 0x00000004,

    // Use application provided versions of feature structures but use the profile feature
    // structures when the application doesn't provide one (robust access disable flags are
    // ignored if the application overrides the corresponding feature structures)
    VP_DEVICE_CREATE_OVERRIDE_FEATURES_BIT = 0x00000008,

    // Only use application provided feature structures, don't add any profile specific
    // feature structures (robust access disable flags are ignored in this case and only the
    // application provided structures are used)
    VP_DEVICE_CREATE_OVERRIDE_ALL_FEATURES_BIT = 0x00000010,

    VP_DEVICE_CREATE_DISABLE_ROBUST_BUFFER_ACCESS_BIT = 0x00000020,
    VP_DEVICE_CREATE_DISABLE_ROBUST_IMAGE_ACCESS_BIT = 0x00000040,
    VP_DEVICE_CREATE_DISABLE_ROBUST_ACCESS =
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

// List the required profiles of a profile
VPAPI_ATTR VkResult vpGetProfileRequiredProfiles(const VpProfileProperties* pProfile, uint32_t* pPropertyCount, VpProfileProperties* pProperties);

// Query the profile required Vulkan API version
VPAPI_ATTR uint32_t vpGetProfileAPIVersion(const VpProfileProperties* pProfile);

// List the recommended fallback profiles of a profile
VPAPI_ATTR VkResult vpGetProfileFallbacks(const VpProfileProperties *pProfile, uint32_t *pPropertyCount, VpProfileProperties *pProperties);

// Query whether the profile has multiple variants. Profiles with multiple variants can only use vpGetInstanceProfileSupport and vpGetPhysicalDeviceProfileSupport capabilities of the library. Other function will return a VK_ERROR_UNKNOWN error
VPAPI_ATTR VkResult vpHasMultipleVariantsProfile(const VpProfileProperties *pProfile, VkBool32 *pHasMultipleVariants);

// Check whether a profile is supported at the instance level
VPAPI_ATTR VkResult vpGetInstanceProfileSupport(const char *pLayerName, const VpProfileProperties *pProfile, VkBool32 *pSupported);

// Create a VkInstance with the profile instance extensions enabled
VPAPI_ATTR VkResult vpCreateInstance(const VpInstanceCreateInfo *pCreateInfo,
                                     const VkAllocationCallbacks *pAllocator, VkInstance *pInstance);

// Check whether a profile is supported by the physical device
VPAPI_ATTR VkResult vpGetPhysicalDeviceProfileSupport(VkInstance instance, VkPhysicalDevice physicalDevice,
                                                      const VpProfileProperties *pProfile, VkBool32 *pSupported);

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
VPAPI_ATTR VkResult vpGetProfileFeatures(const VpProfileProperties *pProfile, void *pNext);

// Query the list of feature structure types specified by the profile
VPAPI_ATTR VkResult vpGetProfileFeatureStructureTypes(const VpProfileProperties *pProfile, uint32_t *pStructureTypeCount,
                                                      VkStructureType *pStructureTypes);

// Fill the property structures with the requirements of a profile
VPAPI_ATTR VkResult vpGetProfileProperties(const VpProfileProperties *pProfile, void *pNext);

// Query the list of property structure types specified by the profile
VPAPI_ATTR VkResult vpGetProfilePropertyStructureTypes(const VpProfileProperties *pProfile, uint32_t *pStructureTypeCount,
                                                       VkStructureType *pStructureTypes);

// Query the requirements of queue families by a profile
VPAPI_ATTR VkResult vpGetProfileQueueFamilyProperties(const VpProfileProperties *pProfile, uint32_t *pPropertyCount,
                                                      VkQueueFamilyProperties2KHR *pProperties);

// Query the list of query family structure types specified by the profile
VPAPI_ATTR VkResult vpGetProfileQueueFamilyStructureTypes(const VpProfileProperties *pProfile, uint32_t *pStructureTypeCount,
                                                          VkStructureType *pStructureTypes);

// Query the list of formats with specified requirements by a profile
VPAPI_ATTR VkResult vpGetProfileFormats(const VpProfileProperties *pProfile, uint32_t *pFormatCount, VkFormat *pFormats);

// Query the requirements of a format for a profile
VPAPI_ATTR VkResult vpGetProfileFormatProperties(const VpProfileProperties *pProfile, VkFormat format, void *pNext);

// Query the list of format structure types specified by the profile
VPAPI_ATTR VkResult vpGetProfileFormatStructureTypes(const VpProfileProperties *pProfile, uint32_t *pStructureTypeCount,
                                                     VkStructureType *pStructureTypes);

namespace detail {


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
using PFN_vpStructChainerCb =  void(*)(VkBaseOutStructure* p, void* pUser);
using PFN_vpStructChainer = void(*)(VkBaseOutStructure* p, void* pUser, PFN_vpStructChainerCb pfnCb);

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
    PFN_vpStructChainer             pfnQueueFamily;
    PFN_vpStructChainer             pfnFormat;
};

struct VpVariantDesc {
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
#ifdef VP_LUNARG_test_profile_a
namespace VP_LUNARG_TEST_PROFILE_A {

static const VkStructureType featureStructTypes[] = {
    VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR,
};

static const VkStructureType propertyStructTypes[] = {
    VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR,
};

static const VkExtensionProperties deviceExtensions[] = {
    VkExtensionProperties{ VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME, 1 },
};

static const VpFeatureDesc featureDesc = {
    [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR: {
                    VkPhysicalDeviceFeatures2KHR* s = static_cast<VkPhysicalDeviceFeatures2KHR*>(static_cast<void*>(p));
                    s->features.depthBiasClamp = VK_TRUE;
                    s->features.depthClamp = VK_TRUE;
                } break;
                default: break;
            }
    },
    [](VkBaseOutStructure* p) -> bool {
        bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR: {
                    VkPhysicalDeviceFeatures2KHR* s = static_cast<VkPhysicalDeviceFeatures2KHR*>(static_cast<void*>(p));
                    ret = ret && (s->features.depthBiasClamp == VK_TRUE);
                    ret = ret && (s->features.depthClamp == VK_TRUE);
                } break;
                default: break;
            }
        return ret;
    }
};

static const VpPropertyDesc propertyDesc = {
    [](VkBaseOutStructure* p) {
    },
    [](VkBaseOutStructure* p) -> bool {
        bool ret = true;
        return ret;
    }
};

static const VpStructChainerDesc chainerDesc = {
    [](VkBaseOutStructure* p, void* pUser, PFN_vpStructChainerCb pfnCb) {
        p->pNext = static_cast<VkBaseOutStructure*>(static_cast<void*>(nullptr));
        pfnCb(p, pUser);
    },
    [](VkBaseOutStructure* p, void* pUser, PFN_vpStructChainerCb pfnCb) {
        p->pNext = static_cast<VkBaseOutStructure*>(static_cast<void*>(nullptr));
        pfnCb(p, pUser);
    },
    [](VkBaseOutStructure* p, void* pUser, PFN_vpStructChainerCb pfnCb) {
        pfnCb(p, pUser);
    },
    [](VkBaseOutStructure* p, void* pUser, PFN_vpStructChainerCb pfnCb) {
        pfnCb(p, pUser);
    },
};

namespace baseline {
static const VkExtensionProperties deviceExtensions[] = {
    VkExtensionProperties{ VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME, 1 },
};

static const VpFeatureDesc featureDesc = {
    [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR: {
                    VkPhysicalDeviceFeatures2KHR* s = static_cast<VkPhysicalDeviceFeatures2KHR*>(static_cast<void*>(p));
                    s->features.depthBiasClamp = VK_TRUE;
                    s->features.depthClamp = VK_TRUE;
                } break;
                default: break;
            }
    },
    [](VkBaseOutStructure* p) -> bool {
        bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR: {
                    VkPhysicalDeviceFeatures2KHR* s = static_cast<VkPhysicalDeviceFeatures2KHR*>(static_cast<void*>(p));
                    ret = ret && (s->features.depthBiasClamp == VK_TRUE);
                    ret = ret && (s->features.depthClamp == VK_TRUE);
                } break;
                default: break;
            }
        return ret;
    }
};

static const VpPropertyDesc propertyDesc = {
    [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR: {
                    VkPhysicalDeviceProperties2KHR* s = static_cast<VkPhysicalDeviceProperties2KHR*>(static_cast<void*>(p));
                    s->properties.limits.maxImageDimension2D = 4096;
                    s->properties.limits.maxImageDimension3D = 4096;
                    s->properties.limits.maxImageDimensionCube = 4096;
                } break;
                default: break;
            }
    },
    [](VkBaseOutStructure* p) -> bool {
        bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR: {
                    VkPhysicalDeviceProperties2KHR* s = static_cast<VkPhysicalDeviceProperties2KHR*>(static_cast<void*>(p));
                    ret = ret && (s->properties.limits.maxImageDimension2D >= 4096);
                    ret = ret && (s->properties.limits.maxImageDimension3D >= 4096);
                    ret = ret && (s->properties.limits.maxImageDimensionCube >= 4096);
                } break;
                default: break;
            }
        return ret;
    }
};

static const VpStructChainerDesc chainerDesc = {
    [](VkBaseOutStructure* p, void* pUser, PFN_vpStructChainerCb pfnCb) {
        p->pNext = static_cast<VkBaseOutStructure*>(static_cast<void*>(nullptr));
        pfnCb(p, pUser);
    },
    [](VkBaseOutStructure* p, void* pUser, PFN_vpStructChainerCb pfnCb) {
        p->pNext = static_cast<VkBaseOutStructure*>(static_cast<void*>(nullptr));
        pfnCb(p, pUser);
    },
    [](VkBaseOutStructure* p, void* pUser, PFN_vpStructChainerCb pfnCb) {
        pfnCb(p, pUser);
    },
    [](VkBaseOutStructure* p, void* pUser, PFN_vpStructChainerCb pfnCb) {
        pfnCb(p, pUser);
    },
};
} //namespace baseline
} // namespace VP_LUNARG_TEST_PROFILE_A
#endif // VP_LUNARG_test_profile_a

#ifdef VP_LUNARG_test_profile_b
namespace VP_LUNARG_TEST_PROFILE_B {

static const VkStructureType featureStructTypes[] = {
    VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR,
};

static const VkStructureType propertyStructTypes[] = {
    VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR,
};

static const VkExtensionProperties deviceExtensions[] = {
    VkExtensionProperties{ VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME, 1 },
    VkExtensionProperties{ VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME, 1 },
};

static const VpFeatureDesc featureDesc = {
    [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR: {
                    VkPhysicalDeviceFeatures2KHR* s = static_cast<VkPhysicalDeviceFeatures2KHR*>(static_cast<void*>(p));
                    s->features.depthBiasClamp = VK_TRUE;
                    s->features.depthClamp = VK_TRUE;
                    s->features.drawIndirectFirstInstance = VK_TRUE;
                } break;
                default: break;
            }
    },
    [](VkBaseOutStructure* p) -> bool {
        bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR: {
                    VkPhysicalDeviceFeatures2KHR* s = static_cast<VkPhysicalDeviceFeatures2KHR*>(static_cast<void*>(p));
                    ret = ret && (s->features.depthBiasClamp == VK_TRUE);
                    ret = ret && (s->features.depthClamp == VK_TRUE);
                    ret = ret && (s->features.drawIndirectFirstInstance == VK_TRUE);
                } break;
                default: break;
            }
        return ret;
    }
};

static const VpPropertyDesc propertyDesc = {
    [](VkBaseOutStructure* p) {
    },
    [](VkBaseOutStructure* p) -> bool {
        bool ret = true;
        return ret;
    }
};

static const VpStructChainerDesc chainerDesc = {
    [](VkBaseOutStructure* p, void* pUser, PFN_vpStructChainerCb pfnCb) {
        p->pNext = static_cast<VkBaseOutStructure*>(static_cast<void*>(nullptr));
        pfnCb(p, pUser);
    },
    [](VkBaseOutStructure* p, void* pUser, PFN_vpStructChainerCb pfnCb) {
        p->pNext = static_cast<VkBaseOutStructure*>(static_cast<void*>(nullptr));
        pfnCb(p, pUser);
    },
    [](VkBaseOutStructure* p, void* pUser, PFN_vpStructChainerCb pfnCb) {
        pfnCb(p, pUser);
    },
    [](VkBaseOutStructure* p, void* pUser, PFN_vpStructChainerCb pfnCb) {
        pfnCb(p, pUser);
    },
};

namespace baseline {
static const VkExtensionProperties deviceExtensions[] = {
    VkExtensionProperties{ VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME, 1 },
};

static const VpFeatureDesc featureDesc = {
    [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR: {
                    VkPhysicalDeviceFeatures2KHR* s = static_cast<VkPhysicalDeviceFeatures2KHR*>(static_cast<void*>(p));
                    s->features.depthBiasClamp = VK_TRUE;
                    s->features.drawIndirectFirstInstance = VK_TRUE;
                } break;
                default: break;
            }
    },
    [](VkBaseOutStructure* p) -> bool {
        bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR: {
                    VkPhysicalDeviceFeatures2KHR* s = static_cast<VkPhysicalDeviceFeatures2KHR*>(static_cast<void*>(p));
                    ret = ret && (s->features.depthBiasClamp == VK_TRUE);
                    ret = ret && (s->features.drawIndirectFirstInstance == VK_TRUE);
                } break;
                default: break;
            }
        return ret;
    }
};

static const VpPropertyDesc propertyDesc = {
    [](VkBaseOutStructure* p) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR: {
                    VkPhysicalDeviceProperties2KHR* s = static_cast<VkPhysicalDeviceProperties2KHR*>(static_cast<void*>(p));
                    s->properties.limits.maxImageDimension1D = 4096;
                    s->properties.limits.maxImageDimension2D = 8192;
                    s->properties.limits.maxImageDimension3D = 2048;
                } break;
                default: break;
            }
    },
    [](VkBaseOutStructure* p) -> bool {
        bool ret = true;
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR: {
                    VkPhysicalDeviceProperties2KHR* s = static_cast<VkPhysicalDeviceProperties2KHR*>(static_cast<void*>(p));
                    ret = ret && (s->properties.limits.maxImageDimension1D >= 4096);
                    ret = ret && (s->properties.limits.maxImageDimension2D >= 8192);
                    ret = ret && (s->properties.limits.maxImageDimension3D >= 2048);
                } break;
                default: break;
            }
        return ret;
    }
};

static const VpStructChainerDesc chainerDesc = {
    [](VkBaseOutStructure* p, void* pUser, PFN_vpStructChainerCb pfnCb) {
        p->pNext = static_cast<VkBaseOutStructure*>(static_cast<void*>(nullptr));
        pfnCb(p, pUser);
    },
    [](VkBaseOutStructure* p, void* pUser, PFN_vpStructChainerCb pfnCb) {
        p->pNext = static_cast<VkBaseOutStructure*>(static_cast<void*>(nullptr));
        pfnCb(p, pUser);
    },
    [](VkBaseOutStructure* p, void* pUser, PFN_vpStructChainerCb pfnCb) {
        pfnCb(p, pUser);
    },
    [](VkBaseOutStructure* p, void* pUser, PFN_vpStructChainerCb pfnCb) {
        pfnCb(p, pUser);
    },
};
} //namespace baseline
} // namespace VP_LUNARG_TEST_PROFILE_B
#endif // VP_LUNARG_test_profile_b


#ifdef VP_LUNARG_test_profile_a
namespace VP_LUNARG_TEST_PROFILE_A {
    static const VpVariantDesc mergedCapabilities[] = {
        0, nullptr,
        static_cast<uint32_t>(std::size(deviceExtensions)), deviceExtensions,
        static_cast<uint32_t>(std::size(featureStructTypes)), featureStructTypes,
            featureDesc,
        0, nullptr,
            propertyDesc,
        0, nullptr,
        0, nullptr,
        0, nullptr,
        0, nullptr,
        chainerDesc,
    };

    namespace baseline {
        static const VpVariantDesc variants[] = {
            0, nullptr,
            static_cast<uint32_t>(std::size(deviceExtensions)), deviceExtensions,
            static_cast<uint32_t>(std::size(featureStructTypes)), featureStructTypes,
            featureDesc,
            static_cast<uint32_t>(std::size(propertyStructTypes)), propertyStructTypes,
            propertyDesc,
            0, nullptr,
            0, nullptr,
            0, nullptr,
            0, nullptr,
            chainerDesc,
        };
        static const uint32_t variantCount = static_cast<uint32_t>(std::size(variants));
    } // namespace baseline

    static const VpCapabilitiesDesc capabilities[] = {
        baseline::variantCount, baseline::variants,
    };
    static const uint32_t capabilityCount = static_cast<uint32_t>(std::size(capabilities));
} // namespace VP_LUNARG_TEST_PROFILE_A
#endif //VP_LUNARG_test_profile_a

#ifdef VP_LUNARG_test_profile_b
namespace VP_LUNARG_TEST_PROFILE_B {
    static const VpVariantDesc mergedCapabilities[] = {
        0, nullptr,
        static_cast<uint32_t>(std::size(deviceExtensions)), deviceExtensions,
        static_cast<uint32_t>(std::size(featureStructTypes)), featureStructTypes,
            featureDesc,
        0, nullptr,
            propertyDesc,
        0, nullptr,
        0, nullptr,
        0, nullptr,
        0, nullptr,
        chainerDesc,
    };

    namespace baseline {
        static const VpVariantDesc variants[] = {
            0, nullptr,
            static_cast<uint32_t>(std::size(deviceExtensions)), deviceExtensions,
            static_cast<uint32_t>(std::size(featureStructTypes)), featureStructTypes,
            featureDesc,
            static_cast<uint32_t>(std::size(propertyStructTypes)), propertyStructTypes,
            propertyDesc,
            0, nullptr,
            0, nullptr,
            0, nullptr,
            0, nullptr,
            chainerDesc,
        };
        static const uint32_t variantCount = static_cast<uint32_t>(std::size(variants));
    } // namespace baseline

    static const VpCapabilitiesDesc capabilities[] = {
        baseline::variantCount, baseline::variants,
    };
    static const uint32_t capabilityCount = static_cast<uint32_t>(std::size(capabilities));

    static const VpProfileProperties profiles[] = {
        {VP_LUNARG_TEST_PROFILE_A_NAME, VP_LUNARG_TEST_PROFILE_A_SPEC_VERSION},
    };
    static const uint32_t profileCount = static_cast<uint32_t>(std::size(profiles));
} // namespace VP_LUNARG_TEST_PROFILE_B
#endif //VP_LUNARG_test_profile_b

static const VpProfileDesc profiles[] = {
#ifdef VP_LUNARG_test_profile_a
    VpProfileDesc{
        VpProfileProperties{ VP_LUNARG_TEST_PROFILE_A_NAME, VP_LUNARG_TEST_PROFILE_A_SPEC_VERSION },
        VP_LUNARG_TEST_PROFILE_A_MIN_API_VERSION,
        VP_LUNARG_TEST_PROFILE_A::mergedCapabilities,
        0, nullptr,
        VP_LUNARG_TEST_PROFILE_A::capabilityCount, VP_LUNARG_TEST_PROFILE_A::capabilities,
        0, nullptr,
    },
#endif // VP_LUNARG_TEST_PROFILE_A
#ifdef VP_LUNARG_test_profile_b
    VpProfileDesc{
        VpProfileProperties{ VP_LUNARG_TEST_PROFILE_B_NAME, VP_LUNARG_TEST_PROFILE_B_SPEC_VERSION },
        VP_LUNARG_TEST_PROFILE_B_MIN_API_VERSION,
        VP_LUNARG_TEST_PROFILE_B::mergedCapabilities,
        VP_LUNARG_TEST_PROFILE_B::profileCount, VP_LUNARG_TEST_PROFILE_B::profiles,
        VP_LUNARG_TEST_PROFILE_B::capabilityCount, VP_LUNARG_TEST_PROFILE_B::capabilities,
        0, nullptr,
    },
#endif // VP_LUNARG_TEST_PROFILE_B
};
static const uint32_t profileCount = static_cast<uint32_t>(std::size(profiles));

VPAPI_ATTR const VpProfileDesc* vpGetProfileDesc(const char profileName[VP_MAX_PROFILE_NAME_SIZE]) {
    for (uint32_t i = 0; i < profileCount; ++i) {
        if (strncmp(profiles[i].props.profileName, profileName, VP_MAX_PROFILE_NAME_SIZE) == 0) return &profiles[i];
    }
    return nullptr;
}

VPAPI_ATTR std::vector<VpProfileProperties> GatherProfiles(const VpProfileProperties& profile) {
    std::vector<VpProfileProperties> profiles;

    const detail::VpProfileDesc* profile_desc = detail::vpGetProfileDesc(profile.profileName);

    for (uint32_t profile_index = 0; profile_index < profile_desc->requiredProfileCount; ++profile_index) {
        profiles.push_back(profile_desc->pRequiredProfiles[profile_index]);
    }

    profiles.push_back(profile);

    return profiles;
}

VPAPI_ATTR bool vpCheckVersion(uint32_t actual, uint32_t expected) {
    uint32_t actualMajor = VK_API_VERSION_MAJOR(actual);
    uint32_t actualMinor = VK_API_VERSION_MINOR(actual);
    uint32_t expectedMajor = VK_API_VERSION_MAJOR(expected);
    uint32_t expectedMinor = VK_API_VERSION_MINOR(expected);
    return actualMajor > expectedMajor || (actualMajor == expectedMajor && actualMinor >= expectedMinor);
}

VPAPI_ATTR bool vpCheckExtension(const VkExtensionProperties *supportedProperties, size_t supportedSize,
                                 const char *requestedExtension) {
    bool found = false;
    for (size_t i = 0, n = supportedSize; i < n; ++i) {
        if (strcmp(supportedProperties[i].extensionName, requestedExtension) == 0) {
            found = true;
            // Drivers don't actually update their spec version, so we cannot rely on this
            // if (supportedProperties[i].specVersion >= expectedVersion) found = true;
        }
    }
    return found;
}

VPAPI_ATTR void vpGetExtensions(uint32_t requestedExtensionCount, const char *const *ppRequestedExtensionNames,
                                uint32_t profileExtensionCount, const VkExtensionProperties *pProfileExtensionProperties,
                                std::vector<const char *> &extensions, bool merge, bool override) {
    if (override) {
        for (uint32_t i = 0; i < requestedExtensionCount; ++i) {
            extensions.push_back(ppRequestedExtensionNames[i]);
        }
    } else {
        for (uint32_t i = 0; i < profileExtensionCount; ++i) {
            extensions.push_back(pProfileExtensionProperties[i].extensionName);
        }

        if (merge) {
            for (uint32_t i = 0; i < requestedExtensionCount; ++i) {
                if (vpCheckExtension(pProfileExtensionProperties, profileExtensionCount, ppRequestedExtensionNames[i])) {
                    continue;
                }
                extensions.push_back(ppRequestedExtensionNames[i]);
            }
        }
    }
}

VPAPI_ATTR const void* vpGetStructure(const void* pNext, VkStructureType type) {
    const VkBaseOutStructure *p = static_cast<const VkBaseOutStructure*>(pNext);
    while (p != nullptr) {
        if (p->sType == type) return p;
        p = p->pNext;
    }
    return nullptr;
}

VPAPI_ATTR void* vpGetStructure(void* pNext, VkStructureType type) {
    VkBaseOutStructure *p = static_cast<VkBaseOutStructure*>(pNext);
    while (p != nullptr) {
        if (p->sType == type) return p;
        p = p->pNext;
    }
    return nullptr;
}

VPAPI_ATTR VkResult vpGetInstanceProfileSupportSingleProfile(
    uint32_t api_version,
    const std::vector<VkExtensionProperties>& supported_extensions,
    const VpProfileProperties* pProfile, VkBool32* pSupported) {
    assert(pProfile != nullptr);

#define VP_ENABLE_EARLY_EXIT

#ifdef VP_ENABLE_EARLY_EXIT
    const bool early_exit = true;
#else //VP_ENABLE_EARLY_EXIT
    const bool early_exit = false;
#endif //VP_ENABLE_EARLY_EXIT

    const detail::VpProfileDesc* pProfileDesc = detail::vpGetProfileDesc(pProfile->profileName);
    if (pProfileDesc == nullptr) {
        *pSupported = VK_FALSE;
        return VK_ERROR_UNKNOWN;
    }

    if (pProfileDesc->props.specVersion < pProfile->specVersion) {
        *pSupported = VK_FALSE;

        if (early_exit) {
            return VK_SUCCESS;
        }
    }

    // Required API version is built in root profile, not need to check dependent profile API versions
    if (api_version != 0) {
        if (!detail::vpCheckVersion(api_version, pProfileDesc->minApiVersion)) {
            const uint32_t version_min_major = VK_API_VERSION_MAJOR(pProfileDesc->minApiVersion);
            const uint32_t version_min_minor = VK_API_VERSION_MINOR(pProfileDesc->minApiVersion);
            const uint32_t version_min_patch = VK_API_VERSION_PATCH(pProfileDesc->minApiVersion);

            const uint32_t version_major = VK_API_VERSION_MAJOR(api_version);
            const uint32_t version_minor = VK_API_VERSION_MINOR(api_version);
            const uint32_t version_patch = VK_API_VERSION_PATCH(api_version);

            *pSupported = VK_FALSE;

            if (early_exit) {
                return VK_SUCCESS;
            }
        }
    }

    for (uint32_t capability_index = 0; capability_index < pProfileDesc->requiredCapabilityCount; ++capability_index) {
        const VpCapabilitiesDesc& capabilities_desc = pProfileDesc->pRequiredCapabilities[capability_index];

        VkBool32 supported_capabilities = VK_FALSE;
        for (uint32_t variant_index = 0; variant_index < capabilities_desc.variantCount; ++variant_index) {
            const VpVariantDesc& variant_desc = capabilities_desc.pVariants[variant_index];

            VkBool32 supported_variant = VK_TRUE;
            for (uint32_t i = 0; i < variant_desc.instanceExtensionCount; ++i) {
                if (!detail::vpCheckExtension(supported_extensions.data(), supported_extensions.size(),
                                              variant_desc.pInstanceExtensions[i].extensionName)) {
                    supported_variant = VK_FALSE;
                    if (early_exit) {
                        break;
                    }
                }
            }

            if (supported_variant == VK_TRUE) {
                supported_capabilities = VK_TRUE;
                if (early_exit) {
                    break;
                }
            }
        }

        if (supported_capabilities == VK_FALSE) {
            *pSupported = VK_FALSE;
            return VK_SUCCESS;
        }
    }

    return VK_SUCCESS;
}

} // namespace detail

VPAPI_ATTR VkResult vpGetProfiles(uint32_t *pPropertyCount, VpProfileProperties *pProperties) {
    VkResult result = VK_SUCCESS;

    if (pProperties == nullptr) {
        *pPropertyCount = detail::profileCount;
    } else {
        if (*pPropertyCount < detail::profileCount) {
            result = VK_INCOMPLETE;
        } else {
            *pPropertyCount = detail::profileCount;
        }
        for (uint32_t i = 0; i < *pPropertyCount; ++i) {
            pProperties[i] = detail::profiles[i].props;
        }
    }
    return result;
}

VPAPI_ATTR VkResult vpGetProfileRequiredProfiles(const VpProfileProperties *pProfile, uint32_t *pPropertyCount, VpProfileProperties *pProperties) {
    VkResult result = VK_SUCCESS;

    const detail::VpProfileDesc* pDesc = detail::vpGetProfileDesc(pProfile->profileName);
    if (pDesc == nullptr) return VK_ERROR_UNKNOWN;

    if (pProperties == nullptr) {
        *pPropertyCount = pDesc->requiredProfileCount;
    } else {
        if (*pPropertyCount < pDesc->requiredProfileCount) {
            result = VK_INCOMPLETE;
        } else {
            *pPropertyCount = pDesc->requiredProfileCount;
        }
        for (uint32_t i = 0; i < *pPropertyCount; ++i) {
            pProperties[i] = pDesc->pRequiredProfiles[i];
        }
    }
    return result;
}

VPAPI_ATTR uint32_t vpGetProfileAPIVersion(const VpProfileProperties* pProfile) {
    const std::vector<VpProfileProperties>& profiles = detail::GatherProfiles(*pProfile);

    uint32_t major = 0;
    uint32_t minor = 0;
    uint32_t patch = 0;

    for (std::size_t i = 0, n = profiles.size(); i < n; ++i) {
        const detail::VpProfileDesc* pDesc = detail::vpGetProfileDesc(profiles[i].profileName);
        if (pDesc == nullptr) return 0;

        major = std::max<uint32_t>(major, VK_API_VERSION_MAJOR(pDesc->minApiVersion));
        minor = std::max<uint32_t>(minor, VK_API_VERSION_MINOR(pDesc->minApiVersion));
        patch = std::max<uint32_t>(patch, VK_API_VERSION_PATCH(pDesc->minApiVersion));
    }

    return VK_MAKE_API_VERSION(0, major, minor, patch);
}

VPAPI_ATTR VkResult vpGetProfileFallbacks(const VpProfileProperties *pProfile, uint32_t *pPropertyCount, VpProfileProperties *pProperties) {
    VkResult result = VK_SUCCESS;

    const detail::VpProfileDesc* pDesc = detail::vpGetProfileDesc(pProfile->profileName);
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

VPAPI_ATTR VkResult vpHasMultipleVariantsProfile(const VpProfileProperties *pProfile, VkBool32 *pHasMultipleVariants) {
    const std::vector<VpProfileProperties>& profiles = detail::GatherProfiles(*pProfile);

    for (std::size_t profile_index = 0, profile_count = profiles.size(); profile_index < profile_count; ++profile_index) {
        const detail::VpProfileDesc* pDesc = detail::vpGetProfileDesc(profiles[profile_index].profileName);
        if (pDesc == nullptr) return VK_ERROR_UNKNOWN;

        for (uint32_t capabilities_index = 0, n = pDesc->requiredCapabilityCount; capabilities_index < n; ++capabilities_index) {
            if (pDesc->pRequiredCapabilities[capabilities_index].variantCount > 1) {
                *pHasMultipleVariants = VK_TRUE;
                return VK_SUCCESS;
            }
        }
    }

    *pHasMultipleVariants = VK_FALSE;
    return VK_SUCCESS;
}

VPAPI_ATTR VkResult vpGetInstanceProfileSupport(const char *pLayerName, const VpProfileProperties *pProfile, VkBool32 *pSupported) {
    VkResult result = VK_SUCCESS;

    uint32_t api_version = VK_MAKE_API_VERSION(0, 1, 0, 0);
    static PFN_vkEnumerateInstanceVersion pfnEnumerateInstanceVersion =
        (PFN_vkEnumerateInstanceVersion)vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceVersion");
    if (pfnEnumerateInstanceVersion != nullptr) {
        result = pfnEnumerateInstanceVersion(&api_version);
        if (result != VK_SUCCESS) {
            *pSupported = VK_FALSE;
            return result;
        }
    }

    uint32_t supported_instance_extension_count = 0;
    result = vkEnumerateInstanceExtensionProperties(pLayerName, &supported_instance_extension_count, nullptr);
    if (result != VK_SUCCESS) {
        *pSupported = VK_FALSE;
        return result;
    }
    std::vector<VkExtensionProperties> supported_instance_extensions;
    if (supported_instance_extension_count > 0) {
        supported_instance_extensions.resize(supported_instance_extension_count);
    }
    result = vkEnumerateInstanceExtensionProperties(pLayerName, &supported_instance_extension_count, supported_instance_extensions.data());
    if (result != VK_SUCCESS) {
        *pSupported = VK_FALSE;
        return result;
    }

#define VP_ENABLE_EARLY_EXIT

#ifdef VP_ENABLE_EARLY_EXIT
    const bool early_exit = true;
#else //VP_ENABLE_EARLY_EXIT
    const bool early_exit = false;
#endif //VP_ENABLE_EARLY_EXIT
    
    VkBool32 supported = VK_TRUE;
    
    // We require VK_KHR_get_physical_device_properties2 if we are on Vulkan 1.0
    if (api_version < VK_API_VERSION_1_1) {
        bool foundGPDP2 = false;
        for (size_t i = 0; i < supported_instance_extensions.size(); ++i) {
            if (strcmp(supported_instance_extensions[i].extensionName, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) == 0) {
                foundGPDP2 = true;
                break;
            }
        }
        if (!foundGPDP2) {
            supported = VK_FALSE;
            if (early_exit) {
                *pSupported = supported;
                return result;
            }
        }
    }

    const detail::VpProfileDesc* pProfileDesc = detail::vpGetProfileDesc(pProfile->profileName);
    if (pProfileDesc == nullptr) return VK_ERROR_UNKNOWN;

    result = detail::vpGetInstanceProfileSupportSingleProfile(api_version, supported_instance_extensions, pProfile, &supported);
    if (result != VK_SUCCESS || (supported == VK_FALSE && early_exit)) {
        *pSupported = supported;
        return result;
    }
 
    for (std::size_t i = 0; i < pProfileDesc->requiredProfileCount; ++i) {
        result = detail::vpGetInstanceProfileSupportSingleProfile(0, supported_instance_extensions, &pProfileDesc->pRequiredProfiles[i], &supported);
        if (result != VK_SUCCESS || (supported == VK_FALSE && early_exit)) {
            *pSupported = supported;
            return result;
        }
    }

    *pSupported = supported;
    return result;
}


VPAPI_ATTR VkResult vpCreateInstance(const VpInstanceCreateInfo *pCreateInfo,
                                     const VkAllocationCallbacks *pAllocator, VkInstance *pInstance) {
    VkInstanceCreateInfo createInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
    std::vector<const char*> extensions;
    VkInstanceCreateInfo* pInstanceCreateInfo = nullptr;

    if (pCreateInfo != nullptr && pCreateInfo->pCreateInfo != nullptr) {
        createInfo = *pCreateInfo->pCreateInfo;
        pInstanceCreateInfo = &createInfo;

        const detail::VpProfileDesc* profile_desc = nullptr;
        if (pCreateInfo->pProfile != nullptr) {
            // Unknown profile
            profile_desc = detail::vpGetProfileDesc(pCreateInfo->pProfile->profileName);
            if (profile_desc == nullptr) return VK_ERROR_UNKNOWN;

            // Multiple variants profile, not supported
            VkBool32 multiple_variants = VK_FALSE;
            VkResult result = vpHasMultipleVariantsProfile(pCreateInfo->pProfile, &multiple_variants);
            if (result != VK_SUCCESS || multiple_variants == VK_TRUE) return VK_ERROR_UNKNOWN;
        }

        if (createInfo.pApplicationInfo == nullptr) {
            if (profile_desc != nullptr) {
                appInfo.apiVersion = profile_desc->minApiVersion;
            }
            createInfo.pApplicationInfo = &appInfo;
        }

        const std::vector<VpProfileProperties>& profiles = detail::GatherProfiles(*pCreateInfo->pProfile);

        for (std::size_t profile_index = 0, profile_count = profiles.size(); profile_index < profile_count; ++profile_index) {
            const char* profile_name = profiles[profile_index].profileName;
            const detail::VpProfileDesc* profile_desc = detail::vpGetProfileDesc(profile_name);

            for (std::size_t caps_index = 0, caps_count = profiles.size(); caps_index < caps_count; ++caps_index) {
                const detail::VpCapabilitiesDesc* caps_desc = &profile_desc->pRequiredCapabilities[caps_index];

                assert(caps_desc->variantCount == 1);
                const detail::VpVariantDesc* variant = caps_desc->pVariants;

                if (variant != nullptr && variant->pInstanceExtensions != nullptr) {
                    bool merge = (pCreateInfo->flags & VP_INSTANCE_CREATE_MERGE_EXTENSIONS_BIT) != 0;
                    bool override = (pCreateInfo->flags & VP_INSTANCE_CREATE_OVERRIDE_EXTENSIONS_BIT) != 0;

                    if (!merge && !override && pCreateInfo->pCreateInfo->enabledExtensionCount > 0) {
                        // If neither merge nor override is used then the application must not specify its
                        // own extensions
                        return VK_ERROR_UNKNOWN;
                    }

                    detail::vpGetExtensions(pCreateInfo->pCreateInfo->enabledExtensionCount,
                                            pCreateInfo->pCreateInfo->ppEnabledExtensionNames, variant->instanceExtensionCount,
                                            variant->pInstanceExtensions, extensions, merge, override);

                    {
                        bool foundPortEnum = false;
                        for (size_t i = 0; i < extensions.size(); ++i) {
                            if (strcmp(extensions[i], VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0) {
                                foundPortEnum = true;
                                break;
                            }
                        }
                        if (foundPortEnum) {
                            createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
                        }
                    }

                    // Need to include VK_KHR_get_physical_device_properties2 if we are on Vulkan 1.0
                    if (createInfo.pApplicationInfo->apiVersion < VK_API_VERSION_1_1) {
                        bool foundGPDP2 = false;
                        for (size_t i = 0; i < extensions.size(); ++i) {
                            if (strcmp(extensions[i], VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) == 0) {
                                foundGPDP2 = true;
                                break;
                            }
                        }
                        if (!foundGPDP2) {
                            extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
                        }
                    }
                }
            }
        }

        if (!extensions.empty()) {
            createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
            createInfo.ppEnabledExtensionNames = extensions.data();
        }
    }

#ifdef __APPLE__
    bool has_portability_ext = false;
    for (std::size_t i = 0, n = extensions.size(); i < n; ++i) {
        if (strcmp(extensions[i], VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0) {
            has_portability_ext = true;
            break;
        }
    }

    if (pInstanceCreateInfo != nullptr && has_portability_ext) {
        pInstanceCreateInfo->flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    }
#endif

    return vkCreateInstance(pInstanceCreateInfo, pAllocator, pInstance);
}

VPAPI_ATTR VkResult vpGetPhysicalDeviceProfileSupport(VkInstance instance, VkPhysicalDevice physicalDevice,
                                                      const VpProfileProperties *pProfile, VkBool32 *pSupported) {
    VkResult result = VK_SUCCESS;

    uint32_t supported_device_extension_count = 0;
    result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &supported_device_extension_count, nullptr);
    if (result != VK_SUCCESS) {
        return result;
    }
    std::vector<VkExtensionProperties> supported_device_extensions;
    if (supported_device_extension_count > 0) {
        supported_device_extensions.resize(supported_device_extension_count);
    }
    result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &supported_device_extension_count, supported_device_extensions.data());
    if (result != VK_SUCCESS) {
        return result;
    }

    // Workaround old loader bug where count could be smaller on the second call to vkEnumerateDeviceExtensionProperties
    if (supported_device_extension_count > 0) {
        supported_device_extensions.resize(supported_device_extension_count);
    }

    const detail::VpProfileDesc* pProfileDesc = detail::vpGetProfileDesc(pProfile->profileName);
    if (pProfileDesc == nullptr) return VK_ERROR_UNKNOWN;

    struct GPDP2EntryPoints {
        PFN_vkGetPhysicalDeviceFeatures2KHR                 pfnGetPhysicalDeviceFeatures2;
        PFN_vkGetPhysicalDeviceProperties2KHR               pfnGetPhysicalDeviceProperties2;
        PFN_vkGetPhysicalDeviceFormatProperties2KHR         pfnGetPhysicalDeviceFormatProperties2;
        PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR    pfnGetPhysicalDeviceQueueFamilyProperties2;
    };

    struct UserData {
        VkPhysicalDevice physicalDevice;
        const detail::VpVariantDesc* variant;
        GPDP2EntryPoints gpdp2;
        uint32_t index;
        uint32_t count;
        detail::PFN_vpStructChainerCb pfnCb;
        bool supported;
    } userData{physicalDevice};

    // Attempt to load core versions of the GPDP2 entry points
    userData.gpdp2.pfnGetPhysicalDeviceFeatures2 =
        (PFN_vkGetPhysicalDeviceFeatures2KHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFeatures2");
    userData.gpdp2.pfnGetPhysicalDeviceProperties2 =
        (PFN_vkGetPhysicalDeviceProperties2KHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties2");
    userData.gpdp2.pfnGetPhysicalDeviceFormatProperties2 =
        (PFN_vkGetPhysicalDeviceFormatProperties2KHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFormatProperties2");
    userData.gpdp2.pfnGetPhysicalDeviceQueueFamilyProperties2 =
        (PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties2");

    // If not successful, try to load KHR variant
    if (userData.gpdp2.pfnGetPhysicalDeviceFeatures2 == nullptr) {
        userData.gpdp2.pfnGetPhysicalDeviceFeatures2 =
            (PFN_vkGetPhysicalDeviceFeatures2KHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFeatures2KHR");
        userData.gpdp2.pfnGetPhysicalDeviceProperties2 =
            (PFN_vkGetPhysicalDeviceProperties2KHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties2KHR");
        userData.gpdp2.pfnGetPhysicalDeviceFormatProperties2 =
            (PFN_vkGetPhysicalDeviceFormatProperties2KHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFormatProperties2KHR");
        userData.gpdp2.pfnGetPhysicalDeviceQueueFamilyProperties2 =
            (PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties2KHR");
    }

    if (userData.gpdp2.pfnGetPhysicalDeviceFeatures2 == nullptr ||
        userData.gpdp2.pfnGetPhysicalDeviceProperties2 == nullptr ||
        userData.gpdp2.pfnGetPhysicalDeviceFormatProperties2 == nullptr ||
        userData.gpdp2.pfnGetPhysicalDeviceQueueFamilyProperties2 == nullptr) {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }


#define VP_ENABLE_EARLY_EXIT

#ifdef VP_ENABLE_EARLY_EXIT
    const bool early_exit = true;
#else //VP_ENABLE_EARLY_EXIT
    const bool early_exit = false;
#endif //VP_ENABLE_EARLY_EXIT

    VkBool32 supported = VK_TRUE;

    const std::vector<VpProfileProperties>& profiles = detail::GatherProfiles(*pProfile);

    for (std::size_t i = 0, n = profiles.size(); i < n; ++i) {
        const char* profile_name = profiles[i].profileName;

        const detail::VpProfileDesc* pProfileDesc = detail::vpGetProfileDesc(profile_name);
        if (pProfileDesc == nullptr) return VK_ERROR_UNKNOWN;

        if (pProfileDesc->props.specVersion < pProfile->specVersion) {
            *pSupported = VK_FALSE;
            return VK_SUCCESS;
        }

        {
            VkPhysicalDeviceProperties props{};
            vkGetPhysicalDeviceProperties(physicalDevice, &props);
            if (!detail::vpCheckVersion(props.apiVersion, pProfileDesc->minApiVersion)) {
                *pSupported = VK_FALSE;
                return VK_SUCCESS;
            }
        }

        for (uint32_t required_capability_index = 0; required_capability_index < pProfileDesc->requiredCapabilityCount; ++required_capability_index) {
            const detail::VpCapabilitiesDesc* required_capabilities = &pProfileDesc->pRequiredCapabilities[required_capability_index];

            for (uint32_t required_variant_index = 0; required_variant_index < required_capabilities->variantCount; ++required_variant_index) {
                const detail::VpVariantDesc* variant = &required_capabilities->pVariants[required_variant_index];

                for (uint32_t i = 0; i < variant->deviceExtensionCount; ++i) {
                    const char *requested_extension = variant->pDeviceExtensions[i].extensionName;
                    if (!detail::vpCheckExtension(supported_device_extensions.data(), supported_device_extensions.size(), requested_extension)) {
                        *pSupported = VK_FALSE;
                        return VK_SUCCESS;
                    }
                }
            }
        }

        for (uint32_t required_capability_index = 0; required_capability_index < pProfileDesc->requiredCapabilityCount; ++required_capability_index) {
            const detail::VpCapabilitiesDesc* required_capabilities = &pProfileDesc->pRequiredCapabilities[required_capability_index];

            for (uint32_t required_variant_index = 0; required_variant_index < required_capabilities->variantCount; ++required_variant_index) {
                userData.variant = &required_capabilities->pVariants[required_variant_index];

                VkPhysicalDeviceFeatures2KHR features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR };
                userData.variant->chainers.pfnFeature(
                    static_cast<VkBaseOutStructure*>(static_cast<void*>(&features)), &userData,
                    [](VkBaseOutStructure* p, void* pUser) {
                        UserData* pUserData = static_cast<UserData*>(pUser);
                        pUserData->gpdp2.pfnGetPhysicalDeviceFeatures2(pUserData->physicalDevice,
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
                    *pSupported = VK_FALSE;
                    return VK_SUCCESS;
                }
            }
        }

        for (uint32_t required_capability_index = 0; required_capability_index < pProfileDesc->requiredCapabilityCount; ++required_capability_index) {
            const detail::VpCapabilitiesDesc* required_capabilities = &pProfileDesc->pRequiredCapabilities[required_capability_index];

            for (uint32_t required_variant_index = 0; required_variant_index < required_capabilities->variantCount; ++required_variant_index) {
                userData.variant = &required_capabilities->pVariants[required_variant_index];
        
                VkPhysicalDeviceProperties2KHR props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR };
                userData.variant->chainers.pfnProperty(
                    static_cast<VkBaseOutStructure*>(static_cast<void*>(&props)), &userData,
                    [](VkBaseOutStructure* p, void* pUser) {
                        UserData* pUserData = static_cast<UserData*>(pUser);
                        pUserData->gpdp2.pfnGetPhysicalDeviceProperties2(pUserData->physicalDevice,
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
                    *pSupported = VK_FALSE;
                    return VK_SUCCESS;
                }
            }
        }

        for (uint32_t required_capability_index = 0; required_capability_index < pProfileDesc->requiredCapabilityCount; ++required_capability_index) {
            const detail::VpCapabilitiesDesc* required_capabilities = &pProfileDesc->pRequiredCapabilities[required_capability_index];

            for (uint32_t required_variant_index = 0; required_variant_index < required_capabilities->variantCount; ++required_variant_index) {
                userData.variant = &required_capabilities->pVariants[required_variant_index];
        
                for (uint32_t i = 0; i < userData.variant->formatCount; ++i) {
                    userData.index = i;
                    VkFormatProperties2KHR props{ VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR };
                    userData.variant->chainers.pfnFormat(
                        static_cast<VkBaseOutStructure*>(static_cast<void*>(&props)), &userData,
                        [](VkBaseOutStructure* p, void* pUser) {
                            UserData* pUserData = static_cast<UserData*>(pUser);
                            pUserData->gpdp2.pfnGetPhysicalDeviceFormatProperties2(pUserData->physicalDevice, pUserData->variant->pFormats[pUserData->index].format,
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
                        *pSupported = VK_FALSE;
                        return VK_SUCCESS;
                    }
                }
            }
        }

        for (uint32_t required_capability_index = 0; required_capability_index < pProfileDesc->requiredCapabilityCount; ++required_capability_index) {
            const detail::VpCapabilitiesDesc* required_capabilities = &pProfileDesc->pRequiredCapabilities[required_capability_index];

            for (uint32_t required_variant_index = 0; required_variant_index < required_capabilities->variantCount; ++required_variant_index) {
                userData.variant = &required_capabilities->pVariants[required_variant_index];
            
                userData.gpdp2.pfnGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &userData.count, nullptr);
                std::vector<VkQueueFamilyProperties2KHR> props(userData.count, { VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2_KHR });
                userData.index = 0;

                detail::PFN_vpStructChainerCb callback = [](VkBaseOutStructure* p, void* pUser) {
                    UserData* pUserData = static_cast<UserData*>(pUser);
                    VkQueueFamilyProperties2KHR* pProps = static_cast<VkQueueFamilyProperties2KHR*>(static_cast<void*>(p));
                    if (++pUserData->index < pUserData->count) {
                        pUserData->variant->chainers.pfnQueueFamily(static_cast<VkBaseOutStructure*>(static_cast<void*>(++pProps)),
                                                                    pUser, pUserData->pfnCb);
                    } else {
                        pProps -= pUserData->count - 1;
                        pUserData->gpdp2.pfnGetPhysicalDeviceQueueFamilyProperties2(pUserData->physicalDevice,
                                                                                    &pUserData->count, pProps);
                        pUserData->supported = true;

                        // Check first that each queue family defined is supported by the device
                        for (uint32_t i = 0; i < pUserData->variant->queueFamilyCount; ++i) {
                            bool found = false;
                            for (uint32_t j = 0; j < pUserData->count; ++j) {
                                bool propsMatch = true;
                                p = static_cast<VkBaseOutStructure*>(static_cast<void*>(&pProps[j]));
                                while (p != nullptr) {
                                    if (!pUserData->variant->pQueueFamilies[i].pfnComparator(p)) {
                                        propsMatch = false;
                                        break;
                                    }
                                    p = p->pNext;
                                }
                                if (propsMatch) {
                                    found = true;
                                    break;
                                }
                            }
                            if (!found) {
                                pUserData->supported = false;
                                return;
                            }
                        }

                        // Then check each permutation to ensure that while order of the queue families
                        // doesn't matter, each queue family property criteria is matched with a separate
                        // queue family of the actual device
                        std::vector<uint32_t> permutation(pUserData->count);
                        for (uint32_t i = 0; i < pUserData->count; ++i) {
                            permutation[i] = i;
                        }
                        bool found = false;
                        do {
                            bool propsMatch = true;
                            for (uint32_t i = 0; i < pUserData->variant->queueFamilyCount && propsMatch; ++i) {
                                p = static_cast<VkBaseOutStructure*>(static_cast<void*>(&pProps[permutation[i]]));
                                while (p != nullptr) {
                                    if (!pUserData->variant->pQueueFamilies[i].pfnComparator(p)) {
                                        propsMatch = false;
                                        break;
                                    }
                                    p = p->pNext;
                                }
                            }
                            if (propsMatch) {
                                found = true;
                                break;
                            }
                        } while (std::next_permutation(permutation.begin(), permutation.end()));

                        if (!found) {
                            pUserData->supported = false;
                        }
                    }
                };
                userData.pfnCb = callback;

                if (userData.count >= userData.variant->queueFamilyCount) {
                    userData.variant->chainers.pfnQueueFamily(static_cast<VkBaseOutStructure*>(static_cast<void*>(props.data())),
                                                              &userData, callback);
                    if (!userData.supported) {
                        *pSupported = VK_FALSE;
                        return VK_SUCCESS;
                    }
                } else {
                    *pSupported = VK_FALSE;
                    return VK_SUCCESS;
                }
            }
        }
    }

    *pSupported = VK_TRUE;
    return VK_SUCCESS;
}

VPAPI_ATTR VkResult vpCreateDevice(VkPhysicalDevice physicalDevice, const VpDeviceCreateInfo *pCreateInfo,
                                   const VkAllocationCallbacks *pAllocator, VkDevice *pDevice) {
    if (physicalDevice == VK_NULL_HANDLE || pCreateInfo == nullptr || pDevice == nullptr) {
        return vkCreateDevice(physicalDevice, pCreateInfo == nullptr ? nullptr : pCreateInfo->pCreateInfo, pAllocator, pDevice);
    }

    const detail::VpProfileDesc* pProfileDesc = detail::vpGetProfileDesc(pCreateInfo->pProfile->profileName);
    if (pProfileDesc == nullptr) return VK_ERROR_UNKNOWN;

    // Multiple variants profile, not supported
    VkBool32 multiple_variants = VK_FALSE;
    VkResult result = vpHasMultipleVariantsProfile(pCreateInfo->pProfile, &multiple_variants);
    if (result != VK_SUCCESS || multiple_variants == VK_TRUE) return VK_ERROR_UNKNOWN;

    const detail::VpVariantDesc* variant = pProfileDesc->pMergedCapabilities;
    
    struct UserData {
        VkPhysicalDevice                physicalDevice;
        const detail::VpVariantDesc*    variant;
        const VpDeviceCreateInfo*       pCreateInfo;
        const VkAllocationCallbacks*    pAllocator;
        VkDevice*                       pDevice;
        VkResult                        result;
    } userData{ physicalDevice, variant, pCreateInfo, pAllocator, pDevice };

    VkPhysicalDeviceFeatures2KHR features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR };
    variant->chainers.pfnFeature(static_cast<VkBaseOutStructure*>(static_cast<void*>(&features)), &userData,
        [](VkBaseOutStructure* p, void* pUser) {
            UserData* pUserData = static_cast<UserData*>(pUser);
            const detail::VpVariantDesc* variant = pUserData->variant;
            const VpDeviceCreateInfo* pCreateInfo = pUserData->pCreateInfo;

            bool merge = (pCreateInfo->flags & VP_DEVICE_CREATE_MERGE_EXTENSIONS_BIT) != 0;
            bool override = (pCreateInfo->flags & VP_DEVICE_CREATE_OVERRIDE_EXTENSIONS_BIT) != 0;

            if (!merge && !override && pCreateInfo->pCreateInfo->enabledExtensionCount > 0) {
                // If neither merge nor override is used then the application must not specify its
                // own extensions
                pUserData->result = VK_ERROR_UNKNOWN;
                return;
            }

            std::vector<const char*> extensions;
            detail::vpGetExtensions(pCreateInfo->pCreateInfo->enabledExtensionCount,
                                    pCreateInfo->pCreateInfo->ppEnabledExtensionNames,
                                    variant->deviceExtensionCount,
                                    variant->pDeviceExtensions,
                                    extensions, merge, override);

            VkBaseOutStructure profileStructList;
            profileStructList.pNext = p;
            VkPhysicalDeviceFeatures2KHR* pFeatures = static_cast<VkPhysicalDeviceFeatures2KHR*>(static_cast<void*>(p));
            if (variant->feature.pfnFiller != nullptr) {
                while (p != nullptr) {
                    variant->feature.pfnFiller(p);
                    p = p->pNext;
                }
            }

            if (pCreateInfo->pCreateInfo->pEnabledFeatures != nullptr) {
                pFeatures->features = *pCreateInfo->pCreateInfo->pEnabledFeatures;
            }

            if (pCreateInfo->flags & VP_DEVICE_CREATE_DISABLE_ROBUST_BUFFER_ACCESS_BIT) {
                pFeatures->features.robustBufferAccess = VK_FALSE;
            }

#ifdef VK_EXT_robustness2
            VkPhysicalDeviceRobustness2FeaturesEXT* pRobustness2FeaturesEXT = static_cast<VkPhysicalDeviceRobustness2FeaturesEXT*>(
                detail::vpGetStructure(pFeatures, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT));
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
            VkPhysicalDeviceImageRobustnessFeaturesEXT* pImageRobustnessFeaturesEXT = static_cast<VkPhysicalDeviceImageRobustnessFeaturesEXT*>(
                detail::vpGetStructure(pFeatures, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES_EXT));
            if (pImageRobustnessFeaturesEXT != nullptr && (pCreateInfo->flags & VP_DEVICE_CREATE_DISABLE_ROBUST_IMAGE_ACCESS_BIT)) {
                pImageRobustnessFeaturesEXT->robustImageAccess = VK_FALSE;
            }
#endif

#ifdef VK_VERSION_1_3
            VkPhysicalDeviceVulkan13Features* pVulkan13Features = static_cast<VkPhysicalDeviceVulkan13Features*>(
                detail::vpGetStructure(pFeatures, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES));
            if (pVulkan13Features != nullptr && (pCreateInfo->flags & VP_DEVICE_CREATE_DISABLE_ROBUST_IMAGE_ACCESS_BIT)) {
                pVulkan13Features->robustImageAccess = VK_FALSE;
            }
#endif

            VkBaseOutStructure* pNext = static_cast<VkBaseOutStructure*>(const_cast<void*>(pCreateInfo->pCreateInfo->pNext));
            if ((pCreateInfo->flags & VP_DEVICE_CREATE_OVERRIDE_ALL_FEATURES_BIT) == 0) {
                for (uint32_t i = 0; i < variant->featureStructTypeCount; ++i) {
                    const void* pRequested = detail::vpGetStructure(pNext, variant->pFeatureStructTypes[i]);
                    if (pRequested == nullptr) {
                        VkBaseOutStructure* pPrevStruct = &profileStructList;
                        VkBaseOutStructure* pCurrStruct = pPrevStruct->pNext;
                        while (pCurrStruct->sType != variant->pFeatureStructTypes[i]) {
                            pPrevStruct = pCurrStruct;
                            pCurrStruct = pCurrStruct->pNext;
                        }
                        pPrevStruct->pNext = pCurrStruct->pNext;
                        pCurrStruct->pNext = pNext;
                        pNext = pCurrStruct;
                    } else
                    if ((pCreateInfo->flags & VP_DEVICE_CREATE_OVERRIDE_FEATURES_BIT) == 0) {
                        // If override is not used then the application must not specify its
                        // own feature structure for anything that the profile defines
                        pUserData->result = VK_ERROR_UNKNOWN;
                        return;
                    }
                }
            }

            VkDeviceCreateInfo createInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
            createInfo.pNext = pNext;
            createInfo.queueCreateInfoCount = pCreateInfo->pCreateInfo->queueCreateInfoCount;
            createInfo.pQueueCreateInfos = pCreateInfo->pCreateInfo->pQueueCreateInfos;
            createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
            createInfo.ppEnabledExtensionNames = extensions.data();
            if (pCreateInfo->flags & VP_DEVICE_CREATE_OVERRIDE_ALL_FEATURES_BIT) {
                createInfo.pEnabledFeatures = pCreateInfo->pCreateInfo->pEnabledFeatures;
            }
            pUserData->result = vkCreateDevice(pUserData->physicalDevice, &createInfo, pUserData->pAllocator, pUserData->pDevice);
        }
    );

    return userData.result;
}

VPAPI_ATTR VkResult vpGetProfileInstanceExtensionProperties(const VpProfileProperties *pProfile, uint32_t *pPropertyCount, VkExtensionProperties *pProperties) {
    // Multiple variants profile, not supported
    VkBool32 multiple_variants = VK_FALSE;
    VkResult result = vpHasMultipleVariantsProfile(pProfile, &multiple_variants);
    if (result != VK_SUCCESS || multiple_variants == VK_TRUE) return VK_ERROR_UNKNOWN;

    std::vector<VkExtensionProperties> results;

    const std::vector<VpProfileProperties>& profiles = detail::GatherProfiles(*pProfile);

    for (std::size_t profile_index = 0, profile_count = profiles.size(); profile_index < profile_count; ++profile_index) {
        const detail::VpProfileDesc* profile_desc = detail::vpGetProfileDesc(profiles[profile_index].profileName);
        if (profile_desc == nullptr) return VK_ERROR_UNKNOWN;

        for (uint32_t capability_index = 0; capability_index < profile_desc->requiredCapabilityCount; ++capability_index) {
            const detail::VpCapabilitiesDesc& capabilities = profile_desc->pRequiredCapabilities[capability_index];

            for (uint32_t variant_index = 0; variant_index < capabilities.variantCount; ++variant_index) {
                const detail::VpVariantDesc& variant = capabilities.pVariants[variant_index];

                for (uint32_t i = 0; i < variant.instanceExtensionCount; ++i) {
                    results.push_back(variant.pInstanceExtensions[i]);
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

VPAPI_ATTR VkResult vpGetProfileDeviceExtensionProperties(const VpProfileProperties *pProfile, uint32_t *pPropertyCount, VkExtensionProperties *pProperties) {
    // Multiple variants profile, not supported
    VkBool32 multiple_variants = VK_FALSE;
    VkResult result = vpHasMultipleVariantsProfile(pProfile, &multiple_variants);
    if (result != VK_SUCCESS || multiple_variants == VK_TRUE) return VK_ERROR_UNKNOWN;

    std::vector<VkExtensionProperties> results;

    const std::vector<VpProfileProperties>& profiles = detail::GatherProfiles(*pProfile);

    for (std::size_t profile_index = 0, profile_count = profiles.size(); profile_index < profile_count; ++profile_index) {
        const detail::VpProfileDesc* profile_desc = detail::vpGetProfileDesc(profiles[profile_index].profileName);
        if (profile_desc == nullptr) return VK_ERROR_UNKNOWN;

        for (uint32_t capability_index = 0; capability_index < profile_desc->requiredCapabilityCount; ++capability_index) {
            const detail::VpCapabilitiesDesc& capabilities = profile_desc->pRequiredCapabilities[capability_index];

            for (uint32_t variant_index = 0; variant_index < capabilities.variantCount; ++variant_index) {
                const detail::VpVariantDesc& variant = capabilities.pVariants[variant_index];

                for (uint32_t i = 0; i < variant.deviceExtensionCount; ++i) {
                    results.push_back(variant.pDeviceExtensions[i]);
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

VPAPI_ATTR VkResult vpGetProfileFeatures(const VpProfileProperties *pProfile, void *pNext) {
    // Multiple variants profile, not supported
    VkBool32 multiple_variants = VK_FALSE;
    VkResult result = vpHasMultipleVariantsProfile(pProfile, &multiple_variants);
    if (result != VK_SUCCESS || multiple_variants == VK_TRUE) return VK_ERROR_UNKNOWN;

    const std::vector<VpProfileProperties>& profiles = detail::GatherProfiles(*pProfile);

    for (std::size_t profile_index = 0, profile_count = profiles.size(); profile_index < profile_count; ++profile_index) {
        const detail::VpProfileDesc* profile_desc = detail::vpGetProfileDesc(profiles[profile_index].profileName);
        if (profile_desc == nullptr) return VK_ERROR_UNKNOWN;

        for (uint32_t capability_index = 0; capability_index < profile_desc->requiredCapabilityCount; ++capability_index) {
            const detail::VpCapabilitiesDesc& capabilities = profile_desc->pRequiredCapabilities[capability_index];

            for (uint32_t variant_index = 0; variant_index < capabilities.variantCount; ++variant_index) {
                const detail::VpVariantDesc& variant = capabilities.pVariants[variant_index];
                if (variant.feature.pfnFiller == nullptr) continue;

                VkBaseOutStructure* p = static_cast<VkBaseOutStructure*>(pNext);
                while (p != nullptr) {
                    variant.feature.pfnFiller(p);
                    p = p->pNext;
                }
            }
        }
    }

    return VK_SUCCESS;
}

VPAPI_ATTR VkResult vpGetProfileFeatureStructureTypes(const VpProfileProperties *pProfile, uint32_t *pStructureTypeCount, VkStructureType *pStructureTypes) {
    // Multiple variants profile, not supported
    VkBool32 multiple_variants = VK_FALSE;
    VkResult result = vpHasMultipleVariantsProfile(pProfile, &multiple_variants);
    if (result != VK_SUCCESS || multiple_variants == VK_TRUE) return VK_ERROR_UNKNOWN;

    std::vector<VkStructureType> results;

    const std::vector<VpProfileProperties>& profiles = detail::GatherProfiles(*pProfile);

    for (std::size_t profile_index = 0, profile_count = profiles.size(); profile_index < profile_count; ++profile_index) {
        const detail::VpProfileDesc* profile_desc = detail::vpGetProfileDesc(profiles[profile_index].profileName);
        if (profile_desc == nullptr) return VK_ERROR_UNKNOWN;

        for (uint32_t capability_index = 0; capability_index < profile_desc->requiredCapabilityCount; ++capability_index) {
            const detail::VpCapabilitiesDesc& capabilities = profile_desc->pRequiredCapabilities[capability_index];

            for (uint32_t variant_index = 0; variant_index < capabilities.variantCount; ++variant_index) {
                const detail::VpVariantDesc& variant = capabilities.pVariants[variant_index];

                for (uint32_t i = 0; i < variant.featureStructTypeCount; ++i) {
                    const VkStructureType type = variant.pFeatureStructTypes[i];
                    if (std::find(results.begin(), results.end(), type) == std::end(results)) {
                        results.push_back(type);
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

VPAPI_ATTR VkResult vpGetProfileProperties(const VpProfileProperties *pProfile, void *pNext) {
    // Multiple variants profile, not supported
    VkBool32 multiple_variants = VK_FALSE;
    VkResult result = vpHasMultipleVariantsProfile(pProfile, &multiple_variants);
    if (result != VK_SUCCESS || multiple_variants == VK_TRUE) return VK_ERROR_UNKNOWN;

    const std::vector<VpProfileProperties>& profiles = detail::GatherProfiles(*pProfile);

    for (std::size_t profile_index = 0, profile_count = profiles.size(); profile_index < profile_count; ++profile_index) {
        const detail::VpProfileDesc* profile_desc = detail::vpGetProfileDesc(profiles[profile_index].profileName);
        if (profile_desc == nullptr) return VK_ERROR_UNKNOWN;

        for (uint32_t capability_index = 0; capability_index < profile_desc->requiredCapabilityCount; ++capability_index) {
            const detail::VpCapabilitiesDesc& capabilities = profile_desc->pRequiredCapabilities[capability_index];

            for (uint32_t variant_index = 0; variant_index < capabilities.variantCount; ++variant_index) {
                const detail::VpVariantDesc& variant = capabilities.pVariants[variant_index];
                if (variant.property.pfnFiller == nullptr) continue;
                
                VkBaseOutStructure* p = static_cast<VkBaseOutStructure*>(pNext);
                while (p != nullptr) {
                    variant.property.pfnFiller(p);
                    p = p->pNext;
                }
            }
        }
    }
    
    return VK_SUCCESS;
}

VPAPI_ATTR VkResult vpGetProfilePropertyStructureTypes(const VpProfileProperties *pProfile, uint32_t *pStructureTypeCount, VkStructureType *pStructureTypes) {
    // Multiple variants profile, not supported
    VkBool32 multiple_variants = VK_FALSE;
    VkResult result = vpHasMultipleVariantsProfile(pProfile, &multiple_variants);
    if (result != VK_SUCCESS || multiple_variants == VK_TRUE) return VK_ERROR_UNKNOWN;

    std::vector<VkStructureType> results;

    const std::vector<VpProfileProperties>& profiles = detail::GatherProfiles(*pProfile);

    for (std::size_t profile_index = 0, profile_count = profiles.size(); profile_index < profile_count; ++profile_index) {
        const detail::VpProfileDesc* profile_desc = detail::vpGetProfileDesc(profiles[profile_index].profileName);
        if (profile_desc == nullptr) return VK_ERROR_UNKNOWN;

        for (uint32_t capability_index = 0; capability_index < profile_desc->requiredCapabilityCount; ++capability_index) {
            const detail::VpCapabilitiesDesc& capabilities = profile_desc->pRequiredCapabilities[capability_index];

            for (uint32_t variant_index = 0; variant_index < capabilities.variantCount; ++variant_index) {
                const detail::VpVariantDesc& variant = capabilities.pVariants[variant_index];

                for (uint32_t i = 0; i < variant.propertyStructTypeCount; ++i) {
                    const VkStructureType type = variant.pPropertyStructTypes[i];
                    if (std::find(results.begin(), results.end(), type) == std::end(results)) {
                        results.push_back(type);
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

VPAPI_ATTR VkResult vpGetProfileQueueFamilyProperties(const VpProfileProperties *pProfile, uint32_t *pPropertyCount, VkQueueFamilyProperties2KHR *pProperties) {
    // Multiple variants profile, not supported
    VkBool32 multiple_variants = VK_FALSE;
    VkResult result = vpHasMultipleVariantsProfile(pProfile, &multiple_variants);
    if (result != VK_SUCCESS || multiple_variants == VK_TRUE) return VK_ERROR_UNKNOWN;

    const std::vector<VpProfileProperties>& profiles = detail::GatherProfiles(*pProfile);
    uint32_t queue_family_count = 0;

    for (std::size_t profile_index = 0, profile_count = profiles.size(); profile_index < profile_count; ++profile_index) {
        const detail::VpProfileDesc* profile_desc = detail::vpGetProfileDesc(profiles[profile_index].profileName);
        if (profile_desc == nullptr) return VK_ERROR_UNKNOWN;

        for (uint32_t capability_index = 0; capability_index < profile_desc->requiredCapabilityCount; ++capability_index) {
            const detail::VpCapabilitiesDesc& capabilities = profile_desc->pRequiredCapabilities[capability_index];

            for (uint32_t variant_index = 0; variant_index < capabilities.variantCount; ++variant_index) {
                const detail::VpVariantDesc& variant = capabilities.pVariants[variant_index];
                queue_family_count += variant.queueFamilyCount;
            }
        }
    }

    if (pProperties == nullptr) {
        *pPropertyCount = queue_family_count;
    } else {
        if (*pPropertyCount < queue_family_count) {
            result = VK_INCOMPLETE;
        } else {
            *pPropertyCount = queue_family_count;
        }

        if (*pPropertyCount > 0) {
            for (std::size_t profile_index = 0, profile_count = profiles.size(); profile_index < profile_count; ++profile_index) {
                const detail::VpProfileDesc& profile_desc = *detail::vpGetProfileDesc(profiles[profile_index].profileName);

                for (uint32_t capability_index = 0; capability_index < profile_desc.requiredCapabilityCount; ++capability_index) {
                    const detail::VpCapabilitiesDesc& capabilities = profile_desc.pRequiredCapabilities[capability_index];

                    for (uint32_t variant_index = 0; variant_index < capabilities.variantCount; ++variant_index) {
                        const detail::VpVariantDesc& variant = capabilities.pVariants[variant_index];
                        queue_family_count += variant.queueFamilyCount;
                
                        for (uint32_t i = 0; i < *pPropertyCount; ++i) {
                            VkBaseOutStructure* p = static_cast<VkBaseOutStructure*>(static_cast<void*>(&pProperties[i]));
                            while (p != nullptr) {
                                if (variant.pQueueFamilies[i].pfnFiller == nullptr) continue;
                                variant.pQueueFamilies[i].pfnFiller(p);
                                p = p->pNext;
                            }
                        }
                    }
                }
            }
        }
    }

    return result;
}

VPAPI_ATTR VkResult vpGetProfileQueueFamilyStructureTypes(const VpProfileProperties *pProfile, uint32_t *pStructureTypeCount, VkStructureType *pStructureTypes) {
    // Multiple variants profile, not supported
    VkBool32 multiple_variants = VK_FALSE;
    VkResult result = vpHasMultipleVariantsProfile(pProfile, &multiple_variants);
    if (result != VK_SUCCESS || multiple_variants == VK_TRUE) return VK_ERROR_UNKNOWN;

    std::vector<VkStructureType> results;

    const std::vector<VpProfileProperties>& profiles = detail::GatherProfiles(*pProfile);

    for (std::size_t profile_index = 0, profile_count = profiles.size(); profile_index < profile_count; ++profile_index) {
        const detail::VpProfileDesc* profile_desc = detail::vpGetProfileDesc(profiles[profile_index].profileName);
        if (profile_desc == nullptr) return VK_ERROR_UNKNOWN;

        for (uint32_t capability_index = 0; capability_index < profile_desc->requiredCapabilityCount; ++capability_index) {
            const detail::VpCapabilitiesDesc& capabilities = profile_desc->pRequiredCapabilities[capability_index];

            for (uint32_t variant_index = 0; variant_index < capabilities.variantCount; ++variant_index) {
                const detail::VpVariantDesc& variant = capabilities.pVariants[variant_index];

                for (uint32_t i = 0; i < variant.queueFamilyStructTypeCount; ++i) {
                    results.push_back(variant.pQueueFamilyStructTypes[i]);
                }
            }
        }
    }

    const uint32_t count = static_cast<uint32_t>(results.size());

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

VPAPI_ATTR VkResult vpGetProfileFormats(const VpProfileProperties *pProfile, uint32_t *pFormatCount, VkFormat *pFormats) {
    // Multiple variants profile, not supported
    VkBool32 multiple_variants = VK_FALSE;
    VkResult result = vpHasMultipleVariantsProfile(pProfile, &multiple_variants);
    if (result != VK_SUCCESS || multiple_variants == VK_TRUE) return VK_ERROR_UNKNOWN;

    std::vector<VkFormat> results;

    const std::vector<VpProfileProperties>& profiles = detail::GatherProfiles(*pProfile);

    for (std::size_t profile_index = 0, profile_count = profiles.size(); profile_index < profile_count; ++profile_index) {
        const detail::VpProfileDesc* profile_desc = detail::vpGetProfileDesc(profiles[profile_index].profileName);
        if (profile_desc == nullptr) return VK_ERROR_UNKNOWN;

        for (uint32_t capability_index = 0; capability_index < profile_desc->requiredCapabilityCount; ++capability_index) {
            const detail::VpCapabilitiesDesc& capabilities = profile_desc->pRequiredCapabilities[capability_index];

            for (uint32_t variant_index = 0; variant_index < capabilities.variantCount; ++variant_index) {
                const detail::VpVariantDesc& variant = capabilities.pVariants[variant_index];

                for (uint32_t i = 0; i < variant.formatCount; ++i) {
                    results.push_back(variant.pFormats[i].format);
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

VPAPI_ATTR VkResult vpGetProfileFormatProperties(const VpProfileProperties *pProfile, VkFormat format, void *pNext) {
    // Multiple variants profile, not supported
    VkBool32 multiple_variants = VK_FALSE;
    VkResult result = vpHasMultipleVariantsProfile(pProfile, &multiple_variants);
    if (result != VK_SUCCESS || multiple_variants == VK_TRUE) return VK_ERROR_UNKNOWN;

    const std::vector<VpProfileProperties>& profiles = detail::GatherProfiles(*pProfile);
    
    for (std::size_t i = 0, n = profiles.size(); i < n; ++i) {
        const char* profile_name = profiles[i].profileName;

        const detail::VpProfileDesc* pProfileDesc = detail::vpGetProfileDesc(profile_name);
        if (pProfileDesc == nullptr) return VK_ERROR_UNKNOWN;

        for (uint32_t required_capability_index = 0; required_capability_index < pProfileDesc->requiredCapabilityCount;
                ++required_capability_index) {
            const detail::VpCapabilitiesDesc& required_capabilities = pProfileDesc->pRequiredCapabilities[required_capability_index];

            for (uint32_t required_variant_index = 0; required_variant_index < required_capabilities.variantCount; ++required_variant_index) {
                const detail::VpVariantDesc& variant = required_capabilities.pVariants[required_variant_index];

                for (uint32_t i = 0; i < variant.formatCount; ++i) {
                    if (variant.pFormats[i].format == format) {
                        VkBaseOutStructure* p = static_cast<VkBaseOutStructure*>(static_cast<void*>(pNext));
                        while (p != nullptr) {
                            variant.pFormats[i].pfnFiller(p);
                            p = p->pNext;
                        }
#if defined(VK_VERSION_1_3) || defined(VK_KHR_format_feature_flags2)
                        VkFormatProperties2KHR* fp2 = static_cast<VkFormatProperties2KHR*>(
                            detail::vpGetStructure(pNext, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR));
                        VkFormatProperties3KHR* fp3 = static_cast<VkFormatProperties3KHR*>(
                            detail::vpGetStructure(pNext, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3_KHR));
                        if (fp3 != nullptr) {
                            VkFormatProperties2KHR fp{ VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR };
                            variant.pFormats[i].pfnFiller(static_cast<VkBaseOutStructure*>(static_cast<void*>(&fp)));
                            fp3->linearTilingFeatures = static_cast<VkFormatFeatureFlags2KHR>(fp3->linearTilingFeatures | fp.formatProperties.linearTilingFeatures);
                            fp3->optimalTilingFeatures = static_cast<VkFormatFeatureFlags2KHR>(fp3->optimalTilingFeatures | fp.formatProperties.optimalTilingFeatures);
                            fp3->bufferFeatures = static_cast<VkFormatFeatureFlags2KHR>(fp3->bufferFeatures | fp.formatProperties.bufferFeatures);
                        }
                        if (fp2 != nullptr) {
                            VkFormatProperties3KHR fp{ VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3_KHR };
                            variant.pFormats[i].pfnFiller(static_cast<VkBaseOutStructure*>(static_cast<void*>(&fp)));
                            fp2->formatProperties.linearTilingFeatures = static_cast<VkFormatFeatureFlags>(fp2->formatProperties.linearTilingFeatures | fp.linearTilingFeatures);
                            fp2->formatProperties.optimalTilingFeatures = static_cast<VkFormatFeatureFlags>(fp2->formatProperties.optimalTilingFeatures | fp.optimalTilingFeatures);
                            fp2->formatProperties.bufferFeatures = static_cast<VkFormatFeatureFlags>(fp2->formatProperties.bufferFeatures | fp.bufferFeatures);
                        }
#endif
                    }
                }
            }
        }
    }

    return VK_SUCCESS;
}
