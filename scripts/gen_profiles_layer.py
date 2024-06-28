#!/usr/bin/python3
#
# Copyright (c) 2021-2024 LunarG, Inc.
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
# - Ziga Markus <ziga@lunarg.com>
# - Christophe Riccio <christophe@lunarg.com>

import gen_profiles_solution
import argparse

COPYRIGHT_HEADER = '''
/*
 * Copyright (C) 2015-2024 Valve Corporation
 * Copyright (C) 2015-2024 LunarG, Inc.
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
 * - Ziga Markus <ziga@lunarg.com>
 * - Christophe Riccio <christophe@lunarg.com>
 * - Mark Lobodzinski <mark@lunarg.com>
 * - Mike Weiblen
 * - Arda Coskunses
 * - Jeremy Kniager

 * This file is ***GENERATED***.  Do Not Edit.
 * See scripts/gen_profiles_layer.py for modifications.
 */
'''

DESCRIPTION_HEADER = '''
/*
 * layer/profiles.cpp - The VK_LAYER_KHRONOS_profiles layer.
 * This Profiles layer simulates a device by loading a JSON configuration file to override values that would normally be returned
 * from a Vulkan implementation.  Configuration files must validate with the Profiles schema; this layer does not redundantly
 * check for configuration errors that would be caught by schema validation.
 *
 * References (several documents are also included in the LunarG Vulkan SDK, see [SDK]):
 * [SPEC]   https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/vkspec.html
 * [SDK]    https://vulkan.lunarg.com/sdk/home
 * [LALI]   https://github.com/KhronosGroup/Vulkan-Loader/blob/main/loader/LoaderAndLayerInterface.md
 *
 * Misc notes:
 * This code generally follows the spirit of the Google C++ styleguide, while accommodating conventions of the Vulkan styleguide.
 * https://google.github.io/styleguide/cppguide.html
 * https://www.khronos.org/registry/vulkan/specs/1.1/styleguide.html
 */
'''

INCLUDES_HEADER = '''
#include "profiles.h"
#include "profiles_util.h"
#include "profiles_json.h"
#include "profiles_settings.h"
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;
'''

GLOBAL_CONSTANTS = '''
// Global constants //////////////////////////////////////////////////////////////////////////////////////////////////////////////

// For new features/functionality, increment the minor level and reset patch level to zero.
// For any changes, at least increment the patch level.  See https://semver.org/
// When updating the version, be sure to make corresponding changes to the layer manifest file at
// layer/VkLayer_khronos_profiles.json.in

const uint32_t kVersionProfilesMajor = 1;
const uint32_t kVersionProfilesMinor = 3;
const uint32_t kVersionProfilesPatch = 0;
const uint32_t kVersionProfilesImplementation =
    VK_MAKE_VERSION(kVersionProfilesMajor, kVersionProfilesMinor, kVersionProfilesPatch);

static const char *SCHEMA_URI_BASE = "https://schema.khronos.org/vulkan/profiles-";

// Properties of this layer:
const VkLayerProperties kLayerProperties[] = {{
    kLayerName,                   // layerName
    VK_MAKE_VERSION(1, 0, 68),       // specVersion (clamped to final 1.0 spec version)
    kVersionProfilesImplementation,  // implementationVersion
    "Khronos Profiles layer"         // description
}};
const uint32_t kLayerPropertiesCount = (sizeof(kLayerProperties) / sizeof(kLayerProperties[0]));

// Instance extensions that this layer provides:
const VkExtensionProperties kInstanceExtensionProperties[] = {
    VkExtensionProperties{VK_EXT_LAYER_SETTINGS_EXTENSION_NAME, VK_EXT_LAYER_SETTINGS_SPEC_VERSION}};
const uint32_t kInstanceExtensionPropertiesCount = static_cast<uint32_t>(std::size(kInstanceExtensionProperties));

// Device extensions that this layer provides:
const std::array<VkExtensionProperties, 2> kDeviceExtensionProperties = {
    {{VK_EXT_TOOLING_INFO_EXTENSION_NAME, VK_EXT_TOOLING_INFO_SPEC_VERSION},
     {VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME, VK_KHR_PORTABILITY_SUBSET_SPEC_VERSION}}};
const uint32_t kDeviceExtensionPropertiesCount = static_cast<uint32_t>(kDeviceExtensionProperties.size());
'''

GLOBAL_VARS = '''
// Global variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t requested_version = 0;
bool device_has_astc_hdr = false;
bool device_has_astc = false;
bool device_has_etc2 = false;
bool device_has_bc = false;
bool device_has_pvrtc = false;

std::recursive_mutex global_lock;  // Enforce thread-safety for this layer.
'''

PHYSICAL_DEVICE_DATA_BEGIN = '''
// PhysicalDeviceData : creates and manages the simulated device configurations //////////////////////////////////////////////////

class PhysicalDeviceData {
   public:
    // Create a new PDD element during vkCreateInstance(), and preserve in map, indexed by physical_device.
    static PhysicalDeviceData &Create(VkPhysicalDevice pd, VkInstance instance) {
        assert(pd != VK_NULL_HANDLE);
        assert(instance != VK_NULL_HANDLE);
        assert(!Find(pd));  // Verify this instance does not already exist.

        const auto result = map().emplace(pd, instance);
        assert(result.second);  // true=insertion, false=replacement
        auto iter = result.first;
        PhysicalDeviceData *pdd = &iter->second;
        assert(Find(pd) == pdd);  // Verify we get the same instance we just inserted.
        return *pdd;
    }

    static void Destroy(const VkPhysicalDevice pd) {
        map().erase(pd);
    }

    // Find a PDD from our map, or nullptr if doesn't exist.
    static PhysicalDeviceData *Find(VkPhysicalDevice pd) {
        const auto iter = map().find(pd);
        return (iter != map().end()) ? &iter->second : nullptr;
    }

    static bool HasExtension(PhysicalDeviceData *pdd, const char *extension_name) {
        return pdd->device_extensions_.count(extension_name) > 0;
    }

    static bool HasSimulatedExtension(VkPhysicalDevice pd, const char *extension_name) {
        return HasSimulatedExtension(Find(pd), extension_name);
    }

    static bool HasSimulatedExtension(PhysicalDeviceData *pdd, const char *extension_name) {
        return pdd->simulation_extensions_.count(extension_name) > 0;
    }

    static bool HasSimulatedOrRealExtension(VkPhysicalDevice pd, const char *extension_name) {
        return HasSimulatedOrRealExtension(Find(pd), extension_name);
    }

    static bool HasSimulatedOrRealExtension(PhysicalDeviceData *pdd, const char *extension_name) {
        return HasSimulatedExtension(pdd, extension_name) || HasExtension(pdd, extension_name);
    }

    uint32_t GetEffectiveVersion() {
        return requested_version < physical_device_properties_.apiVersion ? requested_version
                                                                          : physical_device_properties_.apiVersion;

    }

    VkInstance instance() const { return instance_; }

    MapOfVkExtensionProperties device_extensions_;
    MapOfVkFormatProperties device_formats_;
    MapOfVkFormatProperties3 device_formats_3_;
    ArrayOfVkQueueFamilyProperties device_queue_family_properties_;
    MapOfVkExtensionProperties simulation_extensions_;
    VkPhysicalDeviceProperties physical_device_properties_;
    VkPhysicalDeviceFeatures physical_device_features_;
    VkPhysicalDeviceMemoryProperties physical_device_memory_properties_;
    VkPhysicalDeviceToolProperties physical_device_tool_properties_;
    VkSurfaceCapabilitiesKHR surface_capabilities_;
    MapOfVkFormatProperties map_of_format_properties_;
    MapOfVkFormatProperties3 map_of_format_properties_3_;
    MapOfVkExtensionProperties map_of_extension_properties_;
    ArrayOfVkQueueFamilyProperties arrayof_queue_family_properties_;

    bool vulkan_1_1_properties_written_;
    bool vulkan_1_2_properties_written_;
    bool vulkan_1_3_properties_written_;

    bool vulkan_1_1_features_written_;
    bool vulkan_1_2_features_written_;
    bool vulkan_1_3_features_written_;
'''

PHYSICAL_DEVICE_DATA_CONSTRUCTOR_BEGIN = '''
    PhysicalDeviceData(VkInstance instance) : instance_(instance) {
        physical_device_properties_ = {};
        physical_device_features_ = {};
        physical_device_memory_properties_ = {};
        surface_capabilities_ = {};

        vulkan_1_1_properties_written_ = false;
        vulkan_1_2_properties_written_ = false;
        vulkan_1_3_properties_written_ = false;

        vulkan_1_1_features_written_ = false;
        vulkan_1_2_features_written_ = false;
        vulkan_1_3_features_written_ = false;
'''

PHYSICAL_DEVICE_DATA_END = '''    }
    PhysicalDeviceData(const PhysicalDeviceData &) = delete;
    PhysicalDeviceData &operator=(const PhysicalDeviceData &) = delete;
  private:

    const VkInstance instance_;

    typedef std::unordered_map<VkPhysicalDevice, PhysicalDeviceData> Map;
    static Map& map() {
        static Map map_;
        return map_;
    }
};

'''

JSON_LOADER_BEGIN = '''
// Loader for Profiles JSON configuration files ////////////////////////////////////////////////////////////////////////////////////

class JsonLoader {
   public:
    JsonLoader()
        : layer_settings{},
          pdd_(nullptr),
          profile_api_version_(0),
          excluded_extensions_(),
          excluded_formats_()
    {}
    JsonLoader(const JsonLoader &) = delete;
    JsonLoader &operator=(const JsonLoader &rhs) = delete;

    static JsonLoader &Create() {
        VkInstance temporary = VK_NULL_HANDLE;
        const auto result = profile_map().emplace(std::piecewise_construct, std::make_tuple(temporary), std::make_tuple());
        assert(result.second);  // true=insertion, false=replacement
        auto iter = result.first;
        JsonLoader *profile = &iter->second;
        return *profile;
    }

    static void Store(VkInstance instance) {
        auto nh = profile_map().extract(VK_NULL_HANDLE);
        nh.key() = instance;
        profile_map().insert(std::move(nh));
    }

    static JsonLoader *Find(VkInstance instance) {
        const auto iter = profile_map().find(instance);
        return (iter != profile_map().end()) ? &iter->second : nullptr;
    }

    static void Destroy(VkInstance instance) {
        profile_map().erase(instance);
    }

    void LogFoundProfiles();
    const Json::Value& FindRootFromProfileName(const std::string& profile_name) const;
    VkResult LoadProfilesDatabase();
    VkResult LoadFile(const std::string& filename);
    void ReadProfileApiVersion();
    VkResult LoadDevice(const char* device_name, PhysicalDeviceData *pdd);
    VkResult ReadProfile(const char* device_name, const Json::Value& root, const std::vector<std::vector<std::string>> &capabilities, bool requested_profile, bool enable_warnings);
    uint32_t GetProfileApiVersion() const { return profile_api_version_; }
    void CollectProfiles(const std::string& profile_name, std::vector<std::string>& results) const;

    ProfileLayerSettings layer_settings;

   private:
    PhysicalDeviceData *pdd_;

    std::map<std::string, Json::Value> profiles_file_roots_;

    std::uint32_t profile_api_version_;
    std::vector<std::string> excluded_extensions_;
    std::vector<std::string> excluded_formats_;

    struct Extension {
        std::string name;
        int specVersion;
    };

    enum ExtensionSupport {
        UNSUPPORTED,
        EXCLUDED,
        SUPPORTED,
    };

    bool WarnDuplicatedFeature(const Json::Value &parent);
    bool WarnDuplicatedProperty(const Json::Value &parent);
    bool GetFeature(const char *device_name, bool requested_profile, const Json::Value &features, const std::string &name);
    bool GetProperty(const char *device_name, bool requested_profile, const Json::Value &props, const std::string &name);
    bool GetFormat(const char *device_name, bool requested_profile, const Json::Value &formats, const std::string &format_name, MapOfVkFormatProperties *dest,
                   MapOfVkFormatProperties3 *dest3);
    bool CheckVersionSupport(uint32_t version, const std::string &name);
    ExtensionSupport CheckExtensionSupport(const char *extension, const std::string &name);
    bool valid(ExtensionSupport support);
    bool GetQueueFamilyProperties(const char* device_name, const Json::Value &qf_props, QueueFamilyProperties *dest);
    bool OrderQueueFamilyProperties(ArrayOfVkQueueFamilyProperties *qfp);
    void AddPromotedExtensions(uint32_t api_level);
'''

JSON_LOADER_END = '''
    typedef std::unordered_map<VkInstance, JsonLoader> ProfileMap;
    static ProfileMap& profile_map() {
        static ProfileMap profile_map_;
        return profile_map_;
    }
};
'''

WARN_FUNCTIONS = '''
    static bool WarnIfNotEqualFloat(ProfileLayerSettings *layer_settings, bool enable_warnings, const char* device_name, const char *cap_name, const float new_value, const float old_value, const bool not_modifiable) {
        if (std::abs(new_value - old_value) > 0.0001f) {
            if (enable_warnings) {
                if (not_modifiable) {
                    LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                        "'%s' is not modifiable but the profile value (%3.2f) is different from the device (%s) value (%3.2f)\\n", cap_name, new_value, device_name, old_value);
                } else {
                    LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                        "'%s' profile value (%3.2f) is different from the device (%s) supported value (%3.2f)\\n", cap_name, new_value, device_name, old_value);
                }
            }
            return true;
        }
        return false;
    }

    static bool WarnIfNotEqualBool(ProfileLayerSettings *layer_settings, bool enable_warnings, const char* device_name, const char *cap_name, const bool new_value, const bool old_value, const bool not_modifiable) {
        if (new_value != old_value) {
            if (enable_warnings) {
                if (not_modifiable) {
                    LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                        "'%s' is not modifiable but the profile value (%s) is different from the device (%s) value (%s)\\n", cap_name, new_value ? "true" : "false", device_name, old_value ? "true" : "false");
                } else if (new_value) {
                    LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                        "'%s' profile value is enabled in the profile, but the device (%s) does not support it\\n", cap_name, device_name);
                }
            }
            return true;
        }
        return false;
    }

    static bool WarnIfNotEqualEnum(ProfileLayerSettings *layer_settings, bool enable_warnings, const char* device_name, const char *cap_name, const uint32_t new_value, const uint32_t old_value, const bool not_modifiable) {
        if (new_value != old_value) {
            if (enable_warnings) {
                if (not_modifiable) {
                    LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                        "'%s' is not modifiable but the profile value (%" PRIu32 ") is different from the device (%s) value (%" PRIu32 ")\\n", cap_name, new_value, device_name, old_value);
                } else {
                    LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                        "'%s' profile value (%" PRIu32 ") is different from the device (%s) value (%" PRIu32 ")\\n", cap_name, new_value, device_name, old_value);
                }
            }
            return true;
        }
        return false;
    }

    static bool WarnIfNotEqual(ProfileLayerSettings *layer_settings, bool enable_warnings, const char* device_name, const char *cap_name, const uint32_t new_value, const uint32_t old_value, const bool not_modifiable) {
        if (new_value != old_value) {
            if (enable_warnings) {
                if (not_modifiable) {
                    LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                        "'%s' is not modifiable but the profile value (%" PRIu32 ") is different from the device (%s) value (%" PRIu32 ")\\n", cap_name, new_value, device_name, old_value);
                } else {
                    LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                        "'%s' profile value (%" PRIu32 ") is different from the device (%s) value (%" PRIu32 ")\\n", cap_name, new_value, device_name, old_value);
                }
            }
            return true;
        }
        return false;
    }

    static bool WarnIfNotEqual32u(ProfileLayerSettings *layer_settings, bool enable_warnings, const char* device_name, const char *cap_name, const uint32_t new_value, const uint32_t old_value, const bool not_modifiable) {
        if (new_value != old_value) {
            if (enable_warnings) {
                if (not_modifiable) {
                    LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                        "'%s' is not modifiable but the profile value (%" PRIu32 ") is different from the device (%s) value (%" PRIu32 ")\\n", cap_name, new_value, device_name, old_value);
                } else {
                    LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                        "'%s' profile value (%" PRIu32 ") is different from the device (%s) value (%" PRIu32 ")\\n", cap_name, new_value, device_name, old_value);
                }
            }
            return true;
        }
        return false;
    }

    static bool WarnIfNotEqual(ProfileLayerSettings *layer_settings, bool enable_warnings, const char* device_name, const char *cap_name, const int32_t new_value, const int32_t old_value, const bool not_modifiable) {
        if (new_value != old_value) {
            if (enable_warnings) {
                if (not_modifiable) {
                    LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                        "'%s' is not modifiable but the profile value (%" PRIi32 ") is different from the device (%s) value (%" PRIi32 ")\\n", cap_name, new_value, device_name, old_value);
                } else {
                    LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                        "'%s' profile value (%" PRIi32 ") is different from the device (%s) value (%" PRIi32 ")\\n", cap_name, new_value, device_name, old_value);
                }
            }
            return true;
        }
        return false;
    }

    static bool WarnIfNotEqual64u(ProfileLayerSettings *layer_settings, bool enable_warnings, const char* device_name, const char *cap_name, const uint64_t new_value, const uint64_t old_value, const bool not_modifiable) {
        if (new_value != old_value) {
            if (enable_warnings) {
                if (not_modifiable) {
                    LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                        "'%s' is not modifiable but the profile value (%" PRIu64 ") is different from the device (%s) value (%" PRIu64 ")\\n", cap_name, new_value, device_name, old_value);
                } else {
                    LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                        "'%s' profile value (%" PRIu64 ") is different from the device (%s) value (%" PRIu64 ")\\n", cap_name, new_value, device_name, old_value);
                }
            }
            return true;
        }
        return false;
    }

    static bool WarnIfNotEquali64(ProfileLayerSettings *layer_settings, bool enable_warnings, const char* device_name, const char *cap_name, const int64_t new_value, const int64_t old_value, const bool not_modifiable) {
        if (new_value != old_value) {
            if (enable_warnings) {
                if (not_modifiable) {
                    LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                        "'%s' is not modifiable but the profile value (%" PRIi64 ") is different from the device (%s) value (%" PRIi64 ")\\n", cap_name, new_value, device_name, old_value);
                } else {
                    LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                        "'%s' profile value (%" PRIi64 ") is different from the device (%s) value (%" PRIi64 ")\\n", cap_name, new_value, device_name, old_value);
                }
            }
            return true;
        }
        return false;
    }

    static bool WarnIfNotEqualSizet(ProfileLayerSettings *layer_settings, bool enable_warnings, const char* device_name, const char *cap_name, const size_t new_value, const size_t old_value, const bool not_modifiable) {
        if (new_value != old_value) {
            if (enable_warnings) {
                if (not_modifiable) {
                    LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                        "'%s' is not modifiable but the profile value (%" PRIuLEAST64 ") is different from the device (%s) value (%" PRIuLEAST64 ")\\n", cap_name, new_value, device_name, old_value);
                } else {
                    LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                        "'%s' profile value (%" PRIuLEAST64 ") is different from the device (%s) value (%" PRIuLEAST64 ")\\n", cap_name, new_value, device_name, old_value);
                }
            }
            return true;
        }
        return false;
    }

    static bool WarnIfMissingBit(ProfileLayerSettings *layer_settings, bool enable_warnings, const char* device_name, const char *cap_name, const uint32_t new_value, const uint32_t old_value, const bool not_modifiable) {
        if ((old_value | new_value) != old_value) {
            if (enable_warnings) {
                if (not_modifiable) {
                    LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                        "'%s' is not modifiable but the profile value (%" PRIu32 ") is different from the device (%s) value (%" PRIu32 ")\\n", cap_name, new_value, device_name, old_value);
                } else {
                    LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                        "'%s' profile value (%" PRIu32 ") has bits set that the device (%s) value (%" PRIu32 ") does not\\n", cap_name, new_value, device_name, old_value);
                }
            }
            return true;
        }
        return false;
    }

    static bool WarnIfGreater(ProfileLayerSettings *layer_settings, bool enable_warnings, const char* device_name, const char *cap_name, const uint64_t new_value, const uint64_t old_value, const bool not_modifiable) {
        (void)not_modifiable;

        if (new_value > old_value) {
            if (enable_warnings) {
                LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                    "'%s' profile value (%" PRIu64 ") is greater than device (%s) value (%" PRIu64 ")\\n", cap_name, new_value, device_name, old_value);
            }
            return true;
        }
        return false;
    }

    static bool WarnIfGreaterSizet(ProfileLayerSettings *layer_settings, bool enable_warnings, const char* device_name, const char *cap_name, const size_t new_value, const size_t old_value, const bool not_modifiable) {
        (void)not_modifiable;

        if (new_value > old_value) {
            if (enable_warnings) {
                LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                    "'%s' profile value (%" PRIuLEAST64 ") is greater than device (%s) value (%" PRIuLEAST64 ")\\n", cap_name, new_value, device_name, old_value);
            }
            return true;
        }
        return false;
    }

    static bool WarnIfGreaterFloat(ProfileLayerSettings *layer_settings, bool enable_warnings, const char* device_name, const char *cap_name, const float new_value, const float old_value, const bool not_modifiable) {
        (void)not_modifiable;

        if (new_value > old_value) {
            if (enable_warnings) {
                LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                    "'%s' profile value (%3.2f) is greater than device (%s) value (%3.2f)\\n", cap_name, new_value, device_name, old_value);
            }
            return true;
        }
        return false;
    }

    static bool WarnIfLesser(ProfileLayerSettings *layer_settings, bool enable_warnings, const char* device_name, const char *cap_name, const uint64_t new_value, const uint64_t old_value, const bool not_modifiable) {
        (void)not_modifiable;

        if (new_value < old_value) {
            if (enable_warnings) {
                LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                "'%s' profile value (%" PRIu64 ") is lesser than device (%s) value (%" PRIu64 ")\\n", cap_name, new_value, device_name, old_value);
            }
            return true;
        }
        return false;
    }

    static bool WarnIfLesserSizet(ProfileLayerSettings *layer_settings, bool enable_warnings, const char* device_name, const char *cap_name, const size_t new_value, const size_t old_value, const bool not_modifiable) {
        (void)not_modifiable;

        if (new_value < old_value) {
            if (enable_warnings) {
                LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                    "'%s' profile value (%" PRIuLEAST64 ") is lesser than device (%s) value (%" PRIuLEAST64 ")\\n", cap_name, new_value, device_name, old_value);
            }
            return true;
        }
        return false;
    }

    static bool WarnIfLesserFloat(ProfileLayerSettings *layer_settings, bool enable_warnings, const char* device_name, const char *cap_name, const float new_value, const float old_value, const bool not_modifiable) {
        (void)not_modifiable;

        if (new_value < old_value) {
            if (enable_warnings) {
                LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                    "'%s' profile value (%3.2f) is lesser than device (%s) value (%3.2f)\\n", cap_name, new_value, device_name, old_value);
            }
            return true;
        }
        return false;
    }
'''

