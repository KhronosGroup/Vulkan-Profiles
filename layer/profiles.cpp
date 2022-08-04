
// This file is ***GENERATED***.  Do Not Edit.
// See scripts/gen_layer.py for modifications.

/*
 * Copyright (C) 2015-2022 Valve Corporation
 * Copyright (C) 2015-2022 LunarG, Inc.
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
 * Author: Mike Weiblen <mikew@lunarg.com>
 * Author: Arda Coskunses <arda@lunarg.com>
 * Author: Jeremy Kniager <jeremyk@lunarg.com>
 * Author: Ziga Markus <ziga@lunarg.com>
 * Author: Christophe Riccio <christophe@lunarg.com>
 * Author: Mark Lobodzinski <mark@lunarg.com>
 */

/*
 * layer/profiles.cpp - The VK_LAYER_KHRONOS_profiles layer.
 * This Profiles layer simulates a device by loading a JSON configuration file to override values that would normally be returned
 * from a Vulkan implementation.  Configuration files must validate with the Profiles schema; this layer does not redundantly
 * check for configuration errors that would be caught by schema validation.
 *
 * References (several documents are also included in the LunarG Vulkan SDK, see [SDK]):
 * [SPEC]   https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/vkspec.html
 * [SDK]    https://vulkan.lunarg.com/sdk/home
 * [LALI]   https://github.com/KhronosGroup/Vulkan-Loader/blob/master/loader/LoaderAndLayerInterface.md
 *
 * Misc notes:
 * This code generally follows the spirit of the Google C++ styleguide, while accommodating conventions of the Vulkan styleguide.
 * https://google.github.io/styleguide/cppguide.html
 * https://www.khronos.org/registry/vulkan/specs/1.1/styleguide.html
 */

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <cinttypes>
#include <string.h>

#include <functional>
#include <unordered_map>
#include <vector>
#include <array>
#include <fstream>
#include <mutex>
#include <sstream>

#include <valijson/adapters/jsoncpp_adapter.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validation_results.hpp>
#include <valijson/validator.hpp>

#include <json/json.h>  // https://github.com/open-source-parsers/jsoncpp

#include "vulkan/vk_layer.h"
#include "vulkan/vulkan_beta.h"
#include <vk_layer_config.h>
#include "vk_layer_table.h"
#include "vk_layer_settings.h"
#include "profiles.h"

using valijson::Schema;
using valijson::SchemaParser;
using valijson::ValidationResults;
using valijson::Validator;
using valijson::adapters::JsonCppAdapter;

namespace {

// Global constants //////////////////////////////////////////////////////////////////////////////////////////////////////////////

// For new features/functionality, increment the minor level and reset patch level to zero.
// For any changes, at least increment the patch level.  See https://semver.org/
// When updating the version, be sure to make corresponding changes to the layer manifest file at
// layer/VkLayer_khronos_profiles.json.in

const uint32_t kVersionProfilesMajor = 1;
const uint32_t kVersionProfilesMinor = 0;
const uint32_t kVersionProfilesPatch = 0;
const uint32_t kVersionProfilesImplementation =
    VK_MAKE_VERSION(kVersionProfilesMajor, kVersionProfilesMinor, kVersionProfilesPatch);

static const char *SCHEMA_URI_BASE = "https://schema.khronos.org/vulkan/profiles-";

// Properties of this layer:
const VkLayerProperties kLayerProperties[] = {{
    "VK_LAYER_KHRONOS_profiles",     // layerName
    VK_MAKE_VERSION(1, 0, 68),       // specVersion (clamped to final 1.0 spec version)
    kVersionProfilesImplementation,  // implementationVersion
    "Khronos Profiles layer"         // description
}};
const uint32_t kLayerPropertiesCount = (sizeof(kLayerProperties) / sizeof(kLayerProperties[0]));

// Name of this layer:
const char *kOurLayerName = kLayerProperties[0].layerName;

// Instance extensions that this layer provides:
const std::array<VkExtensionProperties, 0> kInstanceExtensionProperties = {};
const uint32_t kInstanceExtensionPropertiesCount = static_cast<uint32_t>(kInstanceExtensionProperties.size());

// Device extensions that this layer provides:
const std::array<VkExtensionProperties, 2> kDeviceExtensionProperties = {
    {{VK_EXT_TOOLING_INFO_EXTENSION_NAME, VK_EXT_TOOLING_INFO_SPEC_VERSION},
     {VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME, VK_KHR_PORTABILITY_SUBSET_SPEC_VERSION}}};
const uint32_t kDeviceExtensionPropertiesCount = static_cast<uint32_t>(kDeviceExtensionProperties.size());

uint32_t requested_version = 0;
bool device_has_astc_hdr = false;
bool device_has_astc = false;
bool device_has_etc2 = false;
bool device_has_bc = false;
bool device_has_pvrtc = false;

FILE *profiles_log_file = nullptr;

static VkProfileLayerSettingsEXT *layer_settings = new VkProfileLayerSettingsEXT{};

const char *const kLayerSettingsProfileFile = "profile_file";
const char *const kLayerSettingsProfileName = "profile_name";
const char *const kLayerSettingsProfileValidation = "profile_validation";
const char *const kLayerSettingsEmulatePortability = "emulate_portability";
const char *const kLayerSettingsSimulateCapabilities = "simulate_capabilities";
const char *const kLayerSettingsDebugActions = "debug_actions";
const char *const kLayerSettingsDebugFilename = "debug_filename";
const char *const kLayerSettingsDebugFileClear = "debug_file_clear";
const char *const kLayerSettingsDebugFailOnError = "debug_fail_on_error";
const char *const kLayerSettingsDebugReports = "debug_reports";
const char *const kLayerSettingsExcludeDeviceExtensions = "exclude_device_extensions";
const char *const kLayerSettingsExcludeFormats = "exclude_formats";

static SimulateCapabilityFlags GetSimulateCapabilityFlags(const vku::Strings &values) {
    SimulateCapabilityFlags result = 0;

    for (std::size_t i = 0, n = values.size(); i < n; ++i) {
        if (values[i] == "SIMULATE_API_VERSION_BIT") {
            result |= SIMULATE_API_VERSION_BIT;
        } else if (values[i] == "SIMULATE_FEATURES_BIT") {
            result |= SIMULATE_FEATURES_BIT;
        } else if (values[i] == "SIMULATE_PROPERTIES_BIT") {
            result |= SIMULATE_PROPERTIES_BIT;
        } else if (values[i] == "SIMULATE_EXTENSIONS_BIT") {
            result |= SIMULATE_EXTENSIONS_BIT;
        } else if (values[i] == "SIMULATE_FORMATS_BIT") {
            result |= SIMULATE_FORMATS_BIT;
        } else if (values[i] == "SIMULATE_QUEUE_FAMILY_PROPERTIES_BIT") {
            result |= SIMULATE_QUEUE_FAMILY_PROPERTIES_BIT;
        }
    }

    return result;
}

static std::string GetSimulateCapabilitiesLog(SimulateCapabilityFlags flags) {
    std::string result = {};

    if (flags & SIMULATE_API_VERSION_BIT) {
        result += "SIMULATE_API_VERSION_BIT";
    }
    if (flags & SIMULATE_FEATURES_BIT) {
        if (!result.empty()) result += ", ";
        result += "SIMULATE_FEATURES_BIT";
    }
    if (flags & SIMULATE_PROPERTIES_BIT) {
        if (!result.empty()) result += ", ";
        result += "SIMULATE_PROPERTIES_BIT";
    }
    if (flags & SIMULATE_EXTENSIONS_BIT) {
        if (!result.empty()) result += ", ";
        result += "SIMULATE_EXTENSIONS_BIT";
    }
    if (flags & SIMULATE_FORMATS_BIT) {
        if (!result.empty()) result += ", ";
        result += "SIMULATE_FORMATS_BIT";
    }
    if (flags & SIMULATE_QUEUE_FAMILY_PROPERTIES_BIT) {
        if (!result.empty()) result += ", ";
        result += "SIMULATE_QUEUE_FAMILY_PROPERTIES_BIT";
    }

    return result;
}

static std::string GetDebugReportsLog(DebugActionFlags flags) {
    std::string result = {};

    if (flags & DEBUG_REPORT_NOTIFICATION_BIT) {
        result += "DEBUG_REPORT_NOTIFICATION_BIT";
    }
    if (flags & DEBUG_REPORT_WARNING_BIT) {
        if (!result.empty()) result += ", ";
        result += "DEBUG_REPORT_WARNING_BIT";
    }
    if (flags & DEBUG_REPORT_ERROR_BIT) {
        if (!result.empty()) result += ", ";
        result += "DEBUG_REPORT_ERROR_BIT";
    }
    if (flags & DEBUG_REPORT_DEBUG_BIT) {
        if (!result.empty()) result += ", ";
        result += "DEBUG_REPORT_DEBUG_BIT";
    }

    return result;
}

static std::string GetFormatFeatureString(VkFormatFeatureFlags flags) {
    std::string result = {};

    if (flags & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) {
        result += "VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_BLIT_SRC_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_BLIT_SRC_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_BLIT_DST_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_BLIT_DST_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_TRANSFER_SRC_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_TRANSFER_SRC_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_TRANSFER_DST_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_TRANSFER_DST_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_DISJOINT_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_DISJOINT_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR";
    }
    if (flags & VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR";
    }
    if (flags & VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR";
    }
    if (flags & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT";
    }
    if (flags & VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT";
    }
    if (flags & VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR";
    }
    if (flags & VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR";
    }
    if (flags & VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR";
    }

    return result;
}

static std::string GetFormatFeature2String(VkFormatFeatureFlags2 flags) {
    std::string result = {};

    if (flags & VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT) {
        result += "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_STORAGE_IMAGE_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_STORAGE_IMAGE_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_STORAGE_IMAGE_ATOMIC_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_STORAGE_IMAGE_ATOMIC_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_UNIFORM_TEXEL_BUFFER_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_UNIFORM_TEXEL_BUFFER_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_STORAGE_TEXEL_BUFFER_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_STORAGE_TEXEL_BUFFER_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_STORAGE_TEXEL_BUFFER_ATOMIC_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_STORAGE_TEXEL_BUFFER_ATOMIC_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_VERTEX_BUFFER_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_VERTEX_BUFFER_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BLEND_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BLEND_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_DEPTH_STENCIL_ATTACHMENT_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_DEPTH_STENCIL_ATTACHMENT_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_BLIT_SRC_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_BLIT_SRC_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_BLIT_DST_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_BLIT_DST_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_LINEAR_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_LINEAR_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_CUBIC_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_CUBIC_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_TRANSFER_SRC_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_TRANSFER_SRC_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_TRANSFER_DST_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_TRANSFER_DST_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_MINMAX_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_MINMAX_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_MIDPOINT_CHROMA_SAMPLES_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_MIDPOINT_CHROMA_SAMPLES_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_DISJOINT_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_DISJOINT_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_COSITED_CHROMA_SAMPLES_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_COSITED_CHROMA_SAMPLES_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_STORAGE_READ_WITHOUT_FORMAT_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_STORAGE_READ_WITHOUT_FORMAT_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_STORAGE_WRITE_WITHOUT_FORMAT_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_STORAGE_WRITE_WITHOUT_FORMAT_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_DEPTH_COMPARISON_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_DEPTH_COMPARISON_BIT";
    }
    if (flags & VK_FORMAT_FEATURE_2_VIDEO_DECODE_OUTPUT_BIT_KHR) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_VIDEO_DECODE_OUTPUT_BIT_KHR";
    }
    if (flags & VK_FORMAT_FEATURE_2_VIDEO_DECODE_DPB_BIT_KHR) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_VIDEO_DECODE_DPB_BIT_KHR";
    }
    if (flags & VK_FORMAT_FEATURE_2_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR";
    }
    if (flags & VK_FORMAT_FEATURE_2_FRAGMENT_DENSITY_MAP_BIT_EXT) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_FRAGMENT_DENSITY_MAP_BIT_EXT";
    }
    if (flags & VK_FORMAT_FEATURE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR";
    }
    if (flags & VK_FORMAT_FEATURE_2_VIDEO_ENCODE_INPUT_BIT_KHR) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_VIDEO_ENCODE_INPUT_BIT_KHR";
    }
    if (flags & VK_FORMAT_FEATURE_2_VIDEO_ENCODE_DPB_BIT_KHR) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_VIDEO_ENCODE_DPB_BIT_KHR";
    }
    if (flags & VK_FORMAT_FEATURE_2_LINEAR_COLOR_ATTACHMENT_BIT_NV) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_LINEAR_COLOR_ATTACHMENT_BIT_NV";
    }
    if (flags & VK_FORMAT_FEATURE_2_WEIGHT_IMAGE_BIT_QCOM) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_WEIGHT_IMAGE_BIT_QCOM";
    }
    if (flags & VK_FORMAT_FEATURE_2_WEIGHT_SAMPLED_IMAGE_BIT_QCOM) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_WEIGHT_SAMPLED_IMAGE_BIT_QCOM";
    }
    if (flags & VK_FORMAT_FEATURE_2_BLOCK_MATCHING_BIT_QCOM) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_BLOCK_MATCHING_BIT_QCOM";
    }
    if (flags & VK_FORMAT_FEATURE_2_BOX_FILTER_SAMPLED_BIT_QCOM) {
        if (!result.empty()) result += ", ";
        result += "VK_FORMAT_FEATURE_2_BOX_FILTER_SAMPLED_BIT_QCOM";
    }

    return result;
}

static std::string GetQueueFlagsToString(VkQueueFlags flags) {
    std::string result = {};

    if (flags & VK_QUEUE_GRAPHICS_BIT) {
        result += "VK_QUEUE_GRAPHICS_BIT";
    }
    if (flags & VK_QUEUE_COMPUTE_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_QUEUE_COMPUTE_BIT";
    }
    if (flags & VK_QUEUE_TRANSFER_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_QUEUE_TRANSFER_BIT";
    }
    if (flags & VK_QUEUE_SPARSE_BINDING_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_QUEUE_SPARSE_BINDING_BIT";
    }
    if (flags & VK_QUEUE_PROTECTED_BIT) {
        if (!result.empty()) result += ", ";
        result += "VK_QUEUE_PROTECTED_BIT";
    }
    if (flags & VK_QUEUE_VIDEO_DECODE_BIT_KHR) {
        if (!result.empty()) result += ", ";
        result += "VK_QUEUE_VIDEO_DECODE_BIT_KHR";
    }
    if (flags & VK_QUEUE_VIDEO_ENCODE_BIT_KHR) {
        if (!result.empty()) result += ", ";
        result += "VK_QUEUE_VIDEO_ENCODE_BIT_KHR";
    }

    return result;
}

std::string vkFormatToString(VkFormat fmt) {
    switch (fmt) {
        case VK_FORMAT_UNDEFINED:
            return "VK_FORMAT_UNDEFINED";
        case VK_FORMAT_R4G4_UNORM_PACK8:
            return "VK_FORMAT_R4G4_UNORM_PACK8";
        case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
            return "VK_FORMAT_R4G4B4A4_UNORM_PACK16";
        case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
            return "VK_FORMAT_B4G4R4A4_UNORM_PACK16";
        case VK_FORMAT_R5G6B5_UNORM_PACK16:
            return "VK_FORMAT_R5G6B5_UNORM_PACK16";
        case VK_FORMAT_B5G6R5_UNORM_PACK16:
            return "VK_FORMAT_B5G6R5_UNORM_PACK16";
        case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
            return "VK_FORMAT_R5G5B5A1_UNORM_PACK16";
        case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
            return "VK_FORMAT_B5G5R5A1_UNORM_PACK16";
        case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
            return "VK_FORMAT_A1R5G5B5_UNORM_PACK16";
        case VK_FORMAT_R8_UNORM:
            return "VK_FORMAT_R8_UNORM";
        case VK_FORMAT_R8_SNORM:
            return "VK_FORMAT_R8_SNORM";
        case VK_FORMAT_R8_USCALED:
            return "VK_FORMAT_R8_USCALED";
        case VK_FORMAT_R8_SSCALED:
            return "VK_FORMAT_R8_SSCALED";
        case VK_FORMAT_R8_UINT:
            return "VK_FORMAT_R8_UINT";
        case VK_FORMAT_R8_SINT:
            return "VK_FORMAT_R8_SINT";
        case VK_FORMAT_R8_SRGB:
            return "VK_FORMAT_R8_SRGB";
        case VK_FORMAT_R8G8_UNORM:
            return "VK_FORMAT_R8G8_UNORM";
        case VK_FORMAT_R8G8_SNORM:
            return "VK_FORMAT_R8G8_SNORM";
        case VK_FORMAT_R8G8_USCALED:
            return "VK_FORMAT_R8G8_USCALED";
        case VK_FORMAT_R8G8_SSCALED:
            return "VK_FORMAT_R8G8_SSCALED";
        case VK_FORMAT_R8G8_UINT:
            return "VK_FORMAT_R8G8_UINT";
        case VK_FORMAT_R8G8_SINT:
            return "VK_FORMAT_R8G8_SINT";
        case VK_FORMAT_R8G8_SRGB:
            return "VK_FORMAT_R8G8_SRGB";
        case VK_FORMAT_R8G8B8_UNORM:
            return "VK_FORMAT_R8G8B8_UNORM";
        case VK_FORMAT_R8G8B8_SNORM:
            return "VK_FORMAT_R8G8B8_SNORM";
        case VK_FORMAT_R8G8B8_USCALED:
            return "VK_FORMAT_R8G8B8_USCALED";
        case VK_FORMAT_R8G8B8_SSCALED:
            return "VK_FORMAT_R8G8B8_SSCALED";
        case VK_FORMAT_R8G8B8_UINT:
            return "VK_FORMAT_R8G8B8_UINT";
        case VK_FORMAT_R8G8B8_SINT:
            return "VK_FORMAT_R8G8B8_SINT";
        case VK_FORMAT_R8G8B8_SRGB:
            return "VK_FORMAT_R8G8B8_SRGB";
        case VK_FORMAT_B8G8R8_UNORM:
            return "VK_FORMAT_B8G8R8_UNORM";
        case VK_FORMAT_B8G8R8_SNORM:
            return "VK_FORMAT_B8G8R8_SNORM";
        case VK_FORMAT_B8G8R8_USCALED:
            return "VK_FORMAT_B8G8R8_USCALED";
        case VK_FORMAT_B8G8R8_SSCALED:
            return "VK_FORMAT_B8G8R8_SSCALED";
        case VK_FORMAT_B8G8R8_UINT:
            return "VK_FORMAT_B8G8R8_UINT";
        case VK_FORMAT_B8G8R8_SINT:
            return "VK_FORMAT_B8G8R8_SINT";
        case VK_FORMAT_B8G8R8_SRGB:
            return "VK_FORMAT_B8G8R8_SRGB";
        case VK_FORMAT_R8G8B8A8_UNORM:
            return "VK_FORMAT_R8G8B8A8_UNORM";
        case VK_FORMAT_R8G8B8A8_SNORM:
            return "VK_FORMAT_R8G8B8A8_SNORM";
        case VK_FORMAT_R8G8B8A8_USCALED:
            return "VK_FORMAT_R8G8B8A8_USCALED";
        case VK_FORMAT_R8G8B8A8_SSCALED:
            return "VK_FORMAT_R8G8B8A8_SSCALED";
        case VK_FORMAT_R8G8B8A8_UINT:
            return "VK_FORMAT_R8G8B8A8_UINT";
        case VK_FORMAT_R8G8B8A8_SINT:
            return "VK_FORMAT_R8G8B8A8_SINT";
        case VK_FORMAT_R8G8B8A8_SRGB:
            return "VK_FORMAT_R8G8B8A8_SRGB";
        case VK_FORMAT_B8G8R8A8_UNORM:
            return "VK_FORMAT_B8G8R8A8_UNORM";
        case VK_FORMAT_B8G8R8A8_SNORM:
            return "VK_FORMAT_B8G8R8A8_SNORM";
        case VK_FORMAT_B8G8R8A8_USCALED:
            return "VK_FORMAT_B8G8R8A8_USCALED";
        case VK_FORMAT_B8G8R8A8_SSCALED:
            return "VK_FORMAT_B8G8R8A8_SSCALED";
        case VK_FORMAT_B8G8R8A8_UINT:
            return "VK_FORMAT_B8G8R8A8_UINT";
        case VK_FORMAT_B8G8R8A8_SINT:
            return "VK_FORMAT_B8G8R8A8_SINT";
        case VK_FORMAT_B8G8R8A8_SRGB:
            return "VK_FORMAT_B8G8R8A8_SRGB";
        case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
            return "VK_FORMAT_A8B8G8R8_UNORM_PACK32";
        case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
            return "VK_FORMAT_A8B8G8R8_SNORM_PACK32";
        case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
            return "VK_FORMAT_A8B8G8R8_USCALED_PACK32";
        case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
            return "VK_FORMAT_A8B8G8R8_SSCALED_PACK32";
        case VK_FORMAT_A8B8G8R8_UINT_PACK32:
            return "VK_FORMAT_A8B8G8R8_UINT_PACK32";
        case VK_FORMAT_A8B8G8R8_SINT_PACK32:
            return "VK_FORMAT_A8B8G8R8_SINT_PACK32";
        case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
            return "VK_FORMAT_A8B8G8R8_SRGB_PACK32";
        case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
            return "VK_FORMAT_A2R10G10B10_UNORM_PACK32";
        case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
            return "VK_FORMAT_A2R10G10B10_SNORM_PACK32";
        case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
            return "VK_FORMAT_A2R10G10B10_USCALED_PACK32";
        case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
            return "VK_FORMAT_A2R10G10B10_SSCALED_PACK32";
        case VK_FORMAT_A2R10G10B10_UINT_PACK32:
            return "VK_FORMAT_A2R10G10B10_UINT_PACK32";
        case VK_FORMAT_A2R10G10B10_SINT_PACK32:
            return "VK_FORMAT_A2R10G10B10_SINT_PACK32";
        case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
            return "VK_FORMAT_A2B10G10R10_UNORM_PACK32";
        case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
            return "VK_FORMAT_A2B10G10R10_SNORM_PACK32";
        case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
            return "VK_FORMAT_A2B10G10R10_USCALED_PACK32";
        case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
            return "VK_FORMAT_A2B10G10R10_SSCALED_PACK32";
        case VK_FORMAT_A2B10G10R10_UINT_PACK32:
            return "VK_FORMAT_A2B10G10R10_UINT_PACK32";
        case VK_FORMAT_A2B10G10R10_SINT_PACK32:
            return "VK_FORMAT_A2B10G10R10_SINT_PACK32";
        case VK_FORMAT_R16_UNORM:
            return "VK_FORMAT_R16_UNORM";
        case VK_FORMAT_R16_SNORM:
            return "VK_FORMAT_R16_SNORM";
        case VK_FORMAT_R16_USCALED:
            return "VK_FORMAT_R16_USCALED";
        case VK_FORMAT_R16_SSCALED:
            return "VK_FORMAT_R16_SSCALED";
        case VK_FORMAT_R16_UINT:
            return "VK_FORMAT_R16_UINT";
        case VK_FORMAT_R16_SINT:
            return "VK_FORMAT_R16_SINT";
        case VK_FORMAT_R16_SFLOAT:
            return "VK_FORMAT_R16_SFLOAT";
        case VK_FORMAT_R16G16_UNORM:
            return "VK_FORMAT_R16G16_UNORM";
        case VK_FORMAT_R16G16_SNORM:
            return "VK_FORMAT_R16G16_SNORM";
        case VK_FORMAT_R16G16_USCALED:
            return "VK_FORMAT_R16G16_USCALED";
        case VK_FORMAT_R16G16_SSCALED:
            return "VK_FORMAT_R16G16_SSCALED";
        case VK_FORMAT_R16G16_UINT:
            return "VK_FORMAT_R16G16_UINT";
        case VK_FORMAT_R16G16_SINT:
            return "VK_FORMAT_R16G16_SINT";
        case VK_FORMAT_R16G16_SFLOAT:
            return "VK_FORMAT_R16G16_SFLOAT";
        case VK_FORMAT_R16G16B16_UNORM:
            return "VK_FORMAT_R16G16B16_UNORM";
        case VK_FORMAT_R16G16B16_SNORM:
            return "VK_FORMAT_R16G16B16_SNORM";
        case VK_FORMAT_R16G16B16_USCALED:
            return "VK_FORMAT_R16G16B16_USCALED";
        case VK_FORMAT_R16G16B16_SSCALED:
            return "VK_FORMAT_R16G16B16_SSCALED";
        case VK_FORMAT_R16G16B16_UINT:
            return "VK_FORMAT_R16G16B16_UINT";
        case VK_FORMAT_R16G16B16_SINT:
            return "VK_FORMAT_R16G16B16_SINT";
        case VK_FORMAT_R16G16B16_SFLOAT:
            return "VK_FORMAT_R16G16B16_SFLOAT";
        case VK_FORMAT_R16G16B16A16_UNORM:
            return "VK_FORMAT_R16G16B16A16_UNORM";
        case VK_FORMAT_R16G16B16A16_SNORM:
            return "VK_FORMAT_R16G16B16A16_SNORM";
        case VK_FORMAT_R16G16B16A16_USCALED:
            return "VK_FORMAT_R16G16B16A16_USCALED";
        case VK_FORMAT_R16G16B16A16_SSCALED:
            return "VK_FORMAT_R16G16B16A16_SSCALED";
        case VK_FORMAT_R16G16B16A16_UINT:
            return "VK_FORMAT_R16G16B16A16_UINT";
        case VK_FORMAT_R16G16B16A16_SINT:
            return "VK_FORMAT_R16G16B16A16_SINT";
        case VK_FORMAT_R16G16B16A16_SFLOAT:
            return "VK_FORMAT_R16G16B16A16_SFLOAT";
        case VK_FORMAT_R32_UINT:
            return "VK_FORMAT_R32_UINT";
        case VK_FORMAT_R32_SINT:
            return "VK_FORMAT_R32_SINT";
        case VK_FORMAT_R32_SFLOAT:
            return "VK_FORMAT_R32_SFLOAT";
        case VK_FORMAT_R32G32_UINT:
            return "VK_FORMAT_R32G32_UINT";
        case VK_FORMAT_R32G32_SINT:
            return "VK_FORMAT_R32G32_SINT";
        case VK_FORMAT_R32G32_SFLOAT:
            return "VK_FORMAT_R32G32_SFLOAT";
        case VK_FORMAT_R32G32B32_UINT:
            return "VK_FORMAT_R32G32B32_UINT";
        case VK_FORMAT_R32G32B32_SINT:
            return "VK_FORMAT_R32G32B32_SINT";
        case VK_FORMAT_R32G32B32_SFLOAT:
            return "VK_FORMAT_R32G32B32_SFLOAT";
        case VK_FORMAT_R32G32B32A32_UINT:
            return "VK_FORMAT_R32G32B32A32_UINT";
        case VK_FORMAT_R32G32B32A32_SINT:
            return "VK_FORMAT_R32G32B32A32_SINT";
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            return "VK_FORMAT_R32G32B32A32_SFLOAT";
        case VK_FORMAT_R64_UINT:
            return "VK_FORMAT_R64_UINT";
        case VK_FORMAT_R64_SINT:
            return "VK_FORMAT_R64_SINT";
        case VK_FORMAT_R64_SFLOAT:
            return "VK_FORMAT_R64_SFLOAT";
        case VK_FORMAT_R64G64_UINT:
            return "VK_FORMAT_R64G64_UINT";
        case VK_FORMAT_R64G64_SINT:
            return "VK_FORMAT_R64G64_SINT";
        case VK_FORMAT_R64G64_SFLOAT:
            return "VK_FORMAT_R64G64_SFLOAT";
        case VK_FORMAT_R64G64B64_UINT:
            return "VK_FORMAT_R64G64B64_UINT";
        case VK_FORMAT_R64G64B64_SINT:
            return "VK_FORMAT_R64G64B64_SINT";
        case VK_FORMAT_R64G64B64_SFLOAT:
            return "VK_FORMAT_R64G64B64_SFLOAT";
        case VK_FORMAT_R64G64B64A64_UINT:
            return "VK_FORMAT_R64G64B64A64_UINT";
        case VK_FORMAT_R64G64B64A64_SINT:
            return "VK_FORMAT_R64G64B64A64_SINT";
        case VK_FORMAT_R64G64B64A64_SFLOAT:
            return "VK_FORMAT_R64G64B64A64_SFLOAT";
        case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
            return "VK_FORMAT_B10G11R11_UFLOAT_PACK32";
        case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
            return "VK_FORMAT_E5B9G9R9_UFLOAT_PACK32";
        case VK_FORMAT_D16_UNORM:
            return "VK_FORMAT_D16_UNORM";
        case VK_FORMAT_X8_D24_UNORM_PACK32:
            return "VK_FORMAT_X8_D24_UNORM_PACK32";
        case VK_FORMAT_D32_SFLOAT:
            return "VK_FORMAT_D32_SFLOAT";
        case VK_FORMAT_S8_UINT:
            return "VK_FORMAT_S8_UINT";
        case VK_FORMAT_D16_UNORM_S8_UINT:
            return "VK_FORMAT_D16_UNORM_S8_UINT";
        case VK_FORMAT_D24_UNORM_S8_UINT:
            return "VK_FORMAT_D24_UNORM_S8_UINT";
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return "VK_FORMAT_D32_SFLOAT_S8_UINT";
        case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
            return "VK_FORMAT_BC1_RGB_UNORM_BLOCK";
        case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
            return "VK_FORMAT_BC1_RGB_SRGB_BLOCK";
        case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
            return "VK_FORMAT_BC1_RGBA_UNORM_BLOCK";
        case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
            return "VK_FORMAT_BC1_RGBA_SRGB_BLOCK";
        case VK_FORMAT_BC2_UNORM_BLOCK:
            return "VK_FORMAT_BC2_UNORM_BLOCK";
        case VK_FORMAT_BC2_SRGB_BLOCK:
            return "VK_FORMAT_BC2_SRGB_BLOCK";
        case VK_FORMAT_BC3_UNORM_BLOCK:
            return "VK_FORMAT_BC3_UNORM_BLOCK";
        case VK_FORMAT_BC3_SRGB_BLOCK:
            return "VK_FORMAT_BC3_SRGB_BLOCK";
        case VK_FORMAT_BC4_UNORM_BLOCK:
            return "VK_FORMAT_BC4_UNORM_BLOCK";
        case VK_FORMAT_BC4_SNORM_BLOCK:
            return "VK_FORMAT_BC4_SNORM_BLOCK";
        case VK_FORMAT_BC5_UNORM_BLOCK:
            return "VK_FORMAT_BC5_UNORM_BLOCK";
        case VK_FORMAT_BC5_SNORM_BLOCK:
            return "VK_FORMAT_BC5_SNORM_BLOCK";
        case VK_FORMAT_BC6H_UFLOAT_BLOCK:
            return "VK_FORMAT_BC6H_UFLOAT_BLOCK";
        case VK_FORMAT_BC6H_SFLOAT_BLOCK:
            return "VK_FORMAT_BC6H_SFLOAT_BLOCK";
        case VK_FORMAT_BC7_UNORM_BLOCK:
            return "VK_FORMAT_BC7_UNORM_BLOCK";
        case VK_FORMAT_BC7_SRGB_BLOCK:
            return "VK_FORMAT_BC7_SRGB_BLOCK";
        case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
            return "VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK";
        case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
            return "VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK";
        case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
            return "VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK";
        case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
            return "VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK";
        case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
            return "VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK";
        case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
            return "VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK";
        case VK_FORMAT_EAC_R11_UNORM_BLOCK:
            return "VK_FORMAT_EAC_R11_UNORM_BLOCK";
        case VK_FORMAT_EAC_R11_SNORM_BLOCK:
            return "VK_FORMAT_EAC_R11_SNORM_BLOCK";
        case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
            return "VK_FORMAT_EAC_R11G11_UNORM_BLOCK";
        case VK_FORMAT_EAC_R11G11_SNORM_BLOCK:
            return "VK_FORMAT_EAC_R11G11_SNORM_BLOCK";
        case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_4x4_UNORM_BLOCK";
        case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_4x4_SRGB_BLOCK";
        case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_5x4_UNORM_BLOCK";
        case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_5x4_SRGB_BLOCK";
        case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_5x5_UNORM_BLOCK";
        case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_5x5_SRGB_BLOCK";
        case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_6x5_UNORM_BLOCK";
        case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_6x5_SRGB_BLOCK";
        case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_6x6_UNORM_BLOCK";
        case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_6x6_SRGB_BLOCK";
        case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_8x5_UNORM_BLOCK";
        case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_8x5_SRGB_BLOCK";
        case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_8x6_UNORM_BLOCK";
        case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_8x6_SRGB_BLOCK";
        case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_8x8_UNORM_BLOCK";
        case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_8x8_SRGB_BLOCK";
        case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_10x5_UNORM_BLOCK";
        case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_10x5_SRGB_BLOCK";
        case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_10x6_UNORM_BLOCK";
        case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_10x6_SRGB_BLOCK";
        case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_10x8_UNORM_BLOCK";
        case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_10x8_SRGB_BLOCK";
        case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_10x10_UNORM_BLOCK";
        case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_10x10_SRGB_BLOCK";
        case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_12x10_UNORM_BLOCK";
        case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_12x10_SRGB_BLOCK";
        case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_12x12_UNORM_BLOCK";
        case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_12x12_SRGB_BLOCK";
        case VK_FORMAT_G8B8G8R8_422_UNORM:
            return "VK_FORMAT_G8B8G8R8_422_UNORM";
        case VK_FORMAT_B8G8R8G8_422_UNORM:
            return "VK_FORMAT_B8G8R8G8_422_UNORM";
        case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
            return "VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM";
        case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:
            return "VK_FORMAT_G8_B8R8_2PLANE_420_UNORM";
        case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM:
            return "VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM";
        case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM:
            return "VK_FORMAT_G8_B8R8_2PLANE_422_UNORM";
        case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM:
            return "VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM";
        case VK_FORMAT_R10X6_UNORM_PACK16:
            return "VK_FORMAT_R10X6_UNORM_PACK16";
        case VK_FORMAT_R10X6G10X6_UNORM_2PACK16:
            return "VK_FORMAT_R10X6G10X6_UNORM_2PACK16";
        case VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16:
            return "VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16";
        case VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16:
            return "VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16";
        case VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16:
            return "VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16";
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
            return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16";
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
            return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16";
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
            return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16";
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
            return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16";
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
            return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16";
        case VK_FORMAT_R12X4_UNORM_PACK16:
            return "VK_FORMAT_R12X4_UNORM_PACK16";
        case VK_FORMAT_R12X4G12X4_UNORM_2PACK16:
            return "VK_FORMAT_R12X4G12X4_UNORM_2PACK16";
        case VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16:
            return "VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16";
        case VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16:
            return "VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16";
        case VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16:
            return "VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16";
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
            return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16";
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
            return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16";
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
            return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16";
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
            return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16";
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
            return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16";
        case VK_FORMAT_G16B16G16R16_422_UNORM:
            return "VK_FORMAT_G16B16G16R16_422_UNORM";
        case VK_FORMAT_B16G16R16G16_422_UNORM:
            return "VK_FORMAT_B16G16R16G16_422_UNORM";
        case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM:
            return "VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM";
        case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM:
            return "VK_FORMAT_G16_B16R16_2PLANE_420_UNORM";
        case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM:
            return "VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM";
        case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM:
            return "VK_FORMAT_G16_B16R16_2PLANE_422_UNORM";
        case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM:
            return "VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM";
        case VK_FORMAT_G8_B8R8_2PLANE_444_UNORM:
            return "VK_FORMAT_G8_B8R8_2PLANE_444_UNORM";
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16:
            return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16";
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16:
            return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16";
        case VK_FORMAT_G16_B16R16_2PLANE_444_UNORM:
            return "VK_FORMAT_G16_B16R16_2PLANE_444_UNORM";
        case VK_FORMAT_A4R4G4B4_UNORM_PACK16:
            return "VK_FORMAT_A4R4G4B4_UNORM_PACK16";
        case VK_FORMAT_A4B4G4R4_UNORM_PACK16:
            return "VK_FORMAT_A4B4G4R4_UNORM_PACK16";
        case VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK:
            return "VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK";
        case VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK:
            return "VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK";
        case VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK:
            return "VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK";
        case VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK:
            return "VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK";
        case VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK:
            return "VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK";
        case VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK:
            return "VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK";
        case VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK:
            return "VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK";
        case VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK:
            return "VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK";
        case VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK:
            return "VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK";
        case VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK:
            return "VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK";
        case VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK:
            return "VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK";
        case VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK:
            return "VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK";
        case VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK:
            return "VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK";
        case VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK:
            return "VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK";
        case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG:
            return "VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG";
        case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG:
            return "VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG";
        case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG:
            return "VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG";
        case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG:
            return "VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG";
        case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG:
            return "VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG";
        case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG:
            return "VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG";
        case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG:
            return "VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG";
        case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG:
            return "VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG";
        default:
            return "VK_FORMAT_UNDEFINED";
    }
}

static VkFormat StringToFormat(const std::string &input_value) {
    static const std::unordered_map<std::string, VkFormat> map = {
        {"VK_FORMAT_UNDEFINED", VK_FORMAT_UNDEFINED},
        {"VK_FORMAT_R4G4_UNORM_PACK8", VK_FORMAT_R4G4_UNORM_PACK8},
        {"VK_FORMAT_R4G4B4A4_UNORM_PACK16", VK_FORMAT_R4G4B4A4_UNORM_PACK16},
        {"VK_FORMAT_B4G4R4A4_UNORM_PACK16", VK_FORMAT_B4G4R4A4_UNORM_PACK16},
        {"VK_FORMAT_R5G6B5_UNORM_PACK16", VK_FORMAT_R5G6B5_UNORM_PACK16},
        {"VK_FORMAT_B5G6R5_UNORM_PACK16", VK_FORMAT_B5G6R5_UNORM_PACK16},
        {"VK_FORMAT_R5G5B5A1_UNORM_PACK16", VK_FORMAT_R5G5B5A1_UNORM_PACK16},
        {"VK_FORMAT_B5G5R5A1_UNORM_PACK16", VK_FORMAT_B5G5R5A1_UNORM_PACK16},
        {"VK_FORMAT_A1R5G5B5_UNORM_PACK16", VK_FORMAT_A1R5G5B5_UNORM_PACK16},
        {"VK_FORMAT_R8_UNORM", VK_FORMAT_R8_UNORM},
        {"VK_FORMAT_R8_SNORM", VK_FORMAT_R8_SNORM},
        {"VK_FORMAT_R8_USCALED", VK_FORMAT_R8_USCALED},
        {"VK_FORMAT_R8_SSCALED", VK_FORMAT_R8_SSCALED},
        {"VK_FORMAT_R8_UINT", VK_FORMAT_R8_UINT},
        {"VK_FORMAT_R8_SINT", VK_FORMAT_R8_SINT},
        {"VK_FORMAT_R8_SRGB", VK_FORMAT_R8_SRGB},
        {"VK_FORMAT_R8G8_UNORM", VK_FORMAT_R8G8_UNORM},
        {"VK_FORMAT_R8G8_SNORM", VK_FORMAT_R8G8_SNORM},
        {"VK_FORMAT_R8G8_USCALED", VK_FORMAT_R8G8_USCALED},
        {"VK_FORMAT_R8G8_SSCALED", VK_FORMAT_R8G8_SSCALED},
        {"VK_FORMAT_R8G8_UINT", VK_FORMAT_R8G8_UINT},
        {"VK_FORMAT_R8G8_SINT", VK_FORMAT_R8G8_SINT},
        {"VK_FORMAT_R8G8_SRGB", VK_FORMAT_R8G8_SRGB},
        {"VK_FORMAT_R8G8B8_UNORM", VK_FORMAT_R8G8B8_UNORM},
        {"VK_FORMAT_R8G8B8_SNORM", VK_FORMAT_R8G8B8_SNORM},
        {"VK_FORMAT_R8G8B8_USCALED", VK_FORMAT_R8G8B8_USCALED},
        {"VK_FORMAT_R8G8B8_SSCALED", VK_FORMAT_R8G8B8_SSCALED},
        {"VK_FORMAT_R8G8B8_UINT", VK_FORMAT_R8G8B8_UINT},
        {"VK_FORMAT_R8G8B8_SINT", VK_FORMAT_R8G8B8_SINT},
        {"VK_FORMAT_R8G8B8_SRGB", VK_FORMAT_R8G8B8_SRGB},
        {"VK_FORMAT_B8G8R8_UNORM", VK_FORMAT_B8G8R8_UNORM},
        {"VK_FORMAT_B8G8R8_SNORM", VK_FORMAT_B8G8R8_SNORM},
        {"VK_FORMAT_B8G8R8_USCALED", VK_FORMAT_B8G8R8_USCALED},
        {"VK_FORMAT_B8G8R8_SSCALED", VK_FORMAT_B8G8R8_SSCALED},
        {"VK_FORMAT_B8G8R8_UINT", VK_FORMAT_B8G8R8_UINT},
        {"VK_FORMAT_B8G8R8_SINT", VK_FORMAT_B8G8R8_SINT},
        {"VK_FORMAT_B8G8R8_SRGB", VK_FORMAT_B8G8R8_SRGB},
        {"VK_FORMAT_R8G8B8A8_UNORM", VK_FORMAT_R8G8B8A8_UNORM},
        {"VK_FORMAT_R8G8B8A8_SNORM", VK_FORMAT_R8G8B8A8_SNORM},
        {"VK_FORMAT_R8G8B8A8_USCALED", VK_FORMAT_R8G8B8A8_USCALED},
        {"VK_FORMAT_R8G8B8A8_SSCALED", VK_FORMAT_R8G8B8A8_SSCALED},
        {"VK_FORMAT_R8G8B8A8_UINT", VK_FORMAT_R8G8B8A8_UINT},
        {"VK_FORMAT_R8G8B8A8_SINT", VK_FORMAT_R8G8B8A8_SINT},
        {"VK_FORMAT_R8G8B8A8_SRGB", VK_FORMAT_R8G8B8A8_SRGB},
        {"VK_FORMAT_B8G8R8A8_UNORM", VK_FORMAT_B8G8R8A8_UNORM},
        {"VK_FORMAT_B8G8R8A8_SNORM", VK_FORMAT_B8G8R8A8_SNORM},
        {"VK_FORMAT_B8G8R8A8_USCALED", VK_FORMAT_B8G8R8A8_USCALED},
        {"VK_FORMAT_B8G8R8A8_SSCALED", VK_FORMAT_B8G8R8A8_SSCALED},
        {"VK_FORMAT_B8G8R8A8_UINT", VK_FORMAT_B8G8R8A8_UINT},
        {"VK_FORMAT_B8G8R8A8_SINT", VK_FORMAT_B8G8R8A8_SINT},
        {"VK_FORMAT_B8G8R8A8_SRGB", VK_FORMAT_B8G8R8A8_SRGB},
        {"VK_FORMAT_A8B8G8R8_UNORM_PACK32", VK_FORMAT_A8B8G8R8_UNORM_PACK32},
        {"VK_FORMAT_A8B8G8R8_SNORM_PACK32", VK_FORMAT_A8B8G8R8_SNORM_PACK32},
        {"VK_FORMAT_A8B8G8R8_USCALED_PACK32", VK_FORMAT_A8B8G8R8_USCALED_PACK32},
        {"VK_FORMAT_A8B8G8R8_SSCALED_PACK32", VK_FORMAT_A8B8G8R8_SSCALED_PACK32},
        {"VK_FORMAT_A8B8G8R8_UINT_PACK32", VK_FORMAT_A8B8G8R8_UINT_PACK32},
        {"VK_FORMAT_A8B8G8R8_SINT_PACK32", VK_FORMAT_A8B8G8R8_SINT_PACK32},
        {"VK_FORMAT_A8B8G8R8_SRGB_PACK32", VK_FORMAT_A8B8G8R8_SRGB_PACK32},
        {"VK_FORMAT_A2R10G10B10_UNORM_PACK32", VK_FORMAT_A2R10G10B10_UNORM_PACK32},
        {"VK_FORMAT_A2R10G10B10_SNORM_PACK32", VK_FORMAT_A2R10G10B10_SNORM_PACK32},
        {"VK_FORMAT_A2R10G10B10_USCALED_PACK32", VK_FORMAT_A2R10G10B10_USCALED_PACK32},
        {"VK_FORMAT_A2R10G10B10_SSCALED_PACK32", VK_FORMAT_A2R10G10B10_SSCALED_PACK32},
        {"VK_FORMAT_A2R10G10B10_UINT_PACK32", VK_FORMAT_A2R10G10B10_UINT_PACK32},
        {"VK_FORMAT_A2R10G10B10_SINT_PACK32", VK_FORMAT_A2R10G10B10_SINT_PACK32},
        {"VK_FORMAT_A2B10G10R10_UNORM_PACK32", VK_FORMAT_A2B10G10R10_UNORM_PACK32},
        {"VK_FORMAT_A2B10G10R10_SNORM_PACK32", VK_FORMAT_A2B10G10R10_SNORM_PACK32},
        {"VK_FORMAT_A2B10G10R10_USCALED_PACK32", VK_FORMAT_A2B10G10R10_USCALED_PACK32},
        {"VK_FORMAT_A2B10G10R10_SSCALED_PACK32", VK_FORMAT_A2B10G10R10_SSCALED_PACK32},
        {"VK_FORMAT_A2B10G10R10_UINT_PACK32", VK_FORMAT_A2B10G10R10_UINT_PACK32},
        {"VK_FORMAT_A2B10G10R10_SINT_PACK32", VK_FORMAT_A2B10G10R10_SINT_PACK32},
        {"VK_FORMAT_R16_UNORM", VK_FORMAT_R16_UNORM},
        {"VK_FORMAT_R16_SNORM", VK_FORMAT_R16_SNORM},
        {"VK_FORMAT_R16_USCALED", VK_FORMAT_R16_USCALED},
        {"VK_FORMAT_R16_SSCALED", VK_FORMAT_R16_SSCALED},
        {"VK_FORMAT_R16_UINT", VK_FORMAT_R16_UINT},
        {"VK_FORMAT_R16_SINT", VK_FORMAT_R16_SINT},
        {"VK_FORMAT_R16_SFLOAT", VK_FORMAT_R16_SFLOAT},
        {"VK_FORMAT_R16G16_UNORM", VK_FORMAT_R16G16_UNORM},
        {"VK_FORMAT_R16G16_SNORM", VK_FORMAT_R16G16_SNORM},
        {"VK_FORMAT_R16G16_USCALED", VK_FORMAT_R16G16_USCALED},
        {"VK_FORMAT_R16G16_SSCALED", VK_FORMAT_R16G16_SSCALED},
        {"VK_FORMAT_R16G16_UINT", VK_FORMAT_R16G16_UINT},
        {"VK_FORMAT_R16G16_SINT", VK_FORMAT_R16G16_SINT},
        {"VK_FORMAT_R16G16_SFLOAT", VK_FORMAT_R16G16_SFLOAT},
        {"VK_FORMAT_R16G16B16_UNORM", VK_FORMAT_R16G16B16_UNORM},
        {"VK_FORMAT_R16G16B16_SNORM", VK_FORMAT_R16G16B16_SNORM},
        {"VK_FORMAT_R16G16B16_USCALED", VK_FORMAT_R16G16B16_USCALED},
        {"VK_FORMAT_R16G16B16_SSCALED", VK_FORMAT_R16G16B16_SSCALED},
        {"VK_FORMAT_R16G16B16_UINT", VK_FORMAT_R16G16B16_UINT},
        {"VK_FORMAT_R16G16B16_SINT", VK_FORMAT_R16G16B16_SINT},
        {"VK_FORMAT_R16G16B16_SFLOAT", VK_FORMAT_R16G16B16_SFLOAT},
        {"VK_FORMAT_R16G16B16A16_UNORM", VK_FORMAT_R16G16B16A16_UNORM},
        {"VK_FORMAT_R16G16B16A16_SNORM", VK_FORMAT_R16G16B16A16_SNORM},
        {"VK_FORMAT_R16G16B16A16_USCALED", VK_FORMAT_R16G16B16A16_USCALED},
        {"VK_FORMAT_R16G16B16A16_SSCALED", VK_FORMAT_R16G16B16A16_SSCALED},
        {"VK_FORMAT_R16G16B16A16_UINT", VK_FORMAT_R16G16B16A16_UINT},
        {"VK_FORMAT_R16G16B16A16_SINT", VK_FORMAT_R16G16B16A16_SINT},
        {"VK_FORMAT_R16G16B16A16_SFLOAT", VK_FORMAT_R16G16B16A16_SFLOAT},
        {"VK_FORMAT_R32_UINT", VK_FORMAT_R32_UINT},
        {"VK_FORMAT_R32_SINT", VK_FORMAT_R32_SINT},
        {"VK_FORMAT_R32_SFLOAT", VK_FORMAT_R32_SFLOAT},
        {"VK_FORMAT_R32G32_UINT", VK_FORMAT_R32G32_UINT},
        {"VK_FORMAT_R32G32_SINT", VK_FORMAT_R32G32_SINT},
        {"VK_FORMAT_R32G32_SFLOAT", VK_FORMAT_R32G32_SFLOAT},
        {"VK_FORMAT_R32G32B32_UINT", VK_FORMAT_R32G32B32_UINT},
        {"VK_FORMAT_R32G32B32_SINT", VK_FORMAT_R32G32B32_SINT},
        {"VK_FORMAT_R32G32B32_SFLOAT", VK_FORMAT_R32G32B32_SFLOAT},
        {"VK_FORMAT_R32G32B32A32_UINT", VK_FORMAT_R32G32B32A32_UINT},
        {"VK_FORMAT_R32G32B32A32_SINT", VK_FORMAT_R32G32B32A32_SINT},
        {"VK_FORMAT_R32G32B32A32_SFLOAT", VK_FORMAT_R32G32B32A32_SFLOAT},
        {"VK_FORMAT_R64_UINT", VK_FORMAT_R64_UINT},
        {"VK_FORMAT_R64_SINT", VK_FORMAT_R64_SINT},
        {"VK_FORMAT_R64_SFLOAT", VK_FORMAT_R64_SFLOAT},
        {"VK_FORMAT_R64G64_UINT", VK_FORMAT_R64G64_UINT},
        {"VK_FORMAT_R64G64_SINT", VK_FORMAT_R64G64_SINT},
        {"VK_FORMAT_R64G64_SFLOAT", VK_FORMAT_R64G64_SFLOAT},
        {"VK_FORMAT_R64G64B64_UINT", VK_FORMAT_R64G64B64_UINT},
        {"VK_FORMAT_R64G64B64_SINT", VK_FORMAT_R64G64B64_SINT},
        {"VK_FORMAT_R64G64B64_SFLOAT", VK_FORMAT_R64G64B64_SFLOAT},
        {"VK_FORMAT_R64G64B64A64_UINT", VK_FORMAT_R64G64B64A64_UINT},
        {"VK_FORMAT_R64G64B64A64_SINT", VK_FORMAT_R64G64B64A64_SINT},
        {"VK_FORMAT_R64G64B64A64_SFLOAT", VK_FORMAT_R64G64B64A64_SFLOAT},
        {"VK_FORMAT_B10G11R11_UFLOAT_PACK32", VK_FORMAT_B10G11R11_UFLOAT_PACK32},
        {"VK_FORMAT_E5B9G9R9_UFLOAT_PACK32", VK_FORMAT_E5B9G9R9_UFLOAT_PACK32},
        {"VK_FORMAT_D16_UNORM", VK_FORMAT_D16_UNORM},
        {"VK_FORMAT_X8_D24_UNORM_PACK32", VK_FORMAT_X8_D24_UNORM_PACK32},
        {"VK_FORMAT_D32_SFLOAT", VK_FORMAT_D32_SFLOAT},
        {"VK_FORMAT_S8_UINT", VK_FORMAT_S8_UINT},
        {"VK_FORMAT_D16_UNORM_S8_UINT", VK_FORMAT_D16_UNORM_S8_UINT},
        {"VK_FORMAT_D24_UNORM_S8_UINT", VK_FORMAT_D24_UNORM_S8_UINT},
        {"VK_FORMAT_D32_SFLOAT_S8_UINT", VK_FORMAT_D32_SFLOAT_S8_UINT},
        {"VK_FORMAT_BC1_RGB_UNORM_BLOCK", VK_FORMAT_BC1_RGB_UNORM_BLOCK},
        {"VK_FORMAT_BC1_RGB_SRGB_BLOCK", VK_FORMAT_BC1_RGB_SRGB_BLOCK},
        {"VK_FORMAT_BC1_RGBA_UNORM_BLOCK", VK_FORMAT_BC1_RGBA_UNORM_BLOCK},
        {"VK_FORMAT_BC1_RGBA_SRGB_BLOCK", VK_FORMAT_BC1_RGBA_SRGB_BLOCK},
        {"VK_FORMAT_BC2_UNORM_BLOCK", VK_FORMAT_BC2_UNORM_BLOCK},
        {"VK_FORMAT_BC2_SRGB_BLOCK", VK_FORMAT_BC2_SRGB_BLOCK},
        {"VK_FORMAT_BC3_UNORM_BLOCK", VK_FORMAT_BC3_UNORM_BLOCK},
        {"VK_FORMAT_BC3_SRGB_BLOCK", VK_FORMAT_BC3_SRGB_BLOCK},
        {"VK_FORMAT_BC4_UNORM_BLOCK", VK_FORMAT_BC4_UNORM_BLOCK},
        {"VK_FORMAT_BC4_SNORM_BLOCK", VK_FORMAT_BC4_SNORM_BLOCK},
        {"VK_FORMAT_BC5_UNORM_BLOCK", VK_FORMAT_BC5_UNORM_BLOCK},
        {"VK_FORMAT_BC5_SNORM_BLOCK", VK_FORMAT_BC5_SNORM_BLOCK},
        {"VK_FORMAT_BC6H_UFLOAT_BLOCK", VK_FORMAT_BC6H_UFLOAT_BLOCK},
        {"VK_FORMAT_BC6H_SFLOAT_BLOCK", VK_FORMAT_BC6H_SFLOAT_BLOCK},
        {"VK_FORMAT_BC7_UNORM_BLOCK", VK_FORMAT_BC7_UNORM_BLOCK},
        {"VK_FORMAT_BC7_SRGB_BLOCK", VK_FORMAT_BC7_SRGB_BLOCK},
        {"VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK", VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK},
        {"VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK", VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK},
        {"VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK", VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK},
        {"VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK", VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK},
        {"VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK", VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK},
        {"VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK", VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK},
        {"VK_FORMAT_EAC_R11_UNORM_BLOCK", VK_FORMAT_EAC_R11_UNORM_BLOCK},
        {"VK_FORMAT_EAC_R11_SNORM_BLOCK", VK_FORMAT_EAC_R11_SNORM_BLOCK},
        {"VK_FORMAT_EAC_R11G11_UNORM_BLOCK", VK_FORMAT_EAC_R11G11_UNORM_BLOCK},
        {"VK_FORMAT_EAC_R11G11_SNORM_BLOCK", VK_FORMAT_EAC_R11G11_SNORM_BLOCK},
        {"VK_FORMAT_ASTC_4x4_UNORM_BLOCK", VK_FORMAT_ASTC_4x4_UNORM_BLOCK},
        {"VK_FORMAT_ASTC_4x4_SRGB_BLOCK", VK_FORMAT_ASTC_4x4_SRGB_BLOCK},
        {"VK_FORMAT_ASTC_5x4_UNORM_BLOCK", VK_FORMAT_ASTC_5x4_UNORM_BLOCK},
        {"VK_FORMAT_ASTC_5x4_SRGB_BLOCK", VK_FORMAT_ASTC_5x4_SRGB_BLOCK},
        {"VK_FORMAT_ASTC_5x5_UNORM_BLOCK", VK_FORMAT_ASTC_5x5_UNORM_BLOCK},
        {"VK_FORMAT_ASTC_5x5_SRGB_BLOCK", VK_FORMAT_ASTC_5x5_SRGB_BLOCK},
        {"VK_FORMAT_ASTC_6x5_UNORM_BLOCK", VK_FORMAT_ASTC_6x5_UNORM_BLOCK},
        {"VK_FORMAT_ASTC_6x5_SRGB_BLOCK", VK_FORMAT_ASTC_6x5_SRGB_BLOCK},
        {"VK_FORMAT_ASTC_6x6_UNORM_BLOCK", VK_FORMAT_ASTC_6x6_UNORM_BLOCK},
        {"VK_FORMAT_ASTC_6x6_SRGB_BLOCK", VK_FORMAT_ASTC_6x6_SRGB_BLOCK},
        {"VK_FORMAT_ASTC_8x5_UNORM_BLOCK", VK_FORMAT_ASTC_8x5_UNORM_BLOCK},
        {"VK_FORMAT_ASTC_8x5_SRGB_BLOCK", VK_FORMAT_ASTC_8x5_SRGB_BLOCK},
        {"VK_FORMAT_ASTC_8x6_UNORM_BLOCK", VK_FORMAT_ASTC_8x6_UNORM_BLOCK},
        {"VK_FORMAT_ASTC_8x6_SRGB_BLOCK", VK_FORMAT_ASTC_8x6_SRGB_BLOCK},
        {"VK_FORMAT_ASTC_8x8_UNORM_BLOCK", VK_FORMAT_ASTC_8x8_UNORM_BLOCK},
        {"VK_FORMAT_ASTC_8x8_SRGB_BLOCK", VK_FORMAT_ASTC_8x8_SRGB_BLOCK},
        {"VK_FORMAT_ASTC_10x5_UNORM_BLOCK", VK_FORMAT_ASTC_10x5_UNORM_BLOCK},
        {"VK_FORMAT_ASTC_10x5_SRGB_BLOCK", VK_FORMAT_ASTC_10x5_SRGB_BLOCK},
        {"VK_FORMAT_ASTC_10x6_UNORM_BLOCK", VK_FORMAT_ASTC_10x6_UNORM_BLOCK},
        {"VK_FORMAT_ASTC_10x6_SRGB_BLOCK", VK_FORMAT_ASTC_10x6_SRGB_BLOCK},
        {"VK_FORMAT_ASTC_10x8_UNORM_BLOCK", VK_FORMAT_ASTC_10x8_UNORM_BLOCK},
        {"VK_FORMAT_ASTC_10x8_SRGB_BLOCK", VK_FORMAT_ASTC_10x8_SRGB_BLOCK},
        {"VK_FORMAT_ASTC_10x10_UNORM_BLOCK", VK_FORMAT_ASTC_10x10_UNORM_BLOCK},
        {"VK_FORMAT_ASTC_10x10_SRGB_BLOCK", VK_FORMAT_ASTC_10x10_SRGB_BLOCK},
        {"VK_FORMAT_ASTC_12x10_UNORM_BLOCK", VK_FORMAT_ASTC_12x10_UNORM_BLOCK},
        {"VK_FORMAT_ASTC_12x10_SRGB_BLOCK", VK_FORMAT_ASTC_12x10_SRGB_BLOCK},
        {"VK_FORMAT_ASTC_12x12_UNORM_BLOCK", VK_FORMAT_ASTC_12x12_UNORM_BLOCK},
        {"VK_FORMAT_ASTC_12x12_SRGB_BLOCK", VK_FORMAT_ASTC_12x12_SRGB_BLOCK},
        {"VK_FORMAT_G8B8G8R8_422_UNORM", VK_FORMAT_G8B8G8R8_422_UNORM},
        {"VK_FORMAT_B8G8R8G8_422_UNORM", VK_FORMAT_B8G8R8G8_422_UNORM},
        {"VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM", VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM},
        {"VK_FORMAT_G8_B8R8_2PLANE_420_UNORM", VK_FORMAT_G8_B8R8_2PLANE_420_UNORM},
        {"VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM", VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM},
        {"VK_FORMAT_G8_B8R8_2PLANE_422_UNORM", VK_FORMAT_G8_B8R8_2PLANE_422_UNORM},
        {"VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM", VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM},
        {"VK_FORMAT_R10X6_UNORM_PACK16", VK_FORMAT_R10X6_UNORM_PACK16},
        {"VK_FORMAT_R10X6G10X6_UNORM_2PACK16", VK_FORMAT_R10X6G10X6_UNORM_2PACK16},
        {"VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16", VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16},
        {"VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16", VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16},
        {"VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16", VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16},
        {"VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16", VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16},
        {"VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16", VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16},
        {"VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16", VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16},
        {"VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16", VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16},
        {"VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16", VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16},
        {"VK_FORMAT_R12X4_UNORM_PACK16", VK_FORMAT_R12X4_UNORM_PACK16},
        {"VK_FORMAT_R12X4G12X4_UNORM_2PACK16", VK_FORMAT_R12X4G12X4_UNORM_2PACK16},
        {"VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16", VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16},
        {"VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16", VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16},
        {"VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16", VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16},
        {"VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16", VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16},
        {"VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16", VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16},
        {"VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16", VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16},
        {"VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16", VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16},
        {"VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16", VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16},
        {"VK_FORMAT_G16B16G16R16_422_UNORM", VK_FORMAT_G16B16G16R16_422_UNORM},
        {"VK_FORMAT_B16G16R16G16_422_UNORM", VK_FORMAT_B16G16R16G16_422_UNORM},
        {"VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM", VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM},
        {"VK_FORMAT_G16_B16R16_2PLANE_420_UNORM", VK_FORMAT_G16_B16R16_2PLANE_420_UNORM},
        {"VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM", VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM},
        {"VK_FORMAT_G16_B16R16_2PLANE_422_UNORM", VK_FORMAT_G16_B16R16_2PLANE_422_UNORM},
        {"VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM", VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM},
        {"VK_FORMAT_G8_B8R8_2PLANE_444_UNORM", VK_FORMAT_G8_B8R8_2PLANE_444_UNORM},
        {"VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16", VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16},
        {"VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16", VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16},
        {"VK_FORMAT_G16_B16R16_2PLANE_444_UNORM", VK_FORMAT_G16_B16R16_2PLANE_444_UNORM},
        {"VK_FORMAT_A4R4G4B4_UNORM_PACK16", VK_FORMAT_A4R4G4B4_UNORM_PACK16},
        {"VK_FORMAT_A4B4G4R4_UNORM_PACK16", VK_FORMAT_A4B4G4R4_UNORM_PACK16},
        {"VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK", VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK},
        {"VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK", VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK},
        {"VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK", VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK},
        {"VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK", VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK},
        {"VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK", VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK},
        {"VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK", VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK},
        {"VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK", VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK},
        {"VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK", VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK},
        {"VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK", VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK},
        {"VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK", VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK},
        {"VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK", VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK},
        {"VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK", VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK},
        {"VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK", VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK},
        {"VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK", VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK},
        {"VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG", VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG},
        {"VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG", VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG},
        {"VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG", VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG},
        {"VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG", VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG},
        {"VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG", VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG},
        {"VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG", VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG},
        {"VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG", VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG},
        {"VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG", VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG},
        {"VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT", VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT},
        {"VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT", VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT},
        {"VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT", VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT},
        {"VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT", VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT},
        {"VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT", VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT},
        {"VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT", VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT},
        {"VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT", VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT},
        {"VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT", VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT},
        {"VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT", VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT},
        {"VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT", VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT},
        {"VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT", VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT},
        {"VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT", VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT},
        {"VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT", VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT},
        {"VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT", VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT},
        {"VK_FORMAT_G8B8G8R8_422_UNORM_KHR", VK_FORMAT_G8B8G8R8_422_UNORM_KHR},
        {"VK_FORMAT_B8G8R8G8_422_UNORM_KHR", VK_FORMAT_B8G8R8G8_422_UNORM_KHR},
        {"VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM_KHR", VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM_KHR},
        {"VK_FORMAT_G8_B8R8_2PLANE_420_UNORM_KHR", VK_FORMAT_G8_B8R8_2PLANE_420_UNORM_KHR},
        {"VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM_KHR", VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM_KHR},
        {"VK_FORMAT_G8_B8R8_2PLANE_422_UNORM_KHR", VK_FORMAT_G8_B8R8_2PLANE_422_UNORM_KHR},
        {"VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM_KHR", VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM_KHR},
        {"VK_FORMAT_R10X6_UNORM_PACK16_KHR", VK_FORMAT_R10X6_UNORM_PACK16_KHR},
        {"VK_FORMAT_R10X6G10X6_UNORM_2PACK16_KHR", VK_FORMAT_R10X6G10X6_UNORM_2PACK16_KHR},
        {"VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16_KHR", VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16_KHR},
        {"VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16_KHR", VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16_KHR},
        {"VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16_KHR", VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16_KHR},
        {"VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16_KHR", VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16_KHR},
        {"VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16_KHR", VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16_KHR},
        {"VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16_KHR", VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16_KHR},
        {"VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16_KHR", VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16_KHR},
        {"VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16_KHR", VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16_KHR},
        {"VK_FORMAT_R12X4_UNORM_PACK16_KHR", VK_FORMAT_R12X4_UNORM_PACK16_KHR},
        {"VK_FORMAT_R12X4G12X4_UNORM_2PACK16_KHR", VK_FORMAT_R12X4G12X4_UNORM_2PACK16_KHR},
        {"VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16_KHR", VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16_KHR},
        {"VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16_KHR", VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16_KHR},
        {"VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16_KHR", VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16_KHR},
        {"VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16_KHR", VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16_KHR},
        {"VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16_KHR", VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16_KHR},
        {"VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16_KHR", VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16_KHR},
        {"VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16_KHR", VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16_KHR},
        {"VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16_KHR", VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16_KHR},
        {"VK_FORMAT_G16B16G16R16_422_UNORM_KHR", VK_FORMAT_G16B16G16R16_422_UNORM_KHR},
        {"VK_FORMAT_B16G16R16G16_422_UNORM_KHR", VK_FORMAT_B16G16R16G16_422_UNORM_KHR},
        {"VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM_KHR", VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM_KHR},
        {"VK_FORMAT_G16_B16R16_2PLANE_420_UNORM_KHR", VK_FORMAT_G16_B16R16_2PLANE_420_UNORM_KHR},
        {"VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM_KHR", VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM_KHR},
        {"VK_FORMAT_G16_B16R16_2PLANE_422_UNORM_KHR", VK_FORMAT_G16_B16R16_2PLANE_422_UNORM_KHR},
        {"VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM_KHR", VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM_KHR},
        {"VK_FORMAT_G8_B8R8_2PLANE_444_UNORM_EXT", VK_FORMAT_G8_B8R8_2PLANE_444_UNORM_EXT},
        {"VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT", VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT},
        {"VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT", VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT},
        {"VK_FORMAT_G16_B16R16_2PLANE_444_UNORM_EXT", VK_FORMAT_G16_B16R16_2PLANE_444_UNORM_EXT},
        {"VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT", VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT},
        {"VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT", VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT},
    };
    const auto it = map.find(input_value);
    if (it != map.end()) {
        return it->second;
    }
    return VK_FORMAT_UNDEFINED;
}

static uint32_t VkStringToUint(const std::string &input_value) {
    static const std::unordered_map<std::string, uint32_t> map = {
        // VkSampleCountFlagBits
        {"VK_SAMPLE_COUNT_1_BIT", VK_SAMPLE_COUNT_1_BIT},
        {"VK_SAMPLE_COUNT_2_BIT", VK_SAMPLE_COUNT_2_BIT},
        {"VK_SAMPLE_COUNT_4_BIT", VK_SAMPLE_COUNT_4_BIT},
        {"VK_SAMPLE_COUNT_8_BIT", VK_SAMPLE_COUNT_8_BIT},
        {"VK_SAMPLE_COUNT_16_BIT", VK_SAMPLE_COUNT_16_BIT},
        {"VK_SAMPLE_COUNT_32_BIT", VK_SAMPLE_COUNT_32_BIT},
        {"VK_SAMPLE_COUNT_64_BIT", VK_SAMPLE_COUNT_64_BIT},
        // VkResolveModeFlagBits
        {"VK_RESOLVE_MODE_NONE", VK_RESOLVE_MODE_NONE},
        {"VK_RESOLVE_MODE_SAMPLE_ZERO_BIT", VK_RESOLVE_MODE_SAMPLE_ZERO_BIT},
        {"VK_RESOLVE_MODE_AVERAGE_BIT", VK_RESOLVE_MODE_AVERAGE_BIT},
        {"VK_RESOLVE_MODE_MIN_BIT", VK_RESOLVE_MODE_MIN_BIT},
        {"VK_RESOLVE_MODE_MAX_BIT", VK_RESOLVE_MODE_MAX_BIT},
        {"VK_RESOLVE_MODE_NONE_KHR", VK_RESOLVE_MODE_NONE_KHR},
        {"VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR", VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR},
        {"VK_RESOLVE_MODE_AVERAGE_BIT_KHR", VK_RESOLVE_MODE_AVERAGE_BIT_KHR},
        {"VK_RESOLVE_MODE_MIN_BIT_KHR", VK_RESOLVE_MODE_MIN_BIT_KHR},
        {"VK_RESOLVE_MODE_MAX_BIT_KHR", VK_RESOLVE_MODE_MAX_BIT_KHR},
        // VkShaderStageFlagBits
        {"VK_SHADER_STAGE_VERTEX_BIT", VK_SHADER_STAGE_VERTEX_BIT},
        {"VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT", VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT},
        {"VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT", VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT},
        {"VK_SHADER_STAGE_GEOMETRY_BIT", VK_SHADER_STAGE_GEOMETRY_BIT},
        {"VK_SHADER_STAGE_FRAGMENT_BIT", VK_SHADER_STAGE_FRAGMENT_BIT},
        {"VK_SHADER_STAGE_COMPUTE_BIT", VK_SHADER_STAGE_COMPUTE_BIT},
        {"VK_SHADER_STAGE_ALL_GRAPHICS", VK_SHADER_STAGE_ALL_GRAPHICS},
        {"VK_SHADER_STAGE_ALL", VK_SHADER_STAGE_ALL},
        {"VK_SHADER_STAGE_RAYGEN_BIT_KHR", VK_SHADER_STAGE_RAYGEN_BIT_KHR},
        {"VK_SHADER_STAGE_ANY_HIT_BIT_KHR", VK_SHADER_STAGE_ANY_HIT_BIT_KHR},
        {"VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR", VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR},
        {"VK_SHADER_STAGE_MISS_BIT_KHR", VK_SHADER_STAGE_MISS_BIT_KHR},
        {"VK_SHADER_STAGE_INTERSECTION_BIT_KHR", VK_SHADER_STAGE_INTERSECTION_BIT_KHR},
        {"VK_SHADER_STAGE_CALLABLE_BIT_KHR", VK_SHADER_STAGE_CALLABLE_BIT_KHR},
        {"VK_SHADER_STAGE_TASK_BIT_NV", VK_SHADER_STAGE_TASK_BIT_NV},
        {"VK_SHADER_STAGE_MESH_BIT_NV", VK_SHADER_STAGE_MESH_BIT_NV},
        {"VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI", VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI},
        {"VK_SHADER_STAGE_RAYGEN_BIT_NV", VK_SHADER_STAGE_RAYGEN_BIT_NV},
        {"VK_SHADER_STAGE_ANY_HIT_BIT_NV", VK_SHADER_STAGE_ANY_HIT_BIT_NV},
        {"VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV", VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV},
        {"VK_SHADER_STAGE_MISS_BIT_NV", VK_SHADER_STAGE_MISS_BIT_NV},
        {"VK_SHADER_STAGE_INTERSECTION_BIT_NV", VK_SHADER_STAGE_INTERSECTION_BIT_NV},
        {"VK_SHADER_STAGE_CALLABLE_BIT_NV", VK_SHADER_STAGE_CALLABLE_BIT_NV},
        // VkSubgroupFeatureFlagBits
        {"VK_SUBGROUP_FEATURE_BASIC_BIT", VK_SUBGROUP_FEATURE_BASIC_BIT},
        {"VK_SUBGROUP_FEATURE_VOTE_BIT", VK_SUBGROUP_FEATURE_VOTE_BIT},
        {"VK_SUBGROUP_FEATURE_ARITHMETIC_BIT", VK_SUBGROUP_FEATURE_ARITHMETIC_BIT},
        {"VK_SUBGROUP_FEATURE_BALLOT_BIT", VK_SUBGROUP_FEATURE_BALLOT_BIT},
        {"VK_SUBGROUP_FEATURE_SHUFFLE_BIT", VK_SUBGROUP_FEATURE_SHUFFLE_BIT},
        {"VK_SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT", VK_SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT},
        {"VK_SUBGROUP_FEATURE_CLUSTERED_BIT", VK_SUBGROUP_FEATURE_CLUSTERED_BIT},
        {"VK_SUBGROUP_FEATURE_QUAD_BIT", VK_SUBGROUP_FEATURE_QUAD_BIT},
        {"VK_SUBGROUP_FEATURE_PARTITIONED_BIT_NV", VK_SUBGROUP_FEATURE_PARTITIONED_BIT_NV},
        // VkShaderFloatControlsIndependence
        {"VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_32_BIT_ONLY", VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_32_BIT_ONLY},
        {"VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL", VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL},
        {"VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_NONE", VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_NONE},
        {"VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_32_BIT_ONLY_KHR", VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_32_BIT_ONLY_KHR},
        {"VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL_KHR", VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL_KHR},
        {"VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_NONE_KHR", VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_NONE_KHR},
        // VkPointClippingBehavior
        {"VK_POINT_CLIPPING_BEHAVIOR_ALL_CLIP_PLANES", VK_POINT_CLIPPING_BEHAVIOR_ALL_CLIP_PLANES},
        {"VK_POINT_CLIPPING_BEHAVIOR_USER_CLIP_PLANES_ONLY", VK_POINT_CLIPPING_BEHAVIOR_USER_CLIP_PLANES_ONLY},
        {"VK_POINT_CLIPPING_BEHAVIOR_ALL_CLIP_PLANES_KHR", VK_POINT_CLIPPING_BEHAVIOR_ALL_CLIP_PLANES_KHR},
        {"VK_POINT_CLIPPING_BEHAVIOR_USER_CLIP_PLANES_ONLY_KHR", VK_POINT_CLIPPING_BEHAVIOR_USER_CLIP_PLANES_ONLY_KHR},
    };
    const auto it = map.find(input_value);
    if (it != map.end()) {
        return it->second;
    }
    return 0;
}

static VkFormatFeatureFlags StringToVkFormatFeatureFlags(const std::string &input_value) {
    static const std::unordered_map<std::string, VkFormatFeatureFlags> map = {
        {"VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT", VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT},
        {"VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT", VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT},
        {"VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT", VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT},
        {"VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT", VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT},
        {"VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT", VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT},
        {"VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT", VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT},
        {"VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT", VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT},
        {"VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT", VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT},
        {"VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT", VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT},
        {"VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT", VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT},
        {"VK_FORMAT_FEATURE_BLIT_SRC_BIT", VK_FORMAT_FEATURE_BLIT_SRC_BIT},
        {"VK_FORMAT_FEATURE_BLIT_DST_BIT", VK_FORMAT_FEATURE_BLIT_DST_BIT},
        {"VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT", VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT},
        {"VK_FORMAT_FEATURE_TRANSFER_SRC_BIT", VK_FORMAT_FEATURE_TRANSFER_SRC_BIT},
        {"VK_FORMAT_FEATURE_TRANSFER_DST_BIT", VK_FORMAT_FEATURE_TRANSFER_DST_BIT},
        {"VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT", VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT},
        {"VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT", VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT},
        {"VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT", VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT},
        {"VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT", VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT},
        {"VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT", VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT},
        {"VK_FORMAT_FEATURE_DISJOINT_BIT", VK_FORMAT_FEATURE_DISJOINT_BIT},
        {"VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT", VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT},
        {"VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT", VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT},
        {"VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR", VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR},
        {"VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR", VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR},
        {"VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR", VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR},
        {"VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT", VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT},
        {"VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT", VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT},
        {"VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR", VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR},
        {"VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR", VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR},
        {"VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR", VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR},
        {"VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG", VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG},
        {"VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR", VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR},
        {"VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR", VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR},
        {"VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT", VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT},
        {"VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR", VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR},
        {"VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR", VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR},
        {"VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR", VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR},
        {"VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR", VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR},
        {"VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR", VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR},
        {"VK_FORMAT_FEATURE_DISJOINT_BIT_KHR", VK_FORMAT_FEATURE_DISJOINT_BIT_KHR},
        {"VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR", VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR},
    };
    const auto it = map.find(input_value);
    if (it != map.end()) {
        return it->second;
    }
    return VkFormatFeatureFlags{};
}

static VkQueueFlags StringToVkQueueFlags(const std::string &input_value) {
    static const std::unordered_map<std::string, VkQueueFlags> map = {
        {"VK_QUEUE_GRAPHICS_BIT", VK_QUEUE_GRAPHICS_BIT},
        {"VK_QUEUE_COMPUTE_BIT", VK_QUEUE_COMPUTE_BIT},
        {"VK_QUEUE_TRANSFER_BIT", VK_QUEUE_TRANSFER_BIT},
        {"VK_QUEUE_SPARSE_BINDING_BIT", VK_QUEUE_SPARSE_BINDING_BIT},
        {"VK_QUEUE_PROTECTED_BIT", VK_QUEUE_PROTECTED_BIT},
        {"VK_QUEUE_VIDEO_DECODE_BIT_KHR", VK_QUEUE_VIDEO_DECODE_BIT_KHR},
        {"VK_QUEUE_VIDEO_ENCODE_BIT_KHR", VK_QUEUE_VIDEO_ENCODE_BIT_KHR},
    };
    const auto it = map.find(input_value);
    if (it != map.end()) {
        return it->second;
    }
    return VkQueueFlags{};
}

static VkQueueGlobalPriorityKHR StringToVkQueueGlobalPriorityKHR(const std::string &input_value) {
    static const std::unordered_map<std::string, VkQueueGlobalPriorityKHR> map = {
        {"VK_QUEUE_GLOBAL_PRIORITY_LOW_KHR", VK_QUEUE_GLOBAL_PRIORITY_LOW_KHR},
        {"VK_QUEUE_GLOBAL_PRIORITY_MEDIUM_KHR", VK_QUEUE_GLOBAL_PRIORITY_MEDIUM_KHR},
        {"VK_QUEUE_GLOBAL_PRIORITY_HIGH_KHR", VK_QUEUE_GLOBAL_PRIORITY_HIGH_KHR},
        {"VK_QUEUE_GLOBAL_PRIORITY_REALTIME_KHR", VK_QUEUE_GLOBAL_PRIORITY_REALTIME_KHR},
        {"VK_QUEUE_GLOBAL_PRIORITY_LOW_EXT", VK_QUEUE_GLOBAL_PRIORITY_LOW_EXT},
        {"VK_QUEUE_GLOBAL_PRIORITY_MEDIUM_EXT", VK_QUEUE_GLOBAL_PRIORITY_MEDIUM_EXT},
        {"VK_QUEUE_GLOBAL_PRIORITY_HIGH_EXT", VK_QUEUE_GLOBAL_PRIORITY_HIGH_EXT},
        {"VK_QUEUE_GLOBAL_PRIORITY_REALTIME_EXT", VK_QUEUE_GLOBAL_PRIORITY_REALTIME_EXT},
    };
    const auto it = map.find(input_value);
    if (it != map.end()) {
        return it->second;
    }
    return VkQueueGlobalPriorityKHR{};
}

static VkVideoCodecOperationFlagsKHR StringToVkVideoCodecOperationFlagsKHR(const std::string &input_value) {
    static const std::unordered_map<std::string, VkVideoCodecOperationFlagsKHR> map = {
        {"VK_VIDEO_CODEC_OPERATION_INVALID_BIT_KHR", VK_VIDEO_CODEC_OPERATION_INVALID_BIT_KHR},
        {"VK_VIDEO_CODEC_OPERATION_ENCODE_H264_BIT_EXT", VK_VIDEO_CODEC_OPERATION_ENCODE_H264_BIT_EXT},
        {"VK_VIDEO_CODEC_OPERATION_ENCODE_H265_BIT_EXT", VK_VIDEO_CODEC_OPERATION_ENCODE_H265_BIT_EXT},
        {"VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_EXT", VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_EXT},
        {"VK_VIDEO_CODEC_OPERATION_DECODE_H265_BIT_EXT", VK_VIDEO_CODEC_OPERATION_DECODE_H265_BIT_EXT},
    };
    const auto it = map.find(input_value);
    if (it != map.end()) {
        return it->second;
    }
    return VkVideoCodecOperationFlagsKHR{};
}

static VkPipelineStageFlags StringToVkPipelineStageFlags(const std::string &input_value) {
    static const std::unordered_map<std::string, VkPipelineStageFlags> map = {
        {"VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT", VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT},
        {"VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT", VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT},
        {"VK_PIPELINE_STAGE_VERTEX_INPUT_BIT", VK_PIPELINE_STAGE_VERTEX_INPUT_BIT},
        {"VK_PIPELINE_STAGE_VERTEX_SHADER_BIT", VK_PIPELINE_STAGE_VERTEX_SHADER_BIT},
        {"VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT", VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT},
        {"VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT", VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT},
        {"VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT", VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT},
        {"VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT", VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT},
        {"VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT", VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT},
        {"VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT", VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT},
        {"VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT", VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT},
        {"VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT", VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT},
        {"VK_PIPELINE_STAGE_TRANSFER_BIT", VK_PIPELINE_STAGE_TRANSFER_BIT},
        {"VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT", VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT},
        {"VK_PIPELINE_STAGE_HOST_BIT", VK_PIPELINE_STAGE_HOST_BIT},
        {"VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT", VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT},
        {"VK_PIPELINE_STAGE_ALL_COMMANDS_BIT", VK_PIPELINE_STAGE_ALL_COMMANDS_BIT},
        {"VK_PIPELINE_STAGE_NONE", VK_PIPELINE_STAGE_NONE},
        {"VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT", VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT},
        {"VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT", VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT},
        {"VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR", VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR},
        {"VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR", VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR},
        {"VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV", VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV},
        {"VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV", VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV},
        {"VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT", VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT},
        {"VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR", VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR},
        {"VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV", VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV},
        {"VK_PIPELINE_STAGE_SHADING_RATE_IMAGE_BIT_NV", VK_PIPELINE_STAGE_SHADING_RATE_IMAGE_BIT_NV},
        {"VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_NV", VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_NV},
        {"VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV", VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV},
        {"VK_PIPELINE_STAGE_NONE_KHR", VK_PIPELINE_STAGE_NONE_KHR},
    };
    const auto it = map.find(input_value);
    if (it != map.end()) {
        return it->second;
    }
    return VkPipelineStageFlags{};
}

static VkPipelineStageFlags2 StringToVkPipelineStageFlags2(const std::string &input_value) {
    static const std::unordered_map<std::string, VkPipelineStageFlags2> map = {
        {"VK_PIPELINE_STAGE_2_NONE", VK_PIPELINE_STAGE_2_NONE},
        {"VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT", VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT},
        {"VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT", VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT},
        {"VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT", VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT},
        {"VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT", VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT},
        {"VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT", VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT},
        {"VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT", VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT},
        {"VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT", VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT},
        {"VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT", VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT},
        {"VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT", VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT},
        {"VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT", VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT},
        {"VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT", VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT},
        {"VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT", VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT},
        {"VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT", VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT},
        {"VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT", VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT},
        {"VK_PIPELINE_STAGE_2_HOST_BIT", VK_PIPELINE_STAGE_2_HOST_BIT},
        {"VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT", VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT},
        {"VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT", VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT},
        {"VK_PIPELINE_STAGE_2_COPY_BIT", VK_PIPELINE_STAGE_2_COPY_BIT},
        {"VK_PIPELINE_STAGE_2_RESOLVE_BIT", VK_PIPELINE_STAGE_2_RESOLVE_BIT},
        {"VK_PIPELINE_STAGE_2_BLIT_BIT", VK_PIPELINE_STAGE_2_BLIT_BIT},
        {"VK_PIPELINE_STAGE_2_CLEAR_BIT", VK_PIPELINE_STAGE_2_CLEAR_BIT},
        {"VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT", VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT},
        {"VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT", VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT},
        {"VK_PIPELINE_STAGE_2_PRE_RASTERIZATION_SHADERS_BIT", VK_PIPELINE_STAGE_2_PRE_RASTERIZATION_SHADERS_BIT},
        {"VK_PIPELINE_STAGE_2_VIDEO_DECODE_BIT_KHR", VK_PIPELINE_STAGE_2_VIDEO_DECODE_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_VIDEO_ENCODE_BIT_KHR", VK_PIPELINE_STAGE_2_VIDEO_ENCODE_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_TRANSFORM_FEEDBACK_BIT_EXT", VK_PIPELINE_STAGE_2_TRANSFORM_FEEDBACK_BIT_EXT},
        {"VK_PIPELINE_STAGE_2_CONDITIONAL_RENDERING_BIT_EXT", VK_PIPELINE_STAGE_2_CONDITIONAL_RENDERING_BIT_EXT},
        {"VK_PIPELINE_STAGE_2_COMMAND_PREPROCESS_BIT_NV", VK_PIPELINE_STAGE_2_COMMAND_PREPROCESS_BIT_NV},
        {"VK_PIPELINE_STAGE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR", VK_PIPELINE_STAGE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR", VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR", VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_FRAGMENT_DENSITY_PROCESS_BIT_EXT", VK_PIPELINE_STAGE_2_FRAGMENT_DENSITY_PROCESS_BIT_EXT},
        {"VK_PIPELINE_STAGE_2_TASK_SHADER_BIT_NV", VK_PIPELINE_STAGE_2_TASK_SHADER_BIT_NV},
        {"VK_PIPELINE_STAGE_2_MESH_SHADER_BIT_NV", VK_PIPELINE_STAGE_2_MESH_SHADER_BIT_NV},
        {"VK_PIPELINE_STAGE_2_SUBPASS_SHADING_BIT_HUAWEI", VK_PIPELINE_STAGE_2_SUBPASS_SHADING_BIT_HUAWEI},
        {"VK_PIPELINE_STAGE_2_INVOCATION_MASK_BIT_HUAWEI", VK_PIPELINE_STAGE_2_INVOCATION_MASK_BIT_HUAWEI},
        {"VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_COPY_BIT_KHR", VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_COPY_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_NONE_KHR", VK_PIPELINE_STAGE_2_NONE_KHR},
        {"VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT_KHR", VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT_KHR", VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT_KHR", VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT_KHR", VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT_KHR", VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT_KHR", VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT_KHR", VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT_KHR", VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT_KHR", VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT_KHR", VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR", VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT_KHR", VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT_KHR", VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_TRANSFER_BIT", VK_PIPELINE_STAGE_2_TRANSFER_BIT},
        {"VK_PIPELINE_STAGE_2_TRANSFER_BIT_KHR", VK_PIPELINE_STAGE_2_TRANSFER_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT_KHR", VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_HOST_BIT_KHR", VK_PIPELINE_STAGE_2_HOST_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT_KHR", VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR", VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_COPY_BIT_KHR", VK_PIPELINE_STAGE_2_COPY_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_RESOLVE_BIT_KHR", VK_PIPELINE_STAGE_2_RESOLVE_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_BLIT_BIT_KHR", VK_PIPELINE_STAGE_2_BLIT_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_CLEAR_BIT_KHR", VK_PIPELINE_STAGE_2_CLEAR_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT_KHR", VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT_KHR", VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_PRE_RASTERIZATION_SHADERS_BIT_KHR", VK_PIPELINE_STAGE_2_PRE_RASTERIZATION_SHADERS_BIT_KHR},
        {"VK_PIPELINE_STAGE_2_SHADING_RATE_IMAGE_BIT_NV", VK_PIPELINE_STAGE_2_SHADING_RATE_IMAGE_BIT_NV},
        {"VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_NV", VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_NV},
        {"VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_NV", VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_NV},
    };
    const auto it = map.find(input_value);
    if (it != map.end()) {
        return it->second;
    }
    return VkPipelineStageFlags2{};
}

static VkFormatFeatureFlags2 StringToVkFormatFeatureFlags2(const std::string &input_value) {
    static const std::unordered_map<std::string, VkFormatFeatureFlags2> map = {
        {"VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT", VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT},
        {"VK_FORMAT_FEATURE_2_STORAGE_IMAGE_BIT", VK_FORMAT_FEATURE_2_STORAGE_IMAGE_BIT},
        {"VK_FORMAT_FEATURE_2_STORAGE_IMAGE_ATOMIC_BIT", VK_FORMAT_FEATURE_2_STORAGE_IMAGE_ATOMIC_BIT},
        {"VK_FORMAT_FEATURE_2_UNIFORM_TEXEL_BUFFER_BIT", VK_FORMAT_FEATURE_2_UNIFORM_TEXEL_BUFFER_BIT},
        {"VK_FORMAT_FEATURE_2_STORAGE_TEXEL_BUFFER_BIT", VK_FORMAT_FEATURE_2_STORAGE_TEXEL_BUFFER_BIT},
        {"VK_FORMAT_FEATURE_2_STORAGE_TEXEL_BUFFER_ATOMIC_BIT", VK_FORMAT_FEATURE_2_STORAGE_TEXEL_BUFFER_ATOMIC_BIT},
        {"VK_FORMAT_FEATURE_2_VERTEX_BUFFER_BIT", VK_FORMAT_FEATURE_2_VERTEX_BUFFER_BIT},
        {"VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BIT", VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BIT},
        {"VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BLEND_BIT", VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BLEND_BIT},
        {"VK_FORMAT_FEATURE_2_DEPTH_STENCIL_ATTACHMENT_BIT", VK_FORMAT_FEATURE_2_DEPTH_STENCIL_ATTACHMENT_BIT},
        {"VK_FORMAT_FEATURE_2_BLIT_SRC_BIT", VK_FORMAT_FEATURE_2_BLIT_SRC_BIT},
        {"VK_FORMAT_FEATURE_2_BLIT_DST_BIT", VK_FORMAT_FEATURE_2_BLIT_DST_BIT},
        {"VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_LINEAR_BIT", VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_LINEAR_BIT},
        {"VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_CUBIC_BIT", VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_CUBIC_BIT},
        {"VK_FORMAT_FEATURE_2_TRANSFER_SRC_BIT", VK_FORMAT_FEATURE_2_TRANSFER_SRC_BIT},
        {"VK_FORMAT_FEATURE_2_TRANSFER_DST_BIT", VK_FORMAT_FEATURE_2_TRANSFER_DST_BIT},
        {"VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_MINMAX_BIT", VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_MINMAX_BIT},
        {"VK_FORMAT_FEATURE_2_MIDPOINT_CHROMA_SAMPLES_BIT", VK_FORMAT_FEATURE_2_MIDPOINT_CHROMA_SAMPLES_BIT},
        {"VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT", VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT},
        {"VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT", VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT},
        {"VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT", VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT},
        {"VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT", VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT},
        {"VK_FORMAT_FEATURE_2_DISJOINT_BIT", VK_FORMAT_FEATURE_2_DISJOINT_BIT},
        {"VK_FORMAT_FEATURE_2_COSITED_CHROMA_SAMPLES_BIT", VK_FORMAT_FEATURE_2_COSITED_CHROMA_SAMPLES_BIT},
        {"VK_FORMAT_FEATURE_2_STORAGE_READ_WITHOUT_FORMAT_BIT", VK_FORMAT_FEATURE_2_STORAGE_READ_WITHOUT_FORMAT_BIT},
        {"VK_FORMAT_FEATURE_2_STORAGE_WRITE_WITHOUT_FORMAT_BIT", VK_FORMAT_FEATURE_2_STORAGE_WRITE_WITHOUT_FORMAT_BIT},
        {"VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_DEPTH_COMPARISON_BIT", VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_DEPTH_COMPARISON_BIT},
        {"VK_FORMAT_FEATURE_2_VIDEO_DECODE_OUTPUT_BIT_KHR", VK_FORMAT_FEATURE_2_VIDEO_DECODE_OUTPUT_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_VIDEO_DECODE_DPB_BIT_KHR", VK_FORMAT_FEATURE_2_VIDEO_DECODE_DPB_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR", VK_FORMAT_FEATURE_2_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_FRAGMENT_DENSITY_MAP_BIT_EXT", VK_FORMAT_FEATURE_2_FRAGMENT_DENSITY_MAP_BIT_EXT},
        {"VK_FORMAT_FEATURE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR", VK_FORMAT_FEATURE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_VIDEO_ENCODE_INPUT_BIT_KHR", VK_FORMAT_FEATURE_2_VIDEO_ENCODE_INPUT_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_VIDEO_ENCODE_DPB_BIT_KHR", VK_FORMAT_FEATURE_2_VIDEO_ENCODE_DPB_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_LINEAR_COLOR_ATTACHMENT_BIT_NV", VK_FORMAT_FEATURE_2_LINEAR_COLOR_ATTACHMENT_BIT_NV},
        {"VK_FORMAT_FEATURE_2_WEIGHT_IMAGE_BIT_QCOM", VK_FORMAT_FEATURE_2_WEIGHT_IMAGE_BIT_QCOM},
        {"VK_FORMAT_FEATURE_2_WEIGHT_SAMPLED_IMAGE_BIT_QCOM", VK_FORMAT_FEATURE_2_WEIGHT_SAMPLED_IMAGE_BIT_QCOM},
        {"VK_FORMAT_FEATURE_2_BLOCK_MATCHING_BIT_QCOM", VK_FORMAT_FEATURE_2_BLOCK_MATCHING_BIT_QCOM},
        {"VK_FORMAT_FEATURE_2_BOX_FILTER_SAMPLED_BIT_QCOM", VK_FORMAT_FEATURE_2_BOX_FILTER_SAMPLED_BIT_QCOM},
        {"VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT_KHR", VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_STORAGE_IMAGE_BIT_KHR", VK_FORMAT_FEATURE_2_STORAGE_IMAGE_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_STORAGE_IMAGE_ATOMIC_BIT_KHR", VK_FORMAT_FEATURE_2_STORAGE_IMAGE_ATOMIC_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_UNIFORM_TEXEL_BUFFER_BIT_KHR", VK_FORMAT_FEATURE_2_UNIFORM_TEXEL_BUFFER_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_STORAGE_TEXEL_BUFFER_BIT_KHR", VK_FORMAT_FEATURE_2_STORAGE_TEXEL_BUFFER_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_STORAGE_TEXEL_BUFFER_ATOMIC_BIT_KHR", VK_FORMAT_FEATURE_2_STORAGE_TEXEL_BUFFER_ATOMIC_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_VERTEX_BUFFER_BIT_KHR", VK_FORMAT_FEATURE_2_VERTEX_BUFFER_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BIT_KHR", VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BLEND_BIT_KHR", VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BLEND_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_DEPTH_STENCIL_ATTACHMENT_BIT_KHR", VK_FORMAT_FEATURE_2_DEPTH_STENCIL_ATTACHMENT_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_BLIT_SRC_BIT_KHR", VK_FORMAT_FEATURE_2_BLIT_SRC_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_BLIT_DST_BIT_KHR", VK_FORMAT_FEATURE_2_BLIT_DST_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_LINEAR_BIT_KHR", VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_LINEAR_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT", VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT},
        {"VK_FORMAT_FEATURE_2_TRANSFER_SRC_BIT_KHR", VK_FORMAT_FEATURE_2_TRANSFER_SRC_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_TRANSFER_DST_BIT_KHR", VK_FORMAT_FEATURE_2_TRANSFER_DST_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_MINMAX_BIT_KHR", VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_MINMAX_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_MIDPOINT_CHROMA_SAMPLES_BIT_KHR", VK_FORMAT_FEATURE_2_MIDPOINT_CHROMA_SAMPLES_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR", VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR", VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR", VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR", VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_DISJOINT_BIT_KHR", VK_FORMAT_FEATURE_2_DISJOINT_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_COSITED_CHROMA_SAMPLES_BIT_KHR", VK_FORMAT_FEATURE_2_COSITED_CHROMA_SAMPLES_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_STORAGE_READ_WITHOUT_FORMAT_BIT_KHR", VK_FORMAT_FEATURE_2_STORAGE_READ_WITHOUT_FORMAT_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_STORAGE_WRITE_WITHOUT_FORMAT_BIT_KHR", VK_FORMAT_FEATURE_2_STORAGE_WRITE_WITHOUT_FORMAT_BIT_KHR},
        {"VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_DEPTH_COMPARISON_BIT_KHR", VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_DEPTH_COMPARISON_BIT_KHR},
    };
    const auto it = map.find(input_value);
    if (it != map.end()) {
        return it->second;
    }
    return VkFormatFeatureFlags2{};
}

bool HasFlags(VkFlags deviceFlags, VkFlags profileFlags) { return (deviceFlags & profileFlags) == profileFlags; }
bool HasFlags(VkFlags64 deviceFlags, VkFlags64 profileFlags) { return (deviceFlags & profileFlags) == profileFlags; }

std::string format(const char *message, ...) {
    std::size_t const STRING_BUFFER(4096);

    assert(message != nullptr);
    assert(strlen(message) >= 0 && strlen(message) < STRING_BUFFER);

    char buffer[STRING_BUFFER];
    va_list list;

    va_start(list, message);
    vsprintf(buffer, message, list);
    va_end(list);

    return buffer;
}

// Various small utility functions ///////////////////////////////////////////////////////////////////////////////////////////////

#if defined(__ANDROID__)
void AndroidPrintf(DebugReport level, const std::string &message) {
    switch (level) {
        default:
        case DEBUG_REPORT_DEBUG_BIT:
        case DEBUG_REPORT_NOTIFICATION_BIT:
            __android_log_print(ANDROID_LOG_INFO, "Profiles", "%s", message.c_str());
            break;
        case DEBUG_REPORT_WARNING_BIT:
            __android_log_print(ANDROID_LOG_DEBUG, "Profiles", "%s", message.c_str());
            break;
        case DEBUG_REPORT_ERROR_BIT:
            __android_log_print(ANDROID_LOG_ERROR, "Profiles", "%s", message.c_str());
            break;
    }
}
#endif

const char *GetLogPrefix(DebugReport report) {
    static const char *table[] = {"PROFILES NOTIFICATION: ", "PROFILES WARNING: ", "PROFILES ERROR: ", "PROFILES DEBUG: "};

    switch (report) {
        case DEBUG_REPORT_NOTIFICATION_BIT:
            return table[0];
        default:
        case DEBUG_REPORT_WARNING_BIT:
            return table[1];
        case DEBUG_REPORT_ERROR_BIT:
            return table[2];
        case DEBUG_REPORT_DEBUG_BIT:
            return table[3];
    }
}

void LogMessage(DebugReport report, const std::string &message) {
    if (!(layer_settings->debug_reports & report)) return;

    const std::string log = format("%s%s", GetLogPrefix(report), message.c_str());

    if (layer_settings->debug_actions & DEBUG_ACTION_STDOUT_BIT) {
#if defined(__ANDROID__)
        AndroidPrintf(report, message);
#else
        fprintf(stdout, "%s", log.c_str());
#endif
    }

    if (layer_settings->debug_actions & DEBUG_ACTION_FILE_BIT) {
        fprintf(profiles_log_file, "%s", log.c_str());
    }

#if _WIN32
    if (layer_settings->debug_actions & DEBUG_ACTION_OUTPUT_BIT) {
        OutputDebugString(log.c_str());
    }
#endif  //_WIN32

    if (layer_settings->debug_actions & DEBUG_ACTION_BREAKPOINT_BIT) {
#ifdef WIN32
        DebugBreak();
#else
        raise(SIGTRAP);
#endif
    }
}

void LogFlush() {
    if (layer_settings->debug_actions & DEBUG_ACTION_STDOUT_BIT) {
        std::fflush(stdout);
    }
    if (layer_settings->debug_actions & DEBUG_ACTION_FILE_BIT) {
        std::fflush(profiles_log_file);
    }
}

std::string format_device_support_string(VkFormatFeatureFlags format_features) {
    if (format_features == 0) return std::string("does not support it");
    return ::format("only supports:\n\t\"%s\"", GetFormatFeatureString(format_features).c_str());
}

std::string format_device_support_string(VkFormatFeatureFlags2 format_features) {
    if (format_features == 0) return std::string("does not support it");
    return ::format("only supports:\n\t\"%s\"", GetFormatFeature2String(format_features).c_str());
}

void WarnMissingFormatFeatures(const std::string &format_name, const std::string &features, VkFormatFeatureFlags profile_features,
                               VkFormatFeatureFlags device_features) {
    LogMessage(DEBUG_REPORT_WARNING_BIT,
               ::format("For %s `%s`,\nthe Profile requires:\n\t\"%s\"\nbut the Device %s.\nThe "
                        "`%s` can't be simulated on this Device.\n",
                        format_name.c_str(), features.c_str(), GetFormatFeatureString(profile_features).c_str(),
                        format_device_support_string(device_features).c_str(), features.c_str()));
}

void WarnMissingFormatFeatures2(const std::string &format_name, const std::string &features, VkFormatFeatureFlags2 profile_features,
                                VkFormatFeatureFlags2 device_features) {
    LogMessage(DEBUG_REPORT_WARNING_BIT,
               ::format("For %s `%s`,\nthe Profile requires:\n\t\"%s\"\nbut the Device %s.\nThe "
                        "`%s` can't be simulated on this Device.\n",
                        format_name.c_str(), features.c_str(), GetFormatFeature2String(profile_features).c_str(),
                        format_device_support_string(device_features).c_str(), features.c_str()));
}

static std::string StringAPIVersion(uint32_t version) {
    std::stringstream version_name;
    uint32_t major = VK_API_VERSION_MAJOR(version);
    uint32_t minor = VK_API_VERSION_MINOR(version);
    uint32_t patch = VK_API_VERSION_PATCH(version);
    version_name << major << "." << minor << "." << patch;
    return version_name.str();
}

// Get all elements from a vkEnumerate*() lambda into a std::vector.
template <typename T>
VkResult EnumerateAll(std::vector<T> *vect, std::function<VkResult(uint32_t *, T *)> func) {
    VkResult result = VK_INCOMPLETE;
    do {
        uint32_t count = 0;
        result = func(&count, nullptr);
        assert(result == VK_SUCCESS);
        vect->resize(count);
        result = func(&count, vect->data());
    } while (result == VK_INCOMPLETE);
    return result;
}

// Global variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::recursive_mutex global_lock;  // Enforce thread-safety for this layer.

uint32_t loader_layer_iface_version = CURRENT_LOADER_LAYER_INTERFACE_VERSION;

typedef std::unordered_map<uint32_t /*VkFormat*/, VkFormatProperties> ArrayOfVkFormatProperties;
typedef std::unordered_map<uint32_t /*VkFormat*/, VkFormatProperties3> ArrayOfVkFormatProperties3;
typedef std::unordered_map<uint32_t /*VkFormat*/, VkDrmFormatModifierPropertiesList2EXT> ArrayOfVkDrmFormatModifierProperties;
typedef std::vector<VkExtensionProperties> ArrayOfVkExtensionProperties;

struct QueueFamilyProperties {
    VkQueueFamilyProperties2 properties_2 = {};
    VkQueueFamilyGlobalPriorityPropertiesKHR global_priority_properties_ = {};
    VkVideoQueueFamilyProperties2KHR video_properties_2_ = {};
    VkQueueFamilyCheckpointPropertiesNV checkpoint_properties_ = {};
    VkQueueFamilyCheckpointProperties2NV checkpoint_properties_2_ = {};
    VkQueueFamilyQueryResultStatusProperties2KHR query_result_status_properties_2_ = {};

    QueueFamilyProperties() {
        properties_2.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
        global_priority_properties_.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_GLOBAL_PRIORITY_PROPERTIES_EXT;
        video_properties_2_.sType = VK_STRUCTURE_TYPE_VIDEO_QUEUE_FAMILY_PROPERTIES_2_KHR;
        checkpoint_properties_.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_CHECKPOINT_PROPERTIES_NV;
        checkpoint_properties_2_.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_CHECKPOINT_PROPERTIES_2_NV;
        query_result_status_properties_2_.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_QUERY_RESULT_STATUS_PROPERTIES_2_KHR;
    }
};
typedef std::vector<QueueFamilyProperties> ArrayOfVkQueueFamilyProperties;

// FormatProperties utilities ////////////////////////////////////////////////////////////////////////////////////////////////////

bool IsFormatSupported(const VkFormatProperties &props) {
    // Per [SPEC] section 30.3.2 "Format Properties":
    // "... if no format feature flags are supported, the format itself is not supported ..."
    return !(!props.linearTilingFeatures && !props.optimalTilingFeatures && !props.bufferFeatures);
}

// PhysicalDeviceData : creates and manages the simulated device configurations //////////////////////////////////////////////////

class PhysicalDeviceData {
   public:
    // Create a new PDD element during vkCreateInstance(), and preserve in map, indexed by physical_device.
    static PhysicalDeviceData &Create(VkPhysicalDevice pd, VkInstance instance) {
        assert(pd != VK_NULL_HANDLE);
        assert(instance != VK_NULL_HANDLE);
        assert(!Find(pd));  // Verify this instance does not already exist.

        LogMessage(DEBUG_REPORT_DEBUG_BIT, "PhysicalDeviceData::Create()\n");

        const auto result = map_.emplace(pd, PhysicalDeviceData(instance));
        assert(result.second);  // true=insertion, false=replacement
        auto iter = result.first;
        PhysicalDeviceData *pdd = &iter->second;
        assert(Find(pd) == pdd);  // Verify we get the same instance we just inserted.
        return *pdd;
    }

    static void Destroy(const VkPhysicalDevice pd) {
        LogMessage(DEBUG_REPORT_DEBUG_BIT, "PhysicalDeviceData::Destroy()\n");
        map_.erase(pd);
    }

    // Find a PDD from our map, or nullptr if doesn't exist.
    static PhysicalDeviceData *Find(VkPhysicalDevice pd) {
        const auto iter = map_.find(pd);
        return (iter != map_.end()) ? &iter->second : nullptr;
    }

    static bool HasExtension(PhysicalDeviceData *pdd, const char *extension_name) {
        for (const auto &ext_prop : pdd->device_extensions_) {
            if (strncmp(extension_name, ext_prop.extensionName, VK_MAX_EXTENSION_NAME_SIZE) == 0) {
                return true;
            }
        }
        return false;
    }

    static bool HasSimulatedExtension(VkPhysicalDevice pd, const char *extension_name) {
        return HasSimulatedExtension(Find(pd), extension_name);
    }

    static bool HasSimulatedExtension(PhysicalDeviceData *pdd, const char *extension_name) {
        for (const auto &ext_prop : pdd->simulation_extensions_) {
            if (strncmp(extension_name, ext_prop.extensionName, VK_MAX_EXTENSION_NAME_SIZE) == 0) {
                return true;
            }
        }
        return false;
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

    ArrayOfVkExtensionProperties device_extensions_;
    ArrayOfVkFormatProperties device_formats_;
    ArrayOfVkFormatProperties3 device_formats_3_;
    ArrayOfVkQueueFamilyProperties device_queue_family_properties_;
    ArrayOfVkExtensionProperties simulation_extensions_;
    VkPhysicalDeviceProperties physical_device_properties_;
    VkPhysicalDeviceFeatures physical_device_features_;
    VkPhysicalDeviceMemoryProperties physical_device_memory_properties_;
    VkPhysicalDeviceToolProperties physical_device_tool_properties_;
    VkSurfaceCapabilitiesKHR surface_capabilities_;
    ArrayOfVkFormatProperties arrayof_format_properties_;
    ArrayOfVkFormatProperties3 arrayof_format_properties_3_;
    ArrayOfVkExtensionProperties arrayof_extension_properties_;
    ArrayOfVkQueueFamilyProperties arrayof_queue_family_properties_;

    bool vulkan_1_1_properties_written_;
    bool vulkan_1_2_properties_written_;
    bool vulkan_1_3_properties_written_;

    bool vulkan_1_1_features_written_;
    bool vulkan_1_2_features_written_;
    bool vulkan_1_3_features_written_;

    // Core properties
    VkPhysicalDeviceSubgroupProperties physical_device_subgroup_properties_;
    VkPhysicalDeviceProtectedMemoryProperties physical_device_protected_memory_properties_;
    VkPhysicalDeviceVulkan11Properties physical_device_vulkan_11_properties_;
    VkPhysicalDeviceVulkan12Properties physical_device_vulkan_12_properties_;
    VkPhysicalDeviceVulkan13Properties physical_device_vulkan_13_properties_;

    // Core features
    VkPhysicalDeviceProtectedMemoryFeatures physical_device_protected_memory_features_;
    VkPhysicalDeviceShaderDrawParameterFeatures physical_device_shader_draw_parameter_features_;
    VkPhysicalDeviceVulkan11Features physical_device_vulkan_11_features_;
    VkPhysicalDeviceVulkan12Features physical_device_vulkan_12_features_;
    VkPhysicalDeviceVulkan13Features physical_device_vulkan_13_features_;

    // VK_EXT_transform_feedback structs
    VkPhysicalDeviceTransformFeedbackPropertiesEXT physical_device_transform_feedback_properties_;
    VkPhysicalDeviceTransformFeedbackFeaturesEXT physical_device_transform_feedback_features_;

    // VK_KHR_dynamic_rendering structs
    VkPhysicalDeviceDynamicRenderingFeaturesKHR physical_device_dynamic_rendering_features_;

    // VK_NV_corner_sampled_image structs
    VkPhysicalDeviceCornerSampledImageFeaturesNV physical_device_corner_sampled_image_features_;

    // VK_KHR_multiview structs
    VkPhysicalDeviceMultiviewPropertiesKHR physical_device_multiview_properties_;
    VkPhysicalDeviceMultiviewFeaturesKHR physical_device_multiview_features_;

    // VK_EXT_texture_compression_astc_hdr structs
    VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT physical_device_texture_compression_astchdrfeatures_;

    // VK_EXT_astc_decode_mode structs
    VkPhysicalDeviceASTCDecodeFeaturesEXT physical_device_astcdecode_features_;

    // VK_EXT_pipeline_robustness structs
    VkPhysicalDevicePipelineRobustnessPropertiesEXT physical_device_pipeline_robustness_properties_;
    VkPhysicalDevicePipelineRobustnessFeaturesEXT physical_device_pipeline_robustness_features_;

    // VK_KHR_external_memory_capabilities structs
    VkPhysicalDeviceIDPropertiesKHR physical_device_idproperties_;

    // VK_KHR_push_descriptor structs
    VkPhysicalDevicePushDescriptorPropertiesKHR physical_device_push_descriptor_properties_;

    // VK_EXT_conditional_rendering structs
    VkPhysicalDeviceConditionalRenderingFeaturesEXT physical_device_conditional_rendering_features_;

    // VK_KHR_shader_float16_int8 structs
    VkPhysicalDeviceFloat16Int8FeaturesKHR physical_device_float_16_int_8_features_;

    // VK_KHR_16bit_storage structs
    VkPhysicalDevice16BitStorageFeaturesKHR physical_device_16_bit_storage_features_;

    // VK_NVX_multiview_per_view_attributes structs
    VkPhysicalDeviceMultiviewPerViewAttributesPropertiesNVX physical_device_multiview_per_view_attributes_properties_;

    // VK_EXT_discard_rectangles structs
    VkPhysicalDeviceDiscardRectanglePropertiesEXT physical_device_discard_rectangle_properties_;

    // VK_EXT_conservative_rasterization structs
    VkPhysicalDeviceConservativeRasterizationPropertiesEXT physical_device_conservative_rasterization_properties_;

    // VK_EXT_depth_clip_enable structs
    VkPhysicalDeviceDepthClipEnableFeaturesEXT physical_device_depth_clip_enable_features_;

    // VK_KHR_imageless_framebuffer structs
    VkPhysicalDeviceImagelessFramebufferFeaturesKHR physical_device_imageless_framebuffer_features_;

    // VK_KHR_performance_query structs
    VkPhysicalDevicePerformanceQueryPropertiesKHR physical_device_performance_query_properties_;
    VkPhysicalDevicePerformanceQueryFeaturesKHR physical_device_performance_query_features_;

    // VK_KHR_maintenance2 structs
    VkPhysicalDevicePointClippingPropertiesKHR physical_device_point_clipping_properties_;

    // VK_KHR_variable_pointers structs
    VkPhysicalDeviceVariablePointerFeaturesKHR physical_device_variable_pointer_features_;

    // VK_EXT_sampler_filter_minmax structs
    VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT physical_device_sampler_filter_minmax_properties_;

    // VK_EXT_inline_uniform_block structs
    VkPhysicalDeviceInlineUniformBlockPropertiesEXT physical_device_inline_uniform_block_properties_;
    VkPhysicalDeviceInlineUniformBlockFeaturesEXT physical_device_inline_uniform_block_features_;

    // VK_EXT_sample_locations structs
    VkPhysicalDeviceSampleLocationsPropertiesEXT physical_device_sample_locations_properties_;

    // VK_EXT_blend_operation_advanced structs
    VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT physical_device_blend_operation_advanced_properties_;
    VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT physical_device_blend_operation_advanced_features_;

    // VK_KHR_acceleration_structure structs
    VkPhysicalDeviceAccelerationStructurePropertiesKHR physical_device_acceleration_structure_properties_;
    VkPhysicalDeviceAccelerationStructureFeaturesKHR physical_device_acceleration_structure_features_;

    // VK_KHR_ray_tracing_pipeline structs
    VkPhysicalDeviceRayTracingPipelinePropertiesKHR physical_device_ray_tracing_pipeline_properties_;
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR physical_device_ray_tracing_pipeline_features_;

    // VK_KHR_ray_query structs
    VkPhysicalDeviceRayQueryFeaturesKHR physical_device_ray_query_features_;

    // VK_NV_shader_sm_builtins structs
    VkPhysicalDeviceShaderSMBuiltinsPropertiesNV physical_device_shader_smbuiltins_properties_;
    VkPhysicalDeviceShaderSMBuiltinsFeaturesNV physical_device_shader_smbuiltins_features_;

    // VK_KHR_sampler_ycbcr_conversion structs
    VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR physical_device_sampler_ycbcr_conversion_features_;

    // VK_EXT_descriptor_indexing structs
    VkPhysicalDeviceDescriptorIndexingPropertiesEXT physical_device_descriptor_indexing_properties_;
    VkPhysicalDeviceDescriptorIndexingFeaturesEXT physical_device_descriptor_indexing_features_;

    // VK_KHR_portability_subset structs
    VkPhysicalDevicePortabilitySubsetPropertiesKHR physical_device_portability_subset_properties_;
    VkPhysicalDevicePortabilitySubsetFeaturesKHR physical_device_portability_subset_features_;

    // VK_NV_shading_rate_image structs
    VkPhysicalDeviceShadingRateImagePropertiesNV physical_device_shading_rate_image_properties_;
    VkPhysicalDeviceShadingRateImageFeaturesNV physical_device_shading_rate_image_features_;

    // VK_NV_ray_tracing structs
    VkPhysicalDeviceRayTracingPropertiesNV physical_device_ray_tracing_properties_;

    // VK_NV_representative_fragment_test structs
    VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV physical_device_representative_fragment_test_features_;

    // VK_KHR_maintenance3 structs
    VkPhysicalDeviceMaintenance3PropertiesKHR physical_device_maintenance_3_properties_;

    // VK_KHR_shader_subgroup_extended_types structs
    VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR physical_device_shader_subgroup_extended_types_features_;

    // VK_KHR_8bit_storage structs
    VkPhysicalDevice8BitStorageFeaturesKHR physical_device_8_bit_storage_features_;

    // VK_EXT_external_memory_host structs
    VkPhysicalDeviceExternalMemoryHostPropertiesEXT physical_device_external_memory_host_properties_;

    // VK_KHR_shader_atomic_int64 structs
    VkPhysicalDeviceShaderAtomicInt64FeaturesKHR physical_device_shader_atomic_int_64_features_;

    // VK_KHR_shader_clock structs
    VkPhysicalDeviceShaderClockFeaturesKHR physical_device_shader_clock_features_;

    // VK_AMD_shader_core_properties structs
    VkPhysicalDeviceShaderCorePropertiesAMD physical_device_shader_core_properties_;

    // VK_EXT_vertex_attribute_divisor structs
    VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT physical_device_vertex_attribute_divisor_properties_;
    VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT physical_device_vertex_attribute_divisor_features_;

    // VK_KHR_driver_properties structs
    VkPhysicalDeviceDriverPropertiesKHR physical_device_driver_properties_;

    // VK_KHR_shader_float_controls structs
    VkPhysicalDeviceFloatControlsPropertiesKHR physical_device_float_controls_properties_;

    // VK_KHR_depth_stencil_resolve structs
    VkPhysicalDeviceDepthStencilResolvePropertiesKHR physical_device_depth_stencil_resolve_properties_;

    // VK_NV_compute_shader_derivatives structs
    VkPhysicalDeviceComputeShaderDerivativesFeaturesNV physical_device_compute_shader_derivatives_features_;

    // VK_NV_mesh_shader structs
    VkPhysicalDeviceMeshShaderPropertiesNV physical_device_mesh_shader_properties_;
    VkPhysicalDeviceMeshShaderFeaturesNV physical_device_mesh_shader_features_;

    // VK_NV_fragment_shader_barycentric structs
    VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV physical_device_fragment_shader_barycentric_features_;

    // VK_NV_shader_image_footprint structs
    VkPhysicalDeviceShaderImageFootprintFeaturesNV physical_device_shader_image_footprint_features_;

    // VK_NV_scissor_exclusive structs
    VkPhysicalDeviceExclusiveScissorFeaturesNV physical_device_exclusive_scissor_features_;

    // VK_KHR_timeline_semaphore structs
    VkPhysicalDeviceTimelineSemaphorePropertiesKHR physical_device_timeline_semaphore_properties_;
    VkPhysicalDeviceTimelineSemaphoreFeaturesKHR physical_device_timeline_semaphore_features_;

    // VK_INTEL_shader_integer_functions2 structs
    VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL physical_device_shader_integer_functions_2_features_;

    // VK_KHR_vulkan_memory_model structs
    VkPhysicalDeviceVulkanMemoryModelFeaturesKHR physical_device_vulkan_memory_model_features_;

    // VK_EXT_pci_bus_info structs
    VkPhysicalDevicePCIBusInfoPropertiesEXT physical_device_pcibus_info_properties_;

    // VK_KHR_shader_terminate_invocation structs
    VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR physical_device_shader_terminate_invocation_features_;

    // VK_EXT_fragment_density_map structs
    VkPhysicalDeviceFragmentDensityMapPropertiesEXT physical_device_fragment_density_map_properties_;
    VkPhysicalDeviceFragmentDensityMapFeaturesEXT physical_device_fragment_density_map_features_;

    // VK_EXT_scalar_block_layout structs
    VkPhysicalDeviceScalarBlockLayoutFeaturesEXT physical_device_scalar_block_layout_features_;

    // VK_EXT_subgroup_size_control structs
    VkPhysicalDeviceSubgroupSizeControlPropertiesEXT physical_device_subgroup_size_control_properties_;
    VkPhysicalDeviceSubgroupSizeControlFeaturesEXT physical_device_subgroup_size_control_features_;

    // VK_KHR_fragment_shading_rate structs
    VkPhysicalDeviceFragmentShadingRatePropertiesKHR physical_device_fragment_shading_rate_properties_;
    VkPhysicalDeviceFragmentShadingRateFeaturesKHR physical_device_fragment_shading_rate_features_;

    // VK_AMD_shader_core_properties2 structs
    VkPhysicalDeviceShaderCoreProperties2AMD physical_device_shader_core_properties_2_;

    // VK_AMD_device_coherent_memory structs
    VkPhysicalDeviceCoherentMemoryFeaturesAMD physical_device_coherent_memory_features_;

    // VK_EXT_shader_image_atomic_int64 structs
    VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT physical_device_shader_image_atomic_int_64_features_;

    // VK_EXT_memory_priority structs
    VkPhysicalDeviceMemoryPriorityFeaturesEXT physical_device_memory_priority_features_;

    // VK_NV_dedicated_allocation_image_aliasing structs
    VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV physical_device_dedicated_allocation_image_aliasing_features_;

    // VK_KHR_separate_depth_stencil_layouts structs
    VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR physical_device_separate_depth_stencil_layouts_features_;

    // VK_EXT_buffer_device_address structs
    VkPhysicalDeviceBufferAddressFeaturesEXT physical_device_buffer_address_features_;

    // VK_KHR_present_wait structs
    VkPhysicalDevicePresentWaitFeaturesKHR physical_device_present_wait_features_;

    // VK_NV_cooperative_matrix structs
    VkPhysicalDeviceCooperativeMatrixPropertiesNV physical_device_cooperative_matrix_properties_;
    VkPhysicalDeviceCooperativeMatrixFeaturesNV physical_device_cooperative_matrix_features_;

    // VK_NV_coverage_reduction_mode structs
    VkPhysicalDeviceCoverageReductionModeFeaturesNV physical_device_coverage_reduction_mode_features_;

    // VK_EXT_fragment_shader_interlock structs
    VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT physical_device_fragment_shader_interlock_features_;

    // VK_EXT_ycbcr_image_arrays structs
    VkPhysicalDeviceYcbcrImageArraysFeaturesEXT physical_device_ycbcr_image_arrays_features_;

    // VK_KHR_uniform_buffer_standard_layout structs
    VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR physical_device_uniform_buffer_standard_layout_features_;

    // VK_EXT_provoking_vertex structs
    VkPhysicalDeviceProvokingVertexPropertiesEXT physical_device_provoking_vertex_properties_;
    VkPhysicalDeviceProvokingVertexFeaturesEXT physical_device_provoking_vertex_features_;

    // VK_KHR_buffer_device_address structs
    VkPhysicalDeviceBufferDeviceAddressFeaturesKHR physical_device_buffer_device_address_features_;

    // VK_EXT_line_rasterization structs
    VkPhysicalDeviceLineRasterizationPropertiesEXT physical_device_line_rasterization_properties_;
    VkPhysicalDeviceLineRasterizationFeaturesEXT physical_device_line_rasterization_features_;

    // VK_EXT_shader_atomic_float structs
    VkPhysicalDeviceShaderAtomicFloatFeaturesEXT physical_device_shader_atomic_float_features_;

    // VK_EXT_host_query_reset structs
    VkPhysicalDeviceHostQueryResetFeaturesEXT physical_device_host_query_reset_features_;

    // VK_EXT_index_type_uint8 structs
    VkPhysicalDeviceIndexTypeUint8FeaturesEXT physical_device_index_type_uint_8_features_;

    // VK_EXT_extended_dynamic_state structs
    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT physical_device_extended_dynamic_state_features_;

    // VK_KHR_pipeline_executable_properties structs
    VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR physical_device_pipeline_executable_properties_features_;

    // VK_EXT_shader_atomic_float2 structs
    VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT physical_device_shader_atomic_float_2_features_;

    // VK_EXT_shader_demote_to_helper_invocation structs
    VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT physical_device_shader_demote_to_helper_invocation_features_;

    // VK_NV_device_generated_commands structs
    VkPhysicalDeviceDeviceGeneratedCommandsPropertiesNV physical_device_device_generated_commands_properties_;
    VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV physical_device_device_generated_commands_features_;

    // VK_NV_inherited_viewport_scissor structs
    VkPhysicalDeviceInheritedViewportScissorFeaturesNV physical_device_inherited_viewport_scissor_features_;

    // VK_KHR_shader_integer_dot_product structs
    VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR physical_device_shader_integer_dot_product_properties_;
    VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR physical_device_shader_integer_dot_product_features_;

    // VK_EXT_texel_buffer_alignment structs
    VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT physical_device_texel_buffer_alignment_properties_;
    VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT physical_device_texel_buffer_alignment_features_;

    // VK_EXT_device_memory_report structs
    VkPhysicalDeviceDeviceMemoryReportFeaturesEXT physical_device_device_memory_report_features_;

    // VK_EXT_robustness2 structs
    VkPhysicalDeviceRobustness2PropertiesEXT physical_device_robustness_2_properties_;
    VkPhysicalDeviceRobustness2FeaturesEXT physical_device_robustness_2_features_;

    // VK_EXT_custom_border_color structs
    VkPhysicalDeviceCustomBorderColorPropertiesEXT physical_device_custom_border_color_properties_;
    VkPhysicalDeviceCustomBorderColorFeaturesEXT physical_device_custom_border_color_features_;

    // VK_KHR_present_id structs
    VkPhysicalDevicePresentIdFeaturesKHR physical_device_present_id_features_;

    // VK_EXT_private_data structs
    VkPhysicalDevicePrivateDataFeaturesEXT physical_device_private_data_features_;

    // VK_EXT_pipeline_creation_cache_control structs
    VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT physical_device_pipeline_creation_cache_control_features_;

    // VK_NV_device_diagnostics_config structs
    VkPhysicalDeviceDiagnosticsConfigFeaturesNV physical_device_diagnostics_config_features_;

    // VK_KHR_synchronization2 structs
    VkPhysicalDeviceSynchronization2FeaturesKHR physical_device_synchronization_2_features_;

    // VK_EXT_graphics_pipeline_library structs
    VkPhysicalDeviceGraphicsPipelineLibraryPropertiesEXT physical_device_graphics_pipeline_library_properties_;
    VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT physical_device_graphics_pipeline_library_features_;

    // VK_AMD_shader_early_and_late_fragment_tests structs
    VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD physical_device_shader_early_and_late_fragment_tests_features_;

    // VK_KHR_fragment_shader_barycentric structs
    VkPhysicalDeviceFragmentShaderBarycentricPropertiesKHR physical_device_fragment_shader_barycentric_properties_;

    // VK_KHR_shader_subgroup_uniform_control_flow structs
    VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR physical_device_shader_subgroup_uniform_control_flow_features_;

    // VK_KHR_zero_initialize_workgroup_memory structs
    VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR physical_device_zero_initialize_workgroup_memory_features_;

    // VK_NV_fragment_shading_rate_enums structs
    VkPhysicalDeviceFragmentShadingRateEnumsPropertiesNV physical_device_fragment_shading_rate_enums_properties_;
    VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV physical_device_fragment_shading_rate_enums_features_;

    // VK_NV_ray_tracing_motion_blur structs
    VkPhysicalDeviceRayTracingMotionBlurFeaturesNV physical_device_ray_tracing_motion_blur_features_;

    // VK_EXT_ycbcr_2plane_444_formats structs
    VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT physical_device_ycbcr_2_plane_444_formats_features_;

    // VK_EXT_fragment_density_map2 structs
    VkPhysicalDeviceFragmentDensityMap2PropertiesEXT physical_device_fragment_density_map_2_properties_;
    VkPhysicalDeviceFragmentDensityMap2FeaturesEXT physical_device_fragment_density_map_2_features_;

    // VK_EXT_image_robustness structs
    VkPhysicalDeviceImageRobustnessFeaturesEXT physical_device_image_robustness_features_;

    // VK_KHR_workgroup_memory_explicit_layout structs
    VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR physical_device_workgroup_memory_explicit_layout_features_;

    // VK_EXT_image_compression_control structs
    VkPhysicalDeviceImageCompressionControlFeaturesEXT physical_device_image_compression_control_features_;

    // VK_EXT_attachment_feedback_loop_layout structs
    VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT physical_device_attachment_feedback_loop_layout_features_;

    // VK_EXT_4444_formats structs
    VkPhysicalDevice4444FormatsFeaturesEXT physical_device_4444_formats_features_;

    // VK_ARM_rasterization_order_attachment_access structs
    VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM physical_device_rasterization_order_attachment_access_features_;

    // VK_EXT_rgba10x6_formats structs
    VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT physical_device_rgba_10_x_6_formats_features_;

    // VK_VALVE_mutable_descriptor_type structs
    VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE physical_device_mutable_descriptor_type_features_;

    // VK_EXT_vertex_input_dynamic_state structs
    VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT physical_device_vertex_input_dynamic_state_features_;

    // VK_EXT_physical_device_drm structs
    VkPhysicalDeviceDrmPropertiesEXT physical_device_drm_properties_;

    // VK_EXT_depth_clip_control structs
    VkPhysicalDeviceDepthClipControlFeaturesEXT physical_device_depth_clip_control_features_;

    // VK_EXT_primitive_topology_list_restart structs
    VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT physical_device_primitive_topology_list_restart_features_;

    // VK_HUAWEI_subpass_shading structs
    VkPhysicalDeviceSubpassShadingPropertiesHUAWEI physical_device_subpass_shading_properties_;
    VkPhysicalDeviceSubpassShadingFeaturesHUAWEI physical_device_subpass_shading_features_;

    // VK_HUAWEI_invocation_mask structs
    VkPhysicalDeviceInvocationMaskFeaturesHUAWEI physical_device_invocation_mask_features_;

    // VK_NV_external_memory_rdma structs
    VkPhysicalDeviceExternalMemoryRDMAFeaturesNV physical_device_external_memory_rdmafeatures_;

    // VK_EXT_pipeline_properties structs
    VkPhysicalDevicePipelinePropertiesFeaturesEXT physical_device_pipeline_properties_features_;

    // VK_EXT_multisampled_render_to_single_sampled structs
    VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT physical_device_multisampled_render_to_single_sampled_features_;

    // VK_EXT_extended_dynamic_state2 structs
    VkPhysicalDeviceExtendedDynamicState2FeaturesEXT physical_device_extended_dynamic_state_2_features_;

    // VK_EXT_color_write_enable structs
    VkPhysicalDeviceColorWriteEnableFeaturesEXT physical_device_color_write_enable_features_;

    // VK_EXT_primitives_generated_query structs
    VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT physical_device_primitives_generated_query_features_;

    // VK_KHR_ray_tracing_maintenance1 structs
    VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR physical_device_ray_tracing_maintenance_1_features_;

    // VK_EXT_global_priority_query structs
    VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT physical_device_global_priority_query_features_;

    // VK_EXT_image_view_min_lod structs
    VkPhysicalDeviceImageViewMinLodFeaturesEXT physical_device_image_view_min_lod_features_;

    // VK_EXT_multi_draw structs
    VkPhysicalDeviceMultiDrawPropertiesEXT physical_device_multi_draw_properties_;
    VkPhysicalDeviceMultiDrawFeaturesEXT physical_device_multi_draw_features_;

    // VK_EXT_image_2d_view_of_3d structs
    VkPhysicalDeviceImage2DViewOf3DFeaturesEXT physical_device_image_2_dview_of_3_dfeatures_;

    // VK_EXT_border_color_swizzle structs
    VkPhysicalDeviceBorderColorSwizzleFeaturesEXT physical_device_border_color_swizzle_features_;

    // VK_EXT_pageable_device_local_memory structs
    VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT physical_device_pageable_device_local_memory_features_;

    // VK_KHR_maintenance4 structs
    VkPhysicalDeviceMaintenance4PropertiesKHR physical_device_maintenance_4_properties_;
    VkPhysicalDeviceMaintenance4FeaturesKHR physical_device_maintenance_4_features_;

    // VK_VALVE_descriptor_set_host_mapping structs
    VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE physical_device_descriptor_set_host_mapping_features_;

    // VK_EXT_non_seamless_cube_map structs
    VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT physical_device_non_seamless_cube_map_features_;

    // VK_QCOM_fragment_density_map_offset structs
    VkPhysicalDeviceFragmentDensityMapOffsetPropertiesQCOM physical_device_fragment_density_map_offset_properties_;
    VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM physical_device_fragment_density_map_offset_features_;

    // VK_NV_linear_color_attachment structs
    VkPhysicalDeviceLinearColorAttachmentFeaturesNV physical_device_linear_color_attachment_features_;

    // VK_EXT_image_compression_control_swapchain structs
    VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT physical_device_image_compression_control_swapchain_features_;

    // VK_QCOM_image_processing structs
    VkPhysicalDeviceImageProcessingPropertiesQCOM physical_device_image_processing_properties_;
    VkPhysicalDeviceImageProcessingFeaturesQCOM physical_device_image_processing_features_;

    // VK_EXT_subpass_merge_feedback structs
    VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT physical_device_subpass_merge_feedback_features_;

    // VK_EXT_shader_module_identifier structs
    VkPhysicalDeviceShaderModuleIdentifierPropertiesEXT physical_device_shader_module_identifier_properties_;
    VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT physical_device_shader_module_identifier_features_;

    // VK_QCOM_tile_properties structs
    VkPhysicalDeviceTilePropertiesFeaturesQCOM physical_device_tile_properties_features_;

    // VK_SEC_amigo_profiling structs
    VkPhysicalDeviceAmigoProfilingFeaturesSEC physical_device_amigo_profiling_features_;

  private:
    PhysicalDeviceData() = delete;
    PhysicalDeviceData &operator=(const PhysicalDeviceData &) = delete;
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

        // Core properties
        physical_device_subgroup_properties_ = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES };
        physical_device_protected_memory_properties_ = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_PROPERTIES };
        physical_device_vulkan_11_properties_ = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES };
        physical_device_vulkan_12_properties_ = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES };
        physical_device_vulkan_13_properties_ = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES };

        // Core features
        physical_device_protected_memory_features_ = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES };
        physical_device_shader_draw_parameter_features_ = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETER_FEATURES };
        physical_device_vulkan_11_features_ = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES };
        physical_device_vulkan_12_features_ = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
        physical_device_vulkan_13_features_ = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };

        // VK_EXT_transform_feedback structs
        physical_device_transform_feedback_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT};
        physical_device_transform_feedback_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_FEATURES_EXT};

        // VK_KHR_dynamic_rendering structs
        physical_device_dynamic_rendering_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR};

        // VK_NV_corner_sampled_image structs
        physical_device_corner_sampled_image_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CORNER_SAMPLED_IMAGE_FEATURES_NV};

        // VK_KHR_multiview structs
        physical_device_multiview_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR};
        physical_device_multiview_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR};

        // VK_EXT_texture_compression_astc_hdr structs
        physical_device_texture_compression_astchdrfeatures_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_HDR_FEATURES_EXT};

        // VK_EXT_astc_decode_mode structs
        physical_device_astcdecode_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ASTC_DECODE_FEATURES_EXT};

        // VK_EXT_pipeline_robustness structs
        physical_device_pipeline_robustness_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_ROBUSTNESS_PROPERTIES_EXT};
        physical_device_pipeline_robustness_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_ROBUSTNESS_FEATURES_EXT};

        // VK_KHR_external_memory_capabilities structs
        physical_device_idproperties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES_KHR};

        // VK_KHR_push_descriptor structs
        physical_device_push_descriptor_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR};

        // VK_EXT_conditional_rendering structs
        physical_device_conditional_rendering_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONDITIONAL_RENDERING_FEATURES_EXT};

        // VK_KHR_shader_float16_int8 structs
        physical_device_float_16_int_8_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT16_INT8_FEATURES_KHR};

        // VK_KHR_16bit_storage structs
        physical_device_16_bit_storage_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES_KHR};

        // VK_NVX_multiview_per_view_attributes structs
        physical_device_multiview_per_view_attributes_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PER_VIEW_ATTRIBUTES_PROPERTIES_NVX};

        // VK_EXT_discard_rectangles structs
        physical_device_discard_rectangle_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DISCARD_RECTANGLE_PROPERTIES_EXT};

        // VK_EXT_conservative_rasterization structs
        physical_device_conservative_rasterization_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONSERVATIVE_RASTERIZATION_PROPERTIES_EXT};

        // VK_EXT_depth_clip_enable structs
        physical_device_depth_clip_enable_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT};

        // VK_KHR_imageless_framebuffer structs
        physical_device_imageless_framebuffer_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR};

        // VK_KHR_performance_query structs
        physical_device_performance_query_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_PROPERTIES_KHR};
        physical_device_performance_query_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_FEATURES_KHR};

        // VK_KHR_maintenance2 structs
        physical_device_point_clipping_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_POINT_CLIPPING_PROPERTIES_KHR};

        // VK_KHR_variable_pointers structs
        physical_device_variable_pointer_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES};

        // VK_EXT_sampler_filter_minmax structs
        physical_device_sampler_filter_minmax_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_FILTER_MINMAX_PROPERTIES_EXT};

        // VK_EXT_inline_uniform_block structs
        physical_device_inline_uniform_block_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_PROPERTIES_EXT};
        physical_device_inline_uniform_block_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT};

        // VK_EXT_sample_locations structs
        physical_device_sample_locations_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLE_LOCATIONS_PROPERTIES_EXT};

        // VK_EXT_blend_operation_advanced structs
        physical_device_blend_operation_advanced_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_PROPERTIES_EXT};
        physical_device_blend_operation_advanced_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT};

        // VK_KHR_acceleration_structure structs
        physical_device_acceleration_structure_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR};
        physical_device_acceleration_structure_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR};

        // VK_KHR_ray_tracing_pipeline structs
        physical_device_ray_tracing_pipeline_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR};
        physical_device_ray_tracing_pipeline_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR};

        // VK_KHR_ray_query structs
        physical_device_ray_query_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR};

        // VK_NV_shader_sm_builtins structs
        physical_device_shader_smbuiltins_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_PROPERTIES_NV};
        physical_device_shader_smbuiltins_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_FEATURES_NV};

        // VK_KHR_sampler_ycbcr_conversion structs
        physical_device_sampler_ycbcr_conversion_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR};

        // VK_EXT_descriptor_indexing structs
        physical_device_descriptor_indexing_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT};
        physical_device_descriptor_indexing_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT};

        // VK_KHR_portability_subset structs
        physical_device_portability_subset_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES_KHR};
        physical_device_portability_subset_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR};

        // VK_NV_shading_rate_image structs
        physical_device_shading_rate_image_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_PROPERTIES_NV};
        physical_device_shading_rate_image_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_FEATURES_NV};

        // VK_NV_ray_tracing structs
        physical_device_ray_tracing_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_NV};

        // VK_NV_representative_fragment_test structs
        physical_device_representative_fragment_test_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_REPRESENTATIVE_FRAGMENT_TEST_FEATURES_NV};

        // VK_KHR_maintenance3 structs
        physical_device_maintenance_3_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES_KHR};

        // VK_KHR_shader_subgroup_extended_types structs
        physical_device_shader_subgroup_extended_types_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR};

        // VK_KHR_8bit_storage structs
        physical_device_8_bit_storage_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR};

        // VK_EXT_external_memory_host structs
        physical_device_external_memory_host_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_HOST_PROPERTIES_EXT};

        // VK_KHR_shader_atomic_int64 structs
        physical_device_shader_atomic_int_64_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES_KHR};

        // VK_KHR_shader_clock structs
        physical_device_shader_clock_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR};

        // VK_AMD_shader_core_properties structs
        physical_device_shader_core_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_AMD};

        // VK_EXT_vertex_attribute_divisor structs
        physical_device_vertex_attribute_divisor_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_PROPERTIES_EXT};
        physical_device_vertex_attribute_divisor_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT};

        // VK_KHR_driver_properties structs
        physical_device_driver_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES_KHR};

        // VK_KHR_shader_float_controls structs
        physical_device_float_controls_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT_CONTROLS_PROPERTIES_KHR};

        // VK_KHR_depth_stencil_resolve structs
        physical_device_depth_stencil_resolve_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR};

        // VK_NV_compute_shader_derivatives structs
        physical_device_compute_shader_derivatives_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMPUTE_SHADER_DERIVATIVES_FEATURES_NV};

        // VK_NV_mesh_shader structs
        physical_device_mesh_shader_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_NV};
        physical_device_mesh_shader_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_NV};

        // VK_NV_fragment_shader_barycentric structs
        physical_device_fragment_shader_barycentric_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_BARYCENTRIC_FEATURES_NV};

        // VK_NV_shader_image_footprint structs
        physical_device_shader_image_footprint_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_FOOTPRINT_FEATURES_NV};

        // VK_NV_scissor_exclusive structs
        physical_device_exclusive_scissor_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXCLUSIVE_SCISSOR_FEATURES_NV};

        // VK_KHR_timeline_semaphore structs
        physical_device_timeline_semaphore_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_PROPERTIES_KHR};
        physical_device_timeline_semaphore_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR};

        // VK_INTEL_shader_integer_functions2 structs
        physical_device_shader_integer_functions_2_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_FUNCTIONS_2_FEATURES_INTEL};

        // VK_KHR_vulkan_memory_model structs
        physical_device_vulkan_memory_model_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES_KHR};

        // VK_EXT_pci_bus_info structs
        physical_device_pcibus_info_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PCI_BUS_INFO_PROPERTIES_EXT};

        // VK_KHR_shader_terminate_invocation structs
        physical_device_shader_terminate_invocation_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES_KHR};

        // VK_EXT_fragment_density_map structs
        physical_device_fragment_density_map_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_PROPERTIES_EXT};
        physical_device_fragment_density_map_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_FEATURES_EXT};

        // VK_EXT_scalar_block_layout structs
        physical_device_scalar_block_layout_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES_EXT};

        // VK_EXT_subgroup_size_control structs
        physical_device_subgroup_size_control_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES_EXT};
        physical_device_subgroup_size_control_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES_EXT};

        // VK_KHR_fragment_shading_rate structs
        physical_device_fragment_shading_rate_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR};
        physical_device_fragment_shading_rate_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR};

        // VK_AMD_shader_core_properties2 structs
        physical_device_shader_core_properties_2_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_2_AMD};

        // VK_AMD_device_coherent_memory structs
        physical_device_coherent_memory_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COHERENT_MEMORY_FEATURES_AMD};

        // VK_EXT_shader_image_atomic_int64 structs
        physical_device_shader_image_atomic_int_64_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_ATOMIC_INT64_FEATURES_EXT};

        // VK_EXT_memory_priority structs
        physical_device_memory_priority_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PRIORITY_FEATURES_EXT};

        // VK_NV_dedicated_allocation_image_aliasing structs
        physical_device_dedicated_allocation_image_aliasing_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEDICATED_ALLOCATION_IMAGE_ALIASING_FEATURES_NV};

        // VK_KHR_separate_depth_stencil_layouts structs
        physical_device_separate_depth_stencil_layouts_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES_KHR};

        // VK_EXT_buffer_device_address structs
        physical_device_buffer_address_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_ADDRESS_FEATURES_EXT};

        // VK_KHR_present_wait structs
        physical_device_present_wait_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_WAIT_FEATURES_KHR};

        // VK_NV_cooperative_matrix structs
        physical_device_cooperative_matrix_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_PROPERTIES_NV};
        physical_device_cooperative_matrix_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_NV};

        // VK_NV_coverage_reduction_mode structs
        physical_device_coverage_reduction_mode_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COVERAGE_REDUCTION_MODE_FEATURES_NV};

        // VK_EXT_fragment_shader_interlock structs
        physical_device_fragment_shader_interlock_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT};

        // VK_EXT_ycbcr_image_arrays structs
        physical_device_ycbcr_image_arrays_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_IMAGE_ARRAYS_FEATURES_EXT};

        // VK_KHR_uniform_buffer_standard_layout structs
        physical_device_uniform_buffer_standard_layout_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR};

        // VK_EXT_provoking_vertex structs
        physical_device_provoking_vertex_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_PROPERTIES_EXT};
        physical_device_provoking_vertex_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_FEATURES_EXT};

        // VK_KHR_buffer_device_address structs
        physical_device_buffer_device_address_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR};

        // VK_EXT_line_rasterization structs
        physical_device_line_rasterization_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_PROPERTIES_EXT};
        physical_device_line_rasterization_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES_EXT};

        // VK_EXT_shader_atomic_float structs
        physical_device_shader_atomic_float_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT};

        // VK_EXT_host_query_reset structs
        physical_device_host_query_reset_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT};

        // VK_EXT_index_type_uint8 structs
        physical_device_index_type_uint_8_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES_EXT};

        // VK_EXT_extended_dynamic_state structs
        physical_device_extended_dynamic_state_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT};

        // VK_KHR_pipeline_executable_properties structs
        physical_device_pipeline_executable_properties_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_EXECUTABLE_PROPERTIES_FEATURES_KHR};

        // VK_EXT_shader_atomic_float2 structs
        physical_device_shader_atomic_float_2_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_2_FEATURES_EXT};

        // VK_EXT_shader_demote_to_helper_invocation structs
        physical_device_shader_demote_to_helper_invocation_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES_EXT};

        // VK_NV_device_generated_commands structs
        physical_device_device_generated_commands_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_PROPERTIES_NV};
        physical_device_device_generated_commands_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_FEATURES_NV};

        // VK_NV_inherited_viewport_scissor structs
        physical_device_inherited_viewport_scissor_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INHERITED_VIEWPORT_SCISSOR_FEATURES_NV};

        // VK_KHR_shader_integer_dot_product structs
        physical_device_shader_integer_dot_product_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_PROPERTIES_KHR};
        physical_device_shader_integer_dot_product_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_FEATURES_KHR};

        // VK_EXT_texel_buffer_alignment structs
        physical_device_texel_buffer_alignment_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_PROPERTIES_EXT};
        physical_device_texel_buffer_alignment_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT};

        // VK_EXT_device_memory_report structs
        physical_device_device_memory_report_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_MEMORY_REPORT_FEATURES_EXT};

        // VK_EXT_robustness2 structs
        physical_device_robustness_2_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_PROPERTIES_EXT};
        physical_device_robustness_2_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT};

        // VK_EXT_custom_border_color structs
        physical_device_custom_border_color_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_PROPERTIES_EXT};
        physical_device_custom_border_color_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_FEATURES_EXT};

        // VK_KHR_present_id structs
        physical_device_present_id_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_ID_FEATURES_KHR};

        // VK_EXT_private_data structs
        physical_device_private_data_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES_EXT};

        // VK_EXT_pipeline_creation_cache_control structs
        physical_device_pipeline_creation_cache_control_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES_EXT};

        // VK_NV_device_diagnostics_config structs
        physical_device_diagnostics_config_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DIAGNOSTICS_CONFIG_FEATURES_NV};

        // VK_KHR_synchronization2 structs
        physical_device_synchronization_2_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR};

        // VK_EXT_graphics_pipeline_library structs
        physical_device_graphics_pipeline_library_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GRAPHICS_PIPELINE_LIBRARY_PROPERTIES_EXT};
        physical_device_graphics_pipeline_library_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GRAPHICS_PIPELINE_LIBRARY_FEATURES_EXT};

        // VK_AMD_shader_early_and_late_fragment_tests structs
        physical_device_shader_early_and_late_fragment_tests_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_EARLY_AND_LATE_FRAGMENT_TESTS_FEATURES_AMD};

        // VK_KHR_fragment_shader_barycentric structs
        physical_device_fragment_shader_barycentric_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_BARYCENTRIC_PROPERTIES_KHR};

        // VK_KHR_shader_subgroup_uniform_control_flow structs
        physical_device_shader_subgroup_uniform_control_flow_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_FEATURES_KHR};

        // VK_KHR_zero_initialize_workgroup_memory structs
        physical_device_zero_initialize_workgroup_memory_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_WORKGROUP_MEMORY_FEATURES_KHR};

        // VK_NV_fragment_shading_rate_enums structs
        physical_device_fragment_shading_rate_enums_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_ENUMS_PROPERTIES_NV};
        physical_device_fragment_shading_rate_enums_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_ENUMS_FEATURES_NV};

        // VK_NV_ray_tracing_motion_blur structs
        physical_device_ray_tracing_motion_blur_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_MOTION_BLUR_FEATURES_NV};

        // VK_EXT_ycbcr_2plane_444_formats structs
        physical_device_ycbcr_2_plane_444_formats_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_2_PLANE_444_FORMATS_FEATURES_EXT};

        // VK_EXT_fragment_density_map2 structs
        physical_device_fragment_density_map_2_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_PROPERTIES_EXT};
        physical_device_fragment_density_map_2_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_FEATURES_EXT};

        // VK_EXT_image_robustness structs
        physical_device_image_robustness_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES_EXT};

        // VK_KHR_workgroup_memory_explicit_layout structs
        physical_device_workgroup_memory_explicit_layout_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_FEATURES_KHR};

        // VK_EXT_image_compression_control structs
        physical_device_image_compression_control_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_COMPRESSION_CONTROL_FEATURES_EXT};

        // VK_EXT_attachment_feedback_loop_layout structs
        physical_device_attachment_feedback_loop_layout_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ATTACHMENT_FEEDBACK_LOOP_LAYOUT_FEATURES_EXT};

        // VK_EXT_4444_formats structs
        physical_device_4444_formats_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_4444_FORMATS_FEATURES_EXT};

        // VK_ARM_rasterization_order_attachment_access structs
        physical_device_rasterization_order_attachment_access_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RASTERIZATION_ORDER_ATTACHMENT_ACCESS_FEATURES_ARM};

        // VK_EXT_rgba10x6_formats structs
        physical_device_rgba_10_x_6_formats_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RGBA10X6_FORMATS_FEATURES_EXT};

        // VK_VALVE_mutable_descriptor_type structs
        physical_device_mutable_descriptor_type_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MUTABLE_DESCRIPTOR_TYPE_FEATURES_VALVE};

        // VK_EXT_vertex_input_dynamic_state structs
        physical_device_vertex_input_dynamic_state_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_INPUT_DYNAMIC_STATE_FEATURES_EXT};

        // VK_EXT_physical_device_drm structs
        physical_device_drm_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRM_PROPERTIES_EXT};

        // VK_EXT_depth_clip_control structs
        physical_device_depth_clip_control_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_CONTROL_FEATURES_EXT};

        // VK_EXT_primitive_topology_list_restart structs
        physical_device_primitive_topology_list_restart_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVE_TOPOLOGY_LIST_RESTART_FEATURES_EXT};

        // VK_HUAWEI_subpass_shading structs
        physical_device_subpass_shading_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_SHADING_PROPERTIES_HUAWEI};
        physical_device_subpass_shading_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_SHADING_FEATURES_HUAWEI};

        // VK_HUAWEI_invocation_mask structs
        physical_device_invocation_mask_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INVOCATION_MASK_FEATURES_HUAWEI};

        // VK_NV_external_memory_rdma structs
        physical_device_external_memory_rdmafeatures_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_RDMA_FEATURES_NV};

        // VK_EXT_pipeline_properties structs
        physical_device_pipeline_properties_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_PROPERTIES_FEATURES_EXT};

        // VK_EXT_multisampled_render_to_single_sampled structs
        physical_device_multisampled_render_to_single_sampled_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTISAMPLED_RENDER_TO_SINGLE_SAMPLED_FEATURES_EXT};

        // VK_EXT_extended_dynamic_state2 structs
        physical_device_extended_dynamic_state_2_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT};

        // VK_EXT_color_write_enable structs
        physical_device_color_write_enable_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COLOR_WRITE_ENABLE_FEATURES_EXT};

        // VK_EXT_primitives_generated_query structs
        physical_device_primitives_generated_query_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVES_GENERATED_QUERY_FEATURES_EXT};

        // VK_KHR_ray_tracing_maintenance1 structs
        physical_device_ray_tracing_maintenance_1_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_MAINTENANCE_1_FEATURES_KHR};

        // VK_EXT_global_priority_query structs
        physical_device_global_priority_query_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GLOBAL_PRIORITY_QUERY_FEATURES_EXT};

        // VK_EXT_image_view_min_lod structs
        physical_device_image_view_min_lod_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_VIEW_MIN_LOD_FEATURES_EXT};

        // VK_EXT_multi_draw structs
        physical_device_multi_draw_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_PROPERTIES_EXT};
        physical_device_multi_draw_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_FEATURES_EXT};

        // VK_EXT_image_2d_view_of_3d structs
        physical_device_image_2_dview_of_3_dfeatures_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_2D_VIEW_OF_3D_FEATURES_EXT};

        // VK_EXT_border_color_swizzle structs
        physical_device_border_color_swizzle_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BORDER_COLOR_SWIZZLE_FEATURES_EXT};

        // VK_EXT_pageable_device_local_memory structs
        physical_device_pageable_device_local_memory_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PAGEABLE_DEVICE_LOCAL_MEMORY_FEATURES_EXT};

        // VK_KHR_maintenance4 structs
        physical_device_maintenance_4_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_PROPERTIES_KHR};
        physical_device_maintenance_4_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES_KHR};

        // VK_VALVE_descriptor_set_host_mapping structs
        physical_device_descriptor_set_host_mapping_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_SET_HOST_MAPPING_FEATURES_VALVE};

        // VK_EXT_non_seamless_cube_map structs
        physical_device_non_seamless_cube_map_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_NON_SEAMLESS_CUBE_MAP_FEATURES_EXT};

        // VK_QCOM_fragment_density_map_offset structs
        physical_device_fragment_density_map_offset_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_OFFSET_PROPERTIES_QCOM};
        physical_device_fragment_density_map_offset_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_OFFSET_FEATURES_QCOM};

        // VK_NV_linear_color_attachment structs
        physical_device_linear_color_attachment_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINEAR_COLOR_ATTACHMENT_FEATURES_NV};

        // VK_EXT_image_compression_control_swapchain structs
        physical_device_image_compression_control_swapchain_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_COMPRESSION_CONTROL_SWAPCHAIN_FEATURES_EXT};

        // VK_QCOM_image_processing structs
        physical_device_image_processing_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_PROCESSING_PROPERTIES_QCOM};
        physical_device_image_processing_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_PROCESSING_FEATURES_QCOM};

        // VK_EXT_subpass_merge_feedback structs
        physical_device_subpass_merge_feedback_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_MERGE_FEEDBACK_FEATURES_EXT};

        // VK_EXT_shader_module_identifier structs
        physical_device_shader_module_identifier_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MODULE_IDENTIFIER_PROPERTIES_EXT};
        physical_device_shader_module_identifier_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MODULE_IDENTIFIER_FEATURES_EXT};

        // VK_QCOM_tile_properties structs
        physical_device_tile_properties_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TILE_PROPERTIES_FEATURES_QCOM};

        // VK_SEC_amigo_profiling structs
        physical_device_amigo_profiling_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_AMIGO_PROFILING_FEATURES_SEC};
    }

    const VkInstance instance_;

    typedef std::unordered_map<VkPhysicalDevice, PhysicalDeviceData> Map;
    static Map map_;
};

PhysicalDeviceData::Map PhysicalDeviceData::map_;

// Loader for Profiles JSON configuration files ////////////////////////////////////////////////////////////////////////////////////

class JsonLoader {
   public:
    JsonLoader()
        : pdd_(nullptr),
          profile_filename_(),
          root_(Json::nullValue),
          profile_api_version_(0),
          excluded_extensions_(),
          excluded_formats_() {}
    JsonLoader(const JsonLoader &) = delete;
    JsonLoader &operator=(const JsonLoader &rhs) {
        if (this == &rhs) {
            return *this;
        }
        pdd_ = rhs.pdd_;
        profile_filename_ = rhs.profile_filename_;
        root_ = rhs.root_;
        profile_api_version_ = rhs.profile_api_version_;
        excluded_extensions_ = rhs.excluded_extensions_;
        excluded_formats_ = rhs.excluded_formats_;

        return *this;
    }

    static JsonLoader &Create() {
        LogMessage(DEBUG_REPORT_DEBUG_BIT, "JsonLoader::Create()\n");

        VkInstance temporary = VK_NULL_HANDLE;
        const auto result = profile_map_.emplace(std::piecewise_construct, std::make_tuple(temporary), std::make_tuple());
        assert(result.second);  // true=insertion, false=replacement
        auto iter = result.first;
        JsonLoader *profile = &iter->second;
        return *profile;
    }

    static void Store(VkInstance instance) {
        profile_map_[instance] = profile_map_[VK_NULL_HANDLE];
        profile_map_.erase(VK_NULL_HANDLE);
    }

    static JsonLoader *Find(VkInstance instance) {
        const auto iter = profile_map_.find(instance);
        return (iter != profile_map_.end()) ? &iter->second : nullptr;
    }

    static void Destroy(VkInstance instance) {
        LogMessage(DEBUG_REPORT_DEBUG_BIT, "JsonLoader::Destroy()\n");
        profile_map_.erase(instance);
    }

    VkResult LoadFile(std::string filename);
    void ReadProfileApiVersion();
    VkResult LoadDevice(PhysicalDeviceData *pdd);
    VkResult ReadProfile(const Json::Value root, const std::vector<std::string> &capabilities);
    uint32_t GetProfileApiVersion() const { return profile_api_version_; }

   private:
    PhysicalDeviceData *pdd_;

    std::string profile_filename_;
    Json::Value root_;

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
    bool GetFeature(const Json::Value &features, const std::string &name);
    bool GetProperty(const Json::Value &props, const std::string &name);
    bool GetFormat(const Json::Value &formats, const std::string &format_name, ArrayOfVkFormatProperties *dest,
                   ArrayOfVkFormatProperties3 *dest3);
    bool CheckVersionSupport(uint32_t version, const std::string &name);
    ExtensionSupport CheckExtensionSupport(const char *extension, const std::string &name);
    bool valid(ExtensionSupport support);
    bool GetQueueFamilyProperties(const Json::Value &qf_props, QueueFamilyProperties *dest);
    bool OrderQueueFamilyProperties(ArrayOfVkQueueFamilyProperties *qfp);
    void AddPromotedExtensions(uint32_t api_level);

    bool GetValue(const Json::Value &parent, const std::string &member, const char *name, VkExtent2D *dest);
    bool GetValue(const Json::Value &parent, const std::string &member, const char *name, VkExtent2D *dest,
                  std::function<bool(const char *, uint32_t, uint32_t)> warn_func);
    bool GetValue(const Json::Value &parent, const std::string &member, const char *name, VkExtent3D *dest);

    bool GetValue(const Json::Value &parent, VkPhysicalDeviceSubgroupProperties *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceProtectedMemoryProperties *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceVulkan11Properties *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceVulkan12Properties *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceVulkan13Properties *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceProtectedMemoryFeatures *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderDrawParameterFeatures *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceVulkan11Features *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceVulkan12Features *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceVulkan13Features *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceTransformFeedbackPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceTransformFeedbackFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDynamicRenderingFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceCornerSampledImageFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceMultiviewPropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceMultiviewFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceASTCDecodeFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePipelineRobustnessPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePipelineRobustnessFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceIDPropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePushDescriptorPropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceConditionalRenderingFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFloat16Int8FeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevice16BitStorageFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceMultiviewPerViewAttributesPropertiesNVX *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDiscardRectanglePropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceConservativeRasterizationPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDepthClipEnableFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceImagelessFramebufferFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePerformanceQueryPropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePerformanceQueryFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePointClippingPropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceVariablePointerFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceInlineUniformBlockPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceInlineUniformBlockFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceSampleLocationsPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceAccelerationStructurePropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceAccelerationStructureFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceRayTracingPipelinePropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceRayTracingPipelineFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceRayQueryFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderSMBuiltinsPropertiesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderSMBuiltinsFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDescriptorIndexingPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDescriptorIndexingFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePortabilitySubsetPropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePortabilitySubsetFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShadingRateImagePropertiesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShadingRateImageFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceRayTracingPropertiesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceMaintenance3PropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevice8BitStorageFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceExternalMemoryHostPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderAtomicInt64FeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderClockFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderCorePropertiesAMD *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDriverPropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFloatControlsPropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDepthStencilResolvePropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceComputeShaderDerivativesFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceMeshShaderPropertiesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceMeshShaderFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderImageFootprintFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceExclusiveScissorFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceTimelineSemaphorePropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceTimelineSemaphoreFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceVulkanMemoryModelFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePCIBusInfoPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentDensityMapPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentDensityMapFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceScalarBlockLayoutFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceSubgroupSizeControlPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceSubgroupSizeControlFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentShadingRatePropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentShadingRateFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderCoreProperties2AMD *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceCoherentMemoryFeaturesAMD *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceMemoryPriorityFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceBufferAddressFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePresentWaitFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceCooperativeMatrixPropertiesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceCooperativeMatrixFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceCoverageReductionModeFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceYcbcrImageArraysFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceProvokingVertexPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceProvokingVertexFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceBufferDeviceAddressFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceLineRasterizationPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceLineRasterizationFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderAtomicFloatFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceHostQueryResetFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceIndexTypeUint8FeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceExtendedDynamicStateFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDeviceGeneratedCommandsPropertiesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceInheritedViewportScissorFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDeviceMemoryReportFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceRobustness2PropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceRobustness2FeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceCustomBorderColorPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceCustomBorderColorFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePresentIdFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePrivateDataFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDiagnosticsConfigFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceSynchronization2FeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceGraphicsPipelineLibraryPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentShaderBarycentricPropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentShadingRateEnumsPropertiesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceRayTracingMotionBlurFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentDensityMap2PropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentDensityMap2FeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceImageRobustnessFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceImageCompressionControlFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevice4444FormatsFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDrmPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDepthClipControlFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceSubpassShadingPropertiesHUAWEI *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceSubpassShadingFeaturesHUAWEI *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceInvocationMaskFeaturesHUAWEI *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceExternalMemoryRDMAFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePipelinePropertiesFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceExtendedDynamicState2FeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceColorWriteEnableFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceImageViewMinLodFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceMultiDrawPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceMultiDrawFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceImage2DViewOf3DFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceBorderColorSwizzleFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceMaintenance4PropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceMaintenance4FeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentDensityMapOffsetPropertiesQCOM *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceLinearColorAttachmentFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceImageProcessingPropertiesQCOM *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceImageProcessingFeaturesQCOM *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderModuleIdentifierPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceTilePropertiesFeaturesQCOM *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceAmigoProfilingFeaturesSEC *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFeatures *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceProperties *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceLimits *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceSparseProperties *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceToolProperties *dest);

    static void WarnNotModifiable(const char *parent_name, const std::string &member, const char *name) {
        if (member != name) {
            return;
        }
        LogMessage(
            DEBUG_REPORT_WARNING_BIT,
            format("%s::%s value is set in the profile, but it is not modifiable by the Profiles Layer and will not be set.\n",
                   parent_name, name));
    }

    static bool WarnIfGreater(const char *name, const uint64_t new_value, const uint64_t old_value) {
        if (new_value > old_value) {
            LogMessage(
                DEBUG_REPORT_WARNING_BIT,
                format("%s profile value (%" PRIu64 ") is greater than device value (%" PRIu64 ")\n", name, new_value, old_value));
            return true;
        }
        return false;
    }

    static bool WarnIfGreaterSizet(const char *name, const size_t new_value, const size_t old_value) {
        if (new_value > old_value) {
            LogMessage(DEBUG_REPORT_WARNING_BIT,
                       format("%s profile value (%" PRIuLEAST64 ") is greater than device value (%" PRIuLEAST64 ")\n", name,
                              new_value, old_value));
            return true;
        }
        return false;
    }

    static bool WarnIfGreaterFloat(const char *name, const float new_value, const float old_value) {
        if (new_value > old_value) {
            LogMessage(DEBUG_REPORT_WARNING_BIT,
                       format("%s profile value (%3.2f) is greater than device value (%3.2f)\n", name, new_value, old_value));
            return true;
        }
        return false;
    }

    static bool WarnIfNotEqualBool(const char *name, const bool new_value, const bool old_value) {
        if (new_value && !old_value) {
            LogMessage(DEBUG_REPORT_WARNING_BIT,
                       format("%s profile value is enabled in the profile, but the device does not support it.\n", name));
            return true;
        }
        return false;
    }

    static bool WarnIfNotEqualEnum(const char *name, const uint32_t new_value, const uint32_t old_value) {
        if (new_value != old_value) {
            LogMessage(DEBUG_REPORT_WARNING_BIT,
                       format("%s profile value (%" PRIu32 ") is different from the device value (%" PRIu32 ").\n", name, new_value,
                              old_value));
            return true;
        }
        return false;
    }

    static bool WarnIfMissingBit(const char *name, const uint32_t new_value, const uint32_t old_value) {
        if ((old_value | new_value) != old_value) {
            LogMessage(DEBUG_REPORT_WARNING_BIT,
                       format("%s profile value (%" PRIu32 ") has bits set that the device value (%" PRIu32 ") does not\n", name,
                              new_value, old_value));
            return true;
        }
        return false;
    }

    static bool WarnIfMissingBit64(const char *name, const uint64_t new_value, const uint64_t old_value) {
        if ((old_value | new_value) != old_value) {
            LogMessage(DEBUG_REPORT_WARNING_BIT,
                       format("%s JSON value (%" PRIu64 ") has bits set that the existing value (%" PRIu64 ") does not\n", name,
                              new_value, old_value));
            return true;
        }
        return false;
    }

    static bool WarnIfLesser(const char *name, const uint64_t new_value, const uint64_t old_value) {
        if (new_value < old_value) {
            LogMessage(
                DEBUG_REPORT_WARNING_BIT,
                format("%s profile value (%" PRIu64 ") is lesser than device value (%" PRIu64 ")\n", name, new_value, old_value));
            return true;
        }
        return false;
    }

    static bool WarnIfLesserSizet(const char *name, const size_t new_value, const size_t old_value) {
        if (new_value < old_value) {
            LogMessage(DEBUG_REPORT_WARNING_BIT,
                       format("%s profile value (%" PRIuLEAST64 ") is lesser than device value (%" PRIuLEAST64 ")\n", name,
                              new_value, old_value));
            return true;
        }
        return false;
    }

    static bool WarnIfLesserFloat(const char *name, const float new_value, const float old_value) {
        if (new_value < old_value) {
            LogMessage(DEBUG_REPORT_WARNING_BIT,
                       format("%s profile value (%3.2f) is lesser than profile value (%3.2f)\n", name, new_value, old_value));
            return true;
        }
        return false;
    }

    bool GetValue(const Json::Value &parent, const std::string &member, const char *name, float *dest,
                  std::function<bool(const char *, float, float)> warn_func = nullptr) {
        if (member != name) {
            return true;
        }
        const Json::Value value = parent[name];
        if (!value.isDouble()) {
            return true;
        }
        bool valid = true;
        const float new_value = value.asFloat();
        if (warn_func) {
            if (warn_func(name, new_value, *dest)) {
                valid = false;
            }
        }
        *dest = new_value;
        return valid;
    }

    bool GetValue(const Json::Value &parent, const std::string &member, const char *name, int32_t *dest,
                  std::function<bool(const char *, int32_t, int32_t)> warn_func = nullptr) {
        if (member != name) {
            return true;
        }
        const Json::Value value = parent[name];
        if (!value.isInt()) {
            return true;
        }
        bool valid = true;
        const int32_t new_value = value.asInt();
        if (warn_func) {
            if (warn_func(name, new_value, *dest)) {
                valid = false;
            }
        }
        *dest = new_value;
        return valid;
    }

    bool GetValue(const Json::Value &parent, const std::string &member, const char *name, uint32_t *dest,
                  std::function<bool(const char *, uint32_t, uint32_t)> warn_func = nullptr) {
        if (member != name) {
            return true;
        }
        const Json::Value value = parent[name];
        bool valid = true;
        if (value.isBool()) {
            const bool new_value = value.asBool();
            if (warn_func) {
                if (warn_func(name, new_value, *dest)) {
                    valid = false;
                }
            }
            *dest = static_cast<uint32_t>(new_value);
        } else if (value.isArray()) {
            uint32_t sum_bits = 0;
            for (const auto &entry : value) {
                if (entry.isString()) {
                    sum_bits |= VkStringToUint(entry.asString());
                }
            }
            *dest = sum_bits;
        } else if (value.isUInt()) {
            const uint32_t new_value = value.asUInt();
            if (warn_func) {
                if (warn_func(name, new_value, *dest)) {
                    valid = false;
                }
            }
            *dest = new_value;
        }
        return valid;
    }

    bool GetValueSizet(const Json::Value &parent, const std::string &member, const char *name, size_t *dest,
                       std::function<bool(const char *, size_t, size_t)> warn_func = nullptr) {
        if (member != name) {
            return true;
        }
        const Json::Value value = parent[name];
        bool valid = true;
        if (value.isUInt()) {
            const size_t new_value = value.asUInt();
            if (warn_func) {
                if (warn_func(name, new_value, *dest)) {
                    valid = false;
                }
            }
            *dest = new_value;
        }
        return valid;
    }

    bool GetValue(const Json::Value &parent, const std::string &member, const char *name, uint64_t *dest,
                  std::function<bool(const char *, uint64_t, uint64_t)> warn_func = nullptr) {
        if (member != name) {
            return true;
        }
        const Json::Value value = parent[name];
        if (!value.isUInt64()) {
            return true;
        }
        bool valid = true;
        const uint64_t new_value = value.asUInt64();
        if (warn_func) {
            if (warn_func(name, new_value, *dest)) {
                valid = false;
            }
        }
        *dest = new_value;
        return valid;
    }

    template <typename T>  // for Vulkan enum types
    bool GetValueFlag(const Json::Value &parent, const std::string &member, const char *name, T *dest,
                      std::function<bool(const char *, T, T)> warn_func = nullptr) {
        if (member != name) {
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
        if (WarnIfMissingBit(name, new_value, static_cast<uint32_t>(*dest))) {
            valid = false;
        }
        *dest = static_cast<T>(new_value);
        return valid;
    }

    template <typename T>  // for Vulkan enum types
    bool GetValueEnum(const Json::Value &parent, const std::string &member, const char *name, T *dest,
                      std::function<bool(const char *, std::uint32_t, std::uint32_t)> warn_func = nullptr) {
        if (member != name) {
            return true;
        }
        const Json::Value value = parent[name];
        bool valid = true;
        uint32_t new_value = 0;
        if (value.isString()) {
            new_value = static_cast<T>(VkStringToUint(value.asString()));
        }
        if (warn_func) {
            if (warn_func(name, new_value, *dest)) {
                valid = false;
            }
        } else {
            if (WarnIfNotEqualEnum(name, new_value, *dest)) {
                valid = false;
            }
        }
        *dest = static_cast<T>(new_value);
        return valid;
    }

    int GetArray(const Json::Value &parent, const char *name, uint8_t *dest) {
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

    int GetArray(const Json::Value &parent, const char *name, uint32_t *dest) {
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

    int GetArray(const Json::Value &parent, const char *name, float *dest) {
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

    int GetArray(const Json::Value &parent, const char *name, char *dest) {
        const Json::Value value = parent[name];
        if (!value.isString()) {
            return -1;
        }
        const char *new_value = value.asCString();
        int count = 0;
        dest[0] = '\0';
        if (new_value) {
            count = static_cast<int>(strlen(new_value));
            strcpy(dest, new_value);
        }
        return count;
    }

    typedef std::unordered_map<VkInstance, JsonLoader> ProfileMap;
    static ProfileMap profile_map_;
};

JsonLoader::ProfileMap JsonLoader::profile_map_;

static bool IsASTCHDRFormat(VkFormat format) {
    switch (format) {
        case VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK:
        case VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK:
        case VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK:
        case VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK:
        case VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK:
        case VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK:
        case VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK:
        case VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK:
        case VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK:
        case VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK:
        case VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK:
        case VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK:
        case VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK:
        case VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK:
            return true;
        default:
            return false;
    }
}

static bool IsASTCLDRFormat(VkFormat format) {
    switch (format) {
        case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
        case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
        case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
        case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
        case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
        case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
        case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
        case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
        case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
        case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
        case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
        case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
        case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
        case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
        case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
        case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
        case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
        case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
        case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
        case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
        case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
        case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
        case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
        case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
        case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
        case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
        case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
        case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:
            return true;
        default:
            return false;
    }
}

static bool IsBCFormat(VkFormat format) {
    switch (format) {
        case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
        case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
        case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
        case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
        case VK_FORMAT_BC2_UNORM_BLOCK:
        case VK_FORMAT_BC2_SRGB_BLOCK:
        case VK_FORMAT_BC3_UNORM_BLOCK:
        case VK_FORMAT_BC3_SRGB_BLOCK:
        case VK_FORMAT_BC4_UNORM_BLOCK:
        case VK_FORMAT_BC4_SNORM_BLOCK:
        case VK_FORMAT_BC5_UNORM_BLOCK:
        case VK_FORMAT_BC5_SNORM_BLOCK:
        case VK_FORMAT_BC6H_UFLOAT_BLOCK:
        case VK_FORMAT_BC6H_SFLOAT_BLOCK:
        case VK_FORMAT_BC7_UNORM_BLOCK:
        case VK_FORMAT_BC7_SRGB_BLOCK:
            return true;
        default:
            return false;
    }
}

static bool IsEACFormat(VkFormat format) {
    switch (format) {
        case VK_FORMAT_EAC_R11_UNORM_BLOCK:
        case VK_FORMAT_EAC_R11_SNORM_BLOCK:
        case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
        case VK_FORMAT_EAC_R11G11_SNORM_BLOCK:
            return true;
        default:
            return false;
    }
}

static bool IsETC2Format(VkFormat format) {
    switch (format) {
        case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
        case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
        case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
        case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
        case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
        case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
            return true;
        default:
            return false;
    }
}

static bool IsPVRTCFormat(VkFormat format) {
    switch (format) {
        case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG:
        case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG:
        case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG:
        case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG:
        case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG:
        case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG:
        case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG:
        case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG:
            return true;
        default:
            return false;
    }
}

bool WarnDuplicated(const Json::Value &parent, const std::vector<std::string> &members) {
    std::vector<std::string> set;
    for (const auto &member : members) {
        if (parent.isMember(member)) {
            set.push_back(member);
        }
    }

    for (uint32_t i = 1; i < set.size(); ++i) {
        LogMessage(DEBUG_REPORT_WARNING_BIT,
                   format("Profile sets variables for %s while also using %s\n", set[0].c_str(), set[i].c_str()));
    }

    return set.size() <= 1;
}

bool JsonLoader::WarnDuplicatedFeature(const Json::Value &parent) {
    bool valid = true;
    valid &= WarnDuplicated(parent, {"VkPhysicalDevicePrivateDataFeatures", "VkPhysicalDevicePrivateDataFeaturesEXT", "VkPhysicalDeviceVulkan13Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceVariablePointersFeatures", "VkPhysicalDeviceVariablePointersFeaturesKHR", "VkPhysicalDeviceVariablePointerFeaturesKHR", "VkPhysicalDeviceVariablePointerFeatures", "VkPhysicalDeviceVulkan11Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceMultiviewFeatures", "VkPhysicalDeviceMultiviewFeaturesKHR", "VkPhysicalDeviceVulkan11Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDevice16BitStorageFeatures", "VkPhysicalDevice16BitStorageFeaturesKHR", "VkPhysicalDeviceVulkan11Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures", "VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR", "VkPhysicalDeviceVulkan12Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceSamplerYcbcrConversionFeatures", "VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR", "VkPhysicalDeviceVulkan11Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceProtectedMemoryFeatures", "VkPhysicalDeviceVulkan11Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceInlineUniformBlockFeatures", "VkPhysicalDeviceInlineUniformBlockFeaturesEXT", "VkPhysicalDeviceVulkan13Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceMaintenance4Features", "VkPhysicalDeviceMaintenance4FeaturesKHR", "VkPhysicalDeviceVulkan13Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceShaderDrawParametersFeatures", "VkPhysicalDeviceShaderDrawParameterFeatures", "VkPhysicalDeviceVulkan11Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceShaderFloat16Int8Features", "VkPhysicalDeviceShaderFloat16Int8FeaturesKHR", "VkPhysicalDeviceFloat16Int8FeaturesKHR", "VkPhysicalDeviceVulkan12Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceHostQueryResetFeatures", "VkPhysicalDeviceHostQueryResetFeaturesEXT", "VkPhysicalDeviceVulkan12Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceGlobalPriorityQueryFeaturesKHR", "VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceDescriptorIndexingFeatures", "VkPhysicalDeviceDescriptorIndexingFeaturesEXT", "VkPhysicalDeviceVulkan12Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceTimelineSemaphoreFeatures", "VkPhysicalDeviceTimelineSemaphoreFeaturesKHR", "VkPhysicalDeviceVulkan12Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDevice8BitStorageFeatures", "VkPhysicalDevice8BitStorageFeaturesKHR", "VkPhysicalDeviceVulkan12Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceVulkanMemoryModelFeatures", "VkPhysicalDeviceVulkanMemoryModelFeaturesKHR", "VkPhysicalDeviceVulkan12Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceShaderAtomicInt64Features", "VkPhysicalDeviceShaderAtomicInt64FeaturesKHR", "VkPhysicalDeviceVulkan12Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceScalarBlockLayoutFeatures", "VkPhysicalDeviceScalarBlockLayoutFeaturesEXT", "VkPhysicalDeviceVulkan12Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceUniformBufferStandardLayoutFeatures", "VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR", "VkPhysicalDeviceVulkan12Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceBufferDeviceAddressFeatures", "VkPhysicalDeviceBufferDeviceAddressFeaturesKHR", "VkPhysicalDeviceVulkan12Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceBufferDeviceAddressFeaturesEXT", "VkPhysicalDeviceBufferAddressFeaturesEXT"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceImagelessFramebufferFeatures", "VkPhysicalDeviceImagelessFramebufferFeaturesKHR", "VkPhysicalDeviceVulkan12Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceTextureCompressionASTCHDRFeatures", "VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT", "VkPhysicalDeviceVulkan13Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures", "VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR", "VkPhysicalDeviceVulkan12Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures", "VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT", "VkPhysicalDeviceVulkan13Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceSubgroupSizeControlFeatures", "VkPhysicalDeviceSubgroupSizeControlFeaturesEXT", "VkPhysicalDeviceVulkan13Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDevicePipelineCreationCacheControlFeatures", "VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT", "VkPhysicalDeviceVulkan13Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures", "VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR", "VkPhysicalDeviceVulkan13Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceImageRobustnessFeatures", "VkPhysicalDeviceImageRobustnessFeaturesEXT", "VkPhysicalDeviceVulkan13Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceShaderTerminateInvocationFeatures", "VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR", "VkPhysicalDeviceVulkan13Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceSynchronization2Features", "VkPhysicalDeviceSynchronization2FeaturesKHR", "VkPhysicalDeviceVulkan13Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceShaderIntegerDotProductFeatures", "VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR", "VkPhysicalDeviceVulkan13Features"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR", "VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceDynamicRenderingFeatures", "VkPhysicalDeviceDynamicRenderingFeaturesKHR", "VkPhysicalDeviceVulkan13Features"});
    return valid;
}

bool JsonLoader::WarnDuplicatedProperty(const Json::Value &parent) {
    bool valid = true;
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceDriverProperties", "VkPhysicalDeviceDriverPropertiesKHR", "VkPhysicalDeviceVulkan12Properties"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceIDProperties", "VkPhysicalDeviceIDPropertiesKHR", "VkPhysicalDeviceVulkan11Properties"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceMultiviewProperties", "VkPhysicalDeviceMultiviewPropertiesKHR", "VkPhysicalDeviceVulkan11Properties"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceSubgroupProperties", "VkPhysicalDeviceVulkan11Properties"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDevicePointClippingProperties", "VkPhysicalDevicePointClippingPropertiesKHR", "VkPhysicalDeviceVulkan11Properties"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceProtectedMemoryProperties", "VkPhysicalDeviceVulkan11Properties"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceSamplerFilterMinmaxProperties", "VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT", "VkPhysicalDeviceVulkan12Properties"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceInlineUniformBlockProperties", "VkPhysicalDeviceInlineUniformBlockPropertiesEXT", "VkPhysicalDeviceVulkan13Properties"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceMaintenance3Properties", "VkPhysicalDeviceMaintenance3PropertiesKHR", "VkPhysicalDeviceVulkan11Properties"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceMaintenance4Properties", "VkPhysicalDeviceMaintenance4PropertiesKHR", "VkPhysicalDeviceVulkan13Properties"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceFloatControlsProperties", "VkPhysicalDeviceFloatControlsPropertiesKHR", "VkPhysicalDeviceVulkan12Properties"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceDescriptorIndexingProperties", "VkPhysicalDeviceDescriptorIndexingPropertiesEXT", "VkPhysicalDeviceVulkan12Properties"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceTimelineSemaphoreProperties", "VkPhysicalDeviceTimelineSemaphorePropertiesKHR", "VkPhysicalDeviceVulkan12Properties"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceDepthStencilResolveProperties", "VkPhysicalDeviceDepthStencilResolvePropertiesKHR", "VkPhysicalDeviceVulkan12Properties"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceTexelBufferAlignmentProperties", "VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT", "VkPhysicalDeviceVulkan13Properties"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceSubgroupSizeControlProperties", "VkPhysicalDeviceSubgroupSizeControlPropertiesEXT", "VkPhysicalDeviceVulkan13Properties"});
    valid &= WarnDuplicated(parent, {"VkPhysicalDeviceShaderIntegerDotProductProperties", "VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR", "VkPhysicalDeviceVulkan13Properties"});
    return valid;
}

bool JsonLoader::GetFeature(const Json::Value &features, const std::string &name) {
    const Json::Value &feature = features[name];

    if (name == "VkPhysicalDeviceFeatures") {
        return GetValue(feature, &pdd_->physical_device_features_);
    } else if (name == "VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV") {
        auto support = CheckExtensionSupport(VK_NV_DEVICE_GENERATED_COMMANDS_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_device_generated_commands_features_);
    } else if (name == "VkPhysicalDevicePrivateDataFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_PRIVATE_DATA_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_private_data_features_);
    } else if (name == "VkPhysicalDevicePrivateDataFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_3, name)) return false;
        return GetValue(feature, &pdd_->physical_device_private_data_features_);
    } else if (name == "VkPhysicalDeviceVariablePointerFeatures" || name == "VkPhysicalDeviceVariablePointersFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_1, name)) return false;
        return GetValue(feature, &pdd_->physical_device_variable_pointer_features_);
    } else if (name == "VkPhysicalDeviceVariablePointerFeaturesKHR" || name == "VkPhysicalDeviceVariablePointersFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_variable_pointer_features_);
    } else if (name == "VkPhysicalDeviceMultiviewFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_MULTIVIEW_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_multiview_features_);
    } else if (name == "VkPhysicalDeviceMultiviewFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_1, name)) return false;
        return GetValue(feature, &pdd_->physical_device_multiview_features_);
    } else if (name == "VkPhysicalDevicePresentIdFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_PRESENT_ID_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_present_id_features_);
    } else if (name == "VkPhysicalDevicePresentWaitFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_PRESENT_WAIT_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_present_wait_features_);
    } else if (name == "VkPhysicalDevice16BitStorageFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_16BIT_STORAGE_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_16_bit_storage_features_);
    } else if (name == "VkPhysicalDevice16BitStorageFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_1, name)) return false;
        return GetValue(feature, &pdd_->physical_device_16_bit_storage_features_);
    } else if (name == "VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_SHADER_SUBGROUP_EXTENDED_TYPES_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_shader_subgroup_extended_types_features_);
    } else if (name == "VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_2, name)) return false;
        return GetValue(feature, &pdd_->physical_device_shader_subgroup_extended_types_features_);
    } else if (name == "VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_sampler_ycbcr_conversion_features_);
    } else if (name == "VkPhysicalDeviceSamplerYcbcrConversionFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_1, name)) return false;
        return GetValue(feature, &pdd_->physical_device_sampler_ycbcr_conversion_features_);
    } else if (name == "VkPhysicalDeviceProtectedMemoryFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_1, name)) return false;
        return GetValue(feature, &pdd_->physical_device_protected_memory_features_);
    } else if (name == "VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_blend_operation_advanced_features_);
    } else if (name == "VkPhysicalDeviceMultiDrawFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_MULTI_DRAW_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_multi_draw_features_);
    } else if (name == "VkPhysicalDeviceInlineUniformBlockFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_inline_uniform_block_features_);
    } else if (name == "VkPhysicalDeviceInlineUniformBlockFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_3, name)) return false;
        return GetValue(feature, &pdd_->physical_device_inline_uniform_block_features_);
    } else if (name == "VkPhysicalDeviceMaintenance4FeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_MAINTENANCE_4_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_maintenance_4_features_);
    } else if (name == "VkPhysicalDeviceMaintenance4Features") {
        if (!CheckVersionSupport(VK_API_VERSION_1_3, name)) return false;
        return GetValue(feature, &pdd_->physical_device_maintenance_4_features_);
    } else if (name == "VkPhysicalDeviceShaderDrawParameterFeatures" || name == "VkPhysicalDeviceShaderDrawParametersFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_1, name)) return false;
        return GetValue(feature, &pdd_->physical_device_shader_draw_parameter_features_);
    } else if (name == "VkPhysicalDeviceFloat16Int8FeaturesKHR" || name == "VkPhysicalDeviceShaderFloat16Int8FeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_float_16_int_8_features_);
    } else if (name == "VkPhysicalDeviceShaderFloat16Int8Features") {
        if (!CheckVersionSupport(VK_API_VERSION_1_2, name)) return false;
        return GetValue(feature, &pdd_->physical_device_float_16_int_8_features_);
    } else if (name == "VkPhysicalDeviceHostQueryResetFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_host_query_reset_features_);
    } else if (name == "VkPhysicalDeviceHostQueryResetFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_2, name)) return false;
        return GetValue(feature, &pdd_->physical_device_host_query_reset_features_);
    } else if (name == "VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_GLOBAL_PRIORITY_QUERY_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_global_priority_query_features_);
    } else if (name == "VkPhysicalDeviceGlobalPriorityQueryFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_GLOBAL_PRIORITY_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_global_priority_query_features_);
    } else if (name == "VkPhysicalDeviceDeviceMemoryReportFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_DEVICE_MEMORY_REPORT_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_device_memory_report_features_);
    } else if (name == "VkPhysicalDeviceDescriptorIndexingFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_descriptor_indexing_features_);
    } else if (name == "VkPhysicalDeviceDescriptorIndexingFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_2, name)) return false;
        return GetValue(feature, &pdd_->physical_device_descriptor_indexing_features_);
    } else if (name == "VkPhysicalDeviceTimelineSemaphoreFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_timeline_semaphore_features_);
    } else if (name == "VkPhysicalDeviceTimelineSemaphoreFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_2, name)) return false;
        return GetValue(feature, &pdd_->physical_device_timeline_semaphore_features_);
    } else if (name == "VkPhysicalDevice8BitStorageFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_8BIT_STORAGE_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_8_bit_storage_features_);
    } else if (name == "VkPhysicalDevice8BitStorageFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_2, name)) return false;
        return GetValue(feature, &pdd_->physical_device_8_bit_storage_features_);
    } else if (name == "VkPhysicalDeviceConditionalRenderingFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_CONDITIONAL_RENDERING_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_conditional_rendering_features_);
    } else if (name == "VkPhysicalDeviceVulkanMemoryModelFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_VULKAN_MEMORY_MODEL_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_vulkan_memory_model_features_);
    } else if (name == "VkPhysicalDeviceVulkanMemoryModelFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_2, name)) return false;
        return GetValue(feature, &pdd_->physical_device_vulkan_memory_model_features_);
    } else if (name == "VkPhysicalDeviceShaderAtomicInt64FeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_SHADER_ATOMIC_INT64_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_shader_atomic_int_64_features_);
    } else if (name == "VkPhysicalDeviceShaderAtomicInt64Features") {
        if (!CheckVersionSupport(VK_API_VERSION_1_2, name)) return false;
        return GetValue(feature, &pdd_->physical_device_shader_atomic_int_64_features_);
    } else if (name == "VkPhysicalDeviceShaderAtomicFloatFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_shader_atomic_float_features_);
    } else if (name == "VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_SHADER_ATOMIC_FLOAT_2_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_shader_atomic_float_2_features_);
    } else if (name == "VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_vertex_attribute_divisor_features_);
    } else if (name == "VkPhysicalDeviceASTCDecodeFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_ASTC_DECODE_MODE_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_astcdecode_features_);
    } else if (name == "VkPhysicalDeviceTransformFeedbackFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_transform_feedback_features_);
    } else if (name == "VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV") {
        auto support = CheckExtensionSupport(VK_NV_REPRESENTATIVE_FRAGMENT_TEST_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_representative_fragment_test_features_);
    } else if (name == "VkPhysicalDeviceExclusiveScissorFeaturesNV") {
        auto support = CheckExtensionSupport(VK_NV_SCISSOR_EXCLUSIVE_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_exclusive_scissor_features_);
    } else if (name == "VkPhysicalDeviceCornerSampledImageFeaturesNV") {
        auto support = CheckExtensionSupport(VK_NV_CORNER_SAMPLED_IMAGE_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_corner_sampled_image_features_);
    } else if (name == "VkPhysicalDeviceComputeShaderDerivativesFeaturesNV") {
        auto support = CheckExtensionSupport(VK_NV_COMPUTE_SHADER_DERIVATIVES_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_compute_shader_derivatives_features_);
    } else if (name == "VkPhysicalDeviceShaderImageFootprintFeaturesNV") {
        auto support = CheckExtensionSupport(VK_NV_SHADER_IMAGE_FOOTPRINT_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_shader_image_footprint_features_);
    } else if (name == "VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV") {
        auto support = CheckExtensionSupport(VK_NV_DEDICATED_ALLOCATION_IMAGE_ALIASING_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_dedicated_allocation_image_aliasing_features_);
    } else if (name == "VkPhysicalDeviceShadingRateImageFeaturesNV") {
        auto support = CheckExtensionSupport(VK_NV_SHADING_RATE_IMAGE_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_shading_rate_image_features_);
    } else if (name == "VkPhysicalDeviceInvocationMaskFeaturesHUAWEI") {
        auto support = CheckExtensionSupport(VK_HUAWEI_INVOCATION_MASK_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_invocation_mask_features_);
    } else if (name == "VkPhysicalDeviceMeshShaderFeaturesNV") {
        auto support = CheckExtensionSupport(VK_NV_MESH_SHADER_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_mesh_shader_features_);
    } else if (name == "VkPhysicalDeviceAccelerationStructureFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_acceleration_structure_features_);
    } else if (name == "VkPhysicalDeviceRayTracingPipelineFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_ray_tracing_pipeline_features_);
    } else if (name == "VkPhysicalDeviceRayQueryFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_RAY_QUERY_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_ray_query_features_);
    } else if (name == "VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_RAY_TRACING_MAINTENANCE_1_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_ray_tracing_maintenance_1_features_);
    } else if (name == "VkPhysicalDeviceFragmentDensityMapFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_FRAGMENT_DENSITY_MAP_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_fragment_density_map_features_);
    } else if (name == "VkPhysicalDeviceFragmentDensityMap2FeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_FRAGMENT_DENSITY_MAP_2_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_fragment_density_map_2_features_);
    } else if (name == "VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM") {
        auto support = CheckExtensionSupport(VK_QCOM_FRAGMENT_DENSITY_MAP_OFFSET_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_fragment_density_map_offset_features_);
    } else if (name == "VkPhysicalDeviceScalarBlockLayoutFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_scalar_block_layout_features_);
    } else if (name == "VkPhysicalDeviceScalarBlockLayoutFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_2, name)) return false;
        return GetValue(feature, &pdd_->physical_device_scalar_block_layout_features_);
    } else if (name == "VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_uniform_buffer_standard_layout_features_);
    } else if (name == "VkPhysicalDeviceUniformBufferStandardLayoutFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_2, name)) return false;
        return GetValue(feature, &pdd_->physical_device_uniform_buffer_standard_layout_features_);
    } else if (name == "VkPhysicalDeviceDepthClipEnableFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_DEPTH_CLIP_ENABLE_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_depth_clip_enable_features_);
    } else if (name == "VkPhysicalDeviceMemoryPriorityFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_memory_priority_features_);
    } else if (name == "VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_PAGEABLE_DEVICE_LOCAL_MEMORY_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_pageable_device_local_memory_features_);
    } else if (name == "VkPhysicalDeviceBufferDeviceAddressFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_buffer_device_address_features_);
    } else if (name == "VkPhysicalDeviceBufferDeviceAddressFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_2, name)) return false;
        return GetValue(feature, &pdd_->physical_device_buffer_device_address_features_);
    } else if (name == "VkPhysicalDeviceBufferAddressFeaturesEXT" || name == "VkPhysicalDeviceBufferDeviceAddressFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_buffer_address_features_);
    } else if (name == "VkPhysicalDeviceImagelessFramebufferFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_imageless_framebuffer_features_);
    } else if (name == "VkPhysicalDeviceImagelessFramebufferFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_2, name)) return false;
        return GetValue(feature, &pdd_->physical_device_imageless_framebuffer_features_);
    } else if (name == "VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_TEXTURE_COMPRESSION_ASTC_HDR_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_texture_compression_astchdrfeatures_);
    } else if (name == "VkPhysicalDeviceTextureCompressionASTCHDRFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_3, name)) return false;
        return GetValue(feature, &pdd_->physical_device_texture_compression_astchdrfeatures_);
    } else if (name == "VkPhysicalDeviceCooperativeMatrixFeaturesNV") {
        auto support = CheckExtensionSupport(VK_NV_COOPERATIVE_MATRIX_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_cooperative_matrix_features_);
    } else if (name == "VkPhysicalDeviceYcbcrImageArraysFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_YCBCR_IMAGE_ARRAYS_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_ycbcr_image_arrays_features_);
    } else if (name == "VkPhysicalDevicePerformanceQueryFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_PERFORMANCE_QUERY_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_performance_query_features_);
    } else if (name == "VkPhysicalDeviceCoverageReductionModeFeaturesNV") {
        auto support = CheckExtensionSupport(VK_NV_COVERAGE_REDUCTION_MODE_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_coverage_reduction_mode_features_);
    } else if (name == "VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL") {
        auto support = CheckExtensionSupport(VK_INTEL_SHADER_INTEGER_FUNCTIONS_2_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_shader_integer_functions_2_features_);
    } else if (name == "VkPhysicalDeviceShaderClockFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_SHADER_CLOCK_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_shader_clock_features_);
    } else if (name == "VkPhysicalDeviceIndexTypeUint8FeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_INDEX_TYPE_UINT8_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_index_type_uint_8_features_);
    } else if (name == "VkPhysicalDeviceShaderSMBuiltinsFeaturesNV") {
        auto support = CheckExtensionSupport(VK_NV_SHADER_SM_BUILTINS_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_shader_smbuiltins_features_);
    } else if (name == "VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_fragment_shader_interlock_features_);
    } else if (name == "VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_SEPARATE_DEPTH_STENCIL_LAYOUTS_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_separate_depth_stencil_layouts_features_);
    } else if (name == "VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_2, name)) return false;
        return GetValue(feature, &pdd_->physical_device_separate_depth_stencil_layouts_features_);
    } else if (name == "VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_PRIMITIVE_TOPOLOGY_LIST_RESTART_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_primitive_topology_list_restart_features_);
    } else if (name == "VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_PIPELINE_EXECUTABLE_PROPERTIES_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_pipeline_executable_properties_features_);
    } else if (name == "VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_shader_demote_to_helper_invocation_features_);
    } else if (name == "VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_3, name)) return false;
        return GetValue(feature, &pdd_->physical_device_shader_demote_to_helper_invocation_features_);
    } else if (name == "VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_texel_buffer_alignment_features_);
    } else if (name == "VkPhysicalDeviceSubgroupSizeControlFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_subgroup_size_control_features_);
    } else if (name == "VkPhysicalDeviceSubgroupSizeControlFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_3, name)) return false;
        return GetValue(feature, &pdd_->physical_device_subgroup_size_control_features_);
    } else if (name == "VkPhysicalDeviceLineRasterizationFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_line_rasterization_features_);
    } else if (name == "VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_PIPELINE_CREATION_CACHE_CONTROL_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_pipeline_creation_cache_control_features_);
    } else if (name == "VkPhysicalDevicePipelineCreationCacheControlFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_3, name)) return false;
        return GetValue(feature, &pdd_->physical_device_pipeline_creation_cache_control_features_);
    } else if (name == "VkPhysicalDeviceVulkan11Features") {
        if (!CheckVersionSupport(VK_API_VERSION_1_2, name)) return false;
        return GetValue(feature, &pdd_->physical_device_vulkan_11_features_);
    } // Blocks nested too deeply, break
    if (name == "VkPhysicalDeviceVulkan12Features") {
        if (!CheckVersionSupport(VK_API_VERSION_1_2, name)) return false;
        return GetValue(feature, &pdd_->physical_device_vulkan_12_features_);
    } else if (name == "VkPhysicalDeviceVulkan13Features") {
        if (!CheckVersionSupport(VK_API_VERSION_1_3, name)) return false;
        return GetValue(feature, &pdd_->physical_device_vulkan_13_features_);
    } else if (name == "VkPhysicalDeviceCoherentMemoryFeaturesAMD") {
        auto support = CheckExtensionSupport(VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_coherent_memory_features_);
    } else if (name == "VkPhysicalDeviceCustomBorderColorFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_CUSTOM_BORDER_COLOR_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_custom_border_color_features_);
    } else if (name == "VkPhysicalDeviceBorderColorSwizzleFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_BORDER_COLOR_SWIZZLE_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_border_color_swizzle_features_);
    } else if (name == "VkPhysicalDeviceExtendedDynamicStateFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_extended_dynamic_state_features_);
    } else if (name == "VkPhysicalDeviceExtendedDynamicState2FeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_extended_dynamic_state_2_features_);
    } else if (name == "VkPhysicalDeviceDiagnosticsConfigFeaturesNV") {
        auto support = CheckExtensionSupport(VK_NV_DEVICE_DIAGNOSTICS_CONFIG_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_diagnostics_config_features_);
    } else if (name == "VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_ZERO_INITIALIZE_WORKGROUP_MEMORY_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_zero_initialize_workgroup_memory_features_);
    } else if (name == "VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_3, name)) return false;
        return GetValue(feature, &pdd_->physical_device_zero_initialize_workgroup_memory_features_);
    } else if (name == "VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_shader_subgroup_uniform_control_flow_features_);
    } else if (name == "VkPhysicalDeviceRobustness2FeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_ROBUSTNESS_2_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_robustness_2_features_);
    } else if (name == "VkPhysicalDeviceImageRobustnessFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_IMAGE_ROBUSTNESS_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_image_robustness_features_);
    } else if (name == "VkPhysicalDeviceImageRobustnessFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_3, name)) return false;
        return GetValue(feature, &pdd_->physical_device_image_robustness_features_);
    } else if (name == "VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_workgroup_memory_explicit_layout_features_);
    } else if (name == "VkPhysicalDevicePortabilitySubsetFeaturesKHR") {
        return GetValue(feature, &pdd_->physical_device_portability_subset_features_);
    } else if (name == "VkPhysicalDevice4444FormatsFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_4444_FORMATS_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_4444_formats_features_);
    } else if (name == "VkPhysicalDeviceSubpassShadingFeaturesHUAWEI") {
        auto support = CheckExtensionSupport(VK_HUAWEI_SUBPASS_SHADING_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_subpass_shading_features_);
    } else if (name == "VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_SHADER_IMAGE_ATOMIC_INT64_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_shader_image_atomic_int_64_features_);
    } else if (name == "VkPhysicalDeviceFragmentShadingRateFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_fragment_shading_rate_features_);
    } else if (name == "VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_SHADER_TERMINATE_INVOCATION_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_shader_terminate_invocation_features_);
    } else if (name == "VkPhysicalDeviceShaderTerminateInvocationFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_3, name)) return false;
        return GetValue(feature, &pdd_->physical_device_shader_terminate_invocation_features_);
    } else if (name == "VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV") {
        auto support = CheckExtensionSupport(VK_NV_FRAGMENT_SHADING_RATE_ENUMS_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_fragment_shading_rate_enums_features_);
    } else if (name == "VkPhysicalDeviceImage2DViewOf3DFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_IMAGE_2D_VIEW_OF_3D_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_image_2_dview_of_3_dfeatures_);
    } else if (name == "VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE") {
        auto support = CheckExtensionSupport(VK_VALVE_MUTABLE_DESCRIPTOR_TYPE_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_mutable_descriptor_type_features_);
    } else if (name == "VkPhysicalDeviceDepthClipControlFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_DEPTH_CLIP_CONTROL_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_depth_clip_control_features_);
    } else if (name == "VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_vertex_input_dynamic_state_features_);
    } else if (name == "VkPhysicalDeviceExternalMemoryRDMAFeaturesNV") {
        auto support = CheckExtensionSupport(VK_NV_EXTERNAL_MEMORY_RDMA_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_external_memory_rdmafeatures_);
    } else if (name == "VkPhysicalDeviceColorWriteEnableFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_COLOR_WRITE_ENABLE_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_color_write_enable_features_);
    } else if (name == "VkPhysicalDeviceSynchronization2FeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_synchronization_2_features_);
    } else if (name == "VkPhysicalDeviceSynchronization2Features") {
        if (!CheckVersionSupport(VK_API_VERSION_1_3, name)) return false;
        return GetValue(feature, &pdd_->physical_device_synchronization_2_features_);
    } else if (name == "VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_PRIMITIVES_GENERATED_QUERY_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_primitives_generated_query_features_);
    } else if (name == "VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_MULTISAMPLED_RENDER_TO_SINGLE_SAMPLED_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_multisampled_render_to_single_sampled_features_);
    } else if (name == "VkPhysicalDeviceInheritedViewportScissorFeaturesNV") {
        auto support = CheckExtensionSupport(VK_NV_INHERITED_VIEWPORT_SCISSOR_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_inherited_viewport_scissor_features_);
    } else if (name == "VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_YCBCR_2PLANE_444_FORMATS_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_ycbcr_2_plane_444_formats_features_);
    } else if (name == "VkPhysicalDeviceProvokingVertexFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_PROVOKING_VERTEX_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_provoking_vertex_features_);
    } else if (name == "VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_SHADER_INTEGER_DOT_PRODUCT_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_shader_integer_dot_product_features_);
    } else if (name == "VkPhysicalDeviceShaderIntegerDotProductFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_3, name)) return false;
        return GetValue(feature, &pdd_->physical_device_shader_integer_dot_product_features_);
    } else if (name == "VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV") {
        auto support = CheckExtensionSupport(VK_NV_FRAGMENT_SHADER_BARYCENTRIC_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_fragment_shader_barycentric_features_);
    } else if (name == "VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_FRAGMENT_SHADER_BARYCENTRIC_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_fragment_shader_barycentric_features_);
    } else if (name == "VkPhysicalDeviceRayTracingMotionBlurFeaturesNV") {
        auto support = CheckExtensionSupport(VK_NV_RAY_TRACING_MOTION_BLUR_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_ray_tracing_motion_blur_features_);
    } else if (name == "VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_RGBA10X6_FORMATS_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_rgba_10_x_6_formats_features_);
    } else if (name == "VkPhysicalDeviceDynamicRenderingFeaturesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_dynamic_rendering_features_);
    } else if (name == "VkPhysicalDeviceDynamicRenderingFeatures") {
        if (!CheckVersionSupport(VK_API_VERSION_1_3, name)) return false;
        return GetValue(feature, &pdd_->physical_device_dynamic_rendering_features_);
    } else if (name == "VkPhysicalDeviceImageViewMinLodFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_IMAGE_VIEW_MIN_LOD_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_image_view_min_lod_features_);
    } else if (name == "VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM") {
        auto support = CheckExtensionSupport(VK_ARM_RASTERIZATION_ORDER_ATTACHMENT_ACCESS_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_rasterization_order_attachment_access_features_);
    } else if (name == "VkPhysicalDeviceLinearColorAttachmentFeaturesNV") {
        auto support = CheckExtensionSupport(VK_NV_LINEAR_COLOR_ATTACHMENT_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_linear_color_attachment_features_);
    } else if (name == "VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_GRAPHICS_PIPELINE_LIBRARY_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_graphics_pipeline_library_features_);
    } else if (name == "VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE") {
        auto support = CheckExtensionSupport(VK_VALVE_DESCRIPTOR_SET_HOST_MAPPING_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_descriptor_set_host_mapping_features_);
    } else if (name == "VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_SHADER_MODULE_IDENTIFIER_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_shader_module_identifier_features_);
    } else if (name == "VkPhysicalDeviceImageCompressionControlFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_IMAGE_COMPRESSION_CONTROL_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_image_compression_control_features_);
    } else if (name == "VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_IMAGE_COMPRESSION_CONTROL_SWAPCHAIN_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_image_compression_control_swapchain_features_);
    } else if (name == "VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_SUBPASS_MERGE_FEEDBACK_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_subpass_merge_feedback_features_);
    } else if (name == "VkPhysicalDevicePipelinePropertiesFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_pipeline_properties_features_);
    } else if (name == "VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD") {
        auto support = CheckExtensionSupport(VK_AMD_SHADER_EARLY_AND_LATE_FRAGMENT_TESTS_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_shader_early_and_late_fragment_tests_features_);
    } else if (name == "VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_NON_SEAMLESS_CUBE_MAP_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_non_seamless_cube_map_features_);
    } else if (name == "VkPhysicalDevicePipelineRobustnessFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_PIPELINE_ROBUSTNESS_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_pipeline_robustness_features_);
    } else if (name == "VkPhysicalDeviceImageProcessingFeaturesQCOM") {
        auto support = CheckExtensionSupport(VK_QCOM_IMAGE_PROCESSING_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_image_processing_features_);
    } else if (name == "VkPhysicalDeviceTilePropertiesFeaturesQCOM") {
        auto support = CheckExtensionSupport(VK_QCOM_TILE_PROPERTIES_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_tile_properties_features_);
    } else if (name == "VkPhysicalDeviceAmigoProfilingFeaturesSEC") {
        auto support = CheckExtensionSupport(VK_SEC_AMIGO_PROFILING_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_amigo_profiling_features_);
    } else if (name == "VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_ATTACHMENT_FEEDBACK_LOOP_LAYOUT_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(feature, &pdd_->physical_device_attachment_feedback_loop_layout_features_);
    }

    return true;
}

bool JsonLoader::GetProperty(const Json::Value &props, const std::string &name) {
    const Json::Value &property = props[name];

    if (name == "VkPhysicalDeviceProperties") {
        return GetValue(property, &pdd_->physical_device_properties_);
    } else if (name == "VkPhysicalDeviceSparseProperties") {
        return GetValue(property, &pdd_->physical_device_properties_.sparseProperties);
    } else if (name == "VkPhysicalDeviceLimits") {
        return GetValue(property, &pdd_->physical_device_properties_.limits);
    } else if (name == "VkPhysicalDeviceDeviceGeneratedCommandsPropertiesNV") {
        auto support = CheckExtensionSupport(VK_NV_DEVICE_GENERATED_COMMANDS_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_device_generated_commands_properties_);
    } else if (name == "VkPhysicalDeviceMultiDrawPropertiesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_MULTI_DRAW_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_multi_draw_properties_);
    } else if (name == "VkPhysicalDevicePushDescriptorPropertiesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_push_descriptor_properties_);
    } else if (name == "VkPhysicalDeviceDriverPropertiesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_driver_properties_);
    } else if (name == "VkPhysicalDeviceDriverProperties") {
        if (!CheckVersionSupport(VK_API_VERSION_1_2, name)) return false;
        return GetValue(property, &pdd_->physical_device_driver_properties_);
    } else if (name == "VkPhysicalDeviceIDPropertiesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_idproperties_);
    } else if (name == "VkPhysicalDeviceIDProperties") {
        if (!CheckVersionSupport(VK_API_VERSION_1_1, name)) return false;
        return GetValue(property, &pdd_->physical_device_idproperties_);
    } else if (name == "VkPhysicalDeviceMultiviewPropertiesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_MULTIVIEW_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_multiview_properties_);
    } else if (name == "VkPhysicalDeviceMultiviewProperties") {
        if (!CheckVersionSupport(VK_API_VERSION_1_1, name)) return false;
        return GetValue(property, &pdd_->physical_device_multiview_properties_);
    } else if (name == "VkPhysicalDeviceDiscardRectanglePropertiesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_DISCARD_RECTANGLES_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_discard_rectangle_properties_);
    } else if (name == "VkPhysicalDeviceMultiviewPerViewAttributesPropertiesNVX") {
        auto support = CheckExtensionSupport(VK_NVX_MULTIVIEW_PER_VIEW_ATTRIBUTES_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_multiview_per_view_attributes_properties_);
    } else if (name == "VkPhysicalDeviceSubgroupProperties") {
        if (!CheckVersionSupport(VK_API_VERSION_1_1, name)) return false;
        return GetValue(property, &pdd_->physical_device_subgroup_properties_);
    } else if (name == "VkPhysicalDevicePointClippingPropertiesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_MAINTENANCE_2_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_point_clipping_properties_);
    } else if (name == "VkPhysicalDevicePointClippingProperties") {
        if (!CheckVersionSupport(VK_API_VERSION_1_1, name)) return false;
        return GetValue(property, &pdd_->physical_device_point_clipping_properties_);
    } else if (name == "VkPhysicalDeviceProtectedMemoryProperties") {
        if (!CheckVersionSupport(VK_API_VERSION_1_1, name)) return false;
        return GetValue(property, &pdd_->physical_device_protected_memory_properties_);
    } else if (name == "VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_sampler_filter_minmax_properties_);
    } else if (name == "VkPhysicalDeviceSamplerFilterMinmaxProperties") {
        if (!CheckVersionSupport(VK_API_VERSION_1_2, name)) return false;
        return GetValue(property, &pdd_->physical_device_sampler_filter_minmax_properties_);
    } else if (name == "VkPhysicalDeviceSampleLocationsPropertiesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_SAMPLE_LOCATIONS_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_sample_locations_properties_);
    } else if (name == "VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_blend_operation_advanced_properties_);
    } else if (name == "VkPhysicalDeviceInlineUniformBlockPropertiesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_inline_uniform_block_properties_);
    } else if (name == "VkPhysicalDeviceInlineUniformBlockProperties") {
        if (!CheckVersionSupport(VK_API_VERSION_1_3, name)) return false;
        return GetValue(property, &pdd_->physical_device_inline_uniform_block_properties_);
    } else if (name == "VkPhysicalDeviceMaintenance3PropertiesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_MAINTENANCE_3_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_maintenance_3_properties_);
    } else if (name == "VkPhysicalDeviceMaintenance3Properties") {
        if (!CheckVersionSupport(VK_API_VERSION_1_1, name)) return false;
        return GetValue(property, &pdd_->physical_device_maintenance_3_properties_);
    } else if (name == "VkPhysicalDeviceMaintenance4PropertiesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_MAINTENANCE_4_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_maintenance_4_properties_);
    } else if (name == "VkPhysicalDeviceMaintenance4Properties") {
        if (!CheckVersionSupport(VK_API_VERSION_1_3, name)) return false;
        return GetValue(property, &pdd_->physical_device_maintenance_4_properties_);
    } else if (name == "VkPhysicalDeviceFloatControlsPropertiesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_float_controls_properties_);
    } else if (name == "VkPhysicalDeviceFloatControlsProperties") {
        if (!CheckVersionSupport(VK_API_VERSION_1_2, name)) return false;
        return GetValue(property, &pdd_->physical_device_float_controls_properties_);
    } else if (name == "VkPhysicalDeviceExternalMemoryHostPropertiesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_EXTERNAL_MEMORY_HOST_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_external_memory_host_properties_);
    } else if (name == "VkPhysicalDeviceConservativeRasterizationPropertiesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_conservative_rasterization_properties_);
    } else if (name == "VkPhysicalDeviceShaderCorePropertiesAMD") {
        auto support = CheckExtensionSupport(VK_AMD_SHADER_CORE_PROPERTIES_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_shader_core_properties_);
    } else if (name == "VkPhysicalDeviceShaderCoreProperties2AMD") {
        auto support = CheckExtensionSupport(VK_AMD_SHADER_CORE_PROPERTIES_2_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_shader_core_properties_2_);
    } else if (name == "VkPhysicalDeviceDescriptorIndexingPropertiesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_descriptor_indexing_properties_);
    } else if (name == "VkPhysicalDeviceDescriptorIndexingProperties") {
        if (!CheckVersionSupport(VK_API_VERSION_1_2, name)) return false;
        return GetValue(property, &pdd_->physical_device_descriptor_indexing_properties_);
    } else if (name == "VkPhysicalDeviceTimelineSemaphorePropertiesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_timeline_semaphore_properties_);
    } else if (name == "VkPhysicalDeviceTimelineSemaphoreProperties") {
        if (!CheckVersionSupport(VK_API_VERSION_1_2, name)) return false;
        return GetValue(property, &pdd_->physical_device_timeline_semaphore_properties_);
    } else if (name == "VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_vertex_attribute_divisor_properties_);
    } else if (name == "VkPhysicalDevicePCIBusInfoPropertiesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_pcibus_info_properties_);
    } else if (name == "VkPhysicalDeviceDepthStencilResolvePropertiesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_depth_stencil_resolve_properties_);
    } else if (name == "VkPhysicalDeviceDepthStencilResolveProperties") {
        if (!CheckVersionSupport(VK_API_VERSION_1_2, name)) return false;
        return GetValue(property, &pdd_->physical_device_depth_stencil_resolve_properties_);
    } else if (name == "VkPhysicalDeviceTransformFeedbackPropertiesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_transform_feedback_properties_);
    } else if (name == "VkPhysicalDeviceShadingRateImagePropertiesNV") {
        auto support = CheckExtensionSupport(VK_NV_SHADING_RATE_IMAGE_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_shading_rate_image_properties_);
    } else if (name == "VkPhysicalDeviceMeshShaderPropertiesNV") {
        auto support = CheckExtensionSupport(VK_NV_MESH_SHADER_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_mesh_shader_properties_);
    } else if (name == "VkPhysicalDeviceAccelerationStructurePropertiesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_acceleration_structure_properties_);
    } else if (name == "VkPhysicalDeviceRayTracingPipelinePropertiesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_ray_tracing_pipeline_properties_);
    } else if (name == "VkPhysicalDeviceRayTracingPropertiesNV") {
        auto support = CheckExtensionSupport(VK_NV_RAY_TRACING_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_ray_tracing_properties_);
    } else if (name == "VkPhysicalDeviceFragmentDensityMapPropertiesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_FRAGMENT_DENSITY_MAP_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_fragment_density_map_properties_);
    } else if (name == "VkPhysicalDeviceFragmentDensityMap2PropertiesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_FRAGMENT_DENSITY_MAP_2_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_fragment_density_map_2_properties_);
    } else if (name == "VkPhysicalDeviceFragmentDensityMapOffsetPropertiesQCOM") {
        auto support = CheckExtensionSupport(VK_QCOM_FRAGMENT_DENSITY_MAP_OFFSET_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_fragment_density_map_offset_properties_);
    } else if (name == "VkPhysicalDeviceCooperativeMatrixPropertiesNV") {
        auto support = CheckExtensionSupport(VK_NV_COOPERATIVE_MATRIX_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_cooperative_matrix_properties_);
    } else if (name == "VkPhysicalDevicePerformanceQueryPropertiesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_PERFORMANCE_QUERY_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_performance_query_properties_);
    } else if (name == "VkPhysicalDeviceShaderSMBuiltinsPropertiesNV") {
        auto support = CheckExtensionSupport(VK_NV_SHADER_SM_BUILTINS_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_shader_smbuiltins_properties_);
    } else if (name == "VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_texel_buffer_alignment_properties_);
    } else if (name == "VkPhysicalDeviceTexelBufferAlignmentProperties") {
        if (!CheckVersionSupport(VK_API_VERSION_1_3, name)) return false;
        return GetValue(property, &pdd_->physical_device_texel_buffer_alignment_properties_);
    } else if (name == "VkPhysicalDeviceSubgroupSizeControlPropertiesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_subgroup_size_control_properties_);
    } else if (name == "VkPhysicalDeviceSubgroupSizeControlProperties") {
        if (!CheckVersionSupport(VK_API_VERSION_1_3, name)) return false;
        return GetValue(property, &pdd_->physical_device_subgroup_size_control_properties_);
    } else if (name == "VkPhysicalDeviceSubpassShadingPropertiesHUAWEI") {
        auto support = CheckExtensionSupport(VK_HUAWEI_SUBPASS_SHADING_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_subpass_shading_properties_);
    } else if (name == "VkPhysicalDeviceLineRasterizationPropertiesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_line_rasterization_properties_);
    } else if (name == "VkPhysicalDeviceVulkan11Properties") {
        if (!CheckVersionSupport(VK_API_VERSION_1_2, name)) return false;
        return GetValue(property, &pdd_->physical_device_vulkan_11_properties_);
    } else if (name == "VkPhysicalDeviceVulkan12Properties") {
        if (!CheckVersionSupport(VK_API_VERSION_1_2, name)) return false;
        return GetValue(property, &pdd_->physical_device_vulkan_12_properties_);
    } else if (name == "VkPhysicalDeviceVulkan13Properties") {
        if (!CheckVersionSupport(VK_API_VERSION_1_3, name)) return false;
        return GetValue(property, &pdd_->physical_device_vulkan_13_properties_);
    } else if (name == "VkPhysicalDeviceToolPropertiesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_TOOLING_INFO_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_tool_properties_);
    } else if (name == "VkPhysicalDeviceToolProperties") {
        if (!CheckVersionSupport(VK_API_VERSION_1_3, name)) return false;
        return GetValue(property, &pdd_->physical_device_tool_properties_);
    } else if (name == "VkPhysicalDeviceCustomBorderColorPropertiesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_CUSTOM_BORDER_COLOR_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_custom_border_color_properties_);
    } else if (name == "VkPhysicalDeviceRobustness2PropertiesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_ROBUSTNESS_2_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_robustness_2_properties_);
    } else if (name == "VkPhysicalDevicePortabilitySubsetPropertiesKHR") {
        return GetValue(property, &pdd_->physical_device_portability_subset_properties_);
    } else if (name == "VkPhysicalDeviceFragmentShadingRatePropertiesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_fragment_shading_rate_properties_);
    } else if (name == "VkPhysicalDeviceFragmentShadingRateEnumsPropertiesNV") {
        auto support = CheckExtensionSupport(VK_NV_FRAGMENT_SHADING_RATE_ENUMS_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_fragment_shading_rate_enums_properties_);
    } else if (name == "VkPhysicalDeviceProvokingVertexPropertiesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_PROVOKING_VERTEX_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_provoking_vertex_properties_);
    } else if (name == "VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_SHADER_INTEGER_DOT_PRODUCT_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_shader_integer_dot_product_properties_);
    } else if (name == "VkPhysicalDeviceShaderIntegerDotProductProperties") {
        if (!CheckVersionSupport(VK_API_VERSION_1_3, name)) return false;
        return GetValue(property, &pdd_->physical_device_shader_integer_dot_product_properties_);
    } else if (name == "VkPhysicalDeviceDrmPropertiesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_PHYSICAL_DEVICE_DRM_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_drm_properties_);
    } else if (name == "VkPhysicalDeviceFragmentShaderBarycentricPropertiesKHR") {
        auto support = CheckExtensionSupport(VK_KHR_FRAGMENT_SHADER_BARYCENTRIC_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_fragment_shader_barycentric_properties_);
    } else if (name == "VkPhysicalDeviceGraphicsPipelineLibraryPropertiesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_GRAPHICS_PIPELINE_LIBRARY_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_graphics_pipeline_library_properties_);
    } else if (name == "VkPhysicalDeviceShaderModuleIdentifierPropertiesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_SHADER_MODULE_IDENTIFIER_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_shader_module_identifier_properties_);
    } else if (name == "VkPhysicalDevicePipelineRobustnessPropertiesEXT") {
        auto support = CheckExtensionSupport(VK_EXT_PIPELINE_ROBUSTNESS_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_pipeline_robustness_properties_);
    } else if (name == "VkPhysicalDeviceImageProcessingPropertiesQCOM") {
        auto support = CheckExtensionSupport(VK_QCOM_IMAGE_PROCESSING_EXTENSION_NAME, name);
        if (support != ExtensionSupport::SUPPORTED) return valid(support);
        return GetValue(property, &pdd_->physical_device_image_processing_properties_);
    }

    return true;
}

bool JsonLoader::GetFormat(const Json::Value &formats, const std::string &format_name, ArrayOfVkFormatProperties *dest,
                           ArrayOfVkFormatProperties3 *dest3) {
    VkFormat format = StringToFormat(format_name);
    VkFormatProperties profile_properties = {};
    VkFormatProperties3 profile_properties_3 = {};
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
        } else if (name == "VkFormatProperties2") {
            for (const auto &feature : props["formatProperties"]["linearTilingFeatures"]) {
                profile_properties.linearTilingFeatures |= StringToVkFormatFeatureFlags(feature.asString());
            }
            for (const auto &feature : props["formatProperties"]["optimalTilingFeatures"]) {
                profile_properties.optimalTilingFeatures |= StringToVkFormatFeatureFlags(feature.asString());
            }
            for (const auto &feature : props["formatProperties"]["bufferFeatures"]) {
                profile_properties.bufferFeatures |= StringToVkFormatFeatureFlags(feature.asString());
            }
        } else if (name == "VkFormatProperties3") {
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
        WarnMissingFormatFeatures(format_name, "linearTilingFeatures", profile_properties.linearTilingFeatures,
                                  device_properties.linearTilingFeatures);
        valid = false;
    }
    if (!HasFlags(device_properties.optimalTilingFeatures, profile_properties.optimalTilingFeatures)) {
        WarnMissingFormatFeatures(format_name, "optimalTilingFeatures", profile_properties.optimalTilingFeatures,
                                  device_properties.optimalTilingFeatures);
        valid = false;
    }
    if (!HasFlags(device_properties.bufferFeatures, profile_properties.bufferFeatures)) {
        WarnMissingFormatFeatures(format_name, "bufferFeatures", profile_properties.bufferFeatures,
                                  device_properties.bufferFeatures);
        valid = false;
    }

    const VkFormatProperties3 &device_properties_3 = pdd_->device_formats_3_[format];
    if (!HasFlags(device_properties_3.linearTilingFeatures, profile_properties_3.linearTilingFeatures)) {
        WarnMissingFormatFeatures2(format_name, "linearTilingFeatures", profile_properties_3.linearTilingFeatures,
                                   device_properties_3.linearTilingFeatures);
        valid = false;
    }
    if (!HasFlags(device_properties_3.optimalTilingFeatures, profile_properties_3.optimalTilingFeatures)) {
        WarnMissingFormatFeatures2(format_name, "optimalTilingFeatures", profile_properties_3.optimalTilingFeatures,
                                   device_properties_3.optimalTilingFeatures);
        valid = false;
    }
    if (!HasFlags(device_properties_3.bufferFeatures, profile_properties_3.bufferFeatures)) {
        WarnMissingFormatFeatures2(format_name, "bufferFeatures", profile_properties_3.bufferFeatures,
                                   device_properties_3.bufferFeatures);
        valid = false;
    }

    return valid;
}

bool JsonLoader::CheckVersionSupport(uint32_t version, const std::string &name) {
    if (pdd_->GetEffectiveVersion() < version) {
        LogMessage(
            DEBUG_REPORT_ERROR_BIT,
            ::format("Profile sets %s which is provided by Vulkan version %s, but the current effective API version is %s.\n",
                     name.c_str(), StringAPIVersion(version).c_str(), StringAPIVersion(pdd_->GetEffectiveVersion()).c_str()));
        return false;
    }
    return true;
}

JsonLoader::ExtensionSupport JsonLoader::CheckExtensionSupport(const char *extension, const std::string &name) {
    for (const auto &ext : excluded_extensions_) {
        if (ext == extension) {
            LogMessage(DEBUG_REPORT_NOTIFICATION_BIT,
                       ::format("Profile requires %s capabilities, but %s is excluded, device values are used.\n", name.c_str(),
                                extension));
            return JsonLoader::ExtensionSupport::EXCLUDED;
        }
    }
    if (layer_settings->simulate_capabilities & SIMULATE_EXTENSIONS_BIT) {
        if (!PhysicalDeviceData::HasSimulatedExtension(pdd_, extension)) {
            LogMessage(
                DEBUG_REPORT_ERROR_BIT,
                ::format("Profile requires %s capabilitiess, but %s is not required by the profile, device values are used.\n",
                         name.c_str(), extension));
            if (layer_settings->debug_fail_on_error) {
                return JsonLoader::ExtensionSupport::UNSUPPORTED;
            }
        }
    } else {
        if (!PhysicalDeviceData::HasExtension(pdd_, extension)) {
            LogMessage(
                DEBUG_REPORT_WARNING_BIT,
                ::format("Profile requires by %s capabilities, but %s is not supported by the device.\n", name.c_str(), extension));
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

bool QueueFamilyMatch(const VkQueueFamilyProperties &device, const VkQueueFamilyProperties &profile) {
    if ((device.queueFlags & profile.queueFlags) != profile.queueFlags) {
        return false;
    } else if (device.queueCount < profile.queueCount) {
        return false;
    } else if (device.timestampValidBits < profile.timestampValidBits) {
        return false;
    } else if (profile.minImageTransferGranularity.width > 0 &&
               device.minImageTransferGranularity.width > profile.minImageTransferGranularity.width) {
        return false;
    } else if (profile.minImageTransferGranularity.height > 0 &&
               device.minImageTransferGranularity.height > profile.minImageTransferGranularity.height) {
        return false;
    } else if (profile.minImageTransferGranularity.depth > 0 &&
               device.minImageTransferGranularity.depth > profile.minImageTransferGranularity.depth) {
        return false;
    }
    return true;
}

bool GlobalPriorityMatch(const VkQueueFamilyGlobalPriorityPropertiesKHR &device,
                         const VkQueueFamilyGlobalPriorityPropertiesKHR &profile) {
    if (profile.priorityCount == 0) {
        return true;
    } else if (device.priorityCount != profile.priorityCount) {
        return false;
    }

    bool match = true;
    for (uint32_t i = 0; i < device.priorityCount; ++i) {
        if (device.priorities[i] != profile.priorities[i]) {
            match = false;
            break;
        }
    }
    return match;
}

bool JsonLoader::GetQueueFamilyProperties(const Json::Value &qf_props, QueueFamilyProperties *dest) {
    for (const auto &name : qf_props.getMemberNames()) {
        const auto &props = qf_props[name];
        if (name == "VkQueueFamilyProperties") {
            for (const auto &feature : props["queueFlags"]) {
                dest->properties_2.queueFamilyProperties.queueFlags |= StringToVkQueueFlags(feature.asString());
            }
            dest->properties_2.queueFamilyProperties.queueCount = props["queueCount"].asInt();
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
            dest->properties_2.queueFamilyProperties.queueCount = props2["queueCount"].asInt();
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
        } else if (name == "VkVideoQueueFamilyProperties2KHR") {
            for (const auto &feature : props["videoCodecOperations"]) {
                dest->video_properties_2_.videoCodecOperations |= StringToVkVideoCodecOperationFlagsKHR(feature.asString());
            }
        } else if (name == "VkQueueFamilyCheckpointProperties2NV") {
            for (const auto &feature : props["checkpointExecutionStageMask"]) {
                dest->checkpoint_properties_2_.checkpointExecutionStageMask |= StringToVkPipelineStageFlags2(feature.asString());
            }
        } else if (name == "VkQueueFamilyCheckpointPropertiesNV") {
            for (const auto &feature : props["checkpointExecutionStageMask"]) {
                dest->checkpoint_properties_.checkpointExecutionStageMask |= StringToVkPipelineStageFlags(feature.asString());
            }
        } else if (name == "VkQueueFamilyQueryResultStatusProperties2KHR") {
            dest->query_result_status_properties_2_.queryResultStatusSupport = props["queryResultStatusSupport"].asBool() ? VK_TRUE : VK_FALSE;
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
        if ((device_qfp.video_properties_2_.videoCodecOperations & dest->video_properties_2_.videoCodecOperations) !=
            dest->video_properties_2_.videoCodecOperations) {
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
        if (device_qfp.query_result_status_properties_2_.queryResultStatusSupport != dest->query_result_status_properties_2_.queryResultStatusSupport) {
            continue;
        }
        supported = true;
        break;
    }
    if (!supported) {
        std::string message =
            format("Device has no queue family that supports VkQueueFamilyProperties [queueFlags: %s, queueCount: %" PRIu32
                   ", timestampValidBits: %" PRIu32 ", minImageTransferGranularity: [%" PRIu32 ", %" PRIu32 ", %" PRIu32 "]]",
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
                priorities += string_VkQueueGlobalPriorityEXT(dest->global_priority_properties_.priorities[i]);
            }
            priorities += "]";

            message += format(", VkQueueFamilyGlobalPriorityPropertiesKHR [priorityCount: %" PRIu32 ", priorities: %s]",
                              dest->global_priority_properties_.priorityCount, priorities.c_str());
        }
        if (dest->video_properties_2_.videoCodecOperations > 0) {
            message += format(", VkVideoQueueFamilyProperties2KHR [videoCodecOperations: %s]",
                              string_VkVideoCodecOperationFlagsKHR(dest->video_properties_2_.videoCodecOperations).c_str());
        }
        if (dest->checkpoint_properties_.checkpointExecutionStageMask > 0) {
            message += format(", VkQueueFamilyCheckpointPropertiesNV [checkpointExecutionStageMask: %s]",
                              string_VkPipelineStageFlags(dest->checkpoint_properties_.checkpointExecutionStageMask).c_str());
        }
        if (dest->checkpoint_properties_2_.checkpointExecutionStageMask > 0) {
            message += format(", VkQueueFamilyCheckpointProperties2NV [checkpointExecutionStageMask: %s]",
                              string_VkPipelineStageFlags2KHR(dest->checkpoint_properties_2_.checkpointExecutionStageMask).c_str());
        }
        if (dest->query_result_status_properties_2_.queryResultStatusSupport) {
            message += format(", VkQueueFamilyQueryResultStatusProperties2KHR [queryResultStatusSupport: VK_TRUE]");
        }
        message += ".\n";
        LogMessage(DEBUG_REPORT_WARNING_BIT, message);
        valid = false;
    }

    return valid;
}

bool QueueFamilyAndExtensionsMatch(const QueueFamilyProperties &device, const QueueFamilyProperties &profile) {
    if (!QueueFamilyMatch(device.properties_2.queueFamilyProperties, profile.properties_2.queueFamilyProperties)) {
        return false;
    }
    if (!GlobalPriorityMatch(device.global_priority_properties_, profile.global_priority_properties_)) {
        return false;
    }
    if ((device.video_properties_2_.videoCodecOperations & profile.video_properties_2_.videoCodecOperations) !=
        profile.video_properties_2_.videoCodecOperations) {
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
    if (device.query_result_status_properties_2_.queryResultStatusSupport != profile.query_result_status_properties_2_.queryResultStatusSupport) {
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
    LogMessage(DEBUG_REPORT_WARNING_BIT,
               format("Device supports all individual profile queue families, but not all of them simultaneously.\n"));
    return false;
}

void JsonLoader::AddPromotedExtensions(uint32_t api_version) {
	const uint32_t minor = VK_API_VERSION_MINOR(api_version);
	const uint32_t major = VK_API_VERSION_MAJOR(api_version);
	LogMessage(DEBUG_REPORT_NOTIFICATION_BIT,
		format("Adding promoted extensions to core in Vulkan (%" PRIu32 ".%" PRIu32 ")",major, minor));

    static const std::vector<const char *> promoted_1_1 = {
        VK_KHR_MULTIVIEW_EXTENSION_NAME,
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
        VK_KHR_DEVICE_GROUP_EXTENSION_NAME,
        VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
        VK_KHR_MAINTENANCE_1_EXTENSION_NAME,
        VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME,
        VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
        VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME,
        VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME,
        VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME,
        VK_KHR_16BIT_STORAGE_EXTENSION_NAME,
        VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE_EXTENSION_NAME,
        VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME,
        VK_KHR_EXTERNAL_FENCE_EXTENSION_NAME,
        VK_KHR_MAINTENANCE_2_EXTENSION_NAME,
        VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME,
        VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
        VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME,
        VK_KHR_RELAXED_BLOCK_LAYOUT_EXTENSION_NAME,
        VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
        VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME,
        VK_KHR_BIND_MEMORY_2_EXTENSION_NAME,
        VK_KHR_MAINTENANCE_3_EXTENSION_NAME,
    };
    if (api_version >= VK_API_VERSION_1_1) {
        for (const auto& ext : promoted_1_1) {
            VkExtensionProperties extension;
            strcpy(extension.extensionName, ext);
            extension.specVersion = 1;
            if (!PhysicalDeviceData::HasSimulatedExtension(pdd_, ext)) {
                pdd_->simulation_extensions_.push_back(extension);
            }
            pdd_->arrayof_extension_properties_.push_back(extension);
        }
    }
    static const std::vector<const char *> promoted_1_2 = {
        VK_KHR_SAMPLER_MIRROR_CLAMP_TO_EDGE_EXTENSION_NAME,
        VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME,
        VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME,
        VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
        VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME,
        VK_KHR_IMAGE_FORMAT_LIST_EXTENSION_NAME,
        VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
        VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME,
        VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME,
        VK_KHR_SHADER_SUBGROUP_EXTENDED_TYPES_EXTENSION_NAME,
        VK_KHR_8BIT_STORAGE_EXTENSION_NAME,
        VK_KHR_SHADER_ATOMIC_INT64_EXTENSION_NAME,
        VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME,
        VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME,
        VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
        VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
        VK_KHR_VULKAN_MEMORY_MODEL_EXTENSION_NAME,
        VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME,
        VK_KHR_SPIRV_1_4_EXTENSION_NAME,
        VK_KHR_SEPARATE_DEPTH_STENCIL_LAYOUTS_EXTENSION_NAME,
        VK_EXT_SEPARATE_STENCIL_USAGE_EXTENSION_NAME,
        VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT_EXTENSION_NAME,
        VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
        VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME,
    };
    if (api_version >= VK_API_VERSION_1_2) {
        for (const auto& ext : promoted_1_2) {
            VkExtensionProperties extension;
            strcpy(extension.extensionName, ext);
            extension.specVersion = 1;
            if (!PhysicalDeviceData::HasSimulatedExtension(pdd_, ext)) {
                pdd_->simulation_extensions_.push_back(extension);
            }
            pdd_->arrayof_extension_properties_.push_back(extension);
        }
    }
    static const std::vector<const char *> promoted_1_3 = {
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
        VK_EXT_TEXTURE_COMPRESSION_ASTC_HDR_EXTENSION_NAME,
        VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME,
        VK_EXT_PIPELINE_CREATION_FEEDBACK_EXTENSION_NAME,
        VK_KHR_SHADER_TERMINATE_INVOCATION_EXTENSION_NAME,
        VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME,
        VK_EXT_TOOLING_INFO_EXTENSION_NAME,
        VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME,
        VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME,
        VK_KHR_SHADER_INTEGER_DOT_PRODUCT_EXTENSION_NAME,
        VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME,
        VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME,
        VK_EXT_PRIVATE_DATA_EXTENSION_NAME,
        VK_EXT_PIPELINE_CREATION_CACHE_CONTROL_EXTENSION_NAME,
        VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
        VK_KHR_ZERO_INITIALIZE_WORKGROUP_MEMORY_EXTENSION_NAME,
        VK_EXT_YCBCR_2PLANE_444_FORMATS_EXTENSION_NAME,
        VK_EXT_IMAGE_ROBUSTNESS_EXTENSION_NAME,
        VK_KHR_COPY_COMMANDS_2_EXTENSION_NAME,
        VK_EXT_4444_FORMATS_EXTENSION_NAME,
        VK_KHR_FORMAT_FEATURE_FLAGS_2_EXTENSION_NAME,
        VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME,
        VK_KHR_MAINTENANCE_4_EXTENSION_NAME,
    };
    if (api_version >= VK_API_VERSION_1_3) {
        for (const auto& ext : promoted_1_3) {
            VkExtensionProperties extension;
            strcpy(extension.extensionName, ext);
            extension.specVersion = 1;
            if (!PhysicalDeviceData::HasSimulatedExtension(pdd_, ext)) {
                pdd_->simulation_extensions_.push_back(extension);
            }
            pdd_->arrayof_extension_properties_.push_back(extension);
        }
    }
}

static Json::Value ParseJsonFile(std::string filename) {
    Json::Value root = Json::nullValue;

    // Remove newline from filename
    if (int(filename.back() == 0xa)) {
        filename.pop_back();
    }

    std::ifstream file;
    file.open(filename.c_str());
    if (!file.is_open()) {
        return root;
    }

    std::string errs;
    Json::CharReaderBuilder builder;
    Json::parseFromStream(builder, file, &root, &errs);
    file.close();

    return root;
}

struct JsonValidator {
    JsonValidator() {}

    bool Init() {
#ifdef __APPLE__
        const std::string schema_path = "/usr/local/share/vulkan/registry/profiles-0.8-latest.json";
#else
        const char *sdk_path = std::getenv("VULKAN_SDK");
        if (sdk_path == nullptr) return false;
        const std::string schema_path = std::string(sdk_path) + "/share/vulkan/registry/profiles-0.8-latest.json";
#endif

        if (!schema) {
            const Json::Value schema_document = ParseJsonFile(schema_path.c_str());
            if (schema_document == Json::nullValue) {
                return false;
            }

            schema.reset(new Schema);

            SchemaParser parser;
            JsonCppAdapter schema_adapter(schema_document);
            parser.populateSchema(schema_adapter, *schema);
        }

        return schema.get() != nullptr;
    }

    bool Check(const Json::Value &json_document) {
        assert(!json_document.empty());

        if (schema.get() == nullptr) return true;

        Validator validator(Validator::kWeakTypes);
        JsonCppAdapter document_adapter(json_document);

        ValidationResults results;
        if (!validator.validate(*schema, document_adapter, &results)) {
            ValidationResults::Error error;
            unsigned int error_num = 1;
            while (results.popError(error)) {
                std::string context;
                std::vector<std::string>::iterator itr = error.context.begin();
                for (; itr != error.context.end(); itr++) {
                    context += *itr;
                }

                if (error_num <= 3) {
                    std::string log = format("Error #%d\n", error_num);
                    log += "\t context: " + context + "\n";
                    log += "\t desc:    " + error.description + "\n\n";

                    message += log.c_str();
                }

                ++error_num;
            }

            message += format("Total Error Count: %d\n", error_num).c_str();

            return false;
        }

        return true;
    }

    std::string message;
    std::unique_ptr<Schema> schema;
};

VkResult JsonLoader::ReadProfile(const Json::Value root, const std::vector<std::string> &capabilities) {
    bool failed = false;

    uint32_t properties_api_version = 0;
    uint32_t simulated_version = 0;

    const auto &caps = root["capabilities"];
    for (const auto &capability : capabilities) {
        const auto &c = caps[capability];

        const auto &properties = c["properties"];
        if (properties.isMember("VkPhysicalDeviceProperties") && properties["VkPhysicalDeviceProperties"].isMember("apiVersion")) {
            properties_api_version = properties["VkPhysicalDeviceProperties"]["apiVersion"].asInt();
            simulated_version = properties_api_version;
        } else if (layer_settings->simulate_capabilities & SIMULATE_API_VERSION_BIT) {
            simulated_version = profile_api_version_;
        }
    }
    if (simulated_version != 0) {
        AddPromotedExtensions(simulated_version);
    }

    for (const auto &capability : capabilities) {
        const auto &c = caps[capability];
        const auto &properties = c["properties"];

        if (VK_API_VERSION_PATCH(this->profile_api_version_) > VK_API_VERSION_PATCH(pdd_->physical_device_properties_.apiVersion)) {
            LogMessage(DEBUG_REPORT_ERROR_BIT,
                       format("Profile apiVersion (%" PRIu32 ".%" PRIu32 ".%" PRIu32
                              ") is greater than the device apiVersion (%" PRIu32 ".%" PRIu32 ".%" PRIu32 ")\n",
                              VK_API_VERSION_MAJOR(this->profile_api_version_), VK_API_VERSION_MINOR(this->profile_api_version_),
                              VK_API_VERSION_PATCH(this->profile_api_version_),
                              VK_API_VERSION_MAJOR(pdd_->physical_device_properties_.apiVersion),
                              VK_API_VERSION_MINOR(pdd_->physical_device_properties_.apiVersion),
                              VK_API_VERSION_PATCH(pdd_->physical_device_properties_.apiVersion)));
            failed = true;
        }

        if (layer_settings->simulate_capabilities & SIMULATE_EXTENSIONS_BIT) {
            const auto &extensions = c["extensions"];

            pdd_->arrayof_extension_properties_.reserve(extensions.size());
            for (const auto &e : extensions.getMemberNames()) {
                VkExtensionProperties extension;
                strcpy(extension.extensionName, e.c_str());
                extension.specVersion = extensions[e].asInt();
                bool found = false;
                for (const auto &ext : pdd_->arrayof_extension_properties_) {
                    if (strcmp(ext.extensionName, extension.extensionName) == 0) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    bool supported_on_device = false;
                    for (const auto &device_extension : pdd_->device_extensions_) {
                        if (strcmp(device_extension.extensionName, extension.extensionName) == 0) {
                            supported_on_device = true;
                            break;
                        }
                    }
                    if (!supported_on_device) {
                        failed = true;
                    }
                    pdd_->arrayof_extension_properties_.push_back(extension);
                    if (!PhysicalDeviceData::HasSimulatedExtension(pdd_, extension.extensionName)) {
                        pdd_->simulation_extensions_.push_back(extension);
                    }
                }
            }
        }

        if (layer_settings->simulate_capabilities & SIMULATE_FEATURES_BIT) {
            const auto &features = c["features"];

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
                bool success = GetFeature(features, feature);
                if (!success) {
                    failed = true;
                }
            }
        }

        if (layer_settings->simulate_capabilities & SIMULATE_PROPERTIES_BIT) {
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
                bool success = GetProperty(properties, prop);
                if (!success) {
                    failed = true;
                }
            }
        }

        if (layer_settings->simulate_capabilities & SIMULATE_FORMATS_BIT) {
            const auto &formats = c["formats"];

            for (const auto &format : formats.getMemberNames()) {
                bool success = GetFormat(formats, format, &pdd_->arrayof_format_properties_, &pdd_->arrayof_format_properties_3_);
                if (!success) {
                    failed = true;
                }
            }
        }

        if (layer_settings->simulate_capabilities & SIMULATE_QUEUE_FAMILY_PROPERTIES_BIT) {
            const auto &qf_props = c["queueFamiliesProperties"];

            bool queue_families_supported = true;
            for (const auto &qfp : qf_props) {
                pdd_->arrayof_queue_family_properties_.emplace_back();
                bool success = GetQueueFamilyProperties(qfp, &pdd_->arrayof_queue_family_properties_.back());
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

    if (properties_api_version != 0) {
        LogMessage(DEBUG_REPORT_NOTIFICATION_BIT,
                   format("VkPhysicalDeviceProperties API version: %" PRIu32 ".%" PRIu32 ".%" PRIu32
                          ". Using the API version specified by the profile VkPhysicalDeviceProperties structure.\n",
                          VK_API_VERSION_MAJOR(properties_api_version), VK_API_VERSION_MINOR(properties_api_version),
                          VK_API_VERSION_PATCH(properties_api_version)));
    } else if (layer_settings->simulate_capabilities & SIMULATE_API_VERSION_BIT) {
        LogMessage(DEBUG_REPORT_NOTIFICATION_BIT,
                   format("VkPhysicalDeviceProperties API version: %" PRIu32 ".%" PRIu32 ".%" PRIu32
                          ". Using the API version specified by the profile.\n",
                          VK_API_VERSION_MAJOR(this->profile_api_version_), VK_API_VERSION_MINOR(this->profile_api_version_),
                          VK_API_VERSION_PATCH(this->profile_api_version_)));

        pdd_->physical_device_properties_.apiVersion = this->profile_api_version_;
    } else {
        LogMessage(DEBUG_REPORT_NOTIFICATION_BIT, format("VkPhysicalDeviceProperties API version: %" PRIu32 ".%" PRIu32 ".%" PRIu32
                                                         ". Using the device version.\n",
                                                         VK_API_VERSION_MAJOR(pdd_->physical_device_properties_.apiVersion),
                                                         VK_API_VERSION_MINOR(pdd_->physical_device_properties_.apiVersion),
                                                         VK_API_VERSION_PATCH(pdd_->physical_device_properties_.apiVersion)));
    }

    if (failed && layer_settings->debug_fail_on_error) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    return VK_SUCCESS;
}

VkResult JsonLoader::LoadFile(std::string filename) {
    LogMessage(DEBUG_REPORT_DEBUG_BIT, format("JsonLoader::LoadFile(\"%s\")\n", filename.c_str()));

    profile_filename_ = filename;
    if (filename.empty()) {
        if (!layer_settings->profile_name.empty()) {
            LogMessage(DEBUG_REPORT_WARNING_BIT,
                       format("Profile name is set to \"%s\", but profile_file is unset. The profile will not be loaded.\n",
                              layer_settings->profile_name.c_str()));
        }
        return VK_SUCCESS;
    }
    std::ifstream json_file(filename);
    if (!json_file) {
        LogMessage(DEBUG_REPORT_ERROR_BIT, format("Fail to open file \"%s\"\n", filename.c_str()));
        return layer_settings->debug_fail_on_error ? VK_ERROR_INITIALIZATION_FAILED : VK_SUCCESS;
    }

    Json::Value root = Json::nullValue;
    Json::CharReaderBuilder builder;
    std::string errs;
    bool success = Json::parseFromStream(builder, json_file, &root_, &errs);
    if (!success) {
        LogMessage(DEBUG_REPORT_ERROR_BIT, format("Fail to parse file \"%s\" {\n%s}\n", filename.c_str(), errs.c_str()));
        return layer_settings->debug_fail_on_error ? VK_ERROR_INITIALIZATION_FAILED : VK_SUCCESS;
    }
    json_file.close();

    if (root_.type() != Json::objectValue) {
        LogMessage(DEBUG_REPORT_ERROR_BIT, format("Json document root is not an object in file \"%s\"\n", filename.c_str()));
        return layer_settings->debug_fail_on_error ? VK_ERROR_INITIALIZATION_FAILED : VK_SUCCESS;
    }

    ReadProfileApiVersion();

    return VK_SUCCESS;
}

void JsonLoader::ReadProfileApiVersion() {
    const std::string &profile_name = layer_settings->profile_name;
    const Json::Value &profiles = root_["profiles"];
    bool found_profile = false;
    for (const auto &profile : profiles.getMemberNames()) {
        if (profile_name.empty() || profile_name == "${VP_DEFAULT}" || profile == profile_name) {
            const std::string version_string = profiles[profile]["api-version"].asCString();

            uint32_t api_major = 0;
            uint32_t api_minor = 0;
            uint32_t api_patch = 0;
            std::sscanf(version_string.c_str(), "%d.%d.%d", &api_major, &api_minor, &api_patch);
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
            std::sscanf(version_string.c_str(), "%d.%d.%d", &api_major, &api_minor, &api_patch);
            profile_api_version_ = VK_MAKE_API_VERSION(0, api_major, api_minor, api_patch);
            break; // Systematically load the first and default profile when the profile is not found
        }
    }

    for (std::size_t j = 0, m = layer_settings->exclude_device_extensions.size(); j < m; ++j) {
        const auto &extension = layer_settings->exclude_device_extensions[j];
        if (extension.empty()) continue;
        excluded_extensions_.push_back(extension);
    }
    for (std::size_t j = 0, m = layer_settings->exclude_formats.size(); j < m; ++j) {
        const auto &format = layer_settings->exclude_formats[j];
        if (format.empty()) continue;
        excluded_formats_.push_back(format);
    }
}

VkResult JsonLoader::LoadDevice(PhysicalDeviceData *pdd) {
    pdd_ = pdd;

    const std::string &profile_name = layer_settings->profile_name;
    const Json::Value &profiles = root_["profiles"];
    std::vector<std::string> capabilities;

    bool found_profile = false;
    for (const auto &profile : profiles.getMemberNames()) {
        if (profile_name.empty() || profile_name == "${VP_DEFAULT}" || profile == profile_name) {
            const auto &caps = profiles[profile]["capabilities"];

            for (const auto &cap : caps) {
                capabilities.push_back(cap.asString());
            }

            found_profile = true;
            LogMessage(DEBUG_REPORT_NOTIFICATION_BIT, format("Overriding device capabilities with \"%s\" profile capabilities\n", profile.c_str()).c_str());
            break;  // load a single profile
        }
    }
    if (!found_profile) {
        for (const auto &profile : profiles.getMemberNames()) {
            const auto &caps = profiles[profile]["capabilities"];

            for (const auto &cap : caps) {
                capabilities.push_back(cap.asString());
            }

            LogMessage(DEBUG_REPORT_WARNING_BIT,
                format("\"%s\" profile could not be found in \"%s\" file. Loading the default \"%s\" profile of the file.\n",
                    layer_settings->profile_name.c_str(), layer_settings->profile_file.c_str(), profile.c_str()));

            break; // Systematically load the first and default profile
        }
    }

    if (capabilities.empty()) {
        return VK_SUCCESS;
    }

    const Json::Value schema_value = root_["$schema"];
    if (!schema_value.isString()) {
        LogMessage(DEBUG_REPORT_ERROR_BIT, "JSON element \"$schema\" is not a string\n");
        return layer_settings->debug_fail_on_error ? VK_ERROR_INITIALIZATION_FAILED : VK_SUCCESS;
    }

    const std::string schema = schema_value.asCString();
    if (schema.find(SCHEMA_URI_BASE) == std::string::npos) {
        LogMessage(DEBUG_REPORT_ERROR_BIT, format("Document schema \"%s\" not supported by %s\n", schema.c_str(), kOurLayerName));
        return layer_settings->debug_fail_on_error ? VK_ERROR_INITIALIZATION_FAILED : VK_SUCCESS;
    }

    const std::size_t size_schema = schema.size();
    const std::size_t size_base = std::strlen(SCHEMA_URI_BASE);
    const std::size_t size_version = std::strlen(".json#");
    const std::string version = schema.substr(size_base, size_schema - size_base - size_version);

    uint32_t version_major = 0;
    uint32_t version_minor = 0;
    uint32_t version_patch = 0;
    std::sscanf(version.c_str(), "%d.%d.%d", &version_major, &version_minor, &version_patch);
    if (VK_HEADER_VERSION < version_patch) {
        LogMessage(DEBUG_REPORT_WARNING_BIT,
                   format("%s is built against Vulkan Header %d but the profile is written against Vulkan "
                          "Header %d.\n\t- All newer capabilities in the "
                          "profile will be ignored by the layer.\n",
                          kOurLayerName, VK_HEADER_VERSION, version_patch));
    } else if (layer_settings->profile_validation) {
        JsonValidator validator;
        if (!validator.Init()) {
            LogMessage(DEBUG_REPORT_WARNING_BIT,
                       format("%s could not find the profile schema file to validate filename.\n\t- This "
                              "operation requires the Vulkan SDK to be installed.\n\t- Skipping profile file validation.",
                              kOurLayerName, profile_filename_.c_str()));
        } else if (!validator.Check(root_)) {
            LogMessage(DEBUG_REPORT_ERROR_BIT, format("%s is not a valid JSON profile file.\n", profile_filename_.c_str()));
            if (layer_settings->debug_fail_on_error) {
                return VK_ERROR_INITIALIZATION_FAILED;
            } else {
                return VK_SUCCESS;
            }
        }
    }

    VkResult result = VK_SUCCESS;
    if (layer_settings->simulate_capabilities & SIMULATE_EXTENSIONS_BIT) {
        pdd_->simulation_extensions_.clear();
    }

    result = ReadProfile(root_, capabilities);

    return result;
}

// Apply the DRY principle, see https://en.wikipedia.org/wiki/Don%27t_repeat_yourself
#define GET_VALUE(member, name) GetValue(parent, member, #name, &dest->name)
#define GET_ARRAY(name) GetArray(parent, #name, dest->name)
#define GET_MEMBER_VALUE(member, name) GetValue(parent, member, #name, &dest->name)
#define GET_VALUE_WARN(member, name, warn_func)                     \
    if (!GetValue(parent, member, #name, &dest->name, warn_func)) { \
        valid = false;                                              \
    }
#define GET_VALUE_SIZE_T_WARN(member, name, warn_func)                   \
    if (!GetValueSizet(parent, member, #name, &dest->name, warn_func)) { \
        valid = false;                                                   \
    }
#define GET_VALUE_FLAG_WARN(member, name)                    \
    if (!GetValueFlag(parent, member, #name, &dest->name)) { \
        valid = false;                                       \
    }
#define GET_VALUE_ENUM_WARN(member, name, warn_func)                    \
    if (!GetValueEnum(parent, member, #name, &dest->name, warn_func)) { \
        valid = false;                                       \
    }


bool JsonLoader::GetValue(const Json::Value &parent, const std::string &member, const char *name, VkExtent2D *dest) {
    if (member != name) {
        return true;
    }
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return true;
    }
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE(prop, width);
        GET_VALUE(prop, height);
    }
    return true;
}

bool JsonLoader::GetValue(const Json::Value &pparent, const std::string &member, const char *name, VkExtent2D *dest,
                          std::function<bool(const char *, uint32_t, uint32_t)> warn_func = nullptr) {
    if (member != name) {
        return true;
    }
    const Json::Value parent = pparent[name];
    if (parent.type() != Json::objectValue) {
        return true;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, width, warn_func);
        GET_VALUE_WARN(prop, height, warn_func);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &pparent, const std::string &member, const char *name, VkExtent3D *dest) {
    if (member != name) {
        return true;
    }
    const Json::Value parent = pparent[name];
    if (parent.type() != Json::objectValue) {
        return true;
    }
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE(prop, width);
        GET_VALUE(prop, height);
        GET_VALUE(prop, depth);
    }
    return true;
}
bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceSubgroupProperties *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceSubgroupProperties)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, subgroupSize, WarnIfLesser);
        GET_VALUE_FLAG_WARN(member, supportedStages);
        GET_VALUE_FLAG_WARN(member, supportedOperations);
        GET_VALUE_WARN(member, quadOperationsInAllStages, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceProtectedMemoryProperties *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceProtectedMemoryProperties)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        WarnNotModifiable("VkPhysicalDeviceProtectedMemoryProperties", member, "protectedNoFault");
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceVulkan11Properties *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceVulkan11Properties)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        WarnNotModifiable("VkPhysicalDeviceVulkan11Properties", member, "deviceUUID");
        WarnNotModifiable("VkPhysicalDeviceVulkan11Properties", member, "driverUUID");
        WarnNotModifiable("VkPhysicalDeviceVulkan11Properties", member, "deviceLUID");
        WarnNotModifiable("VkPhysicalDeviceVulkan11Properties", member, "deviceNodeMask");
        WarnNotModifiable("VkPhysicalDeviceVulkan11Properties", member, "deviceLUIDValid");
        GET_VALUE_WARN(member, subgroupSize, WarnIfLesser);
        GET_VALUE_FLAG_WARN(member, subgroupSupportedStages);
        GET_VALUE_FLAG_WARN(member, subgroupSupportedOperations);
        GET_VALUE_WARN(member, subgroupQuadOperationsInAllStages, WarnIfNotEqualBool);
        WarnNotModifiable("VkPhysicalDeviceVulkan11Properties", member, "pointClippingBehavior");
        GET_VALUE_WARN(member, maxMultiviewViewCount, WarnIfGreater);
        GET_VALUE_WARN(member, maxMultiviewInstanceIndex, WarnIfGreater);
        WarnNotModifiable("VkPhysicalDeviceVulkan11Properties", member, "protectedNoFault");
        GET_VALUE_WARN(member, maxPerSetDescriptors, WarnIfGreater);
        GET_VALUE_WARN(member, maxMemoryAllocationSize, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceVulkan12Properties *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceVulkan12Properties)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        WarnNotModifiable("VkPhysicalDeviceVulkan12Properties", member, "driverID");
        WarnNotModifiable("VkPhysicalDeviceVulkan12Properties", member, "driverName");
        WarnNotModifiable("VkPhysicalDeviceVulkan12Properties", member, "driverInfo");
        WarnNotModifiable("VkPhysicalDeviceVulkan12Properties", member, "conformanceVersion");
        WarnNotModifiable("VkPhysicalDeviceVulkan12Properties", member, "denormBehaviorIndependence");
        WarnNotModifiable("VkPhysicalDeviceVulkan12Properties", member, "roundingModeIndependence");
        GET_VALUE_WARN(member, shaderSignedZeroInfNanPreserveFloat16, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderSignedZeroInfNanPreserveFloat32, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderSignedZeroInfNanPreserveFloat64, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderDenormPreserveFloat16, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderDenormPreserveFloat32, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderDenormPreserveFloat64, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderDenormFlushToZeroFloat16, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderDenormFlushToZeroFloat32, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderDenormFlushToZeroFloat64, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderRoundingModeRTEFloat16, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderRoundingModeRTEFloat32, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderRoundingModeRTEFloat64, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderRoundingModeRTZFloat16, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderRoundingModeRTZFloat32, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderRoundingModeRTZFloat64, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, maxUpdateAfterBindDescriptorsInAllPools, WarnIfGreater);
        GET_VALUE_WARN(member, shaderUniformBufferArrayNonUniformIndexingNative, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderSampledImageArrayNonUniformIndexingNative, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderStorageBufferArrayNonUniformIndexingNative, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderStorageImageArrayNonUniformIndexingNative, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderInputAttachmentArrayNonUniformIndexingNative, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, robustBufferAccessUpdateAfterBind, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, quadDivergentImplicitLod, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, maxPerStageDescriptorUpdateAfterBindSamplers, WarnIfGreater);
        GET_VALUE_WARN(member, maxPerStageDescriptorUpdateAfterBindUniformBuffers, WarnIfGreater);
        GET_VALUE_WARN(member, maxPerStageDescriptorUpdateAfterBindStorageBuffers, WarnIfGreater);
        GET_VALUE_WARN(member, maxPerStageDescriptorUpdateAfterBindSampledImages, WarnIfGreater);
        GET_VALUE_WARN(member, maxPerStageDescriptorUpdateAfterBindStorageImages, WarnIfGreater);
        GET_VALUE_WARN(member, maxPerStageDescriptorUpdateAfterBindInputAttachments, WarnIfGreater);
        GET_VALUE_WARN(member, maxPerStageUpdateAfterBindResources, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetUpdateAfterBindSamplers, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetUpdateAfterBindUniformBuffers, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetUpdateAfterBindUniformBuffersDynamic, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetUpdateAfterBindStorageBuffers, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetUpdateAfterBindStorageBuffersDynamic, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetUpdateAfterBindSampledImages, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetUpdateAfterBindStorageImages, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetUpdateAfterBindInputAttachments, WarnIfGreater);
        GET_VALUE_FLAG_WARN(member, supportedDepthResolveModes);
        GET_VALUE_FLAG_WARN(member, supportedStencilResolveModes);
        GET_VALUE_WARN(member, independentResolveNone, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, independentResolve, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, filterMinmaxSingleComponentFormats, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, filterMinmaxImageComponentMapping, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, maxTimelineSemaphoreValueDifference, WarnIfGreater);
        GET_VALUE_FLAG_WARN(member, framebufferIntegerColorSampleCounts);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceVulkan13Properties *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceVulkan13Properties)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, minSubgroupSize, WarnIfLesser);
        GET_VALUE_WARN(member, maxSubgroupSize, WarnIfGreater);
        GET_VALUE_WARN(member, maxComputeWorkgroupSubgroups, WarnIfGreater);
        GET_VALUE_FLAG_WARN(member, requiredSubgroupSizeStages);
        GET_VALUE_WARN(member, maxInlineUniformBlockSize, WarnIfGreater);
        GET_VALUE_WARN(member, maxPerStageDescriptorInlineUniformBlocks, WarnIfGreater);
        GET_VALUE_WARN(member, maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetInlineUniformBlocks, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetUpdateAfterBindInlineUniformBlocks, WarnIfGreater);
        GET_VALUE_WARN(member, maxInlineUniformTotalSize, WarnIfGreater);
        GET_VALUE_WARN(member, integerDotProduct8BitUnsignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct8BitSignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct8BitMixedSignednessAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct4x8BitPackedUnsignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct4x8BitPackedSignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct4x8BitPackedMixedSignednessAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct16BitUnsignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct16BitSignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct16BitMixedSignednessAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct32BitUnsignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct32BitSignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct32BitMixedSignednessAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct64BitUnsignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct64BitSignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct64BitMixedSignednessAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating8BitUnsignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating8BitSignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating8BitMixedSignednessAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating4x8BitPackedUnsignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating4x8BitPackedSignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating4x8BitPackedMixedSignednessAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating16BitUnsignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating16BitSignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating16BitMixedSignednessAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating32BitUnsignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating32BitSignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating32BitMixedSignednessAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating64BitUnsignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating64BitSignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating64BitMixedSignednessAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, storageTexelBufferOffsetAlignmentBytes, WarnIfLesser);
        WarnNotModifiable("VkPhysicalDeviceVulkan13Properties", member, "storageTexelBufferOffsetSingleTexelAlignment");
        GET_VALUE_WARN(member, uniformTexelBufferOffsetAlignmentBytes, WarnIfLesser);
        WarnNotModifiable("VkPhysicalDeviceVulkan13Properties", member, "uniformTexelBufferOffsetSingleTexelAlignment");
        GET_VALUE_WARN(member, maxBufferSize, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceProtectedMemoryFeatures *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceProtectedMemoryFeatures)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, protectedMemory, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderDrawParameterFeatures *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceShaderDrawParameterFeatures)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderDrawParameters, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceVulkan11Features *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceVulkan11Features)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, storageBuffer16BitAccess, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, uniformAndStorageBuffer16BitAccess, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, storagePushConstant16, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, storageInputOutput16, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, multiview, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, multiviewGeometryShader, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, multiviewTessellationShader, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, variablePointersStorageBuffer, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, variablePointers, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, protectedMemory, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, samplerYcbcrConversion, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderDrawParameters, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceVulkan12Features *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceVulkan12Features)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, samplerMirrorClampToEdge, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, drawIndirectCount, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, storageBuffer8BitAccess, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, uniformAndStorageBuffer8BitAccess, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, storagePushConstant8, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderBufferInt64Atomics, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderSharedInt64Atomics, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderFloat16, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderInt8, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, descriptorIndexing, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderInputAttachmentArrayDynamicIndexing, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderUniformTexelBufferArrayDynamicIndexing, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderStorageTexelBufferArrayDynamicIndexing, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderUniformBufferArrayNonUniformIndexing, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderSampledImageArrayNonUniformIndexing, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderStorageBufferArrayNonUniformIndexing, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderStorageImageArrayNonUniformIndexing, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderInputAttachmentArrayNonUniformIndexing, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderUniformTexelBufferArrayNonUniformIndexing, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderStorageTexelBufferArrayNonUniformIndexing, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, descriptorBindingUniformBufferUpdateAfterBind, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, descriptorBindingSampledImageUpdateAfterBind, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, descriptorBindingStorageImageUpdateAfterBind, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, descriptorBindingStorageBufferUpdateAfterBind, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, descriptorBindingUniformTexelBufferUpdateAfterBind, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, descriptorBindingStorageTexelBufferUpdateAfterBind, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, descriptorBindingUpdateUnusedWhilePending, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, descriptorBindingPartiallyBound, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, descriptorBindingVariableDescriptorCount, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, runtimeDescriptorArray, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, samplerFilterMinmax, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, scalarBlockLayout, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, imagelessFramebuffer, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, uniformBufferStandardLayout, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderSubgroupExtendedTypes, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, separateDepthStencilLayouts, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, hostQueryReset, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, timelineSemaphore, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, bufferDeviceAddress, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, bufferDeviceAddressCaptureReplay, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, bufferDeviceAddressMultiDevice, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, vulkanMemoryModel, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, vulkanMemoryModelDeviceScope, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, vulkanMemoryModelAvailabilityVisibilityChains, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderOutputViewportIndex, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderOutputLayer, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, subgroupBroadcastDynamicId, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceVulkan13Features *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceVulkan13Features)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, robustImageAccess, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, inlineUniformBlock, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, descriptorBindingInlineUniformBlockUpdateAfterBind, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, pipelineCreationCacheControl, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, privateData, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderDemoteToHelperInvocation, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderTerminateInvocation, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, subgroupSizeControl, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, computeFullSubgroups, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, synchronization2, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, textureCompressionASTC_HDR, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderZeroInitializeWorkgroupMemory, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, dynamicRendering, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderIntegerDotProduct, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, maintenance4, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceTransformFeedbackPropertiesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceTransformFeedbackPropertiesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, maxTransformFeedbackStreams, WarnIfGreater);
        GET_VALUE_WARN(member, maxTransformFeedbackBuffers, WarnIfGreater);
        GET_VALUE_WARN(member, maxTransformFeedbackBufferSize, WarnIfGreater);
        GET_VALUE_WARN(member, maxTransformFeedbackStreamDataSize, WarnIfGreater);
        GET_VALUE_WARN(member, maxTransformFeedbackBufferDataSize, WarnIfGreater);
        GET_VALUE_WARN(member, maxTransformFeedbackBufferDataStride, WarnIfGreater);
        GET_VALUE_WARN(member, transformFeedbackQueries, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, transformFeedbackStreamsLinesTriangles, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, transformFeedbackRasterizationStreamSelect, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, transformFeedbackDraw, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceTransformFeedbackFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceTransformFeedbackFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, transformFeedback, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, geometryStreams, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDynamicRenderingFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceDynamicRenderingFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, dynamicRendering, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceCornerSampledImageFeaturesNV *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceCornerSampledImageFeaturesNV)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, cornerSampledImage, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceMultiviewPropertiesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceMultiviewPropertiesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, maxMultiviewViewCount, WarnIfGreater);
        GET_VALUE_WARN(member, maxMultiviewInstanceIndex, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceMultiviewFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceMultiviewFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, multiview, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, multiviewGeometryShader, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, multiviewTessellationShader, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, textureCompressionASTC_HDR, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceASTCDecodeFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceASTCDecodeFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, decodeModeSharedExponent, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePipelineRobustnessPropertiesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDevicePipelineRobustnessPropertiesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        WarnNotModifiable("VkPhysicalDevicePipelineRobustnessPropertiesEXT", member, "defaultRobustnessStorageBuffers");
        WarnNotModifiable("VkPhysicalDevicePipelineRobustnessPropertiesEXT", member, "defaultRobustnessUniformBuffers");
        WarnNotModifiable("VkPhysicalDevicePipelineRobustnessPropertiesEXT", member, "defaultRobustnessVertexInputs");
        WarnNotModifiable("VkPhysicalDevicePipelineRobustnessPropertiesEXT", member, "defaultRobustnessImages");
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePipelineRobustnessFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDevicePipelineRobustnessFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, pipelineRobustness, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceIDPropertiesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceIDPropertiesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        WarnNotModifiable("VkPhysicalDeviceIDPropertiesKHR", member, "deviceUUID");
        WarnNotModifiable("VkPhysicalDeviceIDPropertiesKHR", member, "driverUUID");
        WarnNotModifiable("VkPhysicalDeviceIDPropertiesKHR", member, "deviceLUID");
        WarnNotModifiable("VkPhysicalDeviceIDPropertiesKHR", member, "deviceNodeMask");
        WarnNotModifiable("VkPhysicalDeviceIDPropertiesKHR", member, "deviceLUIDValid");
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePushDescriptorPropertiesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDevicePushDescriptorPropertiesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, maxPushDescriptors, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceConditionalRenderingFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceConditionalRenderingFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, conditionalRendering, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, inheritedConditionalRendering, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFloat16Int8FeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceFloat16Int8FeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderFloat16, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderInt8, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevice16BitStorageFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDevice16BitStorageFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, storageBuffer16BitAccess, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, uniformAndStorageBuffer16BitAccess, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, storagePushConstant16, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, storageInputOutput16, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceMultiviewPerViewAttributesPropertiesNVX *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceMultiviewPerViewAttributesPropertiesNVX)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, perViewPositionAllComponents, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDiscardRectanglePropertiesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceDiscardRectanglePropertiesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, maxDiscardRectangles, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceConservativeRasterizationPropertiesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceConservativeRasterizationPropertiesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        WarnNotModifiable("VkPhysicalDeviceConservativeRasterizationPropertiesEXT", member, "primitiveOverestimationSize");
        GET_VALUE_WARN(member, maxExtraPrimitiveOverestimationSize, WarnIfGreaterFloat);
        GET_VALUE_WARN(member, extraPrimitiveOverestimationSizeGranularity, WarnIfLesserFloat);
        GET_VALUE_WARN(member, primitiveUnderestimation, WarnIfNotEqualBool);
        WarnNotModifiable("VkPhysicalDeviceConservativeRasterizationPropertiesEXT", member, "conservativePointAndLineRasterization");
        WarnNotModifiable("VkPhysicalDeviceConservativeRasterizationPropertiesEXT", member, "degenerateTrianglesRasterized");
        WarnNotModifiable("VkPhysicalDeviceConservativeRasterizationPropertiesEXT", member, "degenerateLinesRasterized");
        GET_VALUE_WARN(member, fullyCoveredFragmentShaderInputVariable, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, conservativeRasterizationPostDepthCoverage, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDepthClipEnableFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceDepthClipEnableFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, depthClipEnable, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceImagelessFramebufferFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceImagelessFramebufferFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, imagelessFramebuffer, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePerformanceQueryPropertiesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDevicePerformanceQueryPropertiesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, allowCommandBufferQueryCopies, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePerformanceQueryFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDevicePerformanceQueryFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, performanceCounterQueryPools, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, performanceCounterMultipleQueryPools, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePointClippingPropertiesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDevicePointClippingPropertiesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        WarnNotModifiable("VkPhysicalDevicePointClippingPropertiesKHR", member, "pointClippingBehavior");
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceVariablePointerFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceVariablePointerFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, variablePointersStorageBuffer, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, variablePointers, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, filterMinmaxSingleComponentFormats, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, filterMinmaxImageComponentMapping, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceInlineUniformBlockPropertiesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceInlineUniformBlockPropertiesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, maxInlineUniformBlockSize, WarnIfGreater);
        GET_VALUE_WARN(member, maxPerStageDescriptorInlineUniformBlocks, WarnIfGreater);
        GET_VALUE_WARN(member, maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetInlineUniformBlocks, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetUpdateAfterBindInlineUniformBlocks, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceInlineUniformBlockFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceInlineUniformBlockFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, inlineUniformBlock, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, descriptorBindingInlineUniformBlockUpdateAfterBind, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceSampleLocationsPropertiesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceSampleLocationsPropertiesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_FLAG_WARN(member, sampleLocationSampleCounts);
        GET_VALUE_WARN(member, maxSampleLocationGridSize, WarnIfGreater);
        GET_ARRAY(sampleLocationCoordinateRange);
        GET_VALUE_WARN(member, sampleLocationSubPixelBits, WarnIfGreater);
        GET_VALUE_WARN(member, variableSampleLocations, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, advancedBlendMaxColorAttachments, WarnIfGreater);
        GET_VALUE_WARN(member, advancedBlendIndependentBlend, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, advancedBlendNonPremultipliedSrcColor, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, advancedBlendNonPremultipliedDstColor, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, advancedBlendCorrelatedOverlap, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, advancedBlendAllOperations, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, advancedBlendCoherentOperations, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceAccelerationStructurePropertiesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceAccelerationStructurePropertiesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, maxGeometryCount, WarnIfGreater);
        GET_VALUE_WARN(member, maxInstanceCount, WarnIfGreater);
        GET_VALUE_WARN(member, maxPrimitiveCount, WarnIfGreater);
        GET_VALUE_WARN(member, maxPerStageDescriptorAccelerationStructures, WarnIfGreater);
        GET_VALUE_WARN(member, maxPerStageDescriptorUpdateAfterBindAccelerationStructures, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetAccelerationStructures, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetUpdateAfterBindAccelerationStructures, WarnIfGreater);
        GET_VALUE_WARN(member, minAccelerationStructureScratchOffsetAlignment, WarnIfLesser);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceAccelerationStructureFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceAccelerationStructureFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, accelerationStructure, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, accelerationStructureCaptureReplay, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, accelerationStructureIndirectBuild, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, accelerationStructureHostCommands, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, descriptorBindingAccelerationStructureUpdateAfterBind, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceRayTracingPipelinePropertiesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceRayTracingPipelinePropertiesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        WarnNotModifiable("VkPhysicalDeviceRayTracingPipelinePropertiesKHR", member, "shaderGroupHandleSize");
        GET_VALUE_WARN(member, maxRayRecursionDepth, WarnIfGreater);
        GET_VALUE_WARN(member, maxShaderGroupStride, WarnIfGreater);
        WarnNotModifiable("VkPhysicalDeviceRayTracingPipelinePropertiesKHR", member, "shaderGroupBaseAlignment");
        WarnNotModifiable("VkPhysicalDeviceRayTracingPipelinePropertiesKHR", member, "shaderGroupHandleCaptureReplaySize");
        GET_VALUE_WARN(member, maxRayDispatchInvocationCount, WarnIfGreater);
        GET_VALUE_WARN(member, shaderGroupHandleAlignment, WarnIfLesser);
        GET_VALUE_WARN(member, maxRayHitAttributeSize, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceRayTracingPipelineFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceRayTracingPipelineFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, rayTracingPipeline, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, rayTracingPipelineShaderGroupHandleCaptureReplay, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, rayTracingPipelineShaderGroupHandleCaptureReplayMixed, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, rayTracingPipelineTraceRaysIndirect, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, rayTraversalPrimitiveCulling, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceRayQueryFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceRayQueryFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, rayQuery, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderSMBuiltinsPropertiesNV *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceShaderSMBuiltinsPropertiesNV)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderSMCount, WarnIfGreater);
        GET_VALUE_WARN(member, shaderWarpsPerSM, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderSMBuiltinsFeaturesNV *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceShaderSMBuiltinsFeaturesNV)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderSMBuiltins, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, samplerYcbcrConversion, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDescriptorIndexingPropertiesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceDescriptorIndexingPropertiesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, maxUpdateAfterBindDescriptorsInAllPools, WarnIfGreater);
        GET_VALUE_WARN(member, shaderUniformBufferArrayNonUniformIndexingNative, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderSampledImageArrayNonUniformIndexingNative, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderStorageBufferArrayNonUniformIndexingNative, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderStorageImageArrayNonUniformIndexingNative, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderInputAttachmentArrayNonUniformIndexingNative, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, robustBufferAccessUpdateAfterBind, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, quadDivergentImplicitLod, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, maxPerStageDescriptorUpdateAfterBindSamplers, WarnIfGreater);
        GET_VALUE_WARN(member, maxPerStageDescriptorUpdateAfterBindUniformBuffers, WarnIfGreater);
        GET_VALUE_WARN(member, maxPerStageDescriptorUpdateAfterBindStorageBuffers, WarnIfGreater);
        GET_VALUE_WARN(member, maxPerStageDescriptorUpdateAfterBindSampledImages, WarnIfGreater);
        GET_VALUE_WARN(member, maxPerStageDescriptorUpdateAfterBindStorageImages, WarnIfGreater);
        GET_VALUE_WARN(member, maxPerStageDescriptorUpdateAfterBindInputAttachments, WarnIfGreater);
        GET_VALUE_WARN(member, maxPerStageUpdateAfterBindResources, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetUpdateAfterBindSamplers, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetUpdateAfterBindUniformBuffers, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetUpdateAfterBindUniformBuffersDynamic, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetUpdateAfterBindStorageBuffers, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetUpdateAfterBindStorageBuffersDynamic, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetUpdateAfterBindSampledImages, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetUpdateAfterBindStorageImages, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetUpdateAfterBindInputAttachments, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDescriptorIndexingFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceDescriptorIndexingFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderInputAttachmentArrayDynamicIndexing, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderUniformTexelBufferArrayDynamicIndexing, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderStorageTexelBufferArrayDynamicIndexing, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderUniformBufferArrayNonUniformIndexing, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderSampledImageArrayNonUniformIndexing, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderStorageBufferArrayNonUniformIndexing, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderStorageImageArrayNonUniformIndexing, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderInputAttachmentArrayNonUniformIndexing, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderUniformTexelBufferArrayNonUniformIndexing, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderStorageTexelBufferArrayNonUniformIndexing, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, descriptorBindingUniformBufferUpdateAfterBind, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, descriptorBindingSampledImageUpdateAfterBind, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, descriptorBindingStorageImageUpdateAfterBind, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, descriptorBindingStorageBufferUpdateAfterBind, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, descriptorBindingUniformTexelBufferUpdateAfterBind, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, descriptorBindingStorageTexelBufferUpdateAfterBind, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, descriptorBindingUpdateUnusedWhilePending, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, descriptorBindingPartiallyBound, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, descriptorBindingVariableDescriptorCount, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, runtimeDescriptorArray, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePortabilitySubsetPropertiesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDevicePortabilitySubsetPropertiesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, minVertexInputBindingStrideAlignment, WarnIfLesser);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePortabilitySubsetFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDevicePortabilitySubsetFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, constantAlphaColorBlendFactors, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, events, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, imageViewFormatReinterpretation, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, imageViewFormatSwizzle, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, imageView2DOn3DImage, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, multisampleArrayImage, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, mutableComparisonSamplers, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, pointPolygons, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, samplerMipLodBias, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, separateStencilMaskRef, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderSampleRateInterpolationFunctions, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, tessellationIsolines, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, tessellationPointMode, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, triangleFans, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, vertexAttributeAccessBeyondStride, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShadingRateImagePropertiesNV *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceShadingRateImagePropertiesNV)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        WarnNotModifiable("VkPhysicalDeviceShadingRateImagePropertiesNV", member, "shadingRateTexelSize");
        GET_VALUE_WARN(member, shadingRatePaletteSize, WarnIfGreater);
        GET_VALUE_WARN(member, shadingRateMaxCoarseSamples, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShadingRateImageFeaturesNV *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceShadingRateImageFeaturesNV)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shadingRateImage, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shadingRateCoarseSampleOrder, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceRayTracingPropertiesNV *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceRayTracingPropertiesNV)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        WarnNotModifiable("VkPhysicalDeviceRayTracingPropertiesNV", member, "shaderGroupHandleSize");
        GET_VALUE_WARN(member, maxRecursionDepth, WarnIfGreater);
        GET_VALUE_WARN(member, maxShaderGroupStride, WarnIfGreater);
        WarnNotModifiable("VkPhysicalDeviceRayTracingPropertiesNV", member, "shaderGroupBaseAlignment");
        GET_VALUE_WARN(member, maxGeometryCount, WarnIfGreater);
        GET_VALUE_WARN(member, maxInstanceCount, WarnIfGreater);
        GET_VALUE_WARN(member, maxTriangleCount, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetAccelerationStructures, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, representativeFragmentTest, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceMaintenance3PropertiesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceMaintenance3PropertiesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, maxPerSetDescriptors, WarnIfGreater);
        GET_VALUE_WARN(member, maxMemoryAllocationSize, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderSubgroupExtendedTypes, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevice8BitStorageFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDevice8BitStorageFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, storageBuffer8BitAccess, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, uniformAndStorageBuffer8BitAccess, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, storagePushConstant8, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceExternalMemoryHostPropertiesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceExternalMemoryHostPropertiesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, minImportedHostPointerAlignment, WarnIfLesser);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderAtomicInt64FeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceShaderAtomicInt64FeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderBufferInt64Atomics, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderSharedInt64Atomics, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderClockFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceShaderClockFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderSubgroupClock, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderDeviceClock, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderCorePropertiesAMD *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceShaderCorePropertiesAMD)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        WarnNotModifiable("VkPhysicalDeviceShaderCorePropertiesAMD", member, "shaderEngineCount");
        WarnNotModifiable("VkPhysicalDeviceShaderCorePropertiesAMD", member, "shaderArraysPerEngineCount");
        WarnNotModifiable("VkPhysicalDeviceShaderCorePropertiesAMD", member, "computeUnitsPerShaderArray");
        WarnNotModifiable("VkPhysicalDeviceShaderCorePropertiesAMD", member, "simdPerComputeUnit");
        WarnNotModifiable("VkPhysicalDeviceShaderCorePropertiesAMD", member, "wavefrontsPerSimd");
        GET_VALUE_WARN(member, wavefrontSize, WarnIfGreater);
        WarnNotModifiable("VkPhysicalDeviceShaderCorePropertiesAMD", member, "sgprsPerSimd");
        GET_VALUE_WARN(member, minSgprAllocation, WarnIfLesser);
        GET_VALUE_WARN(member, maxSgprAllocation, WarnIfGreater);
        GET_VALUE_WARN(member, sgprAllocationGranularity, WarnIfLesser);
        WarnNotModifiable("VkPhysicalDeviceShaderCorePropertiesAMD", member, "vgprsPerSimd");
        GET_VALUE_WARN(member, minVgprAllocation, WarnIfLesser);
        GET_VALUE_WARN(member, maxVgprAllocation, WarnIfGreater);
        GET_VALUE_WARN(member, vgprAllocationGranularity, WarnIfLesser);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, maxVertexAttribDivisor, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, vertexAttributeInstanceRateDivisor, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, vertexAttributeInstanceRateZeroDivisor, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDriverPropertiesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceDriverPropertiesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        WarnNotModifiable("VkPhysicalDeviceDriverPropertiesKHR", member, "driverID");
        WarnNotModifiable("VkPhysicalDeviceDriverPropertiesKHR", member, "driverName");
        WarnNotModifiable("VkPhysicalDeviceDriverPropertiesKHR", member, "driverInfo");
        WarnNotModifiable("VkPhysicalDeviceDriverPropertiesKHR", member, "conformanceVersion");
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFloatControlsPropertiesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceFloatControlsPropertiesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        WarnNotModifiable("VkPhysicalDeviceFloatControlsPropertiesKHR", member, "denormBehaviorIndependence");
        WarnNotModifiable("VkPhysicalDeviceFloatControlsPropertiesKHR", member, "roundingModeIndependence");
        GET_VALUE_WARN(member, shaderSignedZeroInfNanPreserveFloat16, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderSignedZeroInfNanPreserveFloat32, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderSignedZeroInfNanPreserveFloat64, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderDenormPreserveFloat16, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderDenormPreserveFloat32, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderDenormPreserveFloat64, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderDenormFlushToZeroFloat16, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderDenormFlushToZeroFloat32, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderDenormFlushToZeroFloat64, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderRoundingModeRTEFloat16, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderRoundingModeRTEFloat32, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderRoundingModeRTEFloat64, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderRoundingModeRTZFloat16, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderRoundingModeRTZFloat32, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderRoundingModeRTZFloat64, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDepthStencilResolvePropertiesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceDepthStencilResolvePropertiesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_FLAG_WARN(member, supportedDepthResolveModes);
        GET_VALUE_FLAG_WARN(member, supportedStencilResolveModes);
        GET_VALUE_WARN(member, independentResolveNone, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, independentResolve, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceComputeShaderDerivativesFeaturesNV *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceComputeShaderDerivativesFeaturesNV)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, computeDerivativeGroupQuads, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, computeDerivativeGroupLinear, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceMeshShaderPropertiesNV *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceMeshShaderPropertiesNV)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, maxDrawMeshTasksCount, WarnIfGreater);
        GET_VALUE_WARN(member, maxTaskWorkGroupInvocations, WarnIfGreater);
        GET_ARRAY(maxTaskWorkGroupSize);
        GET_VALUE_WARN(member, maxTaskTotalMemorySize, WarnIfGreater);
        GET_VALUE_WARN(member, maxTaskOutputCount, WarnIfGreater);
        GET_VALUE_WARN(member, maxMeshWorkGroupInvocations, WarnIfGreater);
        GET_ARRAY(maxMeshWorkGroupSize);
        GET_VALUE_WARN(member, maxMeshTotalMemorySize, WarnIfGreater);
        GET_VALUE_WARN(member, maxMeshOutputVertices, WarnIfGreater);
        GET_VALUE_WARN(member, maxMeshOutputPrimitives, WarnIfGreater);
        GET_VALUE_WARN(member, maxMeshMultiviewViewCount, WarnIfGreater);
        GET_VALUE_WARN(member, meshOutputPerVertexGranularity, WarnIfLesser);
        GET_VALUE_WARN(member, meshOutputPerPrimitiveGranularity, WarnIfLesser);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceMeshShaderFeaturesNV *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceMeshShaderFeaturesNV)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, taskShader, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, meshShader, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, fragmentShaderBarycentric, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderImageFootprintFeaturesNV *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceShaderImageFootprintFeaturesNV)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, imageFootprint, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceExclusiveScissorFeaturesNV *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceExclusiveScissorFeaturesNV)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, exclusiveScissor, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceTimelineSemaphorePropertiesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceTimelineSemaphorePropertiesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, maxTimelineSemaphoreValueDifference, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceTimelineSemaphoreFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceTimelineSemaphoreFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, timelineSemaphore, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderIntegerFunctions2, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceVulkanMemoryModelFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceVulkanMemoryModelFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, vulkanMemoryModel, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, vulkanMemoryModelDeviceScope, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, vulkanMemoryModelAvailabilityVisibilityChains, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePCIBusInfoPropertiesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDevicePCIBusInfoPropertiesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        WarnNotModifiable("VkPhysicalDevicePCIBusInfoPropertiesEXT", member, "pciDomain");
        WarnNotModifiable("VkPhysicalDevicePCIBusInfoPropertiesEXT", member, "pciBus");
        WarnNotModifiable("VkPhysicalDevicePCIBusInfoPropertiesEXT", member, "pciDevice");
        WarnNotModifiable("VkPhysicalDevicePCIBusInfoPropertiesEXT", member, "pciFunction");
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderTerminateInvocation, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentDensityMapPropertiesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceFragmentDensityMapPropertiesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, minFragmentDensityTexelSize, WarnIfLesser);
        GET_VALUE_WARN(member, maxFragmentDensityTexelSize, WarnIfGreater);
        GET_VALUE_WARN(member, fragmentDensityInvocations, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentDensityMapFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceFragmentDensityMapFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, fragmentDensityMap, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, fragmentDensityMapDynamic, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, fragmentDensityMapNonSubsampledImages, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceScalarBlockLayoutFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceScalarBlockLayoutFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, scalarBlockLayout, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceSubgroupSizeControlPropertiesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceSubgroupSizeControlPropertiesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, minSubgroupSize, WarnIfLesser);
        GET_VALUE_WARN(member, maxSubgroupSize, WarnIfGreater);
        GET_VALUE_WARN(member, maxComputeWorkgroupSubgroups, WarnIfGreater);
        GET_VALUE_FLAG_WARN(member, requiredSubgroupSizeStages);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceSubgroupSizeControlFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceSubgroupSizeControlFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, subgroupSizeControl, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, computeFullSubgroups, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentShadingRatePropertiesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceFragmentShadingRatePropertiesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, minFragmentShadingRateAttachmentTexelSize, WarnIfLesser);
        GET_VALUE_WARN(member, maxFragmentShadingRateAttachmentTexelSize, WarnIfGreater);
        GET_VALUE_WARN(member, maxFragmentShadingRateAttachmentTexelSizeAspectRatio, WarnIfGreater);
        GET_VALUE_WARN(member, primitiveFragmentShadingRateWithMultipleViewports, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, layeredShadingRateAttachments, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, fragmentShadingRateNonTrivialCombinerOps, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, maxFragmentSize, WarnIfGreater);
        GET_VALUE_WARN(member, maxFragmentSizeAspectRatio, WarnIfGreater);
        GET_VALUE_WARN(member, maxFragmentShadingRateCoverageSamples, WarnIfGreater);
        GET_VALUE_ENUM_WARN(member, maxFragmentShadingRateRasterizationSamples, WarnIfGreater);
        GET_VALUE_WARN(member, fragmentShadingRateWithShaderDepthStencilWrites, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, fragmentShadingRateWithSampleMask, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, fragmentShadingRateWithShaderSampleMask, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, fragmentShadingRateWithConservativeRasterization, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, fragmentShadingRateWithFragmentShaderInterlock, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, fragmentShadingRateWithCustomSampleLocations, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, fragmentShadingRateStrictMultiplyCombiner, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentShadingRateFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceFragmentShadingRateFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, pipelineFragmentShadingRate, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, primitiveFragmentShadingRate, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, attachmentFragmentShadingRate, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderCoreProperties2AMD *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceShaderCoreProperties2AMD)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_FLAG_WARN(member, shaderCoreFeatures);
        GET_VALUE_WARN(member, activeComputeUnitCount, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceCoherentMemoryFeaturesAMD *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceCoherentMemoryFeaturesAMD)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, deviceCoherentMemory, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderImageInt64Atomics, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, sparseImageInt64Atomics, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceMemoryPriorityFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceMemoryPriorityFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, memoryPriority, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, dedicatedAllocationImageAliasing, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, separateDepthStencilLayouts, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceBufferAddressFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceBufferAddressFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, bufferDeviceAddress, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, bufferDeviceAddressCaptureReplay, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, bufferDeviceAddressMultiDevice, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePresentWaitFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDevicePresentWaitFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, presentWait, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceCooperativeMatrixPropertiesNV *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceCooperativeMatrixPropertiesNV)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_FLAG_WARN(member, cooperativeMatrixSupportedStages);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceCooperativeMatrixFeaturesNV *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceCooperativeMatrixFeaturesNV)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, cooperativeMatrix, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, cooperativeMatrixRobustBufferAccess, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceCoverageReductionModeFeaturesNV *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceCoverageReductionModeFeaturesNV)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, coverageReductionMode, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, fragmentShaderSampleInterlock, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, fragmentShaderPixelInterlock, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, fragmentShaderShadingRateInterlock, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceYcbcrImageArraysFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceYcbcrImageArraysFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, ycbcrImageArrays, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, uniformBufferStandardLayout, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceProvokingVertexPropertiesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceProvokingVertexPropertiesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, provokingVertexModePerPipeline, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, transformFeedbackPreservesTriangleFanProvokingVertex, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceProvokingVertexFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceProvokingVertexFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, provokingVertexLast, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, transformFeedbackPreservesProvokingVertex, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceBufferDeviceAddressFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceBufferDeviceAddressFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, bufferDeviceAddress, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, bufferDeviceAddressCaptureReplay, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, bufferDeviceAddressMultiDevice, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceLineRasterizationPropertiesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceLineRasterizationPropertiesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, lineSubPixelPrecisionBits, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceLineRasterizationFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceLineRasterizationFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, rectangularLines, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, bresenhamLines, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, smoothLines, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, stippledRectangularLines, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, stippledBresenhamLines, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, stippledSmoothLines, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderAtomicFloatFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceShaderAtomicFloatFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderBufferFloat32Atomics, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderBufferFloat32AtomicAdd, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderBufferFloat64Atomics, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderBufferFloat64AtomicAdd, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderSharedFloat32Atomics, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderSharedFloat32AtomicAdd, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderSharedFloat64Atomics, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderSharedFloat64AtomicAdd, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderImageFloat32Atomics, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderImageFloat32AtomicAdd, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, sparseImageFloat32Atomics, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, sparseImageFloat32AtomicAdd, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceHostQueryResetFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceHostQueryResetFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, hostQueryReset, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceIndexTypeUint8FeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceIndexTypeUint8FeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, indexTypeUint8, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceExtendedDynamicStateFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceExtendedDynamicStateFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, extendedDynamicState, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, pipelineExecutableInfo, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderBufferFloat16Atomics, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderBufferFloat16AtomicAdd, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderBufferFloat16AtomicMinMax, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderBufferFloat32AtomicMinMax, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderBufferFloat64AtomicMinMax, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderSharedFloat16Atomics, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderSharedFloat16AtomicAdd, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderSharedFloat16AtomicMinMax, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderSharedFloat32AtomicMinMax, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderSharedFloat64AtomicMinMax, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderImageFloat32AtomicMinMax, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, sparseImageFloat32AtomicMinMax, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderDemoteToHelperInvocation, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDeviceGeneratedCommandsPropertiesNV *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceDeviceGeneratedCommandsPropertiesNV)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, maxGraphicsShaderGroupCount, WarnIfGreater);
        GET_VALUE_WARN(member, maxIndirectSequenceCount, WarnIfGreater);
        GET_VALUE_WARN(member, maxIndirectCommandsTokenCount, WarnIfGreater);
        GET_VALUE_WARN(member, maxIndirectCommandsStreamCount, WarnIfGreater);
        GET_VALUE_WARN(member, maxIndirectCommandsTokenOffset, WarnIfGreater);
        GET_VALUE_WARN(member, maxIndirectCommandsStreamStride, WarnIfGreater);
        GET_VALUE_WARN(member, minSequencesCountBufferOffsetAlignment, WarnIfLesser);
        GET_VALUE_WARN(member, minSequencesIndexBufferOffsetAlignment, WarnIfLesser);
        GET_VALUE_WARN(member, minIndirectCommandsBufferOffsetAlignment, WarnIfLesser);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, deviceGeneratedCommands, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceInheritedViewportScissorFeaturesNV *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceInheritedViewportScissorFeaturesNV)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, inheritedViewportScissor2D, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, integerDotProduct8BitUnsignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct8BitSignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct8BitMixedSignednessAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct4x8BitPackedUnsignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct4x8BitPackedSignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct4x8BitPackedMixedSignednessAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct16BitUnsignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct16BitSignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct16BitMixedSignednessAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct32BitUnsignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct32BitSignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct32BitMixedSignednessAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct64BitUnsignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct64BitSignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProduct64BitMixedSignednessAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating8BitUnsignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating8BitSignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating8BitMixedSignednessAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating4x8BitPackedUnsignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating4x8BitPackedSignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating4x8BitPackedMixedSignednessAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating16BitUnsignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating16BitSignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating16BitMixedSignednessAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating32BitUnsignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating32BitSignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating32BitMixedSignednessAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating64BitUnsignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating64BitSignedAccelerated, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, integerDotProductAccumulatingSaturating64BitMixedSignednessAccelerated, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderIntegerDotProduct, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, storageTexelBufferOffsetAlignmentBytes, WarnIfLesser);
        WarnNotModifiable("VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT", member, "storageTexelBufferOffsetSingleTexelAlignment");
        GET_VALUE_WARN(member, uniformTexelBufferOffsetAlignmentBytes, WarnIfLesser);
        WarnNotModifiable("VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT", member, "uniformTexelBufferOffsetSingleTexelAlignment");
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, texelBufferAlignment, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDeviceMemoryReportFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceDeviceMemoryReportFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, deviceMemoryReport, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceRobustness2PropertiesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceRobustness2PropertiesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, robustStorageBufferAccessSizeAlignment, WarnIfLesser);
        GET_VALUE_WARN(member, robustUniformBufferAccessSizeAlignment, WarnIfLesser);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceRobustness2FeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceRobustness2FeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, robustBufferAccess2, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, robustImageAccess2, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, nullDescriptor, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceCustomBorderColorPropertiesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceCustomBorderColorPropertiesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, maxCustomBorderColorSamplers, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceCustomBorderColorFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceCustomBorderColorFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, customBorderColors, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, customBorderColorWithoutFormat, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePresentIdFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDevicePresentIdFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, presentId, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePrivateDataFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDevicePrivateDataFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, privateData, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, pipelineCreationCacheControl, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDiagnosticsConfigFeaturesNV *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceDiagnosticsConfigFeaturesNV)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, diagnosticsConfig, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceSynchronization2FeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceSynchronization2FeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, synchronization2, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceGraphicsPipelineLibraryPropertiesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceGraphicsPipelineLibraryPropertiesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, graphicsPipelineLibraryFastLinking, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, graphicsPipelineLibraryIndependentInterpolationDecoration, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, graphicsPipelineLibrary, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderEarlyAndLateFragmentTests, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentShaderBarycentricPropertiesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceFragmentShaderBarycentricPropertiesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, triStripVertexOrderIndependentOfProvokingVertex, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderSubgroupUniformControlFlow, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderZeroInitializeWorkgroupMemory, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentShadingRateEnumsPropertiesNV *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceFragmentShadingRateEnumsPropertiesNV)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_ENUM_WARN(member, maxFragmentShadingRateInvocationCount, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, fragmentShadingRateEnums, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, supersampleFragmentShadingRates, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, noInvocationFragmentShadingRates, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceRayTracingMotionBlurFeaturesNV *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceRayTracingMotionBlurFeaturesNV)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, rayTracingMotionBlur, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, rayTracingMotionBlurPipelineTraceRaysIndirect, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, ycbcr2plane444Formats, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentDensityMap2PropertiesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceFragmentDensityMap2PropertiesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        WarnNotModifiable("VkPhysicalDeviceFragmentDensityMap2PropertiesEXT", member, "subsampledLoads");
        WarnNotModifiable("VkPhysicalDeviceFragmentDensityMap2PropertiesEXT", member, "subsampledCoarseReconstructionEarlyAccess");
        GET_VALUE_WARN(member, maxSubsampledArrayLayers, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetSubsampledSamplers, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentDensityMap2FeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceFragmentDensityMap2FeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, fragmentDensityMapDeferred, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceImageRobustnessFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceImageRobustnessFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, robustImageAccess, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, workgroupMemoryExplicitLayout, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, workgroupMemoryExplicitLayoutScalarBlockLayout, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, workgroupMemoryExplicitLayout8BitAccess, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, workgroupMemoryExplicitLayout16BitAccess, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceImageCompressionControlFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceImageCompressionControlFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, imageCompressionControl, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, attachmentFeedbackLoopLayout, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevice4444FormatsFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDevice4444FormatsFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, formatA4R4G4B4, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, formatA4B4G4R4, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, rasterizationOrderColorAttachmentAccess, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, rasterizationOrderDepthAttachmentAccess, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, rasterizationOrderStencilAttachmentAccess, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, formatRgba10x6WithoutYCbCrSampler, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, mutableDescriptorType, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, vertexInputDynamicState, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDrmPropertiesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceDrmPropertiesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        WarnNotModifiable("VkPhysicalDeviceDrmPropertiesEXT", member, "hasPrimary");
        WarnNotModifiable("VkPhysicalDeviceDrmPropertiesEXT", member, "hasRender");
        WarnNotModifiable("VkPhysicalDeviceDrmPropertiesEXT", member, "primaryMajor");
        WarnNotModifiable("VkPhysicalDeviceDrmPropertiesEXT", member, "primaryMinor");
        WarnNotModifiable("VkPhysicalDeviceDrmPropertiesEXT", member, "renderMajor");
        WarnNotModifiable("VkPhysicalDeviceDrmPropertiesEXT", member, "renderMinor");
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDepthClipControlFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceDepthClipControlFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, depthClipControl, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, primitiveTopologyListRestart, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, primitiveTopologyPatchListRestart, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceSubpassShadingPropertiesHUAWEI *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceSubpassShadingPropertiesHUAWEI)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, maxSubpassShadingWorkgroupSizeAspectRatio, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceSubpassShadingFeaturesHUAWEI *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceSubpassShadingFeaturesHUAWEI)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, subpassShading, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceInvocationMaskFeaturesHUAWEI *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceInvocationMaskFeaturesHUAWEI)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, invocationMask, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceExternalMemoryRDMAFeaturesNV *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceExternalMemoryRDMAFeaturesNV)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, externalMemoryRDMA, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePipelinePropertiesFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDevicePipelinePropertiesFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, pipelinePropertiesIdentifier, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, multisampledRenderToSingleSampled, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceExtendedDynamicState2FeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceExtendedDynamicState2FeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, extendedDynamicState2, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, extendedDynamicState2LogicOp, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, extendedDynamicState2PatchControlPoints, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceColorWriteEnableFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceColorWriteEnableFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, colorWriteEnable, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, primitivesGeneratedQuery, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, primitivesGeneratedQueryWithRasterizerDiscard, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, primitivesGeneratedQueryWithNonZeroStreams, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, rayTracingMaintenance1, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, rayTracingPipelineTraceRaysIndirect2, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, globalPriorityQuery, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceImageViewMinLodFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceImageViewMinLodFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, minLod, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceMultiDrawPropertiesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceMultiDrawPropertiesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, maxMultiDrawCount, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceMultiDrawFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceMultiDrawFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, multiDraw, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceImage2DViewOf3DFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceImage2DViewOf3DFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, image2DViewOf3D, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, sampler2DViewOf3D, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceBorderColorSwizzleFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceBorderColorSwizzleFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, borderColorSwizzle, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, borderColorSwizzleFromImage, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, pageableDeviceLocalMemory, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceMaintenance4PropertiesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceMaintenance4PropertiesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, maxBufferSize, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceMaintenance4FeaturesKHR *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceMaintenance4FeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, maintenance4, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, descriptorSetHostMapping, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, nonSeamlessCubeMap, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentDensityMapOffsetPropertiesQCOM *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceFragmentDensityMapOffsetPropertiesQCOM)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, fragmentDensityOffsetGranularity, WarnIfLesser);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, fragmentDensityMapOffset, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceLinearColorAttachmentFeaturesNV *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceLinearColorAttachmentFeaturesNV)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, linearColorAttachment, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, imageCompressionControlSwapchain, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceImageProcessingPropertiesQCOM *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceImageProcessingPropertiesQCOM)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, maxWeightFilterPhases, WarnIfGreater);
        GET_VALUE_WARN(member, maxWeightFilterDimension, WarnIfGreater);
        GET_VALUE_WARN(member, maxBlockMatchRegion, WarnIfGreater);
        GET_VALUE_WARN(member, maxBoxFilterBlockSize, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceImageProcessingFeaturesQCOM *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceImageProcessingFeaturesQCOM)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, textureSampleWeighted, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, textureBoxFilter, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, textureBlockMatch, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, subpassMergeFeedback, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderModuleIdentifierPropertiesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceShaderModuleIdentifierPropertiesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        WarnNotModifiable("VkPhysicalDeviceShaderModuleIdentifierPropertiesEXT", member, "shaderModuleIdentifierAlgorithmUUID");
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderModuleIdentifier, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceTilePropertiesFeaturesQCOM *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceTilePropertiesFeaturesQCOM)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, tileProperties, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceAmigoProfilingFeaturesSEC *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceAmigoProfilingFeaturesSEC)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, amigoProfiling, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFeatures *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceFeatures)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, robustBufferAccess, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, fullDrawIndexUint32, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, imageCubeArray, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, independentBlend, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, geometryShader, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, tessellationShader, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, sampleRateShading, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, dualSrcBlend, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, logicOp, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, multiDrawIndirect, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, drawIndirectFirstInstance, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, depthClamp, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, depthBiasClamp, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, fillModeNonSolid, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, depthBounds, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, wideLines, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, largePoints, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, alphaToOne, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, multiViewport, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, samplerAnisotropy, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, textureCompressionETC2, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, textureCompressionASTC_LDR, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, textureCompressionBC, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, occlusionQueryPrecise, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, pipelineStatisticsQuery, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, vertexPipelineStoresAndAtomics, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, fragmentStoresAndAtomics, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderTessellationAndGeometryPointSize, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderImageGatherExtended, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderStorageImageExtendedFormats, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderStorageImageMultisample, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderStorageImageReadWithoutFormat, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderStorageImageWriteWithoutFormat, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderUniformBufferArrayDynamicIndexing, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderSampledImageArrayDynamicIndexing, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderStorageBufferArrayDynamicIndexing, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderStorageImageArrayDynamicIndexing, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderClipDistance, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderCullDistance, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderFloat64, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderInt64, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderInt16, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderResourceResidency, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, shaderResourceMinLod, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, sparseBinding, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, sparseResidencyBuffer, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, sparseResidencyImage2D, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, sparseResidencyImage3D, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, sparseResidency2Samples, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, sparseResidency4Samples, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, sparseResidency8Samples, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, sparseResidency16Samples, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, sparseResidencyAliased, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, variableMultisampleRate, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, inheritedQueries, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceProperties *dest) {
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceProperties)\n");
    bool valid = true;
    if (!GetValue(parent["limits"], &dest->limits)) {
        valid = false;
    }
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE(prop, apiVersion);
        GET_VALUE(prop, driverVersion);
        GET_VALUE(prop, vendorID);
        GET_VALUE(prop, deviceID);
        GET_VALUE_ENUM_WARN(prop, deviceType, WarnIfNotEqualEnum);
        GET_ARRAY(deviceName);         // size < VK_MAX_PHYSICAL_DEVICE_NAME_SIZE
        GET_ARRAY(pipelineCacheUUID);  // size == VK_UUID_SIZE*/
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceLimits *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceLimits)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, maxImageDimension1D, WarnIfGreater);
        GET_VALUE_WARN(member, maxImageDimension2D, WarnIfGreater);
        GET_VALUE_WARN(member, maxImageDimension3D, WarnIfGreater);
        GET_VALUE_WARN(member, maxImageDimensionCube, WarnIfGreater);
        GET_VALUE_WARN(member, maxImageArrayLayers, WarnIfGreater);
        GET_VALUE_WARN(member, maxTexelBufferElements, WarnIfGreater);
        GET_VALUE_WARN(member, maxUniformBufferRange, WarnIfGreater);
        GET_VALUE_WARN(member, maxStorageBufferRange, WarnIfGreater);
        GET_VALUE_WARN(member, maxPushConstantsSize, WarnIfGreater);
        GET_VALUE_WARN(member, maxMemoryAllocationCount, WarnIfGreater);
        GET_VALUE_WARN(member, maxSamplerAllocationCount, WarnIfGreater);
        GET_VALUE_WARN(member, bufferImageGranularity, WarnIfLesser);
        GET_VALUE_WARN(member, sparseAddressSpaceSize, WarnIfGreater);
        GET_VALUE_WARN(member, maxBoundDescriptorSets, WarnIfGreater);
        GET_VALUE_WARN(member, maxPerStageDescriptorSamplers, WarnIfGreater);
        GET_VALUE_WARN(member, maxPerStageDescriptorUniformBuffers, WarnIfGreater);
        GET_VALUE_WARN(member, maxPerStageDescriptorStorageBuffers, WarnIfGreater);
        GET_VALUE_WARN(member, maxPerStageDescriptorSampledImages, WarnIfGreater);
        GET_VALUE_WARN(member, maxPerStageDescriptorStorageImages, WarnIfGreater);
        GET_VALUE_WARN(member, maxPerStageDescriptorInputAttachments, WarnIfGreater);
        GET_VALUE_WARN(member, maxPerStageResources, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetSamplers, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetUniformBuffers, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetUniformBuffersDynamic, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetStorageBuffers, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetStorageBuffersDynamic, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetSampledImages, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetStorageImages, WarnIfGreater);
        GET_VALUE_WARN(member, maxDescriptorSetInputAttachments, WarnIfGreater);
        GET_VALUE_WARN(member, maxVertexInputAttributes, WarnIfGreater);
        GET_VALUE_WARN(member, maxVertexInputBindings, WarnIfGreater);
        GET_VALUE_WARN(member, maxVertexInputAttributeOffset, WarnIfGreater);
        GET_VALUE_WARN(member, maxVertexInputBindingStride, WarnIfGreater);
        GET_VALUE_WARN(member, maxVertexOutputComponents, WarnIfGreater);
        GET_VALUE_WARN(member, maxTessellationGenerationLevel, WarnIfGreater);
        GET_VALUE_WARN(member, maxTessellationPatchSize, WarnIfGreater);
        GET_VALUE_WARN(member, maxTessellationControlPerVertexInputComponents, WarnIfGreater);
        GET_VALUE_WARN(member, maxTessellationControlPerVertexOutputComponents, WarnIfGreater);
        GET_VALUE_WARN(member, maxTessellationControlPerPatchOutputComponents, WarnIfGreater);
        GET_VALUE_WARN(member, maxTessellationControlTotalOutputComponents, WarnIfGreater);
        GET_VALUE_WARN(member, maxTessellationEvaluationInputComponents, WarnIfGreater);
        GET_VALUE_WARN(member, maxTessellationEvaluationOutputComponents, WarnIfGreater);
        GET_VALUE_WARN(member, maxGeometryShaderInvocations, WarnIfGreater);
        GET_VALUE_WARN(member, maxGeometryInputComponents, WarnIfGreater);
        GET_VALUE_WARN(member, maxGeometryOutputComponents, WarnIfGreater);
        GET_VALUE_WARN(member, maxGeometryOutputVertices, WarnIfGreater);
        GET_VALUE_WARN(member, maxGeometryTotalOutputComponents, WarnIfGreater);
        GET_VALUE_WARN(member, maxFragmentInputComponents, WarnIfGreater);
        GET_VALUE_WARN(member, maxFragmentOutputAttachments, WarnIfGreater);
        GET_VALUE_WARN(member, maxFragmentDualSrcAttachments, WarnIfGreater);
        GET_VALUE_WARN(member, maxFragmentCombinedOutputResources, WarnIfGreater);
        GET_VALUE_WARN(member, maxComputeSharedMemorySize, WarnIfGreater);
        GET_ARRAY(maxComputeWorkGroupCount);
        GET_VALUE_WARN(member, maxComputeWorkGroupInvocations, WarnIfGreater);
        GET_ARRAY(maxComputeWorkGroupSize);
        GET_VALUE_WARN(member, subPixelPrecisionBits, WarnIfGreater);
        GET_VALUE_WARN(member, subTexelPrecisionBits, WarnIfGreater);
        GET_VALUE_WARN(member, mipmapPrecisionBits, WarnIfGreater);
        GET_VALUE_WARN(member, maxDrawIndexedIndexValue, WarnIfGreater);
        GET_VALUE_WARN(member, maxDrawIndirectCount, WarnIfGreater);
        GET_VALUE_WARN(member, maxSamplerLodBias, WarnIfGreaterFloat);
        GET_VALUE_WARN(member, maxSamplerAnisotropy, WarnIfGreaterFloat);
        GET_VALUE_WARN(member, maxViewports, WarnIfGreater);
        GET_ARRAY(maxViewportDimensions);
        GET_ARRAY(viewportBoundsRange);
        GET_VALUE_WARN(member, viewportSubPixelBits, WarnIfGreater);
        GET_VALUE_SIZE_T_WARN(member, minMemoryMapAlignment, WarnIfLesserSizet);
        GET_VALUE_WARN(member, minTexelBufferOffsetAlignment, WarnIfLesser);
        GET_VALUE_WARN(member, minUniformBufferOffsetAlignment, WarnIfLesser);
        GET_VALUE_WARN(member, minStorageBufferOffsetAlignment, WarnIfLesser);
        GET_VALUE_WARN(member, minTexelOffset, WarnIfLesser);
        GET_VALUE_WARN(member, maxTexelOffset, WarnIfGreater);
        GET_VALUE_WARN(member, minTexelGatherOffset, WarnIfLesser);
        GET_VALUE_WARN(member, maxTexelGatherOffset, WarnIfGreater);
        GET_VALUE_WARN(member, minInterpolationOffset, WarnIfLesserFloat);
        GET_VALUE_WARN(member, maxInterpolationOffset, WarnIfGreaterFloat);
        GET_VALUE_WARN(member, subPixelInterpolationOffsetBits, WarnIfGreater);
        GET_VALUE_WARN(member, maxFramebufferWidth, WarnIfGreater);
        GET_VALUE_WARN(member, maxFramebufferHeight, WarnIfGreater);
        GET_VALUE_WARN(member, maxFramebufferLayers, WarnIfGreater);
        GET_VALUE_FLAG_WARN(member, framebufferColorSampleCounts);
        GET_VALUE_FLAG_WARN(member, framebufferDepthSampleCounts);
        GET_VALUE_FLAG_WARN(member, framebufferStencilSampleCounts);
        GET_VALUE_FLAG_WARN(member, framebufferNoAttachmentsSampleCounts);
        GET_VALUE_WARN(member, maxColorAttachments, WarnIfGreater);
        GET_VALUE_FLAG_WARN(member, sampledImageColorSampleCounts);
        GET_VALUE_FLAG_WARN(member, sampledImageIntegerSampleCounts);
        GET_VALUE_FLAG_WARN(member, sampledImageDepthSampleCounts);
        GET_VALUE_FLAG_WARN(member, sampledImageStencilSampleCounts);
        GET_VALUE_FLAG_WARN(member, storageImageSampleCounts);
        GET_VALUE_WARN(member, maxSampleMaskWords, WarnIfGreater);
        GET_VALUE_WARN(member, timestampComputeAndGraphics, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, timestampPeriod, WarnIfLesserFloat);
        GET_VALUE_WARN(member, maxClipDistances, WarnIfGreater);
        GET_VALUE_WARN(member, maxCullDistances, WarnIfGreater);
        GET_VALUE_WARN(member, maxCombinedClipAndCullDistances, WarnIfGreater);
        GET_VALUE_WARN(member, discreteQueuePriorities, WarnIfGreater);
        GET_ARRAY(pointSizeRange);
        GET_ARRAY(lineWidthRange);
        GET_VALUE_WARN(member, pointSizeGranularity, WarnIfLesserFloat);
        GET_VALUE_WARN(member, lineWidthGranularity, WarnIfLesserFloat);
        WarnNotModifiable("VkPhysicalDeviceLimits", member, "strictLines");
        WarnNotModifiable("VkPhysicalDeviceLimits", member, "standardSampleLocations");
        GET_VALUE_WARN(member, optimalBufferCopyOffsetAlignment, WarnIfLesser);
        GET_VALUE_WARN(member, optimalBufferCopyRowPitchAlignment, WarnIfLesser);
        GET_VALUE_WARN(member, nonCoherentAtomSize, WarnIfLesser);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceSparseProperties *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceSparseProperties)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, residencyStandard2DBlockShape, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, residencyStandard2DMultisampleBlockShape, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, residencyStandard3DBlockShape, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, residencyAlignedMipSize, WarnIfNotEqualBool);
        GET_VALUE_WARN(member, residencyNonResidentStrict, WarnIfNotEqualBool);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceToolProperties *dest) {
    (void)dest;
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "\tJsonLoader::GetValue(VkPhysicalDeviceToolProperties)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        WarnNotModifiable("VkPhysicalDeviceToolProperties", member, "name");
        WarnNotModifiable("VkPhysicalDeviceToolProperties", member, "version");
        WarnNotModifiable("VkPhysicalDeviceToolProperties", member, "purposes");
        WarnNotModifiable("VkPhysicalDeviceToolProperties", member, "description");
        WarnNotModifiable("VkPhysicalDeviceToolProperties", member, "layer");
    }
    return valid;
}


#undef GET_VALUE
#undef GET_ARRAY

static DebugActionFlags GetDebugActionFlags(const vku::Strings &values) {
    DebugActionFlags result = 0;

    for (std::size_t i = 0, n = values.size(); i < n; ++i) {
        if (values[i] == "DEBUG_ACTION_FILE_BIT") {
            result |= DEBUG_ACTION_FILE_BIT;
        } else if (values[i] == "DEBUG_ACTION_STDOUT_BIT") {
            result |= DEBUG_ACTION_STDOUT_BIT;
        } else if (values[i] == "DEBUG_ACTION_OUTPUT_BIT") {
            result |= DEBUG_ACTION_OUTPUT_BIT;
        } else if (values[i] == "DEBUG_ACTION_BREAKPOINT_BIT") {
            result |= DEBUG_ACTION_BREAKPOINT_BIT;
        }
    }

    return result;
}

static std::string GetDebugActionsLog(DebugActionFlags flags) {
    std::string result = {};

    if (flags & DEBUG_ACTION_FILE_BIT) {
        result += "DEBUG_ACTION_FILE_BIT";
    }
    if (flags & DEBUG_ACTION_STDOUT_BIT) {
        if (!result.empty()) result += ", ";
        result += "DEBUG_ACTION_STDOUT_BIT";
    }
    if (flags & DEBUG_ACTION_OUTPUT_BIT) {
        if (!result.empty()) result += ", ";
        result += "DEBUG_ACTION_OUTPUT_BIT";
    }
    if (flags & DEBUG_ACTION_BREAKPOINT_BIT) {
        if (!result.empty()) result += ", ";
        result += "DEBUG_ACTION_BREAKPOINT_BIT";
    }

    return result;
}

static DebugReportFlags GetDebugReportFlags(const vku::Strings &values) {
    DebugReportFlags result = 0;

    for (std::size_t i = 0, n = values.size(); i < n; ++i) {
        if (values[i] == "DEBUG_REPORT_NOTIFICATION_BIT") {
            result |= DEBUG_REPORT_NOTIFICATION_BIT;
        } else if (values[i] == "DEBUG_REPORT_WARNING_BIT") {
            result |= DEBUG_REPORT_WARNING_BIT;
        } else if (values[i] == "DEBUG_REPORT_ERROR_BIT") {
            result |= DEBUG_REPORT_ERROR_BIT;
        } else if (values[i] == "DEBUG_REPORT_DEBUG_BIT") {
            result |= DEBUG_REPORT_DEBUG_BIT;
        }
    }

    return result;
}

std::string GetString(const vku::List &list) {
    std::string result;
    for (std::size_t i = 0, n = list.size(); i < n; ++i) {
        result += list[i].first;
        if (i < n - 1) result += ", ";
    }
    return result;
}

std::string GetString(const vku::Strings &strings) {
    std::string result;
    for (std::size_t i = 0, n = strings.size(); i < n; ++i) {
        result += strings[i];
        if (i < n - 1) result += ", ";
    }
    return result;
}

const VkProfileLayerSettingsEXT *FindSettingsInChain(const void *next) {
    const VkBaseOutStructure *current = reinterpret_cast<const VkBaseOutStructure *>(next);
    const VkProfileLayerSettingsEXT *found = nullptr;
    while (current) {
        if (VK_STRUCTURE_TYPE_PROFILES_LAYER_SETTINGS_EXT == static_cast<uint32_t>(current->sType)) {
            found = reinterpret_cast<const VkProfileLayerSettingsEXT *>(current);
            current = nullptr;
        } else {
            current = current->pNext;
        }
    }
    return found;
}

static void InitSettings(const void *pnext) {
    const VkProfileLayerSettingsEXT *user_settings = FindSettingsInChain(pnext);
    // Programmatically-specified settings override ENV vars or layer settings file settings
    if (pnext && user_settings) {
        *layer_settings = *user_settings;
    } else {
        if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsProfileFile)) {
            layer_settings->profile_file = vku::GetLayerSettingString(kOurLayerName, kLayerSettingsProfileFile);
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsProfileName)) {
            layer_settings->profile_name = vku::GetLayerSettingString(kOurLayerName, kLayerSettingsProfileName);
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsProfileValidation)) {
            layer_settings->profile_validation = vku::GetLayerSettingBool(kOurLayerName, kLayerSettingsProfileValidation);
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsEmulatePortability)) {
            layer_settings->emulate_portability = vku::GetLayerSettingBool(kOurLayerName, kLayerSettingsEmulatePortability);
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsSimulateCapabilities)) {
            layer_settings->simulate_capabilities =
                GetSimulateCapabilityFlags(vku::GetLayerSettingStrings(kOurLayerName, kLayerSettingsSimulateCapabilities));
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsDebugFailOnError)) {
            layer_settings->debug_fail_on_error = vku::GetLayerSettingBool(kOurLayerName, kLayerSettingsDebugFailOnError);
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsExcludeDeviceExtensions)) {
            layer_settings->exclude_device_extensions =
                vku::GetLayerSettingStrings(kOurLayerName, kLayerSettingsExcludeDeviceExtensions);
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsExcludeFormats)) {
            layer_settings->exclude_formats = vku::GetLayerSettingStrings(kOurLayerName, kLayerSettingsExcludeFormats);
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsDebugActions)) {
            layer_settings->debug_actions =
                GetDebugActionFlags(vku::GetLayerSettingStrings(kOurLayerName, kLayerSettingsDebugActions));
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsDebugFilename)) {
            layer_settings->debug_filename = vku::GetLayerSettingString(kOurLayerName, kLayerSettingsDebugFilename);
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsDebugFileClear)) {
            layer_settings->debug_file_discard = vku::GetLayerSettingBool(kOurLayerName, kLayerSettingsDebugFileClear);
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsDebugReports)) {
            layer_settings->debug_reports =
                GetDebugReportFlags(vku::GetLayerSettingStrings(kOurLayerName, kLayerSettingsDebugReports));
        }
    }

    if (layer_settings->debug_actions & DEBUG_ACTION_FILE_BIT && profiles_log_file == nullptr) {
        profiles_log_file = fopen(layer_settings->debug_filename.c_str(), layer_settings->debug_file_discard ? "w" : "w+");
        if (profiles_log_file == nullptr) {
            layer_settings->debug_actions &= ~DEBUG_ACTION_FILE_BIT;
            layer_settings->debug_actions |= DEBUG_ACTION_STDOUT_BIT;
            LogMessage(DEBUG_REPORT_ERROR_BIT, format("Could not open %s, log to file is being overridden by log to stdout.\n",
                                                      layer_settings->debug_filename.c_str()));
        } else {
            LogMessage(DEBUG_REPORT_DEBUG_BIT, format("Log file %s opened\n", layer_settings->debug_filename.c_str()));
        }
    } else {
        LogMessage(DEBUG_REPORT_DEBUG_BIT, format("No need to open the log file %s\n", layer_settings->debug_filename.c_str()));
    }

    const std::string simulation_capabilities_log = GetSimulateCapabilitiesLog(layer_settings->simulate_capabilities);
    const std::string debug_actions_log = GetDebugActionsLog(layer_settings->debug_actions);
    const std::string debug_reports_log = GetDebugReportsLog(layer_settings->debug_reports);

    std::string settings_log;
    if (user_settings) {
        settings_log += format(
            "NOTE: Settings originate from a user-supplied settings structure: environment variables and "
            "layer settings file were ignored.\n");
    }
    settings_log += format("\t%s: %s\n", kLayerSettingsProfileFile, layer_settings->profile_file.c_str());
    settings_log += format("\t%s: %s\n", kLayerSettingsProfileName, layer_settings->profile_name.c_str());
    settings_log += format("\t%s: %s\n", kLayerSettingsProfileValidation, layer_settings->profile_validation ? "true" : "false");
    settings_log += format("\t%s: %s\n", kLayerSettingsEmulatePortability, layer_settings->emulate_portability ? "true" : "false");
    settings_log += format("\t%s: %s\n", kLayerSettingsSimulateCapabilities, simulation_capabilities_log.c_str());
    settings_log += format("\t%s: %s\n", kLayerSettingsDebugActions, debug_actions_log.c_str());
    settings_log += format("\t%s: %s\n", kLayerSettingsDebugFilename, layer_settings->debug_filename.c_str());
    settings_log += format("\t%s: %s\n", kLayerSettingsDebugFileClear, layer_settings->debug_file_discard ? "true" : "false");
    settings_log += format("\t%s: %s\n", kLayerSettingsDebugFailOnError, layer_settings->debug_fail_on_error ? "true" : "false");
    settings_log += format("\t%s: %s\n", kLayerSettingsDebugReports, debug_reports_log.c_str());
    settings_log +=
        format("\t%s: %s\n", kLayerSettingsExcludeDeviceExtensions, GetString(layer_settings->exclude_device_extensions).c_str());
    settings_log += format("\t%s: %s\n", kLayerSettingsExcludeFormats, GetString(layer_settings->exclude_formats).c_str());

    LogMessage(DEBUG_REPORT_NOTIFICATION_BIT, format("Profile Layers Settings: {\n%s}\n", settings_log.c_str()));
}

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
    // This needs to be created before LogMessage is called, as it is dependent on these settings
    if (layer_settings) {
        delete layer_settings;
    }
    layer_settings = new VkProfileLayerSettingsEXT{};

    LogMessage(DEBUG_REPORT_DEBUG_BIT, "CreateInstance\n");
    LogMessage(DEBUG_REPORT_DEBUG_BIT, ::format("JsonCpp version %s\n", JSONCPP_VERSION_STRING));
    LogMessage(DEBUG_REPORT_NOTIFICATION_BIT, ::format("%s version %d.%d.%d\n", kOurLayerName, kVersionProfilesMajor,
                                                       kVersionProfilesMinor, kVersionProfilesPatch));

    InitSettings(pCreateInfo->pNext);

    JsonLoader &json_loader = JsonLoader::Create();
    VkResult result = json_loader.LoadFile(layer_settings->profile_file);
    if (result != VK_SUCCESS) {
        return result;
    }

    const VkApplicationInfo *app_info = pCreateInfo->pApplicationInfo;
    requested_version = (app_info && app_info->apiVersion) ? app_info->apiVersion : VK_API_VERSION_1_0;
    if (VK_API_VERSION_MAJOR(requested_version) > VK_API_VERSION_MAJOR(VK_HEADER_VERSION_COMPLETE) ||
        VK_API_VERSION_MINOR(requested_version) > VK_API_VERSION_MINOR(VK_HEADER_VERSION_COMPLETE)) {
        LogMessage(DEBUG_REPORT_ERROR_BIT, ::format("The Vulkan application requested a Vulkan %s instance but the %s was build "
                                                    "against %s. Please, update the layer.\n",
                                                    StringAPIVersion(requested_version).c_str(), kOurLayerName,
                                                    StringAPIVersion(VK_HEADER_VERSION_COMPLETE).c_str()));
        if (layer_settings->debug_fail_on_error) {
            return VK_ERROR_INITIALIZATION_FAILED;
        }
    }

    bool changed_version = false;
    if (!layer_settings->profile_file.empty()) {
        const uint32_t profile_api_version = json_loader.GetProfileApiVersion();
        if (VK_API_VERSION_MAJOR(requested_version) < VK_API_VERSION_MAJOR(profile_api_version) ||
            VK_API_VERSION_MINOR(requested_version) < VK_API_VERSION_MINOR(profile_api_version)) {
            if (layer_settings->simulate_capabilities & SIMULATE_API_VERSION_BIT) {
                if (layer_settings->profile_name.empty()) {
                    LogMessage(
                        DEBUG_REPORT_NOTIFICATION_BIT,
                        ::format("The Vulkan application requested a Vulkan %s instance but the selected %s file requires %s. The "
                                 "application requested instance version is overridden to %s.\n",
                                 StringAPIVersion(requested_version).c_str(), layer_settings->profile_file.c_str(),
                                 StringAPIVersion(profile_api_version).c_str(), StringAPIVersion(profile_api_version).c_str()));
                } else {
                    LogMessage(
                        DEBUG_REPORT_NOTIFICATION_BIT,
                        ::format("The Vulkan application requested a Vulkan %s instance but the selected %s profile requires %s. "
                                 "The application requested instance version is overridden to %s.\n",
                                 StringAPIVersion(requested_version).c_str(), layer_settings->profile_name.c_str(),
                                 StringAPIVersion(profile_api_version).c_str(), StringAPIVersion(profile_api_version).c_str()));
                }
                requested_version = profile_api_version;
                changed_version = true;
            } else {
                if (layer_settings->profile_name.empty()) {
                    LogMessage(
                        DEBUG_REPORT_WARNING_BIT,
                        ::format("The Vulkan application requested a Vulkan %s instance but the selected %s file requires %s. The "
                                 "profile may not be initialized correctly which will produce unexpected warning messages.\n",
                                 StringAPIVersion(requested_version).c_str(), layer_settings->profile_file.c_str(),
                                 StringAPIVersion(profile_api_version).c_str()));
                } else {
                    LogMessage(
                        DEBUG_REPORT_WARNING_BIT,
                        ::format("The Vulkan application requested a Vulkan %s instance but the selected %s profile requires %s. "
                                 "The profile may not be initialized correctly which will produce unexpected warning messages.\n",
                                 StringAPIVersion(requested_version).c_str(), layer_settings->profile_name.c_str(),
                                 StringAPIVersion(profile_api_version).c_str()));
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
        LogMessage(DEBUG_REPORT_NOTIFICATION_BIT,
                   ::format("The Profiles Layer requires the %s extension, but it was not included in "
                            "VkInstanceCreateInfo::ppEnabledExtensionNames, adding the extension.\n",
                            VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME));
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
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "DestroyInstance\n");

    if (instance) {
        std::lock_guard<std::recursive_mutex> lock(global_lock);

        {
            const auto dt = instance_dispatch_table(instance);

            std::vector<VkPhysicalDevice> physical_devices;
            VkResult err = EnumerateAll<VkPhysicalDevice>(&physical_devices, [&](uint32_t *count, VkPhysicalDevice *results) {
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

    if (layer_settings->debug_actions & DEBUG_ACTION_FILE_BIT) {
        LogMessage(DEBUG_REPORT_DEBUG_BIT, format("Closing log file %s, bye!\n", layer_settings->debug_filename.c_str()));
        fclose(profiles_log_file);
        profiles_log_file = nullptr;
    }

    delete layer_settings;
    layer_settings = nullptr;
}

void FillPNextChain(PhysicalDeviceData *physicalDeviceData, void *place) {
    while (place) {
        VkBaseOutStructure *structure = (VkBaseOutStructure *)place;

        // These switch statements check which struct is in the pNext chain and, if the physical device has the proper extension,
        // fill the struct with any override data provided by the PhysicalDeviceData object.

        switch (structure->sType) {
            // VK_KHR_portability_subset is a special case since it can also be emulated by the Profiles layer.
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME) ||
                    layer_settings->emulate_portability) {
                    VkPhysicalDevicePortabilitySubsetPropertiesKHR *psp = (VkPhysicalDevicePortabilitySubsetPropertiesKHR *)place;
                    void *pNext = psp->pNext;
                    *psp = physicalDeviceData->physical_device_portability_subset_properties_;
                    psp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES:
                if (physicalDeviceData->GetEffectiveVersion() >= VK_API_VERSION_1_1) {
                    VkPhysicalDeviceSubgroupProperties *data = (VkPhysicalDeviceSubgroupProperties *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_subgroup_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_PROPERTIES:
                if (physicalDeviceData->GetEffectiveVersion() >= VK_API_VERSION_1_1) {
                    VkPhysicalDeviceProtectedMemoryProperties *data = (VkPhysicalDeviceProtectedMemoryProperties *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_protected_memory_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES:
                if (physicalDeviceData->GetEffectiveVersion() >= VK_API_VERSION_1_2) {
                    VkPhysicalDeviceVulkan11Properties *data = (VkPhysicalDeviceVulkan11Properties *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_vulkan_11_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES:
                if (physicalDeviceData->GetEffectiveVersion() >= VK_API_VERSION_1_2) {
                    VkPhysicalDeviceVulkan12Properties *data = (VkPhysicalDeviceVulkan12Properties *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_vulkan_12_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES:
                if (physicalDeviceData->GetEffectiveVersion() >= VK_API_VERSION_1_3) {
                    VkPhysicalDeviceVulkan13Properties *data = (VkPhysicalDeviceVulkan13Properties *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_vulkan_13_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES:
                if (physicalDeviceData->GetEffectiveVersion() >= VK_API_VERSION_1_1) {
                    VkPhysicalDeviceProtectedMemoryFeatures *data = (VkPhysicalDeviceProtectedMemoryFeatures *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_protected_memory_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETER_FEATURES:
                if (physicalDeviceData->GetEffectiveVersion() >= VK_API_VERSION_1_1) {
                    VkPhysicalDeviceShaderDrawParameterFeatures *data = (VkPhysicalDeviceShaderDrawParameterFeatures *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_shader_draw_parameter_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES:
                if (physicalDeviceData->GetEffectiveVersion() >= VK_API_VERSION_1_2) {
                    VkPhysicalDeviceVulkan11Features *data = (VkPhysicalDeviceVulkan11Features *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_vulkan_11_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES:
                if (physicalDeviceData->GetEffectiveVersion() >= VK_API_VERSION_1_2) {
                    VkPhysicalDeviceVulkan12Features *data = (VkPhysicalDeviceVulkan12Features *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_vulkan_12_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES:
                if (physicalDeviceData->GetEffectiveVersion() >= VK_API_VERSION_1_3) {
                    VkPhysicalDeviceVulkan13Features *data = (VkPhysicalDeviceVulkan13Features *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_vulkan_13_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME)) {
                    VkPhysicalDeviceTransformFeedbackPropertiesEXT *data = (VkPhysicalDeviceTransformFeedbackPropertiesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_transform_feedback_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME)) {
                    VkPhysicalDeviceTransformFeedbackFeaturesEXT *data = (VkPhysicalDeviceTransformFeedbackFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_transform_feedback_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME)) {
                    VkPhysicalDeviceDynamicRenderingFeaturesKHR *data = (VkPhysicalDeviceDynamicRenderingFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_dynamic_rendering_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CORNER_SAMPLED_IMAGE_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_CORNER_SAMPLED_IMAGE_EXTENSION_NAME)) {
                    VkPhysicalDeviceCornerSampledImageFeaturesNV *data = (VkPhysicalDeviceCornerSampledImageFeaturesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_corner_sampled_image_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_MULTIVIEW_EXTENSION_NAME)) {
                    VkPhysicalDeviceMultiviewPropertiesKHR *data = (VkPhysicalDeviceMultiviewPropertiesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_multiview_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_MULTIVIEW_EXTENSION_NAME)) {
                    VkPhysicalDeviceMultiviewFeaturesKHR *data = (VkPhysicalDeviceMultiviewFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_multiview_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_HDR_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_TEXTURE_COMPRESSION_ASTC_HDR_EXTENSION_NAME)) {
                    VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT *data = (VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_texture_compression_astchdrfeatures_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ASTC_DECODE_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_ASTC_DECODE_MODE_EXTENSION_NAME)) {
                    VkPhysicalDeviceASTCDecodeFeaturesEXT *data = (VkPhysicalDeviceASTCDecodeFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_astcdecode_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_ROBUSTNESS_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_PIPELINE_ROBUSTNESS_EXTENSION_NAME)) {
                    VkPhysicalDevicePipelineRobustnessPropertiesEXT *data = (VkPhysicalDevicePipelineRobustnessPropertiesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_pipeline_robustness_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_ROBUSTNESS_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_PIPELINE_ROBUSTNESS_EXTENSION_NAME)) {
                    VkPhysicalDevicePipelineRobustnessFeaturesEXT *data = (VkPhysicalDevicePipelineRobustnessFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_pipeline_robustness_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME) && PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME) && PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME)) {
                    VkPhysicalDeviceIDPropertiesKHR *data = (VkPhysicalDeviceIDPropertiesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_idproperties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME)) {
                    VkPhysicalDevicePushDescriptorPropertiesKHR *data = (VkPhysicalDevicePushDescriptorPropertiesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_push_descriptor_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONDITIONAL_RENDERING_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_CONDITIONAL_RENDERING_EXTENSION_NAME)) {
                    VkPhysicalDeviceConditionalRenderingFeaturesEXT *data = (VkPhysicalDeviceConditionalRenderingFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_conditional_rendering_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT16_INT8_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME)) {
                    VkPhysicalDeviceFloat16Int8FeaturesKHR *data = (VkPhysicalDeviceFloat16Int8FeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_float_16_int_8_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_16BIT_STORAGE_EXTENSION_NAME)) {
                    VkPhysicalDevice16BitStorageFeaturesKHR *data = (VkPhysicalDevice16BitStorageFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_16_bit_storage_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PER_VIEW_ATTRIBUTES_PROPERTIES_NVX:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NVX_MULTIVIEW_PER_VIEW_ATTRIBUTES_EXTENSION_NAME)) {
                    VkPhysicalDeviceMultiviewPerViewAttributesPropertiesNVX *data = (VkPhysicalDeviceMultiviewPerViewAttributesPropertiesNVX *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_multiview_per_view_attributes_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DISCARD_RECTANGLE_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_DISCARD_RECTANGLES_EXTENSION_NAME)) {
                    VkPhysicalDeviceDiscardRectanglePropertiesEXT *data = (VkPhysicalDeviceDiscardRectanglePropertiesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_discard_rectangle_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONSERVATIVE_RASTERIZATION_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME)) {
                    VkPhysicalDeviceConservativeRasterizationPropertiesEXT *data = (VkPhysicalDeviceConservativeRasterizationPropertiesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_conservative_rasterization_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_DEPTH_CLIP_ENABLE_EXTENSION_NAME)) {
                    VkPhysicalDeviceDepthClipEnableFeaturesEXT *data = (VkPhysicalDeviceDepthClipEnableFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_depth_clip_enable_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME)) {
                    VkPhysicalDeviceImagelessFramebufferFeaturesKHR *data = (VkPhysicalDeviceImagelessFramebufferFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_imageless_framebuffer_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_PERFORMANCE_QUERY_EXTENSION_NAME)) {
                    VkPhysicalDevicePerformanceQueryPropertiesKHR *data = (VkPhysicalDevicePerformanceQueryPropertiesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_performance_query_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_PERFORMANCE_QUERY_EXTENSION_NAME)) {
                    VkPhysicalDevicePerformanceQueryFeaturesKHR *data = (VkPhysicalDevicePerformanceQueryFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_performance_query_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_POINT_CLIPPING_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_MAINTENANCE_2_EXTENSION_NAME)) {
                    VkPhysicalDevicePointClippingPropertiesKHR *data = (VkPhysicalDevicePointClippingPropertiesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_point_clipping_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME)) {
                    VkPhysicalDeviceVariablePointerFeaturesKHR *data = (VkPhysicalDeviceVariablePointerFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_variable_pointer_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_FILTER_MINMAX_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME)) {
                    VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT *data = (VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_sampler_filter_minmax_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME)) {
                    VkPhysicalDeviceInlineUniformBlockPropertiesEXT *data = (VkPhysicalDeviceInlineUniformBlockPropertiesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_inline_uniform_block_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME)) {
                    VkPhysicalDeviceInlineUniformBlockFeaturesEXT *data = (VkPhysicalDeviceInlineUniformBlockFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_inline_uniform_block_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLE_LOCATIONS_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_SAMPLE_LOCATIONS_EXTENSION_NAME)) {
                    VkPhysicalDeviceSampleLocationsPropertiesEXT *data = (VkPhysicalDeviceSampleLocationsPropertiesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_sample_locations_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME)) {
                    VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT *data = (VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_blend_operation_advanced_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME)) {
                    VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT *data = (VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_blend_operation_advanced_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME)) {
                    VkPhysicalDeviceAccelerationStructurePropertiesKHR *data = (VkPhysicalDeviceAccelerationStructurePropertiesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_acceleration_structure_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME)) {
                    VkPhysicalDeviceAccelerationStructureFeaturesKHR *data = (VkPhysicalDeviceAccelerationStructureFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_acceleration_structure_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME)) {
                    VkPhysicalDeviceRayTracingPipelinePropertiesKHR *data = (VkPhysicalDeviceRayTracingPipelinePropertiesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_ray_tracing_pipeline_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME)) {
                    VkPhysicalDeviceRayTracingPipelineFeaturesKHR *data = (VkPhysicalDeviceRayTracingPipelineFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_ray_tracing_pipeline_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_RAY_QUERY_EXTENSION_NAME)) {
                    VkPhysicalDeviceRayQueryFeaturesKHR *data = (VkPhysicalDeviceRayQueryFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_ray_query_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_PROPERTIES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_SHADER_SM_BUILTINS_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderSMBuiltinsPropertiesNV *data = (VkPhysicalDeviceShaderSMBuiltinsPropertiesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_shader_smbuiltins_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_SHADER_SM_BUILTINS_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderSMBuiltinsFeaturesNV *data = (VkPhysicalDeviceShaderSMBuiltinsFeaturesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_shader_smbuiltins_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME)) {
                    VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR *data = (VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_sampler_ycbcr_conversion_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)) {
                    VkPhysicalDeviceDescriptorIndexingPropertiesEXT *data = (VkPhysicalDeviceDescriptorIndexingPropertiesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_descriptor_indexing_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)) {
                    VkPhysicalDeviceDescriptorIndexingFeaturesEXT *data = (VkPhysicalDeviceDescriptorIndexingFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_descriptor_indexing_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_PROPERTIES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_SHADING_RATE_IMAGE_EXTENSION_NAME)) {
                    VkPhysicalDeviceShadingRateImagePropertiesNV *data = (VkPhysicalDeviceShadingRateImagePropertiesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_shading_rate_image_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_SHADING_RATE_IMAGE_EXTENSION_NAME)) {
                    VkPhysicalDeviceShadingRateImageFeaturesNV *data = (VkPhysicalDeviceShadingRateImageFeaturesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_shading_rate_image_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_RAY_TRACING_EXTENSION_NAME)) {
                    VkPhysicalDeviceRayTracingPropertiesNV *data = (VkPhysicalDeviceRayTracingPropertiesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_ray_tracing_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_REPRESENTATIVE_FRAGMENT_TEST_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_REPRESENTATIVE_FRAGMENT_TEST_EXTENSION_NAME)) {
                    VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV *data = (VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_representative_fragment_test_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_MAINTENANCE_3_EXTENSION_NAME)) {
                    VkPhysicalDeviceMaintenance3PropertiesKHR *data = (VkPhysicalDeviceMaintenance3PropertiesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_maintenance_3_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_SHADER_SUBGROUP_EXTENDED_TYPES_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR *data = (VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_shader_subgroup_extended_types_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_8BIT_STORAGE_EXTENSION_NAME)) {
                    VkPhysicalDevice8BitStorageFeaturesKHR *data = (VkPhysicalDevice8BitStorageFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_8_bit_storage_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_HOST_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_EXTERNAL_MEMORY_HOST_EXTENSION_NAME)) {
                    VkPhysicalDeviceExternalMemoryHostPropertiesEXT *data = (VkPhysicalDeviceExternalMemoryHostPropertiesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_external_memory_host_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_SHADER_ATOMIC_INT64_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderAtomicInt64FeaturesKHR *data = (VkPhysicalDeviceShaderAtomicInt64FeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_shader_atomic_int_64_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_SHADER_CLOCK_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderClockFeaturesKHR *data = (VkPhysicalDeviceShaderClockFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_shader_clock_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_AMD:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_AMD_SHADER_CORE_PROPERTIES_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderCorePropertiesAMD *data = (VkPhysicalDeviceShaderCorePropertiesAMD *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_shader_core_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME)) {
                    VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT *data = (VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_vertex_attribute_divisor_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME)) {
                    VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT *data = (VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_vertex_attribute_divisor_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME)) {
                    VkPhysicalDeviceDriverPropertiesKHR *data = (VkPhysicalDeviceDriverPropertiesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_driver_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT_CONTROLS_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME)) {
                    VkPhysicalDeviceFloatControlsPropertiesKHR *data = (VkPhysicalDeviceFloatControlsPropertiesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_float_controls_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME)) {
                    VkPhysicalDeviceDepthStencilResolvePropertiesKHR *data = (VkPhysicalDeviceDepthStencilResolvePropertiesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_depth_stencil_resolve_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMPUTE_SHADER_DERIVATIVES_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_COMPUTE_SHADER_DERIVATIVES_EXTENSION_NAME)) {
                    VkPhysicalDeviceComputeShaderDerivativesFeaturesNV *data = (VkPhysicalDeviceComputeShaderDerivativesFeaturesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_compute_shader_derivatives_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_MESH_SHADER_EXTENSION_NAME)) {
                    VkPhysicalDeviceMeshShaderPropertiesNV *data = (VkPhysicalDeviceMeshShaderPropertiesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_mesh_shader_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_MESH_SHADER_EXTENSION_NAME)) {
                    VkPhysicalDeviceMeshShaderFeaturesNV *data = (VkPhysicalDeviceMeshShaderFeaturesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_mesh_shader_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_BARYCENTRIC_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_FRAGMENT_SHADER_BARYCENTRIC_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV *data = (VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_fragment_shader_barycentric_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_FOOTPRINT_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_SHADER_IMAGE_FOOTPRINT_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderImageFootprintFeaturesNV *data = (VkPhysicalDeviceShaderImageFootprintFeaturesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_shader_image_footprint_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXCLUSIVE_SCISSOR_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_SCISSOR_EXCLUSIVE_EXTENSION_NAME)) {
                    VkPhysicalDeviceExclusiveScissorFeaturesNV *data = (VkPhysicalDeviceExclusiveScissorFeaturesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_exclusive_scissor_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME)) {
                    VkPhysicalDeviceTimelineSemaphorePropertiesKHR *data = (VkPhysicalDeviceTimelineSemaphorePropertiesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_timeline_semaphore_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME)) {
                    VkPhysicalDeviceTimelineSemaphoreFeaturesKHR *data = (VkPhysicalDeviceTimelineSemaphoreFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_timeline_semaphore_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_FUNCTIONS_2_FEATURES_INTEL:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_INTEL_SHADER_INTEGER_FUNCTIONS_2_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL *data = (VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_shader_integer_functions_2_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_VULKAN_MEMORY_MODEL_EXTENSION_NAME)) {
                    VkPhysicalDeviceVulkanMemoryModelFeaturesKHR *data = (VkPhysicalDeviceVulkanMemoryModelFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_vulkan_memory_model_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PCI_BUS_INFO_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_PCI_BUS_INFO_EXTENSION_NAME)) {
                    VkPhysicalDevicePCIBusInfoPropertiesEXT *data = (VkPhysicalDevicePCIBusInfoPropertiesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_pcibus_info_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_SHADER_TERMINATE_INVOCATION_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR *data = (VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_shader_terminate_invocation_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_FRAGMENT_DENSITY_MAP_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentDensityMapPropertiesEXT *data = (VkPhysicalDeviceFragmentDensityMapPropertiesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_fragment_density_map_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_FRAGMENT_DENSITY_MAP_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentDensityMapFeaturesEXT *data = (VkPhysicalDeviceFragmentDensityMapFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_fragment_density_map_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME)) {
                    VkPhysicalDeviceScalarBlockLayoutFeaturesEXT *data = (VkPhysicalDeviceScalarBlockLayoutFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_scalar_block_layout_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME)) {
                    VkPhysicalDeviceSubgroupSizeControlPropertiesEXT *data = (VkPhysicalDeviceSubgroupSizeControlPropertiesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_subgroup_size_control_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME)) {
                    VkPhysicalDeviceSubgroupSizeControlFeaturesEXT *data = (VkPhysicalDeviceSubgroupSizeControlFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_subgroup_size_control_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentShadingRatePropertiesKHR *data = (VkPhysicalDeviceFragmentShadingRatePropertiesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_fragment_shading_rate_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentShadingRateFeaturesKHR *data = (VkPhysicalDeviceFragmentShadingRateFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_fragment_shading_rate_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_2_AMD:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_AMD_SHADER_CORE_PROPERTIES_2_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderCoreProperties2AMD *data = (VkPhysicalDeviceShaderCoreProperties2AMD *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_shader_core_properties_2_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COHERENT_MEMORY_FEATURES_AMD:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME)) {
                    VkPhysicalDeviceCoherentMemoryFeaturesAMD *data = (VkPhysicalDeviceCoherentMemoryFeaturesAMD *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_coherent_memory_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_ATOMIC_INT64_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_SHADER_IMAGE_ATOMIC_INT64_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT *data = (VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_shader_image_atomic_int_64_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PRIORITY_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME)) {
                    VkPhysicalDeviceMemoryPriorityFeaturesEXT *data = (VkPhysicalDeviceMemoryPriorityFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_memory_priority_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEDICATED_ALLOCATION_IMAGE_ALIASING_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_DEDICATED_ALLOCATION_IMAGE_ALIASING_EXTENSION_NAME)) {
                    VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV *data = (VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_dedicated_allocation_image_aliasing_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_SEPARATE_DEPTH_STENCIL_LAYOUTS_EXTENSION_NAME)) {
                    VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR *data = (VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_separate_depth_stencil_layouts_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_ADDRESS_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME)) {
                    VkPhysicalDeviceBufferAddressFeaturesEXT *data = (VkPhysicalDeviceBufferAddressFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_buffer_address_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_WAIT_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_PRESENT_WAIT_EXTENSION_NAME)) {
                    VkPhysicalDevicePresentWaitFeaturesKHR *data = (VkPhysicalDevicePresentWaitFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_present_wait_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_PROPERTIES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_COOPERATIVE_MATRIX_EXTENSION_NAME)) {
                    VkPhysicalDeviceCooperativeMatrixPropertiesNV *data = (VkPhysicalDeviceCooperativeMatrixPropertiesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_cooperative_matrix_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_COOPERATIVE_MATRIX_EXTENSION_NAME)) {
                    VkPhysicalDeviceCooperativeMatrixFeaturesNV *data = (VkPhysicalDeviceCooperativeMatrixFeaturesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_cooperative_matrix_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COVERAGE_REDUCTION_MODE_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_COVERAGE_REDUCTION_MODE_EXTENSION_NAME)) {
                    VkPhysicalDeviceCoverageReductionModeFeaturesNV *data = (VkPhysicalDeviceCoverageReductionModeFeaturesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_coverage_reduction_mode_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT *data = (VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_fragment_shader_interlock_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_IMAGE_ARRAYS_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_YCBCR_IMAGE_ARRAYS_EXTENSION_NAME)) {
                    VkPhysicalDeviceYcbcrImageArraysFeaturesEXT *data = (VkPhysicalDeviceYcbcrImageArraysFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_ycbcr_image_arrays_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT_EXTENSION_NAME)) {
                    VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR *data = (VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_uniform_buffer_standard_layout_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_PROVOKING_VERTEX_EXTENSION_NAME)) {
                    VkPhysicalDeviceProvokingVertexPropertiesEXT *data = (VkPhysicalDeviceProvokingVertexPropertiesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_provoking_vertex_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_PROVOKING_VERTEX_EXTENSION_NAME)) {
                    VkPhysicalDeviceProvokingVertexFeaturesEXT *data = (VkPhysicalDeviceProvokingVertexFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_provoking_vertex_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME)) {
                    VkPhysicalDeviceBufferDeviceAddressFeaturesKHR *data = (VkPhysicalDeviceBufferDeviceAddressFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_buffer_device_address_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME)) {
                    VkPhysicalDeviceLineRasterizationPropertiesEXT *data = (VkPhysicalDeviceLineRasterizationPropertiesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_line_rasterization_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME)) {
                    VkPhysicalDeviceLineRasterizationFeaturesEXT *data = (VkPhysicalDeviceLineRasterizationFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_line_rasterization_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderAtomicFloatFeaturesEXT *data = (VkPhysicalDeviceShaderAtomicFloatFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_shader_atomic_float_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME)) {
                    VkPhysicalDeviceHostQueryResetFeaturesEXT *data = (VkPhysicalDeviceHostQueryResetFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_host_query_reset_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_INDEX_TYPE_UINT8_EXTENSION_NAME)) {
                    VkPhysicalDeviceIndexTypeUint8FeaturesEXT *data = (VkPhysicalDeviceIndexTypeUint8FeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_index_type_uint_8_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME)) {
                    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT *data = (VkPhysicalDeviceExtendedDynamicStateFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_extended_dynamic_state_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_EXECUTABLE_PROPERTIES_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_PIPELINE_EXECUTABLE_PROPERTIES_EXTENSION_NAME)) {
                    VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR *data = (VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_pipeline_executable_properties_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_2_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_SHADER_ATOMIC_FLOAT_2_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT *data = (VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_shader_atomic_float_2_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT *data = (VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_shader_demote_to_helper_invocation_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_PROPERTIES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_DEVICE_GENERATED_COMMANDS_EXTENSION_NAME)) {
                    VkPhysicalDeviceDeviceGeneratedCommandsPropertiesNV *data = (VkPhysicalDeviceDeviceGeneratedCommandsPropertiesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_device_generated_commands_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_DEVICE_GENERATED_COMMANDS_EXTENSION_NAME)) {
                    VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV *data = (VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_device_generated_commands_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INHERITED_VIEWPORT_SCISSOR_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_INHERITED_VIEWPORT_SCISSOR_EXTENSION_NAME)) {
                    VkPhysicalDeviceInheritedViewportScissorFeaturesNV *data = (VkPhysicalDeviceInheritedViewportScissorFeaturesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_inherited_viewport_scissor_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_SHADER_INTEGER_DOT_PRODUCT_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR *data = (VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_shader_integer_dot_product_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_SHADER_INTEGER_DOT_PRODUCT_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR *data = (VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_shader_integer_dot_product_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME)) {
                    VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT *data = (VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_texel_buffer_alignment_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME)) {
                    VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT *data = (VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_texel_buffer_alignment_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_MEMORY_REPORT_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_DEVICE_MEMORY_REPORT_EXTENSION_NAME)) {
                    VkPhysicalDeviceDeviceMemoryReportFeaturesEXT *data = (VkPhysicalDeviceDeviceMemoryReportFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_device_memory_report_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_ROBUSTNESS_2_EXTENSION_NAME)) {
                    VkPhysicalDeviceRobustness2PropertiesEXT *data = (VkPhysicalDeviceRobustness2PropertiesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_robustness_2_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_ROBUSTNESS_2_EXTENSION_NAME)) {
                    VkPhysicalDeviceRobustness2FeaturesEXT *data = (VkPhysicalDeviceRobustness2FeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_robustness_2_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_CUSTOM_BORDER_COLOR_EXTENSION_NAME)) {
                    VkPhysicalDeviceCustomBorderColorPropertiesEXT *data = (VkPhysicalDeviceCustomBorderColorPropertiesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_custom_border_color_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_CUSTOM_BORDER_COLOR_EXTENSION_NAME)) {
                    VkPhysicalDeviceCustomBorderColorFeaturesEXT *data = (VkPhysicalDeviceCustomBorderColorFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_custom_border_color_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_ID_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_PRESENT_ID_EXTENSION_NAME)) {
                    VkPhysicalDevicePresentIdFeaturesKHR *data = (VkPhysicalDevicePresentIdFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_present_id_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_PRIVATE_DATA_EXTENSION_NAME)) {
                    VkPhysicalDevicePrivateDataFeaturesEXT *data = (VkPhysicalDevicePrivateDataFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_private_data_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_PIPELINE_CREATION_CACHE_CONTROL_EXTENSION_NAME)) {
                    VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT *data = (VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_pipeline_creation_cache_control_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DIAGNOSTICS_CONFIG_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_DEVICE_DIAGNOSTICS_CONFIG_EXTENSION_NAME)) {
                    VkPhysicalDeviceDiagnosticsConfigFeaturesNV *data = (VkPhysicalDeviceDiagnosticsConfigFeaturesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_diagnostics_config_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME)) {
                    VkPhysicalDeviceSynchronization2FeaturesKHR *data = (VkPhysicalDeviceSynchronization2FeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_synchronization_2_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GRAPHICS_PIPELINE_LIBRARY_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_GRAPHICS_PIPELINE_LIBRARY_EXTENSION_NAME)) {
                    VkPhysicalDeviceGraphicsPipelineLibraryPropertiesEXT *data = (VkPhysicalDeviceGraphicsPipelineLibraryPropertiesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_graphics_pipeline_library_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GRAPHICS_PIPELINE_LIBRARY_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_GRAPHICS_PIPELINE_LIBRARY_EXTENSION_NAME)) {
                    VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT *data = (VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_graphics_pipeline_library_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_EARLY_AND_LATE_FRAGMENT_TESTS_FEATURES_AMD:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_AMD_SHADER_EARLY_AND_LATE_FRAGMENT_TESTS_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD *data = (VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_shader_early_and_late_fragment_tests_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_BARYCENTRIC_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_FRAGMENT_SHADER_BARYCENTRIC_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentShaderBarycentricPropertiesKHR *data = (VkPhysicalDeviceFragmentShaderBarycentricPropertiesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_fragment_shader_barycentric_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR *data = (VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_shader_subgroup_uniform_control_flow_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_WORKGROUP_MEMORY_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_ZERO_INITIALIZE_WORKGROUP_MEMORY_EXTENSION_NAME)) {
                    VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR *data = (VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_zero_initialize_workgroup_memory_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_ENUMS_PROPERTIES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_FRAGMENT_SHADING_RATE_ENUMS_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentShadingRateEnumsPropertiesNV *data = (VkPhysicalDeviceFragmentShadingRateEnumsPropertiesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_fragment_shading_rate_enums_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_ENUMS_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_FRAGMENT_SHADING_RATE_ENUMS_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV *data = (VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_fragment_shading_rate_enums_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_MOTION_BLUR_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_RAY_TRACING_MOTION_BLUR_EXTENSION_NAME)) {
                    VkPhysicalDeviceRayTracingMotionBlurFeaturesNV *data = (VkPhysicalDeviceRayTracingMotionBlurFeaturesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_ray_tracing_motion_blur_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_2_PLANE_444_FORMATS_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_YCBCR_2PLANE_444_FORMATS_EXTENSION_NAME)) {
                    VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT *data = (VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_ycbcr_2_plane_444_formats_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_FRAGMENT_DENSITY_MAP_2_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentDensityMap2PropertiesEXT *data = (VkPhysicalDeviceFragmentDensityMap2PropertiesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_fragment_density_map_2_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_FRAGMENT_DENSITY_MAP_2_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentDensityMap2FeaturesEXT *data = (VkPhysicalDeviceFragmentDensityMap2FeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_fragment_density_map_2_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_IMAGE_ROBUSTNESS_EXTENSION_NAME)) {
                    VkPhysicalDeviceImageRobustnessFeaturesEXT *data = (VkPhysicalDeviceImageRobustnessFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_image_robustness_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_EXTENSION_NAME)) {
                    VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR *data = (VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_workgroup_memory_explicit_layout_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_COMPRESSION_CONTROL_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_IMAGE_COMPRESSION_CONTROL_EXTENSION_NAME)) {
                    VkPhysicalDeviceImageCompressionControlFeaturesEXT *data = (VkPhysicalDeviceImageCompressionControlFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_image_compression_control_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ATTACHMENT_FEEDBACK_LOOP_LAYOUT_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_ATTACHMENT_FEEDBACK_LOOP_LAYOUT_EXTENSION_NAME)) {
                    VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT *data = (VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_attachment_feedback_loop_layout_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_4444_FORMATS_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_4444_FORMATS_EXTENSION_NAME)) {
                    VkPhysicalDevice4444FormatsFeaturesEXT *data = (VkPhysicalDevice4444FormatsFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_4444_formats_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RGBA10X6_FORMATS_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_RGBA10X6_FORMATS_EXTENSION_NAME)) {
                    VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT *data = (VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_rgba_10_x_6_formats_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MUTABLE_DESCRIPTOR_TYPE_FEATURES_VALVE:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_VALVE_MUTABLE_DESCRIPTOR_TYPE_EXTENSION_NAME)) {
                    VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE *data = (VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_mutable_descriptor_type_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_INPUT_DYNAMIC_STATE_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME)) {
                    VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT *data = (VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_vertex_input_dynamic_state_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRM_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_PHYSICAL_DEVICE_DRM_EXTENSION_NAME)) {
                    VkPhysicalDeviceDrmPropertiesEXT *data = (VkPhysicalDeviceDrmPropertiesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_drm_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_CONTROL_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_DEPTH_CLIP_CONTROL_EXTENSION_NAME)) {
                    VkPhysicalDeviceDepthClipControlFeaturesEXT *data = (VkPhysicalDeviceDepthClipControlFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_depth_clip_control_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVE_TOPOLOGY_LIST_RESTART_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_PRIMITIVE_TOPOLOGY_LIST_RESTART_EXTENSION_NAME)) {
                    VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT *data = (VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_primitive_topology_list_restart_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_SHADING_PROPERTIES_HUAWEI:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_HUAWEI_SUBPASS_SHADING_EXTENSION_NAME)) {
                    VkPhysicalDeviceSubpassShadingPropertiesHUAWEI *data = (VkPhysicalDeviceSubpassShadingPropertiesHUAWEI *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_subpass_shading_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_SHADING_FEATURES_HUAWEI:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_HUAWEI_SUBPASS_SHADING_EXTENSION_NAME)) {
                    VkPhysicalDeviceSubpassShadingFeaturesHUAWEI *data = (VkPhysicalDeviceSubpassShadingFeaturesHUAWEI *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_subpass_shading_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INVOCATION_MASK_FEATURES_HUAWEI:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_HUAWEI_INVOCATION_MASK_EXTENSION_NAME)) {
                    VkPhysicalDeviceInvocationMaskFeaturesHUAWEI *data = (VkPhysicalDeviceInvocationMaskFeaturesHUAWEI *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_invocation_mask_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_RDMA_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_EXTERNAL_MEMORY_RDMA_EXTENSION_NAME)) {
                    VkPhysicalDeviceExternalMemoryRDMAFeaturesNV *data = (VkPhysicalDeviceExternalMemoryRDMAFeaturesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_external_memory_rdmafeatures_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_PROPERTIES_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME)) {
                    VkPhysicalDevicePipelinePropertiesFeaturesEXT *data = (VkPhysicalDevicePipelinePropertiesFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_pipeline_properties_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTISAMPLED_RENDER_TO_SINGLE_SAMPLED_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_MULTISAMPLED_RENDER_TO_SINGLE_SAMPLED_EXTENSION_NAME)) {
                    VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT *data = (VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_multisampled_render_to_single_sampled_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME)) {
                    VkPhysicalDeviceExtendedDynamicState2FeaturesEXT *data = (VkPhysicalDeviceExtendedDynamicState2FeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_extended_dynamic_state_2_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COLOR_WRITE_ENABLE_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_COLOR_WRITE_ENABLE_EXTENSION_NAME)) {
                    VkPhysicalDeviceColorWriteEnableFeaturesEXT *data = (VkPhysicalDeviceColorWriteEnableFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_color_write_enable_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVES_GENERATED_QUERY_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_PRIMITIVES_GENERATED_QUERY_EXTENSION_NAME)) {
                    VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT *data = (VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_primitives_generated_query_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_MAINTENANCE_1_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_RAY_TRACING_MAINTENANCE_1_EXTENSION_NAME)) {
                    VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR *data = (VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_ray_tracing_maintenance_1_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GLOBAL_PRIORITY_QUERY_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_GLOBAL_PRIORITY_QUERY_EXTENSION_NAME)) {
                    VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT *data = (VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_global_priority_query_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_VIEW_MIN_LOD_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_IMAGE_VIEW_MIN_LOD_EXTENSION_NAME)) {
                    VkPhysicalDeviceImageViewMinLodFeaturesEXT *data = (VkPhysicalDeviceImageViewMinLodFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_image_view_min_lod_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_MULTI_DRAW_EXTENSION_NAME)) {
                    VkPhysicalDeviceMultiDrawPropertiesEXT *data = (VkPhysicalDeviceMultiDrawPropertiesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_multi_draw_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_MULTI_DRAW_EXTENSION_NAME)) {
                    VkPhysicalDeviceMultiDrawFeaturesEXT *data = (VkPhysicalDeviceMultiDrawFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_multi_draw_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_2D_VIEW_OF_3D_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_IMAGE_2D_VIEW_OF_3D_EXTENSION_NAME)) {
                    VkPhysicalDeviceImage2DViewOf3DFeaturesEXT *data = (VkPhysicalDeviceImage2DViewOf3DFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_image_2_dview_of_3_dfeatures_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BORDER_COLOR_SWIZZLE_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_BORDER_COLOR_SWIZZLE_EXTENSION_NAME)) {
                    VkPhysicalDeviceBorderColorSwizzleFeaturesEXT *data = (VkPhysicalDeviceBorderColorSwizzleFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_border_color_swizzle_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PAGEABLE_DEVICE_LOCAL_MEMORY_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_PAGEABLE_DEVICE_LOCAL_MEMORY_EXTENSION_NAME)) {
                    VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT *data = (VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_pageable_device_local_memory_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_MAINTENANCE_4_EXTENSION_NAME)) {
                    VkPhysicalDeviceMaintenance4PropertiesKHR *data = (VkPhysicalDeviceMaintenance4PropertiesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_maintenance_4_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_MAINTENANCE_4_EXTENSION_NAME)) {
                    VkPhysicalDeviceMaintenance4FeaturesKHR *data = (VkPhysicalDeviceMaintenance4FeaturesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_maintenance_4_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_SET_HOST_MAPPING_FEATURES_VALVE:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_VALVE_DESCRIPTOR_SET_HOST_MAPPING_EXTENSION_NAME)) {
                    VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE *data = (VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_descriptor_set_host_mapping_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_NON_SEAMLESS_CUBE_MAP_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_NON_SEAMLESS_CUBE_MAP_EXTENSION_NAME)) {
                    VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT *data = (VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_non_seamless_cube_map_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_OFFSET_PROPERTIES_QCOM:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_QCOM_FRAGMENT_DENSITY_MAP_OFFSET_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentDensityMapOffsetPropertiesQCOM *data = (VkPhysicalDeviceFragmentDensityMapOffsetPropertiesQCOM *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_fragment_density_map_offset_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_OFFSET_FEATURES_QCOM:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_QCOM_FRAGMENT_DENSITY_MAP_OFFSET_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM *data = (VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_fragment_density_map_offset_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINEAR_COLOR_ATTACHMENT_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_LINEAR_COLOR_ATTACHMENT_EXTENSION_NAME)) {
                    VkPhysicalDeviceLinearColorAttachmentFeaturesNV *data = (VkPhysicalDeviceLinearColorAttachmentFeaturesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_linear_color_attachment_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_COMPRESSION_CONTROL_SWAPCHAIN_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_IMAGE_COMPRESSION_CONTROL_SWAPCHAIN_EXTENSION_NAME)) {
                    VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT *data = (VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_image_compression_control_swapchain_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_PROCESSING_PROPERTIES_QCOM:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_QCOM_IMAGE_PROCESSING_EXTENSION_NAME)) {
                    VkPhysicalDeviceImageProcessingPropertiesQCOM *data = (VkPhysicalDeviceImageProcessingPropertiesQCOM *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_image_processing_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_PROCESSING_FEATURES_QCOM:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_QCOM_IMAGE_PROCESSING_EXTENSION_NAME)) {
                    VkPhysicalDeviceImageProcessingFeaturesQCOM *data = (VkPhysicalDeviceImageProcessingFeaturesQCOM *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_image_processing_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_MERGE_FEEDBACK_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_SUBPASS_MERGE_FEEDBACK_EXTENSION_NAME)) {
                    VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT *data = (VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_subpass_merge_feedback_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MODULE_IDENTIFIER_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_SHADER_MODULE_IDENTIFIER_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderModuleIdentifierPropertiesEXT *data = (VkPhysicalDeviceShaderModuleIdentifierPropertiesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_shader_module_identifier_properties_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MODULE_IDENTIFIER_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_SHADER_MODULE_IDENTIFIER_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT *data = (VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_shader_module_identifier_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TILE_PROPERTIES_FEATURES_QCOM:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_QCOM_TILE_PROPERTIES_EXTENSION_NAME)) {
                    VkPhysicalDeviceTilePropertiesFeaturesQCOM *data = (VkPhysicalDeviceTilePropertiesFeaturesQCOM *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_tile_properties_features_;
                    data->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_AMIGO_PROFILING_FEATURES_SEC:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_SEC_AMIGO_PROFILING_EXTENSION_NAME)) {
                    VkPhysicalDeviceAmigoProfilingFeaturesSEC *data = (VkPhysicalDeviceAmigoProfilingFeaturesSEC *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->physical_device_amigo_profiling_features_;
                    data->pNext = pNext;
                }
                break;
            default:
                break;
        }

        place = structure->pNext;
    }
}

void FillQueueFamilyPropertiesPNextChain(PhysicalDeviceData *physicalDeviceData, VkQueueFamilyProperties2KHR *pQueueFamilyProperties2, uint32_t count) {
    for (uint32_t i = 0; i < count; ++i) {
        void* place = pQueueFamilyProperties2[i].pNext;
        while (place) {
            VkBaseOutStructure *structure = (VkBaseOutStructure *)place;

            switch (structure->sType) {
                case VK_STRUCTURE_TYPE_QUEUE_FAMILY_GLOBAL_PRIORITY_PROPERTIES_KHR: {
                    VkQueueFamilyGlobalPriorityPropertiesKHR *data = (VkQueueFamilyGlobalPriorityPropertiesKHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->arrayof_queue_family_properties_[i].global_priority_properties_;
                    data->pNext = pNext;
                } break;
                case VK_STRUCTURE_TYPE_QUEUE_FAMILY_CHECKPOINT_PROPERTIES_NV: {
                    VkQueueFamilyCheckpointPropertiesNV *data = (VkQueueFamilyCheckpointPropertiesNV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->arrayof_queue_family_properties_[i].checkpoint_properties_;
                    data->pNext = pNext;
                } break;
                case VK_STRUCTURE_TYPE_QUEUE_FAMILY_CHECKPOINT_PROPERTIES_2_NV: {
                    VkQueueFamilyCheckpointProperties2NV *data = (VkQueueFamilyCheckpointProperties2NV *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->arrayof_queue_family_properties_[i].checkpoint_properties_2_;
                    data->pNext = pNext;
                } break;
                case VK_STRUCTURE_TYPE_VIDEO_QUEUE_FAMILY_PROPERTIES_2_KHR: {
                    VkVideoQueueFamilyProperties2KHR *data = (VkVideoQueueFamilyProperties2KHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->arrayof_queue_family_properties_[i].video_properties_2_;
                    data->pNext = pNext;
                } break;
                case VK_STRUCTURE_TYPE_QUEUE_FAMILY_QUERY_RESULT_STATUS_PROPERTIES_2_KHR: {
                    VkQueueFamilyQueryResultStatusProperties2KHR *data = (VkQueueFamilyQueryResultStatusProperties2KHR *)place;
                    void *pNext = data->pNext;
                    *data = physicalDeviceData->arrayof_queue_family_properties_[i].query_result_status_properties_2_;
                    data->pNext = pNext;
                } break;
                default:
                    break;
            }

            place = structure->pNext;
        }
    }
}

void FillFormatPropertiesPNextChain(PhysicalDeviceData *physicalDeviceData, void *place, VkFormat format) {
    while (place) {
        VkBaseOutStructure *structure = (VkBaseOutStructure *)place;

        // These switch statements check which struct is in the pNext chain and, if the physical device has the proper extension,
        // fill the struct with any override data provided by the PhysicalDeviceData object.

        switch (structure->sType) {
            case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3: {
                if (!physicalDeviceData->arrayof_format_properties_3_.empty()) {
                    VkFormatProperties3 *sp = (VkFormatProperties3 *)place;
                    void *pNext = sp->pNext;
                    *sp = physicalDeviceData->arrayof_format_properties_3_[format];
                    sp->pNext = pNext;
                }
            } break;
            default:
                break;
        }

        place = structure->pNext;
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties *pProperties) {
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "GetPhysicalDeviceProperties\n");
    LogFlush();

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
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "GetPhysicalDeviceFeatures\n");
    LogFlush();

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
    dt->GetPhysicalDeviceFeatures2(physicalDevice, pFeatures);
    GetPhysicalDeviceFeatures(physicalDevice, &pFeatures->features);
    PhysicalDeviceData *pdd = PhysicalDeviceData::Find(physicalDevice);
    FillPNextChain(pdd, pFeatures->pNext);
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFeatures2KHR(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures2KHR *pFeatures) {
    GetPhysicalDeviceFeatures2(physicalDevice, pFeatures);
}

template <typename T>
VkResult EnumerateProperties(uint32_t src_count, const T *src_props, uint32_t *dst_count, T *dst_props) {
    assert(dst_count);
    if (!dst_props || !src_props) {
        *dst_count = src_count;
        return VK_SUCCESS;
    }

    const uint32_t copy_count = (*dst_count < src_count) ? *dst_count : src_count;
    memcpy(dst_props, src_props, copy_count * sizeof(T));
    *dst_count = copy_count;
    return (copy_count == src_count) ? VK_SUCCESS : VK_INCOMPLETE;
}

VKAPI_ATTR VkResult VKAPI_CALL EnumerateInstanceLayerProperties(uint32_t *pCount, VkLayerProperties *pProperties) {
    LogMessage(DEBUG_REPORT_DEBUG_BIT, format("vkEnumerateInstanceLayerProperties %s \n", pProperties ? "VALUES" : "COUNT"));
    LogFlush();

    return EnumerateProperties(kLayerPropertiesCount, kLayerProperties, pCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL EnumerateInstanceExtensionProperties(const char *pLayerName, uint32_t *pCount,
                                                                    VkExtensionProperties *pProperties) {
    LogMessage(DEBUG_REPORT_DEBUG_BIT, format("vkEnumerateInstanceExtensionProperties \"%s\" %s \n", (pLayerName ? pLayerName : ""),
                                              (pProperties ? "VALUES" : "COUNT")));
    LogFlush();

    if (pLayerName && !strcmp(pLayerName, kOurLayerName)) {
        return EnumerateProperties(kInstanceExtensionPropertiesCount, kInstanceExtensionProperties.data(), pCount, pProperties);
    }
    return VK_ERROR_LAYER_NOT_PRESENT;
}

// Per [LALI], EnumerateDeviceLayerProperties() is deprecated.

VKAPI_ATTR VkResult VKAPI_CALL EnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice, const char *pLayerName,
                                                                  uint32_t *pCount, VkExtensionProperties *pProperties) {
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "EnumerateDeviceExtensionProperties\n");
    LogFlush();

    VkResult result = VK_SUCCESS;
    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(physicalDevice);

    uint32_t pCount_copy = *pCount;

    PhysicalDeviceData *pdd = PhysicalDeviceData::Find(physicalDevice);
    const uint32_t src_count = pdd ? static_cast<uint32_t>(pdd->simulation_extensions_.size()) : 0;
    if (pLayerName) {
        if (strcmp(pLayerName, kOurLayerName) == 0)
            result = EnumerateProperties(kDeviceExtensionPropertiesCount, kDeviceExtensionProperties.data(), pCount, pProperties);
        else
            result = dt->EnumerateDeviceExtensionProperties(physicalDevice, pLayerName, pCount, pProperties);
    //} else if (src_count == 0 || (!(layer_settings->simulate_capabilities & SIMULATE_EXTENSIONS_BIT) &&
    //                              layer_settings->exclude_device_extensions.empty())) {
    } else if (pdd == nullptr || (!(layer_settings->simulate_capabilities & SIMULATE_EXTENSIONS_BIT) &&
                                  layer_settings->exclude_device_extensions.empty())) {
        result = dt->EnumerateDeviceExtensionProperties(physicalDevice, pLayerName, pCount, pProperties);
    } else {
        result = EnumerateProperties(static_cast<uint32_t>(pdd->simulation_extensions_.size()), pdd->simulation_extensions_.data(), pCount, pProperties);
    }

    if (result == VK_SUCCESS && !pLayerName && layer_settings->emulate_portability &&
        !PhysicalDeviceData::HasSimulatedOrRealExtension(physicalDevice, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME)) {
        if (pProperties) {
            if (pCount_copy == *pCount + 1) {
                *pCount = pCount_copy;
            }
            strncpy(pProperties[(*pCount) - 1].extensionName, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME, VK_MAX_EXTENSION_NAME_SIZE);
            pProperties[(*pCount) - 1].specVersion = VK_KHR_PORTABILITY_SUBSET_SPEC_VERSION;
        } else {
            *pCount += 1;
        }
    }

    return result;
}

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
        dt->GetPhysicalDeviceQueueFamilyProperties2KHR(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties2);
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

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format,
                                                             VkFormatProperties *pFormatProperties) {
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "GetPhysicalDeviceFormatProperties\n");

    // Check if Format was excluded
    for (std::size_t j = 0, m = layer_settings->exclude_formats.size(); j < m; ++j) {
        const std::string &excluded_format = layer_settings->exclude_formats[j];
        if (excluded_format.empty()) continue;

        if (StringToFormat(excluded_format) == format) {
            *pFormatProperties = VkFormatProperties{};
            return;
        }
    }

    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(physicalDevice);

    // Are there JSON overrides, or should we call down to return the original values?
    PhysicalDeviceData *pdd = PhysicalDeviceData::Find(physicalDevice);
    const uint32_t src_count = (pdd) ? static_cast<uint32_t>(pdd->arrayof_format_properties_.size()) : 0;
    if (src_count == 0) {
        dt->GetPhysicalDeviceFormatProperties(physicalDevice, format, pFormatProperties);
    } else {
        VkFormatProperties device_format = {};
        dt->GetPhysicalDeviceFormatProperties(physicalDevice, format, &device_format);
        const auto iter = pdd->arrayof_format_properties_.find(format);

        if ((layer_settings->simulate_capabilities & SIMULATE_FORMATS_BIT)) {
            *pFormatProperties = (iter != pdd->arrayof_format_properties_.end()) ? iter->second : VkFormatProperties{};
        } else {
            *pFormatProperties = device_format;
        }

        if (IsFormatSupported(*pFormatProperties) && iter != pdd->arrayof_format_properties_.end()) {
            if ((layer_settings->simulate_capabilities & SIMULATE_FORMATS_BIT)) {
                *pFormatProperties = iter->second;
            } else {
                *pFormatProperties = device_format;
            }

            if (!HasFlags(pFormatProperties->linearTilingFeatures, device_format.linearTilingFeatures) ||
                !HasFlags(pFormatProperties->optimalTilingFeatures, device_format.optimalTilingFeatures) ||
                !HasFlags(pFormatProperties->bufferFeatures, device_format.bufferFeatures)) {
                LogMessage(DEBUG_REPORT_WARNING_BIT,
                           ::format("format %s is simulating unsupported features!\n", vkFormatToString(format).c_str()));
            }
        }
    }

    LogFlush();
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFormatProperties2(VkPhysicalDevice physicalDevice, VkFormat format,
                                                              VkFormatProperties2KHR *pFormatProperties) {
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "GetPhysicalDeviceFormatProperties2\n");
    LogFlush();

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
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "GetPhysicalDeviceImageFormatProperties\n");
    LogFlush();

    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(physicalDevice);

    // Are there JSON overrides, or should we call down to return the original values?
    if (!(layer_settings->simulate_capabilities & SIMULATE_FORMATS_BIT)) {
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
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "GetPhysicalDeviceImageFormatProperties2KHR\n");
    LogFlush();

    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(physicalDevice);
    dt->GetPhysicalDeviceImageFormatProperties2KHR(physicalDevice, pImageFormatInfo, pImageFormatProperties);
    return GetPhysicalDeviceImageFormatProperties(physicalDevice, pImageFormatInfo->format, pImageFormatInfo->type,
                                                  pImageFormatInfo->tiling, pImageFormatInfo->usage, pImageFormatInfo->flags,
                                                  &pImageFormatProperties->imageFormatProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceImageFormatProperties2(VkPhysicalDevice physicalDevice,
                                                                       const VkPhysicalDeviceImageFormatInfo2 *pImageFormatInfo,
                                                                       VkImageFormatProperties2 *pImageFormatProperties) {
    return GetPhysicalDeviceImageFormatProperties2KHR(physicalDevice, pImageFormatInfo, pImageFormatProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceToolPropertiesEXT(VkPhysicalDevice physicalDevice, uint32_t *pToolCount,
                                                                  VkPhysicalDeviceToolPropertiesEXT *pToolProperties) {
    LogMessage(DEBUG_REPORT_DEBUG_BIT, "GetPhysicalDeviceToolPropertiesEXT\n");
    LogFlush();

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

    VkLayerInstanceDispatchTable *pInstanceTable = instance_dispatch_table(physicalDevice);
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

#define TRANSFER_VALUE(name)    \
    if (promoted_written) {     \
        std::memmove(&src->name, &dest->name, sizeof(src->name)); \
    } else {                    \
        std::memmove(&dest->name, &src->name, sizeof(dest->name)); \
    }

#define TRANSFER_VALUE_ARRAY(name)    \
    if (promoted_written) {     \
        std::memmove(src->name, dest->name, sizeof(src->name)); \
    } else {                    \
        std::memmove(dest->name, src->name, sizeof(dest->name)); \
    }


// VK_VULKAN_1_1

void TransferValue(VkPhysicalDeviceVulkan11Properties *dest, VkPhysicalDeviceMultiviewPropertiesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(maxMultiviewViewCount);
    TRANSFER_VALUE(maxMultiviewInstanceIndex);
}

void TransferValue(VkPhysicalDeviceVulkan11Features *dest, VkPhysicalDeviceMultiviewFeaturesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(multiview);
    TRANSFER_VALUE(multiviewGeometryShader);
    TRANSFER_VALUE(multiviewTessellationShader);
}

void TransferValue(VkPhysicalDeviceVulkan11Properties *dest, VkPhysicalDeviceIDPropertiesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(deviceUUID);
    TRANSFER_VALUE(driverUUID);
    TRANSFER_VALUE(deviceLUID);
    TRANSFER_VALUE(deviceNodeMask);
    TRANSFER_VALUE(deviceLUIDValid);
}

void TransferValue(VkPhysicalDeviceVulkan11Features *dest, VkPhysicalDevice16BitStorageFeaturesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(storageBuffer16BitAccess);
    TRANSFER_VALUE(uniformAndStorageBuffer16BitAccess);
    TRANSFER_VALUE(storagePushConstant16);
    TRANSFER_VALUE(storageInputOutput16);
}

void TransferValue(VkPhysicalDeviceVulkan11Properties *dest, VkPhysicalDevicePointClippingPropertiesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(pointClippingBehavior);
}

void TransferValue(VkPhysicalDeviceVulkan11Features *dest, VkPhysicalDeviceVariablePointerFeaturesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(variablePointersStorageBuffer);
    TRANSFER_VALUE(variablePointers);
}

void TransferValue(VkPhysicalDeviceVulkan11Features *dest, VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(samplerYcbcrConversion);
}

void TransferValue(VkPhysicalDeviceVulkan11Properties *dest, VkPhysicalDeviceMaintenance3PropertiesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(maxPerSetDescriptors);
    TRANSFER_VALUE(maxMemoryAllocationSize);
}


// VK_VULKAN_1_2

void TransferValue(VkPhysicalDeviceVulkan12Features *dest, VkPhysicalDeviceFloat16Int8FeaturesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(shaderFloat16);
    TRANSFER_VALUE(shaderInt8);
}

void TransferValue(VkPhysicalDeviceVulkan12Features *dest, VkPhysicalDeviceImagelessFramebufferFeaturesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(imagelessFramebuffer);
}

void TransferValue(VkPhysicalDeviceVulkan12Properties *dest, VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT *src, bool promoted_written) {
    TRANSFER_VALUE(filterMinmaxSingleComponentFormats);
    TRANSFER_VALUE(filterMinmaxImageComponentMapping);
}

void TransferValue(VkPhysicalDeviceVulkan12Properties *dest, VkPhysicalDeviceDescriptorIndexingPropertiesEXT *src, bool promoted_written) {
    TRANSFER_VALUE(maxUpdateAfterBindDescriptorsInAllPools);
    TRANSFER_VALUE(shaderUniformBufferArrayNonUniformIndexingNative);
    TRANSFER_VALUE(shaderSampledImageArrayNonUniformIndexingNative);
    TRANSFER_VALUE(shaderStorageBufferArrayNonUniformIndexingNative);
    TRANSFER_VALUE(shaderStorageImageArrayNonUniformIndexingNative);
    TRANSFER_VALUE(shaderInputAttachmentArrayNonUniformIndexingNative);
    TRANSFER_VALUE(robustBufferAccessUpdateAfterBind);
    TRANSFER_VALUE(quadDivergentImplicitLod);
    TRANSFER_VALUE(maxPerStageDescriptorUpdateAfterBindSamplers);
    TRANSFER_VALUE(maxPerStageDescriptorUpdateAfterBindUniformBuffers);
    TRANSFER_VALUE(maxPerStageDescriptorUpdateAfterBindStorageBuffers);
    TRANSFER_VALUE(maxPerStageDescriptorUpdateAfterBindSampledImages);
    TRANSFER_VALUE(maxPerStageDescriptorUpdateAfterBindStorageImages);
    TRANSFER_VALUE(maxPerStageDescriptorUpdateAfterBindInputAttachments);
    TRANSFER_VALUE(maxPerStageUpdateAfterBindResources);
    TRANSFER_VALUE(maxDescriptorSetUpdateAfterBindSamplers);
    TRANSFER_VALUE(maxDescriptorSetUpdateAfterBindUniformBuffers);
    TRANSFER_VALUE(maxDescriptorSetUpdateAfterBindUniformBuffersDynamic);
    TRANSFER_VALUE(maxDescriptorSetUpdateAfterBindStorageBuffers);
    TRANSFER_VALUE(maxDescriptorSetUpdateAfterBindStorageBuffersDynamic);
    TRANSFER_VALUE(maxDescriptorSetUpdateAfterBindSampledImages);
    TRANSFER_VALUE(maxDescriptorSetUpdateAfterBindStorageImages);
    TRANSFER_VALUE(maxDescriptorSetUpdateAfterBindInputAttachments);
}

void TransferValue(VkPhysicalDeviceVulkan12Features *dest, VkPhysicalDeviceDescriptorIndexingFeaturesEXT *src, bool promoted_written) {
    TRANSFER_VALUE(shaderInputAttachmentArrayDynamicIndexing);
    TRANSFER_VALUE(shaderUniformTexelBufferArrayDynamicIndexing);
    TRANSFER_VALUE(shaderStorageTexelBufferArrayDynamicIndexing);
    TRANSFER_VALUE(shaderUniformBufferArrayNonUniformIndexing);
    TRANSFER_VALUE(shaderSampledImageArrayNonUniformIndexing);
    TRANSFER_VALUE(shaderStorageBufferArrayNonUniformIndexing);
    TRANSFER_VALUE(shaderStorageImageArrayNonUniformIndexing);
    TRANSFER_VALUE(shaderInputAttachmentArrayNonUniformIndexing);
    TRANSFER_VALUE(shaderUniformTexelBufferArrayNonUniformIndexing);
    TRANSFER_VALUE(shaderStorageTexelBufferArrayNonUniformIndexing);
    TRANSFER_VALUE(descriptorBindingUniformBufferUpdateAfterBind);
    TRANSFER_VALUE(descriptorBindingSampledImageUpdateAfterBind);
    TRANSFER_VALUE(descriptorBindingStorageImageUpdateAfterBind);
    TRANSFER_VALUE(descriptorBindingStorageBufferUpdateAfterBind);
    TRANSFER_VALUE(descriptorBindingUniformTexelBufferUpdateAfterBind);
    TRANSFER_VALUE(descriptorBindingStorageTexelBufferUpdateAfterBind);
    TRANSFER_VALUE(descriptorBindingUpdateUnusedWhilePending);
    TRANSFER_VALUE(descriptorBindingPartiallyBound);
    TRANSFER_VALUE(descriptorBindingVariableDescriptorCount);
    TRANSFER_VALUE(runtimeDescriptorArray);
}

void TransferValue(VkPhysicalDeviceVulkan12Features *dest, VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(shaderSubgroupExtendedTypes);
}

void TransferValue(VkPhysicalDeviceVulkan12Features *dest, VkPhysicalDevice8BitStorageFeaturesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(storageBuffer8BitAccess);
    TRANSFER_VALUE(uniformAndStorageBuffer8BitAccess);
    TRANSFER_VALUE(storagePushConstant8);
}

void TransferValue(VkPhysicalDeviceVulkan12Features *dest, VkPhysicalDeviceShaderAtomicInt64FeaturesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(shaderBufferInt64Atomics);
    TRANSFER_VALUE(shaderSharedInt64Atomics);
}

void TransferValue(VkPhysicalDeviceVulkan12Properties *dest, VkPhysicalDeviceDriverPropertiesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(driverID);
    TRANSFER_VALUE(driverName);
    TRANSFER_VALUE(driverInfo);
    TRANSFER_VALUE(conformanceVersion);
}

void TransferValue(VkPhysicalDeviceVulkan12Properties *dest, VkPhysicalDeviceFloatControlsPropertiesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(denormBehaviorIndependence);
    TRANSFER_VALUE(roundingModeIndependence);
    TRANSFER_VALUE(shaderSignedZeroInfNanPreserveFloat16);
    TRANSFER_VALUE(shaderSignedZeroInfNanPreserveFloat32);
    TRANSFER_VALUE(shaderSignedZeroInfNanPreserveFloat64);
    TRANSFER_VALUE(shaderDenormPreserveFloat16);
    TRANSFER_VALUE(shaderDenormPreserveFloat32);
    TRANSFER_VALUE(shaderDenormPreserveFloat64);
    TRANSFER_VALUE(shaderDenormFlushToZeroFloat16);
    TRANSFER_VALUE(shaderDenormFlushToZeroFloat32);
    TRANSFER_VALUE(shaderDenormFlushToZeroFloat64);
    TRANSFER_VALUE(shaderRoundingModeRTEFloat16);
    TRANSFER_VALUE(shaderRoundingModeRTEFloat32);
    TRANSFER_VALUE(shaderRoundingModeRTEFloat64);
    TRANSFER_VALUE(shaderRoundingModeRTZFloat16);
    TRANSFER_VALUE(shaderRoundingModeRTZFloat32);
    TRANSFER_VALUE(shaderRoundingModeRTZFloat64);
}

void TransferValue(VkPhysicalDeviceVulkan12Properties *dest, VkPhysicalDeviceDepthStencilResolvePropertiesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(supportedDepthResolveModes);
    TRANSFER_VALUE(supportedStencilResolveModes);
    TRANSFER_VALUE(independentResolveNone);
    TRANSFER_VALUE(independentResolve);
}

void TransferValue(VkPhysicalDeviceVulkan12Properties *dest, VkPhysicalDeviceTimelineSemaphorePropertiesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(maxTimelineSemaphoreValueDifference);
}

void TransferValue(VkPhysicalDeviceVulkan12Features *dest, VkPhysicalDeviceTimelineSemaphoreFeaturesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(timelineSemaphore);
}

void TransferValue(VkPhysicalDeviceVulkan12Features *dest, VkPhysicalDeviceVulkanMemoryModelFeaturesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(vulkanMemoryModel);
    TRANSFER_VALUE(vulkanMemoryModelDeviceScope);
    TRANSFER_VALUE(vulkanMemoryModelAvailabilityVisibilityChains);
}

void TransferValue(VkPhysicalDeviceVulkan12Features *dest, VkPhysicalDeviceScalarBlockLayoutFeaturesEXT *src, bool promoted_written) {
    TRANSFER_VALUE(scalarBlockLayout);
}

void TransferValue(VkPhysicalDeviceVulkan12Features *dest, VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(separateDepthStencilLayouts);
}

void TransferValue(VkPhysicalDeviceVulkan12Features *dest, VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(uniformBufferStandardLayout);
}

void TransferValue(VkPhysicalDeviceVulkan12Features *dest, VkPhysicalDeviceBufferDeviceAddressFeaturesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(bufferDeviceAddress);
    TRANSFER_VALUE(bufferDeviceAddressCaptureReplay);
    TRANSFER_VALUE(bufferDeviceAddressMultiDevice);
}

void TransferValue(VkPhysicalDeviceVulkan12Features *dest, VkPhysicalDeviceHostQueryResetFeaturesEXT *src, bool promoted_written) {
    TRANSFER_VALUE(hostQueryReset);
}


// VK_VULKAN_1_3

void TransferValue(VkPhysicalDeviceVulkan13Features *dest, VkPhysicalDeviceDynamicRenderingFeaturesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(dynamicRendering);
}

void TransferValue(VkPhysicalDeviceVulkan13Features *dest, VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT *src, bool promoted_written) {
    TRANSFER_VALUE(textureCompressionASTC_HDR);
}

void TransferValue(VkPhysicalDeviceVulkan13Properties *dest, VkPhysicalDeviceInlineUniformBlockPropertiesEXT *src, bool promoted_written) {
    TRANSFER_VALUE(maxInlineUniformBlockSize);
    TRANSFER_VALUE(maxPerStageDescriptorInlineUniformBlocks);
    TRANSFER_VALUE(maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks);
    TRANSFER_VALUE(maxDescriptorSetInlineUniformBlocks);
    TRANSFER_VALUE(maxDescriptorSetUpdateAfterBindInlineUniformBlocks);
}

void TransferValue(VkPhysicalDeviceVulkan13Features *dest, VkPhysicalDeviceInlineUniformBlockFeaturesEXT *src, bool promoted_written) {
    TRANSFER_VALUE(inlineUniformBlock);
    TRANSFER_VALUE(descriptorBindingInlineUniformBlockUpdateAfterBind);
}

void TransferValue(VkPhysicalDeviceVulkan13Features *dest, VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(shaderTerminateInvocation);
}

void TransferValue(VkPhysicalDeviceVulkan13Properties *dest, VkPhysicalDeviceSubgroupSizeControlPropertiesEXT *src, bool promoted_written) {
    TRANSFER_VALUE(minSubgroupSize);
    TRANSFER_VALUE(maxSubgroupSize);
    TRANSFER_VALUE(maxComputeWorkgroupSubgroups);
    TRANSFER_VALUE(requiredSubgroupSizeStages);
}

void TransferValue(VkPhysicalDeviceVulkan13Features *dest, VkPhysicalDeviceSubgroupSizeControlFeaturesEXT *src, bool promoted_written) {
    TRANSFER_VALUE(subgroupSizeControl);
    TRANSFER_VALUE(computeFullSubgroups);
}

void TransferValue(VkPhysicalDeviceVulkan13Features *dest, VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT *src, bool promoted_written) {
    TRANSFER_VALUE(shaderDemoteToHelperInvocation);
}

void TransferValue(VkPhysicalDeviceVulkan13Properties *dest, VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(integerDotProduct8BitUnsignedAccelerated);
    TRANSFER_VALUE(integerDotProduct8BitSignedAccelerated);
    TRANSFER_VALUE(integerDotProduct8BitMixedSignednessAccelerated);
    TRANSFER_VALUE(integerDotProduct4x8BitPackedUnsignedAccelerated);
    TRANSFER_VALUE(integerDotProduct4x8BitPackedSignedAccelerated);
    TRANSFER_VALUE(integerDotProduct4x8BitPackedMixedSignednessAccelerated);
    TRANSFER_VALUE(integerDotProduct16BitUnsignedAccelerated);
    TRANSFER_VALUE(integerDotProduct16BitSignedAccelerated);
    TRANSFER_VALUE(integerDotProduct16BitMixedSignednessAccelerated);
    TRANSFER_VALUE(integerDotProduct32BitUnsignedAccelerated);
    TRANSFER_VALUE(integerDotProduct32BitSignedAccelerated);
    TRANSFER_VALUE(integerDotProduct32BitMixedSignednessAccelerated);
    TRANSFER_VALUE(integerDotProduct64BitUnsignedAccelerated);
    TRANSFER_VALUE(integerDotProduct64BitSignedAccelerated);
    TRANSFER_VALUE(integerDotProduct64BitMixedSignednessAccelerated);
    TRANSFER_VALUE(integerDotProductAccumulatingSaturating8BitUnsignedAccelerated);
    TRANSFER_VALUE(integerDotProductAccumulatingSaturating8BitSignedAccelerated);
    TRANSFER_VALUE(integerDotProductAccumulatingSaturating8BitMixedSignednessAccelerated);
    TRANSFER_VALUE(integerDotProductAccumulatingSaturating4x8BitPackedUnsignedAccelerated);
    TRANSFER_VALUE(integerDotProductAccumulatingSaturating4x8BitPackedSignedAccelerated);
    TRANSFER_VALUE(integerDotProductAccumulatingSaturating4x8BitPackedMixedSignednessAccelerated);
    TRANSFER_VALUE(integerDotProductAccumulatingSaturating16BitUnsignedAccelerated);
    TRANSFER_VALUE(integerDotProductAccumulatingSaturating16BitSignedAccelerated);
    TRANSFER_VALUE(integerDotProductAccumulatingSaturating16BitMixedSignednessAccelerated);
    TRANSFER_VALUE(integerDotProductAccumulatingSaturating32BitUnsignedAccelerated);
    TRANSFER_VALUE(integerDotProductAccumulatingSaturating32BitSignedAccelerated);
    TRANSFER_VALUE(integerDotProductAccumulatingSaturating32BitMixedSignednessAccelerated);
    TRANSFER_VALUE(integerDotProductAccumulatingSaturating64BitUnsignedAccelerated);
    TRANSFER_VALUE(integerDotProductAccumulatingSaturating64BitSignedAccelerated);
    TRANSFER_VALUE(integerDotProductAccumulatingSaturating64BitMixedSignednessAccelerated);
}

void TransferValue(VkPhysicalDeviceVulkan13Features *dest, VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(shaderIntegerDotProduct);
}

void TransferValue(VkPhysicalDeviceVulkan13Properties *dest, VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT *src, bool promoted_written) {
    TRANSFER_VALUE(storageTexelBufferOffsetAlignmentBytes);
    TRANSFER_VALUE(storageTexelBufferOffsetSingleTexelAlignment);
    TRANSFER_VALUE(uniformTexelBufferOffsetAlignmentBytes);
    TRANSFER_VALUE(uniformTexelBufferOffsetSingleTexelAlignment);
}

void TransferValue(VkPhysicalDeviceVulkan13Features *dest, VkPhysicalDevicePrivateDataFeaturesEXT *src, bool promoted_written) {
    TRANSFER_VALUE(privateData);
}

void TransferValue(VkPhysicalDeviceVulkan13Features *dest, VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT *src, bool promoted_written) {
    TRANSFER_VALUE(pipelineCreationCacheControl);
}

void TransferValue(VkPhysicalDeviceVulkan13Features *dest, VkPhysicalDeviceSynchronization2FeaturesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(synchronization2);
}

void TransferValue(VkPhysicalDeviceVulkan13Features *dest, VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(shaderZeroInitializeWorkgroupMemory);
}

void TransferValue(VkPhysicalDeviceVulkan13Features *dest, VkPhysicalDeviceImageRobustnessFeaturesEXT *src, bool promoted_written) {
    TRANSFER_VALUE(robustImageAccess);
}

void TransferValue(VkPhysicalDeviceVulkan13Properties *dest, VkPhysicalDeviceMaintenance4PropertiesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(maxBufferSize);
}

void TransferValue(VkPhysicalDeviceVulkan13Features *dest, VkPhysicalDeviceMaintenance4FeaturesKHR *src, bool promoted_written) {
    TRANSFER_VALUE(maintenance4);
}

#undef TRANSFER_VALUE_ARRAY
#undef TRANSFER_VALUE

void LoadDeviceFormats(VkInstance instance, PhysicalDeviceData *pdd, VkPhysicalDevice pd, ArrayOfVkFormatProperties *dest,
                       ArrayOfVkFormatProperties3 *dest3) {
    std::vector<VkFormat> formats = {
        VK_FORMAT_UNDEFINED,
        VK_FORMAT_R4G4_UNORM_PACK8,
        VK_FORMAT_R4G4B4A4_UNORM_PACK16,
        VK_FORMAT_B4G4R4A4_UNORM_PACK16,
        VK_FORMAT_R5G6B5_UNORM_PACK16,
        VK_FORMAT_B5G6R5_UNORM_PACK16,
        VK_FORMAT_R5G5B5A1_UNORM_PACK16,
        VK_FORMAT_B5G5R5A1_UNORM_PACK16,
        VK_FORMAT_A1R5G5B5_UNORM_PACK16,
        VK_FORMAT_R8_UNORM,
        VK_FORMAT_R8_SNORM,
        VK_FORMAT_R8_USCALED,
        VK_FORMAT_R8_SSCALED,
        VK_FORMAT_R8_UINT,
        VK_FORMAT_R8_SINT,
        VK_FORMAT_R8_SRGB,
        VK_FORMAT_R8G8_UNORM,
        VK_FORMAT_R8G8_SNORM,
        VK_FORMAT_R8G8_USCALED,
        VK_FORMAT_R8G8_SSCALED,
        VK_FORMAT_R8G8_UINT,
        VK_FORMAT_R8G8_SINT,
        VK_FORMAT_R8G8_SRGB,
        VK_FORMAT_R8G8B8_UNORM,
        VK_FORMAT_R8G8B8_SNORM,
        VK_FORMAT_R8G8B8_USCALED,
        VK_FORMAT_R8G8B8_SSCALED,
        VK_FORMAT_R8G8B8_UINT,
        VK_FORMAT_R8G8B8_SINT,
        VK_FORMAT_R8G8B8_SRGB,
        VK_FORMAT_B8G8R8_UNORM,
        VK_FORMAT_B8G8R8_SNORM,
        VK_FORMAT_B8G8R8_USCALED,
        VK_FORMAT_B8G8R8_SSCALED,
        VK_FORMAT_B8G8R8_UINT,
        VK_FORMAT_B8G8R8_SINT,
        VK_FORMAT_B8G8R8_SRGB,
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_FORMAT_R8G8B8A8_SNORM,
        VK_FORMAT_R8G8B8A8_USCALED,
        VK_FORMAT_R8G8B8A8_SSCALED,
        VK_FORMAT_R8G8B8A8_UINT,
        VK_FORMAT_R8G8B8A8_SINT,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_FORMAT_B8G8R8A8_UNORM,
        VK_FORMAT_B8G8R8A8_SNORM,
        VK_FORMAT_B8G8R8A8_USCALED,
        VK_FORMAT_B8G8R8A8_SSCALED,
        VK_FORMAT_B8G8R8A8_UINT,
        VK_FORMAT_B8G8R8A8_SINT,
        VK_FORMAT_B8G8R8A8_SRGB,
        VK_FORMAT_A8B8G8R8_UNORM_PACK32,
        VK_FORMAT_A8B8G8R8_SNORM_PACK32,
        VK_FORMAT_A8B8G8R8_USCALED_PACK32,
        VK_FORMAT_A8B8G8R8_SSCALED_PACK32,
        VK_FORMAT_A8B8G8R8_UINT_PACK32,
        VK_FORMAT_A8B8G8R8_SINT_PACK32,
        VK_FORMAT_A8B8G8R8_SRGB_PACK32,
        VK_FORMAT_A2R10G10B10_UNORM_PACK32,
        VK_FORMAT_A2R10G10B10_SNORM_PACK32,
        VK_FORMAT_A2R10G10B10_USCALED_PACK32,
        VK_FORMAT_A2R10G10B10_SSCALED_PACK32,
        VK_FORMAT_A2R10G10B10_UINT_PACK32,
        VK_FORMAT_A2R10G10B10_SINT_PACK32,
        VK_FORMAT_A2B10G10R10_UNORM_PACK32,
        VK_FORMAT_A2B10G10R10_SNORM_PACK32,
        VK_FORMAT_A2B10G10R10_USCALED_PACK32,
        VK_FORMAT_A2B10G10R10_SSCALED_PACK32,
        VK_FORMAT_A2B10G10R10_UINT_PACK32,
        VK_FORMAT_A2B10G10R10_SINT_PACK32,
        VK_FORMAT_R16_UNORM,
        VK_FORMAT_R16_SNORM,
        VK_FORMAT_R16_USCALED,
        VK_FORMAT_R16_SSCALED,
        VK_FORMAT_R16_UINT,
        VK_FORMAT_R16_SINT,
        VK_FORMAT_R16_SFLOAT,
        VK_FORMAT_R16G16_UNORM,
        VK_FORMAT_R16G16_SNORM,
        VK_FORMAT_R16G16_USCALED,
        VK_FORMAT_R16G16_SSCALED,
        VK_FORMAT_R16G16_UINT,
        VK_FORMAT_R16G16_SINT,
        VK_FORMAT_R16G16_SFLOAT,
        VK_FORMAT_R16G16B16_UNORM,
        VK_FORMAT_R16G16B16_SNORM,
        VK_FORMAT_R16G16B16_USCALED,
        VK_FORMAT_R16G16B16_SSCALED,
        VK_FORMAT_R16G16B16_UINT,
        VK_FORMAT_R16G16B16_SINT,
        VK_FORMAT_R16G16B16_SFLOAT,
        VK_FORMAT_R16G16B16A16_UNORM,
        VK_FORMAT_R16G16B16A16_SNORM,
        VK_FORMAT_R16G16B16A16_USCALED,
        VK_FORMAT_R16G16B16A16_SSCALED,
        VK_FORMAT_R16G16B16A16_UINT,
        VK_FORMAT_R16G16B16A16_SINT,
        VK_FORMAT_R16G16B16A16_SFLOAT,
        VK_FORMAT_R32_UINT,
        VK_FORMAT_R32_SINT,
        VK_FORMAT_R32_SFLOAT,
        VK_FORMAT_R32G32_UINT,
        VK_FORMAT_R32G32_SINT,
        VK_FORMAT_R32G32_SFLOAT,
        VK_FORMAT_R32G32B32_UINT,
        VK_FORMAT_R32G32B32_SINT,
        VK_FORMAT_R32G32B32_SFLOAT,
        VK_FORMAT_R32G32B32A32_UINT,
        VK_FORMAT_R32G32B32A32_SINT,
        VK_FORMAT_R32G32B32A32_SFLOAT,
        VK_FORMAT_R64_UINT,
        VK_FORMAT_R64_SINT,
        VK_FORMAT_R64_SFLOAT,
        VK_FORMAT_R64G64_UINT,
        VK_FORMAT_R64G64_SINT,
        VK_FORMAT_R64G64_SFLOAT,
        VK_FORMAT_R64G64B64_UINT,
        VK_FORMAT_R64G64B64_SINT,
        VK_FORMAT_R64G64B64_SFLOAT,
        VK_FORMAT_R64G64B64A64_UINT,
        VK_FORMAT_R64G64B64A64_SINT,
        VK_FORMAT_R64G64B64A64_SFLOAT,
        VK_FORMAT_B10G11R11_UFLOAT_PACK32,
        VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,
        VK_FORMAT_D16_UNORM,
        VK_FORMAT_X8_D24_UNORM_PACK32,
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_BC1_RGB_UNORM_BLOCK,
        VK_FORMAT_BC1_RGB_SRGB_BLOCK,
        VK_FORMAT_BC1_RGBA_UNORM_BLOCK,
        VK_FORMAT_BC1_RGBA_SRGB_BLOCK,
        VK_FORMAT_BC2_UNORM_BLOCK,
        VK_FORMAT_BC2_SRGB_BLOCK,
        VK_FORMAT_BC3_UNORM_BLOCK,
        VK_FORMAT_BC3_SRGB_BLOCK,
        VK_FORMAT_BC4_UNORM_BLOCK,
        VK_FORMAT_BC4_SNORM_BLOCK,
        VK_FORMAT_BC5_UNORM_BLOCK,
        VK_FORMAT_BC5_SNORM_BLOCK,
        VK_FORMAT_BC6H_UFLOAT_BLOCK,
        VK_FORMAT_BC6H_SFLOAT_BLOCK,
        VK_FORMAT_BC7_UNORM_BLOCK,
        VK_FORMAT_BC7_SRGB_BLOCK,
        VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK,
        VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK,
        VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK,
        VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK,
        VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK,
        VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK,
        VK_FORMAT_EAC_R11_UNORM_BLOCK,
        VK_FORMAT_EAC_R11_SNORM_BLOCK,
        VK_FORMAT_EAC_R11G11_UNORM_BLOCK,
        VK_FORMAT_EAC_R11G11_SNORM_BLOCK,
        VK_FORMAT_ASTC_4x4_UNORM_BLOCK,
        VK_FORMAT_ASTC_4x4_SRGB_BLOCK,
        VK_FORMAT_ASTC_5x4_UNORM_BLOCK,
        VK_FORMAT_ASTC_5x4_SRGB_BLOCK,
        VK_FORMAT_ASTC_5x5_UNORM_BLOCK,
        VK_FORMAT_ASTC_5x5_SRGB_BLOCK,
        VK_FORMAT_ASTC_6x5_UNORM_BLOCK,
        VK_FORMAT_ASTC_6x5_SRGB_BLOCK,
        VK_FORMAT_ASTC_6x6_UNORM_BLOCK,
        VK_FORMAT_ASTC_6x6_SRGB_BLOCK,
        VK_FORMAT_ASTC_8x5_UNORM_BLOCK,
        VK_FORMAT_ASTC_8x5_SRGB_BLOCK,
        VK_FORMAT_ASTC_8x6_UNORM_BLOCK,
        VK_FORMAT_ASTC_8x6_SRGB_BLOCK,
        VK_FORMAT_ASTC_8x8_UNORM_BLOCK,
        VK_FORMAT_ASTC_8x8_SRGB_BLOCK,
        VK_FORMAT_ASTC_10x5_UNORM_BLOCK,
        VK_FORMAT_ASTC_10x5_SRGB_BLOCK,
        VK_FORMAT_ASTC_10x6_UNORM_BLOCK,
        VK_FORMAT_ASTC_10x6_SRGB_BLOCK,
        VK_FORMAT_ASTC_10x8_UNORM_BLOCK,
        VK_FORMAT_ASTC_10x8_SRGB_BLOCK,
        VK_FORMAT_ASTC_10x10_UNORM_BLOCK,
        VK_FORMAT_ASTC_10x10_SRGB_BLOCK,
        VK_FORMAT_ASTC_12x10_UNORM_BLOCK,
        VK_FORMAT_ASTC_12x10_SRGB_BLOCK,
        VK_FORMAT_ASTC_12x12_UNORM_BLOCK,
        VK_FORMAT_ASTC_12x12_SRGB_BLOCK,
        VK_FORMAT_G8B8G8R8_422_UNORM,
        VK_FORMAT_B8G8R8G8_422_UNORM,
        VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM,
        VK_FORMAT_G8_B8R8_2PLANE_420_UNORM,
        VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM,
        VK_FORMAT_G8_B8R8_2PLANE_422_UNORM,
        VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM,
        VK_FORMAT_R10X6_UNORM_PACK16,
        VK_FORMAT_R10X6G10X6_UNORM_2PACK16,
        VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16,
        VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16,
        VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16,
        VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16,
        VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16,
        VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16,
        VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16,
        VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16,
        VK_FORMAT_R12X4_UNORM_PACK16,
        VK_FORMAT_R12X4G12X4_UNORM_2PACK16,
        VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16,
        VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16,
        VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16,
        VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16,
        VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16,
        VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16,
        VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16,
        VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16,
        VK_FORMAT_G16B16G16R16_422_UNORM,
        VK_FORMAT_B16G16R16G16_422_UNORM,
        VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM,
        VK_FORMAT_G16_B16R16_2PLANE_420_UNORM,
        VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM,
        VK_FORMAT_G16_B16R16_2PLANE_422_UNORM,
        VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM,
        VK_FORMAT_G8_B8R8_2PLANE_444_UNORM,
        VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16,
        VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16,
        VK_FORMAT_G16_B16R16_2PLANE_444_UNORM,
        VK_FORMAT_A4R4G4B4_UNORM_PACK16,
        VK_FORMAT_A4B4G4R4_UNORM_PACK16,
        VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK,
        VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK,
        VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK,
        VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK,
        VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK,
        VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK,
        VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK,
        VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK,
        VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK,
        VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK,
        VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK,
        VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK,
        VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK,
        VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK,
        VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG,
        VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG,
        VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG,
        VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG,
        VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG,
        VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG,
        VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG,
        VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG,
    };
    const auto dt = instance_dispatch_table(instance);
    for (const auto format : formats) {
        VkFormatProperties3KHR format_properties_3 = {};
        format_properties_3.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3_KHR;

        VkFormatProperties2 format_properties = {};
        format_properties.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
        format_properties.pNext = &format_properties_3;

        if (pdd->GetEffectiveVersion() >= VK_API_VERSION_1_1) {
            dt->GetPhysicalDeviceFormatProperties2(pd, format, &format_properties);
        } else {
            dt->GetPhysicalDeviceFormatProperties2KHR(pd, format, &format_properties);
        }
        (*dest)[format] = format_properties.formatProperties;
        (*dest3)[format] = format_properties_3;
    }
}

void LoadQueueFamilyProperties(VkInstance instance, VkPhysicalDevice pd, PhysicalDeviceData *pdd) {
    const auto dt = instance_dispatch_table(instance);
    uint32_t count = 0;
    dt->GetPhysicalDeviceQueueFamilyProperties2KHR(pd, &count, nullptr);
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
                pdd->device_queue_family_properties_[i].video_properties_2_.pNext = pNext[i];

                pNext[i] = &pdd->device_queue_family_properties_[i].video_properties_2_;

                pdd->device_queue_family_properties_[i].query_result_status_properties_2_.pNext = pNext[i];

                pNext[i] = &pdd->device_queue_family_properties_[i].query_result_status_properties_2_;
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
        dt->GetPhysicalDeviceQueueFamilyProperties2KHR(pd, &count, props.data());
        for (uint32_t i = 0; i < count; ++i) {
            pdd->device_queue_family_properties_[i].properties_2 = props[i];
        }
    }
}

VKAPI_ATTR VkResult VKAPI_CALL EnumeratePhysicalDevices(VkInstance instance, uint32_t *pPhysicalDeviceCount,
                                                        VkPhysicalDevice *pPhysicalDevices) {
    // Our layer-specific initialization...

    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(instance);
    VkResult result = dt->EnumeratePhysicalDevices(instance, pPhysicalDeviceCount, pPhysicalDevices);

    // HACK!! epd_count is used to ensure the following code only gets called _after_ vkCreateInstance finishes *in the "vkcube +
    // profiles" use case*
    if (pPhysicalDevices && (VK_SUCCESS == result)) {
        std::vector<VkPhysicalDevice> physical_devices;
        result = EnumerateAll<VkPhysicalDevice>(&physical_devices, [&](uint32_t *count, VkPhysicalDevice *results) {
            return dt->EnumeratePhysicalDevices(instance, count, results);
        });
        if (result != VK_SUCCESS) {
            return result;
        }

        // For each physical device, create and populate a PDD instance.
        for (const auto &physical_device : physical_devices) {
            if (PhysicalDeviceData::Find(physical_device)) {
                continue;
            }

            PhysicalDeviceData &pdd = PhysicalDeviceData::Create(physical_device, instance);

            EnumerateAll<VkExtensionProperties>(&(pdd.device_extensions_), [&](uint32_t *count, VkExtensionProperties *results) {
                return dt->EnumerateDeviceExtensionProperties(physical_device, nullptr, count, results);
            });

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
                } else if (layer_settings->emulate_portability) {
                    pdd.physical_device_portability_subset_properties_ = {
                        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES_KHR, nullptr, 1};
                    pdd.physical_device_portability_subset_features_ = {
                        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR,
                        nullptr,
                        VK_TRUE,
                        VK_TRUE,
                        VK_TRUE,
                        VK_TRUE,
                        VK_TRUE,
                        VK_TRUE,
                        VK_TRUE,
                        VK_TRUE,
                        VK_TRUE,
                        VK_TRUE,
                        VK_TRUE,
                        VK_TRUE,
                        VK_TRUE,
                        VK_TRUE,
                        VK_TRUE};
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME)) {
                    pdd.physical_device_transform_feedback_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_transform_feedback_properties_);

                    pdd.physical_device_transform_feedback_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_transform_feedback_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME)) {
                    pdd.physical_device_dynamic_rendering_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_dynamic_rendering_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_NV_CORNER_SAMPLED_IMAGE_EXTENSION_NAME)) {
                    pdd.physical_device_corner_sampled_image_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_corner_sampled_image_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_MULTIVIEW_EXTENSION_NAME)) {
                    pdd.physical_device_multiview_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_multiview_properties_);

                    pdd.physical_device_multiview_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_multiview_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_TEXTURE_COMPRESSION_ASTC_HDR_EXTENSION_NAME)) {
                    pdd.physical_device_texture_compression_astchdrfeatures_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_texture_compression_astchdrfeatures_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_ASTC_DECODE_MODE_EXTENSION_NAME)) {
                    pdd.physical_device_astcdecode_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_astcdecode_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_PIPELINE_ROBUSTNESS_EXTENSION_NAME)) {
                    pdd.physical_device_pipeline_robustness_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_pipeline_robustness_properties_);

                    pdd.physical_device_pipeline_robustness_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_pipeline_robustness_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME)) {
                    pdd.physical_device_idproperties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_idproperties_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME)) {
                    pdd.physical_device_push_descriptor_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_push_descriptor_properties_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_CONDITIONAL_RENDERING_EXTENSION_NAME)) {
                    pdd.physical_device_conditional_rendering_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_conditional_rendering_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME)) {
                    pdd.physical_device_float_16_int_8_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_float_16_int_8_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_16BIT_STORAGE_EXTENSION_NAME)) {
                    pdd.physical_device_16_bit_storage_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_16_bit_storage_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_NVX_MULTIVIEW_PER_VIEW_ATTRIBUTES_EXTENSION_NAME)) {
                    pdd.physical_device_multiview_per_view_attributes_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_multiview_per_view_attributes_properties_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_DISCARD_RECTANGLES_EXTENSION_NAME)) {
                    pdd.physical_device_discard_rectangle_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_discard_rectangle_properties_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME)) {
                    pdd.physical_device_conservative_rasterization_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_conservative_rasterization_properties_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_DEPTH_CLIP_ENABLE_EXTENSION_NAME)) {
                    pdd.physical_device_depth_clip_enable_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_depth_clip_enable_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME)) {
                    pdd.physical_device_imageless_framebuffer_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_imageless_framebuffer_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_PERFORMANCE_QUERY_EXTENSION_NAME)) {
                    pdd.physical_device_performance_query_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_performance_query_properties_);

                    pdd.physical_device_performance_query_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_performance_query_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_MAINTENANCE_2_EXTENSION_NAME)) {
                    pdd.physical_device_point_clipping_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_point_clipping_properties_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME)) {
                    pdd.physical_device_variable_pointer_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_variable_pointer_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME)) {
                    pdd.physical_device_sampler_filter_minmax_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_sampler_filter_minmax_properties_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME)) {
                    pdd.physical_device_inline_uniform_block_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_inline_uniform_block_properties_);

                    pdd.physical_device_inline_uniform_block_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_inline_uniform_block_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_SAMPLE_LOCATIONS_EXTENSION_NAME)) {
                    pdd.physical_device_sample_locations_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_sample_locations_properties_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME)) {
                    pdd.physical_device_blend_operation_advanced_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_blend_operation_advanced_properties_);

                    pdd.physical_device_blend_operation_advanced_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_blend_operation_advanced_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME)) {
                    pdd.physical_device_acceleration_structure_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_acceleration_structure_properties_);

                    pdd.physical_device_acceleration_structure_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_acceleration_structure_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME)) {
                    pdd.physical_device_ray_tracing_pipeline_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_ray_tracing_pipeline_properties_);

                    pdd.physical_device_ray_tracing_pipeline_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_ray_tracing_pipeline_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_RAY_QUERY_EXTENSION_NAME)) {
                    pdd.physical_device_ray_query_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_ray_query_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_NV_SHADER_SM_BUILTINS_EXTENSION_NAME)) {
                    pdd.physical_device_shader_smbuiltins_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_shader_smbuiltins_properties_);

                    pdd.physical_device_shader_smbuiltins_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_smbuiltins_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME)) {
                    pdd.physical_device_sampler_ycbcr_conversion_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_sampler_ycbcr_conversion_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)) {
                    pdd.physical_device_descriptor_indexing_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_descriptor_indexing_properties_);

                    pdd.physical_device_descriptor_indexing_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_descriptor_indexing_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_NV_SHADING_RATE_IMAGE_EXTENSION_NAME)) {
                    pdd.physical_device_shading_rate_image_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_shading_rate_image_properties_);

                    pdd.physical_device_shading_rate_image_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shading_rate_image_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_NV_RAY_TRACING_EXTENSION_NAME)) {
                    pdd.physical_device_ray_tracing_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_ray_tracing_properties_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_NV_REPRESENTATIVE_FRAGMENT_TEST_EXTENSION_NAME)) {
                    pdd.physical_device_representative_fragment_test_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_representative_fragment_test_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_MAINTENANCE_3_EXTENSION_NAME)) {
                    pdd.physical_device_maintenance_3_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_maintenance_3_properties_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_SHADER_SUBGROUP_EXTENDED_TYPES_EXTENSION_NAME)) {
                    pdd.physical_device_shader_subgroup_extended_types_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_subgroup_extended_types_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_8BIT_STORAGE_EXTENSION_NAME)) {
                    pdd.physical_device_8_bit_storage_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_8_bit_storage_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_EXTERNAL_MEMORY_HOST_EXTENSION_NAME)) {
                    pdd.physical_device_external_memory_host_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_external_memory_host_properties_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_SHADER_ATOMIC_INT64_EXTENSION_NAME)) {
                    pdd.physical_device_shader_atomic_int_64_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_atomic_int_64_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_SHADER_CLOCK_EXTENSION_NAME)) {
                    pdd.physical_device_shader_clock_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_clock_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_AMD_SHADER_CORE_PROPERTIES_EXTENSION_NAME)) {
                    pdd.physical_device_shader_core_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_shader_core_properties_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME)) {
                    pdd.physical_device_vertex_attribute_divisor_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_vertex_attribute_divisor_properties_);

                    pdd.physical_device_vertex_attribute_divisor_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_vertex_attribute_divisor_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME)) {
                    pdd.physical_device_driver_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_driver_properties_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME)) {
                    pdd.physical_device_float_controls_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_float_controls_properties_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME)) {
                    pdd.physical_device_depth_stencil_resolve_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_depth_stencil_resolve_properties_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_NV_COMPUTE_SHADER_DERIVATIVES_EXTENSION_NAME)) {
                    pdd.physical_device_compute_shader_derivatives_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_compute_shader_derivatives_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_NV_MESH_SHADER_EXTENSION_NAME)) {
                    pdd.physical_device_mesh_shader_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_mesh_shader_properties_);

                    pdd.physical_device_mesh_shader_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_mesh_shader_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_NV_FRAGMENT_SHADER_BARYCENTRIC_EXTENSION_NAME)) {
                    pdd.physical_device_fragment_shader_barycentric_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_fragment_shader_barycentric_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_NV_SHADER_IMAGE_FOOTPRINT_EXTENSION_NAME)) {
                    pdd.physical_device_shader_image_footprint_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_image_footprint_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_NV_SCISSOR_EXCLUSIVE_EXTENSION_NAME)) {
                    pdd.physical_device_exclusive_scissor_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_exclusive_scissor_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME)) {
                    pdd.physical_device_timeline_semaphore_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_timeline_semaphore_properties_);

                    pdd.physical_device_timeline_semaphore_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_timeline_semaphore_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_INTEL_SHADER_INTEGER_FUNCTIONS_2_EXTENSION_NAME)) {
                    pdd.physical_device_shader_integer_functions_2_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_integer_functions_2_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_VULKAN_MEMORY_MODEL_EXTENSION_NAME)) {
                    pdd.physical_device_vulkan_memory_model_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_vulkan_memory_model_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_PCI_BUS_INFO_EXTENSION_NAME)) {
                    pdd.physical_device_pcibus_info_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_pcibus_info_properties_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_SHADER_TERMINATE_INVOCATION_EXTENSION_NAME)) {
                    pdd.physical_device_shader_terminate_invocation_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_terminate_invocation_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_FRAGMENT_DENSITY_MAP_EXTENSION_NAME)) {
                    pdd.physical_device_fragment_density_map_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_fragment_density_map_properties_);

                    pdd.physical_device_fragment_density_map_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_fragment_density_map_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME)) {
                    pdd.physical_device_scalar_block_layout_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_scalar_block_layout_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME)) {
                    pdd.physical_device_subgroup_size_control_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_subgroup_size_control_properties_);

                    pdd.physical_device_subgroup_size_control_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_subgroup_size_control_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME)) {
                    pdd.physical_device_fragment_shading_rate_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_fragment_shading_rate_properties_);

                    pdd.physical_device_fragment_shading_rate_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_fragment_shading_rate_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_AMD_SHADER_CORE_PROPERTIES_2_EXTENSION_NAME)) {
                    pdd.physical_device_shader_core_properties_2_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_shader_core_properties_2_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME)) {
                    pdd.physical_device_coherent_memory_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_coherent_memory_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_SHADER_IMAGE_ATOMIC_INT64_EXTENSION_NAME)) {
                    pdd.physical_device_shader_image_atomic_int_64_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_image_atomic_int_64_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME)) {
                    pdd.physical_device_memory_priority_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_memory_priority_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_NV_DEDICATED_ALLOCATION_IMAGE_ALIASING_EXTENSION_NAME)) {
                    pdd.physical_device_dedicated_allocation_image_aliasing_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_dedicated_allocation_image_aliasing_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_SEPARATE_DEPTH_STENCIL_LAYOUTS_EXTENSION_NAME)) {
                    pdd.physical_device_separate_depth_stencil_layouts_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_separate_depth_stencil_layouts_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME)) {
                    pdd.physical_device_buffer_address_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_buffer_address_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_PRESENT_WAIT_EXTENSION_NAME)) {
                    pdd.physical_device_present_wait_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_present_wait_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_NV_COOPERATIVE_MATRIX_EXTENSION_NAME)) {
                    pdd.physical_device_cooperative_matrix_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_cooperative_matrix_properties_);

                    pdd.physical_device_cooperative_matrix_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_cooperative_matrix_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_NV_COVERAGE_REDUCTION_MODE_EXTENSION_NAME)) {
                    pdd.physical_device_coverage_reduction_mode_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_coverage_reduction_mode_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME)) {
                    pdd.physical_device_fragment_shader_interlock_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_fragment_shader_interlock_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_YCBCR_IMAGE_ARRAYS_EXTENSION_NAME)) {
                    pdd.physical_device_ycbcr_image_arrays_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_ycbcr_image_arrays_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT_EXTENSION_NAME)) {
                    pdd.physical_device_uniform_buffer_standard_layout_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_uniform_buffer_standard_layout_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_PROVOKING_VERTEX_EXTENSION_NAME)) {
                    pdd.physical_device_provoking_vertex_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_provoking_vertex_properties_);

                    pdd.physical_device_provoking_vertex_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_provoking_vertex_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME)) {
                    pdd.physical_device_buffer_device_address_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_buffer_device_address_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME)) {
                    pdd.physical_device_line_rasterization_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_line_rasterization_properties_);

                    pdd.physical_device_line_rasterization_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_line_rasterization_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME)) {
                    pdd.physical_device_shader_atomic_float_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_atomic_float_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME)) {
                    pdd.physical_device_host_query_reset_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_host_query_reset_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_INDEX_TYPE_UINT8_EXTENSION_NAME)) {
                    pdd.physical_device_index_type_uint_8_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_index_type_uint_8_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME)) {
                    pdd.physical_device_extended_dynamic_state_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_extended_dynamic_state_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_PIPELINE_EXECUTABLE_PROPERTIES_EXTENSION_NAME)) {
                    pdd.physical_device_pipeline_executable_properties_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_pipeline_executable_properties_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_SHADER_ATOMIC_FLOAT_2_EXTENSION_NAME)) {
                    pdd.physical_device_shader_atomic_float_2_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_atomic_float_2_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME)) {
                    pdd.physical_device_shader_demote_to_helper_invocation_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_demote_to_helper_invocation_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_NV_DEVICE_GENERATED_COMMANDS_EXTENSION_NAME)) {
                    pdd.physical_device_device_generated_commands_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_device_generated_commands_properties_);

                    pdd.physical_device_device_generated_commands_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_device_generated_commands_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_NV_INHERITED_VIEWPORT_SCISSOR_EXTENSION_NAME)) {
                    pdd.physical_device_inherited_viewport_scissor_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_inherited_viewport_scissor_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_SHADER_INTEGER_DOT_PRODUCT_EXTENSION_NAME)) {
                    pdd.physical_device_shader_integer_dot_product_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_shader_integer_dot_product_properties_);

                    pdd.physical_device_shader_integer_dot_product_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_integer_dot_product_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME)) {
                    pdd.physical_device_texel_buffer_alignment_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_texel_buffer_alignment_properties_);

                    pdd.physical_device_texel_buffer_alignment_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_texel_buffer_alignment_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_DEVICE_MEMORY_REPORT_EXTENSION_NAME)) {
                    pdd.physical_device_device_memory_report_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_device_memory_report_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_ROBUSTNESS_2_EXTENSION_NAME)) {
                    pdd.physical_device_robustness_2_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_robustness_2_properties_);

                    pdd.physical_device_robustness_2_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_robustness_2_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_CUSTOM_BORDER_COLOR_EXTENSION_NAME)) {
                    pdd.physical_device_custom_border_color_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_custom_border_color_properties_);

                    pdd.physical_device_custom_border_color_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_custom_border_color_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_PRESENT_ID_EXTENSION_NAME)) {
                    pdd.physical_device_present_id_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_present_id_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_PRIVATE_DATA_EXTENSION_NAME)) {
                    pdd.physical_device_private_data_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_private_data_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_PIPELINE_CREATION_CACHE_CONTROL_EXTENSION_NAME)) {
                    pdd.physical_device_pipeline_creation_cache_control_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_pipeline_creation_cache_control_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_NV_DEVICE_DIAGNOSTICS_CONFIG_EXTENSION_NAME)) {
                    pdd.physical_device_diagnostics_config_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_diagnostics_config_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME)) {
                    pdd.physical_device_synchronization_2_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_synchronization_2_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_GRAPHICS_PIPELINE_LIBRARY_EXTENSION_NAME)) {
                    pdd.physical_device_graphics_pipeline_library_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_graphics_pipeline_library_properties_);

                    pdd.physical_device_graphics_pipeline_library_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_graphics_pipeline_library_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_AMD_SHADER_EARLY_AND_LATE_FRAGMENT_TESTS_EXTENSION_NAME)) {
                    pdd.physical_device_shader_early_and_late_fragment_tests_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_early_and_late_fragment_tests_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_FRAGMENT_SHADER_BARYCENTRIC_EXTENSION_NAME)) {
                    pdd.physical_device_fragment_shader_barycentric_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_fragment_shader_barycentric_properties_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_EXTENSION_NAME)) {
                    pdd.physical_device_shader_subgroup_uniform_control_flow_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_subgroup_uniform_control_flow_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_ZERO_INITIALIZE_WORKGROUP_MEMORY_EXTENSION_NAME)) {
                    pdd.physical_device_zero_initialize_workgroup_memory_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_zero_initialize_workgroup_memory_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_NV_FRAGMENT_SHADING_RATE_ENUMS_EXTENSION_NAME)) {
                    pdd.physical_device_fragment_shading_rate_enums_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_fragment_shading_rate_enums_properties_);

                    pdd.physical_device_fragment_shading_rate_enums_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_fragment_shading_rate_enums_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_NV_RAY_TRACING_MOTION_BLUR_EXTENSION_NAME)) {
                    pdd.physical_device_ray_tracing_motion_blur_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_ray_tracing_motion_blur_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_YCBCR_2PLANE_444_FORMATS_EXTENSION_NAME)) {
                    pdd.physical_device_ycbcr_2_plane_444_formats_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_ycbcr_2_plane_444_formats_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_FRAGMENT_DENSITY_MAP_2_EXTENSION_NAME)) {
                    pdd.physical_device_fragment_density_map_2_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_fragment_density_map_2_properties_);

                    pdd.physical_device_fragment_density_map_2_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_fragment_density_map_2_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_IMAGE_ROBUSTNESS_EXTENSION_NAME)) {
                    pdd.physical_device_image_robustness_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_image_robustness_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_EXTENSION_NAME)) {
                    pdd.physical_device_workgroup_memory_explicit_layout_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_workgroup_memory_explicit_layout_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_IMAGE_COMPRESSION_CONTROL_EXTENSION_NAME)) {
                    pdd.physical_device_image_compression_control_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_image_compression_control_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_ATTACHMENT_FEEDBACK_LOOP_LAYOUT_EXTENSION_NAME)) {
                    pdd.physical_device_attachment_feedback_loop_layout_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_attachment_feedback_loop_layout_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_4444_FORMATS_EXTENSION_NAME)) {
                    pdd.physical_device_4444_formats_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_4444_formats_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_ARM_RASTERIZATION_ORDER_ATTACHMENT_ACCESS_EXTENSION_NAME)) {
                    pdd.physical_device_rasterization_order_attachment_access_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_rasterization_order_attachment_access_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_RGBA10X6_FORMATS_EXTENSION_NAME)) {
                    pdd.physical_device_rgba_10_x_6_formats_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_rgba_10_x_6_formats_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_VALVE_MUTABLE_DESCRIPTOR_TYPE_EXTENSION_NAME)) {
                    pdd.physical_device_mutable_descriptor_type_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_mutable_descriptor_type_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME)) {
                    pdd.physical_device_vertex_input_dynamic_state_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_vertex_input_dynamic_state_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_PHYSICAL_DEVICE_DRM_EXTENSION_NAME)) {
                    pdd.physical_device_drm_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_drm_properties_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_DEPTH_CLIP_CONTROL_EXTENSION_NAME)) {
                    pdd.physical_device_depth_clip_control_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_depth_clip_control_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_PRIMITIVE_TOPOLOGY_LIST_RESTART_EXTENSION_NAME)) {
                    pdd.physical_device_primitive_topology_list_restart_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_primitive_topology_list_restart_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_HUAWEI_SUBPASS_SHADING_EXTENSION_NAME)) {
                    pdd.physical_device_subpass_shading_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_subpass_shading_properties_);

                    pdd.physical_device_subpass_shading_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_subpass_shading_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_HUAWEI_INVOCATION_MASK_EXTENSION_NAME)) {
                    pdd.physical_device_invocation_mask_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_invocation_mask_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_NV_EXTERNAL_MEMORY_RDMA_EXTENSION_NAME)) {
                    pdd.physical_device_external_memory_rdmafeatures_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_external_memory_rdmafeatures_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME)) {
                    pdd.physical_device_pipeline_properties_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_pipeline_properties_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_MULTISAMPLED_RENDER_TO_SINGLE_SAMPLED_EXTENSION_NAME)) {
                    pdd.physical_device_multisampled_render_to_single_sampled_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_multisampled_render_to_single_sampled_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME)) {
                    pdd.physical_device_extended_dynamic_state_2_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_extended_dynamic_state_2_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_COLOR_WRITE_ENABLE_EXTENSION_NAME)) {
                    pdd.physical_device_color_write_enable_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_color_write_enable_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_PRIMITIVES_GENERATED_QUERY_EXTENSION_NAME)) {
                    pdd.physical_device_primitives_generated_query_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_primitives_generated_query_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_RAY_TRACING_MAINTENANCE_1_EXTENSION_NAME)) {
                    pdd.physical_device_ray_tracing_maintenance_1_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_ray_tracing_maintenance_1_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_GLOBAL_PRIORITY_QUERY_EXTENSION_NAME)) {
                    pdd.physical_device_global_priority_query_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_global_priority_query_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_IMAGE_VIEW_MIN_LOD_EXTENSION_NAME)) {
                    pdd.physical_device_image_view_min_lod_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_image_view_min_lod_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_MULTI_DRAW_EXTENSION_NAME)) {
                    pdd.physical_device_multi_draw_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_multi_draw_properties_);

                    pdd.physical_device_multi_draw_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_multi_draw_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_IMAGE_2D_VIEW_OF_3D_EXTENSION_NAME)) {
                    pdd.physical_device_image_2_dview_of_3_dfeatures_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_image_2_dview_of_3_dfeatures_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_BORDER_COLOR_SWIZZLE_EXTENSION_NAME)) {
                    pdd.physical_device_border_color_swizzle_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_border_color_swizzle_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_PAGEABLE_DEVICE_LOCAL_MEMORY_EXTENSION_NAME)) {
                    pdd.physical_device_pageable_device_local_memory_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_pageable_device_local_memory_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_KHR_MAINTENANCE_4_EXTENSION_NAME)) {
                    pdd.physical_device_maintenance_4_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_maintenance_4_properties_);

                    pdd.physical_device_maintenance_4_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_maintenance_4_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_VALVE_DESCRIPTOR_SET_HOST_MAPPING_EXTENSION_NAME)) {
                    pdd.physical_device_descriptor_set_host_mapping_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_descriptor_set_host_mapping_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_NON_SEAMLESS_CUBE_MAP_EXTENSION_NAME)) {
                    pdd.physical_device_non_seamless_cube_map_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_non_seamless_cube_map_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_QCOM_FRAGMENT_DENSITY_MAP_OFFSET_EXTENSION_NAME)) {
                    pdd.physical_device_fragment_density_map_offset_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_fragment_density_map_offset_properties_);

                    pdd.physical_device_fragment_density_map_offset_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_fragment_density_map_offset_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_NV_LINEAR_COLOR_ATTACHMENT_EXTENSION_NAME)) {
                    pdd.physical_device_linear_color_attachment_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_linear_color_attachment_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_IMAGE_COMPRESSION_CONTROL_SWAPCHAIN_EXTENSION_NAME)) {
                    pdd.physical_device_image_compression_control_swapchain_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_image_compression_control_swapchain_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_QCOM_IMAGE_PROCESSING_EXTENSION_NAME)) {
                    pdd.physical_device_image_processing_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_image_processing_properties_);

                    pdd.physical_device_image_processing_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_image_processing_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_SUBPASS_MERGE_FEEDBACK_EXTENSION_NAME)) {
                    pdd.physical_device_subpass_merge_feedback_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_subpass_merge_feedback_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_EXT_SHADER_MODULE_IDENTIFIER_EXTENSION_NAME)) {
                    pdd.physical_device_shader_module_identifier_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_shader_module_identifier_properties_);

                    pdd.physical_device_shader_module_identifier_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_module_identifier_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_QCOM_TILE_PROPERTIES_EXTENSION_NAME)) {
                    pdd.physical_device_tile_properties_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_tile_properties_features_);
                }

                if (PhysicalDeviceData::HasExtension(&pdd, VK_SEC_AMIGO_PROFILING_EXTENSION_NAME)) {
                    pdd.physical_device_amigo_profiling_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_amigo_profiling_features_);
                }

                if (api_version_above_1_1) {
                    pdd.physical_device_subgroup_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_subgroup_properties_);
                }

                if (api_version_above_1_1) {
                    pdd.physical_device_protected_memory_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_protected_memory_properties_);
                }

                if (api_version_above_1_2) {
                    pdd.physical_device_vulkan_11_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_vulkan_11_properties_);
                }

                if (api_version_above_1_2) {
                    pdd.physical_device_vulkan_12_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_vulkan_12_properties_);
                }

                if (api_version_above_1_3) {
                    pdd.physical_device_vulkan_13_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_vulkan_13_properties_);
                }

                if (api_version_above_1_1) {
                    pdd.physical_device_protected_memory_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_protected_memory_features_);
                }

                if (api_version_above_1_1) {
                    pdd.physical_device_shader_draw_parameter_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_draw_parameter_features_);
                }

                if (api_version_above_1_2) {
                    pdd.physical_device_vulkan_11_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_vulkan_11_features_);
                }

                if (api_version_above_1_2) {
                    pdd.physical_device_vulkan_12_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_vulkan_12_features_);
                }

                if (api_version_above_1_3) {
                    pdd.physical_device_vulkan_13_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_vulkan_13_features_);
                }

                if (VK_API_VERSION_MINOR(pdd.GetEffectiveVersion())) {
                    dt->GetPhysicalDeviceProperties2(physical_device, &property_chain);
                    dt->GetPhysicalDeviceFeatures2(physical_device, &feature_chain);
                    dt->GetPhysicalDeviceMemoryProperties2(physical_device, &memory_chain);
                } else {
                    dt->GetPhysicalDeviceProperties2KHR(physical_device, &property_chain);
                    dt->GetPhysicalDeviceFeatures2KHR(physical_device, &feature_chain);
                    dt->GetPhysicalDeviceMemoryProperties2KHR(physical_device, &memory_chain);
                }

                pdd.physical_device_properties_ = property_chain.properties;
                pdd.physical_device_features_ = feature_chain.features;
                pdd.physical_device_memory_properties_ = memory_chain.memoryProperties;
            }

            ::device_has_astc = pdd.physical_device_features_.textureCompressionASTC_LDR;
            ::device_has_bc = pdd.physical_device_features_.textureCompressionBC;
            ::device_has_etc2 = pdd.physical_device_features_.textureCompressionETC2;

            if (layer_settings->simulate_capabilities & SIMULATE_FORMATS_BIT) {
                LoadDeviceFormats(instance, &pdd, physical_device, &pdd.device_formats_, &pdd.device_formats_3_);
            }
            if (layer_settings->simulate_capabilities & SIMULATE_QUEUE_FAMILY_PROPERTIES_BIT) {
                LoadQueueFamilyProperties(instance, physical_device, &pdd);
            }

            LogMessage(DEBUG_REPORT_NOTIFICATION_BIT,
                       format("Running on \"%s\" with Vulkan %d.%d.%d driver.\n", pdd.physical_device_properties_.deviceName,
                              VK_API_VERSION_MAJOR(pdd.physical_device_properties_.apiVersion),
                              VK_API_VERSION_MINOR(pdd.physical_device_properties_.apiVersion),
                              VK_API_VERSION_PATCH(pdd.physical_device_properties_.apiVersion)));

            // Override PDD members with values from configuration file(s).
            if (result == VK_SUCCESS) {
                JsonLoader &json_loader = *JsonLoader::Find(instance);
                result = json_loader.LoadDevice(&pdd);
            }

            // VK_VULKAN_1_1
            TransferValue(&(pdd.physical_device_vulkan_11_properties_), &(pdd.physical_device_multiview_properties_), pdd.vulkan_1_1_properties_written_);
            TransferValue(&(pdd.physical_device_vulkan_11_features_), &(pdd.physical_device_multiview_features_), pdd.vulkan_1_1_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_11_properties_), &(pdd.physical_device_idproperties_), pdd.vulkan_1_1_properties_written_);
            TransferValue(&(pdd.physical_device_vulkan_11_features_), &(pdd.physical_device_16_bit_storage_features_), pdd.vulkan_1_1_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_11_properties_), &(pdd.physical_device_point_clipping_properties_), pdd.vulkan_1_1_properties_written_);
            TransferValue(&(pdd.physical_device_vulkan_11_features_), &(pdd.physical_device_variable_pointer_features_), pdd.vulkan_1_1_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_11_features_), &(pdd.physical_device_sampler_ycbcr_conversion_features_), pdd.vulkan_1_1_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_11_properties_), &(pdd.physical_device_maintenance_3_properties_), pdd.vulkan_1_1_properties_written_);

            // VK_VULKAN_1_2
            TransferValue(&(pdd.physical_device_vulkan_12_features_), &(pdd.physical_device_float_16_int_8_features_), pdd.vulkan_1_2_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_12_features_), &(pdd.physical_device_imageless_framebuffer_features_), pdd.vulkan_1_2_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_12_properties_), &(pdd.physical_device_sampler_filter_minmax_properties_), pdd.vulkan_1_2_properties_written_);
            TransferValue(&(pdd.physical_device_vulkan_12_properties_), &(pdd.physical_device_descriptor_indexing_properties_), pdd.vulkan_1_2_properties_written_);
            TransferValue(&(pdd.physical_device_vulkan_12_features_), &(pdd.physical_device_descriptor_indexing_features_), pdd.vulkan_1_2_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_12_features_), &(pdd.physical_device_shader_subgroup_extended_types_features_), pdd.vulkan_1_2_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_12_features_), &(pdd.physical_device_8_bit_storage_features_), pdd.vulkan_1_2_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_12_features_), &(pdd.physical_device_shader_atomic_int_64_features_), pdd.vulkan_1_2_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_12_properties_), &(pdd.physical_device_driver_properties_), pdd.vulkan_1_2_properties_written_);
            TransferValue(&(pdd.physical_device_vulkan_12_properties_), &(pdd.physical_device_float_controls_properties_), pdd.vulkan_1_2_properties_written_);
            TransferValue(&(pdd.physical_device_vulkan_12_properties_), &(pdd.physical_device_depth_stencil_resolve_properties_), pdd.vulkan_1_2_properties_written_);
            TransferValue(&(pdd.physical_device_vulkan_12_properties_), &(pdd.physical_device_timeline_semaphore_properties_), pdd.vulkan_1_2_properties_written_);
            TransferValue(&(pdd.physical_device_vulkan_12_features_), &(pdd.physical_device_timeline_semaphore_features_), pdd.vulkan_1_2_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_12_features_), &(pdd.physical_device_vulkan_memory_model_features_), pdd.vulkan_1_2_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_12_features_), &(pdd.physical_device_scalar_block_layout_features_), pdd.vulkan_1_2_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_12_features_), &(pdd.physical_device_separate_depth_stencil_layouts_features_), pdd.vulkan_1_2_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_12_features_), &(pdd.physical_device_uniform_buffer_standard_layout_features_), pdd.vulkan_1_2_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_12_features_), &(pdd.physical_device_buffer_device_address_features_), pdd.vulkan_1_2_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_12_features_), &(pdd.physical_device_host_query_reset_features_), pdd.vulkan_1_2_features_written_);

            // VK_VULKAN_1_3
            TransferValue(&(pdd.physical_device_vulkan_13_features_), &(pdd.physical_device_dynamic_rendering_features_), pdd.vulkan_1_3_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_13_features_), &(pdd.physical_device_texture_compression_astchdrfeatures_), pdd.vulkan_1_3_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_13_properties_), &(pdd.physical_device_inline_uniform_block_properties_), pdd.vulkan_1_3_properties_written_);
            TransferValue(&(pdd.physical_device_vulkan_13_features_), &(pdd.physical_device_inline_uniform_block_features_), pdd.vulkan_1_3_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_13_features_), &(pdd.physical_device_shader_terminate_invocation_features_), pdd.vulkan_1_3_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_13_properties_), &(pdd.physical_device_subgroup_size_control_properties_), pdd.vulkan_1_3_properties_written_);
            TransferValue(&(pdd.physical_device_vulkan_13_features_), &(pdd.physical_device_subgroup_size_control_features_), pdd.vulkan_1_3_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_13_features_), &(pdd.physical_device_shader_demote_to_helper_invocation_features_), pdd.vulkan_1_3_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_13_properties_), &(pdd.physical_device_shader_integer_dot_product_properties_), pdd.vulkan_1_3_properties_written_);
            TransferValue(&(pdd.physical_device_vulkan_13_features_), &(pdd.physical_device_shader_integer_dot_product_features_), pdd.vulkan_1_3_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_13_properties_), &(pdd.physical_device_texel_buffer_alignment_properties_), pdd.vulkan_1_3_properties_written_);
            TransferValue(&(pdd.physical_device_vulkan_13_features_), &(pdd.physical_device_private_data_features_), pdd.vulkan_1_3_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_13_features_), &(pdd.physical_device_pipeline_creation_cache_control_features_), pdd.vulkan_1_3_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_13_features_), &(pdd.physical_device_synchronization_2_features_), pdd.vulkan_1_3_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_13_features_), &(pdd.physical_device_zero_initialize_workgroup_memory_features_), pdd.vulkan_1_3_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_13_features_), &(pdd.physical_device_image_robustness_features_), pdd.vulkan_1_3_features_written_);
            TransferValue(&(pdd.physical_device_vulkan_13_properties_), &(pdd.physical_device_maintenance_4_properties_), pdd.vulkan_1_3_properties_written_);
            TransferValue(&(pdd.physical_device_vulkan_13_features_), &(pdd.physical_device_maintenance_4_features_), pdd.vulkan_1_3_features_written_);

            if (layer_settings->simulate_capabilities & SIMULATE_EXTENSIONS_BIT) {
                pdd.simulation_extensions_ = pdd.arrayof_extension_properties_;
            } else {
                pdd.simulation_extensions_ = pdd.device_extensions_;
            }

            for (std::size_t j = 0, m = layer_settings->exclude_device_extensions.size(); j < m; ++j) {
                const std::string &extension = layer_settings->exclude_device_extensions[j];
                if (extension.empty()) continue;

                for (size_t i = 0; i < pdd.simulation_extensions_.size(); ++i) {
                    if (extension == pdd.simulation_extensions_[i].extensionName) {
                        pdd.simulation_extensions_.erase(pdd.simulation_extensions_.begin() + i);
                        break;
                    }
                }
            }
        }
    }

    LogFlush();

    return result;
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetInstanceProcAddr(VkInstance instance, const char *pName) {
// Apply the DRY principle, see https://en.wikipedia.org/wiki/Don%27t_repeat_yourself
#define GET_PROC_ADDR(func) \
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

}  // anonymous namespace

// Function symbols statically exported by this layer's library //////////////////////////////////////////////////////////////////
// Keep synchronized with VisualStudio's VkLayer_khronos_profiles.def

VK_LAYER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(VkInstance instance, const char *pName) {
    return GetInstanceProcAddr(instance, pName);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateInstance(const VkInstanceCreateInfo *pCreateInfo,
                                                                const VkAllocationCallbacks *pAllocator, VkInstance *pInstance) {
    return CreateInstance(pCreateInfo, pAllocator, pInstance);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceLayerProperties(uint32_t *pCount,
                                                                                  VkLayerProperties *pProperties) {
    return EnumerateInstanceLayerProperties(pCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceExtensionProperties(const char *pLayerName, uint32_t *pCount,
                                                                                      VkExtensionProperties *pProperties) {
    return EnumerateInstanceExtensionProperties(pLayerName, pCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumeratePhysicalDevices(VkInstance instance, uint32_t *pPhysicalDeviceCount,
                                                                          VkPhysicalDevice *pPhysicalDevices) {
    return EnumeratePhysicalDevices(instance, pPhysicalDeviceCount, pPhysicalDevices);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkNegotiateLoaderLayerInterfaceVersion(VkNegotiateLayerInterface *pVersionStruct) {
    assert(pVersionStruct != NULL);
    assert(pVersionStruct->sType == LAYER_NEGOTIATE_INTERFACE_STRUCT);

    if (pVersionStruct->loaderLayerInterfaceVersion > CURRENT_LOADER_LAYER_INTERFACE_VERSION) {
        // Loader is requesting newer interface version; reduce to the version we support.
        pVersionStruct->loaderLayerInterfaceVersion = CURRENT_LOADER_LAYER_INTERFACE_VERSION;
    } else if (pVersionStruct->loaderLayerInterfaceVersion < CURRENT_LOADER_LAYER_INTERFACE_VERSION) {
        // Loader is requesting older interface version; record the Loader's version
        loader_layer_iface_version = pVersionStruct->loaderLayerInterfaceVersion;
    }

    if (pVersionStruct->loaderLayerInterfaceVersion >= 2) {
        pVersionStruct->pfnGetInstanceProcAddr = vkGetInstanceProcAddr;
        pVersionStruct->pfnGetDeviceProcAddr = nullptr;
        pVersionStruct->pfnGetPhysicalDeviceProcAddr = nullptr;
    }

    return VK_SUCCESS;
}

// vim: set sw=4 ts=8 et ic ai:
