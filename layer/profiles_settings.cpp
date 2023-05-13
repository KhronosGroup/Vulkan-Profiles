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

const char *const kLayerSettingsProfileFile = "profile_file";
const char *const kLayerSettingsProfileName = "profile_name";
const char *const kLayerSettingsProfileValidation = "profile_validation";
const char *const kLayerSettingsEmulatePortability = "emulate_portability";
const char *const kLayerSettings_constantAlphaColorBlendFactors = "constantAlphaColorBlendFactors";
const char *const kLayerSettings_events = "events";
const char *const kLayerSettings_imageViewFormatReinterpretation = "imageViewFormatReinterpretation";
const char *const kLayerSettings_imageViewFormatSwizzle = "imageViewFormatSwizzle";
const char *const kLayerSettings_imageView2DOn3DImage = "imageView2DOn3DImage";
const char *const kLayerSettings_multisampleArrayImage = "multisampleArrayImage";
const char *const kLayerSettings_mutableComparisonSamplers = "mutableComparisonSamplers";
const char *const kLayerSettings_pointPolygons = "pointPolygons";
const char *const kLayerSettings_samplerMipLodBias = "samplerMipLodBias";
const char *const kLayerSettings_separateStencilMaskRef = "separateStencilMaskRef";
const char *const kLayerSettings_shaderSampleRateInterpolationFunctions = "shaderSampleRateInterpolationFunctions";
const char *const kLayerSettings_tessellationIsolines = "tessellationIsolines";
const char *const kLayerSettings_tessellationPointMode = "tessellationPointMode";
const char *const kLayerSettings_triangleFans = "triangleFans";
const char *const kLayerSettings_vertexAttributeAccessBeyondStride = "vertexAttributeAccessBeyondStride";
const char *const kLayerSettings_minVertexInputBindingStrideAlignment = "minVertexInputBindingStrideAlignment";
const char *const kLayerSettingsSimulateCapabilities = "simulate_capabilities";
const char *const kLayerSettingsDebugActions = "debug_actions";
const char *const kLayerSettingsDebugFilename = "debug_filename";
const char *const kLayerSettingsDebugFileClear = "debug_file_clear";
const char *const kLayerSettingsDebugFailOnError = "debug_fail_on_error";
const char *const kLayerSettingsDebugReports = "debug_reports";
const char *const kLayerSettingsExcludeDeviceExtensions = "exclude_device_extensions";
const char *const kLayerSettingsExcludeFormats = "exclude_formats";
const char *const kLayerSettingsDefaultFeatureValues = "default_feature_values";
const char *const kLayerSettingsProfileVariantsMode = "profile_variants_mode";
const char *const kLayerSettingsForceDevice = "force_device";
const char *const kLayerSettingsForceDeviceUUID = "force_device_uuid";
const char *const kLayerSettingsForceDeviceName = "force_device_name";

VkProfileLayerSettingsEXT *layer_settings = new VkProfileLayerSettingsEXT{};
FILE *profiles_log_file = nullptr;

DefaultFeatureValues GetDefaultFeatureValues(const std::string &value) {
    if (value == "DEFAULT_FEATURE_VALUES_FALSE") {
        return DEFAULT_FEATURE_VALUES_FALSE;
    } else if (value == "DEFAULT_FEATURE_VALUES_DEVICE") {
        return DEFAULT_FEATURE_VALUES_DEVICE;
    }

    return DEFAULT_FEATURE_VALUES_DEVICE;
}

std::string GetDefaultFeatureValuesLog(DefaultFeatureValues value) {
    if (value == DEFAULT_FEATURE_VALUES_FALSE) {
        return "DEFAULT_FEATURE_VALUES_FALSE";
    } else if (value == DEFAULT_FEATURE_VALUES_DEVICE) {
        return "DEFAULT_FEATURE_VALUES_DEVICE";
    }

    return "DEFAULT_FEATURE_VALUES_DEVICE";
}