GET_VALUE_FUNCTIONS = '''
    bool GetValue(const char* device_name, const Json::Value &parent, const std::string &member, const char *name, float *dest, bool not_modifiable, bool requested_profile,
                  std::function<bool(ProfileLayerSettings *, bool, const char *, const char *, float, float, bool)> warn_func = nullptr) {
        if (member != name) {
            return true;
        }
        // If the value is not modifiable and we don't warn, we can return immediately, we will use the native value
        if (not_modifiable && warn_func == nullptr) {
            return true;
        }

        const Json::Value value = parent[name];
        if (!value.isDouble()) {
            return true;
        }
        bool valid = true;
        const float new_value = value.asFloat();
        if (warn_func) {
            if (warn_func(&layer_settings, requested_profile, device_name, name, new_value, *dest, not_modifiable)) {
                valid = false;
            }
        }
        if (!not_modifiable) {
            *dest = new_value;
        }

        return valid;
    }

    bool GetValue(const char* device_name, const Json::Value &parent, const std::string &member, const char *name, uint8_t *dest, bool not_modifiable, bool requested_profile,
                  std::function<bool(ProfileLayerSettings *, bool, const char *, const char *, uint8_t, uint8_t, bool)> warn_func = nullptr) {
        if (member != name) {
            return true;
        }
        // If the value is not modifiable and we don't warn, we can return immediately, we will use the native value
        if (not_modifiable && warn_func == nullptr) {
            return true;
        }

        const Json::Value value = parent[name];
        bool valid = true;
        if (value.isBool()) {
            const bool new_value = value.asBool();
            if (warn_func) {
                if (warn_func(&layer_settings, requested_profile, device_name, name, new_value, *dest, not_modifiable)) {
                    valid = false;
                }
            }
            *dest = static_cast<uint8_t>(new_value);
        } else if (value.isArray()) {
            uint32_t sum_bits = 0;
            for (const auto &entry : value) {
                if (entry.isString()) {
                    sum_bits |= VkStringToUint(entry.asString());
                }
            }
            if (!not_modifiable) {
                *dest = static_cast<uint8_t>(sum_bits);
            }
        } else if (value.isUInt()) {
            const uint8_t new_value = static_cast<uint8_t>(value.asUInt());
            if (warn_func) {
                if (warn_func(&layer_settings, requested_profile, device_name, name, new_value, *dest, not_modifiable)) {
                    valid = false;
                }
            }
            if (!not_modifiable) {
                *dest = static_cast<uint8_t>(new_value);
            }
        }
        return valid;
    }

    bool GetValue(const char* device_name, const Json::Value &parent, const std::string &member, const char *name, int32_t *dest, bool not_modifiable, bool requested_profile,
                  std::function<bool(ProfileLayerSettings *, bool, const char *, const char *, int32_t, int32_t, bool)> warn_func = nullptr) {
        if (member != name) {
            return true;
        }
        // If the value is not modifiable and we don't warn, we can return immediately, we will use the native value
        if (not_modifiable && warn_func == nullptr) {
            return true;
        }

        const Json::Value value = parent[name];
        if (!value.isInt()) {
            return true;
        }
        bool valid = true;
        const int32_t new_value = value.asInt();
        if (warn_func) {
            if (warn_func(&layer_settings, requested_profile, device_name, name, new_value, *dest, not_modifiable)) {
                valid = false;
            }
        }

        if (!not_modifiable) {
            *dest = new_value;
        }
        return valid;
    }

    bool GetValue(const char* device_name, const Json::Value &parent, const std::string &member, const char *name, int64_t *dest, bool not_modifiable, bool requested_profile,
                  std::function<bool(ProfileLayerSettings *, bool, const char *, const char *, int64_t, int64_t, bool)> warn_func = nullptr) {
        if (member != name) {
            return true;
        }
        // If the value is not modifiable and we don't warn, we can return immediately, we will use the native value
        if (not_modifiable && warn_func == nullptr) {
            return true;
        }

        const Json::Value value = parent[name];
        if (!value.isInt64()) {
            return true;
        }
        bool valid = true;
        const int64_t new_value = value.asInt64();
        if (warn_func) {
            if (warn_func(&layer_settings, requested_profile, device_name, name, new_value, *dest, not_modifiable)) {
                valid = false;
            }
        }

        if (!not_modifiable) {
            *dest = new_value;
        }
        return valid;
    }

    bool GetValue(const char* device_name, const Json::Value &parent, const std::string &member, const char *name, uint32_t *dest, bool not_modifiable, bool requested_profile,
                  std::function<bool(ProfileLayerSettings *, bool, const char *, const char *, uint32_t, uint32_t, bool)> warn_func = nullptr) {
        if (member != name) {
            return true;
        }
        // If the value is not modifiable and we don't warn, we can return immediately, we will use the native value
        if (not_modifiable && warn_func == nullptr) {
            return true;
        }

        const Json::Value value = parent[name];
        bool valid = true;
        if (value.isBool()) {
            const bool new_value = value.asBool();
            if (warn_func) {
                if (warn_func(&layer_settings, requested_profile, device_name, name, new_value, *dest, not_modifiable)) {
                    valid = false;
                }
            }

            if (!not_modifiable) {
                *dest = static_cast<uint32_t>(new_value);
            }
        } else if (value.isArray()) {
            uint32_t sum_bits = 0;
            for (const auto &entry : value) {
                if (entry.isString()) {
                    sum_bits |= VkStringToUint(entry.asString());
                }
            }

            if (!not_modifiable) {
                *dest = sum_bits;
            }
        } else if (value.isUInt()) {
            const uint32_t new_value = value.asUInt();
            if (warn_func) {
                if (warn_func(&layer_settings, requested_profile, device_name, name, new_value, *dest, not_modifiable)) {
                    valid = false;
                }
            }

            if (!not_modifiable) {
                *dest = new_value;
            }
        }
        return valid;
    }

    bool GetValue(const char* device_name, const Json::Value &parent, const std::string &member, const char *name, uint64_t *dest, bool not_modifiable, bool requested_profile,
                  std::function<bool(ProfileLayerSettings *, bool, const char *, const char *, uint64_t, uint64_t, bool)> warn_func = nullptr) {
        if (member != name) {
            return true;
        }
        // If the value is not modifiable and we don't warn, we can return immediately, we will use the native value
        if (not_modifiable && warn_func == nullptr) {
            return true;
        }

        const Json::Value value = parent[name];
        if (!value.isUInt64()) {
            return true;
        }
        bool valid = true;
        const uint64_t new_value = value.asUInt64();
        if (warn_func) {
            if (warn_func(&layer_settings, requested_profile, device_name, name, new_value, *dest, not_modifiable)) {
                valid = false;
            }
        }

        if (!not_modifiable) {
            *dest = new_value;
        }
        return valid;
    }

    bool GetValue(const char* device_name, const Json::Value &pparent, const std::string &member, const char *name, VkExtent2D *dest, bool not_modifiable, bool requested_profile,
                  std::function<bool(ProfileLayerSettings *, bool, const char *, const char *, uint32_t, uint32_t, bool)> warn_func = nullptr) {
        if (member != name) {
            return true;
        }
        // If the value is not modifiable and we don't warn, we can return immediately, we will use the native value
        if (not_modifiable && warn_func == nullptr) {
            return true;
        }

        const Json::Value parent = pparent[name];
        if (parent.type() != Json::objectValue) {
            return true;
        }
        bool valid = true;
        for (const auto &prop : parent.getMemberNames()) {
            GET_VALUE_WARN(prop, width, not_modifiable, requested_profile, warn_func);
            GET_VALUE_WARN(prop, height, not_modifiable, requested_profile, warn_func);
        }
        return valid;
    }

    bool GetValue(const char* device_name, const Json::Value &pparent, const std::string &member, const char *name, VkExtent3D *dest, bool not_modifiable, bool requested_profile,
                  std::function<bool(ProfileLayerSettings *, bool, const char *, const char *, uint32_t, uint32_t, bool)> warn_func = nullptr) {
        if (member != name) {
            return true;
        }
        // If the value is not modifiable and we don't warn, we can return immediately, we will use the native value
        if (not_modifiable && warn_func == nullptr) {
            return true;
        }

        const Json::Value parent = pparent[name];
        if (parent.type() != Json::objectValue) {
            return true;
        }
        bool valid = true;
        for (const auto &prop : parent.getMemberNames()) {
            GET_VALUE_WARN(prop, width, not_modifiable, requested_profile, warn_func);
            GET_VALUE_WARN(prop, height, not_modifiable, requested_profile, warn_func);
            GET_VALUE_WARN(prop, depth, not_modifiable, requested_profile, warn_func);
        }
        return valid;
    }

    bool GetValueSizet(const char* device_name, const Json::Value &parent, const std::string &member, const char *name, size_t *dest, bool not_modifiable, bool requested_profile,
                       std::function<bool(ProfileLayerSettings *, bool, const char *, const char *, size_t, size_t, bool)> warn_func = nullptr) {
        if (member != name) {
            return true;
        }
        // If the value is not modifiable and we don't warn, we can return immediately, we will use the native value
        if (not_modifiable && warn_func == nullptr) {
            return true;
        }

        const Json::Value value = parent[name];
        bool valid = true;
        if (value.isUInt()) {
            const size_t new_value = value.asUInt();
            if (warn_func) {
                if (warn_func(&layer_settings, requested_profile, device_name, name, new_value, *dest, not_modifiable)) {
                    valid = false;
                }
            }

            if (!not_modifiable) {
                *dest = new_value;
            }
        }
        return valid;
    }

    template <typename T>  // for Vulkan enum types
    bool GetValueFlag(const char* device_name, const Json::Value &parent, const std::string &member, const char *name, T *dest, bool not_modifiable, bool requested_profile,
                      std::function<bool(ProfileLayerSettings *, bool, const char *, const char *, T, T, bool)> warn_func = nullptr) {
        if (member != name) {
            return true;
        }
        // If the value is not modifiable and we don't warn, we can return immediately, we will use the native value
        if (not_modifiable && warn_func == nullptr) {
            return true;
        }

        const Json::Value value = parent[name];
        bool valid = true;
        uint32_t new_value = 0;
        if (value.isArray()) {
            for (const auto &entry : value) {
                if (entry.isString()) {
                    new_value |= VkStringToUint(entry.asString());
                }
            }
        }
        if (WarnIfMissingBit(&layer_settings, requested_profile, device_name, name, new_value, static_cast<uint32_t>(*dest), not_modifiable)) {
            valid = false;
        }

        if (!not_modifiable) {
            *dest = static_cast<T>(new_value);
        }
        return valid;
    }

    template <typename T>  // for Vulkan enum types
    bool GetValueEnum(const char* device_name, const Json::Value &parent, const std::string &member, const char *name, T *dest, bool not_modifiable, bool requested_profile,
                      std::function<bool(ProfileLayerSettings *, bool, const char *, const char *, uint32_t, uint32_t, bool)> warn_func = nullptr) {
        if (member != name) {
            return true;
        }
        // If the value is not modifiable and we don't warn, we can return immediately, we will use the native value
        if (not_modifiable && warn_func == nullptr) {
            return true;
        }

        const Json::Value value = parent[name];
        bool valid = true;
        uint32_t new_value = 0;
        if (value.isString()) {
            new_value = static_cast<T>(VkStringToUint(value.asString()));
        }
        if (warn_func) {
            if (warn_func(&layer_settings, requested_profile, device_name, name, new_value, *dest, not_modifiable)) {
                valid = false;
            }
        } else {
            if (WarnIfNotEqualEnum(&layer_settings, requested_profile, device_name, name, new_value, *dest, not_modifiable)) {
                valid = false;
            }
        }

        if (!not_modifiable) {
            *dest = static_cast<T>(new_value);
        }
        return valid;
    }

    int GetArray(const char* device_name, const Json::Value &parent, const std::string &member, const char *name, uint8_t *dest, bool not_modifiable) {
        (void)not_modifiable;
        (void)device_name;

        if (member != name) {
            return -1;
        }

        const Json::Value value = parent[name];
        if (value.type() != Json::arrayValue) {
            return -1;
        }
        const int count = static_cast<int>(value.size());
        for (int i = 0; i < count; ++i) {
            dest[i] = static_cast<uint8_t>(value[i].asUInt());
        }
        return count;
    }

    int GetArray(const char* device_name, const Json::Value &parent, const std::string &member, const char *name, uint32_t *dest, bool not_modifiable) {
        (void)not_modifiable;
        (void)device_name;

        if (member != name) {
            return -1;
        }

        const Json::Value value = parent[name];
        if (value.type() != Json::arrayValue) {
            return -1;
        }
        const int count = static_cast<int>(value.size());
        for (int i = 0; i < count; ++i) {
            dest[i] = static_cast<uint32_t>(value[i].asUInt());
        }
        return count;
    }

    int GetArray(const char* device_name, const Json::Value &parent, const std::string &member, const char *name, float *dest, bool not_modifiable) {
        (void)not_modifiable;
        (void)device_name;

        if (member != name) {
            return -1;
        }

        const Json::Value value = parent[name];
        if (value.type() != Json::arrayValue) {
            return -1;
        }
        const int count = static_cast<int>(value.size());
        for (int i = 0; i < count; ++i) {
            dest[i] = value[i].asFloat();
        }
        return count;
    }

    int GetArray(const char* device_name, const Json::Value &parent, const std::string &member, const char *name, char *dest, bool not_modifiable) {
        (void)not_modifiable;
        (void)device_name;

        if (member != name) {
            return -1;
        }

        const Json::Value value = parent[name];
        if (!value.isString()) {
            return -1;
        }
        const char *new_value = value.asCString();
        int count = 0;
        dest[0] = '\\0';
        if (new_value) {
            count = static_cast<int>(strlen(new_value));
            strcpy(dest, new_value);
        }
        return count;
    }

    int GetArray(const char* device_name, const Json::Value &parent, const std::string &member, const char *name, VkImageLayout *dest, bool not_modifiable) {
        (void)not_modifiable;
        (void)device_name;

        if (member != name) {
            return -1;
        }

        const Json::Value value = parent[name];
        if (value.type() != Json::arrayValue) {
            return -1;
        }
        const int count = static_cast<int>(value.size());
        for (int i = 0; i < count; ++i) {
            dest[i] = StringToImageLayout(value[i].asCString());
        }
        return count;
    }
'''

