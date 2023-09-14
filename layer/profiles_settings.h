/*
 * Copyright (C) 2022-2023 Valve Corporation
 * Copyright (C) 2022-2023 LunarG, Inc.
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
 * Author: Christophe Riccio <christophe@lunarg.com>
 */

#pragma once

#include <vulkan/layer/vk_layer_settings.hpp>
#include <memory>
#include <vector>
#include <string>

enum SettingId {
    SETTING_PROFILE_FILE = 0,
    SETTING_PROFILE_NAME,
    SETTING_PROFILE_VALIDATION,
    SETTING_EMULATE_PORTABILITY,
    SETTING_CONTANT_ALPHA_COLOR_BLEND_FACTORS,
    SETTING_EVENTS,
    SETTING_IMAGE_VIEW_FORMAT_REINTERPRETATION,
    SETTING_IMAGE_VIEW_FORMAT_SWIZZLE,
    SETTING_IMAGE_VIEW_2D_ON_3D_IMAGE,
    SETTING_MULTISAMPLE_ARRAY_IMAGE,
    SETTING_MUTABLE_COMPARISON_SAMPLERS,
    SETTING_POINT_POLYGONS,
    SETTING_SAMPLER_MIP_LOD_BIAS,
    SETTING_SEPARATE_STENCIL_MASK_REF,
    SETTING_SEPARATE_SAMPLE_RATE_INTERPOLATION_FUNCTIONS,
    SETTING_TESSELLATION_ISOLINES,
    SETTING_TESSELLATION_POINT_MODE,
    SETTING_TRIANGLE_FANS,
    SETTING_VERTEX_ATTRIBUTE_ACCESS_BEYOND_STRIDE,
    SETTING_MIN_VERTEX_INPUT_BINDING_STRIDE_ALIGMENT,
    SETTING_SIMULATE_CAPABILITIES,
    SETTING_DEBUG_ACTIONS,
    SETTING_DEBUG_FILENAME,
    SETTING_DEBUG_FILE_CLEAR,
    SETTING_DEBUG_FAIL_ON_ERROR,
    SETTING_DEBUG_REPORTS,
    SETTING_EXCLUDE_DEVICE_EXTENSIONS,
    SETTING_EXCLUDE_FORMATS,
    SETTING_DEFAULT_FEATURE_VALUES,
    SETTING_FORCE_DEVICE,
    SETTING_FORCE_DEVICE_UUID,
    SETTING_FORCE_DEVICE_NAME,

    SETTING_FIRST = SETTING_PROFILE_FILE,
    SETTING_LAST = SETTING_FORCE_DEVICE_NAME
};

enum { SETTING_COUNT = SETTING_LAST - SETTING_FIRST + 1 };

static const char *kSettingNames[] = {"profile_file",
                                      "profile_name",
                                      "profile_validation",
                                      "emulate_portability",
                                      "constantAlphaColorBlendFactors",
                                      "events",
                                      "imageViewFormatReinterpretation",
                                      "imageViewFormatSwizzle",
                                      "imageView2DOn3DImage",
                                      "multisampleArrayImage",
                                      "mutableComparisonSamplers",
                                      "pointPolygons",
                                      "samplerMipLodBias",
                                      "separateStencilMaskRef",
                                      "shaderSampleRateInterpolationFunctions",
                                      "tessellationIsolines",
                                      "tessellationPointMode",
                                      "triangleFans",
                                      "vertexAttributeAccessBeyondStride",
                                      "minVertexInputBindingStrideAlignment",
                                      "simulate_capabilities",
                                      "debug_actions",
                                      "debug_filename",
                                      "debug_file_clear",
                                      "debug_fail_on_error",
                                      "debug_reports",
                                      "exclude_device_extensions",
                                      "exclude_formats",
                                      "default_feature_values",
                                      "force_device",
                                      "force_device_uuid",
                                      "force_device_name"};
static_assert(std::size(kSettingNames) == SETTING_COUNT);

inline const char *GetString(SettingId setting_id) { return kSettingNames[setting_id]; }

enum SimulateCapabilityBits {
    SIMULATE_API_VERSION_BIT = 1 << 0,
    SIMULATE_FEATURES_BIT = 1 << 1,
    SIMULATE_PROPERTIES_BIT = 1 << 2,
    SIMULATE_EXTENSIONS_BIT = 1 << 3,
    SIMULATE_FORMATS_BIT = 1 << 4,
    SIMULATE_QUEUE_FAMILY_PROPERTIES_BIT = 1 << 5,
    SIMULATE_MAX_ENUM = 0x7FFFFFFF
};
typedef int SimulateCapabilityFlags;

enum DebugActionBits {
    DEBUG_ACTION_FILE_BIT = (1 << 0),
    DEBUG_ACTION_STDOUT_BIT = (1 << 1),
    DEBUG_ACTION_OUTPUT_BIT = (1 << 2),
    DEBUG_ACTION_BREAKPOINT_BIT = (1 << 3),
    DEBUG_ACTION_MAX_ENUM = 0x7FFFFFFF
};
typedef int DebugActionFlags;

inline DebugActionFlags GetDebugActionFlags(const std::vector<std::string> &values) {
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
        } else if (values[i] == "DEBUG_ACTION_MAX_ENUM") {
            result = DEBUG_ACTION_MAX_ENUM;
        }
    }

    return result;
}

