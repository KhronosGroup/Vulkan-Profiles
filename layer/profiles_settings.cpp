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

#include "profiles_settings.h"
#include "profiles_util.h"

void WarnMissingFormatFeatures(ProfileLayerSettings *layer_settings, const char *device_name, const std::string &format_name,
                               const std::string &features, VkFormatFeatureFlags profile_features,
                               VkFormatFeatureFlags device_features) {
    if (!(layer_settings->log.debug_reports & DEBUG_REPORT_WARNING_BIT)) {
        return;
    }

    LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
               "For %s `%s`,\nthe Profile requires:\n\t\"%s\"\nbut the Device (%s) %s.\nThe "
               "`%s` can't be simulated on this Device.\n",
               format_name.c_str(), features.c_str(), GetFormatFeatureString(profile_features).c_str(), device_name,
               format_device_support_string(device_features).c_str(), features.c_str());
}

void WarnMissingFormatFeatures2(ProfileLayerSettings *layer_settings, const char *device_name, const std::string &format_name,
                                const std::string &features, VkFormatFeatureFlags2 profile_features,
                                VkFormatFeatureFlags2 device_features) {
    if (!(layer_settings->log.debug_reports & DEBUG_REPORT_WARNING_BIT)) {
        return;
    }

    LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
               "For %s `%s`,\nthe Profile requires:\n\\t\"%s\"\nbut the Device (%s) %s.\nThe "
               "`%s` can't be simulated on this Device.\n",
               format_name.c_str(), features.c_str(), GetFormatFeature2String(profile_features).c_str(), device_name,
               format_device_support_string(device_features).c_str(), features.c_str());
}

std::string GetDebugActionsLog(DebugActionFlags flags) {
    std::string result = {};

    std::vector<std::string> tmp = GetDebugActionStrings(flags);

    for (std::size_t i = 0, n = tmp.size(); i < n; ++i) {
        if (!result.empty()) {
            result += ", ";
        }
        result += tmp[i];
    }

    return result;
}

#if defined(__ANDROID__)

#include <android/log.h>