JSON_LOADER_NON_GENERATED = '''
bool JsonLoader::GetFormat(const char *device_name, bool requested_profile, const Json::Value &formats, const std::string &format_name, MapOfVkFormatProperties *dest,
                           MapOfVkFormatProperties3 *dest3) {
    (void)requested_profile;

    VkFormat format = StringToFormat(format_name);
    VkFormatProperties profile_properties = {};
    VkFormatProperties3 profile_properties_3 = {VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3};
    const auto &member = formats[format_name];
    for (const auto &name : member.getMemberNames()) {
        const auto &props = member[name];
        if (name == "VkFormatProperties") {
            for (const auto &feature : props["linearTilingFeatures"]) {
                profile_properties.linearTilingFeatures |= StringToVkFormatFeatureFlags(feature.asString());
            }
            for (const auto &feature : props["optimalTilingFeatures"]) {
                profile_properties.optimalTilingFeatures |= StringToVkFormatFeatureFlags(feature.asString());
            }
            for (const auto &feature : props["bufferFeatures"]) {
                profile_properties.bufferFeatures |= StringToVkFormatFeatureFlags(feature.asString());
            }
        } else if (name == "VkFormatProperties2" || name == "VkFormatProperties2KHR") {
            const auto &format_properties = props["formatProperties"];
            for (const auto &feature : format_properties["linearTilingFeatures"]) {
                profile_properties.linearTilingFeatures |= StringToVkFormatFeatureFlags(feature.asString());
            }
            for (const auto &feature : format_properties["optimalTilingFeatures"]) {
                profile_properties.optimalTilingFeatures |= StringToVkFormatFeatureFlags(feature.asString());
            }
            for (const auto &feature : format_properties["bufferFeatures"]) {
                profile_properties.bufferFeatures |= StringToVkFormatFeatureFlags(feature.asString());
            }
        } else if (name == "VkFormatProperties3" || name == "VkFormatProperties3KHR") {
            for (const auto &feature : props["linearTilingFeatures"]) {
                profile_properties_3.linearTilingFeatures |= StringToVkFormatFeatureFlags2(feature.asString());
            }
            for (const auto &feature : props["optimalTilingFeatures"]) {
                profile_properties_3.optimalTilingFeatures |= StringToVkFormatFeatureFlags2(feature.asString());
            }
            for (const auto &feature : props["bufferFeatures"]) {
                profile_properties_3.bufferFeatures |= StringToVkFormatFeatureFlags2(feature.asString());
            }
        }
    }

    profile_properties_3.linearTilingFeatures |= profile_properties.linearTilingFeatures;
    profile_properties_3.optimalTilingFeatures |= profile_properties.optimalTilingFeatures;
    profile_properties_3.bufferFeatures |= profile_properties.bufferFeatures;

    profile_properties.linearTilingFeatures |= static_cast<VkFormatFeatureFlags>(profile_properties_3.linearTilingFeatures);
    profile_properties.optimalTilingFeatures |= static_cast<VkFormatFeatureFlags>(profile_properties_3.optimalTilingFeatures);
    profile_properties.bufferFeatures |= static_cast<VkFormatFeatureFlags>(profile_properties_3.bufferFeatures);

    (*dest)[format] = profile_properties;
    (*dest3)[format] = profile_properties_3;

    if (IsASTCHDRFormat(format) && !device_has_astc_hdr) {
        // We already notified that ASTC HDR is not supported, no spamming
        return false;
    }
    if (IsASTCLDRFormat(format) && !device_has_astc) {
        // We already notified that ASTC is not supported, no spamming
        return false;
    }
    if ((IsETC2Format(format) || IsEACFormat(format)) && !device_has_etc2) {
        // We already notified that ETC2 is not supported, no spamming
        return false;
    }
    if (IsBCFormat(format) && !device_has_bc) {
        // We already notified that BC is not supported, no spamming
        return false;
    }
    if (IsPVRTCFormat(format) &&!device_has_pvrtc) {
        // We already notified that PVRTC is not supported, no spamming
        return false;
    }

    bool valid = true;

    const VkFormatProperties &device_properties = pdd_->device_formats_[format];
    if (!HasFlags(device_properties.linearTilingFeatures, profile_properties.linearTilingFeatures)) {
        WarnMissingFormatFeatures(&layer_settings, device_name, format_name, "linearTilingFeatures", profile_properties.linearTilingFeatures,
                                  device_properties.linearTilingFeatures);
        valid = false;
    }
    if (!HasFlags(device_properties.optimalTilingFeatures, profile_properties.optimalTilingFeatures)) {
        WarnMissingFormatFeatures(&layer_settings, device_name, format_name, "optimalTilingFeatures", profile_properties.optimalTilingFeatures,
                                  device_properties.optimalTilingFeatures);
        valid = false;
    }
    if (!HasFlags(device_properties.bufferFeatures, profile_properties.bufferFeatures)) {
        WarnMissingFormatFeatures(&layer_settings, device_name, format_name, "bufferFeatures", profile_properties.bufferFeatures,
                                  device_properties.bufferFeatures);
        valid = false;
    }

    const VkFormatProperties3 &device_properties_3 = pdd_->device_formats_3_[format];
    if (!HasFlags(device_properties_3.linearTilingFeatures, profile_properties_3.linearTilingFeatures)) {
        WarnMissingFormatFeatures2(&layer_settings, device_name, format_name, "linearTilingFeatures", profile_properties_3.linearTilingFeatures,
                                   device_properties_3.linearTilingFeatures);
        valid = false;
    }
    if (!HasFlags(device_properties_3.optimalTilingFeatures, profile_properties_3.optimalTilingFeatures)) {
        WarnMissingFormatFeatures2(&layer_settings, device_name, format_name, "optimalTilingFeatures", profile_properties_3.optimalTilingFeatures,
                                   device_properties_3.optimalTilingFeatures);
        valid = false;
    }
    if (!HasFlags(device_properties_3.bufferFeatures, profile_properties_3.bufferFeatures)) {
        WarnMissingFormatFeatures2(&layer_settings, device_name, format_name, "bufferFeatures", profile_properties_3.bufferFeatures,
                                   device_properties_3.bufferFeatures);
        valid = false;
    }

    return valid;
}

bool JsonLoader::CheckVersionSupport(uint32_t version, const std::string &name) {
    if (pdd_->GetEffectiveVersion() < version) {
        LogMessage(&layer_settings,
            DEBUG_REPORT_ERROR_BIT,
            "Profile sets %s which is provided by Vulkan version %s, but the current effective API version is %s.\\n",
                     name.c_str(), StringAPIVersion(version).c_str(), StringAPIVersion(pdd_->GetEffectiveVersion()).c_str());
        return false;
    }
    return true;
}

JsonLoader::ExtensionSupport JsonLoader::CheckExtensionSupport(const char *extension, const std::string &name) {
    for (const auto &ext : excluded_extensions_) {
        if (ext == extension) {
            LogMessage(&layer_settings, DEBUG_REPORT_NOTIFICATION_BIT,
                       "Profile requires %s capabilities, but %s is excluded, device values are used.\\n", name.c_str(),
                                extension);
            return JsonLoader::ExtensionSupport::EXCLUDED;
        }
    }
    if (layer_settings.simulate.capabilities & SIMULATE_EXTENSIONS_BIT) {
        if (!PhysicalDeviceData::HasSimulatedExtension(pdd_, extension)) {
            LogMessage(&layer_settings, DEBUG_REPORT_ERROR_BIT,
                "Profile requires %s capabilitiess, but %s is not required by the profile, device values are used.\\n",
                         name.c_str(), extension);
            if (layer_settings.log.debug_fail_on_error) {
                return JsonLoader::ExtensionSupport::UNSUPPORTED;
            }
        }
    } else {
        if (!PhysicalDeviceData::HasExtension(pdd_, extension)) {
            LogMessage(&layer_settings,
                DEBUG_REPORT_WARNING_BIT,
                "Profile requires by %s capabilities, but %s is not supported by the device.\\n", name.c_str(), extension);
        }
    }
    return JsonLoader::ExtensionSupport::SUPPORTED;
}

bool JsonLoader::valid(ExtensionSupport support) {
    if (support == JsonLoader::ExtensionSupport::UNSUPPORTED) {
        return false;
    }
    return true;
}

bool JsonLoader::GetQueueFamilyProperties(const char* device_name, const Json::Value &qf_props, QueueFamilyProperties *dest) {
    for (const auto &name : qf_props.getMemberNames()) {
        const auto &props = qf_props[name];
        if (name == "VkQueueFamilyProperties") {
            for (const auto &feature : props["queueFlags"]) {
                dest->properties_2.queueFamilyProperties.queueFlags |= StringToVkQueueFlags(feature.asString());
            }
            dest->properties_2.queueFamilyProperties.queueCount = static_cast<uint32_t>(props["queueCount"].asInt());
            dest->properties_2.queueFamilyProperties.timestampValidBits = props["timestampValidBits"].asUInt();
            const auto &minImagetransferGranularity = props["minImageTransferGranularity"];
            dest->properties_2.queueFamilyProperties.minImageTransferGranularity.width =
                minImagetransferGranularity["width"].asUInt();
            dest->properties_2.queueFamilyProperties.minImageTransferGranularity.height =
                minImagetransferGranularity["height"].asUInt();
            dest->properties_2.queueFamilyProperties.minImageTransferGranularity.depth =
                minImagetransferGranularity["depth"].asUInt();
        } else if (name == "VkQueueFamilyProperties2" || name == "VkQueueFamilyProperties2KHR") {
            const auto &props2 = props["queueFamilyProperties"];
            for (const auto &feature : props2["queueFlags"]) {
                dest->properties_2.queueFamilyProperties.queueFlags |= StringToVkQueueFlags(feature.asString());
            }
            dest->properties_2.queueFamilyProperties.queueCount = static_cast<uint32_t>(props2["queueCount"].asInt());
            dest->properties_2.queueFamilyProperties.timestampValidBits = props2["timestampValidBits"].asUInt();
            const auto &minImagetransferGranularity = props2["minImageTransferGranularity"];
            dest->properties_2.queueFamilyProperties.minImageTransferGranularity.width =
                minImagetransferGranularity["width"].asUInt();
            dest->properties_2.queueFamilyProperties.minImageTransferGranularity.height =
                minImagetransferGranularity["height"].asUInt();
            dest->properties_2.queueFamilyProperties.minImageTransferGranularity.depth =
                minImagetransferGranularity["depth"].asUInt();
        } else if (name == "VkQueueFamilyGlobalPriorityPropertiesKHR" || name == "VkQueueFamilyGlobalPriorityPropertiesEXT") {
            uint32_t i = 0;
            for (const auto &feature : props["priorities"]) {
                dest->global_priority_properties_.priorities[i++] = StringToVkQueueGlobalPriorityKHR(feature.asString());
            }
            dest->global_priority_properties_.priorityCount = props["priorityCount"].asUInt();
        } else if (name == "VkQueueFamilyVideoPropertiesKHR") {
            for (const auto &feature : props["videoCodecOperations"]) {
                dest->video_properties_.videoCodecOperations |= StringToVkVideoCodecOperationFlagsKHR(feature.asString());
            }
        } else if (name == "VkQueueFamilyCheckpointProperties2NV") {
            for (const auto &feature : props["checkpointExecutionStageMask"]) {
                dest->checkpoint_properties_2_.checkpointExecutionStageMask |= StringToVkPipelineStageFlags2(feature.asString());
            }
        } else if (name == "VkQueueFamilyCheckpointPropertiesNV") {
            for (const auto &feature : props["checkpointExecutionStageMask"]) {
                dest->checkpoint_properties_.checkpointExecutionStageMask |= StringToVkPipelineStageFlags(feature.asString());
            }
        } else if (name == "VkQueueFamilyQueryResultStatusPropertiesKHR") {
            dest->query_result_status_properties_.queryResultStatusSupport = props["queryResultStatusSupport"].asBool() ? VK_TRUE : VK_FALSE;
        }
    }

    bool valid = true;

    bool supported = false;
    for (const auto &device_qfp : pdd_->device_queue_family_properties_) {
        if (!QueueFamilyMatch(device_qfp.properties_2.queueFamilyProperties, dest->properties_2.queueFamilyProperties)) {
            continue;
        }
        if (!GlobalPriorityMatch(device_qfp.global_priority_properties_, dest->global_priority_properties_)) {
            continue;
        }
        if ((device_qfp.video_properties_.videoCodecOperations & dest->video_properties_.videoCodecOperations) !=
            dest->video_properties_.videoCodecOperations) {
            continue;
        }
        if ((device_qfp.checkpoint_properties_.checkpointExecutionStageMask &
             dest->checkpoint_properties_.checkpointExecutionStageMask) !=
            dest->checkpoint_properties_.checkpointExecutionStageMask) {
            continue;
        }
        if ((device_qfp.checkpoint_properties_2_.checkpointExecutionStageMask &
             dest->checkpoint_properties_2_.checkpointExecutionStageMask) !=
            dest->checkpoint_properties_2_.checkpointExecutionStageMask) {
            continue;
        }
        if (device_qfp.query_result_status_properties_.queryResultStatusSupport != dest->query_result_status_properties_.queryResultStatusSupport) {
            continue;
        }
        supported = true;
        break;
    }
    if (!supported) {
        std::string message =
            format("Device (%s) has no queue family that supports VkQueueFamilyProperties [queueFlags: %s, queueCount: %" PRIu32
                   ", timestampValidBits: %" PRIu32 ", minImageTransferGranularity: [%" PRIu32 ", %" PRIu32 ", %" PRIu32 "]]",
                   device_name,
                   GetQueueFlagsToString(dest->properties_2.queueFamilyProperties.queueFlags).c_str(),
                   dest->properties_2.queueFamilyProperties.queueCount, dest->properties_2.queueFamilyProperties.timestampValidBits,
                   dest->properties_2.queueFamilyProperties.minImageTransferGranularity.width,
                   dest->properties_2.queueFamilyProperties.minImageTransferGranularity.height,
                   dest->properties_2.queueFamilyProperties.minImageTransferGranularity.depth);
        if (dest->global_priority_properties_.priorityCount > 0) {
            std::string priorities = "[";
            for (uint32_t i = 0; i < dest->global_priority_properties_.priorityCount; ++i) {
                if (i > 0) {
                    priorities += ", ";
                }
                priorities += string_VkQueueGlobalPriorityKHR(dest->global_priority_properties_.priorities[i]);
            }
            priorities += "]";

            message += format(", VkQueueFamilyGlobalPriorityPropertiesKHR [priorityCount: %" PRIu32 ", priorities: %s]",
                              dest->global_priority_properties_.priorityCount, priorities.c_str());
        }
        if (dest->video_properties_.videoCodecOperations > 0) {
            message += format(", VkQueueFamilyVideoPropertiesKHR [videoCodecOperations: %s]",
                              string_VkVideoCodecOperationFlagsKHR(dest->video_properties_.videoCodecOperations).c_str());
        }
        if (dest->checkpoint_properties_.checkpointExecutionStageMask > 0) {
            message += format(", VkQueueFamilyCheckpointPropertiesNV [checkpointExecutionStageMask: %s]",
                              string_VkPipelineStageFlags(dest->checkpoint_properties_.checkpointExecutionStageMask).c_str());
        }
        if (dest->checkpoint_properties_2_.checkpointExecutionStageMask > 0) {
            message += format(", VkQueueFamilyCheckpointProperties2NV [checkpointExecutionStageMask: %s]",
                              string_VkPipelineStageFlags2(dest->checkpoint_properties_2_.checkpointExecutionStageMask).c_str());
        }
        if (dest->query_result_status_properties_.queryResultStatusSupport) {
            message += format(", VkQueueFamilyQueryResultStatusPropertiesKHR [queryResultStatusSupport: VK_TRUE]");
        }
        message += ".\\n";
        LogMessage(&layer_settings, DEBUG_REPORT_WARNING_BIT, message.c_str());
        valid = false;
    }

    return valid;
}
'''

QUEUE_FAMILY_FUNCTIONS = '''
bool QueueFamilyAndExtensionsMatch(const QueueFamilyProperties &device, const QueueFamilyProperties &profile) {
    if (!QueueFamilyMatch(device.properties_2.queueFamilyProperties, profile.properties_2.queueFamilyProperties)) {
        return false;
    }
    if (!GlobalPriorityMatch(device.global_priority_properties_, profile.global_priority_properties_)) {
        return false;
    }
    if ((device.video_properties_.videoCodecOperations & profile.video_properties_.videoCodecOperations) !=
        profile.video_properties_.videoCodecOperations) {
        return false;
    }
    if ((device.checkpoint_properties_.checkpointExecutionStageMask &
         profile.checkpoint_properties_.checkpointExecutionStageMask) !=
        profile.checkpoint_properties_.checkpointExecutionStageMask) {
        return false;
    }
    if ((device.checkpoint_properties_2_.checkpointExecutionStageMask &
         profile.checkpoint_properties_2_.checkpointExecutionStageMask) !=
        profile.checkpoint_properties_2_.checkpointExecutionStageMask) {
        return false;
    }
    if (device.query_result_status_properties_.queryResultStatusSupport != profile.query_result_status_properties_.queryResultStatusSupport) {
        return false;
    }
    return true;
}

void CopyUnsetQueueFamilyProperties(const QueueFamilyProperties *device, QueueFamilyProperties *profile) {
    if (profile->properties_2.queueFamilyProperties.queueFlags == 0) {
        profile->properties_2.queueFamilyProperties.queueFlags = device->properties_2.queueFamilyProperties.queueFlags;
    }
    if (profile->properties_2.queueFamilyProperties.queueCount == 0) {
        profile->properties_2.queueFamilyProperties.queueCount = device->properties_2.queueFamilyProperties.queueCount;
    }
    if (profile->properties_2.queueFamilyProperties.timestampValidBits == 0) {
        profile->properties_2.queueFamilyProperties.timestampValidBits =
            device->properties_2.queueFamilyProperties.timestampValidBits;
    }
    if (profile->properties_2.queueFamilyProperties.minImageTransferGranularity.width == 0) {
        profile->properties_2.queueFamilyProperties.minImageTransferGranularity.width =
            device->properties_2.queueFamilyProperties.minImageTransferGranularity.width;
    }
    if (profile->properties_2.queueFamilyProperties.minImageTransferGranularity.height == 0) {
        profile->properties_2.queueFamilyProperties.minImageTransferGranularity.height =
            device->properties_2.queueFamilyProperties.minImageTransferGranularity.height;
    }
    if (profile->properties_2.queueFamilyProperties.minImageTransferGranularity.depth == 0) {
        profile->properties_2.queueFamilyProperties.minImageTransferGranularity.depth =
            device->properties_2.queueFamilyProperties.minImageTransferGranularity.depth;
    }
}

bool JsonLoader::OrderQueueFamilyProperties(ArrayOfVkQueueFamilyProperties *qfp) {
    if (qfp->empty()) {
        return true;
    }
    // If device has less queue families than needed we can't load all profile queue families
    if (pdd_->device_queue_family_properties_.size() < qfp->size()) {
        return false;
    }
    // Find all permutations and see if any of them supports profile queue families
    std::vector<uint32_t> permutations(pdd_->device_queue_family_properties_.size());
    uint32_t count = static_cast<uint32_t>(permutations.size());
    for (uint32_t i = 0; i < count; ++i) {
        permutations[i] = i;
    }
    do {
        bool match = true;
        for (uint32_t i = 0; i < count; ++i) {
            if (permutations[i] < qfp->size() &&
                !QueueFamilyAndExtensionsMatch(pdd_->device_queue_family_properties_[i], (*qfp)[permutations[i]])) {
                match = false;
                break;
            }
        }
        if (match) {
            // Empty queue families at the end are not needed
            while (permutations[count - 1] >= qfp->size()) {
                --count;
            }
            ArrayOfVkQueueFamilyProperties ordered;
            for (uint32_t i = 0; i < count; ++i) {
                if (permutations[i] < qfp->size()) {
                    ordered.push_back((*qfp)[permutations[i]]);
                } else {
                    ordered.push_back(QueueFamilyProperties());
                }
            }
            *qfp = ordered;
            for (uint32_t i = 0; i < count; ++i) {
                CopyUnsetQueueFamilyProperties(&pdd_->device_queue_family_properties_[i], &(*qfp)[i]);
            }
            return true;
        }
    } while (std::next_permutation(permutations.begin(), permutations.end()));
    LogMessage(&layer_settings, DEBUG_REPORT_WARNING_BIT,
               "Device supports all individual profile queue families, but not all of them simultaneously.\\n");
    return false;
}
'''