ForceDevice GetForceDevice(const std::string& value) {
    if (value == "FORCE_DEVICE_OFF") {
        return FORCE_DEVICE_OFF;
    } else if (value == "FORCE_DEVICE_WITH_UUID") {
        return FORCE_DEVICE_WITH_UUID;
    } else if (value == "FORCE_DEVICE_WITH_NAME") {
        return FORCE_DEVICE_WITH_NAME;
    }

    return FORCE_DEVICE_OFF;
}

ProfileVariantsMode GetProfileVariantsMode(const std::string &value) {
    if (value == "VARIANTS_MODE_ALL") {
        return VARIANTS_MODE_ALL;
    } else if (value == "VARIANTS_MODE_FIRST_SUPPORTED") {
        return VARIANTS_MODE_FIRST_SUPPORTED;
    }

    return VARIANTS_MODE_FIRST_SUPPORTED;
}

DebugActionFlags GetDebugActionFlags(const std::vector<std::string> &values) {
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

std::string GetDebugActionsLog(DebugActionFlags flags) {
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

DebugReportFlags GetDebugReportFlags(const std::vector<std::string> &values) {
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

void LogMessage(DebugReport report, const char *message, ...) {
    if (!(layer_settings->debug_reports & report)) return;

    std::size_t const STRING_BUFFER(4096);

    assert(message != nullptr);
    assert(strlen(message) < STRING_BUFFER);

    char log[STRING_BUFFER];
    snprintf(log, STRING_BUFFER, "%s", GetLogPrefix(report));

    va_list list;

    va_start(list, message);
    vsnprintf(log, STRING_BUFFER, message, list);
    va_end(list);

    if (layer_settings->debug_actions & DEBUG_ACTION_STDOUT_BIT) {
#if defined(__ANDROID__)
        AndroidPrintf(report, message);
#else
        fprintf(stdout, "%s", log);
#endif
    }

    if (layer_settings->debug_actions & DEBUG_ACTION_FILE_BIT) {
        fprintf(profiles_log_file, "%s", log);
    }

#if _WIN32
    if (layer_settings->debug_actions & DEBUG_ACTION_OUTPUT_BIT) {
        OutputDebugString(log);
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

static const VkProfileLayerSettingsEXT *FindSettingsInChain(const void *next) {
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

static void GetLayerSettingValue(const char *pSettingName, bool &settingValue) {
    uint32_t value_count = 1;
    std::vector<VkBool32> values(value_count);
    vlGetLayerSettingValues(pSettingName, VK_LAYER_SETTING_TYPE_BOOL_EXT, &value_count, &values[0]);
    settingValue = values[0] == VK_TRUE;
}

static void GetLayerSettingValue(const char *pSettingName, uint32_t &settingValue) {
    uint32_t value_count = 1;
    std::vector<uint32_t> values(value_count);
    vlGetLayerSettingValues(pSettingName, VK_LAYER_SETTING_TYPE_UINT32_EXT, &value_count, &values[0]);
    settingValue = values[0];
}

static void GetLayerSettingValue(const char* pSettingName, std::string& settingValue) {
    uint32_t value_count = 1;
    std::vector<const char *> values(value_count);
    vlGetLayerSettingValues(pSettingName, VK_LAYER_SETTING_TYPE_STRING_EXT, &value_count, &values[0]);
    settingValue = values[0];
}

static void GetLayerSettingValues(const char *pSettingName, std::vector<std::string> &settingValues) {
    uint32_t value_count = 0;
    vlGetLayerSettingValues(pSettingName, VK_LAYER_SETTING_TYPE_STRING_EXT, &value_count, nullptr);
    std::vector<const char *> values(value_count);
    vlGetLayerSettingValues(pSettingName, VK_LAYER_SETTING_TYPE_STRING_EXT, &value_count, &values[0]);
    settingValues.assign(values.begin(), values.end());
}

void InitSettings(const void *pNext) {
    const VkProfileLayerSettingsEXT *user_settings = FindSettingsInChain(pNext);

    // Programmatically-specified settings override ENV vars or layer settings file settings
    if (pNext && user_settings) {
        *layer_settings = *user_settings;
    } else {
        if (vlHasLayerSetting(kLayerSettingsProfileFile)) {
            GetLayerSettingValue(kLayerSettingsProfileFile, layer_settings->profile_file);

            if (vlHasLayerSetting(kLayerSettingsProfileName)) {
                GetLayerSettingValue(kLayerSettingsProfileName, layer_settings->profile_name);
            }

            if (vlHasLayerSetting(kLayerSettingsProfileValidation)) {
                GetLayerSettingValue(kLayerSettingsProfileValidation, layer_settings->profile_validation);
            }

            if (vlHasLayerSetting(kLayerSettingsSimulateCapabilities)) {
                std::vector<std::string> values;
                GetLayerSettingValues(kLayerSettingsSimulateCapabilities, values);
                layer_settings->simulate_capabilities = GetSimulateCapabilityFlags(values);
            }

            if (vlHasLayerSetting(kLayerSettingsDefaultFeatureValues)) {
                std::string value;
                GetLayerSettingValue(kLayerSettingsDefaultFeatureValues, value);
                layer_settings->default_feature_values = GetDefaultFeatureValues(value);
            }

            if (vlHasLayerSetting(kLayerSettingsProfileVariantsMode)) {
                std::string value;
                GetLayerSettingValue(kLayerSettingsProfileVariantsMode, value);
                layer_settings->profile_variants_mode = GetProfileVariantsMode(value);
            }
        }

        if (vlHasLayerSetting(kLayerSettingsEmulatePortability)) {
            GetLayerSettingValue(kLayerSettingsEmulatePortability, layer_settings->emulate_portability);

            if (vlHasLayerSetting(kLayerSettings_constantAlphaColorBlendFactors)) {
                GetLayerSettingValue(kLayerSettings_constantAlphaColorBlendFactors, layer_settings->constantAlphaColorBlendFactors);
            }

            if (vlHasLayerSetting(kLayerSettings_events)) {
                GetLayerSettingValue(kLayerSettings_events, layer_settings->events);
            }

            if (vlHasLayerSetting(kLayerSettings_imageViewFormatReinterpretation)) {
                GetLayerSettingValue(kLayerSettings_imageViewFormatReinterpretation, layer_settings->imageViewFormatReinterpretation);
            }

            if (vlHasLayerSetting(kLayerSettings_imageViewFormatSwizzle)) {
                GetLayerSettingValue(kLayerSettings_imageViewFormatSwizzle, layer_settings->imageViewFormatSwizzle);
            }

            if (vlHasLayerSetting(kLayerSettings_imageView2DOn3DImage)) {
                GetLayerSettingValue(kLayerSettings_imageView2DOn3DImage, layer_settings->imageView2DOn3DImage);
            }

            if (vlHasLayerSetting(kLayerSettings_multisampleArrayImage)) {
                GetLayerSettingValue(kLayerSettings_multisampleArrayImage, layer_settings->multisampleArrayImage);
            }

            if (vlHasLayerSetting(kLayerSettings_mutableComparisonSamplers)) {
                GetLayerSettingValue(kLayerSettings_mutableComparisonSamplers, layer_settings->mutableComparisonSamplers);
            }

            if (vlHasLayerSetting(kLayerSettings_pointPolygons)) {
                GetLayerSettingValue(kLayerSettings_pointPolygons, layer_settings->pointPolygons);
            }

            if (vlHasLayerSetting(kLayerSettings_samplerMipLodBias)) {
                GetLayerSettingValue(kLayerSettings_samplerMipLodBias, layer_settings->samplerMipLodBias);
            }

            if (vlHasLayerSetting(kLayerSettings_separateStencilMaskRef)) {
                GetLayerSettingValue(kLayerSettings_separateStencilMaskRef, layer_settings->separateStencilMaskRef);
            }

            if (vlHasLayerSetting(kLayerSettings_shaderSampleRateInterpolationFunctions)) {
                GetLayerSettingValue(kLayerSettings_shaderSampleRateInterpolationFunctions, layer_settings->shaderSampleRateInterpolationFunctions);
            }

            if (vlHasLayerSetting(kLayerSettings_tessellationIsolines)) {
                GetLayerSettingValue(kLayerSettings_tessellationIsolines, layer_settings->tessellationIsolines);
            }

            if (vlHasLayerSetting(kLayerSettings_tessellationPointMode)) {
                GetLayerSettingValue(kLayerSettings_tessellationPointMode, layer_settings->tessellationPointMode);
            }

            if (vlHasLayerSetting(kLayerSettings_triangleFans)) {
                GetLayerSettingValue(kLayerSettings_triangleFans, layer_settings->triangleFans);
            }

            if (vlHasLayerSetting(kLayerSettings_vertexAttributeAccessBeyondStride)) {
                GetLayerSettingValue(kLayerSettings_vertexAttributeAccessBeyondStride, layer_settings->vertexAttributeAccessBeyondStride);
            }

            if (vlHasLayerSetting(kLayerSettings_minVertexInputBindingStrideAlignment)) {
                GetLayerSettingValue(kLayerSettings_minVertexInputBindingStrideAlignment, layer_settings->minVertexInputBindingStrideAlignment);
            }
        }

        if (vlHasLayerSetting(kLayerSettingsForceDevice)) {
            std::string value;
            GetLayerSettingValue(kLayerSettingsForceDevice, value);
            layer_settings->force_device = GetForceDevice(value);
        }

        if (vlHasLayerSetting(kLayerSettingsForceDeviceUUID)) {
            GetLayerSettingValue(kLayerSettingsForceDeviceUUID, layer_settings->force_device_uuid);
        }

        if (vlHasLayerSetting(kLayerSettingsForceDeviceName)) {
            GetLayerSettingValue(kLayerSettingsForceDeviceName, layer_settings->force_device_name);
        }

        if (vlHasLayerSetting(kLayerSettingsDebugFailOnError)) {
            GetLayerSettingValue(kLayerSettingsDebugFailOnError, layer_settings->debug_fail_on_error);
        }

        if (vlHasLayerSetting(kLayerSettingsExcludeDeviceExtensions)) {
            GetLayerSettingValues(kLayerSettingsExcludeDeviceExtensions, layer_settings->exclude_device_extensions);
        }

        if (vlHasLayerSetting(kLayerSettingsExcludeFormats)) {
            GetLayerSettingValues(kLayerSettingsExcludeFormats, layer_settings->exclude_formats);
        }

        if (vlHasLayerSetting(kLayerSettingsDebugActions)) {
            std::vector<std::string> values;
            GetLayerSettingValues(kLayerSettingsDebugActions, values);
            layer_settings->debug_actions = GetDebugActionFlags(values);
        }

        if (vlHasLayerSetting(kLayerSettingsDebugFilename)) {
            GetLayerSettingValue(kLayerSettingsDebugFilename, layer_settings->debug_filename);
        }

        if (vlHasLayerSetting(kLayerSettingsDebugFileClear)) {
            GetLayerSettingValue(kLayerSettingsDebugFileClear, layer_settings->debug_file_discard);
        }

        if (vlHasLayerSetting(kLayerSettingsDebugReports)) {
            std::vector<std::string> values;
            GetLayerSettingValues(kLayerSettingsDebugReports, values);
            layer_settings->debug_reports = GetDebugReportFlags(values);
        }
    }

    if (layer_settings->debug_actions & DEBUG_ACTION_FILE_BIT && profiles_log_file == nullptr) {
        profiles_log_file = fopen(layer_settings->debug_filename.c_str(), layer_settings->debug_file_discard ? "w" : "w+");
        if (profiles_log_file == nullptr) {
            layer_settings->debug_actions &= ~DEBUG_ACTION_FILE_BIT;
            layer_settings->debug_actions |= DEBUG_ACTION_STDOUT_BIT;
            LogMessage(DEBUG_REPORT_ERROR_BIT, "Could not open %s, log to file is being overridden by log to stdout.\n",
                       layer_settings->debug_filename.c_str());
        } else {
            LogMessage(DEBUG_REPORT_DEBUG_BIT, "Log file %s opened\n", layer_settings->debug_filename.c_str());
        }
    } else {
        LogMessage(DEBUG_REPORT_DEBUG_BIT, "No need to open the log file %s\n", layer_settings->debug_filename.c_str());
    }

    const std::string simulation_capabilities_log = GetSimulateCapabilitiesLog(layer_settings->simulate_capabilities);
    const std::string default_feature_values = GetDefaultFeatureValuesLog(layer_settings->default_feature_values);
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
    settings_log += format("\t%s: %s\n", kLayerSettingsSimulateCapabilities, simulation_capabilities_log.c_str());
    settings_log += format("\t%s: %s\n", kLayerSettingsDefaultFeatureValues, default_feature_values.c_str());
    settings_log +=
        format("\t%s: %s\n", kLayerSettingsEmulatePortability, layer_settings->emulate_portability ? "true" : "false");
    if (layer_settings->emulate_portability) {
        settings_log += format("\t\t%s: %s\n", kLayerSettings_constantAlphaColorBlendFactors,
                               layer_settings->constantAlphaColorBlendFactors ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_events, layer_settings->events ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_imageViewFormatReinterpretation,
                               layer_settings->imageViewFormatReinterpretation ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_imageViewFormatSwizzle,
                               layer_settings->imageViewFormatSwizzle ? "true" : "false");
        settings_log +=
            format("\t\t%s: %s\n", kLayerSettings_imageView2DOn3DImage, layer_settings->imageView2DOn3DImage ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_multisampleArrayImage,
                               layer_settings->multisampleArrayImage ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_mutableComparisonSamplers,
                               layer_settings->mutableComparisonSamplers ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_pointPolygons, layer_settings->pointPolygons ? "true" : "false");
        settings_log +=
            format("\t\t%s: %s\n", kLayerSettings_samplerMipLodBias, layer_settings->samplerMipLodBias ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_separateStencilMaskRef,
                               layer_settings->separateStencilMaskRef ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_shaderSampleRateInterpolationFunctions,
                               layer_settings->shaderSampleRateInterpolationFunctions ? "true" : "false");
        settings_log +=
            format("\t\t%s: %s\n", kLayerSettings_tessellationIsolines, layer_settings->tessellationIsolines ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_triangleFans, layer_settings->triangleFans ? "true" : "false");
        settings_log += format("\t\t%s: %s\n", kLayerSettings_vertexAttributeAccessBeyondStride,
                               layer_settings->vertexAttributeAccessBeyondStride ? "true" : "false");
        settings_log += format("\t\t%s: %d\n", kLayerSettings_minVertexInputBindingStrideAlignment,
                               static_cast<int>(layer_settings->minVertexInputBindingStrideAlignment));
    }
    settings_log += format("\t%s: %s\n", kLayerSettingsDebugActions, debug_actions_log.c_str());
    settings_log += format("\t%s: %s\n", kLayerSettingsDebugFilename, layer_settings->debug_filename.c_str());
    settings_log += format("\t%s: %s\n", kLayerSettingsDebugFileClear, layer_settings->debug_file_discard ? "true" : "false");
    settings_log += format("\t%s: %s\n", kLayerSettingsDebugFailOnError, layer_settings->debug_fail_on_error ? "true" : "false");
    settings_log += format("\t%s: %s\n", kLayerSettingsDebugReports, debug_reports_log.c_str());
    settings_log +=
        format("\t%s: %s\n", kLayerSettingsExcludeDeviceExtensions, GetString(layer_settings->exclude_device_extensions).c_str());
    settings_log += format("\t%s: %s\n", kLayerSettingsExcludeFormats, GetString(layer_settings->exclude_formats).c_str());

    LogMessage(DEBUG_REPORT_NOTIFICATION_BIT, "Profile Layers Settings: {\n%s}\n", settings_log.c_str());
}

