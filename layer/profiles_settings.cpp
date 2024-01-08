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
        AndroidPrintf(report, log);
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

    const VkLayerSettingsCreateInfoEXT *create_info = vkuFindLayerSettingsCreateInfo(pCreateInfo);

    VkuLayerSettingSet layerSettingSet = VK_NULL_HANDLE;
    vkuCreateLayerSettingSet(kLayerName, create_info, pAllocator, nullptr, &layerSettingSet);

    // Check if there is unknown settings if API settings are set
    if (create_info != nullptr) {
        static const char *setting_names[] = {kLayerSettingsProfileEmulation,
                                              kLayerSettingsProfileFile,
                                              kLayerSettingsProfileDirs,
                                              kLayerSettingsProfileName,
                                              kLayerSettingsProfileValidation,
                                              kLayerSettingsEmulatePortability,
                                              kLayerSettings_constantAlphaColorBlendFactors,
                                              kLayerSettings_events,
                                              kLayerSettings_imageViewFormatReinterpretation,
                                              kLayerSettings_imageViewFormatSwizzle,
                                              kLayerSettings_imageView2DOn3DImage,
                                              kLayerSettings_multisampleArrayImage,
                                              kLayerSettings_mutableComparisonSamplers,
                                              kLayerSettings_pointPolygons,
                                              kLayerSettings_samplerMipLodBias,
                                              kLayerSettings_separateStencilMaskRef,
                                              kLayerSettings_shaderSampleRateInterpolationFunctions,
                                              kLayerSettings_tessellationIsolines,
                                              kLayerSettings_tessellationPointMode,
                                              kLayerSettings_triangleFans,
                                              kLayerSettings_vertexAttributeAccessBeyondStride,
                                              kLayerSettings_minVertexInputBindingStrideAlignment,
                                              kLayerSettingsSimulateCapabilities,
                                              kLayerSettingsDebugActions,
                                              kLayerSettingsDebugFilename,
                                              kLayerSettingsDebugFileClear,
                                              kLayerSettingsDebugFailOnError,
                                              kLayerSettingsDebugReports,
                                              kLayerSettingsExcludeDeviceExtensions,
                                              kLayerSettingsExcludeFormats,
                                              kLayerSettingsDefaultFeatureValues,
                                              kLayerSettingsForceDevice,
                                              kLayerSettingsForceDeviceUUID,
                                              kLayerSettingsForceDeviceName};
        uint32_t setting_name_count = static_cast<uint32_t>(std::size(setting_names));

        std::vector<const char *> unknown_settings;
        vkuGetUnknownSettings(create_info, setting_name_count, setting_names, unknown_settings);

        for (std::size_t i = 0, n = unknown_settings.size(); i < n; ++i) {
            LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT,
                       "Unknown %s setting listed in VkLayerSettingsCreateInfoEXT, this setting is ignored.\n",
                       unknown_settings[i]);
        }
    }

    if (vkuHasLayerSetting(layerSettingSet, kLayerSettingsProfileEmulation)) {
        vkuGetLayerSettingValue(layerSettingSet, kLayerSettingsProfileEmulation, layer_settings->simulate.profile_emulation);
    }

    if (layer_settings->simulate.profile_emulation) {
        if (vkuHasLayerSetting(layerSettingSet, kLayerSettingsProfileDirs)) {
            vkuGetLayerSettingValues(layerSettingSet, kLayerSettingsProfileDirs, layer_settings->simulate.profile_dirs);
        }

        if (vkuHasLayerSetting(layerSettingSet, kLayerSettingsProfileFile)) {
            LogMessage(layer_settings, DEBUG_REPORT_WARNING_BIT, "'%s' setting is deprecated, please use '%s' instead.\n", kLayerSettingsProfileFile, kLayerSettingsProfileDirs);
            vkuGetLayerSettingValue(layerSettingSet, kLayerSettingsProfileFile, layer_settings->simulate.profile_file);
        }
    }

    if (!layer_settings->simulate.profile_dirs.empty() || !layer_settings->simulate.profile_file.empty()) {
        if (vkuHasLayerSetting(layerSettingSet, kLayerSettingsProfileName)) {
            vkuGetLayerSettingValue(layerSettingSet, kLayerSettingsProfileName, layer_settings->simulate.profile_name);
        }

        if (vkuHasLayerSetting(layerSettingSet, kLayerSettingsProfileValidation)) {
            vkuGetLayerSettingValue(layerSettingSet, kLayerSettingsProfileValidation, layer_settings->simulate.profile_validation);
        }

        if (vkuHasLayerSetting(layerSettingSet, kLayerSettingsSimulateCapabilities)) {
            std::vector<std::string> values;
            vkuGetLayerSettingValues(layerSettingSet, kLayerSettingsSimulateCapabilities, values);
            for (std::size_t i = 0, n = values.size(); i < n; ++i) {
                values[i] = ToUpper(values[i]);
            }
            layer_settings->simulate.capabilities = GetSimulateCapabilityFlags(values);
        }

        if (vkuHasLayerSetting(layerSettingSet, kLayerSettingsDefaultFeatureValues)) {
            std::string value;
            vkuGetLayerSettingValue(layerSettingSet, kLayerSettingsDefaultFeatureValues, value);
            layer_settings->simulate.default_feature_values = GetDefaultFeatureValues(ToUpper(value));
        }
    }

    if (vkuHasLayerSetting(layerSettingSet, kLayerSettingsExcludeDeviceExtensions)) {
        vkuGetLayerSettingValues(layerSettingSet, kLayerSettingsExcludeDeviceExtensions,
                                 layer_settings->simulate.exclude_device_extensions);
    }

    if (vkuHasLayerSetting(layerSettingSet, kLayerSettingsExcludeFormats)) {
        vkuGetLayerSettingValues(layerSettingSet, kLayerSettingsExcludeFormats, layer_settings->simulate.exclude_formats);
    }

    if (vkuHasLayerSetting(layerSettingSet, kLayerSettingsEmulatePortability)) {
        vkuGetLayerSettingValue(layerSettingSet, kLayerSettingsEmulatePortability, layer_settings->simulate.emulate_portability);

        if (vkuHasLayerSetting(layerSettingSet, kLayerSettings_constantAlphaColorBlendFactors)) {
            vkuGetLayerSettingValue(layerSettingSet, kLayerSettings_constantAlphaColorBlendFactors,
                                    layer_settings->portability.constantAlphaColorBlendFactors);
        }

        if (vkuHasLayerSetting(layerSettingSet, kLayerSettings_events)) {
            vkuGetLayerSettingValue(layerSettingSet, kLayerSettings_events, layer_settings->portability.events);
        }

        if (vkuHasLayerSetting(layerSettingSet, kLayerSettings_imageViewFormatReinterpretation)) {
            vkuGetLayerSettingValue(layerSettingSet, kLayerSettings_imageViewFormatReinterpretation,
                                    layer_settings->portability.imageViewFormatReinterpretation);
        }

        if (vkuHasLayerSetting(layerSettingSet, kLayerSettings_imageViewFormatSwizzle)) {
            vkuGetLayerSettingValue(layerSettingSet, kLayerSettings_imageViewFormatSwizzle,
                                    layer_settings->portability.imageViewFormatSwizzle);
        }

        if (vkuHasLayerSetting(layerSettingSet, kLayerSettings_imageView2DOn3DImage)) {
            vkuGetLayerSettingValue(layerSettingSet, kLayerSettings_imageView2DOn3DImage,
                                    layer_settings->portability.imageView2DOn3DImage);
        }

        if (vkuHasLayerSetting(layerSettingSet, kLayerSettings_multisampleArrayImage)) {
            vkuGetLayerSettingValue(layerSettingSet, kLayerSettings_multisampleArrayImage,
                                    layer_settings->portability.multisampleArrayImage);
        }

        if (vkuHasLayerSetting(layerSettingSet, kLayerSettings_mutableComparisonSamplers)) {
            vkuGetLayerSettingValue(layerSettingSet, kLayerSettings_mutableComparisonSamplers,
                                    layer_settings->portability.mutableComparisonSamplers);
        }

        if (vkuHasLayerSetting(layerSettingSet, kLayerSettings_pointPolygons)) {
            vkuGetLayerSettingValue(layerSettingSet, kLayerSettings_pointPolygons, layer_settings->portability.pointPolygons);
        }

        if (vkuHasLayerSetting(layerSettingSet, kLayerSettings_samplerMipLodBias)) {
            vkuGetLayerSettingValue(layerSettingSet, kLayerSettings_samplerMipLodBias,
                                    layer_settings->portability.samplerMipLodBias);
        }

        if (vkuHasLayerSetting(layerSettingSet, kLayerSettings_separateStencilMaskRef)) {
            vkuGetLayerSettingValue(layerSettingSet, kLayerSettings_separateStencilMaskRef,
                                    layer_settings->portability.separateStencilMaskRef);
        }

        if (vkuHasLayerSetting(layerSettingSet, kLayerSettings_shaderSampleRateInterpolationFunctions)) {
            vkuGetLayerSettingValue(layerSettingSet, kLayerSettings_shaderSampleRateInterpolationFunctions,
                                    layer_settings->portability.shaderSampleRateInterpolationFunctions);
        }

        if (vkuHasLayerSetting(layerSettingSet, kLayerSettings_tessellationIsolines)) {
            vkuGetLayerSettingValue(layerSettingSet, kLayerSettings_tessellationIsolines,
                                    layer_settings->portability.tessellationIsolines);
        }

        if (vkuHasLayerSetting(layerSettingSet, kLayerSettings_tessellationPointMode)) {
            vkuGetLayerSettingValue(layerSettingSet, kLayerSettings_tessellationPointMode,
                                    layer_settings->portability.tessellationPointMode);
        }

        if (vkuHasLayerSetting(layerSettingSet, kLayerSettings_triangleFans)) {
            vkuGetLayerSettingValue(layerSettingSet, kLayerSettings_triangleFans, layer_settings->portability.triangleFans);
        }

        if (vkuHasLayerSetting(layerSettingSet, kLayerSettings_vertexAttributeAccessBeyondStride)) {
            vkuGetLayerSettingValue(layerSettingSet, kLayerSettings_vertexAttributeAccessBeyondStride,
                                    layer_settings->portability.vertexAttributeAccessBeyondStride);
        }

        if (vkuHasLayerSetting(layerSettingSet, kLayerSettings_minVertexInputBindingStrideAlignment)) {
            vkuGetLayerSettingValue(layerSettingSet, kLayerSettings_minVertexInputBindingStrideAlignment,
                                    layer_settings->portability.minVertexInputBindingStrideAlignment);
        }
    }

    if (vkuHasLayerSetting(layerSettingSet, kLayerSettingsForceDevice)) {
        std::string value;
        vkuGetLayerSettingValue(layerSettingSet, kLayerSettingsForceDevice, value);
        layer_settings->device.force_device = GetForceDevice(ToUpper(value));

        if (vkuHasLayerSetting(layerSettingSet, kLayerSettingsForceDeviceUUID)) {
            vkuGetLayerSettingValue(layerSettingSet, kLayerSettingsForceDeviceUUID, layer_settings->device.force_device_uuid);
        }

        if (vkuHasLayerSetting(layerSettingSet, kLayerSettingsForceDeviceName)) {
            vkuGetLayerSettingValue(layerSettingSet, kLayerSettingsForceDeviceName, layer_settings->device.force_device_name);
        }
    }

    if (vkuHasLayerSetting(layerSettingSet, kLayerSettingsDebugFailOnError)) {
        vkuGetLayerSettingValue(layerSettingSet, kLayerSettingsDebugFailOnError, layer_settings->log.debug_fail_on_error);
    }

    if (vkuHasLayerSetting(layerSettingSet, kLayerSettingsDebugActions)) {
        std::vector<std::string> values;
        vkuGetLayerSettingValues(layerSettingSet, kLayerSettingsDebugActions, values);
        for (std::size_t i = 0, n = values.size(); i < n; ++i) {
            values[i] = ToUpper(values[i]);
        }
        layer_settings->log.debug_actions = GetDebugActionFlags(values);
    }

    if (vkuHasLayerSetting(layerSettingSet, kLayerSettingsDebugFilename)) {
        vkuGetLayerSettingValue(layerSettingSet, kLayerSettingsDebugFilename, layer_settings->log.debug_filename);
    }

    if (vkuHasLayerSetting(layerSettingSet, kLayerSettingsDebugFileClear)) {
        vkuGetLayerSettingValue(layerSettingSet, kLayerSettingsDebugFileClear, layer_settings->log.debug_file_discard);
    }

    if (vkuHasLayerSetting(layerSettingSet, kLayerSettingsDebugReports)) {
        std::vector<std::string> values;
        vkuGetLayerSettingValues(layerSettingSet, kLayerSettingsDebugReports, values);
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

    const std::string profile_dirs = GetString(layer_settings->simulate.profile_dirs);
    const std::string simulation_capabilities_log = GetSimulateCapabilitiesLog(layer_settings->simulate.capabilities);
    const std::string default_feature_values = GetDefaultFeatureValuesString(layer_settings->simulate.default_feature_values);
    const std::string debug_actions_log = GetDebugActionsLog(layer_settings->log.debug_actions);
    const std::string debug_reports_log = GetDebugReportsLog(layer_settings->log.debug_reports);

    std::string settings_log;
    settings_log += format("\t%s: %s\n", kLayerSettingsProfileEmulation, layer_settings->simulate.profile_emulation ? "true" : "false");
    settings_log += format("\t%s: %s\n", kLayerSettingsProfileFile, layer_settings->simulate.profile_file.c_str());
    settings_log += format("\t%s: %s\n", kLayerSettingsProfileDirs, profile_dirs.c_str());
    settings_log += format("\t%s: %s\n", kLayerSettingsProfileName, layer_settings->simulate.profile_name.c_str());
    settings_log +=
        format("\t%s: %s\n", kLayerSettingsProfileValidation, layer_settings->simulate.profile_validation ? "true" : "false");
    settings_log += format("\t%s: %s\n", kLayerSettingsSimulateCapabilities, simulation_capabilities_log.c_str());
    settings_log += format("\t%s: %s\n", kLayerSettingsDefaultFeatureValues, default_feature_values.c_str());

    settings_log +=
        format("\t%s: %s\n", kLayerSettingsEmulatePortability, layer_settings->simulate.emulate_portability ? "true" : "false");
    if (layer_settings->simulate.emulate_portability) {
        settings_log += format("\t\t%s: %s\n", kLayerSettings_constantAlphaColorBlendFactors,
                               layer_settings->portability.constantAlphaColorBlendFactors ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_events, layer_settings->portability.events ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_imageViewFormatReinterpretation,
                               layer_settings->portability.imageViewFormatReinterpretation ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_imageViewFormatSwizzle,
                               layer_settings->portability.imageViewFormatSwizzle ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_imageView2DOn3DImage,
                               layer_settings->portability.imageView2DOn3DImage ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_multisampleArrayImage,
                               layer_settings->portability.multisampleArrayImage ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_mutableComparisonSamplers,
                               layer_settings->portability.mutableComparisonSamplers ? "true" : "false");
        settings_log +=
            format("\t\t%s: %s\n", kLayerSettings_pointPolygons, layer_settings->portability.pointPolygons ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_samplerMipLodBias,
                               layer_settings->portability.samplerMipLodBias ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_separateStencilMaskRef,
                               layer_settings->portability.separateStencilMaskRef ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_shaderSampleRateInterpolationFunctions,
                               layer_settings->portability.shaderSampleRateInterpolationFunctions ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_tessellationIsolines,
                               layer_settings->portability.tessellationIsolines ? "true" : "false");
        settings_log +=
            format("\t\t%s: %s\n", kLayerSettings_triangleFans, layer_settings->portability.triangleFans ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_vertexAttributeAccessBeyondStride,
                               layer_settings->portability.vertexAttributeAccessBeyondStride ? "true" : "false");
        settings_log += format("\t\t%s: %d\n", kLayerSettings_minVertexInputBindingStrideAlignment,
                               static_cast<int>(layer_settings->portability.minVertexInputBindingStrideAlignment));
    }
    settings_log += format("\t%s: %s\n", kLayerSettingsDebugActions, debug_actions_log.c_str());
    settings_log += format("\t%s: %s\n", kLayerSettingsDebugFilename, layer_settings->log.debug_filename.c_str());
    settings_log += format("\t%s: %s\n", kLayerSettingsDebugFileClear, layer_settings->log.debug_file_discard ? "true" : "false");
    settings_log +=
        format("\t%s: %s\n", kLayerSettingsDebugFailOnError, layer_settings->log.debug_fail_on_error ? "true" : "false");
    settings_log += format("\t%s: %s\n", kLayerSettingsDebugReports, debug_reports_log.c_str());
    settings_log += format("\t%s: %s\n", kLayerSettingsExcludeDeviceExtensions,
                           GetString(layer_settings->simulate.exclude_device_extensions).c_str());
    settings_log += format("\t%s: %s\n", kLayerSettingsExcludeFormats, GetString(layer_settings->simulate.exclude_formats).c_str());

    LogMessage(layer_settings, DEBUG_REPORT_NOTIFICATION_BIT, "Profile Layers Settings: {\n%s}\n", settings_log.c_str());

    vkuDestroyLayerSettingSet(layerSettingSet, pAllocator);
}