READ_PROFILE = '''
VkResult JsonLoader::ReadProfile(const char *device_name, const Json::Value& root, const std::vector<std::vector<std::string>> &capabilities, bool requested_profile, bool enable_warnings) {
    bool failed = false;

    uint32_t properties_api_version = 0;
    uint32_t simulated_version = 0;

    const auto &cap_definisions = root["capabilities"];
    for (const auto &capability_variants : capabilities) {
        for (const auto &cap_variant : capability_variants) {
            const auto &cap_definision = cap_definisions[cap_variant];

            const auto &properties = cap_definision["properties"];
            if (properties.isMember("VkPhysicalDeviceProperties") && properties["VkPhysicalDeviceProperties"].isMember("apiVersion")) {
                properties_api_version = static_cast<uint32_t>(properties["VkPhysicalDeviceProperties"]["apiVersion"].asInt());
                simulated_version = properties_api_version;
            } else if (layer_settings.simulate.capabilities & SIMULATE_API_VERSION_BIT) {
                simulated_version = profile_api_version_;
            }
        }
    }
    if (simulated_version != 0 && requested_profile) {
        AddPromotedExtensions(simulated_version);
    }

    for (const auto &capability_variants : capabilities) {
        for (const auto &capability_variant : capability_variants) {
            const auto &cap_definision = cap_definisions[capability_variant];
            const auto &properties = cap_definision["properties"];

            if (VK_API_VERSION_PATCH(this->profile_api_version_) > VK_API_VERSION_PATCH(pdd_->physical_device_properties_.apiVersion)) {
                LogMessage(&layer_settings, DEBUG_REPORT_WARNING_BIT,
                    "Profile apiVersion (%" PRIu32 ".%" PRIu32 ".%" PRIu32 ") is greater than the device apiVersion (%" PRIu32 ".%" PRIu32 ".%" PRIu32 ").\\n",
                        VK_API_VERSION_MAJOR(this->profile_api_version_),
                        VK_API_VERSION_MINOR(this->profile_api_version_),
                        VK_API_VERSION_PATCH(this->profile_api_version_),
                        VK_API_VERSION_MAJOR(pdd_->physical_device_properties_.apiVersion),
                        VK_API_VERSION_MINOR(pdd_->physical_device_properties_.apiVersion),
                        VK_API_VERSION_PATCH(pdd_->physical_device_properties_.apiVersion));
                failed = true;
            }

            if (layer_settings.simulate.capabilities & SIMULATE_EXTENSIONS_BIT) {
                const auto &extensions = cap_definision["extensions"];

                pdd_->map_of_extension_properties_.reserve(extensions.size());
                for (const auto &e : extensions.getMemberNames()) {
                    VkExtensionProperties extension;
                    strcpy(extension.extensionName, e.c_str());
                    extension.specVersion = static_cast<uint32_t>(extensions[e].asInt());

                    bool found = pdd_->map_of_extension_properties_.count(e) > 0;

                    if (IsInstanceExtension(e.c_str())) {
                        LogMessage(&layer_settings, DEBUG_REPORT_NOTIFICATION_BIT,
                            "Required %s extension is an instance extension. The Profiles layer can't override instance extension, the extension is ignored.\\n", e.c_str());
                    }

                    if (!found) {
                        bool supported_on_device = pdd_->device_extensions_.count(e) > 0;

                        if (!supported_on_device) {
                            failed = true;
                        }
                        pdd_->map_of_extension_properties_.insert({e, extension});
                        if (!PhysicalDeviceData::HasSimulatedExtension(pdd_, extension.extensionName)) {
                            pdd_->simulation_extensions_.insert({e, extension});
                        }
                    }
                }
            }

            if (layer_settings.simulate.capabilities & SIMULATE_FEATURES_BIT) {
                const auto &features = cap_definision["features"];

                bool duplicated = !WarnDuplicatedFeature(features);
                if (duplicated) {
                    failed = true;
                }

                for (const auto &feature : features.getMemberNames()) {
                    if (features.isMember("VkPhysicalDeviceVulkan11Features")) {
                        pdd_->vulkan_1_1_features_written_ = true;
                    }
                    if (features.isMember("VkPhysicalDeviceVulkan12Features")) {
                        pdd_->vulkan_1_2_features_written_ = true;
                    }
                    if (features.isMember("VkPhysicalDeviceVulkan13Features")) {
                        pdd_->vulkan_1_3_features_written_ = true;
                    }
                    bool success = GetFeature(device_name, enable_warnings, features, feature);
                    if (!success) {
                        failed = true;
                    }
                }
            }

            if (layer_settings.simulate.capabilities & SIMULATE_PROPERTIES_BIT) {
                bool duplicated = !WarnDuplicatedProperty(properties);
                if (duplicated) {
                    failed = true;
                }

                if (properties.isMember("VkPhysicalDeviceVulkan11Properties")) {
                    pdd_->vulkan_1_1_properties_written_ = true;
                }
                if (properties.isMember("VkPhysicalDeviceVulkan12Properties")) {
                    pdd_->vulkan_1_2_properties_written_ = true;
                }
                if (properties.isMember("VkPhysicalDeviceVulkan13Properties")) {
                    pdd_->vulkan_1_3_properties_written_ = true;
                }
                for (const auto &prop : properties.getMemberNames()) {
                    bool success = GetProperty(device_name, enable_warnings, properties, prop);
                    if (!success) {
                        failed = true;
                    }
                }
            }

            if (layer_settings.simulate.capabilities & SIMULATE_FORMATS_BIT) {
                const auto &formats = cap_definision["formats"];

                for (const auto &format : formats.getMemberNames()) {
                    bool success = GetFormat(device_name, enable_warnings, formats, format, &pdd_->map_of_format_properties_, &pdd_->map_of_format_properties_3_);
                    if (!success) {
                        failed = true;
                    }
                }
            }

            if (layer_settings.simulate.capabilities & SIMULATE_QUEUE_FAMILY_PROPERTIES_BIT) {
                const auto &qf_props = cap_definision["queueFamiliesProperties"];

                bool queue_families_supported = true;
                for (const auto &qfp : qf_props) {
                    pdd_->arrayof_queue_family_properties_.emplace_back();
                    bool success = GetQueueFamilyProperties(device_name, qfp, &pdd_->arrayof_queue_family_properties_.back());
                    if (!success) {
                        queue_families_supported = false;
                        failed = true;
                    }
                }
                if (queue_families_supported) {
                    bool success = OrderQueueFamilyProperties(&pdd_->arrayof_queue_family_properties_);
                    if (!success) {
                        failed = true;
                    }
                }
            }
        }
    }

    if (requested_profile) {
        if (properties_api_version != 0) {
            LogMessage(&layer_settings, DEBUG_REPORT_NOTIFICATION_BIT,
                "- VkPhysicalDeviceProperties API version: %" PRIu32 ".%" PRIu32 ".%" PRIu32 ". Using the API version specified by the profile VkPhysicalDeviceProperties structure.\\n",
                VK_API_VERSION_MAJOR(properties_api_version), VK_API_VERSION_MINOR(properties_api_version), VK_API_VERSION_PATCH(properties_api_version));
        } else if (layer_settings.simulate.capabilities & SIMULATE_API_VERSION_BIT) {
            LogMessage(&layer_settings, DEBUG_REPORT_NOTIFICATION_BIT,
                "- VkPhysicalDeviceProperties API version: %" PRIu32 ".%" PRIu32 ".%" PRIu32". Using the API version specified by the profile.\\n",
                VK_API_VERSION_MAJOR(this->profile_api_version_), VK_API_VERSION_MINOR(this->profile_api_version_), VK_API_VERSION_PATCH(this->profile_api_version_));

            pdd_->physical_device_properties_.apiVersion = this->profile_api_version_;
        } else {
            LogMessage(&layer_settings, DEBUG_REPORT_NOTIFICATION_BIT,
                "- VkPhysicalDeviceProperties API version: %" PRIu32 ".%" PRIu32 ".%" PRIu32 ". Using the device version.\\n",
                    VK_API_VERSION_MAJOR(pdd_->physical_device_properties_.apiVersion),
                    VK_API_VERSION_MINOR(pdd_->physical_device_properties_.apiVersion),
                    VK_API_VERSION_PATCH(pdd_->physical_device_properties_.apiVersion));
        }
    }

    if (failed && layer_settings.log.debug_fail_on_error) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    return VK_SUCCESS;
}

VkResult JsonLoader::LoadFile(const std::string& filename) {
    if (filename.empty()) {
        return VK_SUCCESS;
    }
    std::ifstream json_file(filename);
    if (!json_file) {
        LogMessage(&layer_settings, DEBUG_REPORT_ERROR_BIT, "Fail to open file \\"%s\\"\\n", filename.c_str());
        return VK_SUCCESS;
    }

    Json::Value root = Json::nullValue;
    Json::CharReaderBuilder builder;
    std::string errs;
    bool success = Json::parseFromStream(builder, json_file, &root, &errs);
    if (!success) {
        LogMessage(&layer_settings, DEBUG_REPORT_ERROR_BIT, "Fail to parse file \\"%s\\" {\\n%s}\\n", filename.c_str(), errs.c_str());
        return VK_SUCCESS;
    }
    json_file.close();

    if (root.type() != Json::objectValue) {
        LogMessage(&layer_settings, DEBUG_REPORT_ERROR_BIT, "Json document root is not an object in file \\"%s\\"\\n", filename.c_str());
        return VK_SUCCESS;
    }

    const Json::Value& schema_node = root["$schema"];
    if (schema_node == Json::Value::nullSingleton()) {
        return VK_SUCCESS;
    }

    const std::string &schema = schema_node.asCString();
    if (schema.find("https://schema.khronos.org/vulkan/profiles") == std::string::npos) {
        return VK_SUCCESS;
    }

    LogMessage(&layer_settings, DEBUG_REPORT_NOTIFICATION_BIT, "Loading \\"%s\\"\\n", filename.c_str());

    if (layer_settings.simulate.profile_validation) {
        JsonValidator validator;
        if (!validator.Init()) {
            LogMessage(&layer_settings, DEBUG_REPORT_WARNING_BIT,
                "%s could not find the profile schema file to validate filename. This operation requires the Vulkan SDK to be installed. Skipping profile file validation.",
                kLayerName, filename.c_str());
        } else if (!validator.Check(root)) {
            LogMessage(&layer_settings, DEBUG_REPORT_ERROR_BIT,
                "%s is not a valid JSON profile file.", filename.c_str());
            if (layer_settings.log.debug_fail_on_error) {
                return VK_ERROR_INITIALIZATION_FAILED;
            } else {
                return VK_SUCCESS;
            }
        }
    }

    this->profiles_file_roots_.insert(std::pair(filename, root));

    return VK_SUCCESS;
}

VkResult JsonLoader::LoadProfilesDatabase() {
    if (!layer_settings.simulate.profile_file.empty()) {
        VkResult result = this->LoadFile(layer_settings.simulate.profile_file);
        if (result != VK_SUCCESS) {
            return result;
        }
    }

    for (std::size_t i = 0, n = layer_settings.simulate.profile_dirs.size(); i < n; ++i) {
        const std::string& path = layer_settings.simulate.profile_dirs[i];

      for (const auto& entry : fs::directory_iterator(path)) {
          if (fs::is_directory(entry.path())) {
              continue;
          }

          const std::string& file_path = entry.path().generic_string();
          if (!EndsWith(file_path, ".json")) {
              continue;
          }

          VkResult result = this->LoadFile(file_path);
          if (result != VK_SUCCESS) {
              continue;
          }
      }
    }

    LogFoundProfiles();

    ReadProfileApiVersion();

    return VK_SUCCESS;
}

void JsonLoader::LogFoundProfiles() {
    for (const auto& root : this->profiles_file_roots_) {
        LogMessage(&layer_settings, DEBUG_REPORT_NOTIFICATION_BIT, "Profiles found in \'%s\' file:\\n", root.first.c_str());

        const Json::Value &profiles = root.second["profiles"];

        for (const std::string &profile : profiles.getMemberNames()) {
            LogMessage(&layer_settings, DEBUG_REPORT_NOTIFICATION_BIT, "- %s\\n", profile.c_str());
        }
    }
}

const Json::Value& JsonLoader::FindRootFromProfileName(const std::string& profile_name) const {
    for (const auto& root : this->profiles_file_roots_) {
        const Json::Value &profiles = root.second["profiles"];

        for (const auto &profile : profiles.getMemberNames()) {
            if (profile_name.empty() || profile_name == "${VP_DEFAULT}" || profile == profile_name) {
                return root.second;
            }
        }
    }

    return Json::Value::nullSingleton();
}

void JsonLoader::ReadProfileApiVersion() {
    const std::string &profile_name = layer_settings.simulate.profile_name;
    const Json::Value &profiles = FindRootFromProfileName(profile_name)["profiles"];
    bool found_profile = false;
    for (const auto &profile : profiles.getMemberNames()) {
        if (profile_name.empty() || profile_name == "${VP_DEFAULT}" || profile == profile_name) {
            const std::string version_string = profiles[profile]["api-version"].asCString();

            uint32_t api_major = 0;
            uint32_t api_minor = 0;
            uint32_t api_patch = 0;
            std::sscanf(version_string.c_str(), "%u.%u.%u", &api_major, &api_minor, &api_patch);
            profile_api_version_ = VK_MAKE_API_VERSION(0, api_major, api_minor, api_patch);
            found_profile = true;
            break;
        }
    }
    if (!found_profile) {
        for (const auto &profile : profiles.getMemberNames()) {
            const std::string version_string = profiles[profile]["api-version"].asCString();

            uint32_t api_major = 0;
            uint32_t api_minor = 0;
            uint32_t api_patch = 0;
            std::sscanf(version_string.c_str(), "%u.%u.%u", &api_major, &api_minor, &api_patch);
            profile_api_version_ = VK_MAKE_API_VERSION(0, api_major, api_minor, api_patch);
            break; // Systematically load the first and default profile when the profile is not found
        }
    }

    for (const auto& extension : layer_settings.simulate.exclude_device_extensions) {
        if (extension.empty()) continue;
        excluded_extensions_.push_back(extension);
    }
    for (const auto& format : layer_settings.simulate.exclude_formats) {
        if (format.empty()) continue;
        excluded_formats_.push_back(format);
    }
}

void JsonLoader::CollectProfiles(const std::string& profile_name, std::vector<std::string>& results) const {
    const auto &root = FindRootFromProfileName(profile_name);

    if (root != Json::Value::nullSingleton()) {
        const Json::Value &profiles = root["profiles"];
        std::vector<std::vector<std::string>> capabilities;

        for (const auto &profile : profiles.getMemberNames()) {
            if (profile == profile_name) {
                const auto &required_profiles = profiles[profile]["profiles"];

                for (const auto &required_profile : required_profiles) {
                    this->CollectProfiles(required_profile.asString().c_str(), results);
                }

                break;  // load a single profile
            }
        }
    }

    results.push_back(profile_name);
}

VkResult JsonLoader::LoadDevice(const char* device_name, PhysicalDeviceData *pdd) {
    pdd_ = pdd;

    const std::string &requested_profile_name = layer_settings.simulate.profile_name;

    if (this->profiles_file_roots_.empty() && (requested_profile_name.empty() || requested_profile_name == "${VP_DEFAULT}")) {
        return VK_SUCCESS;
    }

    VkResult result = VK_SUCCESS;

    std::vector<std::string> required_profiles;
    CollectProfiles(requested_profile_name, required_profiles);

    for (const std::string& profile_name : required_profiles) {
        const auto& root = FindRootFromProfileName(profile_name);

        if (root == Json::Value::nullSingleton()) {
            if (requested_profile_name == profile_name) {
                LogMessage(&layer_settings, DEBUG_REPORT_ERROR_BIT, "- \'%s\' profile not found.\\n", profile_name.c_str());
            } else {
                LogMessage(&layer_settings, DEBUG_REPORT_ERROR_BIT, "- \'%s\' profile required by \'%s\' not found.\\n", profile_name.c_str(), requested_profile_name.c_str());
            }

            result = VK_ERROR_UNKNOWN;
        } else {
            if (requested_profile_name == profile_name) {
                LogMessage(&layer_settings, DEBUG_REPORT_NOTIFICATION_BIT, "- Overriding device capabilities with the \'%s\' profile capabilities.\\n", profile_name.c_str());
            } else {
                LogMessage(&layer_settings, DEBUG_REPORT_NOTIFICATION_BIT, "- Overriding device capabilities with the \'%s\' profile capabilities required by the requested \'%s\' profile.\\n", profile_name.c_str(), requested_profile_name.c_str());
            }

            const Json::Value &profiles = root["profiles"];
            std::vector<std::vector<std::string>> capabilities;

            bool found_profile = false;
            for (const auto &profile : profiles.getMemberNames()) {
                if (profile_name.empty() || profile_name == "${VP_DEFAULT}" || profile == profile_name) {
                    const auto &caps = profiles[profile]["capabilities"];

                    for (const auto &cap : caps) {
                        std::vector<std::string> cap_variants;
                        if (cap.isArray()) {
                            for (const auto &cap_variant : cap) {
                                cap_variants.push_back(cap_variant.asString());
                            }
                        } else {
                            cap_variants.push_back(cap.asString());
                        }
                        capabilities.push_back(cap_variants);
                    }

                    found_profile = true;
                    break;  // load a single profile
                }
            }
            if (!found_profile) {
                for (const auto &profile : profiles.getMemberNames()) {
                    const auto &caps = profiles[profile]["capabilities"];

                    for (const auto &cap : caps) {
                        std::vector<std::string> cap_variants;
                        if (cap.isArray()) {
                            for (const auto &cap_variant : cap) {
                                cap_variants.push_back(cap_variant.asString());
                            }
                        } else {
                            cap_variants.push_back(cap.asString());
                        }
                        capabilities.push_back(cap_variants);
                    }

                    break; // Systematically load the first and default profile
                }
            }

            if (capabilities.empty()) {
                return VK_SUCCESS;
            }

            const Json::Value schema_value = root["$schema"];
            if (!schema_value.isString()) {
                LogMessage(&layer_settings, DEBUG_REPORT_ERROR_BIT, "JSON element \\"$schema\\" is not a string\\n");
                return layer_settings.log.debug_fail_on_error ? VK_ERROR_INITIALIZATION_FAILED : VK_SUCCESS;
            }

            const std::string schema = schema_value.asCString();
            if (schema.find(SCHEMA_URI_BASE) == std::string::npos) {
                LogMessage(&layer_settings, DEBUG_REPORT_ERROR_BIT, "Document schema \\"%s\\" not supported by %s\\n", schema.c_str(), kLayerName);
                return layer_settings.log.debug_fail_on_error ? VK_ERROR_INITIALIZATION_FAILED : VK_SUCCESS;
            }

            const std::size_t size_schema = schema.size();
            const std::size_t size_base = std::strlen(SCHEMA_URI_BASE);
            const std::size_t size_version = std::strlen(".json#");
            const std::string version = schema.substr(size_base, size_schema - size_base - size_version);

            uint32_t version_major = 0;
            uint32_t version_minor = 0;
            uint32_t version_patch = 0;
            std::sscanf(version.c_str(), "%u.%u.%u", &version_major, &version_minor, &version_patch);
            if (VK_HEADER_VERSION < version_patch) {
                LogMessage(&layer_settings, DEBUG_REPORT_WARNING_BIT,
                    "%s is built against Vulkan Header %d but the profile is written against Vulkan Header %d.\\n\\t- All newer capabilities in the profile will be ignored by the layer.\\n",
                    kLayerName, VK_HEADER_VERSION, version_patch);
            }

            VkResult tmp_result = VK_SUCCESS;
            if (layer_settings.simulate.capabilities & SIMULATE_EXTENSIONS_BIT) {
                pdd_->simulation_extensions_.clear();
            }

            tmp_result = ReadProfile(device_name, root, capabilities, requested_profile_name == profile_name, required_profiles.size() == 1);
            if (tmp_result != VK_SUCCESS) {
                result = tmp_result;
            }
        }

        if (result != VK_SUCCESS) {
            break;
        }
    }

    return result;
}
'''

GET_DEFINES = '''
#define GET_VALUE(member, name, not_modifiable, requested_profile) GetValue(device_name, parent, member, #name, &dest->name, not_modifiable, requested_profile)
#define GET_ARRAY(member, name, not_modifiable) GetArray(device_name, parent, member, #name, dest->name, not_modifiable)

#define GET_VALUE_WARN(member, name, not_modifiable, requested_profile, warn_func)                     \\
    if (!GetValue(device_name, parent, member, #name, &dest->name, not_modifiable, requested_profile, warn_func)) { \\
        valid = false;                                                              \\
    }
#define GET_VALUE_SIZE_T_WARN(member, name, not_modifiable, requested_profile, warn_func)                   \\
    if (!GetValueSizet(device_name, parent, member, #name, &dest->name, not_modifiable, requested_profile, warn_func)) { \\
        valid = false;                                                                   \\
    }
#define GET_VALUE_FLAG_WARN(member, name, not_modifiable, requested_profile)                    \\
    if (!GetValueFlag(device_name, parent, member, #name, &dest->name, not_modifiable, requested_profile)) { \\
        valid = false;                                                       \\
    }
#define GET_VALUE_ENUM_WARN(member, name, not_modifiable, requested_profile, warn_func)                    \\
    if (!GetValueEnum(device_name, parent, member, #name, &dest->name, not_modifiable, requested_profile, warn_func)) { \\
        valid = false;                                                                  \\
    }

'''

GET_UNDEFINE = '''
#undef GET_VALUE
#undef GET_ARRAY
'''

INSTANCE_FUNCTIONS = '''
// Generic layer dispatch table setup, see [LALI].
static VkResult LayerSetupCreateInstance(const VkInstanceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator,
                                         VkInstance *pInstance) {
    VkLayerInstanceCreateInfo *chain_info = get_chain_info(pCreateInfo, VK_LAYER_LINK_INFO);
    assert(chain_info->u.pLayerInfo);

    PFN_vkGetInstanceProcAddr fp_get_instance_proc_addr = chain_info->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    PFN_vkCreateInstance fp_create_instance = (PFN_vkCreateInstance)fp_get_instance_proc_addr(nullptr, "vkCreateInstance");
    if (!fp_create_instance) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    chain_info->u.pLayerInfo = chain_info->u.pLayerInfo->pNext;
    VkResult result = fp_create_instance(pCreateInfo, pAllocator, pInstance);
    if (result == VK_SUCCESS) {
        initInstanceTable(*pInstance, fp_get_instance_proc_addr);
        JsonLoader::Store(*pInstance);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateInstance(const VkInstanceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator,
                                              VkInstance *pInstance) {
    JsonLoader &json_loader = JsonLoader::Create();

    ProfileLayerSettings *layer_settings = &json_loader.layer_settings;

    InitProfilesLayerSettings(pCreateInfo, pAllocator, layer_settings);

    LogMessage(layer_settings, DEBUG_REPORT_DEBUG_BIT, "CreateInstance\\n");
    LogMessage(layer_settings, DEBUG_REPORT_DEBUG_BIT, "JsonCpp version %s\\n", JSONCPP_VERSION_STRING);
    LogMessage(layer_settings, DEBUG_REPORT_NOTIFICATION_BIT, "%s version %d.%d.%d\\n", kLayerName, kVersionProfilesMajor,
                                                       kVersionProfilesMinor, kVersionProfilesPatch);

    VkResult result = json_loader.LoadProfilesDatabase();
    if (result != VK_SUCCESS) {
        return result;
    }

    const VkApplicationInfo *app_info = pCreateInfo->pApplicationInfo;
    requested_version = (app_info && app_info->apiVersion) ? app_info->apiVersion : VK_API_VERSION_1_0;
    if (VK_API_VERSION_MAJOR(requested_version) > VK_API_VERSION_MAJOR(VK_HEADER_VERSION_COMPLETE) ||
        VK_API_VERSION_MINOR(requested_version) > VK_API_VERSION_MINOR(VK_HEADER_VERSION_COMPLETE)) {
        LogMessage(layer_settings, DEBUG_REPORT_ERROR_BIT, "The Vulkan application requested a Vulkan %s instance but the %s was build "
                                                    "against %s. Please, update the layer.\\n",
                                                    StringAPIVersion(requested_version).c_str(), kLayerName,
                                                    StringAPIVersion(VK_HEADER_VERSION_COMPLETE).c_str());
        if (layer_settings->log.debug_fail_on_error) {
            return VK_ERROR_INITIALIZATION_FAILED;
        }
    }


    bool changed_version = false;
    if (!layer_settings->simulate.profile_file.empty() || !layer_settings->simulate.profile_dirs.empty()) {
        const uint32_t profile_api_version = json_loader.GetProfileApiVersion();
        if (VK_API_VERSION_MAJOR(requested_version) < VK_API_VERSION_MAJOR(profile_api_version) ||
            VK_API_VERSION_MINOR(requested_version) < VK_API_VERSION_MINOR(profile_api_version)) {
            if (layer_settings->simulate.capabilities & SIMULATE_API_VERSION_BIT) {
                if (layer_settings->simulate.profile_name.empty()) {
                    LogMessage(layer_settings,
                        DEBUG_REPORT_NOTIFICATION_BIT,
                        "The Vulkan application requested a Vulkan %s instance but the selected %s file requires %s. The "
                                 "application requested instance version is overridden to %s.\\n",
                                 StringAPIVersion(requested_version).c_str(), layer_settings->simulate.profile_file.c_str(),
                                 StringAPIVersion(profile_api_version).c_str(), StringAPIVersion(profile_api_version).c_str());
                } else {
                    LogMessage(layer_settings,
                        DEBUG_REPORT_NOTIFICATION_BIT,
                        "The Vulkan application requested a Vulkan %s instance but the selected %s profile requires %s. "
                                 "The application requested instance version is overridden to %s.\\n",
                                 StringAPIVersion(requested_version).c_str(), layer_settings->simulate.profile_name.c_str(),
                                 StringAPIVersion(profile_api_version).c_str(), StringAPIVersion(profile_api_version).c_str());
                }
                requested_version = profile_api_version;
                changed_version = true;
            } else {
                if (layer_settings->simulate.profile_name.empty()) {
                    LogMessage(layer_settings,
                        DEBUG_REPORT_WARNING_BIT,
                        "The Vulkan application requested a Vulkan %s instance but the selected %s file requires %s. The "
                                 "profile may not be initialized correctly which will produce unexpected warning messages.\\n",
                                 StringAPIVersion(requested_version).c_str(), layer_settings->simulate.profile_file.c_str(),
                                 StringAPIVersion(profile_api_version).c_str());
                } else {
                    LogMessage(layer_settings,
                        DEBUG_REPORT_WARNING_BIT,
                        "The Vulkan application requested a Vulkan %s instance but the selected %s profile requires %s. "
                                 "The profile may not be initialized correctly which will produce unexpected warning messages.\\n",
                                 StringAPIVersion(requested_version).c_str(), layer_settings->simulate.profile_name.c_str(),
                                 StringAPIVersion(profile_api_version).c_str());
                }
            }
        }
    }

    std::lock_guard<std::recursive_mutex> lock(global_lock);

    bool get_physical_device_properties2_active = false;
    if (VK_API_VERSION_MINOR(requested_version) > 0) {
        get_physical_device_properties2_active = true;
    } else {
        for (uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; i++) {
            if (strncmp(pCreateInfo->ppEnabledExtensionNames[i], VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
                        VK_MAX_EXTENSION_NAME_SIZE) == 0) {
                get_physical_device_properties2_active = true;
                break;
            }
        }
    }
    if (!changed_version && get_physical_device_properties2_active) {
        return LayerSetupCreateInstance(pCreateInfo, pAllocator, pInstance);
    }

    if (!get_physical_device_properties2_active) {
        LogMessage(layer_settings, DEBUG_REPORT_NOTIFICATION_BIT,
                   "The Profiles Layer requires the %s extension, but it was not included in "
                            "VkInstanceCreateInfo::ppEnabledExtensionNames, adding the extension.\\n",
                            VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    }

    // Handle VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
    bool has_port_enum = false;
    for (uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; i++) {
        if (strncmp(pCreateInfo->ppEnabledExtensionNames[i], VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
                    VK_MAX_EXTENSION_NAME_SIZE) == 0) {
            has_port_enum = true;
            break;
        }
    }

    // Add VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
    VkApplicationInfo new_app_info;
    if (app_info) {
        new_app_info.sType = app_info->sType;
        new_app_info.pNext = app_info->pNext;
        new_app_info.pApplicationName = app_info->pApplicationName;
        new_app_info.applicationVersion = app_info->applicationVersion;
        new_app_info.pEngineName = app_info->pEngineName;
        new_app_info.engineVersion = app_info->engineVersion;
        new_app_info.apiVersion = requested_version;
    } else {
        new_app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        new_app_info.apiVersion = requested_version;
    }

    VkInstanceCreateInfo create_info;
    create_info.sType = pCreateInfo->sType;
    create_info.pNext = pCreateInfo->pNext;
    create_info.flags = pCreateInfo->flags;
    if (has_port_enum) {
        create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    }

    if (changed_version) {
        create_info.pApplicationInfo = &new_app_info;
    } else {
        create_info.pApplicationInfo = app_info;
    }
    create_info.enabledLayerCount = pCreateInfo->enabledLayerCount;
    create_info.ppEnabledLayerNames = pCreateInfo->ppEnabledLayerNames;
    std::vector<const char *> extension_names;
    if (!get_physical_device_properties2_active) {
        create_info.enabledExtensionCount = pCreateInfo->enabledExtensionCount + 1;
        extension_names.resize(create_info.enabledExtensionCount);
        for (uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; ++i) {
            extension_names[i] = pCreateInfo->ppEnabledExtensionNames[i];
        }
        extension_names[pCreateInfo->enabledExtensionCount] = VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME;
        create_info.ppEnabledExtensionNames = extension_names.data();
    } else {
        create_info.enabledExtensionCount = pCreateInfo->enabledExtensionCount;
        create_info.ppEnabledExtensionNames = pCreateInfo->ppEnabledExtensionNames;
    }
    return LayerSetupCreateInstance(&create_info, pAllocator, pInstance);
}

VKAPI_ATTR void VKAPI_CALL DestroyInstance(VkInstance instance, const VkAllocationCallbacks *pAllocator) {
    if (instance) {
        std::lock_guard<std::recursive_mutex> lock(global_lock);

        ProfileLayerSettings* layer_settings = &JsonLoader::Find(instance)->layer_settings;

        LogMessage(layer_settings, DEBUG_REPORT_DEBUG_BIT, "DestroyInstance\\n");

        {
            const auto dt = instance_dispatch_table(instance);

            std::vector<VkPhysicalDevice> physical_devices;
            VkResult err = EnumerateAll<VkPhysicalDevice>(physical_devices, [&](uint32_t *count, VkPhysicalDevice *results) {
                return dt->EnumeratePhysicalDevices(instance, count, results);
            });
            assert(!err);
            if (!err)
                for (const auto pd : physical_devices) PhysicalDeviceData::Destroy(pd);

            dt->DestroyInstance(instance, pAllocator);
        }
        destroy_instance_dispatch_table(get_dispatch_key(instance));

        JsonLoader::Destroy(instance);
    }
}
'''

FORMAT_PROPERTIES_PNEXT = '''
void FillFormatPropertiesPNextChain(PhysicalDeviceData *physicalDeviceData, void *place, VkFormat format) {
    while (place) {
        VkBaseOutStructure *structure = (VkBaseOutStructure *)place;

        // These switch statements check which struct is in the pNext chain and, if the physical device has the proper extension,
        // fill the struct with any override data provided by the PhysicalDeviceData object.

        switch (structure->sType) {
            case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3: {
                if (!physicalDeviceData->map_of_format_properties_3_.empty()) {
                    VkFormatProperties3 *sp = (VkFormatProperties3 *)place;
                    void *pNext = sp->pNext;
                    *sp = physicalDeviceData->map_of_format_properties_3_[format];
                    sp->pNext = pNext;
                }
            } break;
            default:
                break;
        }

        place = structure->pNext;
    }
}
'''

