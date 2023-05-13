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

DebugActionFlags GetDebugActionFlags(const vku::Strings &values) {
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

DebugReportFlags GetDebugReportFlags(const vku::Strings &values) {
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

void InitSettings(const void *pNext) {
    const VkProfileLayerSettingsEXT *user_settings = FindSettingsInChain(pNext);
    // Programmatically-specified settings override ENV vars or layer settings file settings
    if (pNext && user_settings) {
        *layer_settings = *user_settings;
    } else {
        if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsProfileFile)) {
            layer_settings->profile_file = vku::GetLayerSettingString(kOurLayerName, kLayerSettingsProfileFile);
            if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsProfileName)) {
                layer_settings->profile_name = vku::GetLayerSettingString(kOurLayerName, kLayerSettingsProfileName);
            }

            if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsProfileValidation)) {
                layer_settings->profile_validation = vku::GetLayerSettingBool(kOurLayerName, kLayerSettingsProfileValidation);
            }

            if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsSimulateCapabilities)) {
                layer_settings->simulate_capabilities =
                    GetSimulateCapabilityFlags(vku::GetLayerSettingStrings(kOurLayerName, kLayerSettingsSimulateCapabilities));
            }

            if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsDefaultFeatureValues)) {
                layer_settings->default_feature_values =
                    GetDefaultFeatureValues(vku::GetLayerSettingString(kOurLayerName, kLayerSettingsDefaultFeatureValues));
            }

            if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsProfileVariantsMode)) {
                layer_settings->profile_variants_mode =
                    GetProfileVariantsMode(vku::GetLayerSettingString(kOurLayerName, kLayerSettingsProfileVariantsMode));
            }
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsEmulatePortability)) {
            layer_settings->emulate_portability = vku::GetLayerSettingBool(kOurLayerName, kLayerSettingsEmulatePortability);
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettings_constantAlphaColorBlendFactors)) {
            layer_settings->constantAlphaColorBlendFactors =
                vku::GetLayerSettingBool(kOurLayerName, kLayerSettings_constantAlphaColorBlendFactors);
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettings_events)) {
            layer_settings->events = vku::GetLayerSettingBool(kOurLayerName, kLayerSettings_events);
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettings_imageViewFormatReinterpretation)) {
            layer_settings->imageViewFormatReinterpretation =
                vku::GetLayerSettingBool(kOurLayerName, kLayerSettings_imageViewFormatReinterpretation);
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettings_imageViewFormatSwizzle)) {
            layer_settings->imageViewFormatSwizzle = vku::GetLayerSettingBool(kOurLayerName, kLayerSettings_imageViewFormatSwizzle);
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettings_imageView2DOn3DImage)) {
            layer_settings->imageView2DOn3DImage = vku::GetLayerSettingBool(kOurLayerName, kLayerSettings_imageView2DOn3DImage);
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettings_multisampleArrayImage)) {
            layer_settings->multisampleArrayImage = vku::GetLayerSettingBool(kOurLayerName, kLayerSettings_multisampleArrayImage);
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettings_mutableComparisonSamplers)) {
            layer_settings->mutableComparisonSamplers =
                vku::GetLayerSettingBool(kOurLayerName, kLayerSettings_mutableComparisonSamplers);
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettings_pointPolygons)) {
            layer_settings->pointPolygons = vku::GetLayerSettingBool(kOurLayerName, kLayerSettings_pointPolygons);
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettings_samplerMipLodBias)) {
            layer_settings->samplerMipLodBias = vku::GetLayerSettingBool(kOurLayerName, kLayerSettings_samplerMipLodBias);
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettings_separateStencilMaskRef)) {
            layer_settings->separateStencilMaskRef = vku::GetLayerSettingBool(kOurLayerName, kLayerSettings_separateStencilMaskRef);
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettings_shaderSampleRateInterpolationFunctions)) {
            layer_settings->shaderSampleRateInterpolationFunctions =
                vku::GetLayerSettingBool(kOurLayerName, kLayerSettings_shaderSampleRateInterpolationFunctions);
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettings_tessellationIsolines)) {
            layer_settings->tessellationIsolines = vku::GetLayerSettingBool(kOurLayerName, kLayerSettings_tessellationIsolines);
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettings_tessellationPointMode)) {
            layer_settings->tessellationPointMode = vku::GetLayerSettingBool(kOurLayerName, kLayerSettings_tessellationPointMode);
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettings_triangleFans)) {
            layer_settings->triangleFans = vku::GetLayerSettingBool(kOurLayerName, kLayerSettings_triangleFans);
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettings_vertexAttributeAccessBeyondStride)) {
            layer_settings->vertexAttributeAccessBeyondStride =
                vku::GetLayerSettingBool(kOurLayerName, kLayerSettings_vertexAttributeAccessBeyondStride);
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettings_minVertexInputBindingStrideAlignment)) {
            layer_settings->minVertexInputBindingStrideAlignment =
                static_cast<uint32_t>(vku::GetLayerSettingInt(kOurLayerName, kLayerSettings_minVertexInputBindingStrideAlignment));
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsForceDevice)) {
            layer_settings->force_device =
                GetForceDevice(vku::GetLayerSettingString(kOurLayerName, kLayerSettingsForceDevice));
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsForceDeviceUUID)) {
            layer_settings->force_device_uuid =
                vku::GetLayerSettingString(kOurLayerName, kLayerSettingsForceDeviceUUID);
        }

        if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsForceDeviceName)) {
            layer_settings->force_device_name =
                vku::GetLayerSettingString(kOurLayerName, kLayerSettingsForceDeviceName);
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

