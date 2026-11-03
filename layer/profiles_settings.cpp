/*
 * Copyright (C) 2022-2026 Valve Corporation
 * Copyright (C) 2022-2026 LunarG, Inc.
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
    if (!IsLogEnabled(layer_settings, DEBUG_REPORT_WARNING_BIT)) {
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
    if (!IsLogEnabled(layer_settings, DEBUG_REPORT_WARNING_BIT)) {
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

static VkDebugUtilsMessageSeverityFlagBitsEXT GetDebugUtilsMessageSeverity(DebugReportBits report) {
    switch (report) {
        case DEBUG_REPORT_NOTIFICATION_BIT:
            return VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        default:
        case DEBUG_REPORT_WARNING_BIT:
            return VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
        case DEBUG_REPORT_ERROR_BIT:
            return VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        case DEBUG_REPORT_DEBUG_BIT:
            return VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
    }
}

// The layer messages are about the simulated capabilities rather than API usage validation
static const VkDebugUtilsMessageTypeFlagsEXT kDebugUtilsMessageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;

void AddDebugUtilsMessenger(ProfileLayerSettings *layer_settings, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                            VkDebugUtilsMessengerEXT messenger, bool instance) {
    assert(layer_settings != nullptr && pCreateInfo != nullptr);

    DebugUtilsMessenger debug_utils_messenger;
    debug_utils_messenger.handle = messenger;
    debug_utils_messenger.message_severity = pCreateInfo->messageSeverity;
    debug_utils_messenger.message_type = pCreateInfo->messageType;
    debug_utils_messenger.callback = pCreateInfo->pfnUserCallback;
    debug_utils_messenger.user_data = pCreateInfo->pUserData;
    debug_utils_messenger.instance = instance;

    std::lock_guard<std::mutex> lock(layer_settings->log.debug_utils_mutex);
    layer_settings->log.debug_utils_messengers.push_back(debug_utils_messenger);
}

void RemoveDebugUtilsMessenger(ProfileLayerSettings *layer_settings, VkDebugUtilsMessengerEXT messenger) {
    assert(layer_settings != nullptr);

    std::lock_guard<std::mutex> lock(layer_settings->log.debug_utils_mutex);
    std::vector<DebugUtilsMessenger> &messengers = layer_settings->log.debug_utils_messengers;
    messengers.erase(std::remove_if(messengers.begin(), messengers.end(),
                                    [messenger](const DebugUtilsMessenger &item) { return !item.instance && item.handle == messenger; }),
                     messengers.end());
}

void StoreInstanceDebugUtilsMessengers(ProfileLayerSettings *layer_settings, const VkInstanceCreateInfo *pCreateInfo) {
    assert(layer_settings != nullptr && pCreateInfo != nullptr);

    layer_settings->log.instance_debug_utils_messengers.clear();
    for (const VkBaseInStructure *next = reinterpret_cast<const VkBaseInStructure *>(pCreateInfo->pNext); next != nullptr;
         next = next->pNext) {
        if (next->sType == VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT) {
            VkDebugUtilsMessengerCreateInfoEXT create_info = *reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT *>(next);
            create_info.pNext = nullptr;
            layer_settings->log.instance_debug_utils_messengers.push_back(create_info);
        }
    }
}

InstanceDebugUtilsMessengersScope::InstanceDebugUtilsMessengersScope(ProfileLayerSettings *layer_settings)
    : layer_settings_(layer_settings) {
    assert(layer_settings_ != nullptr);

    for (const VkDebugUtilsMessengerCreateInfoEXT &create_info : layer_settings_->log.instance_debug_utils_messengers) {
        AddDebugUtilsMessenger(layer_settings_, &create_info, VK_NULL_HANDLE, true);
    }
}

InstanceDebugUtilsMessengersScope::~InstanceDebugUtilsMessengersScope() {
    std::lock_guard<std::mutex> lock(layer_settings_->log.debug_utils_mutex);
    std::vector<DebugUtilsMessenger> &messengers = layer_settings_->log.debug_utils_messengers;
    messengers.erase(std::remove_if(messengers.begin(), messengers.end(), [](const DebugUtilsMessenger &item) { return item.instance; }),
                     messengers.end());
}

// The debug utils mutex must be held by the caller
static bool UseDebugActions(const ProfileLayerSettings *layer_settings, DebugReportBits report) {
    // As with the validation layer report_flags, debug_reports only filters the messages of the debug actions, not of the messengers
    if (!(layer_settings->log.debug_reports & report)) {
        return false;
    }

    // As with the validation layer VK_DBG_LAYER_ACTION_DEFAULT, the default debug actions are replaced by the debug utils messengers
    return layer_settings->log.debug_utils_messengers.empty() || !layer_settings->log.debug_actions_default;
}

bool IsLogEnabled(ProfileLayerSettings *layer_settings, DebugReportBits report) {
    std::lock_guard<std::mutex> lock(layer_settings->log.debug_utils_mutex);
    const std::vector<DebugUtilsMessenger> &messengers = layer_settings->log.debug_utils_messengers;

    if (UseDebugActions(layer_settings, report)) {
        return true;
    }

    const VkDebugUtilsMessageSeverityFlagBitsEXT severity = GetDebugUtilsMessageSeverity(report);
    return std::any_of(messengers.begin(), messengers.end(), [severity](const DebugUtilsMessenger &item) {
        return (item.message_severity & severity) && (item.message_type & kDebugUtilsMessageType);
    });
}

void LogMessage(ProfileLayerSettings *layer_settings, DebugReportBits report, const char *message, ...) {
#if defined(__ANDROID__)
    if (!layer_settings) return;
#else
    assert(layer_settings);
#endif

    // The matching messengers are copied so that the application callbacks are called without holding the lock
    std::vector<DebugUtilsMessenger> messengers;
    bool use_debug_actions = false;
    const VkDebugUtilsMessageSeverityFlagBitsEXT severity = GetDebugUtilsMessageSeverity(report);
    {
        std::lock_guard<std::mutex> lock(layer_settings->log.debug_utils_mutex);
        use_debug_actions = UseDebugActions(layer_settings, report);
        for (const DebugUtilsMessenger &item : layer_settings->log.debug_utils_messengers) {
            if ((item.message_severity & severity) && (item.message_type & kDebugUtilsMessageType)) {
                messengers.push_back(item);
            }
        }
    }

    if (!use_debug_actions && messengers.empty()) {
        return;
    }

    std::size_t const STRING_BUFFER(4096);

    assert(message != nullptr);
    assert(strlen(message) < STRING_BUFFER);

    char log[STRING_BUFFER];
    snprintf(log, STRING_BUFFER, "%s", GetLogPrefix(report));
    std::size_t len = std::strlen(log);

    va_list list;

    va_start(list, message);
    vsnprintf(log + len, STRING_BUFFER - len, message, list);
    va_end(list);

    if (!messengers.empty()) {
        // The severity is reported by the messenger callback, so the message has no prefix, nor a trailing new line
        std::string text(log + len);
        if (!text.empty() && text.back() == '\n') {
            text.pop_back();
        }

        VkDebugUtilsMessengerCallbackDataEXT callback_data{VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CALLBACK_DATA_EXT};
        callback_data.pMessageIdName = kLayerName;
        callback_data.messageIdNumber = 0;
        callback_data.pMessage = text.c_str();

        // The VkBool32 returned by the callback is ignored, VK_TRUE is reserved for layer development and the layer doesn't abort calls
        for (const DebugUtilsMessenger &item : messengers) {
            item.callback(severity, kDebugUtilsMessageType, &callback_data, item.user_data);
        }
    }

    if (!use_debug_actions) {
        return;
    }

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

static std::vector<std::string> Split(const std::string &value, const std::string &delimiter) {
    std::vector<std::string> result;

    std::string parse = value;

    std::size_t start = 0;
    std::size_t end = parse.find(delimiter);
    while (end != std::string::npos) {
        result.push_back(parse.substr(start, end - start));
        start = end + delimiter.length();
        end = parse.find(delimiter, start);
    }

    const std::string last = parse.substr(start, end);
    if (!last.empty()) {
        result.push_back(last);
    }

    return result;
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
                                              kLayerSettingsSimulateCapabilities,
                                              kLayerSettingsDebugActions,
                                              kLayerSettingsDebugFilename,
                                              kLayerSettingsDebugFileClear,
                                              kLayerSettingsDebugFailOnError,
                                              kLayerSettingsDebugReports,
                                              kLayerSettingsExcludeDeviceExtensions,
                                              kLayerSettingsExcludeFormats,
                                              kLayerSettingsDefaultFeatureValues,
                                              kLayerSettingsUnknownFeatureValues};
        uint32_t setting_name_count = static_cast<uint32_t>(std::size(setting_names));

        std::vector<const char *> unknown_settings;
        vkuGetUnknownSettings(layerSettingSet, setting_name_count, setting_names, create_info, unknown_settings);

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
            layer_settings->simulate.profile_dirs.clear();

            std::vector<std::string> profile_dirs_list;
            vkuGetLayerSettingValues(layerSettingSet, kLayerSettingsProfileDirs, profile_dirs_list);
            for (std::size_t i = 0, n = profile_dirs_list.size(); i < n; ++i) {
                std::vector<std::string> profile_dirs = Split(profile_dirs_list[i], ",");
                layer_settings->simulate.profile_dirs.insert(
                    layer_settings->simulate.profile_dirs.end(), profile_dirs.begin(), profile_dirs.end());
            }
        }

        if (vkuHasLayerSetting(layerSettingSet, kLayerSettingsProfileFile)) {
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

    if (vkuHasLayerSetting(layerSettingSet, kLayerSettingsUnknownFeatureValues)) {
        std::string value;
        vkuGetLayerSettingValue(layerSettingSet, kLayerSettingsUnknownFeatureValues, value);
        layer_settings->simulate.unknown_feature_values = GetUnknownFeatureValues(ToUpper(value));
    }

    if (vkuHasLayerSetting(layerSettingSet, kLayerSettingsExcludeDeviceExtensions)) {
        vkuGetLayerSettingValues(layerSettingSet, kLayerSettingsExcludeDeviceExtensions,
                                 layer_settings->simulate.exclude_device_extensions);
    }

    if (vkuHasLayerSetting(layerSettingSet, kLayerSettingsExcludeFormats)) {
        vkuGetLayerSettingValues(layerSettingSet, kLayerSettingsExcludeFormats, layer_settings->simulate.exclude_formats);
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
        layer_settings->log.debug_actions_default = false;
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
    const std::string unknown_feature_values = GetUnknownFeatureValuesString(layer_settings->simulate.unknown_feature_values);
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
    settings_log += format("\t%s: %s\n", kLayerSettingsUnknownFeatureValues, unknown_feature_values.c_str());

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
