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
                               const std::string &features,
                               VkFormatFeatureFlags profile_features, VkFormatFeatureFlags device_features) {
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
                                const std::string &features,
                                VkFormatFeatureFlags2 profile_features, VkFormatFeatureFlags2 device_features) {
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

static void GetLayerSettingValue(VlLayerSettingSet layerSettingSet, const char *pSettingName, bool &settingValue) {
    uint32_t value_count = 1;
    std::vector<VkBool32> values(value_count);
    vlGetLayerSettingValues(layerSettingSet, pSettingName, VK_LAYER_SETTING_TYPE_BOOL_EXT, &value_count, &values[0]);
    settingValue = values[0] == VK_TRUE;
}

static void GetLayerSettingValue(VlLayerSettingSet layerSettingSet, const char *pSettingName, uint32_t &settingValue) {
    uint32_t value_count = 1;
    std::vector<uint32_t> values(value_count);
    vlGetLayerSettingValues(layerSettingSet, pSettingName, VK_LAYER_SETTING_TYPE_UINT32_EXT, &value_count, &values[0]);
    settingValue = values[0];
}

static void GetLayerSettingValue(VlLayerSettingSet layerSettingSet, const char *pSettingName, std::string &settingValue) {
    uint32_t value_count = 1;
    std::vector<const char *> values(value_count);
    vlGetLayerSettingValues(layerSettingSet, pSettingName, VK_LAYER_SETTING_TYPE_STRING_EXT, &value_count, &values[0]);
    settingValue = values[0];
}

static void GetLayerSettingValues(VlLayerSettingSet layerSettingSet, const char *pSettingName,
                                  std::vector<std::string> &settingValues) {
    uint32_t value_count = 0;
    vlGetLayerSettingValues(layerSettingSet, pSettingName, VK_LAYER_SETTING_TYPE_STRING_EXT, &value_count, nullptr);
    if (value_count > 0) {
        std::vector<const char *> values(value_count);
        vlGetLayerSettingValues(layerSettingSet, pSettingName, VK_LAYER_SETTING_TYPE_STRING_EXT, &value_count, &values[0]);
        settingValues.assign(values.begin(), values.end());
    }
}

static const VkLayerSettingsCreateInfoEXT *FindSettingsInChain(const void *next) {
    const VkBaseOutStructure *current = reinterpret_cast<const VkBaseOutStructure *>(next);
    const VkLayerSettingsCreateInfoEXT *found = nullptr;
    while (current) {
        if (VK_STRUCTURE_TYPE_LAYER_SETTINGS_EXT == current->sType) {
            found = reinterpret_cast<const VkLayerSettingsCreateInfoEXT *>(current);
            current = nullptr;
        } else {
            current = current->pNext;
        }
    }
    return found;
}

void InitProfilesLayerSettings(const VkInstanceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator,
                               ProfileLayerSettings *layer_settings) {
    assert(layer_settings != nullptr);

    VlLayerSettingSet layerSettingSet = VK_NULL_HANDLE;
    vlCreateLayerSettingSet(kLayerName, FindSettingsInChain(pCreateInfo), pAllocator, nullptr, &layerSettingSet);

    if (vlHasLayerSetting(layerSettingSet, kLayerSettingsProfileFile)) {
        GetLayerSettingValue(layerSettingSet, kLayerSettingsProfileFile, layer_settings->simulate.profile_file);

        if (vlHasLayerSetting(layerSettingSet, kLayerSettingsProfileName)) {
            GetLayerSettingValue(layerSettingSet, kLayerSettingsProfileName, layer_settings->simulate.profile_name);
        }

        if (vlHasLayerSetting(layerSettingSet, kLayerSettingsProfileValidation)) {
            GetLayerSettingValue(layerSettingSet, kLayerSettingsProfileValidation, layer_settings->simulate.profile_validation);
        }

        if (vlHasLayerSetting(layerSettingSet, kLayerSettingsSimulateCapabilities)) {
            std::vector<std::string> values;
            GetLayerSettingValues(layerSettingSet, kLayerSettingsSimulateCapabilities, values);
            for (std::size_t i = 0, n = values.size(); i < n; ++i) {
                values[i] = ToUpper(values[i]);
            }
            layer_settings->simulate.capabilities = GetSimulateCapabilityFlags(values);
        }

        if (vlHasLayerSetting(layerSettingSet, kLayerSettingsDefaultFeatureValues)) {
            std::string value;
            GetLayerSettingValue(layerSettingSet, kLayerSettingsDefaultFeatureValues, value);
            layer_settings->simulate.default_feature_values = GetDefaultFeatureValues(ToUpper(value));
        }
    }

    if (vlHasLayerSetting(layerSettingSet, kLayerSettingsExcludeDeviceExtensions)) {
        GetLayerSettingValues(layerSettingSet, kLayerSettingsExcludeDeviceExtensions,
                              layer_settings->simulate.exclude_device_extensions);
    }

    if (vlHasLayerSetting(layerSettingSet, kLayerSettingsExcludeFormats)) {
        GetLayerSettingValues(layerSettingSet, kLayerSettingsExcludeFormats, layer_settings->simulate.exclude_formats);
    }

    if (vlHasLayerSetting(layerSettingSet, kLayerSettingsEmulatePortability)) {
        GetLayerSettingValue(layerSettingSet, kLayerSettingsEmulatePortability, layer_settings->simulate.emulate_portability);

        if (vlHasLayerSetting(layerSettingSet, kLayerSettings_constantAlphaColorBlendFactors)) {
            GetLayerSettingValue(layerSettingSet, kLayerSettings_constantAlphaColorBlendFactors,
                                 layer_settings->portability.constantAlphaColorBlendFactors);
        }

        if (vlHasLayerSetting(layerSettingSet, kLayerSettings_events)) {
            GetLayerSettingValue(layerSettingSet, kLayerSettings_events, layer_settings->portability.events);
        }

        if (vlHasLayerSetting(layerSettingSet, kLayerSettings_imageViewFormatReinterpretation)) {
            GetLayerSettingValue(layerSettingSet, kLayerSettings_imageViewFormatReinterpretation,
                                 layer_settings->portability.imageViewFormatReinterpretation);
        }

        if (vlHasLayerSetting(layerSettingSet, kLayerSettings_imageViewFormatSwizzle)) {
            GetLayerSettingValue(layerSettingSet, kLayerSettings_imageViewFormatSwizzle,
                                 layer_settings->portability.imageViewFormatSwizzle);
        }

        if (vlHasLayerSetting(layerSettingSet, kLayerSettings_imageView2DOn3DImage)) {
            GetLayerSettingValue(layerSettingSet, kLayerSettings_imageView2DOn3DImage,
                                 layer_settings->portability.imageView2DOn3DImage);
        }

        if (vlHasLayerSetting(layerSettingSet, kLayerSettings_multisampleArrayImage)) {
            GetLayerSettingValue(layerSettingSet, kLayerSettings_multisampleArrayImage,
                                 layer_settings->portability.multisampleArrayImage);
        }

        if (vlHasLayerSetting(layerSettingSet, kLayerSettings_mutableComparisonSamplers)) {
            GetLayerSettingValue(layerSettingSet, kLayerSettings_mutableComparisonSamplers,
                                 layer_settings->portability.mutableComparisonSamplers);
        }

        if (vlHasLayerSetting(layerSettingSet, kLayerSettings_pointPolygons)) {
            GetLayerSettingValue(layerSettingSet, kLayerSettings_pointPolygons, layer_settings->portability.pointPolygons);
        }

        if (vlHasLayerSetting(layerSettingSet, kLayerSettings_samplerMipLodBias)) {
            GetLayerSettingValue(layerSettingSet, kLayerSettings_samplerMipLodBias,
                                 layer_settings->portability.samplerMipLodBias);
        }

        if (vlHasLayerSetting(layerSettingSet, kLayerSettings_separateStencilMaskRef)) {
            GetLayerSettingValue(layerSettingSet, kLayerSettings_separateStencilMaskRef,
                                 layer_settings->portability.separateStencilMaskRef);
        }

        if (vlHasLayerSetting(layerSettingSet, kLayerSettings_shaderSampleRateInterpolationFunctions)) {
            GetLayerSettingValue(layerSettingSet, kLayerSettings_shaderSampleRateInterpolationFunctions,
                                 layer_settings->portability.shaderSampleRateInterpolationFunctions);
        }

        if (vlHasLayerSetting(layerSettingSet, kLayerSettings_tessellationIsolines)) {
            GetLayerSettingValue(layerSettingSet, kLayerSettings_tessellationIsolines,
                                 layer_settings->portability.tessellationIsolines);
        }

        if (vlHasLayerSetting(layerSettingSet, kLayerSettings_tessellationPointMode)) {
            GetLayerSettingValue(layerSettingSet, kLayerSettings_tessellationPointMode,
                                 layer_settings->portability.tessellationPointMode);
        }

        if (vlHasLayerSetting(layerSettingSet, kLayerSettings_triangleFans)) {
            GetLayerSettingValue(layerSettingSet, kLayerSettings_triangleFans, layer_settings->portability.triangleFans);
        }

        if (vlHasLayerSetting(layerSettingSet, kLayerSettings_vertexAttributeAccessBeyondStride)) {
            GetLayerSettingValue(layerSettingSet, kLayerSettings_vertexAttributeAccessBeyondStride,
                                 layer_settings->portability.vertexAttributeAccessBeyondStride);
        }

        if (vlHasLayerSetting(layerSettingSet, kLayerSettings_minVertexInputBindingStrideAlignment)) {
            GetLayerSettingValue(layerSettingSet, kLayerSettings_minVertexInputBindingStrideAlignment,
                                 layer_settings->portability.minVertexInputBindingStrideAlignment);
        }
    }

    if (vlHasLayerSetting(layerSettingSet, kLayerSettingsForceDevice)) {
        std::string value;
        GetLayerSettingValue(layerSettingSet, kLayerSettingsForceDevice, value);
        layer_settings->device.force_device = GetForceDevice(ToUpper(value));

        if (vlHasLayerSetting(layerSettingSet, kLayerSettingsForceDeviceUUID)) {
            GetLayerSettingValue(layerSettingSet, kLayerSettingsForceDeviceUUID, layer_settings->device.force_device_uuid);
        }

        if (vlHasLayerSetting(layerSettingSet, kLayerSettingsForceDeviceName)) {
            GetLayerSettingValue(layerSettingSet, kLayerSettingsForceDeviceName, layer_settings->device.force_device_name);
        }
    }

    if (vlHasLayerSetting(layerSettingSet, kLayerSettingsDebugFailOnError)) {
        GetLayerSettingValue(layerSettingSet, kLayerSettingsDebugFailOnError, layer_settings->log.debug_fail_on_error);
    }

    if (vlHasLayerSetting(layerSettingSet, kLayerSettingsDebugActions)) {
        std::vector<std::string> values;
        GetLayerSettingValues(layerSettingSet, kLayerSettingsDebugActions, values);
        for (std::size_t i = 0, n = values.size(); i < n; ++i) {
            values[i] = ToUpper(values[i]);
        }
        layer_settings->log.debug_actions = GetDebugActionFlags(values);
    }

    if (vlHasLayerSetting(layerSettingSet, kLayerSettingsDebugFilename)) {
        GetLayerSettingValue(layerSettingSet, kLayerSettingsDebugFilename, layer_settings->log.debug_filename);
    }

    if (vlHasLayerSetting(layerSettingSet, kLayerSettingsDebugFileClear)) {
        GetLayerSettingValue(layerSettingSet, kLayerSettingsDebugFileClear, layer_settings->log.debug_file_discard);
    }

    if (vlHasLayerSetting(layerSettingSet, kLayerSettingsDebugReports)) {
        std::vector<std::string> values;
        GetLayerSettingValues(layerSettingSet, kLayerSettingsDebugReports, values);
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
    settings_log += format("\t%s: %s\n", kLayerSettingsProfileFile, layer_settings->simulate.profile_file.c_str());
    settings_log += format("\t%s: %s\n", kLayerSettingsProfileName, layer_settings->simulate.profile_name.c_str());
    settings_log += format("\t%s: %s\n", kLayerSettingsProfileValidation, layer_settings->simulate.profile_validation ? "true" : "false");
    settings_log += format("\t%s: %s\n", kLayerSettingsSimulateCapabilities, simulation_capabilities_log.c_str());
    settings_log += format("\t%s: %s\n", kLayerSettingsDefaultFeatureValues, default_feature_values.c_str());

    settings_log += format("\t%s: %s\n", kLayerSettingsEmulatePortability, layer_settings->simulate.emulate_portability ? "true" : "false");
    if (layer_settings->simulate.emulate_portability) {
        settings_log += format("\t\t%s: %s\n", kLayerSettings_constantAlphaColorBlendFactors, layer_settings->portability.constantAlphaColorBlendFactors ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_events, layer_settings->portability.events ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_imageViewFormatReinterpretation, layer_settings->portability.imageViewFormatReinterpretation ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_imageViewFormatSwizzle, layer_settings->portability.imageViewFormatSwizzle ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_imageView2DOn3DImage, layer_settings->portability.imageView2DOn3DImage ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_multisampleArrayImage, layer_settings->portability.multisampleArrayImage ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_mutableComparisonSamplers, layer_settings->portability.mutableComparisonSamplers ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_pointPolygons, layer_settings->portability.pointPolygons ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_samplerMipLodBias, layer_settings->portability.samplerMipLodBias ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_separateStencilMaskRef, layer_settings->portability.separateStencilMaskRef ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_shaderSampleRateInterpolationFunctions, layer_settings->portability.shaderSampleRateInterpolationFunctions ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_tessellationIsolines, layer_settings->portability.tessellationIsolines ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_triangleFans, layer_settings->portability.triangleFans ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_vertexAttributeAccessBeyondStride, layer_settings->portability.vertexAttributeAccessBeyondStride ? "true" : "false");
        settings_log += format("\t\t%s: %d\n", kLayerSettings_minVertexInputBindingStrideAlignment, static_cast<int>(layer_settings->portability.minVertexInputBindingStrideAlignment));
    }
    settings_log += format("\t%s: %s\n", kLayerSettingsDebugActions, debug_actions_log.c_str());
    settings_log += format("\t%s: %s\n", kLayerSettingsDebugFilename, layer_settings->log.debug_filename.c_str());
    settings_log += format("\t%s: %s\n", kLayerSettingsDebugFileClear, layer_settings->log.debug_file_discard ? "true" : "false");
    settings_log += format("\t%s: %s\n", kLayerSettingsDebugFailOnError, layer_settings->log.debug_fail_on_error ? "true" : "false");
    settings_log += format("\t%s: %s\n", kLayerSettingsDebugReports, debug_reports_log.c_str());
    settings_log += format("\t%s: %s\n", kLayerSettingsExcludeDeviceExtensions, GetString(layer_settings->simulate.exclude_device_extensions).c_str());
    settings_log += format("\t%s: %s\n", kLayerSettingsExcludeFormats, GetString(layer_settings->simulate.exclude_formats).c_str());

    LogMessage(layer_settings, DEBUG_REPORT_NOTIFICATION_BIT, "Profile Layers Settings: {\n%s}\n", settings_log.c_str());

    vlDestroyLayerSettingSet(layerSettingSet, pAllocator);
}