void AndroidPrintf(DebugReportBits level, const std::string &message) {
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

const char *GetLogPrefix(DebugReportBits report) {
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

void LogMessage(ProfileLayerSettings *layer_settings, DebugReportBits report, const char *message, ...) {
#if defined(__ANDROID__)
    if (!layer_settings) return;
#else
    assert(layer_settings);
#endif

    if (!(layer_settings->log.debug_reports & report)) return;

    std::size_t const STRING_BUFFER(4096);

    assert(message != nullptr);
    assert(strlen(message) < STRING_BUFFER);

    char log[STRING_BUFFER];
    snprintf(log, STRING_BUFFER, "%s", GetLogPrefix(report));

    va_list list;

    va_start(list, message);
    vsnprintf(log, STRING_BUFFER, message, list);
    va_end(list);

    if (layer_settings->log.debug_actions & DEBUG_ACTION_STDOUT_BIT) {
#if defined(__ANDROID__)
        AndroidPrintf(report, message);
#else
        fprintf(stdout, "%s", log);
#endif
    }

    if (layer_settings->log.debug_actions & DEBUG_ACTION_FILE_BIT) {
        fprintf(layer_settings->log.profiles_log_file, "%s", log);
    }

#if _WIN32
    if (layer_settings->log.debug_actions & DEBUG_ACTION_OUTPUT_BIT) {
        OutputDebugString(log);
    }
#endif  //_WIN32

    if (layer_settings->log.debug_actions & DEBUG_ACTION_BREAKPOINT_BIT) {
#ifdef WIN32
        DebugBreak();
#else
        raise(SIGTRAP);
#endif
    }
}

void LogFlush(ProfileLayerSettings *layer_settings) {
#if defined(__ANDROID__)
    if (!layer_settings) return;
#else
    assert(layer_settings);
#endif

    if (layer_settings->log.debug_actions & DEBUG_ACTION_STDOUT_BIT) {
        std::fflush(stdout);
    }
    if (layer_settings->log.debug_actions & DEBUG_ACTION_FILE_BIT) {
        std::fflush(layer_settings->log.profiles_log_file);
    }
}

static ForceDevice GetForceDevice(const std::string &value) {
    if (value == "FORCE_DEVICE_OFF") {
        return FORCE_DEVICE_OFF;
    } else if (value == "FORCE_DEVICE_WITH_UUID") {
        return FORCE_DEVICE_WITH_UUID;
    } else if (value == "FORCE_DEVICE_WITH_NAME") {
        return FORCE_DEVICE_WITH_NAME;
    }

    return FORCE_DEVICE_OFF;
}

void InitProfilesLayerSettings(const VkInstanceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator,
                               ProfileLayerSettings *layer_settings) {
    assert(layer_settings != nullptr);

    const VkLayerSettingsCreateInfoEXT *create_info = vlFindLayerSettingsCreateInfo(pCreateInfo);

    VlLayerSettingSet layerSettingSet = VK_NULL_HANDLE;
    vlCreateLayerSettingSet(kLayerName, create_info, pAllocator, nullptr, &layerSettingSet);

    uint32_t unknown_setting_count = 0;
    vlGetUnknownSettings(create_info, static_cast<uint32_t>(SETTING_COUNT), kSettingNames, &unknown_setting_count, nullptr);

    if (unknown_setting_count > 0) {
        std::vector<const char *> unknown_settings;
        unknown_settings.resize(unknown_setting_count);

        vlGetUnknownSettings(create_info, static_cast<uint32_t>(SETTING_COUNT), kSettingNames, &unknown_setting_count,
                             &unknown_settings[0]);

        for (std::size_t i = 0, n = unknown_settings.size(); i < n; ++i) {
            LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                       "Unknown %s setting listed in VkLayerSettingsCreateInfoEXT, This setting is ignored.\n", unknown_settings[i]);
        }
    }

    const char *setting_profile_file = GetString(SETTING_PROFILE_FILE);
    if (vlHasLayerSetting(layerSettingSet, setting_profile_file)) {
        vlGetLayerSettingValue(layerSettingSet, setting_profile_file, layer_settings->simulate.profile_file);

        const char *setting_profile_name = GetString(SETTING_PROFILE_NAME);
        if (vlHasLayerSetting(layerSettingSet, setting_profile_name)) {
            vlGetLayerSettingValue(layerSettingSet, setting_profile_name, layer_settings->simulate.profile_name);
        }

        const char *setting_profile_validation = GetString(SETTING_PROFILE_VALIDATION);
        if (vlHasLayerSetting(layerSettingSet, setting_profile_validation)) {
            vlGetLayerSettingValue(layerSettingSet, setting_profile_validation, layer_settings->simulate.profile_validation);
        }

        const char *setting_simulate_caps = GetString(SETTING_SIMULATE_CAPABILITIES);
        if (vlHasLayerSetting(layerSettingSet, setting_simulate_caps)) {
            std::vector<std::string> values;
            vlGetLayerSettingValues(layerSettingSet, setting_simulate_caps, values);
            for (std::size_t i = 0, n = values.size(); i < n; ++i) {
                values[i] = ToUpper(values[i]);
            }
            layer_settings->simulate.capabilities = GetSimulateCapabilityFlags(values);
        }

        const char *setting_default_feature_values = GetString(SETTING_DEFAULT_FEATURE_VALUES);
        if (vlHasLayerSetting(layerSettingSet, setting_default_feature_values)) {
            std::string value;
            vlGetLayerSettingValue(layerSettingSet, setting_default_feature_values, value);
            layer_settings->simulate.default_feature_values = GetDefaultFeatureValues(ToUpper(value));
        }
    }

    const char *setting_exclude_device_ext = GetString(SETTING_EXCLUDE_DEVICE_EXTENSIONS);
    if (vlHasLayerSetting(layerSettingSet, setting_exclude_device_ext)) {
        vlGetLayerSettingValues(layerSettingSet, setting_exclude_device_ext,
                                layer_settings->simulate.exclude_device_extensions);
    }

    const char *setting_exclude_formats = GetString(SETTING_EXCLUDE_FORMATS);
    if (vlHasLayerSetting(layerSettingSet, setting_exclude_formats)) {
        vlGetLayerSettingValues(layerSettingSet, setting_exclude_formats, layer_settings->simulate.exclude_formats);
    }

    const char *setting_emulate_port = GetString(SETTING_EMULATE_PORTABILITY);
    if (vlHasLayerSetting(layerSettingSet, setting_emulate_port)) {
        vlGetLayerSettingValue(layerSettingSet, setting_emulate_port, layer_settings->simulate.emulate_portability);

        const char *setting_contant_factors = GetString(SETTING_CONTANT_ALPHA_COLOR_BLEND_FACTORS);
        if (vlHasLayerSetting(layerSettingSet, setting_contant_factors)) {
            vlGetLayerSettingValue(layerSettingSet, setting_contant_factors,
                                   layer_settings->portability.constantAlphaColorBlendFactors);
        }

        const char *setting_events = GetString(SETTING_EVENTS);
        if (vlHasLayerSetting(layerSettingSet, setting_events)) {
            vlGetLayerSettingValue(layerSettingSet, setting_events, layer_settings->portability.events);
        }

        const char *setting_image_view_reinterp = GetString(SETTING_IMAGE_VIEW_FORMAT_REINTERPRETATION);
        if (vlHasLayerSetting(layerSettingSet, setting_image_view_reinterp)) {
            vlGetLayerSettingValue(layerSettingSet, setting_image_view_reinterp,
                                   layer_settings->portability.imageViewFormatReinterpretation);
        }

        const char *setting_image_view_swizzle = GetString(SETTING_IMAGE_VIEW_FORMAT_SWIZZLE);
        if (vlHasLayerSetting(layerSettingSet, setting_image_view_swizzle)) {
            vlGetLayerSettingValue(layerSettingSet, setting_image_view_swizzle,
                                   layer_settings->portability.imageViewFormatSwizzle);
        }

        const char *setting_image_view_2d_on_3d = GetString(SETTING_IMAGE_VIEW_2D_ON_3D_IMAGE);
        if (vlHasLayerSetting(layerSettingSet, setting_image_view_2d_on_3d)) {
            vlGetLayerSettingValue(layerSettingSet, setting_image_view_2d_on_3d,
                                   layer_settings->portability.imageView2DOn3DImage);
        }

        const char *setting_multisample_array = GetString(SETTING_MULTISAMPLE_ARRAY_IMAGE);
        if (vlHasLayerSetting(layerSettingSet, setting_multisample_array)) {
            vlGetLayerSettingValue(layerSettingSet, setting_multisample_array,
                                   layer_settings->portability.multisampleArrayImage);
        }

        const char *setting_mutable_comparison_samplers = GetString(SETTING_MUTABLE_COMPARISON_SAMPLERS);
        if (vlHasLayerSetting(layerSettingSet, setting_mutable_comparison_samplers)) {
            vlGetLayerSettingValue(layerSettingSet, setting_mutable_comparison_samplers,
                                   layer_settings->portability.mutableComparisonSamplers);
        }

        const char *setting_point_poly = GetString(SETTING_POINT_POLYGONS);
        if (vlHasLayerSetting(layerSettingSet, setting_point_poly)) {
            vlGetLayerSettingValue(layerSettingSet, setting_point_poly, layer_settings->portability.pointPolygons);
        }

        const char *setting_sampler_mip_lod_bias = GetString(SETTING_SAMPLER_MIP_LOD_BIAS);
        if (vlHasLayerSetting(layerSettingSet, setting_sampler_mip_lod_bias)) {
            vlGetLayerSettingValue(layerSettingSet, setting_sampler_mip_lod_bias,
                                   layer_settings->portability.samplerMipLodBias);
        }

        const char *setting_separate_stencil_mask_ref = GetString(SETTING_SEPARATE_STENCIL_MASK_REF);
        if (vlHasLayerSetting(layerSettingSet, setting_separate_stencil_mask_ref)) {
            vlGetLayerSettingValue(layerSettingSet, setting_separate_stencil_mask_ref,
                                   layer_settings->portability.separateStencilMaskRef);
        }

        const char *setting_separate_sample_rate = GetString(SETTING_SEPARATE_SAMPLE_RATE_INTERPOLATION_FUNCTIONS);
        if (vlHasLayerSetting(layerSettingSet, setting_separate_sample_rate)) {
            vlGetLayerSettingValue(layerSettingSet, setting_separate_sample_rate,
                                   layer_settings->portability.shaderSampleRateInterpolationFunctions);
        }

        const char *setting_tessellation_isolines = GetString(SETTING_TESSELLATION_ISOLINES);
        if (vlHasLayerSetting(layerSettingSet, setting_tessellation_isolines)) {
            vlGetLayerSettingValue(layerSettingSet, setting_tessellation_isolines,
                                   layer_settings->portability.tessellationIsolines);
        }

        const char *setting_tessellation_point_mode = GetString(SETTING_TESSELLATION_POINT_MODE);
        if (vlHasLayerSetting(layerSettingSet, setting_tessellation_point_mode)) {
            vlGetLayerSettingValue(layerSettingSet, setting_tessellation_point_mode,
                                   layer_settings->portability.tessellationPointMode);
        }

        const char *setting_triangle_fans = GetString(SETTING_TRIANGLE_FANS);
        if (vlHasLayerSetting(layerSettingSet, setting_triangle_fans)) {
            vlGetLayerSettingValue(layerSettingSet, setting_triangle_fans, layer_settings->portability.triangleFans);
        }

        const char *setting_vertex_attrib_stride = GetString(SETTING_VERTEX_ATTRIBUTE_ACCESS_BEYOND_STRIDE);
        if (vlHasLayerSetting(layerSettingSet, setting_vertex_attrib_stride)) {
            vlGetLayerSettingValue(layerSettingSet, setting_vertex_attrib_stride,
                                   layer_settings->portability.vertexAttributeAccessBeyondStride);
        }

        const char *setting_min_vertex_attrib_stride = GetString(SETTING_MIN_VERTEX_INPUT_BINDING_STRIDE_ALIGMENT);
        if (vlHasLayerSetting(layerSettingSet, setting_min_vertex_attrib_stride)) {
            vlGetLayerSettingValue(layerSettingSet, setting_min_vertex_attrib_stride,
                                   layer_settings->portability.minVertexInputBindingStrideAlignment);
        }
    }

    const char *setting_force_device = GetString(SETTING_FORCE_DEVICE);
    if (vlHasLayerSetting(layerSettingSet, setting_force_device)) {
        std::string value;
        vlGetLayerSettingValue(layerSettingSet, setting_force_device, value);
        layer_settings->device.force_device = GetForceDevice(ToUpper(value));

        const char *setting_force_device_uuid = GetString(SETTING_FORCE_DEVICE_UUID);
        if (vlHasLayerSetting(layerSettingSet, setting_force_device_uuid)) {
            vlGetLayerSettingValue(layerSettingSet, setting_force_device_uuid, layer_settings->device.force_device_uuid);
        }

        const char *setting_force_device_name = GetString(SETTING_FORCE_DEVICE_NAME);
        if (vlHasLayerSetting(layerSettingSet, setting_force_device_name)) {
            vlGetLayerSettingValue(layerSettingSet, setting_force_device_name, layer_settings->device.force_device_name);
        }
    }

    const char *setting_debug_fail = GetString(SETTING_DEBUG_FAIL_ON_ERROR);
    if (vlHasLayerSetting(layerSettingSet, setting_debug_fail)) {
        vlGetLayerSettingValue(layerSettingSet, setting_debug_fail, layer_settings->log.debug_fail_on_error);
    }

    const char *setting_debug_actions = GetString(SETTING_DEBUG_ACTIONS);
    if (vlHasLayerSetting(layerSettingSet, setting_debug_actions)) {
        std::vector<std::string> values;
        vlGetLayerSettingValues(layerSettingSet, setting_debug_actions, values);
        for (std::size_t i = 0, n = values.size(); i < n; ++i) {
            values[i] = ToUpper(values[i]);
        }
        layer_settings->log.debug_actions = GetDebugActionFlags(values);
    }

    const char *setting_debug_filename = GetString(SETTING_DEBUG_FILENAME);
    if (vlHasLayerSetting(layerSettingSet, setting_debug_filename)) {
        vlGetLayerSettingValue(layerSettingSet, setting_debug_filename, layer_settings->log.debug_filename);
    }

    const char *setting_debug_file_clear = GetString(SETTING_DEBUG_FILE_CLEAR);
    if (vlHasLayerSetting(layerSettingSet, setting_debug_file_clear)) {
        vlGetLayerSettingValue(layerSettingSet, setting_debug_file_clear, layer_settings->log.debug_file_discard);
    }

    const char *setting_debug_reports = GetString(SETTING_DEBUG_REPORTS);
    if (vlHasLayerSetting(layerSettingSet, setting_debug_reports)) {
        std::vector<std::string> values;
        vlGetLayerSettingValues(layerSettingSet, setting_debug_reports, values);
        for (std::size_t i = 0, n = values.size(); i < n; ++i) {
            values[i] = ToUpper(values[i]);
        }
        layer_settings->log.debug_reports = GetDebugReportFlags(values);
    }

    if (layer_settings->log.debug_actions & DEBUG_ACTION_FILE_BIT && layer_settings->log.profiles_log_file == nullptr) {
        layer_settings->log.profiles_log_file =
            fopen(layer_settings->log.debug_filename.c_str(), layer_settings->log.debug_file_discard ? "w" : "w+");
        if (layer_settings->log.profiles_log_file == nullptr) {
            layer_settings->log.debug_actions &= ~DEBUG_ACTION_FILE_BIT;
            layer_settings->log.debug_actions |= DEBUG_ACTION_STDOUT_BIT;
            LogMessage(layer_settings, DEBUG_REPORT_ERROR_BIT,
                       "Could not open %s, log to file is being overridden by log to stdout.\n",
                       layer_settings->log.debug_filename.c_str());
        } else {
            LogMessage(layer_settings, DEBUG_REPORT_DEBUG_BIT, "Log file %s opened\n", layer_settings->log.debug_filename.c_str());
        }
    } else {
        LogMessage(layer_settings, DEBUG_REPORT_DEBUG_BIT, "No need to open the log file %s\n",
                   layer_settings->log.debug_filename.c_str());
    }

    const std::string simulation_capabilities_log = GetSimulateCapabilitiesLog(layer_settings->simulate.capabilities);
    const std::string default_feature_values = GetDefaultFeatureValuesString(layer_settings->simulate.default_feature_values);
    const std::string debug_actions_log = GetDebugActionsLog(layer_settings->log.debug_actions);
    const std::string debug_reports_log = GetDebugReportsLog(layer_settings->log.debug_reports);

    std::string settings_log;
    settings_log += format("\t%s: %s\n", GetString(SETTING_PROFILE_FILE), layer_settings->simulate.profile_file.c_str());
    settings_log += format("\t%s: %s\n", GetString(SETTING_PROFILE_NAME), layer_settings->simulate.profile_name.c_str());
    settings_log +=
        format("\t%s: %s\n", GetString(SETTING_PROFILE_VALIDATION), layer_settings->simulate.profile_validation ? "true" : "false");
    settings_log += format("\t%s: %s\n", GetString(SETTING_SIMULATE_CAPABILITIES), simulation_capabilities_log.c_str());
    settings_log += format("\t%s: %s\n", GetString(SETTING_DEFAULT_FEATURE_VALUES), default_feature_values.c_str());

    settings_log += format("\t%s: %s\n", GetString(SETTING_EMULATE_PORTABILITY),
                           layer_settings->simulate.emulate_portability ? "true" : "false");
    if (layer_settings->simulate.emulate_portability) {
        settings_log += format("\t\t%s: %s\n", GetString(SETTING_CONTANT_ALPHA_COLOR_BLEND_FACTORS),
                               layer_settings->portability.constantAlphaColorBlendFactors ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", GetString(SETTING_EVENTS),
                               layer_settings->portability.events ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", GetString(SETTING_IMAGE_VIEW_FORMAT_REINTERPRETATION),
                               layer_settings->portability.imageViewFormatReinterpretation ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", GetString(SETTING_IMAGE_VIEW_FORMAT_SWIZZLE),
                               layer_settings->portability.imageViewFormatSwizzle ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", GetString(SETTING_IMAGE_VIEW_2D_ON_3D_IMAGE),
                               layer_settings->portability.imageView2DOn3DImage ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", GetString(SETTING_MULTISAMPLE_ARRAY_IMAGE),
                               layer_settings->portability.multisampleArrayImage ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", GetString(SETTING_MUTABLE_COMPARISON_SAMPLERS),
                               layer_settings->portability.mutableComparisonSamplers ? "true" : "false");
        settings_log +=
            format("\t\t%s: %s\n", GetString(SETTING_POINT_POLYGONS), layer_settings->portability.pointPolygons ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", GetString(SETTING_SAMPLER_MIP_LOD_BIAS),
                               layer_settings->portability.samplerMipLodBias ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", GetString(SETTING_SEPARATE_STENCIL_MASK_REF),
                               layer_settings->portability.separateStencilMaskRef ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", GetString(SETTING_SEPARATE_SAMPLE_RATE_INTERPOLATION_FUNCTIONS),
                               layer_settings->portability.shaderSampleRateInterpolationFunctions ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", GetString(SETTING_TESSELLATION_ISOLINES),
                               layer_settings->portability.tessellationIsolines ? "true" : "false");
        settings_log +=
            format("\t\t%s: %s\n", GetString(SETTING_TRIANGLE_FANS), layer_settings->portability.triangleFans ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", GetString(SETTING_VERTEX_ATTRIBUTE_ACCESS_BEYOND_STRIDE),
                               layer_settings->portability.vertexAttributeAccessBeyondStride ? "true" : "false");
        settings_log += format("\t\t%s: %d\n", GetString(SETTING_MIN_VERTEX_INPUT_BINDING_STRIDE_ALIGMENT),
                               static_cast<int>(layer_settings->portability.minVertexInputBindingStrideAlignment));
    }
    settings_log += format("\t%s: %s\n", GetString(SETTING_DEBUG_ACTIONS), debug_actions_log.c_str());
    settings_log += format("\t%s: %s\n", GetString(SETTING_DEBUG_FILENAME), layer_settings->log.debug_filename.c_str());
    settings_log +=
        format("\t%s: %s\n", GetString(SETTING_DEBUG_FILE_CLEAR), layer_settings->log.debug_file_discard ? "true" : "false");
    settings_log +=
        format("\t%s: %s\n", GetString(SETTING_DEBUG_FAIL_ON_ERROR), layer_settings->log.debug_fail_on_error ? "true" : "false");
    settings_log += format("\t%s: %s\n", GetString(SETTING_DEBUG_REPORTS), debug_reports_log.c_str());
    settings_log += format("\t%s: %s\n", GetString(SETTING_EXCLUDE_DEVICE_EXTENSIONS),
                           GetString(layer_settings->simulate.exclude_device_extensions).c_str());
    settings_log +=
        format("\t%s: %s\n", GetString(SETTING_EXCLUDE_FORMATS), GetString(layer_settings->simulate.exclude_formats).c_str());

    LogMessage(layer_settings, DEBUG_REPORT_NOTIFICATION_BIT, "Profile Layers Settings: {\n%s}\n", settings_log.c_str());

    vlDestroyLayerSettingSet(layerSettingSet, pAllocator);
}