GET_PHYSICAL_DEVICE_FEATURES_PROPERTIES_FUNCTIONS = '''
VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties *pProperties) {
    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(physicalDevice);

    PhysicalDeviceData *pdd = PhysicalDeviceData::Find(physicalDevice);
    if (pdd) {
        *pProperties = pdd->physical_device_properties_;
    } else {
        dt->GetPhysicalDeviceProperties(physicalDevice, pProperties);
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceProperties2(VkPhysicalDevice physicalDevice,
                                                        VkPhysicalDeviceProperties2KHR *pProperties) {
    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(physicalDevice);
    dt->GetPhysicalDeviceProperties2(physicalDevice, pProperties);
    GetPhysicalDeviceProperties(physicalDevice, &pProperties->properties);
    PhysicalDeviceData *pdd = PhysicalDeviceData::Find(physicalDevice);
    FillPNextChain(pdd, pProperties->pNext);
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceProperties2KHR(VkPhysicalDevice physicalDevice,
                                                           VkPhysicalDeviceProperties2KHR *pProperties) {
    GetPhysicalDeviceProperties2(physicalDevice, pProperties);
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures *pFeatures) {
    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(physicalDevice);

    PhysicalDeviceData *pdd = PhysicalDeviceData::Find(physicalDevice);
    if (pdd) {
        *pFeatures = pdd->physical_device_features_;
    } else {
        dt->GetPhysicalDeviceFeatures(physicalDevice, pFeatures);
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFeatures2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures2KHR *pFeatures) {
    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(physicalDevice);

    PhysicalDeviceData *pdd = PhysicalDeviceData::Find(physicalDevice);
    if (pdd) {
        FillPNextChain(pdd, pFeatures->pNext);
    } else {
        dt->GetPhysicalDeviceFeatures2(physicalDevice, pFeatures);
    }

    GetPhysicalDeviceFeatures(physicalDevice, &pFeatures->features);
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFeatures2KHR(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures2KHR *pFeatures) {
    GetPhysicalDeviceFeatures2(physicalDevice, pFeatures);
}
'''

ENUMERATE_FUNCTIONS = '''
VKAPI_ATTR VkResult VKAPI_CALL EnumerateInstanceLayerProperties(uint32_t *pCount, VkLayerProperties *pProperties) {
    return EnumerateProperties(kLayerPropertiesCount, kLayerProperties, pCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL EnumerateInstanceExtensionProperties(const char *pLayerName, uint32_t *pCount,
                                                                    VkExtensionProperties *pProperties) {
    if (pLayerName && !strcmp(pLayerName, kLayerName)) {
        return EnumerateProperties(kInstanceExtensionPropertiesCount, kInstanceExtensionProperties, pCount, pProperties);
    }
    return VK_ERROR_LAYER_NOT_PRESENT;
}

// Per [LALI], EnumerateDeviceLayerProperties() is deprecated.

VKAPI_ATTR VkResult VKAPI_CALL EnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice, const char *pLayerName,
                                                                  uint32_t *pCount, VkExtensionProperties *pProperties) {
    VkResult result = VK_SUCCESS;
    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(physicalDevice);

    uint32_t pCount_copy = *pCount;

    PhysicalDeviceData *pdd = PhysicalDeviceData::Find(physicalDevice);
    ProfileLayerSettings* layer_settings = &JsonLoader::Find(pdd->instance())->layer_settings;
    if (pLayerName) {
        if (strcmp(pLayerName, kLayerName) == 0)
            result = EnumerateProperties(kDeviceExtensionPropertiesCount, kDeviceExtensionProperties.data(), pCount, pProperties);
        else
            result = dt->EnumerateDeviceExtensionProperties(physicalDevice, pLayerName, pCount, pProperties);
    } else if (pdd == nullptr || (!(layer_settings->simulate.capabilities & SIMULATE_EXTENSIONS_BIT) &&
                                  layer_settings->simulate.exclude_device_extensions.empty())) {
        result = dt->EnumerateDeviceExtensionProperties(physicalDevice, pLayerName, pCount, pProperties);
    } else {
        result = EnumerateExtensions(pdd->simulation_extensions_, pCount, pProperties);
    }

    if (result == VK_SUCCESS && !pLayerName && layer_settings->simulate.emulate_portability &&
        !PhysicalDeviceData::HasSimulatedOrRealExtension(physicalDevice, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME)) {
        if (pProperties) {
            if (pCount_copy >= *pCount + 1) {
                strncpy(pProperties[*pCount].extensionName, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME, VK_MAX_EXTENSION_NAME_SIZE);
                pProperties[*pCount].specVersion = VK_KHR_PORTABILITY_SUBSET_SPEC_VERSION;
                *pCount += 1;
            }
        } else {
            *pCount += 1;
        }
    }

    return result;
}
'''

QUEUE_FAMILY_PROPERTIES_FUNCTIONS = '''
VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice,
                                                                  uint32_t *pQueueFamilyPropertyCount,
                                                                  VkQueueFamilyProperties *pQueueFamilyProperties) {
    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(physicalDevice);

    // Are there JSON overrides, or should we call down to return the original values?
    PhysicalDeviceData *pdd = PhysicalDeviceData::Find(physicalDevice);
    const uint32_t src_count = (pdd) ? static_cast<uint32_t>(pdd->arrayof_queue_family_properties_.size()) : 0;
    if (src_count == 0) {
        dt->GetPhysicalDeviceQueueFamilyProperties(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
        return;
    }

    // Careful: cannot use EnumerateProperties() here! (because src and dst structs are not the same type)
    if (!pQueueFamilyProperties) {
        *pQueueFamilyPropertyCount = src_count;
        return;
    }
    const uint32_t copy_count = (*pQueueFamilyPropertyCount < src_count) ? *pQueueFamilyPropertyCount : src_count;
    const QueueFamilyProperties *src_props = pdd->arrayof_queue_family_properties_.data();
    for (uint32_t i = 0; i < copy_count; ++i) {
        pQueueFamilyProperties[i] = src_props[i].properties_2.queueFamilyProperties;
    }
    *pQueueFamilyPropertyCount = copy_count;
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceQueueFamilyProperties2KHR(VkPhysicalDevice physicalDevice,
                                                                      uint32_t *pQueueFamilyPropertyCount,
                                                                      VkQueueFamilyProperties2KHR *pQueueFamilyProperties2) {
    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(physicalDevice);

    // Are there JSON overrides, or should we call down to return the original values?
    PhysicalDeviceData *pdd = PhysicalDeviceData::Find(physicalDevice);
    const uint32_t src_count = (pdd) ? static_cast<uint32_t>(pdd->arrayof_queue_family_properties_.size()) : 0;
    if (src_count == 0) {
        dt->GetPhysicalDeviceQueueFamilyProperties2(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties2);
        return;
    }

    if (!pQueueFamilyProperties2) {
        *pQueueFamilyPropertyCount = src_count;
        return;
    }

    // Careful: cannot use EnumerateProperties() here! (because src and dst structs are not the same type)
    const uint32_t copy_count = (*pQueueFamilyPropertyCount < src_count) ? *pQueueFamilyPropertyCount : src_count;
    const QueueFamilyProperties *src_props = pdd->arrayof_queue_family_properties_.data();
    for (uint32_t i = 0; i < copy_count; ++i) {
        pQueueFamilyProperties2[i].queueFamilyProperties = src_props[i].properties_2.queueFamilyProperties;
    }
    *pQueueFamilyPropertyCount = copy_count;
    FillQueueFamilyPropertiesPNextChain(pdd, pQueueFamilyProperties2, copy_count);
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceQueueFamilyProperties2(VkPhysicalDevice physicalDevice,
                                                                   uint32_t *pQueueFamilyPropertyCount,
                                                                   VkQueueFamilyProperties2KHR *pQueueFamilyProperties2) {
    GetPhysicalDeviceQueueFamilyProperties2KHR(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties2);
}
'''

PHYSICAL_DEVICE_FORMAT_FUNCTIONS = '''
VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format,
                                                             VkFormatProperties *pFormatProperties) {
    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(physicalDevice);

    // Are there JSON overrides, or should we call down to return the original values?
    PhysicalDeviceData *pdd = PhysicalDeviceData::Find(physicalDevice);
    ProfileLayerSettings* layer_settings = &JsonLoader::Find(pdd->instance())->layer_settings;

    // Check if Format was excluded
    for (std::size_t j = 0, m = layer_settings->simulate.exclude_formats.size(); j < m; ++j) {
        const std::string &excluded_format = layer_settings->simulate.exclude_formats[j];
        if (excluded_format.empty()) continue;

        if (StringToFormat(excluded_format) == format) {
            *pFormatProperties = VkFormatProperties{};
            return;
        }
    }

    const uint32_t src_count = (pdd) ? static_cast<uint32_t>(pdd->map_of_format_properties_.size()) : 0;
    if (src_count == 0) {
        dt->GetPhysicalDeviceFormatProperties(physicalDevice, format, pFormatProperties);
    } else {
        VkFormatProperties device_format = {};
        dt->GetPhysicalDeviceFormatProperties(physicalDevice, format, &device_format);
        const auto iter = pdd->map_of_format_properties_.find(format);

        if ((layer_settings->simulate.capabilities & SIMULATE_FORMATS_BIT)) {
            *pFormatProperties = (iter != pdd->map_of_format_properties_.end()) ? iter->second : VkFormatProperties{};
        } else {
            *pFormatProperties = device_format;
        }

        if (IsFormatSupported(*pFormatProperties) && iter != pdd->map_of_format_properties_.end()) {
            if ((layer_settings->simulate.capabilities & SIMULATE_FORMATS_BIT)) {
                *pFormatProperties = iter->second;
            } else {
                *pFormatProperties = device_format;
            }

            if (!HasFlags(device_format.linearTilingFeatures, pFormatProperties->linearTilingFeatures) ||
                !HasFlags(device_format.optimalTilingFeatures, pFormatProperties->optimalTilingFeatures) ||
                !HasFlags(device_format.bufferFeatures, pFormatProperties->bufferFeatures)) {
                LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                           "format %s is simulating unsupported features!\\n", vkFormatToString(format).c_str());
            }
        }
    }

    LogFlush(layer_settings);
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFormatProperties2(VkPhysicalDevice physicalDevice, VkFormat format,
                                                              VkFormatProperties2KHR *pFormatProperties) {
    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(physicalDevice);
    dt->GetPhysicalDeviceFormatProperties2(physicalDevice, format, pFormatProperties);
    GetPhysicalDeviceFormatProperties(physicalDevice, format, &pFormatProperties->formatProperties);
    PhysicalDeviceData *pdd = PhysicalDeviceData::Find(physicalDevice);
    FillFormatPropertiesPNextChain(pdd, pFormatProperties->pNext, format);
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFormatProperties2KHR(VkPhysicalDevice physicalDevice, VkFormat format,
                                                                 VkFormatProperties2KHR *pFormatProperties) {
    GetPhysicalDeviceFormatProperties2(physicalDevice, format, pFormatProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format,
                                                                      VkImageType type, VkImageTiling tiling,
                                                                      VkImageUsageFlags usage, VkImageCreateFlags flags,
                                                                      VkImageFormatProperties *pImageFormatProperties) {
    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(physicalDevice);

    PhysicalDeviceData *pdd = PhysicalDeviceData::Find(physicalDevice);
    ProfileLayerSettings* layer_settings = &JsonLoader::Find(pdd->instance())->layer_settings;

    // Are there JSON overrides, or should we call down to return the original values?
    if (!(layer_settings->simulate.capabilities & SIMULATE_FORMATS_BIT)) {
        return dt->GetPhysicalDeviceImageFormatProperties(physicalDevice, format, type, tiling, usage, flags,
                                                          pImageFormatProperties);
    }

    VkFormatProperties fmt_props = {};
    GetPhysicalDeviceFormatProperties(physicalDevice, format, &fmt_props);

    if (!IsFormatSupported(fmt_props)) {
        *pImageFormatProperties = VkImageFormatProperties{};
        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    }

    VkResult result =
        dt->GetPhysicalDeviceImageFormatProperties(physicalDevice, format, type, tiling, usage, flags, pImageFormatProperties);

    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceImageFormatProperties2KHR(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceImageFormatInfo2KHR *pImageFormatInfo,
    VkImageFormatProperties2KHR *pImageFormatProperties) {
    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(physicalDevice);
    dt->GetPhysicalDeviceImageFormatProperties2(physicalDevice, pImageFormatInfo, pImageFormatProperties);
    return GetPhysicalDeviceImageFormatProperties(physicalDevice, pImageFormatInfo->format, pImageFormatInfo->type,
                                                  pImageFormatInfo->tiling, pImageFormatInfo->usage, pImageFormatInfo->flags,
                                                  &pImageFormatProperties->imageFormatProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceImageFormatProperties2(VkPhysicalDevice physicalDevice,
                                                                       const VkPhysicalDeviceImageFormatInfo2 *pImageFormatInfo,
                                                                       VkImageFormatProperties2 *pImageFormatProperties) {
    return GetPhysicalDeviceImageFormatProperties2KHR(physicalDevice, pImageFormatInfo, pImageFormatProperties);
}
'''

TOOL_PROPERTIES_FUNCTIONS = '''
VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceToolPropertiesEXT(VkPhysicalDevice physicalDevice, uint32_t *pToolCount,
                                                                  VkPhysicalDeviceToolPropertiesEXT *pToolProperties) {
    std::stringstream version_stream;
    version_stream << kVersionProfilesMajor << "." << kVersionProfilesMinor << "." << kVersionProfilesPatch;
    std::string version_string(version_stream.str());

    static VkPhysicalDeviceToolPropertiesEXT profiles_layer_tool_props = {};
    profiles_layer_tool_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TOOL_PROPERTIES_EXT;
    profiles_layer_tool_props.pNext = nullptr;
    strcpy(profiles_layer_tool_props.name, kLayerProperties[0].description);
    strcpy(profiles_layer_tool_props.version, version_string.c_str());
    profiles_layer_tool_props.purposes = VK_TOOL_PURPOSE_MODIFYING_FEATURES_BIT_EXT;
    strcpy(profiles_layer_tool_props.description, kLayerProperties[0].description);
    strcpy(profiles_layer_tool_props.layer, kLayerProperties[0].layerName);

    auto original_pToolProperties = pToolProperties;
    if (pToolProperties != nullptr) {
        *pToolProperties = profiles_layer_tool_props;
        pToolProperties = ((*pToolCount > 1) ? &pToolProperties[1] : nullptr);
        (*pToolCount)--;
    }

    VkuInstanceDispatchTable *pInstanceTable = instance_dispatch_table(physicalDevice);
    VkResult result = pInstanceTable->GetPhysicalDeviceToolPropertiesEXT(physicalDevice, pToolCount, pToolProperties);

    if (original_pToolProperties != nullptr) {
        pToolProperties = original_pToolProperties;
    }

    (*pToolCount)++;

    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceToolProperties(VkPhysicalDevice physicalDevice, uint32_t *pToolCount,
                                                               VkPhysicalDeviceToolProperties *pToolProperties) {
    return GetPhysicalDeviceToolPropertiesEXT(physicalDevice, pToolCount, pToolProperties);
}
'''

TRANSFER_DEFINES_ARRAY = '''
#define TRANSFER_VALUE_ARRAY(name)    \\
    if (promoted_written) {     \\
        std::memmove(src->name, dest->name, sizeof(src->name)); \\
    } else {                    \\
        std::memmove(dest->name, src->name, sizeof(dest->name)); \\
    }
'''

TRANSFER_DEFINES = '''
#define TRANSFER_VALUE(name)    \\
    if (promoted_written) {     \\
        std::memmove(&src->name, &dest->name, sizeof(src->name)); \\
    } else {                    \\
        std::memmove(&dest->name, &src->name, sizeof(dest->name)); \\
    }
'''

TRANSFER_UNDEFINE = '''
#undef TRANSFER_VALUE_ARRAY
#undef TRANSFER_VALUE
'''

LOAD_QUEUE_FAMILY_PROPERTIES = '''
void LoadQueueFamilyProperties(VkInstance instance, VkPhysicalDevice pd, PhysicalDeviceData *pdd) {
    const auto dt = instance_dispatch_table(instance);
    uint32_t count = 0;
    if (pdd->GetEffectiveVersion() >= VK_API_VERSION_1_1) {
        dt->GetPhysicalDeviceQueueFamilyProperties2(pd, &count, nullptr);
    } else {
        dt->GetPhysicalDeviceQueueFamilyProperties2KHR(pd, &count, nullptr);
    }
    if (count > 0) {
        pdd->device_queue_family_properties_.resize(count);
        std::vector<void *> pNext(count);
        std::vector<VkQueueFamilyProperties2> props(count);
        for (uint32_t i = 0; i < count; ++i) {
            if (PhysicalDeviceData::HasExtension(pdd, VK_KHR_GLOBAL_PRIORITY_EXTENSION_NAME)) {
                pdd->device_queue_family_properties_[i].global_priority_properties_.pNext = pNext[i];

                pNext[i] = &pdd->device_queue_family_properties_[i].global_priority_properties_;
            }
            if (PhysicalDeviceData::HasExtension(pdd, VK_KHR_VIDEO_QUEUE_EXTENSION_NAME)) {
                pdd->device_queue_family_properties_[i].video_properties_.pNext = pNext[i];

                pNext[i] = &pdd->device_queue_family_properties_[i].video_properties_;

                pdd->device_queue_family_properties_[i].query_result_status_properties_.pNext = pNext[i];

                pNext[i] = &pdd->device_queue_family_properties_[i].query_result_status_properties_;
            }
            if (PhysicalDeviceData::HasExtension(pdd, VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME)) {
                pdd->device_queue_family_properties_[i].checkpoint_properties_.pNext = pNext[i];

                pNext[i] = &pdd->device_queue_family_properties_[i].checkpoint_properties_;

                if (PhysicalDeviceData::HasExtension(pdd, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME)) {
                    pdd->device_queue_family_properties_[i].checkpoint_properties_2_.pNext = pNext[i];

                    pNext[i] = &pdd->device_queue_family_properties_[i].checkpoint_properties_2_;
                }
            }
            pdd->device_queue_family_properties_[i].properties_2.pNext = pNext[i];
            props[i] = pdd->device_queue_family_properties_[i].properties_2;
        }
        if (pdd->GetEffectiveVersion() >= VK_API_VERSION_1_1) {
            dt->GetPhysicalDeviceQueueFamilyProperties2(pd, &count, props.data());
        } else {
            dt->GetPhysicalDeviceQueueFamilyProperties2KHR(pd, &count, props.data());
        }
        for (uint32_t i = 0; i < count; ++i) {
            pdd->device_queue_family_properties_[i].properties_2 = props[i];
        }
    }
}
'''