inline std::vector<std::string> GetDebugActionStrings(DebugActionFlags flags) {
    static const char *table[] = {"DEBUG_ACTION_FILE_BIT", "DEBUG_ACTION_STDOUT_BIT", "DEBUG_ACTION_OUTPUT_BIT",
                                  "DEBUG_ACTION_BREAKPOINT_BIT"};

    std::vector<std::string> result;

    for (std::size_t i = 0, n = std::size(table); i < n; ++i) {
        if (flags & (1 << i)) {
            result.push_back(table[i]);
        }
    }

    return result;
}

std::string GetDebugActionsLog(DebugActionFlags flags);

enum DebugReportBits {
    DEBUG_REPORT_NOTIFICATION_BIT = (1 << 0),
    DEBUG_REPORT_WARNING_BIT = (1 << 1),
    DEBUG_REPORT_ERROR_BIT = (1 << 2),
    DEBUG_REPORT_DEBUG_BIT = (1 << 3),
    DEBUG_REPORT_MAX_ENUM = 0x7FFFFFFF
};
typedef int DebugReportFlags;

inline DebugReportFlags GetDebugReportFlags(const std::vector<std::string> &values) {
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
        } else if (values[i] == "DEBUG_REPORT_MAX_ENUM") {
            result = DEBUG_REPORT_MAX_ENUM;
        }
    }

    return result;
}

enum DefaultFeatureValues {
    DEFAULT_FEATURE_VALUES_FALSE = 0,
    DEFAULT_FEATURE_VALUES_DEVICE
};

inline DefaultFeatureValues GetDefaultFeatureValues(const std::string &value) {
    if (value == "DEFAULT_FEATURE_VALUES_FALSE") {
        return DEFAULT_FEATURE_VALUES_FALSE;
    } else if (value == "DEFAULT_FEATURE_VALUES_DEVICE") {
        return DEFAULT_FEATURE_VALUES_DEVICE;
    }

    return DEFAULT_FEATURE_VALUES_DEVICE;
}

inline std::string GetDefaultFeatureValuesString(DefaultFeatureValues value) {
    if (value == DEFAULT_FEATURE_VALUES_FALSE) {
        return "DEFAULT_FEATURE_VALUES_FALSE";
    } else if (value == DEFAULT_FEATURE_VALUES_DEVICE) {
        return "DEFAULT_FEATURE_VALUES_DEVICE";
    }

    return "DEFAULT_FEATURE_VALUES_DEVICE";
}

enum ForceDevice {
    FORCE_DEVICE_OFF = 0,
    FORCE_DEVICE_WITH_UUID,
    FORCE_DEVICE_WITH_NAME
};

struct ProfileLayerSettings {
    ~ProfileLayerSettings() {
        if (log.profiles_log_file != nullptr) {
            fclose(log.profiles_log_file);
            log.profiles_log_file = nullptr;
        }
    }

    struct Simulate {
        std::string profile_file{};
        std::string profile_name{"${VP_DEFAULT}"};
        bool profile_validation{false};
        SimulateCapabilityFlags capabilities{SIMULATE_API_VERSION_BIT | SIMULATE_FEATURES_BIT | SIMULATE_PROPERTIES_BIT};
        DefaultFeatureValues default_feature_values{DEFAULT_FEATURE_VALUES_DEVICE};
        std::vector<std::string> exclude_device_extensions;
        std::vector<std::string> exclude_formats;
        bool emulate_portability{true};
    } simulate;

    struct Portability {
        bool constantAlphaColorBlendFactors{false};
        bool events{false};
        bool imageViewFormatReinterpretation{false};
        bool imageViewFormatSwizzle{false};
        bool imageView2DOn3DImage{false};
        bool multisampleArrayImage{false};
        bool mutableComparisonSamplers{false};
        bool pointPolygons{false};
        bool samplerMipLodBias{false};
        bool separateStencilMaskRef{false};
        bool shaderSampleRateInterpolationFunctions{false};
        bool tessellationIsolines{false};
        bool tessellationPointMode{false};
        bool triangleFans{false};
        bool vertexAttributeAccessBeyondStride{false};
        uint32_t minVertexInputBindingStrideAlignment{4};
    } portability;

    struct Log {
        DebugActionFlags debug_actions{DEBUG_ACTION_STDOUT_BIT};
        std::string debug_filename{"profiles_layer_log.txt"};
        bool debug_file_discard{true};
        DebugReportFlags debug_reports{DEBUG_REPORT_WARNING_BIT | DEBUG_REPORT_ERROR_BIT};
        bool debug_fail_on_error{false};
        FILE *profiles_log_file{nullptr};
    } log;

    struct Device {
        ForceDevice force_device{FORCE_DEVICE_OFF};
        std::string force_device_uuid;
        std::string force_device_name;
    } device;
};

void InitProfilesLayerSettings(const VkInstanceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator,
                               ProfileLayerSettings *layer_settings);

void WarnMissingFormatFeatures(ProfileLayerSettings* layer_settings, const char *device_name, const std::string &format_name,
                               const std::string &features,
                               VkFormatFeatureFlags profile_features, VkFormatFeatureFlags device_features);

void WarnMissingFormatFeatures2(ProfileLayerSettings *layer_settings, const char *device_name, const std::string &format_name,
                                const std::string &features,
                                VkFormatFeatureFlags2 profile_features, VkFormatFeatureFlags2 device_features);

void LogMessage(ProfileLayerSettings *layer_settings, DebugReportBits report, const char *message, ...);

void LogFlush(ProfileLayerSettings *layer_settings);