ENUMERATE_PHYSICAL_DEVICES_BEGIN = '''
VKAPI_ATTR VkResult VKAPI_CALL EnumeratePhysicalDevices(VkInstance instance, uint32_t *pPhysicalDeviceCount,
                                                        VkPhysicalDevice *pPhysicalDevices) {
    // Our layer-specific initialization...

    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(instance);

    ProfileLayerSettings *layer_settings = &JsonLoader::Find(instance)->layer_settings;

    VkResult result = VK_SUCCESS;
    result = dt->EnumeratePhysicalDevices(instance, pPhysicalDeviceCount, pPhysicalDevices);

    // HACK!! epd_count is used to ensure the following code only gets called _after_ vkCreateInstance finishes *in the "vkcube +
    // profiles" use case*
    if (pPhysicalDevices && (VK_SUCCESS == result)) {
        std::vector<VkPhysicalDevice> physical_devices;
        result = EnumerateAll<VkPhysicalDevice>(physical_devices, [&](uint32_t *count, VkPhysicalDevice *results) {
            return dt->EnumeratePhysicalDevices(instance, count, results);
        });

        if (result != VK_SUCCESS) {
            return result;
        }

        if (layer_settings->device.force_device != FORCE_DEVICE_OFF && *pPhysicalDeviceCount == 1) {
            LogMessage(layer_settings, DEBUG_REPORT_NOTIFICATION_BIT, "Forced physical device is disabled because a single physical device was found.\\n");
            layer_settings->device.force_device = FORCE_DEVICE_OFF;
        }

        switch (layer_settings->device.force_device) {
            default:
            case FORCE_DEVICE_OFF: {
                break;
            }
            case FORCE_DEVICE_WITH_UUID: {
                bool found = false;
                for (std::size_t i = 0, n = physical_devices.size(); i < n; ++i) {
                    VkPhysicalDeviceIDPropertiesKHR properties_deviceid{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES_KHR};
                    VkPhysicalDeviceProperties2 properties2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, &properties_deviceid};

                    dt->GetPhysicalDeviceProperties2(physical_devices[i], &properties2);

                    if (layer_settings->device.force_device_uuid == GetUUIDString(properties_deviceid.deviceUUID)) {
                        layer_settings->device.force_device_name = properties2.properties.deviceName;
                        *pPhysicalDevices = physical_devices[i];
                        found = true;
                        break;
                    }
                }

                static bool force_physical_device_log_once = false;
                if (found) {
                    *pPhysicalDeviceCount = 1;

                    std::vector<VkPhysicalDevice> physical_devices_tmp;
                    physical_devices_tmp.push_back(*pPhysicalDevices);
                    std::swap(physical_devices, physical_devices_tmp);

                    if (!force_physical_device_log_once) {
                        LogMessage(layer_settings, DEBUG_REPORT_NOTIFICATION_BIT,
                            "Force physical device by device UUID: '%s'('%s').\\n",
                            layer_settings->device.force_device_uuid.c_str(),
                            layer_settings->device.force_device_name.c_str());
                    }
                } else {
                    if (!force_physical_device_log_once) {
                        LogMessage(layer_settings, DEBUG_REPORT_ERROR_BIT,
                            "Force physical device by device UUID is active but the requested physical device '%s'('%s') couldn't be found.\\n",
                            layer_settings->device.force_device_uuid.c_str(),
                            layer_settings->device.force_device_name.c_str());
                    }
                }
                force_physical_device_log_once = true;
                break;
            }
            case FORCE_DEVICE_WITH_NAME: {
                bool found = false;
                for (std::size_t i = 0, n = physical_devices.size(); i < n; ++i) {
                    VkPhysicalDeviceProperties physical_device_properties;
                    dt->GetPhysicalDeviceProperties(physical_devices[i], &physical_device_properties);

                    if (layer_settings->device.force_device_name == physical_device_properties.deviceName) {
                        *pPhysicalDevices = physical_devices[i];
                        found = true;
                        break;
                    }
                }

                static bool force_physical_device_log_once = false;
                if (found) {
                    *pPhysicalDeviceCount = 1;

                    std::vector<VkPhysicalDevice> physical_devices_tmp;
                    physical_devices_tmp.push_back(*pPhysicalDevices);
                    std::swap(physical_devices, physical_devices_tmp);

                    if (!force_physical_device_log_once) {
                        LogMessage(layer_settings, DEBUG_REPORT_NOTIFICATION_BIT,
                            "Force physical device by device name: '%s'.\\n",
                            layer_settings->device.force_device_name.c_str());
                    }
                }
                else {
                    if (!force_physical_device_log_once) {
                        LogMessage(layer_settings, DEBUG_REPORT_ERROR_BIT,
                            "Force physical device by device name is active but the requested physical device '%s' couldn't be found.\\n",
                            layer_settings->device.force_device_name.c_str());
                    }
                }
                force_physical_device_log_once = true;
                break;
            }
        }

        // For each physical device, create and populate a PDD instance.
        for (const auto &physical_device : physical_devices) {
            if (PhysicalDeviceData::Find(physical_device)) {
                continue;
            }

            PhysicalDeviceData &pdd = PhysicalDeviceData::Create(physical_device, instance);
            ArrayOfVkExtensionProperties local_device_extensions;
            EnumerateAll<VkExtensionProperties>(local_device_extensions, [&](uint32_t *count, VkExtensionProperties *results) {
                return dt->EnumerateDeviceExtensionProperties(physical_device, nullptr, count, results);
            });

            pdd.device_extensions_.reserve(local_device_extensions.size());
            for(const auto& ext: local_device_extensions) {
                pdd.device_extensions_.insert({&(ext.extensionName[0]), ext});
            }

            pdd.simulation_extensions_ = pdd.device_extensions_;

            dt->GetPhysicalDeviceProperties(physical_device, &pdd.physical_device_properties_);
            uint32_t effective_api_version = pdd.GetEffectiveVersion();
            bool api_version_above_1_1 = effective_api_version >= VK_API_VERSION_1_1;
            bool api_version_above_1_2 = effective_api_version >= VK_API_VERSION_1_2;
            bool api_version_above_1_3 = effective_api_version >= VK_API_VERSION_1_3;

            ::device_has_astc_hdr = ::PhysicalDeviceData::HasExtension(&pdd, VK_EXT_TEXTURE_COMPRESSION_ASTC_HDR_EXTENSION_NAME);
            ::device_has_pvrtc = ::PhysicalDeviceData::HasExtension(&pdd, VK_IMG_FORMAT_PVRTC_EXTENSION_NAME);

            // Initialize PDD members to the actual Vulkan implementation's defaults.
            {
                VkPhysicalDeviceProperties2KHR property_chain = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR};
                VkPhysicalDeviceFeatures2KHR feature_chain = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR};
                VkPhysicalDeviceMemoryProperties2KHR memory_chain = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2_KHR};

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME)) {
                    property_chain.pNext = &(pdd.physical_device_portability_subset_properties_);
                    feature_chain.pNext = &(pdd.physical_device_portability_subset_features_);
                } else if (layer_settings->simulate.emulate_portability) {
                    pdd.physical_device_portability_subset_properties_ = {
                        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES_KHR, nullptr, layer_settings->portability.minVertexInputBindingStrideAlignment};
                    pdd.physical_device_portability_subset_features_ = {
                        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR,
                        nullptr,
                        layer_settings->portability.constantAlphaColorBlendFactors,
                        layer_settings->portability.events,
                        layer_settings->portability.imageViewFormatReinterpretation,
                        layer_settings->portability.imageViewFormatSwizzle,
                        layer_settings->portability.imageView2DOn3DImage,
                        layer_settings->portability.multisampleArrayImage,
                        layer_settings->portability.mutableComparisonSamplers,
                        layer_settings->portability.pointPolygons,
                        layer_settings->portability.samplerMipLodBias,
                        layer_settings->portability.separateStencilMaskRef,
                        layer_settings->portability.shaderSampleRateInterpolationFunctions,
                        layer_settings->portability.tessellationIsolines,
                        layer_settings->portability.tessellationPointMode,
                        layer_settings->portability.triangleFans,
                        layer_settings->portability.vertexAttributeAccessBeyondStride};
                }
'''

ENUMERATE_PHYSICAL_DEVICES_MIDDLE = '''
                if (pdd.GetEffectiveVersion() >= VK_API_VERSION_1_1) {
                    dt->GetPhysicalDeviceProperties2(physical_device, &property_chain);
                    if (layer_settings->simulate.default_feature_values == DEFAULT_FEATURE_VALUES_DEVICE) {
                        dt->GetPhysicalDeviceFeatures2(physical_device, &feature_chain);
                    }
                    dt->GetPhysicalDeviceMemoryProperties2(physical_device, &memory_chain);
                } else {
                    dt->GetPhysicalDeviceProperties2(physical_device, &property_chain);
                    if (layer_settings->simulate.default_feature_values == DEFAULT_FEATURE_VALUES_DEVICE) {
                        dt->GetPhysicalDeviceFeatures2(physical_device, &feature_chain);
                    }
                    dt->GetPhysicalDeviceMemoryProperties2(physical_device, &memory_chain);
                }

                pdd.physical_device_properties_ = property_chain.properties;
                pdd.physical_device_features_ = feature_chain.features;
                pdd.physical_device_memory_properties_ = memory_chain.memoryProperties;
            }

            ::device_has_astc = pdd.physical_device_features_.textureCompressionASTC_LDR == VK_TRUE;
            ::device_has_bc = pdd.physical_device_features_.textureCompressionBC == VK_TRUE;
            ::device_has_etc2 = pdd.physical_device_features_.textureCompressionETC2 == VK_TRUE;

            if (layer_settings->simulate.capabilities & SIMULATE_FORMATS_BIT) {
                LoadDeviceFormats(instance, &pdd, physical_device, &pdd.device_formats_, &pdd.device_formats_3_);
            }
            if (layer_settings->simulate.capabilities & SIMULATE_QUEUE_FAMILY_PROPERTIES_BIT) {
                LoadQueueFamilyProperties(instance, physical_device, &pdd);
            }

            LogMessage(layer_settings, DEBUG_REPORT_NOTIFICATION_BIT,
                       "Found \\"%s\\" with Vulkan %d.%d.%d driver.\\n", pdd.physical_device_properties_.deviceName,
                              VK_API_VERSION_MAJOR(pdd.physical_device_properties_.apiVersion),
                              VK_API_VERSION_MINOR(pdd.physical_device_properties_.apiVersion),
                              VK_API_VERSION_PATCH(pdd.physical_device_properties_.apiVersion));

            // Override PDD members with values from configuration file(s).
            if (result == VK_SUCCESS) {
                JsonLoader &json_loader = *JsonLoader::Find(instance);
                result = json_loader.LoadDevice(pdd.physical_device_properties_.deviceName, &pdd);
            }
'''

ENUMERATE_PHYSICAL_DEVICES_END = '''
            if (layer_settings->simulate.capabilities & SIMULATE_EXTENSIONS_BIT) {
                pdd.simulation_extensions_ = pdd.map_of_extension_properties_;
            } else {
                pdd.simulation_extensions_ = pdd.device_extensions_;
            }

            for (std::size_t j = 0, m = layer_settings->simulate.exclude_device_extensions.size(); j < m; ++j) {
                pdd.simulation_extensions_.erase(layer_settings->simulate.exclude_device_extensions[j].c_str());
            }
        }
    }

    LogFlush(layer_settings);

    return result;
}
'''

GET_INSTANCE_PROC_ADDR = '''
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetInstanceProcAddr(VkInstance instance, const char *pName) {
// Apply the DRY principle, see https://en.wikipedia.org/wiki/Don%27t_repeat_yourself
#define GET_PROC_ADDR(func) \\
    if (strcmp("vk" #func, pName) == 0) return reinterpret_cast<PFN_vkVoidFunction>(func);
    GET_PROC_ADDR(GetInstanceProcAddr);
    GET_PROC_ADDR(CreateInstance);
    GET_PROC_ADDR(EnumerateInstanceLayerProperties);
    GET_PROC_ADDR(EnumerateInstanceExtensionProperties);
    GET_PROC_ADDR(EnumerateDeviceExtensionProperties);
    GET_PROC_ADDR(EnumeratePhysicalDevices);
    GET_PROC_ADDR(DestroyInstance);
    GET_PROC_ADDR(GetPhysicalDeviceProperties);
    GET_PROC_ADDR(GetPhysicalDeviceProperties2);
    GET_PROC_ADDR(GetPhysicalDeviceProperties2KHR);
    GET_PROC_ADDR(GetPhysicalDeviceFeatures);
    GET_PROC_ADDR(GetPhysicalDeviceFeatures2);
    GET_PROC_ADDR(GetPhysicalDeviceFeatures2KHR);
    GET_PROC_ADDR(GetPhysicalDeviceFormatProperties);
    GET_PROC_ADDR(GetPhysicalDeviceFormatProperties2);
    GET_PROC_ADDR(GetPhysicalDeviceFormatProperties2KHR);
    GET_PROC_ADDR(GetPhysicalDeviceImageFormatProperties);
    GET_PROC_ADDR(GetPhysicalDeviceImageFormatProperties2);
    GET_PROC_ADDR(GetPhysicalDeviceImageFormatProperties2KHR);
    GET_PROC_ADDR(GetPhysicalDeviceToolProperties);
    GET_PROC_ADDR(GetPhysicalDeviceToolPropertiesEXT);
    GET_PROC_ADDR(GetPhysicalDeviceQueueFamilyProperties);
    GET_PROC_ADDR(GetPhysicalDeviceQueueFamilyProperties2);
    GET_PROC_ADDR(GetPhysicalDeviceQueueFamilyProperties2KHR);
#undef GET_PROC_ADDR

    if (!instance) {
        return nullptr;
    }

    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(instance);

    if (!dt->GetInstanceProcAddr) {
        return nullptr;
    }
    return dt->GetInstanceProcAddr(instance, pName);
}
'''

GET_VALUE_PHYSICAL_DEVICE_PROPERTIES = '''
bool JsonLoader::GetStruct(const char* device_name, bool requested_profile, const Json::Value &parent, VkPhysicalDeviceProperties *dest) {
    LogMessage(&layer_settings, DEBUG_REPORT_DEBUG_BIT, \"\\tJsonLoader::GetStruct(VkPhysicalDeviceProperties)\\n");
    bool valid = true;
    if (!GetStruct(device_name, requested_profile, parent["limits"], &dest->limits)) {
        valid = false;
    }
    if (!GetStruct(device_name, requested_profile, parent["sparseProperties"], &dest->sparseProperties)) {
        valid = false;
    }
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE(prop, apiVersion, false, requested_profile);
        GET_VALUE(prop, driverVersion, true, requested_profile);
        GET_VALUE(prop, vendorID, true, requested_profile);
        GET_VALUE(prop, deviceID, true, requested_profile);
        GET_VALUE_ENUM_WARN(prop, deviceType, true, requested_profile, WarnIfNotEqualEnum);
        GetArray(device_name, parent, prop, "deviceName", dest->deviceName, true);         // size < VK_MAX_PHYSICAL_DEVICE_NAME_SIZE
        GetArray(device_name, parent, prop, "pipelineCacheUUID", dest->pipelineCacheUUID, true);  // size == VK_UUID_SIZE*/
    }
    return valid;
}
'''

GET_VALUE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES = '''
bool JsonLoader::GetStruct(const char* device_name, bool requested_profile, const Json::Value &parent, VkPhysicalDevicePortabilitySubsetPropertiesKHR *dest) {
    LogMessage(&layer_settings, DEBUG_REPORT_DEBUG_BIT, \"\\tJsonLoader::GetStruct(VkPhysicalDevicePortabilitySubsetPropertiesKHR)\\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, minVertexInputBindingStrideAlignment, false, requested_profile, WarnIfLesser);
    }
    return valid;
}
'''

GET_VALUE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES = '''
bool JsonLoader::GetStruct(const char* device_name, bool requested_profile, const Json::Value &parent, VkPhysicalDevicePortabilitySubsetFeaturesKHR *dest) {
    LogMessage(&layer_settings, DEBUG_REPORT_DEBUG_BIT, \"\\tJsonLoader::GetStruct(VkPhysicalDevicePortabilitySubsetFeaturesKHR)\\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        if (layer_settings.simulate.emulate_portability) {
            dest->constantAlphaColorBlendFactors = layer_settings.portability.constantAlphaColorBlendFactors;
            dest->events = layer_settings.portability.events;
            dest->imageViewFormatReinterpretation = layer_settings.portability.imageViewFormatReinterpretation;
            dest->imageViewFormatSwizzle = layer_settings.portability.imageViewFormatSwizzle;
            dest->imageView2DOn3DImage = layer_settings.portability.imageView2DOn3DImage;
            dest->multisampleArrayImage = layer_settings.portability.multisampleArrayImage;
            dest->mutableComparisonSamplers = layer_settings.portability.mutableComparisonSamplers;
            dest->pointPolygons = layer_settings.portability.pointPolygons;
            dest->samplerMipLodBias = layer_settings.portability.samplerMipLodBias;
            dest->separateStencilMaskRef = layer_settings.portability.separateStencilMaskRef;
            dest->shaderSampleRateInterpolationFunctions = layer_settings.portability.shaderSampleRateInterpolationFunctions;
            dest->tessellationIsolines = layer_settings.portability.tessellationIsolines;
            dest->tessellationPointMode = layer_settings.portability.tessellationPointMode;
            dest->triangleFans = layer_settings.portability.triangleFans;
            dest->vertexAttributeAccessBeyondStride = layer_settings.portability.vertexAttributeAccessBeyondStride;
        } else {
            GET_VALUE_WARN(member, constantAlphaColorBlendFactors, false, requested_profile, WarnIfNotEqualBool);
            GET_VALUE_WARN(member, events, false, requested_profile, WarnIfNotEqualBool);
            GET_VALUE_WARN(member, imageViewFormatReinterpretation, false, requested_profile, WarnIfNotEqualBool);
            GET_VALUE_WARN(member, imageViewFormatSwizzle, false, requested_profile, WarnIfNotEqualBool);
            GET_VALUE_WARN(member, imageView2DOn3DImage, false, requested_profile, WarnIfNotEqualBool);
            GET_VALUE_WARN(member, multisampleArrayImage, false, requested_profile, WarnIfNotEqualBool);
            GET_VALUE_WARN(member, mutableComparisonSamplers, false, requested_profile, WarnIfNotEqualBool);
            GET_VALUE_WARN(member, pointPolygons, false, requested_profile, WarnIfNotEqualBool);
            GET_VALUE_WARN(member, samplerMipLodBias, false, requested_profile, WarnIfNotEqualBool);
            GET_VALUE_WARN(member, separateStencilMaskRef, false, requested_profile, WarnIfNotEqualBool);
            GET_VALUE_WARN(member, shaderSampleRateInterpolationFunctions, false, requested_profile, WarnIfNotEqualBool);
            GET_VALUE_WARN(member, tessellationIsolines, false, requested_profile, WarnIfNotEqualBool);
            GET_VALUE_WARN(member, tessellationPointMode, false, requested_profile, WarnIfNotEqualBool);
            GET_VALUE_WARN(member, triangleFans, false, requested_profile, WarnIfNotEqualBool);
            GET_VALUE_WARN(member, vertexAttributeAccessBeyondStride, false, requested_profile, WarnIfNotEqualBool);
        }
    }
    return valid;
}
'''

class VulkanProfilesLayerGenerator():
    emulated_extensions = ['VK_KHR_portability_subset']
    additional_features = ['VkPhysicalDeviceFeatures', 'VkPhysicalDevicePortabilitySubsetFeaturesKHR']
    additional_properties = ['VkPhysicalDeviceProperties', 'VkPhysicalDeviceLimits', 'VkPhysicalDeviceSparseProperties', 'VkPhysicalDeviceToolProperties', 'VkPhysicalDevicePortabilitySubsetPropertiesKHR']
    # VkPhysicalDeviceHostImageCopyFeaturesEXT is not ignored to allow the people using the MockICD to still have the feature enabled,
    # but use the properties in the MockICD until HostImageCopyPropertiesEXT is fixed.
    ignored_structs = ['VkPhysicalDeviceHostImageCopyPropertiesEXT', 'VkPhysicalDeviceLayeredApiPropertiesListKHR']

    def generate(self, path, registry):
        self.registry = registry
        self.get_pdd_structs()
        with open(path, 'w') as f:
            f.write(COPYRIGHT_HEADER)
            f.write(DESCRIPTION_HEADER)
            f.write(INCLUDES_HEADER)
            f.write(self.generate_helpers())
            f.write(GLOBAL_CONSTANTS)
            f.write(GLOBAL_VARS)
            f.write(GET_DEFINES)
            f.write(self.generate_is_instance_extension())
            f.write(self.generate_physical_device_data())
            f.write(self.generate_json_loader())
            f.write(self.generate_is_format_functions())
            f.write(self.generate_warn_duplicated())
            f.write(self.generate_get_feature())
            f.write(self.generate_get_property())
            f.write(JSON_LOADER_NON_GENERATED)
            f.write(self.generate_get_queue_family_properties())
            f.write(QUEUE_FAMILY_FUNCTIONS)
            f.write(self.generate_add_promoted_extensions())
            f.write(READ_PROFILE)
            f.write(self.generate_json_get_value())
            f.write(GET_UNDEFINE)
            f.write(INSTANCE_FUNCTIONS)
            f.write(self.generate_fill_physical_device_pnext_chain())
            f.write(self.generate_fill_queue_family_properties_pnext_chain())
            f.write(FORMAT_PROPERTIES_PNEXT)
            f.write(GET_PHYSICAL_DEVICE_FEATURES_PROPERTIES_FUNCTIONS)
            f.write(ENUMERATE_FUNCTIONS)
            f.write(QUEUE_FAMILY_PROPERTIES_FUNCTIONS)
            f.write(PHYSICAL_DEVICE_FORMAT_FUNCTIONS)
            f.write(TOOL_PROPERTIES_FUNCTIONS)
            f.write(TRANSFER_DEFINES)
            f.write(TRANSFER_DEFINES_ARRAY)
            f.write(self.generate_transfer_values())
            f.write(TRANSFER_UNDEFINE)
            f.write(self.generate_load_device_formats())
            f.write(LOAD_QUEUE_FAMILY_PROPERTIES)
            f.write(self.generate_enumerate_physical_device())
            f.write(GET_INSTANCE_PROC_ADDR)

    def struct_or_extension_platform(self, struct_or_ext_name):
        if struct_or_ext_name is None:
            return None
        if struct_or_ext_name in self.registry.structs:
            if self.registry.structs[struct_or_ext_name].definedByVersion:
                # Structure defined by a core version, not platform-specific
                return None
            else:
                # Structure defined by an extension, use extension name to continue the search
                struct_or_ext_name = self.registry.structs[struct_or_ext_name].definedByExtensions[0]
        return self.registry.extensions[struct_or_ext_name].platform

    def generate_platform_protect_begin(self, struct_or_ext_name):
        platform = self.struct_or_extension_platform(struct_or_ext_name)
        if platform:
            return '#ifdef ' + self.registry.platforms[platform].protect + '\n'
        else:
            return ''

    def generate_platform_protect_end(self, struct_or_ext_name):
        platform = self.struct_or_extension_platform(struct_or_ext_name)
        if platform:
            return '#endif // ' + self.registry.platforms[platform].protect + '\n'
        else:
            return ''

    def generate_helpers(self):
        gen = self.generate_string_to_enum('SimulateCapabilityFlags', ('SIMULATE_API_VERSION_BIT', 'SIMULATE_FEATURES_BIT', 'SIMULATE_PROPERTIES_BIT', 'SIMULATE_EXTENSIONS_BIT', 'SIMULATE_FORMATS_BIT', 'SIMULATE_QUEUE_FAMILY_PROPERTIES_BIT', 'SIMULATE_MAX_ENUM'))
        gen += self.generate_enum_to_string('SimulateCapabilityFlags', ('SIMULATE_API_VERSION_BIT', 'SIMULATE_FEATURES_BIT', 'SIMULATE_PROPERTIES_BIT', 'SIMULATE_EXTENSIONS_BIT', 'SIMULATE_FORMATS_BIT', 'SIMULATE_QUEUE_FAMILY_PROPERTIES_BIT'), 'GetSimulateCapabilitiesLog')
        gen += self.generate_enum_to_string('DebugActionFlags', ('DEBUG_REPORT_NOTIFICATION_BIT', 'DEBUG_REPORT_WARNING_BIT', 'DEBUG_REPORT_ERROR_BIT', 'DEBUG_REPORT_DEBUG_BIT'), 'GetDebugReportsLog')

        gen += self.generate_enum_to_string('VkFormatFeatureFlags', self.get_non_aliased_list(registry.bitmasks["VkFormatFeatureFlags"].bitsType.values, registry.bitmasks["VkFormatFeatureFlags"].bitsType.aliasValues), 'GetFormatFeatureString')
        gen += self.generate_enum_to_string('VkFormatFeatureFlags2', self.get_non_aliased_list(registry.bitmasks["VkFormatFeatureFlags2"].bitsType.values, registry.bitmasks["VkFormatFeatureFlags2"].bitsType.aliasValues), 'GetFormatFeature2String')
        gen += self.generate_enum_to_string('VkQueueFlags', registry.bitmasks["VkQueueFlags"].bitsType.values, 'GetQueueFlagsToString')

        gen += self.generate_format_to_string(registry.enums['VkFormat'].values, registry.enums['VkFormat'].aliasValues)
        gen += self.generate_string_to_format(registry.enums['VkFormat'].values)

        gen += self.generate_string_to_image_layout(registry.enums['VkImageLayout'].values)

        gen += self.generate_string_to_uint(('VkToolPurposeFlagBits', 'VkSampleCountFlagBits', 'VkResolveModeFlagBits', 'VkShaderStageFlagBits', 'VkSubgroupFeatureFlagBits', 'VkShaderFloatControlsIndependence', 'VkPointClippingBehavior', 'VkOpticalFlowGridSizeFlagBitsNV', 'VkQueueFlagBits', 'VkMemoryDecompressionMethodFlagBitsNV', 'VkLayeredDriverUnderlyingApiMSFT', 'VkImageUsageFlagBits', 'VkBufferUsageFlagBits', 'VkPhysicalDeviceSchedulingControlsFlagBitsARM'), registry.enums)

        gen += self.generate_string_to_flag_functions(('VkFormatFeatureFlags', 'VkQueueFlags', 'VkQueueGlobalPriorityKHR', 'VkVideoCodecOperationFlagsKHR', 'VkPipelineStageFlags', 'VkPipelineStageFlags2', 'VkFormatFeatureFlags2'))

        return gen

    def generate_physical_device_data(self):
        gen = PHYSICAL_DEVICE_DATA_BEGIN
        gen += '\n    // Core properties\n'
        for property in self.non_extension_properties:
            typename = self.registry.getNonAliasTypeName(property, self.registry.structs)
            gen += '    ' + typename + ' ' + self.create_var_name(property) + ';\n'

        gen += '\n    // Core features\n'
        for feature in self.non_extension_features:
            typename = self.registry.getNonAliasTypeName(feature, self.registry.structs)
            gen += '    ' + typename + ' ' + self.create_var_name(feature) + ';\n'

        for ext, properties, features in self.extension_structs:
            gen += '\n    // ' + ext + ' structs\n'
            gen += self.generate_platform_protect_begin(ext)
            for property in properties:
                gen += '    ' + property + ' ' + self.create_var_name(property) + ';\n'
            for feature in features:
                gen += '    ' + feature + ' ' + self.create_var_name(feature) + ';\n'
            gen += self.generate_platform_protect_end(ext)

        gen += PHYSICAL_DEVICE_DATA_CONSTRUCTOR_BEGIN

        gen += '\n        // Core properties\n'
        for property in self.non_extension_properties:
            stype = self.registry.structs[self.registry.getNonAliasTypeName(property, self.registry.structs)].sType
            gen += '        ' + self.create_var_name(property) + ' = { ' + stype +  ' };\n'
        gen += '\n        // Core features\n'
        for feature in self.non_extension_features:
            stype = self.registry.structs[self.registry.getNonAliasTypeName(feature, self.registry.structs)].sType
            gen += '        ' + self.create_var_name(feature) + ' = { ' + stype +  ' };\n'
        for ext, properties, features in self.extension_structs:
            gen += '\n        // ' + ext + ' structs\n'
            gen += self.generate_platform_protect_begin(ext)
            for property in properties:
                gen += '        ' + self.create_var_name(property) + ' = {' + registry.structs[property].sType +  '};\n'
            for feature in features:
                gen += '        ' + self.create_var_name(feature) + ' = {' + registry.structs[feature].sType +  '};\n'
            gen += self.generate_platform_protect_end(ext)

        gen += PHYSICAL_DEVICE_DATA_END

        return gen

    def generate_is_instance_extension(self):
        gen = 'static bool IsInstanceExtension(const char* name) {\n'

        gen += '\t const char* table[] = {\n'

        first = True
        for extension in registry.extensions.values():
            if (extension.type == 'instance'):
                if not first:
                    gen += ',\n'
                first = False

                gen += '          "' + extension.name + '"'

        gen += '\n     };\n\n'

        gen += '     bool result = false;\n'
        gen += '     for (std::size_t i = 0, n = std::size(table); i < n; ++i) {\n'
        gen += '           if (strcmp(table[i], name) == 0) {\n'
        gen += '               result = true;\n'
        gen += '               break;\n'
        gen += '           }\n'
        gen += '     }\n\n'
        gen += '     return result;\n'
        gen += '}\n'

        return gen

    def generate_is_format_functions(self):
        # Find all unique compressed formats in registry
        compressed_formats = set()
        for format, compressed_format in registry.formatCompression.items():
            compressed_formats.add(compressed_format)
        compressed_formats = sorted(compressed_formats)

        # Generate an is function for each compressed format
        gen = ''
        for compressed_format in compressed_formats:
            gen += '\nstatic bool Is' + compressed_format.replace(' ', '') + 'Format(VkFormat format) {\n'
            gen += '    switch (format) {\n'
            for format in registry.formatCompression:
                if registry.formatCompression[format] == compressed_format:
                    gen += '        case ' + format + ':\n'
            gen += '            return true;\n'
            gen += '        default:\n'
            gen += '            return false;\n'
            gen += '    }\n'
            gen += '}\n'
        return gen

    def generate_warn_duplicated(self):
        gen = '\nbool JsonLoader::WarnDuplicatedFeature(const Json::Value &parent) {\n'
        gen += '    bool valid = true;\n'
        gen += self.generate_duplicated_checks('VkPhysicalDeviceFeatures2')
        gen += '    return valid;\n'
        gen += '}\n'

        gen += '\nbool JsonLoader::WarnDuplicatedProperty(const Json::Value &parent) {\n'
        gen += '    bool valid = true;\n'
        gen += self.generate_duplicated_checks('VkPhysicalDeviceProperties2')
        gen += '    return valid;\n'
        gen += '}\n'

        return gen

    def generate_duplicated_checks(self, extends):
        gen = ''
        for name, value in registry.structs.items():
            if extends in value.extends and value.isAlias == False:
                aliases = value.aliases.copy()
                aliases.remove(name)
                promoted = self.find_promoted_struct(value)
                if promoted:
                    aliases.append(promoted)
                if aliases:
                    gen += '    valid &= WarnDuplicated(&layer_settings, parent, {\"' + name + '\"'
                    for alias in aliases:
                        gen += ', \"' + alias + '\"'
                    gen += '});\n'
        return gen

    def generate_get_feature(self):
        gen = '\nbool JsonLoader::GetFeature(const char *device_name, bool requested_profile, const Json::Value &features, const std::string &name) {\n'
        gen += '    const Json::Value &feature = features[name];\n\n'
        gen += self.generate_get_struct('feature', 'VkPhysicalDeviceFeatures2', self.additional_features)
        gen += '\n\n    return true;\n'
        gen += '}\n'
        return gen

    def generate_get_property(self):
        gen = '\nbool JsonLoader::GetProperty(const char *device_name, bool requested_profile, const Json::Value &props, const std::string &name) {\n'
        gen += '    const Json::Value &property = props[name];\n\n'
        gen += self.generate_get_struct('property', 'VkPhysicalDeviceProperties2', self.additional_properties)
        gen += '\n\n    return true;\n'
        gen += '}\n'
        return gen

    def generate_get_struct(self, struct, extends, additional):
        gen = ''
        first = True
        count = 0
        for name, value  in registry.structs.items():
            if name in self.ignored_structs:
                continue
            if (extends in value.extends and value.isAlias == False) or (name in additional):
                aliases = value.aliases.copy()
                count += 1
                if count == 75:
                    count = 0
                    first = True
                    gen += ' // Blocks nested too deeply, break\n'
                while (aliases):
                    current = aliases.pop()
                    if first:
                        first = False
                        gen += '    '
                    else:
                        gen += ' else '
                    gen += 'if (name == \"' + current + '\"'
                    copy_aliases = aliases.copy()
                    for alias in copy_aliases:
                        same_version = registry.structs[current].definedByVersion and registry.structs[alias].definedByVersion
                        same_extension = registry.structs[current].definedByExtensions and registry.structs[current].definedByExtensions == registry.structs[alias].definedByExtensions
                        if same_version or same_extension:
                            gen += ' || name == \"' + alias + '\"'
                            aliases.remove(alias)

                    gen += ') {\n'

                    version = registry.structs[current].definedByVersion
                    if version:
                        if version and (version.major != 1 or version.minor != 0):
                            gen += '        if (!CheckVersionSupport(' + registry.structs[current].definedByVersion.versionMacro + ', name)) return false;\n'
                    else:
                        ext = registry.extensions[registry.structs[current].definedByExtensions[0]]
                        gen += self.generate_platform_protect_begin(ext.name)
                        if not ext.name in self.emulated_extensions:
                            ext_name = ext.upperCaseName + '_EXTENSION_NAME'
                            gen += '        auto support = CheckExtensionSupport(' + ext_name + ', name);\n'
                            gen += '        if (support != ExtensionSupport::SUPPORTED) return valid(support);\n'
                    # Workarounds
                    if current == 'VkPhysicalDeviceLimits':
                        gen += '        return GetStruct(device_name, requested_profile, ' + struct + ', &pdd_->physical_device_properties_.limits);\n'
                    elif current == 'VkPhysicalDeviceSparseProperties':
                        gen += '        return GetStruct(device_name, requested_profile, ' + struct + ', &pdd_->physical_device_properties_.sparseProperties);\n'
                    else:
                        gen += '        return GetStruct(device_name, requested_profile, ' + struct + ', &pdd_->' + self.create_var_name(current) + ');\n'

                    if self.struct_or_extension_platform(current):
                        gen += '#else\n        return false;\n'
                        gen += self.generate_platform_protect_end(ext.name)

                    gen += '    }'
        return gen

    def generate_get_queue_family_properties(self):
        return ''
        gen = '\nbool JsonLoader::GetQueueFamilyProperties(const char* device_name, const Json::Value &qf_props, QueueFamilyProperties *dest) {\n'
        gen += '    for (const auto &name : qf_props.getMemberNames()) {\n'
        gen += '        const auto &props = qf_props[name];\n'

        structs = ['VkQueueFamilyProperties', 'VkQueueFamilyProperties2']
        for name, value  in registry.structs.items():
            if 'VkQueueFamilyProperties2' in value.extends and not value.isAlias:
                structs.append(name)

        first = True
        for struct in structs:
            if first:
                first = False
                gen += '        '
            else:
                gen += ' else '
            gen += 'if (name == \"' + struct + '\"'
            for name, value in registry.structs.items():
                if struct != name and value.aliases and struct in value.aliases:
                    gen += ' || \"' + name + '\"'
            gen += ') {\n'
            for member_name in registry.structs[struct].members:
                member = registry.structs[struct].members[member_name]
                if (member.limittype == 'bitmask'):
                    gen += '            for (const auto &feature : props[\"' + member_name + '\"]) {\n'
                    gen += '                dest->' + self.create_var_name(struct) + '.' + member_name + ' |= StringTo' + member.type + '(feature.asString())\n'
                    gen += '            }\n'
                elif member.isArray:
                    gen += '            uint32_t i = 0;\n'
                    gen += '            for (const auto &feature : props[\"' + member_name + '\"]) {\n'
                    gen += '                dest->' + self.create_var_name(struct) + '.' + member_name + ' = StringTo' + member.type + '(feature.asString());\n'
                    gen += '            }\n'
                elif member.type == 'VkExtent3D':
                    gen += '            dest->' + self.create_var_name(struct) + '.' + member_name + '.width = props[\"' + member_name + '\"][\"width\"].' + self.get_read_from_type(member.type) + ';\n'
                    gen += '            dest->' + self.create_var_name(struct) + '.' + member_name + '.height = props[\"' + member_name + '\"][\"height\"].' + self.get_read_from_type(member.type) + ';\n'
                    gen += '            dest->' + self.create_var_name(struct) + '.' + member_name + '.depth = props[\"' + member_name + '\"][\"depth\"].' + self.get_read_from_type(member.type) + ';\n'
                else:
                    gen += '            dest->' + self.create_var_name(struct) + '.' + member_name + ' = props[\"' + member_name + '\"].' + self.get_read_from_type(member.type) + ';\n'
            gen += '        }'
        gen += '\n'

        return gen

    def generate_add_promoted_extensions(self):
        gen = '\nvoid JsonLoader::AddPromotedExtensions(uint32_t api_version) {\n'
        gen += '\tconst uint32_t minor = VK_API_VERSION_MINOR(api_version);\n'
        gen += '\tconst uint32_t major = VK_API_VERSION_MAJOR(api_version);\n'
        gen += '\tLogMessage(&layer_settings, DEBUG_REPORT_NOTIFICATION_BIT,\n'
        gen += '\t\"- Adding promoted extensions to core in Vulkan (%" PRIu32 ".%" PRIu32 ").\\n", major, minor);\n\n'

        for i in range(registry.headerVersionNumber.major):
            major = str(i + 1)
            for j in range(registry.headerVersionNumber.minor):
                minor = str(j + 1)
                gen += '    static const std::vector<const char *> promoted_' + major + '_' + minor + ' = {\n'
                for ext in registry.extensions:
                    extension = registry.extensions[ext]
                    if 'VK_VERSION_' + major + '_' + minor in extension.promotedTo:
                        gen += '        ' + extension.upperCaseName + '_EXTENSION_NAME,\n'
                gen += '    };\n'
                gen += '    if (api_version >= VK_API_VERSION_' + major + '_' + minor + ') {\n'
                gen += '        for (const auto& ext : promoted_' + major + '_' + minor + ') {\n'
                gen += '            VkExtensionProperties extension;\n'
                gen += '            strcpy(extension.extensionName, ext);\n'
                gen += '            extension.specVersion = 1;\n'
                gen += '            if (!PhysicalDeviceData::HasSimulatedExtension(pdd_, ext)) {\n'
                gen += '                pdd_->simulation_extensions_.insert({ext, extension});\n'
                gen += '            }\n'
                gen += '            pdd_->map_of_extension_properties_.insert({ext, extension});\n'
                gen += '        }\n'
                gen += '    }\n'
        gen += '}\n'
        return gen

    def generate_json_get_value(self):
        gen = '\n'

        for property in self.non_extension_properties:
            gen += self.generate_get_value_function(property)
        for feature in self.non_extension_features:
            gen += self.generate_get_value_function(feature)
        for ext, properties, features in self.extension_structs:
            for property in properties:
                if property != 'VkPhysicalDevicePortabilitySubsetPropertiesKHR':
                    gen += self.generate_get_value_function(property)
            for feature in features:
                if feature != 'VkPhysicalDevicePortabilitySubsetFeaturesKHR':
                    gen += self.generate_get_value_function(feature)
        for struct in self.additional_features:
            if struct == 'VkPhysicalDevicePortabilitySubsetFeaturesKHR':
                gen += GET_VALUE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES
            else:
                gen += self.generate_get_value_function(struct)
        for struct in self.additional_properties:
            if struct == 'VkPhysicalDeviceProperties':
                gen += GET_VALUE_PHYSICAL_DEVICE_PROPERTIES
            elif struct == 'VkPhysicalDevicePortabilitySubsetPropertiesKHR':
                gen += GET_VALUE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES
            else:
                gen += self.generate_get_value_function(struct)

        return gen

    def generate_fill_physical_device_pnext_chain(self):
        gen = '\nvoid FillPNextChain(PhysicalDeviceData *physicalDeviceData, void *place) {\n'
        gen += '    ProfileLayerSettings *layer_settings = &JsonLoader::Find(physicalDeviceData->instance())->layer_settings;\n'
        gen += '    while (place) {\n'
        gen += '        VkBaseOutStructure *structure = (VkBaseOutStructure *)place;\n\n'
        gen += '        // These switch statements check which struct is in the pNext chain and, if the physical device has the proper extension,\n'
        gen += '        // fill the struct with any override data provided by the PhysicalDeviceData object.\n\n'
        gen += '        switch (structure->sType) {\n'
        gen += '            // VK_KHR_portability_subset is a special case since it can also be emulated by the Profiles layer.\n'
        gen += '            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES_KHR:\n'
        gen += '                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME) ||\n'
        gen += '                    layer_settings->simulate.emulate_portability) {\n'
        gen += '                    VkPhysicalDevicePortabilitySubsetPropertiesKHR *psp = (VkPhysicalDevicePortabilitySubsetPropertiesKHR *)place;\n'
        gen += '                    void *pNext = psp->pNext;\n'
        gen += '                    *psp = physicalDeviceData->physical_device_portability_subset_properties_;\n'
        gen += '                    if (layer_settings->portability.vertexAttributeAccessBeyondStride) {\n'
        gen += '                        psp->minVertexInputBindingStrideAlignment = layer_settings->portability.minVertexInputBindingStrideAlignment;\n'
        gen += '                    }\n'
        gen += '                    psp->pNext = pNext;\n'
        gen += '                }\n'
        gen += '                break;\n'

        for property in self.non_extension_properties:
            gen += self.generate_fill_case(property)
        for feature in self.non_extension_features:
            gen += self.generate_fill_case(feature)
        for ext, properties, features in self.extension_structs:
            gen += self.generate_platform_protect_begin(ext)
            for property in properties:
                # exception, already handled above
                if property == 'VkPhysicalDevicePortabilitySubsetPropertiesKHR':
                    continue
                gen += self.generate_fill_case(property)
            for feature in features:
                gen += self.generate_fill_case(feature)
            gen += self.generate_platform_protect_end(ext)

        gen += '            default:\n'
        gen += '                break;\n'

        gen += '        }\n\n'
        gen += '        place = structure->pNext;\n'
        gen += '    }\n'
        gen += '}\n'

        return gen

    def generate_fill_queue_family_properties_pnext_chain(self):
        gen = '\nvoid FillQueueFamilyPropertiesPNextChain(PhysicalDeviceData *physicalDeviceData, VkQueueFamilyProperties2KHR *pQueueFamilyProperties2, uint32_t count) {\n'
        gen += '    for (uint32_t i = 0; i < count; ++i) {\n'
        gen += '        void* place = pQueueFamilyProperties2[i].pNext;\n'
        gen += '        while (place) {\n'
        gen += '            VkBaseOutStructure *structure = (VkBaseOutStructure *)place;\n\n'
        gen += '            switch (structure->sType) {\n'

        for name, value  in registry.structs.items():
            if 'VkQueueFamilyProperties2' in value.extends and not value.isAlias:
                gen += '                case ' + value.sType + ': {\n'
                gen += '                    ' + name + ' *data = (' + name + ' *)place;\n'
                gen += '                    void *pNext = data->pNext;\n'
                gen += '                    *data = physicalDeviceData->' + self.create_var_name(name) + ';\n'
                gen += '                    data->pNext = pNext;\n'
                gen += '                } break;\n'

        gen += '                default:\n'
        gen += '                    break;\n'
        gen += '            }\n\n'
        gen += '            place = structure->pNext;\n'
        gen += '        }\n'
        gen += '    }\n'
        gen += '}\n'
        return gen

    def generate_transfer_values(self):
        gen = ''
        for i in range(registry.headerVersionNumber.major):
            major = str(i + 1)
            for j in range(registry.headerVersionNumber.minor):
                minor = str(j + 1)
                gen += '\n\n// VK_VULKAN_' + major + '_' + minor + '\n'

                for ext, property_names, feature_names in self.extension_structs:
                    for property_name in property_names:
                        property = registry.structs[property_name]
                        version = None
                        if property.definedByVersion:
                            version = property.definedByVersion
                        else:
                            for alias_name in property.aliases:
                                alias = registry.structs[alias_name]
                                if alias.definedByVersion:
                                    version = alias.definedByVersion
                                    break
                        if version and version.major == int(major) and version.minor == int(minor):
                            gen += self.generate_transfer_function(major, minor, 'Properties', property_name)

                    for feature_name in feature_names:
                        feature = registry.structs[feature_name]
                        version = None
                        if feature.definedByVersion:
                            version = feature.definedByVersion
                        else:
                            for alias_name in feature.aliases:
                                alias = registry.structs[alias_name]
                                if alias.definedByVersion:
                                    version = alias.definedByVersion
                                    break
                        if version and version.major == int(major) and version.minor == int(minor):
                            gen += self.generate_transfer_function(major, minor, 'Features', feature_name)

        return gen

    def generate_load_device_formats(self):
        gen = '\nvoid LoadDeviceFormats(VkInstance instance, PhysicalDeviceData *pdd, VkPhysicalDevice pd, MapOfVkFormatProperties *dest,\n'
        gen += '                       MapOfVkFormatProperties3 *dest3) {\n'
        gen += '    std::vector<VkFormat> formats = {\n'
        a = registry.enums['VkFormat']
        for format in registry.enums['VkFormat'].values:
            if format not in registry.enums['VkFormat'].aliasValues:
                gen += '        ' + format + ',\n'
        gen += '    };\n'
        gen += '    const auto dt = instance_dispatch_table(instance);\n'
        gen += '    for (const auto format : formats) {\n'
        gen += '        VkFormatProperties3KHR format_properties_3 = {};\n'
        gen += '        format_properties_3.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3_KHR;\n\n'
        gen += '        VkFormatProperties2 format_properties = {};\n'
        gen += '        format_properties.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;\n'
        gen += '        format_properties.pNext = &format_properties_3;\n\n'
        gen += '        if (pdd->GetEffectiveVersion() >= VK_API_VERSION_1_1) {\n'
        gen += '            dt->GetPhysicalDeviceFormatProperties2(pd, format, &format_properties);\n'
        gen += '        } else {\n'
        gen += '            dt->GetPhysicalDeviceFormatProperties2KHR(pd, format, &format_properties);\n'
        gen += '        }\n'
        gen += '        (*dest)[format] = format_properties.formatProperties;\n'
        gen += '        (*dest3)[format] = format_properties_3;\n'
        gen += '    }\n'
        gen += '}\n'
        return gen

    def generate_enumerate_physical_device(self):
        gen = ENUMERATE_PHYSICAL_DEVICES_BEGIN

        for ext, properties, features in self.extension_structs:
            if ext == 'VK_KHR_portability_subset': # portability subset can be emulated and is handled differently
                continue
            gen += self.generate_physical_device_chain_case(ext, None, properties, features)

        for property in self.non_extension_properties:
            version = registry.structs[property].definedByVersion
            gen += self.generate_physical_device_chain_case(None, version, [property], [])
        for feature in self.non_extension_features:
            version = registry.structs[feature].definedByVersion
            gen += self.generate_physical_device_chain_case(None, version, [], [feature])

        gen += ENUMERATE_PHYSICAL_DEVICES_MIDDLE

        for i in range(registry.headerVersionNumber.major):
            version_major = i + 1
            major = str(version_major)
            for j in range(registry.headerVersionNumber.minor):
                version_minor = j + 1
                minor = str(version_minor)
                gen += '\n            // VK_VULKAN_' + str(major) + '_' + str(minor) + '\n'
                for ext, property_names, feature_names in self.extension_structs:
                    for property_name in property_names:
                        property = registry.structs[property_name]
                        promoted_version = None
                        if property.definedByVersion:
                            promoted_version = property.definedByVersion
                        else:
                            for alias_name in property.aliases:
                                alias = registry.structs[alias_name]
                                if alias.definedByVersion:
                                    promoted_version = alias.definedByVersion
                                    break
                        if promoted_version and version_major == promoted_version.major and version_minor == promoted_version.minor:
                            gen += '            TransferValue(&(pdd.physical_device_vulkan_' + major + minor + '_properties_), &(pdd.' + self.create_var_name(property_name) + '), pdd.vulkan_' + major + '_' + minor + '_properties_written_);\n'
                    for feature_name in feature_names:
                        feature = registry.structs[feature_name]
                        promoted_version = None
                        if feature.definedByVersion:
                            promoted_version = feature.definedByVersion
                        else:
                            for alias_name in feature.aliases:
                                alias = registry.structs[alias_name]
                                if alias.definedByVersion:
                                    promoted_version = alias.definedByVersion
                                    break
                        if promoted_version and version_major == promoted_version.major and version_minor == promoted_version.minor:
                            gen += '            TransferValue(&(pdd.physical_device_vulkan_' + major + minor + '_features_), &(pdd.' + self.create_var_name(feature_name) + '), pdd.vulkan_' + major + '_' + minor + '_features_written_);\n'

        gen += ENUMERATE_PHYSICAL_DEVICES_END

        return gen

    def generate_physical_device_chain_case(self, ext, version, property_names, feature_names):
        gen = self.generate_platform_protect_begin(ext)
        if ext:
            gen += '\n                if ('
            first = True
            for promotedTo in [ext] + registry.getExtensionPromotedToExtensionList(ext):
                if first:
                    first = False
                else:
                    gen += ' || '
                gen += 'PhysicalDeviceData::HasExtension(&pdd, '
                gen += registry.extensions[promotedTo].upperCaseName + '_EXTENSION_NAME'
                gen += ')'
            gen += ') {\n'
        else:
            gen += '\n                if (api_version_above_' + str(version.major) + '_' + str(version.minor) + ') {\n'
        for property_name in property_names:
            name = self.create_var_name(property_name)
            gen += '                    pdd.' + name + '.pNext = property_chain.pNext;\n\n'
            gen += '                    property_chain.pNext = &(pdd.' + name + ');\n'
        for feature_name in feature_names:
            name = self.create_var_name(feature_name)
            gen += '                    pdd.' + name + '.pNext = feature_chain.pNext;\n\n'
            gen += '                    feature_chain.pNext = &(pdd.' + name + ');\n'
        gen += '                }\n'
        gen += self.generate_platform_protect_end(ext)
        return gen

    def generate_transfer_function(self, major, minor, type, name):
        gen = '\nvoid TransferValue(VkPhysicalDeviceVulkan' + major + minor + type + ' *dest, ' + name + ' *src, bool promoted_written) {\n'
        for member_name in registry.structs[name].members:
            member = registry.structs[name].members[member_name]
            # The arrays need a enum member to specify the size of the array
            if hasattr(member, 'enum'):
                gen += '    TRANSFER_VALUE_ARRAY(' + member_name + ');\n'
            else:
                gen += '    TRANSFER_VALUE(' + member_name + ');\n'
        gen += '}\n'
        return gen

    def generate_fill_case(self, struct):
        structure = registry.structs[struct]
        if structure.name in self.ignored_structs:
            return ''
        gen = '            case ' + structure.sType + ':\n'
        gen += '                '
        if structure.definedByExtensions:
            gen += 'if ('
            first = True
            for ext in structure.definedByExtensions:
                if first:
                    first = False
                else:
                    gen += ' && '
                for promotedTo in [ext] + registry.getExtensionPromotedToExtensionList(ext):
                    if promotedTo != ext:
                        gen += ' || '
                    else:
                        gen += '('
                    gen += 'PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, '
                    gen += registry.extensions[promotedTo].upperCaseName + '_EXTENSION_NAME'
                    gen += ')'
                gen += ')'
            gen += ') '
        elif structure.definedByVersion and (structure.definedByVersion.major != 1 or structure.definedByVersion.minor != 0):
            gen += 'if (physicalDeviceData->GetEffectiveVersion() >= ' + structure.definedByVersion.versionMacro + ') '
        gen += '{\n'
        gen += '                    ' + structure.name + ' *data = (' + structure.name + ' *)place;\n'
        gen += '                    void *pNext = data->pNext;\n'
        gen += '                    *data = physicalDeviceData->' + self.create_var_name(structure.name) + ';\n'
        gen += '                    data->pNext = pNext;\n'
        gen += '                }\n'
        gen += '                break;\n'
        return gen

    def generate_get_value_function(self, structure):
        if (structure in self.ignored_structs):
            return ''
        gen = self.generate_platform_protect_begin(structure)
        gen += 'bool JsonLoader::GetStruct(const char* device_name, bool requested_profile, const Json::Value &parent, ' + structure + ' *dest) {\n'
        gen += '    (void)dest;\n'
        gen += '    (void)requested_profile;\n'
        gen += '    LogMessage(&layer_settings, DEBUG_REPORT_DEBUG_BIT, \"\\tJsonLoader::GetStruct(' + structure + ')\\n\");\n'
        gen += '    bool valid = true;\n'
        gen += '    for (const auto &member : parent.getMemberNames()) {\n'
        for member_name in registry.structs[structure].members:
            member = registry.structs[structure].members[member_name]
            not_modifiable = str(member.limittype == 'exact' or member.limittype == 'noauto').lower()
            if member.isArray:
                gen += '        GetArray(device_name, parent, member, "' + member_name + '", dest->' + member_name + ', ' + not_modifiable + ');\n'
            elif member.type in registry.enums:
                gen += '        GET_VALUE_ENUM_WARN(member, ' + member_name + ', ' + not_modifiable + ', requested_profile, WarnIfNotEqualEnum);\n'
            elif member.type == 'VkConformanceVersion' or member.type == 'VkToolPurposeFlags':
                continue
            elif member.type == 'VkBool32':
                gen += '        GET_VALUE_WARN(member, ' + member_name + ', ' + not_modifiable + ', requested_profile, WarnIfNotEqualBool);\n'
            elif member.type == 'size_t':
                if 'min' in member.limittype:
                    gen += '        GET_VALUE_SIZE_T_WARN(member, ' + member_name + ', ' + not_modifiable + ', requested_profile, WarnIfLesserSizet);\n'
                elif 'max' in member.limittype or 'bits' in member.limittype:
                    gen += '        GET_VALUE_SIZE_T_WARN(member, ' + member_name + ', ' + not_modifiable + ', requested_profile, WarnIfGreaterSizet);\n'
                #elif member.limittype == 'pot':
                else:
                    gen += '        GET_VALUE_SIZE_T_WARN(member, ' + member_name + ', ' + not_modifiable + ', requested_profile, WarnIfNotEqualSizet);\n'
            elif member.type == 'uint64_t' or member.type == 'int32_t' or member.type == 'VkDeviceSize':
                if 'min' in member.limittype:
                    gen += '        GET_VALUE_WARN(member, ' + member_name + ', ' + not_modifiable + ', requested_profile, WarnIfLesser);\n'
                elif 'max' in member.limittype or 'bits' in member.limittype:
                    gen += '        GET_VALUE_WARN(member, ' + member_name + ', ' + not_modifiable + ', requested_profile, WarnIfGreater);\n'
                else:
                    gen += '        GET_VALUE_WARN(member, ' + member_name + ', ' + not_modifiable + ', requested_profile, WarnIfNotEqual64u);\n'
            elif member.type == 'int64_t':
                if 'min' in member.limittype:
                    gen += '        GET_VALUE_WARN(member, ' + member_name + ', ' + not_modifiable + ', requested_profile, WarnIfLesser);\n'
                elif 'max' in member.limittype or 'bits' in member.limittype:
                    gen += '        GET_VALUE_WARN(member, ' + member_name + ', ' + not_modifiable + ', requested_profile, WarnIfGreater);\n'
                else:
                    gen += '        GET_VALUE_WARN(member, ' + member_name + ', ' + not_modifiable + ', requested_profile, WarnIfNotEquali64);\n'
            elif member.type == 'uint32_t':
                if 'min' in member.limittype:
                    gen += '        GET_VALUE_WARN(member, ' + member_name + ', ' + not_modifiable + ', requested_profile, WarnIfLesser);\n'
                elif 'max' in member.limittype or 'bits' in member.limittype:
                    gen += '        GET_VALUE_WARN(member, ' + member_name + ', ' + not_modifiable + ', requested_profile, WarnIfGreater);\n'
                else:
                    gen += '        GET_VALUE_WARN(member, ' + member_name + ', ' + not_modifiable + ', requested_profile, WarnIfNotEqual32u);\n'
            elif member.type == 'VkExtent2D' or member.type == 'VkExtent3D':
                if 'min' in member.limittype:
                    gen += '        GET_VALUE_WARN(member, ' + member_name + ', ' + not_modifiable + ', requested_profile, WarnIfLesser);\n'
                elif 'max' in member.limittype or 'bits' in member.limittype:
                    gen += '        GET_VALUE_WARN(member, ' + member_name + ', ' + not_modifiable + ', requested_profile, WarnIfGreater);\n'
                else:
                    gen += '        GET_VALUE_WARN(member, ' + member_name + ', ' + not_modifiable + ', requested_profile, WarnIfNotEqual32u);\n'
            elif member.type == 'float':
                if 'min' in member.limittype:
                    gen += '        GET_VALUE_WARN(member, ' + member_name + ', ' + not_modifiable + ', requested_profile, WarnIfLesserFloat);\n'
                elif 'max' in member.limittype:
                    gen += '        GET_VALUE_WARN(member, ' + member_name + ', ' + not_modifiable + ', requested_profile, WarnIfGreaterFloat);\n'
                else:
                    gen += '        GET_VALUE_WARN(member, ' + member_name + ', ' + not_modifiable + ', requested_profile, WarnIfNotEqualFloat);\n'
            elif member.limittype == 'bitmask':
                gen += '        GET_VALUE_FLAG_WARN(member, ' + member_name + ', ' + not_modifiable + ', requested_profile);\n'
            elif member.limittype == 'min': # enum values
                gen += '        GET_VALUE_ENUM_WARN(member, ' + member_name + ', ' + not_modifiable + ', requested_profile, WarnIfLesser);\n'
            elif member.limittype == 'max' or member.limittype == 'bits': # enum values
                gen += '        GET_VALUE_ENUM_WARN(member, ' + member_name + ', ' + not_modifiable + ', requested_profile, WarnIfGreater);\n'
            else:
                print("ERROR: Unsupported limittype '{0}' in member '{1}' of structure '{2}'".format(member.limittype, member_name, structure))

        gen += '    }\n'
        gen += '    return valid;\n'
        gen += '}\n\n'
        gen += self.generate_platform_protect_end(structure)
        return gen

    def get_read_from_type(self, type):
        if type == 'uint32_t':
            return 'asUint()'
        elif type == 'int32_t':
            return 'asInt()'
        return 'asInt()'

    def find_promoted_struct(self, value):
        if value.name.startswith('VkPhysicalDeviceVulkan'):
            if value.name[len('VkPhysicalDeviceVulkan'):][:2].isdigit():
                return None
        if value.definedByVersion:
            promoted = 'VkPhysicalDeviceVulkan' + value.definedByVersion.versionStructSuffic
            if (('VkPhysicalDeviceProperties2' in value.extends)):
                promoted += 'Properties'
            else:
                promoted += 'Features'
            return promoted
        return None


    def generate_json_loader(self):
        gen = JSON_LOADER_BEGIN
        for property in self.non_extension_properties:
            gen += '    bool GetStruct(const char* device_name, bool requested_profile, const Json::Value &parent, ' + property + ' *dest);\n'
        for feature in self.non_extension_features:
            gen += '    bool GetStruct(const char* device_name, bool requested_profile, const Json::Value &parent, ' + feature + ' *dest);\n'
        for ext, properties, features in self.extension_structs:
            gen += self.generate_platform_protect_begin(ext)
            for property in properties:
                if property != 'VkPhysicalDevicePortabilitySubsetPropertiesKHR':
                    gen += '    bool GetStruct(const char* device_name, bool requested_profile, const Json::Value &parent, ' + property + ' *dest);\n'
            for feature in features:
                if feature != 'VkPhysicalDevicePortabilitySubsetFeaturesKHR':
                    gen += '    bool GetStruct(const char* device_name, bool requested_profile, const Json::Value &parent, ' + feature + ' *dest);\n'
            gen += self.generate_platform_protect_end(ext)
        for struct in self.additional_features:
            gen += '    bool GetStruct(const char* device_name, bool requested_profile, const Json::Value &parent, ' + struct + ' *dest);\n'
        for struct in self.additional_properties:
            gen += '    bool GetStruct(const char* device_name, bool requested_profile, const Json::Value &parent, ' + struct + ' *dest);\n'

        gen += WARN_FUNCTIONS
        gen += GET_VALUE_FUNCTIONS
        gen += JSON_LOADER_END
        return gen

    def get_pdd_structs(self):
        properties = []
        properties_alias = []
        features = []
        features_alias = []
        for name, value  in registry.structs.items():
            if ('VkPhysicalDeviceProperties2' in value.extends):
                if (value.isAlias == False):
                    properties.append((name, value.definedByExtensions))
                else:
                    properties_alias.append((name, value.definedByExtensions, value.aliases))
            if ('VkPhysicalDeviceFeatures2' in value.extends):
                if (value.isAlias == False):
                    features.append((name, value.definedByExtensions))
                else:
                    features_alias.append((name, value.definedByExtensions, value.aliases))

        for property_alias in properties_alias:
            for i, j in enumerate(properties):
                for alias in property_alias[2]:
                    if alias == properties[i][0] and self.should_replace(properties[i][1], property_alias[1]):
                        properties[i] = (property_alias[0], property_alias[1])
                        break

        for feature_alias in features_alias:
            for i, j in enumerate(features):
                for alias in feature_alias[2]:
                    if alias == features[i][0] and self.should_replace(features[i][1], feature_alias[1]):
                        features[i] = (feature_alias[0], feature_alias[1])
                        break

        self.non_extension_properties = []
        for property_name, ext in properties:
            if not ext:
                self.non_extension_properties.append(property_name)

        self.non_extension_features = []
        for feature_name, ext in features:
            if not ext:
                self.non_extension_features.append(feature_name)

        self.extension_structs = []
        for extension in registry.extensions:
            feature_names = []
            property_names = []
            for property in properties:
                if property[1] and property[1][0] == extension:
                    property_names.append(property[0])
            for feature in features:
                if feature[1] and feature[1][0] == extension:
                    feature_names.append(feature[0])
            if feature_names or property_names:
                self.extension_structs.append((extension, property_names, feature_names))

    def get_ext(self, extension):
        i = 3
        while (i < len(extension)):
            if (extension[i] == '_'):
                break
            i += 1
        return extension[3:i]

    def should_replace(self, old_extension, new_extension):
        if (not old_extension):
            return True
        if (not new_extension):
            return False
        old_ext = self.get_ext(old_extension[0])
        new_ext = self.get_ext(new_extension[0])
        if old_ext == 'KHR' or (old_ext == 'EXT' and new_ext != 'KHR'):
            return True
        return False

    def create_var_name(self, struct):
        nv = struct.endswith("NV")
        arm = struct.endswith("ARM")
        ext = struct.endswith("EXT")
        var_name = ''
        while struct[-1].isupper():
            struct = struct[:-1]
        c = 0
        while c < len(struct):
            upper = struct[c].isupper()
            number = struct[c].isdigit()
            if upper or number:
                var_name += '_'
                while c < len(struct) and (upper and struct[c].isupper() or number and struct[c].isdigit()):
                    var_name += struct[c].lower()
                    c += 1
            else:
                var_name += struct[c]
                c += 1
        var_name = var_name[4:] + '_'

        # Exceptions
        if var_name == 'physical_device_shader_float_16_int_8_features_':
            var_name = 'physical_device_float_16_int_8_features_'
        if 'queue_family_' in var_name:
            var_name = 'arrayof_queue_family_properties_[i].' + var_name.replace('queue_family_', '')
        if (var_name == 'physical_device_mesh_shader_features_' or var_name == 'physical_device_mesh_shader_properties_') and nv:
            var_name += 'nv_'
        if (var_name == 'physical_device_cooperative_matrix_features_' or var_name == 'physical_device_cooperative_matrix_properties_') and nv:
            var_name += 'nv_'
        if (var_name == 'physical_device_device_generated_commands_properties_' or var_name == 'physical_device_device_generated_commands_features_') and nv:
            var_name += 'nv_'
        if (var_name == 'physical_device_shader_core_properties_' and arm):
            var_name += 'arm_'
        if (var_name == 'physical_device_vertex_attribute_divisor_properties_' and ext):
            var_name += 'ext_'
        return var_name

    def get_non_aliased_list(self, list, aliases):
        ret = []
        for el in list:
            if el not in aliases:
                ret.append(el)
        return ret

    def generate_string_to_uint(self, lists, enums):
        gen = '\nstatic uint32_t VkStringToUint(const std::string &input_value) {\n'
        gen += '    static const std::unordered_map<std::string, uint32_t> map = {\n'
        for list in lists:
            if list not in enums:
                continue
            gen += '        // ' + list + '\n'
            for enum in enums[list].values:
                gen += '        {\"' + enum + '\", static_cast<uint32_t>(' + enum + ')},\n'
        gen += '    };\n'
        gen += '    const auto it = map.find(input_value);\n'
        gen += '    if (it != map.end()) {\n'
        gen += '        return it->second;\n'
        gen += '    }\n'
        gen += '    return 0;\n'
        gen += '}\n'
        return gen


    def generate_string_to_flag_functions(self, function_names):
        gen = ''
        for name in function_names:
            if name in registry.bitmasks:
                gen += self.generate_string_to_flags(name, registry.bitmasks[name].bitsType)
            elif name in registry.enums:
                gen += self.generate_string_to_flags(name, registry.enums[name])
        return gen

    def generate_string_to_flags(self, type, enums):
        gen = '\nstatic ' + type + ' StringTo' + type + '(const std::string &input_value) {\n'
        gen += '    static const std::unordered_map<std::string, ' + type + '> map = {\n'
        for enum in enums.values:
            gen += '        {\"' + enum + '\", ' + enum + '},\n'
        gen += '    };\n'
        gen += '    const auto it = map.find(input_value);\n'
        gen += '    if (it != map.end()) {\n'
        gen += '        return it->second;\n'
        gen += '    }\n'
        gen += '    return ' + type + '{};\n'
        gen += '}\n'
        return gen

    def generate_string_to_enum(self, enum_name, values):
        gen = '\n' + enum_name + ' Get' + enum_name + '(const std::vector<std::string> &values) {\n'
        gen += '    ' + enum_name + ' result = 0;\n\n'
        gen += '    for (std::size_t i = 0, n = values.size(); i < n; ++i) {\n'
        first = True
        for value in values:
            if first:
                first = False
                gen += '        '
            else:
                gen += ' else '
            gen += 'if (values[i] == \"' + value + '\") {\n'
            gen += '            result |= ' + value + ';\n'
            gen += '        }'
        gen += '\n    }\n\n'
        gen += "    return result;\n"
        gen += '}\n'
        return gen

    def generate_enum_to_string(self, enum_name, values, func_name):
        gen = '\nstd::string ' + func_name + '(' + enum_name + ' flags) {\n'
        gen += '    std::string result = {};\n\n'
        first = True
        for value in values:
            gen += '    if (flags & ' + value + ') {\n'
            if first:
                first = False
            else:
                gen += '        if (!result.empty()) result += \", \";\n'
            gen += '        result += \"' + value + '\";\n'
            gen += '    }\n'
        gen += '\n'
        gen += '    return result;\n'
        gen += '}\n'
        return gen

    def generate_format_to_string(self, formats, aliases):
        gen = '\nstd::string vkFormatToString(VkFormat fmt) {\n'
        gen += '    switch (fmt) {\n'
        for format in formats:
            if format in aliases:
                continue
            gen += '        case ' + format + ':\n'
            gen += '            return \"' + format + '\";\n'
        gen += '        default:\n'
        gen += '            return \"VK_FORMAT_UNDEFINED\";\n'
        gen += '    }\n'
        gen += '}\n'
        return gen

    def generate_string_to_format(self, formats):
        gen = '\nstatic VkFormat StringToFormat(const std::string &input_value) {\n'
        gen += '    static const std::unordered_map<std::string, VkFormat> map = {\n'
        for format in formats:
            gen += '        {\"' + format + '\", ' + format + '},\n'
        gen += '    };\n'
        gen += '    const auto it = map.find(input_value);\n'
        gen += '    if (it != map.end()) {\n'
        gen += '        return it->second;\n'
        gen += '    }\n'
        gen += '    return VK_FORMAT_UNDEFINED;\n'
        gen += '}\n'
        return gen

    def generate_string_to_image_layout(self, imageLayouts):
        gen = '\nstatic VkImageLayout StringToImageLayout(const std::string &input_value) {\n'
        gen += '    static const std::unordered_map<std::string, VkImageLayout> map = {\n'
        for imageLayout in imageLayouts:
            gen += '        {\"' + imageLayout + '\", ' + imageLayout + '},\n'
        gen += '    };\n'
        gen += '    const auto it = map.find(input_value);\n'
        gen += '    if (it != map.end()) {\n'
        gen += '        return it->second;\n'
        gen += '    }\n'
        gen += '    return VK_IMAGE_LAYOUT_UNDEFINED;\n'
        gen += '}\n'
        return gen

if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument('--api', action='store',
                        default='vulkan',
                        choices=['vulkan'],
                        help="Target API")
    parser.add_argument('--registry', '-r', action='store', help='Use specified registry file instead of vk.xml')
    parser.add_argument('--out-layer', action='store', help='Output the layer source file')

    args = parser.parse_args()

    registryPath  = '../external/Debug/Vulkan-Headers/build/install/share/vulkan/registry/vk.xml'
    if args.registry is not None:
        registryPath = args.registry

    outputPath = "../layer/profiles_generated.cpp"
    if args.out_layer is not None:
        outputPath = args.out_layer

    registry = gen_profiles_solution.VulkanRegistry(registryPath, args.api)

    generator = VulkanProfilesLayerGenerator()
    generator.generate(outputPath, registry)
