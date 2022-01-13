/*
 * Copyright (C) 2015-2021 Valve Corporation
 * Copyright (C) 2015-2021 LunarG, Inc.
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
 */

/*
 * layersvt/device_simulation.cpp - The VK_LAYER_KHRONOS_profiles layer.
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

#include <json/json.h>  // https://github.com/open-source-parsers/jsoncpp

#include "vulkan/vk_layer.h"
#include "vulkan/vulkan_beta.h"
#include <vk_layer_config.h>
#include "vk_layer_table.h"
#include "../vku/vk_layer_settings.h"

namespace {

// Global constants //////////////////////////////////////////////////////////////////////////////////////////////////////////////

// For new features/functionality, increment the minor level and reset patch level to zero.
// For any changes, at least increment the patch level.  See https://semver.org/
// When updating the version, be sure to make corresponding changes to the layer manifest file at
// layersvt/VkLayer_device_simulation.json.in

const uint32_t kVersionProfilesMajor = 1;
const uint32_t kVersionProfilesMinor = 9;
const uint32_t kVersionProfilesPatch = 1;
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

// TODO (ncesario): Where should this live?
bool get_physical_device_properties2_active = false;

// Device extensions that this layer provides:
const std::array<VkExtensionProperties, 2> kDeviceExtensionProperties = {
    {{VK_EXT_TOOLING_INFO_EXTENSION_NAME, VK_EXT_TOOLING_INFO_SPEC_VERSION},
     {VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME, VK_KHR_PORTABILITY_SUBSET_SPEC_VERSION}}};
const uint32_t kDeviceExtensionPropertiesCount = static_cast<uint32_t>(kDeviceExtensionProperties.size());

enum SetCombinationMode {
    SET_CHECK_SUPPORT,
    SET_FROM_DEVICE,
    SET_FROM_PROFILE,
    SET_FROM_PROFILE_OVERRIDE,
};

// Environment variables defined by this layer ///////////////////////////////////////////////////////////////////////////////////

#if defined(__ANDROID__)
const char *const kEnvarProfilesFilename = "debug.vulkan.profiles.filepath";  // path of the configuration file(s) to load.
const char *const kEnvarProfilesDebugEnable =
    "debug.vulkan.profiles.debugenable";  // a non-zero integer will enable debugging output.
const char *const kEnvarProfilesEmulatePortability =
    "debug.vulkan.profiles.emulateportability";  // a non-zero integer will enable emulation of the VK_KHR_portability_subset
                                                 // extension.
const char *const kEnvarProfilesModifyExtensionList =
    "debug.vulkan.profiles.modifyextensionlist";  // a non-zero integer will enable modifying device extensions list.
const char *const kEnvarProfilesModifyFormatList =
    "debug.vulkan.Profiles.modifyformatlist";  // an ArrayCombinationMode value sets how the device and config format lists are
                                               // combined.
const char *const kEnvarProfilesModifyFormatProperties =
    "debug.vulkan.Profiles.modifyformatproperties";  // an ArrayCombinationMode value sets how the device and config format
                                                     // properties are combined.
const char *const kEnvarProfilesProfileName = "debug.vulkan.profiles.profilename";  // name of the profile to be used
#else
const char *const kEnvarProfilesFilename = "VK_KHRONOS_PROFILES_FILENAME";  // path of the configuration file(s) to load.
const char *const kEnvarProfilesDebugEnable =
    "VK_KHRONOS_PROFILES_DEBUG_ENABLE";  // a non-zero integer will enable debugging output.
const char *const kEnvarProfilesEmulatePortability =
    "VK_KHRONOS_PROFILES_EMULATE_PORTABILITY_SUBSET_EXTENSION";  // a non-zero integer will enable emulation of the
                                                                 // VK_KHR_portability_subset extension.
const char *const kEnvarProfilesModifyExtensionList =
    "VK_KHRONOS_PROFILES_MODIFY_EXTENSION_LIST";  // an ArrayCombinationMode value sets how the device and config extension lists
                                                  // are combined.
const char *const kEnvarProfilesModifyFormatList =
    "VK_KHRONOS_PROFILES_MODIFY_FORMAT_LIST";  // an ArrayCombinationMode value sets how the device and config format lists are
                                               // combined.
const char *const kEnvarProfilesModifyFormatProperties =
    "VK_KHRONOS_PROFILES_MODIFY_FORMAT_PROPERTIES";  // an ArrayCombinationMode value sets how the device and config format
                                                     // properties are combined.
const char *const kEnvarProfilesProfileName = "VK_KHRONOS_PROFILES_PROFILE_NAME";  // name of the profile to be used
#endif

const char *const kLayerSettingsProfileFile = "profile_file";
const char *const kLayerSettingsProfileName = "profile_name";
const char *const kLayerSettingsDebugOutput = "debug_output";
const char *const kLayerSettingsEmulatePortability = "emulate_portability";
const char *const kLayerSettingsModifyExtensionList = "emulate_extension_list";
const char *const kLayerSettingsModifyFormatList = "emulate_format_list";
const char *const kLayerSettingsModifyFormatProperties = "emulate_format_properties";

struct SetCombinationModeSetting {
    SetCombinationMode mode;
    bool fromEnvVar;
};

struct IntSetting {
    int num;
    bool fromEnvVar;
};

struct StringSetting {
    std::string str;
    bool fromEnvVar;
};

struct StringSetting inputFilename;
struct StringSetting profileName = {};
struct IntSetting debugLevel;
struct IntSetting emulatePortability;
struct SetCombinationModeSetting modifyExtensionList;
struct SetCombinationModeSetting modifyFormatList;
struct SetCombinationModeSetting modifyFormatProperties;

// Various small utility functions ///////////////////////////////////////////////////////////////////////////////////////////////

#if defined(__ANDROID__)
#include <android/log.h>
const uint32_t MAX_BUFFER_SIZE = 255;
typedef enum { VK_LOG_NONE = 0, VK_LOG_ERROR, VK_LOG_WARNING, VK_LOG_VERBOSE, VK_LOG_DEBUG } VkLogLevel;

std::string AndroidGetEnv(const char *key) {
    std::string command("settings get global ");
    command += key;

    std::string android_env;
    FILE *pipe = popen(command.c_str(), "r");
    if (pipe != nullptr) {
        char buffer[MAX_BUFFER_SIZE] = {};
        while (fgets(buffer, MAX_BUFFER_SIZE, pipe) != NULL) {
            android_env.append(buffer);
        }
        pclose(pipe);
    }

    // Only if the value is set will we get a string back
    if (android_env.length() > 0) {
        __android_log_print(ANDROID_LOG_INFO, "profiles", "Vulkan device simulation layer %s: %s", command.c_str(),
                            android_env.c_str());
        android_env.erase(android_env.find_last_not_of(" \n\r\t") + 1);
        return android_env;
    }

    return "";
}
#endif

// Retrieve the value of an environment variable.
std::string GetEnvarValue(const char *name) {
    std::string value = "";
#if defined(_WIN32)
    DWORD size = GetEnvironmentVariable(name, nullptr, 0);
    if (size > 0) {
        std::vector<char> buffer(size);
        GetEnvironmentVariable(name, buffer.data(), size);
        value = buffer.data();
    }
#elif defined(__ANDROID__)
    std::string v = AndroidGetEnv(name);
    if (v.length() > 0) value = v;
#else
    const char *v = getenv(name);
    if (v) value = v;
#endif
    // printf("envar %s = \"%s\"\n", name, value.c_str());
    return value;
}

#if defined(__ANDROID__)
void AndroidPrintf(VkLogLevel level, const char *fmt, va_list args) {
    int requiredLength;
    va_list argcopy;
    va_copy(argcopy, args);
    requiredLength = vsnprintf(NULL, 0, fmt, argcopy) + 1;
    va_end(argcopy);

    char *message = (char *)malloc(requiredLength);
    vsnprintf(message, requiredLength, fmt, args);
    switch (level) {
        case VK_LOG_DEBUG:
            __android_log_print(ANDROID_LOG_DEBUG, "profiles", "%s", message);
            break;
        case VK_LOG_ERROR:
            __android_log_print(ANDROID_LOG_ERROR, "profiles", "%s", message);
            break;
        default:
            __android_log_print(ANDROID_LOG_INFO, "profiles", "%s", message);
            break;
    }
    free(message);
}
#endif

void DebugPrintf(const char *fmt, ...) {
    if (debugLevel.num > 0) {
#if !defined(__ANDROID__)
        printf("\tDEBUG profiles ");
#endif
        va_list args;
        va_start(args, fmt);
#if defined(__ANDROID__)
        AndroidPrintf(VK_LOG_DEBUG, fmt, args);
#else
        vprintf(fmt, args);
#endif
        va_end(args);
    }
}

void ErrorPrintf(const char *fmt, ...) {
#if !defined(__ANDROID__)
    fprintf(stderr, "\tERROR profiles ");
#endif
    va_list args;
    va_start(args, fmt);
#if defined(__ANDROID__)
    AndroidPrintf(VK_LOG_ERROR, fmt, args);
#else
    vfprintf(stderr, fmt, args);
#endif
    va_end(args);
}

#define FORMAT_TO_STRING(format) \
    case format:                 \
        return #format

std::string vkFormatToString(VkFormat fmt) {
    switch (fmt) {
        FORMAT_TO_STRING(VK_FORMAT_UNDEFINED);
        FORMAT_TO_STRING(VK_FORMAT_R4G4_UNORM_PACK8);
        FORMAT_TO_STRING(VK_FORMAT_R4G4B4A4_UNORM_PACK16);
        FORMAT_TO_STRING(VK_FORMAT_B4G4R4A4_UNORM_PACK16);
        FORMAT_TO_STRING(VK_FORMAT_R5G6B5_UNORM_PACK16);
        FORMAT_TO_STRING(VK_FORMAT_B5G6R5_UNORM_PACK16);
        FORMAT_TO_STRING(VK_FORMAT_R5G5B5A1_UNORM_PACK16);
        FORMAT_TO_STRING(VK_FORMAT_B5G5R5A1_UNORM_PACK16);
        FORMAT_TO_STRING(VK_FORMAT_A1R5G5B5_UNORM_PACK16);
        FORMAT_TO_STRING(VK_FORMAT_R8_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_R8_SNORM);
        FORMAT_TO_STRING(VK_FORMAT_R8_USCALED);
        FORMAT_TO_STRING(VK_FORMAT_R8_SSCALED);
        FORMAT_TO_STRING(VK_FORMAT_R8_UINT);
        FORMAT_TO_STRING(VK_FORMAT_R8_SINT);
        FORMAT_TO_STRING(VK_FORMAT_R8_SRGB);
        FORMAT_TO_STRING(VK_FORMAT_R8G8_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_R8G8_SNORM);
        FORMAT_TO_STRING(VK_FORMAT_R8G8_USCALED);
        FORMAT_TO_STRING(VK_FORMAT_R8G8_SSCALED);
        FORMAT_TO_STRING(VK_FORMAT_R8G8_UINT);
        FORMAT_TO_STRING(VK_FORMAT_R8G8_SINT);
        FORMAT_TO_STRING(VK_FORMAT_R8G8_SRGB);
        FORMAT_TO_STRING(VK_FORMAT_R8G8B8_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_R8G8B8_SNORM);
        FORMAT_TO_STRING(VK_FORMAT_R8G8B8_USCALED);
        FORMAT_TO_STRING(VK_FORMAT_R8G8B8_SSCALED);
        FORMAT_TO_STRING(VK_FORMAT_R8G8B8_UINT);
        FORMAT_TO_STRING(VK_FORMAT_R8G8B8_SINT);
        FORMAT_TO_STRING(VK_FORMAT_R8G8B8_SRGB);
        FORMAT_TO_STRING(VK_FORMAT_B8G8R8_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_B8G8R8_SNORM);
        FORMAT_TO_STRING(VK_FORMAT_B8G8R8_USCALED);
        FORMAT_TO_STRING(VK_FORMAT_B8G8R8_SSCALED);
        FORMAT_TO_STRING(VK_FORMAT_B8G8R8_UINT);
        FORMAT_TO_STRING(VK_FORMAT_B8G8R8_SINT);
        FORMAT_TO_STRING(VK_FORMAT_B8G8R8_SRGB);
        FORMAT_TO_STRING(VK_FORMAT_R8G8B8A8_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_R8G8B8A8_SNORM);
        FORMAT_TO_STRING(VK_FORMAT_R8G8B8A8_USCALED);
        FORMAT_TO_STRING(VK_FORMAT_R8G8B8A8_SSCALED);
        FORMAT_TO_STRING(VK_FORMAT_R8G8B8A8_UINT);
        FORMAT_TO_STRING(VK_FORMAT_R8G8B8A8_SINT);
        FORMAT_TO_STRING(VK_FORMAT_R8G8B8A8_SRGB);
        FORMAT_TO_STRING(VK_FORMAT_B8G8R8A8_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_B8G8R8A8_SNORM);
        FORMAT_TO_STRING(VK_FORMAT_B8G8R8A8_USCALED);
        FORMAT_TO_STRING(VK_FORMAT_B8G8R8A8_SSCALED);
        FORMAT_TO_STRING(VK_FORMAT_B8G8R8A8_UINT);
        FORMAT_TO_STRING(VK_FORMAT_B8G8R8A8_SINT);
        FORMAT_TO_STRING(VK_FORMAT_B8G8R8A8_SRGB);
        FORMAT_TO_STRING(VK_FORMAT_A8B8G8R8_UNORM_PACK32);
        FORMAT_TO_STRING(VK_FORMAT_A8B8G8R8_SNORM_PACK32);
        FORMAT_TO_STRING(VK_FORMAT_A8B8G8R8_USCALED_PACK32);
        FORMAT_TO_STRING(VK_FORMAT_A8B8G8R8_SSCALED_PACK32);
        FORMAT_TO_STRING(VK_FORMAT_A8B8G8R8_UINT_PACK32);
        FORMAT_TO_STRING(VK_FORMAT_A8B8G8R8_SINT_PACK32);
        FORMAT_TO_STRING(VK_FORMAT_A8B8G8R8_SRGB_PACK32);
        FORMAT_TO_STRING(VK_FORMAT_A2R10G10B10_UNORM_PACK32);
        FORMAT_TO_STRING(VK_FORMAT_A2R10G10B10_SNORM_PACK32);
        FORMAT_TO_STRING(VK_FORMAT_A2R10G10B10_USCALED_PACK32);
        FORMAT_TO_STRING(VK_FORMAT_A2R10G10B10_SSCALED_PACK32);
        FORMAT_TO_STRING(VK_FORMAT_A2R10G10B10_UINT_PACK32);
        FORMAT_TO_STRING(VK_FORMAT_A2R10G10B10_SINT_PACK32);
        FORMAT_TO_STRING(VK_FORMAT_A2B10G10R10_UNORM_PACK32);
        FORMAT_TO_STRING(VK_FORMAT_A2B10G10R10_SNORM_PACK32);
        FORMAT_TO_STRING(VK_FORMAT_A2B10G10R10_USCALED_PACK32);
        FORMAT_TO_STRING(VK_FORMAT_A2B10G10R10_SSCALED_PACK32);
        FORMAT_TO_STRING(VK_FORMAT_A2B10G10R10_UINT_PACK32);
        FORMAT_TO_STRING(VK_FORMAT_A2B10G10R10_SINT_PACK32);
        FORMAT_TO_STRING(VK_FORMAT_R16_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_R16_SNORM);
        FORMAT_TO_STRING(VK_FORMAT_R16_USCALED);
        FORMAT_TO_STRING(VK_FORMAT_R16_SSCALED);
        FORMAT_TO_STRING(VK_FORMAT_R16_UINT);
        FORMAT_TO_STRING(VK_FORMAT_R16_SINT);
        FORMAT_TO_STRING(VK_FORMAT_R16_SFLOAT);
        FORMAT_TO_STRING(VK_FORMAT_R16G16_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_R16G16_SNORM);
        FORMAT_TO_STRING(VK_FORMAT_R16G16_USCALED);
        FORMAT_TO_STRING(VK_FORMAT_R16G16_SSCALED);
        FORMAT_TO_STRING(VK_FORMAT_R16G16_UINT);
        FORMAT_TO_STRING(VK_FORMAT_R16G16_SINT);
        FORMAT_TO_STRING(VK_FORMAT_R16G16_SFLOAT);
        FORMAT_TO_STRING(VK_FORMAT_R16G16B16_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_R16G16B16_SNORM);
        FORMAT_TO_STRING(VK_FORMAT_R16G16B16_USCALED);
        FORMAT_TO_STRING(VK_FORMAT_R16G16B16_SSCALED);
        FORMAT_TO_STRING(VK_FORMAT_R16G16B16_UINT);
        FORMAT_TO_STRING(VK_FORMAT_R16G16B16_SINT);
        FORMAT_TO_STRING(VK_FORMAT_R16G16B16_SFLOAT);
        FORMAT_TO_STRING(VK_FORMAT_R16G16B16A16_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_R16G16B16A16_SNORM);
        FORMAT_TO_STRING(VK_FORMAT_R16G16B16A16_USCALED);
        FORMAT_TO_STRING(VK_FORMAT_R16G16B16A16_SSCALED);
        FORMAT_TO_STRING(VK_FORMAT_R16G16B16A16_UINT);
        FORMAT_TO_STRING(VK_FORMAT_R16G16B16A16_SINT);
        FORMAT_TO_STRING(VK_FORMAT_R16G16B16A16_SFLOAT);
        FORMAT_TO_STRING(VK_FORMAT_R32_UINT);
        FORMAT_TO_STRING(VK_FORMAT_R32_SINT);
        FORMAT_TO_STRING(VK_FORMAT_R32_SFLOAT);
        FORMAT_TO_STRING(VK_FORMAT_R32G32_UINT);
        FORMAT_TO_STRING(VK_FORMAT_R32G32_SINT);
        FORMAT_TO_STRING(VK_FORMAT_R32G32_SFLOAT);
        FORMAT_TO_STRING(VK_FORMAT_R32G32B32_UINT);
        FORMAT_TO_STRING(VK_FORMAT_R32G32B32_SINT);
        FORMAT_TO_STRING(VK_FORMAT_R32G32B32_SFLOAT);
        FORMAT_TO_STRING(VK_FORMAT_R32G32B32A32_UINT);
        FORMAT_TO_STRING(VK_FORMAT_R32G32B32A32_SINT);
        FORMAT_TO_STRING(VK_FORMAT_R32G32B32A32_SFLOAT);
        FORMAT_TO_STRING(VK_FORMAT_R64_UINT);
        FORMAT_TO_STRING(VK_FORMAT_R64_SINT);
        FORMAT_TO_STRING(VK_FORMAT_R64_SFLOAT);
        FORMAT_TO_STRING(VK_FORMAT_R64G64_UINT);
        FORMAT_TO_STRING(VK_FORMAT_R64G64_SINT);
        FORMAT_TO_STRING(VK_FORMAT_R64G64_SFLOAT);
        FORMAT_TO_STRING(VK_FORMAT_R64G64B64_UINT);
        FORMAT_TO_STRING(VK_FORMAT_R64G64B64_SINT);
        FORMAT_TO_STRING(VK_FORMAT_R64G64B64_SFLOAT);
        FORMAT_TO_STRING(VK_FORMAT_R64G64B64A64_UINT);
        FORMAT_TO_STRING(VK_FORMAT_R64G64B64A64_SINT);
        FORMAT_TO_STRING(VK_FORMAT_R64G64B64A64_SFLOAT);
        FORMAT_TO_STRING(VK_FORMAT_B10G11R11_UFLOAT_PACK32);
        FORMAT_TO_STRING(VK_FORMAT_E5B9G9R9_UFLOAT_PACK32);
        FORMAT_TO_STRING(VK_FORMAT_D16_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_X8_D24_UNORM_PACK32);
        FORMAT_TO_STRING(VK_FORMAT_D32_SFLOAT);
        FORMAT_TO_STRING(VK_FORMAT_S8_UINT);
        FORMAT_TO_STRING(VK_FORMAT_D16_UNORM_S8_UINT);
        FORMAT_TO_STRING(VK_FORMAT_D24_UNORM_S8_UINT);
        FORMAT_TO_STRING(VK_FORMAT_D32_SFLOAT_S8_UINT);
        FORMAT_TO_STRING(VK_FORMAT_BC1_RGB_UNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_BC1_RGB_SRGB_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_BC1_RGBA_UNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_BC1_RGBA_SRGB_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_BC2_UNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_BC2_SRGB_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_BC3_UNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_BC3_SRGB_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_BC4_UNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_BC4_SNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_BC5_UNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_BC5_SNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_BC6H_UFLOAT_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_BC6H_SFLOAT_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_BC7_UNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_BC7_SRGB_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_EAC_R11_UNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_EAC_R11_SNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_EAC_R11G11_UNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_EAC_R11G11_SNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_4x4_UNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_4x4_SRGB_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_5x4_UNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_5x4_SRGB_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_5x5_UNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_5x5_SRGB_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_6x5_UNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_6x5_SRGB_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_6x6_UNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_6x6_SRGB_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_8x5_UNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_8x5_SRGB_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_8x6_UNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_8x6_SRGB_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_8x8_UNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_8x8_SRGB_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_10x5_UNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_10x5_SRGB_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_10x6_UNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_10x6_SRGB_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_10x8_UNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_10x8_SRGB_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_10x10_UNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_10x10_SRGB_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_12x10_UNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_12x10_SRGB_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_12x12_UNORM_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_12x12_SRGB_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_G8B8G8R8_422_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_B8G8R8G8_422_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_G8_B8R8_2PLANE_420_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_G8_B8R8_2PLANE_422_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_R10X6_UNORM_PACK16);
        FORMAT_TO_STRING(VK_FORMAT_R10X6G10X6_UNORM_2PACK16);
        FORMAT_TO_STRING(VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16);
        FORMAT_TO_STRING(VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16);
        FORMAT_TO_STRING(VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16);
        FORMAT_TO_STRING(VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16);
        FORMAT_TO_STRING(VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16);
        FORMAT_TO_STRING(VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16);
        FORMAT_TO_STRING(VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16);
        FORMAT_TO_STRING(VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16);
        FORMAT_TO_STRING(VK_FORMAT_R12X4_UNORM_PACK16);
        FORMAT_TO_STRING(VK_FORMAT_R12X4G12X4_UNORM_2PACK16);
        FORMAT_TO_STRING(VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16);
        FORMAT_TO_STRING(VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16);
        FORMAT_TO_STRING(VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16);
        FORMAT_TO_STRING(VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16);
        FORMAT_TO_STRING(VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16);
        FORMAT_TO_STRING(VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16);
        FORMAT_TO_STRING(VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16);
        FORMAT_TO_STRING(VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16);
        FORMAT_TO_STRING(VK_FORMAT_G16B16G16R16_422_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_B16G16R16G16_422_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_G16_B16R16_2PLANE_420_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_G16_B16R16_2PLANE_422_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_G8_B8R8_2PLANE_444_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16);
        FORMAT_TO_STRING(VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16);
        FORMAT_TO_STRING(VK_FORMAT_G16_B16R16_2PLANE_444_UNORM);
        FORMAT_TO_STRING(VK_FORMAT_A4R4G4B4_UNORM_PACK16);
        FORMAT_TO_STRING(VK_FORMAT_A4B4G4R4_UNORM_PACK16);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK);
        FORMAT_TO_STRING(VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG);
        FORMAT_TO_STRING(VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG);
        FORMAT_TO_STRING(VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG);
        FORMAT_TO_STRING(VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG);
        FORMAT_TO_STRING(VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG);
        FORMAT_TO_STRING(VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG);
        FORMAT_TO_STRING(VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG);
        FORMAT_TO_STRING(VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG);
        default:
            return "VK_FORMAT_UNDEFINED";
    }
}

#undef FORMAT_TO_STRING

#define STRING_TO_UINT(bit) \
    { #bit, bit }

static inline uint32_t VkStringToUint(const std::string &input_value) {
    static const std::unordered_map<std::string, uint32_t> map = {
        // VkSampleCountFlagBits
        STRING_TO_UINT(VK_SAMPLE_COUNT_1_BIT),
        STRING_TO_UINT(VK_SAMPLE_COUNT_2_BIT),
        STRING_TO_UINT(VK_SAMPLE_COUNT_4_BIT),
        STRING_TO_UINT(VK_SAMPLE_COUNT_8_BIT),
        STRING_TO_UINT(VK_SAMPLE_COUNT_16_BIT),
        STRING_TO_UINT(VK_SAMPLE_COUNT_32_BIT),
        STRING_TO_UINT(VK_SAMPLE_COUNT_64_BIT),
        // VkResolveModeFlagBits
        STRING_TO_UINT(VK_RESOLVE_MODE_NONE),
        STRING_TO_UINT(VK_RESOLVE_MODE_SAMPLE_ZERO_BIT),
        STRING_TO_UINT(VK_RESOLVE_MODE_AVERAGE_BIT),
        STRING_TO_UINT(VK_RESOLVE_MODE_MIN_BIT),
        STRING_TO_UINT(VK_RESOLVE_MODE_MAX_BIT),
        STRING_TO_UINT(VK_RESOLVE_MODE_NONE_KHR),
        STRING_TO_UINT(VK_RESOLVE_MODE_SAMPLE_ZERO_BIT_KHR),
        STRING_TO_UINT(VK_RESOLVE_MODE_AVERAGE_BIT_KHR),
        STRING_TO_UINT(VK_RESOLVE_MODE_MIN_BIT_KHR),
        STRING_TO_UINT(VK_RESOLVE_MODE_MAX_BIT_KHR),
        // VkShaderStageFlagBits
        STRING_TO_UINT(VK_SHADER_STAGE_VERTEX_BIT),
        STRING_TO_UINT(VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT),
        STRING_TO_UINT(VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT),
        STRING_TO_UINT(VK_SHADER_STAGE_GEOMETRY_BIT),
        STRING_TO_UINT(VK_SHADER_STAGE_FRAGMENT_BIT),
        STRING_TO_UINT(VK_SHADER_STAGE_COMPUTE_BIT),
        STRING_TO_UINT(VK_SHADER_STAGE_ALL_GRAPHICS),
        STRING_TO_UINT(VK_SHADER_STAGE_ALL),
        STRING_TO_UINT(VK_SHADER_STAGE_RAYGEN_BIT_KHR),
        STRING_TO_UINT(VK_SHADER_STAGE_ANY_HIT_BIT_KHR),
        STRING_TO_UINT(VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR),
        STRING_TO_UINT(VK_SHADER_STAGE_MISS_BIT_KHR),
        STRING_TO_UINT(VK_SHADER_STAGE_INTERSECTION_BIT_KHR),
        STRING_TO_UINT(VK_SHADER_STAGE_CALLABLE_BIT_KHR),
        STRING_TO_UINT(VK_SHADER_STAGE_TASK_BIT_NV),
        STRING_TO_UINT(VK_SHADER_STAGE_MESH_BIT_NV),
        STRING_TO_UINT(VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI),
        STRING_TO_UINT(VK_SHADER_STAGE_RAYGEN_BIT_NV),
        STRING_TO_UINT(VK_SHADER_STAGE_ANY_HIT_BIT_NV),
        STRING_TO_UINT(VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV),
        STRING_TO_UINT(VK_SHADER_STAGE_MISS_BIT_NV),
        STRING_TO_UINT(VK_SHADER_STAGE_INTERSECTION_BIT_NV),
        STRING_TO_UINT(VK_SHADER_STAGE_CALLABLE_BIT_NV),
        // VkSubgroupFeatureFlagBits
        STRING_TO_UINT(VK_SUBGROUP_FEATURE_BASIC_BIT),
        STRING_TO_UINT(VK_SUBGROUP_FEATURE_VOTE_BIT),
        STRING_TO_UINT(VK_SUBGROUP_FEATURE_ARITHMETIC_BIT),
        STRING_TO_UINT(VK_SUBGROUP_FEATURE_BALLOT_BIT),
        STRING_TO_UINT(VK_SUBGROUP_FEATURE_SHUFFLE_BIT),
        STRING_TO_UINT(VK_SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT),
        STRING_TO_UINT(VK_SUBGROUP_FEATURE_CLUSTERED_BIT),
        STRING_TO_UINT(VK_SUBGROUP_FEATURE_QUAD_BIT),
        STRING_TO_UINT(VK_SUBGROUP_FEATURE_PARTITIONED_BIT_NV),
        // VkShaderFloatControlsIndependence
        STRING_TO_UINT(VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_32_BIT_ONLY),
        STRING_TO_UINT(VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL),
        STRING_TO_UINT(VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_NONE),
        STRING_TO_UINT(VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_32_BIT_ONLY_KHR),
        STRING_TO_UINT(VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL_KHR),
        STRING_TO_UINT(VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_NONE_KHR),
        // VkPointClippingBehavior
        STRING_TO_UINT(VK_POINT_CLIPPING_BEHAVIOR_ALL_CLIP_PLANES),
        STRING_TO_UINT(VK_POINT_CLIPPING_BEHAVIOR_USER_CLIP_PLANES_ONLY),
        STRING_TO_UINT(VK_POINT_CLIPPING_BEHAVIOR_ALL_CLIP_PLANES_KHR),
        STRING_TO_UINT(VK_POINT_CLIPPING_BEHAVIOR_USER_CLIP_PLANES_ONLY_KHR),
    };
    auto it = map.find(input_value);
    if (it != map.end()) {
        return it->second;
    }
    return 0;
}

#undef STRING_TO_UINT

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
typedef std::vector<VkExtensionProperties> ArrayOfVkExtensionProperties;

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
        const auto result = map_.emplace(pd, PhysicalDeviceData(instance));
        assert(result.second);  // true=insertion, false=replacement
        auto iter = result.first;
        PhysicalDeviceData *pdd = &iter->second;
        assert(Find(pd) == pdd);  // Verify we get the same instance we just inserted.
        DebugPrintf("PhysicalDeviceData::Create()\n");
        return *pdd;
    }

    static void Destroy(const VkPhysicalDevice pd) {
        assert(Find(pd));
        map_.erase(pd);
        DebugPrintf("PhysicalDeviceData::Destroy()\n");
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

    uint32_t combineExtensionLists() {
        switch (extension_list_combination_mode_) {
            case SET_CHECK_SUPPORT:
            case SET_FROM_DEVICE:
                simulation_extensions_ = device_extensions_;
                break;
            case SET_FROM_PROFILE:
            case SET_FROM_PROFILE_OVERRIDE:
                simulation_extensions_ = arrayof_extension_properties_;
                break;
            default:
                simulation_extensions_ = device_extensions_;
        }

        return static_cast<uint32_t>(simulation_extensions_.size());
    }

    VkInstance instance() const { return instance_; }

    ArrayOfVkExtensionProperties device_extensions_;
    ArrayOfVkFormatProperties device_formats_;

    SetCombinationMode extension_list_combination_mode_;
    ArrayOfVkExtensionProperties simulation_extensions_;

    SetCombinationMode format_list_combination_mode_;
    SetCombinationMode format_properties_combination_mode_;

    VkPhysicalDeviceProperties physical_device_properties_;
    VkPhysicalDeviceFeatures physical_device_features_;
    VkPhysicalDeviceMemoryProperties physical_device_memory_properties_;
    VkSurfaceCapabilitiesKHR surface_capabilities_;
    ArrayOfVkFormatProperties arrayof_format_properties_;
    ArrayOfVkExtensionProperties arrayof_extension_properties_;

    // Vulkan 1.3 structs
    VkPhysicalDeviceVulkan13Properties physical_device_vulkan_1_3_properties_;
    VkPhysicalDeviceVulkan13Features physical_device_vulkan_1_3_features_;

    // Vulkan 1.2 structs for summarizing core extension properties and features
    VkPhysicalDeviceVulkan12Properties physical_device_vulkan_1_2_properties_;
    VkPhysicalDeviceVulkan12Features physical_device_vulkan_1_2_features_;

    // Vulkan 1.1 structs
    VkPhysicalDeviceVulkan11Properties physical_device_vulkan_1_1_properties_;
    VkPhysicalDeviceVulkan11Features physical_device_vulkan_1_1_features_;

    VkPhysicalDeviceProtectedMemoryProperties physical_device_protected_memory_properties_;
    VkPhysicalDeviceProtectedMemoryFeatures physical_device_protected_memory_features_;

    VkPhysicalDeviceShaderDrawParametersFeatures physical_device_shader_draw_parameters_features_;

    VkPhysicalDeviceSubgroupProperties physical_device_subgroup_properties_;

    // VK_KHR_8bit_storage structs
    VkPhysicalDevice8BitStorageFeaturesKHR physical_device_8bit_storage_features_;

    // VK_KHR_16bit_storage structs
    VkPhysicalDevice16BitStorageFeaturesKHR physical_device_16bit_storage_features_;

    // VK_KHR_buffer_device_address structs
    VkPhysicalDeviceBufferDeviceAddressFeaturesKHR physical_device_buffer_device_address_features_;

    // VK_KHR_depth_stencil_resolve structs
    VkPhysicalDeviceDepthStencilResolvePropertiesKHR physical_device_depth_stencil_resolve_properties_;

    // VK_EXT_descriptor_indexing structs
    VkPhysicalDeviceDescriptorIndexingPropertiesEXT physical_device_descriptor_indexing_properties_;
    VkPhysicalDeviceDescriptorIndexingFeaturesEXT physical_device_descriptor_indexing_features_;

    // VK_EXT_host_query_reset structs
    VkPhysicalDeviceHostQueryResetFeaturesEXT physical_device_host_query_reset_features_;

    // VK_KHR_imageless_framebuffer structs
    VkPhysicalDeviceImagelessFramebufferFeaturesKHR physical_device_imageless_framebuffer_features_;

    // VK_KHR_maintenance3 structs
    VkPhysicalDeviceMaintenance3PropertiesKHR physical_device_maintenance_3_properties_;

    // VK_KHR_maintenance4 structs
    VkPhysicalDeviceMaintenance4FeaturesKHR physical_device_maintenance_4_features_;
    VkPhysicalDeviceMaintenance4PropertiesKHR physical_device_maintenance_4_properties_;

    // VK_KHR_multiview structs
    VkPhysicalDeviceMultiviewPropertiesKHR physical_device_multiview_properties_;
    VkPhysicalDeviceMultiviewFeaturesKHR physical_device_multiview_features_;

    // VK_KHR_portability_subset structs
    VkPhysicalDevicePortabilitySubsetPropertiesKHR physical_device_portability_subset_properties_;
    VkPhysicalDevicePortabilitySubsetFeaturesKHR physical_device_portability_subset_features_;

    // VK_EXT_sampler_filter_minmax structs
    VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT physical_device_sampler_filter_minmax_properties_;

    // VK_KHR_sampler_ycbcr_conversion structs
    VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR physical_device_sampler_ycbcr_conversion_features_;

    // VK_EXT_scalar_block_layout structs
    VkPhysicalDeviceScalarBlockLayoutFeaturesEXT physical_device_scalar_block_layout_features_;

    // VK_KHR_separate_depth_stencil_layouts structs
    VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR physical_device_separate_depth_stencil_layouts_features_;

    // VK_KHR_shader_atomic_int64 structs
    VkPhysicalDeviceShaderAtomicInt64FeaturesKHR physical_device_shader_atomic_int64_features_;

    // VK_KHR_shader_float_controls structs
    VkPhysicalDeviceFloatControlsPropertiesKHR physical_device_float_controls_properties_;

    // VK_KHR_shader_float16_int8 structs
    VkPhysicalDeviceShaderFloat16Int8FeaturesKHR physical_device_shader_float16_int8_features_;

    // VK_KHR_shader_subgroup_extended_types structs
    VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR physical_device_shader_subgroup_extended_types_features_;

    // VK_KHR_timeline_semaphore structs
    VkPhysicalDeviceTimelineSemaphorePropertiesKHR physical_device_timeline_semaphore_properties_;
    VkPhysicalDeviceTimelineSemaphoreFeaturesKHR physical_device_timeline_semaphore_features_;

    // VK_KHR_uniform_buffer_standard_layout structs
    VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR physical_device_uniform_buffer_standard_layout_features_;

    // VK_KHR_variable_pointers structs
    VkPhysicalDeviceVariablePointersFeaturesKHR physical_device_variable_pointers_features_;

    // VK_KHR_vulkan_memory_model structs
    VkPhysicalDeviceVulkanMemoryModelFeaturesKHR physical_device_vulkan_memory_model_features_;

    // VK_KHR_zero_initialize_workgroup_memory structs
    VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR physical_device_zero_initialize_workgroup_memory_features_;

    // VK_KHR_acceleration_structure structs
    VkPhysicalDeviceAccelerationStructureFeaturesKHR physical_device_acceleration_structure_features_;
    VkPhysicalDeviceAccelerationStructurePropertiesKHR physical_device_acceleration_structure_properties_;

    // VK_KHR_performance_query structs
    VkPhysicalDevicePerformanceQueryFeaturesKHR physical_device_performance_query_features_;
    VkPhysicalDevicePerformanceQueryPropertiesKHR physical_device_performance_query_properties_;

    // VK_KHR_pipeline_executable_properties structs
    VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR physical_device_pipeline_executable_properties_features_;

    // VK_KHR_present_id structs
    VkPhysicalDevicePresentIdFeaturesKHR physical_device_present_id_features_;

    // VK_KHR_present_wait structs
    VkPhysicalDevicePresentWaitFeaturesKHR physical_device_present_wait_features_;

    // VK_KHR_push_descriptor structs
    VkPhysicalDevicePushDescriptorPropertiesKHR physical_device_push_descriptor_properites_;

    // VK_KHR_ray_query structs
    VkPhysicalDeviceRayQueryFeaturesKHR physical_device_ray_query_features_;

    // VK_KHR_ray_tracing_pipeline structs
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR physical_device_ray_tracing_pipeline_features_;
    VkPhysicalDeviceRayTracingPipelinePropertiesKHR physical_device_ray_tracing_pipeline_properties_;

    // VK_KHR_shader_clock structs
    VkPhysicalDeviceShaderClockFeaturesKHR physical_device_shader_clock_features_;

    // VK_KHR_shader_integer_dot_product structs
    VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR physical_device_shader_integer_dot_product_features_;
    VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR physical_device_shader_integer_dot_products_properties_;

    // VK_KHR_shader_subgroup_uniform_control_flow structs
    VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR physical_device_shader_subgroup_uniform_control_flow_features_;

    // VK_KHR_shader_terminate_invocation structs
    VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR physical_device_shader_terminate_invocation_features_;

    // VK_KHR_synchronization2 structs
    VkPhysicalDeviceSynchronization2FeaturesKHR physical_device_synchronization2_features_;

    // VK_KHR_workgroup_memory_explicit_layout structs
    VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR physical_device_workgroup_memory_explicit_layout_features_;

    // VK_EXT_4444_formats structs
    VkPhysicalDevice4444FormatsFeaturesEXT physical_device_4444_formats_features_;

    // VK_EXT_astc_decode_mode structs
    VkPhysicalDeviceASTCDecodeFeaturesEXT physical_device_astc_decode_features_;

    // VK_EXT_blend_operation_advanced structs
    VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT physical_device_blend_operation_advanced_features_;
    VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT physical_device_blend_operation_advanced_properties_;

    // VK_EXT_border_color_swizzle structs
    VkPhysicalDeviceBorderColorSwizzleFeaturesEXT physical_device_border_color_swizzle_features_;

    // VK_EXT_color_write_enable structs
    VkPhysicalDeviceColorWriteEnableFeaturesEXT physical_device_color_write_enable_features_;

    // VK_EXT_conditional_rendering structs
    VkPhysicalDeviceConditionalRenderingFeaturesEXT physical_device_conditional_rendering_features_;

    // VK_EXT_conservative_rasterization structs
    VkPhysicalDeviceConservativeRasterizationPropertiesEXT physical_device_conservative_rasterization_properties_;

    // VK_EXT_custom_border_color structs
    VkPhysicalDeviceCustomBorderColorFeaturesEXT physical_device_custom_border_color_features_;
    VkPhysicalDeviceCustomBorderColorPropertiesEXT physical_device_custom_border_color_properties_;

    // VK_EXT_depth_clip_enable structs
    VkPhysicalDeviceDepthClipEnableFeaturesEXT physical_device_depth_clip_enable_features_ext_;

    // VK_EXT_device_memory_report structs
    VkPhysicalDeviceDeviceMemoryReportFeaturesEXT physical_device_device_memory_report_features_;

    // VK_EXT_discard_rectangles structs
    VkPhysicalDeviceDiscardRectanglePropertiesEXT physical_device_discard_rectangle_properties_;

    // VK_EXT_extended_dynamic_state structs
    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT physical_device_extended_dynamic_state_features_;

    // VK_EXT_extended_dynamic_state2 structs
    VkPhysicalDeviceExtendedDynamicState2FeaturesEXT physical_device_extended_dynamic_state2_features_;

    // VK_EXT_external_memory_host structs
    VkPhysicalDeviceExternalMemoryHostPropertiesEXT physical_device_external_memory_host_properties_;

    // VK_EXT_fragment_density_map structs
    VkPhysicalDeviceFragmentDensityMapFeaturesEXT physical_device_fragment_density_map_features_;
    VkPhysicalDeviceFragmentDensityMapPropertiesEXT physical_device_fragment_density_map_properties_;

    // VK_EXT_fragment_shader_interlock structs
    VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT physical_device_fragment_shader_interlock_features_;

    // VK_EXT_global_priority_query structs
    VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT physical_device_global_priority_query_features_;

    // VK_EXT_image_robustness structs
    VkPhysicalDeviceImageRobustnessFeaturesEXT physical_device_image_robustness_features_;

    // VK_EXT_index_type_uint8 structs
    VkPhysicalDeviceIndexTypeUint8FeaturesEXT physical_device_index_type_uint8_features_;

    // VK_EXT_inline_uniform_block structs
    VkPhysicalDeviceInlineUniformBlockFeaturesEXT physical_device_inline_uniform_block_features_;
    VkPhysicalDeviceInlineUniformBlockPropertiesEXT physical_device_inline_uniform_block_properties_;

    // VK_EXT_line_rasterization structs
    VkPhysicalDeviceLineRasterizationFeaturesEXT physical_device_line_rasterization_features_;
    VkPhysicalDeviceLineRasterizationPropertiesEXT physical_device_line_rasterization_properties_;

    // VK_EXT_memory_priority structs
    VkPhysicalDeviceMemoryPriorityFeaturesEXT physical_device_memory_priority_features_;

    // VK_EXT_multi_draw structs
    VkPhysicalDeviceMultiDrawFeaturesEXT physical_device_multi_draw_features_;
    VkPhysicalDeviceMultiDrawPropertiesEXT physical_device_multi_draw_properties_;

    // VK_EXT_pageable_device_local_memory structs
    VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT physical_device_pageable_device_local_memory_features_;

    // VK_EXT_pipeline_creation_cache_control structs
    VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT physical_device_pipeline_creation_cache_control_features_;

    // VK_EXT_primitive_topology_list_restart structs
    VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT physical_device_primitive_topology_list_restart_features_;

    // VK_EXT_private_data structs
    VkPhysicalDevicePrivateDataFeaturesEXT physical_device_private_data_features_;

    // VK_EXT_provoking_vertex structs
    VkPhysicalDeviceProvokingVertexFeaturesEXT physical_device_provoking_vertex_features_;
    VkPhysicalDeviceProvokingVertexPropertiesEXT physical_device_provoking_vertex_properties_;

    // VK_EXT_rgba10x6_formats structs
    VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT physical_device_rgba10x6_formats_features_;

    // VK_EXT_robustness2 structs
    VkPhysicalDeviceRobustness2FeaturesEXT physical_device_robustness_2_features_;
    VkPhysicalDeviceRobustness2PropertiesEXT physical_device_robustness_2_properties_;

    // VK_EXT_sample_locations structs
    VkPhysicalDeviceSampleLocationsPropertiesEXT physical_device_sample_locations_properties_;

    // VK_EXT_shader_atomic_float structs
    VkPhysicalDeviceShaderAtomicFloatFeaturesEXT physical_device_shader_atomic_float_features_;

    // VK_EXT_shader_atomic_float2 structs
    VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT physical_device_shader_atomic_float2_features_;

    // VK_EXT_shader_demote_to_helper_invocation structs
    VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT physical_device_shader_demote_to_helper_invocation_features_;

    // VK_EXT_shader_image_atomic_int64 structs
    VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT physical_device_shader_image_atomic_int64_features_;

    // VK_EXT_subgroup_size_control structs
    VkPhysicalDeviceSubgroupSizeControlFeaturesEXT physical_device_subgroup_size_control_features_;
    VkPhysicalDeviceSubgroupSizeControlPropertiesEXT physical_device_subgroup_size_control_properties_;

    // VK_EXT_texel_buffer_alignment structs
    VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT physical_device_texel_buffer_alignment_features_;
    VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT physical_device_texel_buffer_alignment_properties_;

    // VK_EXT_texture_compression_astc_hdr structs
    VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT physical_device_texture_compression_astc_hdr_features_;

    // VK_EXT_transform_feedback structs
    VkPhysicalDeviceTransformFeedbackFeaturesEXT physical_device_transform_feedback_features_;
    VkPhysicalDeviceTransformFeedbackPropertiesEXT physical_device_transform_feedback_properties_;

    // VK_EXT_vertex_attribute_divisor structs
    VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT physical_device_vertex_attribute_divisor_features_;
    VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT physical_device_vertex_attirbute_divisor_properties_;

    // VK_EXT_vertex_input_dynamic_state structs
    VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT physical_device_vertex_input_dynamic_state_features_;

    // VK_EXT_ycbcr_2plane_444_formats structs
    VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT physical_device_ycbcr_2plane_444_formats_features_;

    // VK_EXT_ycbcr_image_arrays structs
    VkPhysicalDeviceYcbcrImageArraysFeaturesEXT physical_device_ycbcr_image_arrays_features_;

    // VK_KHR_fragment_shading_rate structs
    VkPhysicalDeviceFragmentShadingRateFeaturesKHR physical_device_fragment_shading_rate_features_;
    VkPhysicalDeviceFragmentShadingRatePropertiesKHR physical_device_fragment_shading_rate_properties_;

    // VK_AMD_device_coherent_memory structs
    VkPhysicalDeviceCoherentMemoryFeaturesAMD physical_device_coherent_memory_features_;

    // VK_AMD_shader_core_properties structs
    VkPhysicalDeviceShaderCorePropertiesAMD physical_device_shader_core_properties_;

    // VK_AMD_shader_core_properties2 structs
    VkPhysicalDeviceShaderCoreProperties2AMD physical_device_shader_core_properties_2_;

    // VK_HUAWEI_invocation_mask structs
    VkPhysicalDeviceInvocationMaskFeaturesHUAWEI physical_device_invocation_mask_features_;

    // VK_HUAWEI_subpass_shading structs
    VkPhysicalDeviceSubpassShadingFeaturesHUAWEI physical_device_subpass_shading_features_;
    VkPhysicalDeviceSubpassShadingPropertiesHUAWEI physical_device_subpass_shading_properties_;

    // VK_INTEL_shader_integer_functions2 structs
    VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL physical_device_shader_integer_functions_2_features_;

    // VK_NV_compute_shader_derivatives structs
    VkPhysicalDeviceComputeShaderDerivativesFeaturesNV physical_device_compute_shader_derivatives_features_;

    // VK_NV_cooperative_matrix structs
    VkPhysicalDeviceCooperativeMatrixFeaturesNV physical_device_cooperative_matrix_features_;
    VkPhysicalDeviceCooperativeMatrixPropertiesNV physical_device_cooperative_matrix_properties_;

    // VK_NV_corner_sampled_image structs
    VkPhysicalDeviceCornerSampledImageFeaturesNV physical_device_corner_sampled_image_features_;

    // VK_NV_coverage_reduction_mode structs
    VkPhysicalDeviceCoverageReductionModeFeaturesNV physical_device_coverage_reduction_mode_features_;

    // VK_NV_dedicated_allocation_image_aliasing structs
    VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV physical_device_dedicated_allocation_image_aliasing_features_;

    // VK_NV_device_diagnostics_config structs
    VkPhysicalDeviceDiagnosticsConfigFeaturesNV physical_device_diagnostics_config_features_;

    // VK_NV_device_generated_commands structs
    VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV physical_device_device_generated_commands_features_;
    VkPhysicalDeviceDeviceGeneratedCommandsPropertiesNV physical_device_device_generated_commands_properties_;

    // VK_NV_external_memory_rdma structs
    VkPhysicalDeviceExternalMemoryRDMAFeaturesNV physical_device_external_memory_rdma_features_;

    // VK_NV_fragment_shader_barycentric structs
    VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV physical_device_fragment_shader_barycentric_features_;

    // VK_NV_fragment_shading_rate_enums structs
    VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV physical_device_fragment_shading_rate_enums_features_;
    VkPhysicalDeviceFragmentShadingRateEnumsPropertiesNV physical_device_fragment_shading_rate_enums_properties_;

    // VK_NV_inherited_viewport_scissor structs
    VkPhysicalDeviceInheritedViewportScissorFeaturesNV physical_device_inherited_viewport_scissor_features_;

    // VK_NV_mesh_shader structs
    VkPhysicalDeviceMeshShaderFeaturesNV physical_device_mesh_shader_features_;
    VkPhysicalDeviceMeshShaderPropertiesNV physical_device_mesh_shader_properties_;

    // VK_NV_ray_tracing structs
    VkPhysicalDeviceRayTracingPropertiesNV physical_device_ray_tracing_properties_;

    // VK_NV_ray_tracing_motion_blur structs
    VkPhysicalDeviceRayTracingMotionBlurFeaturesNV physical_device_ray_tracing_motiuon_blur_features_;

    // VK_NV_representative_fragment_test structs
    VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV physical_device_representative_fragment_test_features_;

    // VK_NV_scissor_exclusive structs
    VkPhysicalDeviceExclusiveScissorFeaturesNV physical_device_exclusive_scissor_features_;

    // VK_NV_shader_image_footprint structs
    VkPhysicalDeviceShaderImageFootprintFeaturesNV physical_device_shader_image_footprint_features_;

    // VK_NV_shader_sm_builtins structs
    VkPhysicalDeviceShaderSMBuiltinsFeaturesNV physical_device_shader_sm_builtins_features_;
    VkPhysicalDeviceShaderSMBuiltinsPropertiesNV physical_device_shader_sm_builtins_properties_;

    // VK_NV_shading_rate_image structs
    VkPhysicalDeviceShadingRateImageFeaturesNV physical_device_shading_rate_image_features_;
    VkPhysicalDeviceShadingRateImagePropertiesNV physical_device_shading_rate_image_properties_;

    // VK_VALVE_mutable_descriptor_type structs
    VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE physical_device_mutable_descriptor_type_features_;

    // VK_KHR_dynamic_rendering structs
    VkPhysicalDeviceDynamicRenderingFeaturesKHR physical_device_dynamic_rendering_features_;

    // VK_EXT_image_view_min_lod structs
    VkPhysicalDeviceImageViewMinLodFeaturesEXT physical_device_image_view_min_lod_features_;

    // VK_EXT_fragment_density_map2 structs
    VkPhysicalDeviceFragmentDensityMap2FeaturesEXT physical_device_fragment_density_map_2_features_;
    VkPhysicalDeviceFragmentDensityMap2PropertiesEXT physical_device_fragment_density_map_2_properties_;

    // VK_QCOM_fragment_density_map_offset structs
    VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM physical_device_fragment_density_map_offset_features_;
    VkPhysicalDeviceFragmentDensityMapOffsetPropertiesQCOM physical_device_fragment_density_map_offset_properties_;

    // VK_EXT_depth_clip_control structs
    VkPhysicalDeviceDepthClipControlFeaturesEXT physical_device_depth_clip_control_features_;

    // VK_ARM_rasterization_order_attachment_access structs
    VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM physical_device_rasterization_order_attachment_access_features_;

    // VK_NV_linear_color_attachment structs
    VkPhysicalDeviceLinearColorAttachmentFeaturesNV physical_device_linear_color_attachment_features_;

   private:
    PhysicalDeviceData() = delete;
    PhysicalDeviceData &operator=(const PhysicalDeviceData &) = delete;
    PhysicalDeviceData(VkInstance instance) : instance_(instance) {
        extension_list_combination_mode_ = SET_CHECK_SUPPORT;

        format_list_combination_mode_ = SET_CHECK_SUPPORT;
        format_properties_combination_mode_ = SET_CHECK_SUPPORT;

        physical_device_properties_ = {};
        physical_device_features_ = {};
        physical_device_memory_properties_ = {};
        surface_capabilities_ = {};

        // Vulkan 1.1 structs
        physical_device_vulkan_1_3_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES};
        physical_device_vulkan_1_3_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};

        // Vulkan 1.2 structs for summarizing core extension properties and features
        physical_device_vulkan_1_2_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES};
        physical_device_vulkan_1_2_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};

        // Vulkan 1.1 structs
        physical_device_vulkan_1_1_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES};
        physical_device_vulkan_1_1_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES};

        physical_device_protected_memory_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_PROPERTIES};
        physical_device_protected_memory_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES};

        physical_device_shader_draw_parameters_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES};

        physical_device_subgroup_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES};

        // VK_KHR_8bit_storage structs
        physical_device_8bit_storage_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR};

        // VK_KHR_16bit_storage structs
        physical_device_16bit_storage_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES_KHR};

        // VK_KHR_buffer_device_address structs
        physical_device_buffer_device_address_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_EXT};

        // VK_KHR_depth_stencil_resolve structs
        physical_device_depth_stencil_resolve_properties_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR};

        // VK_EXT_descriptor_indexing structs
        physical_device_descriptor_indexing_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT};
        physical_device_descriptor_indexing_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT};

        // VK_EXT_host_query_reset structs
        physical_device_host_query_reset_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT};

        // VK_KHR_imageless_framebuffer structs
        physical_device_imageless_framebuffer_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR};

        // VK_KHR_maintenance3 structs
        physical_device_maintenance_3_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES_KHR};

        // VK_KHR_maintenance4 structs
        physical_device_maintenance_4_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES_KHR};
        physical_device_maintenance_4_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_PROPERTIES_KHR};

        // VK_KHR_multiview structs
        physical_device_multiview_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR};
        physical_device_multiview_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR};

        // VK_KHR_portability_subset structs
        physical_device_portability_subset_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES_KHR};
        physical_device_portability_subset_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR};

        // VK_EXT_sampler_filter_minmax structs
        physical_device_sampler_filter_minmax_properties_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_FILTER_MINMAX_PROPERTIES_EXT};

        // VK_KHR_sampler_ycbcr_conversion structs
        physical_device_sampler_ycbcr_conversion_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR};

        // VK_KHR_separate_depth_stencil_layouts structs
        physical_device_separate_depth_stencil_layouts_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES_KHR};

        // VK_EXT_scalar_block_layout structs
        physical_device_scalar_block_layout_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES_EXT};

        // VK_KHR_shader_atomic_int64 structs
        physical_device_shader_atomic_int64_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES_KHR};

        // VK_KHR_shader_float_controls structs
        physical_device_float_controls_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT_CONTROLS_PROPERTIES_KHR};

        // VK_KHR_shader_float16_int8 structs
        physical_device_shader_float16_int8_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR};

        // VK_KHR_shader_subgroup_extended_types structs
        physical_device_shader_subgroup_extended_types_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR};

        // VK_KHR_timeline_semaphore structs
        physical_device_timeline_semaphore_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_PROPERTIES_KHR};
        physical_device_timeline_semaphore_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR};

        // VK_KHR_uniform_buffer_standard_layout structs
        physical_device_uniform_buffer_standard_layout_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR};

        // VK_KHR_variable_pointers structs
        physical_device_variable_pointers_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES_KHR};

        // VK_KHR_vulkan_memory_model structs
        physical_device_vulkan_memory_model_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES_KHR};

        // VK_KHR_zero_initialize_workgroup_memory structs
        physical_device_zero_initialize_workgroup_memory_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_WORKGROUP_MEMORY_FEATURES_KHR};

        // VK_KHR_acceleration_structure structs
        physical_device_acceleration_structure_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR};
        physical_device_acceleration_structure_properties_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR};

        // VK_KHR_performance_query structs
        physical_device_performance_query_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_FEATURES_KHR};
        physical_device_performance_query_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_PROPERTIES_KHR};

        // VK_KHR_pipeline_executable_properties structs
        physical_device_pipeline_executable_properties_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_EXECUTABLE_PROPERTIES_FEATURES_KHR};

        // VK_KHR_present_id structs
        physical_device_present_id_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_ID_FEATURES_KHR};

        // VK_KHR_present_wait structs
        physical_device_present_wait_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_WAIT_FEATURES_KHR};

        // VK_KHR_push_descriptor structs
        physical_device_push_descriptor_properites_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR};

        // VK_KHR_ray_query structs
        physical_device_ray_query_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR};

        // VK_KHR_ray_tracing_pipeline structs
        physical_device_ray_tracing_pipeline_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR};
        physical_device_ray_tracing_pipeline_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR};

        // VK_KHR_shader_clock structs
        physical_device_shader_clock_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR};

        // VK_KHR_shader_integer_dot_product structs
        physical_device_shader_integer_dot_product_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_FEATURES_KHR};
        physical_device_shader_integer_dot_products_properties_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_PROPERTIES_KHR};

        // VK_KHR_shader_subgroup_uniform_control_flow structs
        physical_device_shader_subgroup_uniform_control_flow_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_FEATURES_KHR};

        // VK_KHR_shader_terminate_invocation structs
        physical_device_shader_terminate_invocation_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES_KHR};

        // VK_KHR_synchronization2 structs
        physical_device_synchronization2_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR};

        // VK_KHR_workgroup_memory_explicit_layout structs
        physical_device_workgroup_memory_explicit_layout_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_FEATURES_KHR};

        // VK_EXT_4444_formats structs
        physical_device_4444_formats_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_4444_FORMATS_FEATURES_EXT};

        // VK_EXT_astc_decode_mode structs
        physical_device_astc_decode_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ASTC_DECODE_FEATURES_EXT};

        // VK_EXT_blend_operation_advanced structs
        physical_device_blend_operation_advanced_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT};
        physical_device_blend_operation_advanced_properties_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_PROPERTIES_EXT};

        // VK_EXT_border_color_swizzle structs
        physical_device_border_color_swizzle_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BORDER_COLOR_SWIZZLE_FEATURES_EXT};

        // VK_EXT_color_write_enable structs
        physical_device_color_write_enable_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COLOR_WRITE_ENABLE_FEATURES_EXT};

        // VK_EXT_conditional_rendering structs
        physical_device_conditional_rendering_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONDITIONAL_RENDERING_FEATURES_EXT};

        // VK_EXT_conservative_rasterization structs
        physical_device_conservative_rasterization_properties_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONSERVATIVE_RASTERIZATION_PROPERTIES_EXT};

        // VK_EXT_custom_border_color structs
        physical_device_custom_border_color_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_FEATURES_EXT};
        physical_device_custom_border_color_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_PROPERTIES_EXT};

        // VK_EXT_depth_clip_enable structs
        physical_device_depth_clip_enable_features_ext_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT};

        // VK_EXT_device_memory_report structs
        physical_device_device_memory_report_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_MEMORY_REPORT_FEATURES_EXT};

        // VK_EXT_discard_rectangles structs
        physical_device_discard_rectangle_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DISCARD_RECTANGLE_PROPERTIES_EXT};

        // VK_EXT_extended_dynamic_state structs
        physical_device_extended_dynamic_state_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT};

        // VK_EXT_extended_dynamic_state2 structs
        physical_device_extended_dynamic_state2_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT};

        // VK_EXT_external_memory_host structs
        physical_device_external_memory_host_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_HOST_PROPERTIES_EXT};

        // VK_EXT_fragment_density_map structs
        physical_device_fragment_density_map_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_FEATURES_EXT};
        physical_device_fragment_density_map_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_PROPERTIES_EXT};

        // VK_EXT_fragment_shader_interlock structs
        physical_device_fragment_shader_interlock_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT};

        // VK_EXT_global_priority_query structs
        physical_device_global_priority_query_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GLOBAL_PRIORITY_QUERY_FEATURES_EXT};

        // VK_EXT_image_robustness structs
        physical_device_image_robustness_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES_EXT};

        // VK_EXT_index_type_uint8 structs
        physical_device_index_type_uint8_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES_EXT};

        // VK_EXT_inline_uniform_block structs
        physical_device_inline_uniform_block_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT};
        physical_device_inline_uniform_block_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_PROPERTIES_EXT};

        // VK_EXT_line_rasterization structs
        physical_device_line_rasterization_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES_EXT};
        physical_device_line_rasterization_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_PROPERTIES_EXT};

        // VK_EXT_memory_priority structs
        physical_device_memory_priority_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PRIORITY_FEATURES_EXT};

        // VK_EXT_multi_draw structs
        physical_device_multi_draw_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_FEATURES_EXT};
        physical_device_multi_draw_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_PROPERTIES_EXT};

        // VK_EXT_pageable_device_local_memory structs
        physical_device_pageable_device_local_memory_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PAGEABLE_DEVICE_LOCAL_MEMORY_FEATURES_EXT};

        // VK_EXT_pipeline_creation_cache_control structs
        physical_device_pipeline_creation_cache_control_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES_EXT};

        // VK_EXT_primitive_topology_list_restart structs
        physical_device_primitive_topology_list_restart_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVE_TOPOLOGY_LIST_RESTART_FEATURES_EXT};

        // VK_EXT_private_data structs
        physical_device_private_data_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES_EXT};

        // VK_EXT_provoking_vertex structs
        physical_device_provoking_vertex_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_FEATURES_EXT};
        physical_device_provoking_vertex_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_PROPERTIES_EXT};

        // VK_EXT_rgba10x6_formats structs
        physical_device_rgba10x6_formats_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RGBA10X6_FORMATS_FEATURES_EXT};

        // VK_EXT_robustness2 structs
        physical_device_robustness_2_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT};
        physical_device_robustness_2_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_PROPERTIES_EXT};

        // VK_EXT_sample_locations structs
        physical_device_sample_locations_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLE_LOCATIONS_PROPERTIES_EXT};

        // VK_EXT_shader_atomic_float structs
        physical_device_shader_atomic_float_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT};

        // VK_EXT_shader_atomic_float2 structs
        physical_device_shader_atomic_float2_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_2_FEATURES_EXT};

        // VK_EXT_shader_demote_to_helper_invocation structs
        physical_device_shader_demote_to_helper_invocation_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES_EXT};

        // VK_EXT_shader_image_atomic_int64 structs
        physical_device_shader_image_atomic_int64_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_ATOMIC_INT64_FEATURES_EXT};

        // VK_EXT_subgroup_size_control structs
        physical_device_subgroup_size_control_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES_EXT};
        physical_device_subgroup_size_control_properties_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES_EXT};

        // VK_EXT_texel_buffer_alignment structs
        physical_device_texel_buffer_alignment_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT};
        physical_device_texel_buffer_alignment_properties_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_PROPERTIES_EXT};

        // VK_EXT_texture_compression_astc_hdr structs
        physical_device_texture_compression_astc_hdr_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_HDR_FEATURES_EXT};

        // VK_EXT_transform_feedback structs
        physical_device_transform_feedback_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_FEATURES_EXT};
        physical_device_transform_feedback_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT};

        // VK_EXT_vertex_attribute_divisor structs
        physical_device_vertex_attribute_divisor_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT};
        physical_device_vertex_attirbute_divisor_properties_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_PROPERTIES_EXT};

        // VK_EXT_vertex_input_dynamic_state structs
        physical_device_vertex_input_dynamic_state_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_INPUT_DYNAMIC_STATE_FEATURES_EXT};

        // VK_EXT_ycbcr_2plane_444_formats structs
        physical_device_ycbcr_2plane_444_formats_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_2_PLANE_444_FORMATS_FEATURES_EXT};

        // VK_EXT_ycbcr_image_arrays structs
        physical_device_ycbcr_image_arrays_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_IMAGE_ARRAYS_FEATURES_EXT};

        // VK_KHR_fragment_shading_rate structs
        physical_device_fragment_shading_rate_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR};
        physical_device_fragment_shading_rate_properties_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR};

        // VK_AMD_device_coherent_memory structs
        physical_device_coherent_memory_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COHERENT_MEMORY_FEATURES_AMD};

        // VK_AMD_shader_core_properties structs
        physical_device_shader_core_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_AMD};

        // VK_AMD_shader_core_properties2 structs
        physical_device_shader_core_properties_2_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_2_AMD};

        // VK_HUAWEI_invocation_mask structs
        physical_device_invocation_mask_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INVOCATION_MASK_FEATURES_HUAWEI};

        // VK_HUAWEI_subpass_shading structs
        physical_device_subpass_shading_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_SHADING_FEATURES_HUAWEI};
        physical_device_subpass_shading_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_SHADING_PROPERTIES_HUAWEI};

        // VK_INTEL_shader_integer_functions2 structs
        physical_device_shader_integer_functions_2_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_FUNCTIONS_2_FEATURES_INTEL};

        // VK_NV_compute_shader_derivatives structs
        physical_device_compute_shader_derivatives_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMPUTE_SHADER_DERIVATIVES_FEATURES_NV};

        // VK_NV_cooperative_matrix structs
        physical_device_cooperative_matrix_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_NV};
        physical_device_cooperative_matrix_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_PROPERTIES_NV};

        // VK_NV_corner_sampled_image structs
        physical_device_corner_sampled_image_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CORNER_SAMPLED_IMAGE_FEATURES_NV};

        // VK_NV_coverage_reduction_mode structs
        physical_device_coverage_reduction_mode_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COVERAGE_REDUCTION_MODE_FEATURES_NV};

        // VK_NV_dedicated_allocation_image_aliasing structs
        physical_device_dedicated_allocation_image_aliasing_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEDICATED_ALLOCATION_IMAGE_ALIASING_FEATURES_NV};

        // VK_NV_device_diagnostics_config structs
        physical_device_diagnostics_config_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DIAGNOSTICS_CONFIG_FEATURES_NV};

        // VK_NV_device_generated_commands structs
        physical_device_device_generated_commands_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_FEATURES_NV};
        physical_device_device_generated_commands_properties_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_PROPERTIES_NV};

        // VK_NV_external_memory_rdma structs
        physical_device_external_memory_rdma_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_RDMA_FEATURES_NV};

        // VK_NV_fragment_shader_barycentric structs
        physical_device_fragment_shader_barycentric_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_BARYCENTRIC_FEATURES_NV};

        // VK_NV_fragment_shading_rate_enums structs
        physical_device_fragment_shading_rate_enums_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_ENUMS_FEATURES_NV};
        physical_device_fragment_shading_rate_enums_properties_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_ENUMS_PROPERTIES_NV};

        // VK_NV_inherited_viewport_scissor structs
        physical_device_inherited_viewport_scissor_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INHERITED_VIEWPORT_SCISSOR_FEATURES_NV};

        // VK_NV_mesh_shader structs
        physical_device_mesh_shader_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_NV};
        physical_device_mesh_shader_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_NV};

        // VK_NV_ray_tracing structs
        physical_device_ray_tracing_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_NV};

        // VK_NV_ray_tracing_motion_blur structs
        physical_device_ray_tracing_motiuon_blur_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_MOTION_BLUR_FEATURES_NV};

        // VK_NV_representative_fragment_test structs
        physical_device_representative_fragment_test_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_REPRESENTATIVE_FRAGMENT_TEST_FEATURES_NV};

        // VK_NV_scissor_exclusive structs
        physical_device_exclusive_scissor_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXCLUSIVE_SCISSOR_FEATURES_NV};

        // VK_NV_shader_image_footprint structs
        physical_device_shader_image_footprint_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_FOOTPRINT_FEATURES_NV};

        // VK_NV_shader_sm_builtins structs
        physical_device_shader_sm_builtins_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_FEATURES_NV};
        physical_device_shader_sm_builtins_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_PROPERTIES_NV};

        // VK_NV_shading_rate_image structs
        physical_device_shading_rate_image_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_FEATURES_NV};
        physical_device_shading_rate_image_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_PROPERTIES_NV};

        // VK_VALVE_mutable_descriptor_type structs
        physical_device_mutable_descriptor_type_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MUTABLE_DESCRIPTOR_TYPE_FEATURES_VALVE};

        // VK_KHR_dynamic_rendering structs
        physical_device_dynamic_rendering_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR};

        // VK_EXT_image_view_min_lod structs
        physical_device_image_view_min_lod_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_VIEW_MIN_LOD_FEATURES_EXT};

        // VK_EXT_fragment_density_map2 structs
        physical_device_fragment_density_map_2_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_FEATURES_EXT};
        physical_device_fragment_density_map_2_properties_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_PROPERTIES_EXT};

        // VK_QCOM_fragment_density_map_offset structs
        physical_device_fragment_density_map_offset_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_OFFSET_FEATURES_QCOM};
        physical_device_fragment_density_map_offset_properties_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_OFFSET_PROPERTIES_QCOM};

        // VK_EXT_depth_clip_control structs
        physical_device_depth_clip_control_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_CONTROL_FEATURES_EXT};

        // VK_ARM_rasterization_order_attachment_access structs
        physical_device_rasterization_order_attachment_access_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RASTERIZATION_ORDER_ATTACHMENT_ACCESS_FEATURES_ARM};

        // VK_NV_linear_color_attachment structs
        physical_device_linear_color_attachment_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINEAR_COLOR_ATTACHMENT_FEATURES_NV};
    }

    const VkInstance instance_;

    typedef std::unordered_map<VkPhysicalDevice, PhysicalDeviceData> Map;
    static Map map_;
};

PhysicalDeviceData::Map PhysicalDeviceData::map_;

// Loader for Profiles JSON configuration files ////////////////////////////////////////////////////////////////////////////////////

class JsonLoader {
   public:
    JsonLoader(PhysicalDeviceData &pdd) : pdd_(pdd) {}
    JsonLoader() = delete;
    JsonLoader(const JsonLoader &) = delete;
    JsonLoader &operator=(const JsonLoader &) = delete;

    VkResult LoadFiles();
    VkResult LoadFiles(const char *filename_list);
    VkResult LoadFile(const char *filename);
    bool ReturnError() const;
    VkResult ReadProfile(const Json::Value root, const std::vector<std::string> &capabilities);

   private:
    struct Extension {
        std::string name;
        int specVersion;
    };

    bool GetFeature(const Json::Value &features, const std::string &feature_name);
    bool GetProperty(const Json::Value &props, const std::string &property_name);
    bool GetFormat(const Json::Value &formats, const std::string &format_name, ArrayOfVkFormatProperties *dest);
    bool CheckExtensionSupport(const char *extension);
    void AddPromotedExtensions(uint32_t api_level);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceProperties *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDepthStencilResolveProperties *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDescriptorIndexingPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFloatControlsPropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceMaintenance3PropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceMaintenance4FeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceMaintenance4PropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceMultiviewPropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceGroupPropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePortabilitySubsetPropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceProtectedMemoryProperties *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceTimelineSemaphorePropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceLimits *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceSparseProperties *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceSubgroupProperties *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFeatures *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevice8BitStorageFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevice16BitStorageFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceBufferDeviceAddressFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDescriptorIndexingFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceHostQueryResetFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceImagelessFramebufferFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceMultiviewFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePortabilitySubsetFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceProtectedMemoryFeatures *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceScalarBlockLayoutFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderAtomicInt64FeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderDrawParametersFeatures *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderFloat16Int8FeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceTimelineSemaphoreFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceVariablePointersFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceVulkanMemoryModelFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceAccelerationStructureFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceAccelerationStructurePropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePerformanceQueryFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePerformanceQueryPropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePresentIdFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePresentWaitFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePushDescriptorPropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceRayQueryFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceRayTracingPipelineFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceRayTracingPipelinePropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderClockFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceSynchronization2FeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevice4444FormatsFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceASTCDecodeFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceBorderColorSwizzleFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceColorWriteEnableFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceConditionalRenderingFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceConservativeRasterizationPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceCustomBorderColorFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceCustomBorderColorPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDepthClipEnableFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDeviceMemoryReportFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDiscardRectanglePropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceExtendedDynamicStateFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceExtendedDynamicState2FeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceExternalMemoryHostPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentDensityMapFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentDensityMapPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceImageRobustnessFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceIndexTypeUint8FeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceInlineUniformBlockFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceInlineUniformBlockPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceLineRasterizationFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceLineRasterizationPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceMemoryPriorityFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceMultiDrawFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceMultiDrawPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDevicePrivateDataFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceProvokingVertexFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceProvokingVertexPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceRobustness2FeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceRobustness2PropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceSampleLocationsPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderAtomicFloatFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceSubgroupSizeControlFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceSubgroupSizeControlPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceTransformFeedbackFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceTransformFeedbackPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceYcbcrImageArraysFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentShadingRateFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentShadingRatePropertiesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceCoherentMemoryFeaturesAMD *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderCorePropertiesAMD *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderCoreProperties2AMD *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceInvocationMaskFeaturesHUAWEI *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceSubpassShadingFeaturesHUAWEI *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceSubpassShadingPropertiesHUAWEI *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceComputeShaderDerivativesFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceCooperativeMatrixFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceCooperativeMatrixPropertiesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceCornerSampledImageFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceCoverageReductionModeFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDiagnosticsConfigFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDeviceGeneratedCommandsPropertiesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceExternalMemoryRDMAFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentShadingRateEnumsPropertiesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceInheritedViewportScissorFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceMeshShaderFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceMeshShaderPropertiesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceRayTracingPropertiesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceRayTracingMotionBlurFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceExclusiveScissorFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderImageFootprintFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderSMBuiltinsFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShaderSMBuiltinsPropertiesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShadingRateImageFeaturesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceShadingRateImagePropertiesNV *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDynamicRenderingFeaturesKHR *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceImageViewMinLodFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentDensityMap2FeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentDensityMap2PropertiesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentDensityMapOffsetPropertiesQCOM *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceDepthClipControlFeaturesEXT *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceLinearColorAttachmentFeaturesNV *dest);
    void GetValue(const Json::Value &parent, int index, VkMemoryType *dest);
    bool GetValue(const Json::Value &parent, int index, VkMemoryHeap *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceMemoryProperties *dest);
    bool GetValue(const Json::Value &parent, VkSurfaceCapabilitiesKHR *dest);
    bool GetValue(const Json::Value &parent, const std::string &member, const char *name, VkExtent2D *dest);
    bool GetValue(const Json::Value &parent, const std::string &member, const char *name, VkExtent2D *dest,
                  std::function<bool(const char *, uint32_t, uint32_t)> warn_func);
    bool GetValue(const Json::Value &parent, const std::string &member, const char *name, VkExtent3D *dest);
    void GetValue(const Json::Value &parent, int index, VkQueueFamilyProperties *dest);
    void GetValue(const Json::Value &parent, int index, VkLayerProperties *dest);
    void GetValue(const Json::Value &parent, int index, VkExtensionProperties *dest);
    void GetValue(const Json::Value &parent, int index, VkSurfaceFormatKHR *dest);
    void GetValue(const Json::Value &parent, int index, VkPresentModeKHR *dest);
    void GetValue(const Json::Value &parent, int index, VkPhysicalDevice *dest);
    void GetValue(const Json::Value &parent, int index, VkDeviceSize *dest);

    // Non-modifiable
    bool GetValuePhysicalDevicePointClippingPropertiesKHR(const Json::Value &parent);
    bool GetValuePhysicalDeviceDriverProperties(const Json::Value &parent);
    bool GetValuePhysicalDeviceIDProperties(const Json::Value &parent);
    bool GetValuePhysicalDeviceMemoryBudgetPropertiesEXT(const Json::Value &parent);
    bool GetValuePhysicalDevicePCIBusInfoPropertiesEXT(const Json::Value &parent);
    bool GetValuePhysicalDeviceDrmPropertiesEXT(const Json::Value &parent);
    bool GetValuePhysicalDeviceToolPropertiesEXT(const Json::Value &parent);

    bool GetValue(const Json::Value &parent, VkPhysicalDeviceVulkan12Properties *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceVulkan12Features *dest);

    bool GetValue(const Json::Value &parent, VkPhysicalDeviceVulkan13Properties *dest);
    bool GetValue(const Json::Value &parent, VkPhysicalDeviceVulkan13Features *dest);

    // GPUinfo config file format
    void GetValueGPUinfoCore11(const Json::Value &parent);
    void GetValueGPUinfoCore12(const Json::Value &parent);
    void GetValueGPUinfoSurfaceCapabilities(const Json::Value &parent);

    static void WarnNotModifiable(const char *parent_name, const std::string &member, const char *name) {
        if (member != name) {
            return;
        }
        DebugPrintf(
            "WARN \"%s::%s\" value is set in the profile, but it is not modifiable by the Profiles Layer and will not be set.\n",
            parent_name, name);
    }

    static bool WarnIfGreater(const char *name, const uint64_t new_value, const uint64_t old_value) {
        if (new_value > old_value) {
            DebugPrintf("WARN \"%s\" JSON value (%" PRIu64 ") is greater than existing value (%" PRIu64 ")\n", name, new_value,
                        old_value);
            return true;
        }
        return false;
    }

    static bool WarnIfGreaterSizet(const char *name, const size_t new_value, const size_t old_value) {
        if (new_value > old_value) {
            DebugPrintf("WARN \"%s\" JSON value (%" PRIuLEAST64 ") is greater than existing value (%" PRIuLEAST64 ")\n", name,
                        new_value, old_value);
            return true;
        }
        return false;
    }

    static bool WarnIfGreaterFloat(const char *name, const float new_value, const float old_value) {
        if (new_value > old_value) {
            DebugPrintf("WARN \"%s\" JSON value (%3.2f) is greater than existing value (%3.2f)\n", name, new_value, old_value);
            return true;
        }
        return false;
    }

    static bool WarnIfNotEqual(const char *name, const bool new_value, const bool old_value) {
        if (new_value && !old_value) {
            DebugPrintf("WARN \"%s\" JSON value is enabled in the profile, but the device does not support it.\n", name);
            return true;
        }
        return false;
    }

    static bool WarnIfLesser(const char *name, const uint64_t new_value, const uint64_t old_value) {
        if (new_value < old_value) {
            DebugPrintf("WARN \"%s\" JSON value (%" PRIu64 ") is lesser than existing value (%" PRIu64 ")\n", name, new_value,
                        old_value);
            return true;
        }
        return false;
    }

    static bool WarnIfLesserSizet(const char *name, const size_t new_value, const size_t old_value) {
        if (new_value < old_value) {
            DebugPrintf("WARN \"%s\" JSON value (%" PRIuLEAST64 ") is lesser than existing value (%" PRIuLEAST64 ")\n", name,
                        new_value, old_value);
            return true;
        }
        return false;
    }

    static bool WarnIfLesserFloat(const char *name, const float new_value, const float old_value) {
        if (new_value < old_value) {
            DebugPrintf("WARN \"%s\" JSON value (%3.2f) is lesser than existing value (%3.2f)\n", name, new_value, old_value);
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
        const uint32_t new_value = value.asInt();
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
        if (value.isBool()) {
            const bool new_value = value.asBool();
            if (warn_func) {
                if (warn_func(name, new_value, *dest)) {
                    valid = false;
                }
            }
            *dest = static_cast<size_t>(new_value);
        } else if (value.isArray()) {
            size_t sum_bits = 0;
            for (const auto &entry : value) {
                if (entry.isString()) {
                    sum_bits |= VkStringToUint(entry.asString());
                }
            }
            *dest = sum_bits;
        } else if (value.isUInt()) {
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

    bool GetValue(const Json::Value &parent, const char *name, uint32_t *dest,
                  std::function<bool(const char *, uint32_t, uint32_t)> warn_func = nullptr) {
        return GetValue(parent, name, name, dest, warn_func);
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

    bool GetValue(const Json::Value &parent, const char *name, uint64_t *dest,
                  std::function<bool(const char *, uint64_t, uint64_t)> warn_func = nullptr) {
        return GetValue(parent, name, name, dest, warn_func);
    }

    bool GetValue(const Json::Value &parent, const std::string &member, const char *name, int64_t *dest,
                  std::function<bool(const char *, int64_t, int64_t)> warn_func = nullptr) {
        if (member != name) {
            return true;
        }
        const Json::Value value = parent[name];
        if (!value.isInt64()) {
            return true;
        }
        bool valid = true;
        const int64_t new_value = value.asInt64();
        if (warn_func) {
            if (warn_func(name, new_value, *dest)) {
                valid = false;
            }
        }
        *dest = new_value;
        return valid;
    }

    bool GetValue(const Json::Value &parent, const std::string &member, const char *name, uint8_t *dest,
                  std::function<bool(const char *, uint8_t, uint8_t)> warn_func = nullptr) {
        if (member != name) {
            return true;
        }
        const Json::Value value = parent[name];
        if (!value.isUInt()) {
            return true;
        }
        bool valid = true;
        const uint8_t new_value = value.asUInt();
        if (warn_func) {
            if (warn_func(name, new_value, *dest)) {
                valid = false;
            }
        }
        *dest = new_value;
        return valid;
    }

    template <typename T>  // for Vulkan enum types
    bool GetValue(const Json::Value &parent, const std::string &member, const char *name, T *dest,
                  std::function<bool(const char *, T, T)> warn_func = nullptr) {
        if (member != name) {
            return true;
        }
        const Json::Value value = parent[name];
        bool valid = true;
        if (value.isArray()) {
            uint32_t sum_bits = {};
            for (const auto &entry : value) {
                if (entry.isString()) {
                    sum_bits |= VkStringToUint(entry.asString());
                }
            }
            *dest = static_cast<T>(sum_bits);
        } else if (value.isInt()) {
            const T new_value = static_cast<T>(value.asInt());
            if (warn_func) {
                if (warn_func(name, new_value, *dest)) {
                    valid = false;
                }
            }
            *dest = new_value;
        }
        return valid;
    }

    template <typename T>  // for Vulkan enum types
    bool GetValue(const Json::Value &parent, const char *name, T *dest,
                  std::function<bool(const char *, T, T)> warn_func = nullptr) {
        return GetValue(parent, name, name, dest, warn_func);
    }

    int GetArray(const Json::Value &parent, const char *name, uint8_t *dest) {
        const Json::Value value = parent[name];
        if (value.type() != Json::arrayValue) {
            return -1;
        }
        const int count = static_cast<int>(value.size());
        for (int i = 0; i < count; ++i) {
            dest[i] = value[i].asUInt();
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
            dest[i] = value[i].asUInt();
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

    int GetArray(const Json::Value &parent, const char *name, VkMemoryType *dest) {
        const Json::Value value = parent[name];
        if (value.type() != Json::arrayValue) {
            return -1;
        }
        const int count = static_cast<int>(value.size());
        for (int i = 0; i < count; ++i) {
            GetValue(value, i, &dest[i]);
        }
        return count;
    }

    int GetArray(const Json::Value &parent, const char *name, VkMemoryHeap *dest) {
        const Json::Value value = parent[name];
        if (value.type() != Json::arrayValue) {
            return -1;
        }
        const int count = static_cast<int>(value.size());
        for (int i = 0; i < count; ++i) {
            GetValue(value, i, &dest[i]);
        }
        return count;
    }

    PhysicalDeviceData &pdd_;
};

static inline VkFormat StringToFormat(const std::string &input_value) {
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
        {"VK_FORMAT_G8_B8R8_2PLANE_444_UNORM_EXT", VK_FORMAT_G8_B8R8_2PLANE_444_UNORM_EXT},
        {"VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT", VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT},
        {"VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT", VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT},
        {"VK_FORMAT_G16_B16R16_2PLANE_444_UNORM_EXT", VK_FORMAT_G16_B16R16_2PLANE_444_UNORM_EXT},
        {"VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT", VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT},
        {"VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT", VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT},
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
    };
    const auto it = map.find(input_value);
    if (it != map.end()) {
        return it->second;
    }
    return VK_FORMAT_UNDEFINED;
}

static inline VkFormatFeatureFlags StringToVkFormatFeatureFlags(const std::string &input_value) {
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
        {"VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT",
         VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT},
        {"VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT",
         VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT},
        {"VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT",
         VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT},
        {"VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT",
         VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT},
        {"VK_FORMAT_FEATURE_DISJOINT_BIT", VK_FORMAT_FEATURE_DISJOINT_BIT},
        {"VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT", VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT},
        {"VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT", VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT},
        {"VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG", VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG},
        {"VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR", VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR},
        {"VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR", VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR},
        {"VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR",
         VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR},
        {"VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT", VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT},
        {"VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR", VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR},
        {"VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR", VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR},
        {"VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR", VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR},
        {"VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR", VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR},
        {"VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR", VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR},
        {"VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT", VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT},
        {"VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR", VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR},
        {"VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR",
         VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR},
        {"VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR",
         VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR},
        {"VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR",
         VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR},
        {"VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR",
         VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR},
        {"VK_FORMAT_FEATURE_DISJOINT_BIT_KHR", VK_FORMAT_FEATURE_DISJOINT_BIT_KHR},
        {"VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR", VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR},
        {"VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT", VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT},
    };
    const auto it = map.find(input_value);
    if (it != map.end()) {
        return it->second;
    }
    return 0;
}

VkResult JsonLoader::LoadFiles() {
    if (inputFilename.str.empty()) {
        ErrorPrintf("envar %s and %s in vk_layer_settings.txt are unset\n", kEnvarProfilesFilename, kLayerSettingsProfileFile);
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    const char *filename_list = inputFilename.str.c_str();
    if (inputFilename.fromEnvVar) {
        DebugPrintf("envar %s = \"%s\"\n", kEnvarProfilesFilename, filename_list);
    } else {
        DebugPrintf("vk_layer_settings.txt setting %s = \"%s\"\n", kLayerSettingsProfileFile, filename_list);
    }
    return LoadFiles(filename_list);
}

VkResult JsonLoader::LoadFiles(const char *filename_list) {
#if defined(_WIN32)
    const char delimiter = ';';
#else
    const char delimiter = ':';
#endif
    std::stringstream ss_list(filename_list);
    std::string filename;

    while (std::getline(ss_list, filename, delimiter)) {
        VkResult result = LoadFile(filename.c_str());
        if (result != VK_SUCCESS) {
            return result;
        }
    }
    return VK_SUCCESS;
}

bool JsonLoader::GetFeature(const Json::Value &features, const std::string &feature_name) {
    const Json::Value &feature = features[feature_name];

    if (feature_name == "VkPhysicalDeviceFeatures") {
        return GetValue(feature, &pdd_.physical_device_features_);
    } else if (feature_name == "VkPhysicalDeviceHostQueryResetFeatures" ||
               feature_name == "VkPhysicalDeviceHostQueryResetFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_host_query_reset_features_);
    } else if (feature_name == "VkPhysicalDeviceMaintenance4Features" ||
               feature_name == "VkPhysicalDeviceMaintenance4FeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_maintenance_4_features_);
    } else if (feature_name == "VkPhysicalDevice16BitStorageFeatures" ||
               feature_name == "VkPhysicalDevice16BitStorageFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_16bit_storage_features_);
    } else if (feature_name == "VkPhysicalDevice8BitStorageFeatures" || feature_name == "VkPhysicalDevice8BitStorageFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_8bit_storage_features_);
    } else if (feature_name == "VkPhysicalDeviceBufferDeviceAddressFeatures" ||
               feature_name == "VkPhysicalDeviceBufferDeviceAddressFeaturesKHR" ||
               feature_name == "VkPhysicalDeviceBufferDeviceAddressFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_buffer_device_address_features_);
    } else if (feature_name == "VkPhysicalDeviceDescriptorIndexingFeatures" ||
               feature_name == "VkPhysicalDeviceDescriptorIndexingFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_descriptor_indexing_features_);
    } else if (feature_name == "VkPhysicalDeviceImagelessFramebufferFeatures" ||
               feature_name == "VkPhysicalDeviceImagelessFramebufferFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_imageless_framebuffer_features_);
    } else if (feature_name == "VkPhysicalDeviceMultiviewFeatures" || feature_name == "VkPhysicalDeviceMultiviewFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_multiview_features_);
    } else if (feature_name == "VkPhysicalDeviceProtectedMemoryFeatures") {
        return GetValue(feature, &pdd_.physical_device_protected_memory_features_);
    } else if (feature_name == "VkPhysicalDeviceSamplerYcbcrConversionFeatures" ||
               feature_name == "VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_sampler_ycbcr_conversion_features_);
    } else if (feature_name == "VkPhysicalDeviceScalarBlockLayoutFeatures" ||
               feature_name == "VkPhysicalDeviceScalarBlockLayoutFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_scalar_block_layout_features_);
    } else if (feature_name == "VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures" ||
               feature_name == "VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_separate_depth_stencil_layouts_features_);
    } else if (feature_name == "VkPhysicalDeviceShaderAtomicInt64Features" ||
               feature_name == "VkPhysicalDeviceShaderAtomicInt64FeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_shader_atomic_int64_features_);
    } else if (feature_name == "VkPhysicalDeviceShaderDrawParametersFeatures") {
        return GetValue(feature, &pdd_.physical_device_shader_draw_parameters_features_);
    } else if (feature_name == "VkPhysicalDeviceShaderFloat16Int8Features" ||
               feature_name == "VkPhysicalDeviceShaderFloat16Int8FeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_shader_float16_int8_features_);
    } else if (feature_name == "VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures" ||
               feature_name == "VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_shader_subgroup_extended_types_features_);
    } else if (feature_name == "VkPhysicalDeviceTimelineSemaphoreFeatures" ||
               feature_name == "VkPhysicalDeviceTimelineSemaphoreFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_timeline_semaphore_features_);
    } else if (feature_name == "VkPhysicalDeviceUniformBufferStandardLayoutFeatures" ||
               feature_name == "VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_uniform_buffer_standard_layout_features_);
    } else if (feature_name == "VkPhysicalDeviceVariablePointersFeatures" ||
               feature_name == "VkPhysicalDeviceVariablePointersFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_variable_pointers_features_);
    } else if (feature_name == "VkPhysicalDeviceVulkanMemoryModelFeatures" ||
               feature_name == "VkPhysicalDeviceVulkanMemoryModelFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_vulkan_memory_model_features_);
    } else if (feature_name == "VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures" ||
               feature_name == "VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_zero_initialize_workgroup_memory_features_);
    } else if (feature_name == "VkPhysicalDeviceAccelerationStructureFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_acceleration_structure_features_);
    } else if (feature_name == "VkPhysicalDevicePerformanceQueryFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_performance_query_features_);
    } else if (feature_name == "VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_pipeline_executable_properties_features_);
    } else if (feature_name == "VkPhysicalDevicePresentIdFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_present_id_features_);
    } else if (feature_name == "VkPhysicalDevicePresentWaitFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_present_wait_features_);
    } else if (feature_name == "VkPhysicalDeviceRayQueryFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_ray_query_features_);
    } else if (feature_name == "VkPhysicalDeviceRayTracingPipelineFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_ray_tracing_pipeline_features_);
    } else if (feature_name == "VkPhysicalDeviceShaderClockFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_shader_clock_features_);
    } else if (feature_name == "VkPhysicalDeviceShaderIntegerDotProductFeatures" ||
               feature_name == "VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_shader_integer_dot_product_features_);
    } else if (feature_name == "VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_shader_subgroup_uniform_control_flow_features_);
    } else if (feature_name == "VkPhysicalDeviceShaderTerminateInvocationFeatures" ||
               feature_name == "VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_shader_terminate_invocation_features_);
    } else if (feature_name == "VkPhysicalDeviceSynchronization2Features" ||
               feature_name == "VkPhysicalDeviceSynchronization2FeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_synchronization2_features_);
    } else if (feature_name == "VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_workgroup_memory_explicit_layout_features_);
    } else if (feature_name == "VkPhysicalDevice4444FormatsFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_4444_formats_features_);
    } else if (feature_name == "VkPhysicalDeviceASTCDecodeFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_astc_decode_features_);
    } else if (feature_name == "VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_blend_operation_advanced_features_);
    } else if (feature_name == "VkPhysicalDeviceBorderColorSwizzleFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_border_color_swizzle_features_);
    } else if (feature_name == "VkPhysicalDeviceColorWriteEnableFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_color_write_enable_features_);
    } else if (feature_name == "VkPhysicalDeviceConditionalRenderingFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_conditional_rendering_features_);
    } else if (feature_name == "VkPhysicalDeviceCustomBorderColorFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_custom_border_color_features_);
    } else if (feature_name == "VkPhysicalDeviceDepthClipEnableFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_depth_clip_enable_features_ext_);
    } else if (feature_name == "VkPhysicalDeviceDeviceMemoryReportFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_device_memory_report_features_);
    } else if (feature_name == "VkPhysicalDeviceExtendedDynamicStateFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_extended_dynamic_state_features_);
    } else if (feature_name == "VkPhysicalDeviceExtendedDynamicState2FeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_extended_dynamic_state2_features_);
    } else if (feature_name == "VkPhysicalDeviceFragmentDensityMapFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_fragment_density_map_features_);
    } else if (feature_name == "VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_fragment_shader_interlock_features_);
    } else if (feature_name == "VkPhysicalDeviceGlobalPriorityQueryFeaturesKHR" ||
               feature_name == "VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_global_priority_query_features_);
    } else if (feature_name == "VkPhysicalDeviceImageRobustnessFeatures" ||
               feature_name == "VkPhysicalDeviceImageRobustnessFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_image_robustness_features_);
    } else if (feature_name == "VkPhysicalDeviceIndexTypeUint8FeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_index_type_uint8_features_);
    } else if (feature_name == "VkPhysicalDeviceInlineUniformBlockFeatures" ||
               feature_name == "VkPhysicalDeviceInlineUniformBlockFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_inline_uniform_block_features_);
    } else if (feature_name == "VkPhysicalDeviceLineRasterizationFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_line_rasterization_features_);
    } else if (feature_name == "VkPhysicalDeviceMemoryPriorityFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_memory_priority_features_);
    } else if (feature_name == "VkPhysicalDeviceMultiDrawFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_multi_draw_features_);
    } else if (feature_name == "VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_pageable_device_local_memory_features_);
    } else if (feature_name == "VkPhysicalDevicePipelineCreationCacheControlFeatures" ||
               feature_name == "VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_pipeline_creation_cache_control_features_);
    } else if (feature_name == "VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_primitive_topology_list_restart_features_);
    } else if (feature_name == "VkPhysicalDevicePrivateDataFeatures" || feature_name == "VkPhysicalDevicePrivateDataFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_private_data_features_);
    } else if (feature_name == "VkPhysicalDeviceProvokingVertexFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_provoking_vertex_features_);
    } else if (feature_name == "VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_rgba10x6_formats_features_);
    } else if (feature_name == "VkPhysicalDeviceRobustness2FeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_robustness_2_features_);
    } else if (feature_name == "VkPhysicalDeviceShaderAtomicFloatFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_shader_atomic_float_features_);
    } else if (feature_name == "VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_shader_atomic_float2_features_);
    } else if (feature_name == "VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures" ||
               feature_name == "VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_shader_demote_to_helper_invocation_features_);
    } else if (feature_name == "VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_shader_image_atomic_int64_features_);
    } else if (feature_name == "VkPhysicalDeviceSubgroupSizeControlFeatures" ||
               feature_name == "VkPhysicalDeviceSubgroupSizeControlFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_subgroup_size_control_features_);
    } else if (feature_name == "VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_texel_buffer_alignment_features_);
    } else if (feature_name == "VkPhysicalDeviceTextureCompressionASTCHDRFeatures" ||
               feature_name == "VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_texture_compression_astc_hdr_features_);
    } else if (feature_name == "VkPhysicalDeviceTransformFeedbackFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_transform_feedback_features_);
    } else if (feature_name == "VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_vertex_attribute_divisor_features_);
    } else if (feature_name == "VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_vertex_input_dynamic_state_features_);
    } else if (feature_name == "VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_ycbcr_2plane_444_formats_features_);
    } else if (feature_name == "VkPhysicalDeviceYcbcrImageArraysFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_ycbcr_image_arrays_features_);
    } else if (feature_name == "VkPhysicalDeviceFragmentShadingRateFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_fragment_shading_rate_features_);
    } else if (feature_name == "VkPhysicalDeviceCoherentMemoryFeaturesAMD") {
        return GetValue(feature, &pdd_.physical_device_coherent_memory_features_);
    } else if (feature_name == "VkPhysicalDeviceInvocationMaskFeaturesHUAWEI") {
        return GetValue(feature, &pdd_.physical_device_invocation_mask_features_);
    } else if (feature_name == "VkPhysicalDeviceSubpassShadingFeaturesHUAWEI") {
        return GetValue(feature, &pdd_.physical_device_subpass_shading_features_);
    } else if (feature_name == "VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL") {
        return GetValue(feature, &pdd_.physical_device_shader_integer_functions_2_features_);
    } else if (feature_name == "VkPhysicalDeviceComputeShaderDerivativesFeaturesNV") {
        return GetValue(feature, &pdd_.physical_device_compute_shader_derivatives_features_);
    } else if (feature_name == "VkPhysicalDeviceCooperativeMatrixFeaturesNV") {
        return GetValue(feature, &pdd_.physical_device_cooperative_matrix_features_);
    } else if (feature_name == "VkPhysicalDeviceCornerSampledImageFeaturesNV") {
        return GetValue(feature, &pdd_.physical_device_corner_sampled_image_features_);
    } else if (feature_name == "VkPhysicalDeviceCoverageReductionModeFeaturesNV") {
        return GetValue(feature, &pdd_.physical_device_coverage_reduction_mode_features_);
    } else if (feature_name == "VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV") {
        return GetValue(feature, &pdd_.physical_device_dedicated_allocation_image_aliasing_features_);
    } else if (feature_name == "VkPhysicalDeviceDiagnosticsConfigFeaturesNV") {
        return GetValue(feature, &pdd_.physical_device_diagnostics_config_features_);
    } else if (feature_name == "VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV") {
        return GetValue(feature, &pdd_.physical_device_device_generated_commands_features_);
    } else if (feature_name == "VkPhysicalDeviceExternalMemoryRDMAFeaturesNV") {
        return GetValue(feature, &pdd_.physical_device_external_memory_rdma_features_);
    } else if (feature_name == "VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV") {
        return GetValue(feature, &pdd_.physical_device_fragment_shader_barycentric_features_);
    } else if (feature_name == "VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV") {
        return GetValue(feature, &pdd_.physical_device_fragment_shading_rate_enums_features_);
    } else if (feature_name == "VkPhysicalDeviceInheritedViewportScissorFeaturesNV") {
        return GetValue(feature, &pdd_.physical_device_inherited_viewport_scissor_features_);
    } else if (feature_name == "VkPhysicalDeviceMeshShaderFeaturesNV") {
        return GetValue(feature, &pdd_.physical_device_mesh_shader_features_);
    } else if (feature_name == "VkPhysicalDeviceRayTracingMotionBlurFeaturesNV") {
        return GetValue(feature, &pdd_.physical_device_ray_tracing_motiuon_blur_features_);
    } else if (feature_name == "VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV") {
        return GetValue(feature, &pdd_.physical_device_representative_fragment_test_features_);
    } else if (feature_name == "VkPhysicalDeviceExclusiveScissorFeaturesNV") {
        return GetValue(feature, &pdd_.physical_device_exclusive_scissor_features_);
    } else if (feature_name == "VkPhysicalDeviceShaderImageFootprintFeaturesNV") {
        return GetValue(feature, &pdd_.physical_device_shader_image_footprint_features_);
    } else if (feature_name == "VkPhysicalDeviceShaderSMBuiltinsFeaturesNV") {
        return GetValue(feature, &pdd_.physical_device_shader_sm_builtins_features_);
    } else if (feature_name == "VkPhysicalDeviceShadingRateImageFeaturesNV") {
        return GetValue(feature, &pdd_.physical_device_shading_rate_image_features_);
    } else if (feature_name == "VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE") {
        return GetValue(feature, &pdd_.physical_device_mutable_descriptor_type_features_);
    } else if (feature_name == "VkPhysicalDeviceDynamicRenderingFeatures" ||
               feature_name == "VkPhysicalDeviceDynamicRenderingFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_dynamic_rendering_features_);
    } else if (feature_name == "VkPhysicalDeviceImageViewMinLodFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_image_view_min_lod_features_);
    } else if (feature_name == "VkPhysicalDeviceFragmentDensityMap2FeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_fragment_density_map_2_features_);
    } else if (feature_name == "VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM") {
        return GetValue(feature, &pdd_.physical_device_fragment_density_map_offset_features_);
    } else if (feature_name == "VkPhysicalDeviceDepthClipControlFeaturesEXT") {
        return GetValue(feature, &pdd_.physical_device_depth_clip_control_features_);
    } else if (feature_name == "VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM") {
        return GetValue(feature, &pdd_.physical_device_rasterization_order_attachment_access_features_);
    } else if (feature_name == "VkPhysicalDeviceLinearColorAttachmentFeaturesNV") {
        return GetValue(feature, &pdd_.physical_device_linear_color_attachment_features_);
    } else if (feature_name == "Vulkan12Features") {
        return GetValue(feature, &pdd_.physical_device_vulkan_1_2_features_);
    } else if (feature_name == "VkPhysicalDevicePortabilitySubsetFeaturesKHR") {
        return GetValue(feature, &pdd_.physical_device_portability_subset_features_);
    }
    return true;
}

bool JsonLoader::GetProperty(const Json::Value &props, const std::string &property_name) {
    const Json::Value &prop = props[property_name];

    if (property_name == "VkPhysicalDeviceProperties") {
        return GetValue(prop, &pdd_.physical_device_properties_);
    } else if (property_name == "VkPhysicalDeviceLimits") {
        return GetValue(prop, &pdd_.physical_device_properties_.limits);
    } else if (property_name == "VkPhysicalDeviceSparseProperties") {
        return GetValue(prop, &pdd_.physical_device_properties_.sparseProperties);
    } else if (property_name == "VkPhysicalDeviceDepthStencilResolveProperties" ||
               property_name == "VkPhysicalDeviceDepthStencilResolvePropertiesKHR") {
        return GetValue(prop, &pdd_.physical_device_depth_stencil_resolve_properties_);
    } else if (property_name == "VkPhysicalDeviceSubgroupProperties") {
        return GetValue(prop, &pdd_.physical_device_subgroup_properties_);
    } else if (property_name == "VkPhysicalDeviceDescriptorIndexingProperties" ||
               property_name == "VkPhysicalDeviceDescriptorIndexingPropertiesEXT") {
        return GetValue(prop, &pdd_.physical_device_descriptor_indexing_properties_);
    } else if (property_name == "VkPhysicalDeviceFloatControlsProperties" ||
               property_name == "VkPhysicalDeviceFloatControlsPropertiesKHR") {
        return GetValue(prop, &pdd_.physical_device_float_controls_properties_);
    } else if (property_name == "VkPhysicalDeviceMaintenance3Properties" ||
               property_name == "VkPhysicalDeviceMaintenance3PropertiesKHR") {
        return GetValue(prop, &pdd_.physical_device_maintenance_3_properties_);
    } else if (property_name == "VkPhysicalDeviceMaintenance4Properties" ||
               property_name == "VkPhysicalDeviceMaintenance4PropertiesKHR") {
        return GetValue(prop, &pdd_.physical_device_maintenance_4_properties_);
    } else if (property_name == "VkPhysicalDeviceMultiviewProperties" ||
               property_name == "VkPhysicalDeviceMultiviewPropertiesKHR") {
        return GetValue(prop, &pdd_.physical_device_multiview_properties_);
    } else if (property_name == "VkPhysicalDeviceProtectedMemoryProperties") {
        return GetValue(prop, &pdd_.physical_device_protected_memory_properties_);
    } else if (property_name == "VkPhysicalDeviceTimelineSemaphoreProperties" ||
               property_name == "VkPhysicalDeviceTimelineSemaphorePropertiesKHR") {
        return GetValue(prop, &pdd_.physical_device_timeline_semaphore_properties_);
    } else if (property_name == "VkPhysicalDeviceSamplerFilterMinmaxProperties" ||
               property_name == "VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT") {
        return GetValue(prop, &pdd_.physical_device_sampler_filter_minmax_properties_);
    } else if (property_name == "VkPhysicalDeviceAccelerationStructurePropertiesKHR") {
        return GetValue(prop, &pdd_.physical_device_acceleration_structure_properties_);
    } else if (property_name == "VkPhysicalDevicePerformanceQueryPropertiesKHR") {
        return GetValue(prop, &pdd_.physical_device_performance_query_properties_);
    } else if (property_name == "VkPhysicalDevicePushDescriptorPropertiesKHR") {
        return GetValue(prop, &pdd_.physical_device_push_descriptor_properites_);
    } else if (property_name == "VkPhysicalDeviceRayTracingPipelinePropertiesKHR") {
        return GetValue(prop, &pdd_.physical_device_ray_tracing_pipeline_properties_);
    } else if (property_name == "VkPhysicalDeviceShaderIntegerDotProductProperties" ||
               property_name == "VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR") {
        return GetValue(prop, &pdd_.physical_device_shader_integer_dot_products_properties_);
    } else if (property_name == "VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT") {
        return GetValue(prop, &pdd_.physical_device_blend_operation_advanced_properties_);
    } else if (property_name == "VkPhysicalDeviceConservativeRasterizationPropertiesEXT") {
        return GetValue(prop, &pdd_.physical_device_conservative_rasterization_properties_);
    } else if (property_name == "VkPhysicalDeviceCustomBorderColorPropertiesEXT") {
        return GetValue(prop, &pdd_.physical_device_custom_border_color_properties_);
    } else if (property_name == "VkPhysicalDeviceDiscardRectanglePropertiesEXT") {
        return GetValue(prop, &pdd_.physical_device_discard_rectangle_properties_);
    } else if (property_name == "VkPhysicalDeviceExternalMemoryHostPropertiesEXT") {
        return GetValue(prop, &pdd_.physical_device_external_memory_host_properties_);
    } else if (property_name == "VkPhysicalDeviceFragmentDensityMapPropertiesEXT") {
        return GetValue(prop, &pdd_.physical_device_fragment_density_map_properties_);
    } else if (property_name == "VkPhysicalDeviceInlineUniformBlockProperties" ||
               property_name == "VkPhysicalDeviceInlineUniformBlockPropertiesEXT") {
        return GetValue(prop, &pdd_.physical_device_inline_uniform_block_properties_);
    } else if (property_name == "VkPhysicalDeviceLineRasterizationPropertiesEXT") {
        return GetValue(prop, &pdd_.physical_device_line_rasterization_properties_);
    } else if (property_name == "VkPhysicalDeviceMultiDrawPropertiesEXT") {
        return GetValue(prop, &pdd_.physical_device_multi_draw_properties_);
    } else if (property_name == "VkPhysicalDeviceProvokingVertexPropertiesEXT") {
        return GetValue(prop, &pdd_.physical_device_provoking_vertex_properties_);
    } else if (property_name == "VkPhysicalDeviceRobustness2PropertiesEXT") {
        return GetValue(prop, &pdd_.physical_device_robustness_2_properties_);
    } else if (property_name == "VkPhysicalDeviceSampleLocationsPropertiesEXT") {
        return GetValue(prop, &pdd_.physical_device_sample_locations_properties_);
    } else if (property_name == "VkPhysicalDeviceSubgroupSizeControlProperties" ||
               property_name == "VkPhysicalDeviceSubgroupSizeControlPropertiesEXT") {
        return GetValue(prop, &pdd_.physical_device_subgroup_size_control_properties_);
    } else if (property_name == "VkPhysicalDeviceTexelBufferAlignmentProperties" ||
               property_name == "VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT") {
        return GetValue(prop, &pdd_.physical_device_texel_buffer_alignment_properties_);
    } else if (property_name == "VkPhysicalDeviceTransformFeedbackPropertiesEXT") {
        return GetValue(prop, &pdd_.physical_device_transform_feedback_properties_);
    } else if (property_name == "VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT") {
        return GetValue(prop, &pdd_.physical_device_vertex_attirbute_divisor_properties_);
    } else if (property_name == "VkPhysicalDeviceFragmentShadingRatePropertiesKHR") {
        return GetValue(prop, &pdd_.physical_device_fragment_shading_rate_properties_);
    } else if (property_name == "VkPhysicalDeviceShaderCorePropertiesAMD") {
        return GetValue(prop, &pdd_.physical_device_shader_core_properties_);
    } else if (property_name == "VkPhysicalDeviceShaderCoreProperties2AMD") {
        return GetValue(prop, &pdd_.physical_device_shader_core_properties_2_);
    } else if (property_name == "VkPhysicalDeviceSubpassShadingPropertiesHUAWEI") {
        return GetValue(prop, &pdd_.physical_device_subpass_shading_properties_);
    } else if (property_name == "VkPhysicalDeviceCooperativeMatrixPropertiesNV") {
        return GetValue(prop, &pdd_.physical_device_cooperative_matrix_properties_);
    } else if (property_name == "VkPhysicalDeviceDeviceGeneratedCommandsPropertiesNV") {
        return GetValue(prop, &pdd_.physical_device_device_generated_commands_properties_);
    } else if (property_name == "VkPhysicalDeviceFragmentShadingRateEnumsPropertiesNV") {
        return GetValue(prop, &pdd_.physical_device_fragment_shading_rate_enums_properties_);
    } else if (property_name == "VkPhysicalDeviceMeshShaderPropertiesNV") {
        return GetValue(prop, &pdd_.physical_device_mesh_shader_properties_);
    } else if (property_name == "VkPhysicalDeviceRayTracingPropertiesNV") {
        return GetValue(prop, &pdd_.physical_device_ray_tracing_properties_);
    } else if (property_name == "VkPhysicalDeviceShaderSMBuiltinsPropertiesNV") {
        return GetValue(prop, &pdd_.physical_device_shader_sm_builtins_properties_);
    } else if (property_name == "VkPhysicalDeviceShadingRateImagePropertiesNV") {
        return GetValue(prop, &pdd_.physical_device_shading_rate_image_properties_);
    } else if (property_name == "VkPhysicalDeviceFragmentDensityMap2PropertiesEXT") {
        return GetValue(prop, &pdd_.physical_device_fragment_density_map_2_properties_);
    } else if (property_name == "VkPhysicalDeviceFragmentDensityMapOffsetPropertiesQCOM") {
        return GetValue(prop, &pdd_.physical_device_fragment_density_map_offset_properties_);
    } else if (property_name == "VkPhysicalDeviceMemoryProperties") {
        return GetValue(prop, &pdd_.physical_device_memory_properties_);
    } else if (property_name == "VkSurfaceCapabilitiesKHR") {
        return GetValue(prop, &pdd_.surface_capabilities_);
    } else if (property_name == "Vulkan12Properties") {
        return GetValue(prop, &pdd_.physical_device_vulkan_1_2_properties_);
    } else if (property_name == "Vulkan13Properties") {
        return GetValue(prop, &pdd_.physical_device_vulkan_1_3_properties_);
    } else if (property_name == "VkPhysicalDevicePortabilitySubsetPropertiesKHR") {
        return GetValue(prop, &pdd_.physical_device_portability_subset_properties_);
    } else if (property_name == "VkPhysicalDevicePointClippingProperties") {
        return GetValuePhysicalDevicePointClippingPropertiesKHR(prop);
    } else if (property_name == "VkPhysicalDevicePointClippingPropertiesKHR") {
        return GetValuePhysicalDevicePointClippingPropertiesKHR(prop);
    } else if (property_name == "VkPhysicalDeviceDriverProperties" || property_name == "VkPhysicalDeviceDriverPropertiesKHR") {
        return GetValuePhysicalDeviceDriverProperties(prop);
    } else if (property_name == "VkPhysicalDeviceIDProperties" || property_name == "VkPhysicalDeviceIDPropertiesKHR") {
        return GetValuePhysicalDeviceIDProperties(prop);
    } else if (property_name == "VkPhysicalDeviceMemoryBudgetPropertiesEXT") {
        return GetValuePhysicalDeviceMemoryBudgetPropertiesEXT(prop);
    } else if (property_name == "VkPhysicalDevicePCIBusInfoPropertiesEXT") {
        return GetValuePhysicalDevicePCIBusInfoPropertiesEXT(prop);
    } else if (property_name == "VkPhysicalDeviceDrmPropertiesEXT") {
        return GetValuePhysicalDeviceDrmPropertiesEXT(prop);
    } else if (property_name == "VkPhysicalDeviceToolProperties" || property_name == "VkPhysicalDeviceToolPropertiesEXT") {
        return GetValuePhysicalDeviceToolPropertiesEXT(prop);
    }
    return true;
}

bool JsonLoader::GetFormat(const Json::Value &formats, const std::string &format_name, ArrayOfVkFormatProperties *dest) {
    VkFormat format = StringToFormat(format_name);
    VkFormatProperties format_properties = {};
    const auto &member = formats[format_name];
    const auto &props = member["VkFormatProperties"];
    for (const auto &feature : props["linearTilingFeatures"]) {
        format_properties.linearTilingFeatures |= StringToVkFormatFeatureFlags(feature.asString());
    }
    for (const auto &feature : props["optimalTilingFeatures"]) {
        format_properties.optimalTilingFeatures |= StringToVkFormatFeatureFlags(feature.asString());
    }
    for (const auto &feature : props["bufferFeatures"]) {
        format_properties.bufferFeatures |= StringToVkFormatFeatureFlags(feature.asString());
    }

    (*dest)[format] = format_properties;

    if ((pdd_.device_formats_[format].linearTilingFeatures & format_properties.linearTilingFeatures) !=
        format_properties.linearTilingFeatures) {
        DebugPrintf("JSON file sets variables for format %s, but set linearTilingFeatures are not supported.\n",
                    format_name.c_str());
        return false;
    }
    if ((pdd_.device_formats_[format].optimalTilingFeatures & format_properties.optimalTilingFeatures) !=
        format_properties.optimalTilingFeatures) {
        DebugPrintf("JSON file sets variables for format %s, but set optimalTilingFeatures are not supported.\n",
                    format_name.c_str());
        return false;
    }
    if ((pdd_.device_formats_[format].bufferFeatures & format_properties.bufferFeatures) != format_properties.bufferFeatures) {
        DebugPrintf("JSON file sets variables for format %s, but set bufferFeatures are not supported.\n", format_name.c_str());
        return false;
    }
    if (pdd_.format_list_combination_mode_ == SetCombinationMode::SET_CHECK_SUPPORT) {
        return true;
    }
    return true;
}

bool JsonLoader::CheckExtensionSupport(const char *extension) {
    if (pdd_.extension_list_combination_mode_ == SetCombinationMode::SET_CHECK_SUPPORT ||
        pdd_.extension_list_combination_mode_ == SetCombinationMode::SET_FROM_DEVICE) {
        if (!PhysicalDeviceData::HasExtension(&pdd_, extension)) {
            ErrorPrintf(
                "JSON file sets variables for structs provided by %s, but %s is "
                "not supported by the device.\n",
                extension, extension);
            return false;
        }
    } else if (pdd_.extension_list_combination_mode_ == SetCombinationMode::SET_FROM_PROFILE ||
               pdd_.extension_list_combination_mode_ == SetCombinationMode::SET_FROM_PROFILE_OVERRIDE) {
        if (!PhysicalDeviceData::HasSimulatedExtension(&pdd_, extension)) {
            ErrorPrintf(
                "JSON file sets variables for structs provided by %s, but %s is "
                "not enabled by the profile.\n",
                extension, extension);
            if (pdd_.extension_list_combination_mode_ == SetCombinationMode::SET_FROM_PROFILE) {
                return false;
            }
        }
    }
    return true;
}

void JsonLoader::AddPromotedExtensions(uint32_t api_version) {
    static const std::vector<const char *> promoted_1_1 = {
        VK_KHR_MULTIVIEW_EXTENSION_NAME,
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
        VK_KHR_DEVICE_GROUP_EXTENSION_NAME,
        VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
        VK_KHR_MAINTENANCE1_EXTENSION_NAME,
        VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME,
        VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
        VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME,
        VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME,
        VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME,
        VK_KHR_16BIT_STORAGE_EXTENSION_NAME,
        VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE_EXTENSION_NAME,
        VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME,
        VK_KHR_EXTERNAL_FENCE_EXTENSION_NAME,
        VK_KHR_MAINTENANCE2_EXTENSION_NAME,
        VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME,
        VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
        VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME,
        VK_KHR_RELAXED_BLOCK_LAYOUT_EXTENSION_NAME,
        VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
        VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME,
        VK_KHR_BIND_MEMORY_2_EXTENSION_NAME,
        VK_KHR_MAINTENANCE3_EXTENSION_NAME,
    };
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
    if (api_version >= VK_API_VERSION_1_1) {
        for (const auto ext : promoted_1_1) {
            if (!PhysicalDeviceData::HasSimulatedExtension(&pdd_, ext)) {
                VkExtensionProperties extension;
                strcpy(extension.extensionName, ext);
                extension.specVersion = 1;
                pdd_.simulation_extensions_.push_back(extension);
            }
        }
    }
    if (api_version >= VK_API_VERSION_1_2) {
        for (const auto ext : promoted_1_2) {
            if (!PhysicalDeviceData::HasSimulatedExtension(&pdd_, ext)) {
                VkExtensionProperties extension;
                strcpy(extension.extensionName, ext);
                extension.specVersion = 1;
                pdd_.simulation_extensions_.push_back(extension);
            }
        }
    }
    if (api_version >= VK_API_VERSION_1_3) {
        for (const auto ext : promoted_1_3) {
            if (!PhysicalDeviceData::HasSimulatedExtension(&pdd_, ext)) {
                VkExtensionProperties extension;
                strcpy(extension.extensionName, ext);
                extension.specVersion = 1;
                pdd_.simulation_extensions_.push_back(extension);
            }
        }
    }
}

bool JsonLoader::ReturnError() const {
    if (pdd_.extension_list_combination_mode_ == SetCombinationMode::SET_FROM_PROFILE) {
        return true;
    }
    return false;
}

VkResult JsonLoader::ReadProfile(const Json::Value root, const std::vector<std::string> &capabilities) {
    const auto &caps = root["capabilities"];
    for (const auto &capability : capabilities) {
        const auto &c = caps[capability];

        const auto &extensions = c["extensions"];
        const auto &features = c["features"];
        const auto &properties = c["properties"];
        const auto &formats = c["formats"];

        uint32_t apiVersion = properties["VkPhysicalDeviceProperties"]["apiVersion"].asInt();
        AddPromotedExtensions(apiVersion);

        pdd_.arrayof_extension_properties_.reserve(extensions.size());
        for (const auto &e : extensions.getMemberNames()) {
            VkExtensionProperties extension;
            strcpy(extension.extensionName, e.c_str());
            extension.specVersion = extensions[e].asInt();
            bool found = false;
            for (const auto &ext : pdd_.arrayof_extension_properties_) {
                if (strcmp(ext.extensionName, extension.extensionName) == 0) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                bool supported_on_device = false;
                for (const auto &device_extension : pdd_.device_extensions_) {
                    if (strcmp(device_extension.extensionName, extension.extensionName) == 0) {
                        supported_on_device = true;
                        break;
                    }
                }
                if (!supported_on_device && ReturnError()) {
                    return VK_ERROR_INITIALIZATION_FAILED;
                }
                pdd_.arrayof_extension_properties_.push_back(extension);
                if (pdd_.extension_list_combination_mode_ == SetCombinationMode::SET_FROM_PROFILE ||
                    pdd_.extension_list_combination_mode_ == SetCombinationMode::SET_FROM_PROFILE_OVERRIDE) {
                    if (!PhysicalDeviceData::HasSimulatedExtension(&pdd_, extension.extensionName)) {
                        pdd_.simulation_extensions_.push_back(extension);
                    }
                }
            }
        }

        for (const auto &feature : features.getMemberNames()) {
            bool success = GetFeature(features, feature);
            if (!success && ReturnError()) {
                return VK_ERROR_INITIALIZATION_FAILED;
            }
        }

        for (const auto &prop : properties.getMemberNames()) {
            bool success = GetProperty(properties, prop);
            if (!success && ReturnError()) {
                return VK_ERROR_INITIALIZATION_FAILED;
            }
        }

        for (const auto &format : formats.getMemberNames()) {
            bool success = GetFormat(formats, format, &pdd_.arrayof_format_properties_);
            if (!success && ReturnError()) {
                return VK_ERROR_INITIALIZATION_FAILED;
            }
        }
    }
    return VK_SUCCESS;
}

VkResult JsonLoader::LoadFile(const char *filename) {
    std::ifstream json_file(filename);
    if (!json_file) {
        ErrorPrintf("JsonLoader failed to open file \"%s\"\n", filename);
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    DebugPrintf("JsonLoader::LoadFile(\"%s\")\n", filename);
    Json::Reader reader;
    Json::Value root = Json::nullValue;
    bool success = reader.parse(json_file, root, false);
    if (!success) {
        ErrorPrintf("Json::Reader failed {\n%s}\n", reader.getFormattedErrorMessages().c_str());
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    json_file.close();

    if (root.type() != Json::objectValue) {
        ErrorPrintf("Json document root is not an object\n");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    const std::string profile_name = profileName.str;
    if (inputFilename.fromEnvVar) {
        DebugPrintf("envar %s = \"%s\"\n", kEnvarProfilesProfileName, profile_name.c_str());
    } else {
        DebugPrintf("vk_layer_settings.txt setting %s = \"%s\"\n", kEnvarProfilesProfileName, profile_name.c_str());
    }

    const auto &profiles = root["profiles"];
    std::vector<std::string> capabilities;
    for (const auto &profile : profiles.getMemberNames()) {
        bool select = (profile_name.empty() && !profiles[profile].isMember("fallback")) || (profile == profile_name);
        if (select) {
            const auto &caps = profiles[profile]["capabilities"];
            for (const auto &cap : caps) {
                capabilities.push_back(cap.asString());
            }
            break;
        }
    }
    if (capabilities.empty()) {
        return VK_SUCCESS;
    }

    DebugPrintf("{\n");
    const Json::Value schema_value = root["$schema"];
    if (!schema_value.isString()) {
        ErrorPrintf("JSON element \"$schema\" is not a string\n");
        return VK_ERROR_INITIALIZATION_FAILED;
    } 
    
    const std::string schema = schema_value.asCString();
    if (schema.find(SCHEMA_URI_BASE) == std::string::npos) {
        DebugPrintf("Document schema \"%s\" not supported by %s\n", schema.c_str(), kOurLayerName);
        DebugPrintf("}\n");
        return VK_ERROR_INITIALIZATION_FAILED;
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
        DebugPrintf("%s is built againt Vulkan Header %d but the profile is written againt Vulkan Header %d. All newer capabilities in the profile will be ignored by the layer.\n", 
            kOurLayerName, VK_HEADER_VERSION, version_patch);
    }

    VkResult result = VK_SUCCESS;
    if (pdd_.extension_list_combination_mode_ == SetCombinationMode::SET_FROM_PROFILE ||
        pdd_.extension_list_combination_mode_ == SetCombinationMode::SET_FROM_PROFILE_OVERRIDE) {
        pdd_.simulation_extensions_.clear();
    }
    if (pdd_.extension_list_combination_mode_ != SetCombinationMode::SET_FROM_DEVICE) {
        result = ReadProfile(root, capabilities);
    }

    // GPUinfo structures
    GetValueGPUinfoCore11(root);
    GetValueGPUinfoCore12(root);
    GetValueGPUinfoSurfaceCapabilities(root);

    DebugPrintf("}\n");

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
#define GET_MEMBER_VALUE_WARN(member, name, warn_func)              \
    if (!GetValue(parent, member, #name, &dest->name, warn_func)) { \
        valid = false;                                              \
    }

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceProperties *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceProperties)\n");
    bool valid = true;
    if (!GetValue(parent["limits"], &dest->limits)) {
        valid = false;
    }
    for (const auto &prop : parent) {
        GET_VALUE("apiVersion", apiVersion);
        GET_VALUE("driverVersion", driverVersion);
        GET_VALUE("vendorID", vendorID);
        GET_VALUE("deviceID", deviceID);
        GET_VALUE("deviceType", deviceType);
        GetValue(parent, "deviceType", "deviceType", &dest->deviceType);
        GET_ARRAY(deviceName);         // size < VK_MAX_PHYSICAL_DEVICE_NAME_SIZE
        GET_ARRAY(pipelineCacheUUID);  // size == VK_UUID_SIZE*/
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDepthStencilResolveProperties *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceDepthStencilResolveProperties)\n");
    if (!CheckExtensionSupport(VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE(prop, supportedDepthResolveModes);
        GET_VALUE(prop, supportedStencilResolveModes);
        GET_VALUE_WARN(prop, independentResolveNone, WarnIfNotEqual);
        GET_VALUE_WARN(prop, independentResolve, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDescriptorIndexingPropertiesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceDescriptorIndexingPropertiesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, maxUpdateAfterBindDescriptorsInAllPools, WarnIfGreater);
        GET_VALUE_WARN(prop, maxUpdateAfterBindDescriptorsInAllPools, WarnIfGreater);
        GET_VALUE_WARN(prop, shaderUniformBufferArrayNonUniformIndexingNative, WarnIfNotEqual);
        GET_VALUE_WARN(prop, shaderSampledImageArrayNonUniformIndexingNative, WarnIfNotEqual);
        GET_VALUE_WARN(prop, shaderStorageBufferArrayNonUniformIndexingNative, WarnIfNotEqual);
        GET_VALUE_WARN(prop, shaderStorageImageArrayNonUniformIndexingNative, WarnIfNotEqual);
        GET_VALUE_WARN(prop, shaderInputAttachmentArrayNonUniformIndexingNative, WarnIfNotEqual);
        GET_VALUE_WARN(prop, robustBufferAccessUpdateAfterBind, WarnIfNotEqual);
        GET_VALUE_WARN(prop, quadDivergentImplicitLod, WarnIfNotEqual);
        GET_VALUE_WARN(prop, maxPerStageDescriptorUpdateAfterBindSamplers, WarnIfGreater);
        GET_VALUE_WARN(prop, maxPerStageDescriptorUpdateAfterBindUniformBuffers, WarnIfGreater);
        GET_VALUE_WARN(prop, maxPerStageDescriptorUpdateAfterBindStorageBuffers, WarnIfGreater);
        GET_VALUE_WARN(prop, maxPerStageDescriptorUpdateAfterBindSampledImages, WarnIfGreater);
        GET_VALUE_WARN(prop, maxPerStageDescriptorUpdateAfterBindStorageImages, WarnIfGreater);
        GET_VALUE_WARN(prop, maxPerStageDescriptorUpdateAfterBindInputAttachments, WarnIfGreater);
        GET_VALUE_WARN(prop, maxPerStageUpdateAfterBindResources, WarnIfGreater);
        GET_VALUE_WARN(prop, maxDescriptorSetUpdateAfterBindSamplers, WarnIfGreater);
        GET_VALUE_WARN(prop, maxDescriptorSetUpdateAfterBindUniformBuffers, WarnIfGreater);
        GET_VALUE_WARN(prop, maxDescriptorSetUpdateAfterBindUniformBuffersDynamic, WarnIfGreater);
        GET_VALUE_WARN(prop, maxDescriptorSetUpdateAfterBindStorageBuffers, WarnIfGreater);
        GET_VALUE_WARN(prop, maxDescriptorSetUpdateAfterBindStorageBuffersDynamic, WarnIfGreater);
        GET_VALUE_WARN(prop, maxDescriptorSetUpdateAfterBindSampledImages, WarnIfGreater);
        GET_VALUE_WARN(prop, maxDescriptorSetUpdateAfterBindStorageImages, WarnIfGreater);
        GET_VALUE_WARN(prop, maxDescriptorSetUpdateAfterBindInputAttachments, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFloatControlsPropertiesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceFloatControlsPropertiesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE(prop, denormBehaviorIndependence);
        GET_VALUE(prop, roundingModeIndependence);
        GET_VALUE_WARN(prop, shaderSignedZeroInfNanPreserveFloat16, WarnIfNotEqual);
        GET_VALUE_WARN(prop, shaderSignedZeroInfNanPreserveFloat32, WarnIfNotEqual);
        GET_VALUE_WARN(prop, shaderSignedZeroInfNanPreserveFloat64, WarnIfNotEqual);
        GET_VALUE_WARN(prop, shaderDenormPreserveFloat16, WarnIfNotEqual);
        GET_VALUE_WARN(prop, shaderDenormPreserveFloat32, WarnIfNotEqual);
        GET_VALUE_WARN(prop, shaderDenormPreserveFloat64, WarnIfNotEqual);
        GET_VALUE_WARN(prop, shaderDenormFlushToZeroFloat16, WarnIfNotEqual);
        GET_VALUE_WARN(prop, shaderDenormFlushToZeroFloat32, WarnIfNotEqual);
        GET_VALUE_WARN(prop, shaderDenormFlushToZeroFloat64, WarnIfNotEqual);
        GET_VALUE_WARN(prop, shaderRoundingModeRTEFloat16, WarnIfNotEqual);
        GET_VALUE_WARN(prop, shaderRoundingModeRTEFloat32, WarnIfNotEqual);
        GET_VALUE_WARN(prop, shaderRoundingModeRTEFloat64, WarnIfNotEqual);
        GET_VALUE_WARN(prop, shaderRoundingModeRTZFloat16, WarnIfNotEqual);
        GET_VALUE_WARN(prop, shaderRoundingModeRTZFloat32, WarnIfNotEqual);
        GET_VALUE_WARN(prop, shaderRoundingModeRTZFloat64, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceMaintenance3PropertiesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceMaintenance3PropertiesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_MAINTENANCE3_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, maxPerSetDescriptors, WarnIfGreater);
        GET_VALUE_WARN(prop, maxMemoryAllocationSize, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceMaintenance4FeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceMaintenance4FeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_MAINTENANCE_4_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, maintenance4, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceMaintenance4PropertiesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceMaintenance4PropertiesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_MAINTENANCE_4_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, maxBufferSize, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceMultiviewPropertiesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceMultiviewPropertiesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_MULTIVIEW_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, maxMultiviewViewCount, WarnIfGreater);
        GET_VALUE_WARN(prop, maxMultiviewInstanceIndex, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValuePhysicalDevicePointClippingPropertiesKHR(const Json::Value &parent) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePointClippingPropertiesKHR)\n");
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        WarnNotModifiable("VkPhysicalDevicePointClippingPropertiesKHR", prop, "pointClippingBehavior");
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceGroupPropertiesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceGroupPropertiesKHR)\n");
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        WarnNotModifiable("VkPhysicalDeviceGroupPropertiesKHR", prop, "physicalDeviceCount");
        WarnNotModifiable("VkPhysicalDeviceGroupPropertiesKHR", prop, "physicalDevices");
        WarnNotModifiable("VkPhysicalDeviceGroupPropertiesKHR", prop, "subsetAllocation");
    }
    return valid;
}

bool JsonLoader::GetValuePhysicalDeviceDriverProperties(const Json::Value &parent) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceDriverPropertiesKHR)\n");
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        WarnNotModifiable("VkPhysicalDeviceDriverPropertiesKHR", prop, "driverName");
        WarnNotModifiable("VkPhysicalDeviceDriverPropertiesKHR", prop, "driverInfo");
        WarnNotModifiable("VkPhysicalDeviceDriverPropertiesKHR", prop, "conformanceVersion");
    }
    return valid;
}

bool JsonLoader::GetValuePhysicalDeviceIDProperties(const Json::Value &parent) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceIDPropertiesKHR)\n");
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        WarnNotModifiable("VkPhysicalDeviceIDPropertiesKHR", prop, "deviceUUID");
        WarnNotModifiable("VkPhysicalDeviceIDPropertiesKHR", prop, "driverUUID");
        WarnNotModifiable("VkPhysicalDeviceIDPropertiesKHR", prop, "deviceLUID");
        WarnNotModifiable("VkPhysicalDeviceIDPropertiesKHR", prop, "deviceNodeMask");
        WarnNotModifiable("VkPhysicalDeviceIDPropertiesKHR", prop, "deviceLUIDValid");
    }
    return valid;
}

bool JsonLoader::GetValuePhysicalDeviceMemoryBudgetPropertiesEXT(const Json::Value &parent) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceMemoryBudgetPropertiesEXT)\n");
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        WarnNotModifiable("VkPhysicalDeviceMemoryBudgetPropertiesEXT", prop, "heapBudget");
        WarnNotModifiable("VkPhysicalDeviceMemoryBudgetPropertiesEXT", prop, "heapUsage");
    }
    return valid;
}

bool JsonLoader::GetValuePhysicalDevicePCIBusInfoPropertiesEXT(const Json::Value &parent) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePCIBusInfoPropertiesEXT)\n");
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        WarnNotModifiable("VkPhysicalDevicePCIBusInfoPropertiesEXT", prop, "pciDomain");
        WarnNotModifiable("VkPhysicalDevicePCIBusInfoPropertiesEXT", prop, "pciBus");
        WarnNotModifiable("VkPhysicalDevicePCIBusInfoPropertiesEXT", prop, "pciDevice");
        WarnNotModifiable("VkPhysicalDevicePCIBusInfoPropertiesEXT", prop, "pciFunction");
    }
    return valid;
}

bool JsonLoader::GetValuePhysicalDeviceDrmPropertiesEXT(const Json::Value &parent) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceDrmPropertiesEXT)\n");
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        WarnNotModifiable("PhysicalDeviceDrmPropertiesEXT", prop, "hasPrimary");
        WarnNotModifiable("PhysicalDeviceDrmPropertiesEXT", prop, "hasRender");
        WarnNotModifiable("PhysicalDeviceDrmPropertiesEXT", prop, "primaryMajor");
        WarnNotModifiable("PhysicalDeviceDrmPropertiesEXT", prop, "primaryMinor");
        WarnNotModifiable("PhysicalDeviceDrmPropertiesEXT", prop, "renderMajor");
        WarnNotModifiable("PhysicalDeviceDrmPropertiesEXT", prop, "renderMinor");
    }
    return valid;
}

bool JsonLoader::GetValuePhysicalDeviceToolPropertiesEXT(const Json::Value &parent) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceToolPropertiesEXT)\n");
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        WarnNotModifiable("VkPhysicalDeviceToolPropertiesEXT", prop, "name");
        WarnNotModifiable("VkPhysicalDeviceToolPropertiesEXT", prop, "version");
        WarnNotModifiable("VkPhysicalDeviceToolPropertiesEXT", prop, "purposes");
        WarnNotModifiable("VkPhysicalDeviceToolPropertiesEXT", prop, "description");
        WarnNotModifiable("VkPhysicalDeviceToolPropertiesEXT", prop, "layer");
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePortabilitySubsetPropertiesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePortabilitySubsetPropertiesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME) && emulatePortability.num <= 0) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_portability_subset, but VK_KHR_portability_subset is "
            "not supported by the device and emulation is not turned on.\nIf you wish to emulate "
            "VK_KHR_portability_subset, please set environment variable %s to 1.\n",
            kEnvarProfilesEmulatePortability);
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, minVertexInputBindingStrideAlignment, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceProtectedMemoryProperties *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceProtectedMemoryProperties)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, protectedNoFault, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, filterMinmaxSingleComponentFormats, WarnIfNotEqual);
        GET_VALUE_WARN(prop, filterMinmaxImageComponentMapping, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceTimelineSemaphorePropertiesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceTimelineSemaphorePropertiesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, maxTimelineSemaphoreValueDifference, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceLimits *dest) {
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, maxImageDimension1D, WarnIfGreater);
        GET_VALUE_WARN(prop, maxImageDimension2D, WarnIfGreater);
        GET_VALUE_WARN(prop, maxImageDimension3D, WarnIfGreater);
        GET_VALUE_WARN(prop, maxImageDimensionCube, WarnIfGreater);
        GET_VALUE_WARN(prop, maxImageArrayLayers, WarnIfGreater);
        GET_VALUE_WARN(prop, maxTexelBufferElements, WarnIfGreater);
        GET_VALUE_WARN(prop, maxUniformBufferRange, WarnIfGreater);
        GET_VALUE_WARN(prop, maxStorageBufferRange, WarnIfGreater);
        GET_VALUE_WARN(prop, maxPushConstantsSize, WarnIfGreater);
        GET_VALUE_WARN(prop, maxMemoryAllocationCount, WarnIfGreater);
        GET_VALUE_WARN(prop, maxSamplerAllocationCount, WarnIfGreater);
        GET_VALUE_WARN(prop, bufferImageGranularity, WarnIfGreater);
        GET_VALUE_WARN(prop, sparseAddressSpaceSize, WarnIfGreater);
        GET_VALUE_WARN(prop, maxBoundDescriptorSets, WarnIfGreater);
        GET_VALUE_WARN(prop, maxPerStageDescriptorSamplers, WarnIfGreater);
        GET_VALUE_WARN(prop, maxPerStageDescriptorUniformBuffers, WarnIfGreater);
        GET_VALUE_WARN(prop, maxPerStageDescriptorStorageBuffers, WarnIfGreater);
        GET_VALUE_WARN(prop, maxPerStageDescriptorSampledImages, WarnIfGreater);
        GET_VALUE_WARN(prop, maxPerStageDescriptorStorageImages, WarnIfGreater);
        GET_VALUE_WARN(prop, maxPerStageDescriptorInputAttachments, WarnIfGreater);
        GET_VALUE_WARN(prop, maxPerStageResources, WarnIfGreater);
        GET_VALUE_WARN(prop, maxDescriptorSetSamplers, WarnIfGreater);
        GET_VALUE_WARN(prop, maxDescriptorSetUniformBuffers, WarnIfGreater);
        GET_VALUE_WARN(prop, maxDescriptorSetUniformBuffersDynamic, WarnIfGreater);
        GET_VALUE_WARN(prop, maxDescriptorSetStorageBuffers, WarnIfGreater);
        GET_VALUE_WARN(prop, maxDescriptorSetStorageBuffersDynamic, WarnIfGreater);
        GET_VALUE_WARN(prop, maxDescriptorSetSampledImages, WarnIfGreater);
        GET_VALUE_WARN(prop, maxDescriptorSetStorageImages, WarnIfGreater);
        GET_VALUE_WARN(prop, maxDescriptorSetInputAttachments, WarnIfGreater);
        GET_VALUE_WARN(prop, maxVertexInputAttributes, WarnIfGreater);
        GET_VALUE_WARN(prop, maxVertexInputBindings, WarnIfGreater);
        GET_VALUE_WARN(prop, maxVertexInputAttributeOffset, WarnIfGreater);
        GET_VALUE_WARN(prop, maxVertexInputBindingStride, WarnIfGreater);
        GET_VALUE_WARN(prop, maxVertexOutputComponents, WarnIfGreater);
        GET_VALUE_WARN(prop, maxTessellationGenerationLevel, WarnIfGreater);
        GET_VALUE_WARN(prop, maxTessellationPatchSize, WarnIfGreater);
        GET_VALUE_WARN(prop, maxTessellationControlPerVertexInputComponents, WarnIfGreater);
        GET_VALUE_WARN(prop, maxTessellationControlPerVertexOutputComponents, WarnIfGreater);
        GET_VALUE_WARN(prop, maxTessellationControlPerPatchOutputComponents, WarnIfGreater);
        GET_VALUE_WARN(prop, maxTessellationControlTotalOutputComponents, WarnIfGreater);
        GET_VALUE_WARN(prop, maxTessellationEvaluationInputComponents, WarnIfGreater);
        GET_VALUE_WARN(prop, maxTessellationEvaluationOutputComponents, WarnIfGreater);
        GET_VALUE_WARN(prop, maxGeometryShaderInvocations, WarnIfGreater);
        GET_VALUE_WARN(prop, maxGeometryInputComponents, WarnIfGreater);
        GET_VALUE_WARN(prop, maxGeometryOutputComponents, WarnIfGreater);
        GET_VALUE_WARN(prop, maxGeometryOutputVertices, WarnIfGreater);
        GET_VALUE_WARN(prop, maxGeometryTotalOutputComponents, WarnIfGreater);
        GET_VALUE_WARN(prop, maxFragmentInputComponents, WarnIfGreater);
        GET_VALUE_WARN(prop, maxFragmentOutputAttachments, WarnIfGreater);
        GET_VALUE_WARN(prop, maxFragmentDualSrcAttachments, WarnIfGreater);
        GET_VALUE_WARN(prop, maxFragmentCombinedOutputResources, WarnIfGreater);
        GET_VALUE_WARN(prop, maxComputeSharedMemorySize, WarnIfGreater);
        GET_ARRAY(maxComputeWorkGroupCount);  // size == 3
        GET_VALUE_WARN(prop, maxComputeWorkGroupInvocations, WarnIfGreater);
        GET_ARRAY(maxComputeWorkGroupSize);  // size == 3
        GET_VALUE_WARN(prop, subPixelPrecisionBits, WarnIfGreater);
        GET_VALUE_WARN(prop, subTexelPrecisionBits, WarnIfGreater);
        GET_VALUE_WARN(prop, mipmapPrecisionBits, WarnIfGreater);
        GET_VALUE_WARN(prop, maxDrawIndexedIndexValue, WarnIfGreater);
        GET_VALUE_WARN(prop, maxDrawIndirectCount, WarnIfGreater);
        GET_VALUE_WARN(prop, maxSamplerLodBias, WarnIfGreaterFloat);
        GET_VALUE_WARN(prop, maxSamplerAnisotropy, WarnIfGreaterFloat);
        GET_VALUE_WARN(prop, maxViewports, WarnIfGreater);
        GET_ARRAY(maxViewportDimensions);  // size == 2
        GET_ARRAY(viewportBoundsRange);    // size == 2
        GET_VALUE_WARN(prop, viewportSubPixelBits, WarnIfGreater);
        GET_VALUE_SIZE_T_WARN(prop, minMemoryMapAlignment, WarnIfGreaterSizet);
        GET_VALUE_WARN(prop, minTexelBufferOffsetAlignment, WarnIfGreater);
        GET_VALUE_WARN(prop, minUniformBufferOffsetAlignment, WarnIfGreater);
        GET_VALUE_WARN(prop, minStorageBufferOffsetAlignment, WarnIfGreater);
        GET_VALUE_WARN(prop, minTexelOffset, WarnIfLesser);
        GET_VALUE_WARN(prop, maxTexelOffset, WarnIfGreater);
        GET_VALUE_WARN(prop, minTexelGatherOffset, WarnIfLesser);
        GET_VALUE_WARN(prop, maxTexelGatherOffset, WarnIfGreater);
        GET_VALUE_WARN(prop, minInterpolationOffset, WarnIfLesserFloat);
        GET_VALUE_WARN(prop, maxInterpolationOffset, WarnIfGreaterFloat);
        GET_VALUE_WARN(prop, subPixelInterpolationOffsetBits, WarnIfGreater);
        GET_VALUE_WARN(prop, maxFramebufferWidth, WarnIfGreater);
        GET_VALUE_WARN(prop, maxFramebufferHeight, WarnIfGreater);
        GET_VALUE_WARN(prop, maxFramebufferLayers, WarnIfGreater);
        GET_VALUE_WARN(prop, framebufferColorSampleCounts, WarnIfGreater);
        GET_VALUE_WARN(prop, framebufferDepthSampleCounts, WarnIfGreater);
        GET_VALUE_WARN(prop, framebufferStencilSampleCounts, WarnIfGreater);
        GET_VALUE_WARN(prop, framebufferNoAttachmentsSampleCounts, WarnIfGreater);
        GET_VALUE_WARN(prop, maxColorAttachments, WarnIfGreater);
        GET_VALUE_WARN(prop, sampledImageColorSampleCounts, WarnIfGreater);
        GET_VALUE_WARN(prop, sampledImageIntegerSampleCounts, WarnIfGreater);
        GET_VALUE_WARN(prop, sampledImageDepthSampleCounts, WarnIfGreater);
        GET_VALUE_WARN(prop, sampledImageStencilSampleCounts, WarnIfGreater);
        GET_VALUE_WARN(prop, storageImageSampleCounts, WarnIfGreater);
        GET_VALUE_WARN(prop, maxSampleMaskWords, WarnIfGreater);
        GET_VALUE_WARN(prop, timestampComputeAndGraphics, WarnIfNotEqual);
        GET_VALUE_WARN(prop, timestampPeriod, WarnIfGreaterFloat);
        GET_VALUE_WARN(prop, maxClipDistances, WarnIfGreater);
        GET_VALUE_WARN(prop, maxCullDistances, WarnIfGreater);
        GET_VALUE_WARN(prop, maxCombinedClipAndCullDistances, WarnIfGreater);
        GET_VALUE_WARN(prop, discreteQueuePriorities, WarnIfGreater);
        GET_ARRAY(pointSizeRange);  // size == 2
        GET_ARRAY(lineWidthRange);  // size == 2
        GET_VALUE_WARN(prop, pointSizeGranularity, WarnIfGreaterFloat);
        GET_VALUE_WARN(prop, lineWidthGranularity, WarnIfGreaterFloat);
        GET_VALUE_WARN(prop, strictLines, WarnIfNotEqual);
        GET_VALUE_WARN(prop, standardSampleLocations, WarnIfNotEqual);
        GET_VALUE_WARN(prop, optimalBufferCopyOffsetAlignment, WarnIfGreater);
        GET_VALUE_WARN(prop, optimalBufferCopyRowPitchAlignment, WarnIfGreater);
        GET_VALUE_WARN(prop, nonCoherentAtomSize, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceSparseProperties *dest) {
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, residencyStandard2DBlockShape, WarnIfNotEqual);
        GET_VALUE_WARN(prop, residencyStandard2DMultisampleBlockShape, WarnIfNotEqual);
        GET_VALUE_WARN(prop, residencyStandard3DBlockShape, WarnIfNotEqual);
        GET_VALUE_WARN(prop, residencyAlignedMipSize, WarnIfNotEqual);
        GET_VALUE_WARN(prop, residencyNonResidentStrict, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceSubgroupProperties *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceSubgroupProperties)\n");
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, subgroupSize, WarnIfGreater);
        GET_VALUE_WARN(prop, supportedStages, WarnIfGreater);
        GET_VALUE_WARN(prop, supportedOperations, WarnIfGreater);
        GET_VALUE_WARN(prop, quadOperationsInAllStages, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFeatures *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceFeatures)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_MEMBER_VALUE_WARN(member, robustBufferAccess, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, fullDrawIndexUint32, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, imageCubeArray, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, independentBlend, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, geometryShader, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, tessellationShader, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, sampleRateShading, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, dualSrcBlend, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, logicOp, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, multiDrawIndirect, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, drawIndirectFirstInstance, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, depthClamp, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, depthBiasClamp, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, fillModeNonSolid, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, depthBounds, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, wideLines, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, largePoints, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, alphaToOne, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, multiViewport, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, samplerAnisotropy, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, textureCompressionETC2, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, textureCompressionASTC_LDR, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, textureCompressionBC, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, occlusionQueryPrecise, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, pipelineStatisticsQuery, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, vertexPipelineStoresAndAtomics, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, fragmentStoresAndAtomics, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, shaderTessellationAndGeometryPointSize, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, shaderImageGatherExtended, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, shaderStorageImageExtendedFormats, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, shaderStorageImageMultisample, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, shaderStorageImageReadWithoutFormat, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, shaderStorageImageWriteWithoutFormat, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, shaderUniformBufferArrayDynamicIndexing, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, shaderSampledImageArrayDynamicIndexing, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, shaderStorageBufferArrayDynamicIndexing, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, shaderStorageImageArrayDynamicIndexing, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, shaderClipDistance, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, shaderCullDistance, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, shaderFloat64, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, shaderInt64, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, shaderInt16, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, shaderResourceResidency, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, shaderResourceMinLod, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, sparseBinding, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, sparseResidencyBuffer, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, sparseResidencyImage2D, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, sparseResidencyImage3D, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, sparseResidency2Samples, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, sparseResidency4Samples, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, sparseResidency8Samples, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, sparseResidency16Samples, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, sparseResidencyAliased, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, variableMultisampleRate, WarnIfNotEqual);
        GET_MEMBER_VALUE_WARN(member, inheritedQueries, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevice8BitStorageFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevice8BitStorageFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_8BIT_STORAGE_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, storageBuffer8BitAccess, WarnIfNotEqual);
        GET_VALUE_WARN(member, uniformAndStorageBuffer8BitAccess, WarnIfNotEqual);
        GET_VALUE_WARN(member, storagePushConstant8, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevice16BitStorageFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevice16BitStorageFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_16BIT_STORAGE_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, storageBuffer16BitAccess, WarnIfNotEqual);
        GET_VALUE_WARN(member, uniformAndStorageBuffer16BitAccess, WarnIfNotEqual);
        GET_VALUE_WARN(member, storagePushConstant16, WarnIfNotEqual);
        GET_VALUE_WARN(member, storageInputOutput16, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceBufferDeviceAddressFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceBufferDeviceAddressFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, bufferDeviceAddress, WarnIfNotEqual);
        GET_VALUE_WARN(member, bufferDeviceAddressCaptureReplay, WarnIfNotEqual);
        GET_VALUE_WARN(member, bufferDeviceAddressMultiDevice, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDescriptorIndexingFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceDescriptorIndexingFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderInputAttachmentArrayDynamicIndexing, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderUniformTexelBufferArrayDynamicIndexing, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderStorageTexelBufferArrayDynamicIndexing, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderUniformBufferArrayNonUniformIndexing, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderSampledImageArrayNonUniformIndexing, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderStorageBufferArrayNonUniformIndexing, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderStorageImageArrayNonUniformIndexing, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderInputAttachmentArrayNonUniformIndexing, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderUniformTexelBufferArrayNonUniformIndexing, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderStorageTexelBufferArrayNonUniformIndexing, WarnIfNotEqual);
        GET_VALUE_WARN(member, descriptorBindingUniformBufferUpdateAfterBind, WarnIfNotEqual);
        GET_VALUE_WARN(member, descriptorBindingSampledImageUpdateAfterBind, WarnIfNotEqual);
        GET_VALUE_WARN(member, descriptorBindingStorageImageUpdateAfterBind, WarnIfNotEqual);
        GET_VALUE_WARN(member, descriptorBindingStorageBufferUpdateAfterBind, WarnIfNotEqual);
        GET_VALUE_WARN(member, descriptorBindingUniformTexelBufferUpdateAfterBind, WarnIfNotEqual);
        GET_VALUE_WARN(member, descriptorBindingStorageTexelBufferUpdateAfterBind, WarnIfNotEqual);
        GET_VALUE_WARN(member, descriptorBindingUpdateUnusedWhilePending, WarnIfNotEqual);
        GET_VALUE_WARN(member, descriptorBindingPartiallyBound, WarnIfNotEqual);
        GET_VALUE_WARN(member, descriptorBindingVariableDescriptorCount, WarnIfNotEqual);
        GET_VALUE_WARN(member, runtimeDescriptorArray, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceHostQueryResetFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceHostQueryResetFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, hostQueryReset, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceImagelessFramebufferFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceImagelessFramebufferFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, imagelessFramebuffer, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceMultiviewFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceMultiviewFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_MULTIVIEW_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, multiview, WarnIfNotEqual);
        GET_VALUE_WARN(member, multiviewGeometryShader, WarnIfNotEqual);
        GET_VALUE_WARN(member, multiviewTessellationShader, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePortabilitySubsetFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePortabilitySubsetFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME) && emulatePortability.num <= 0) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_portability_subset, but VK_KHR_portability_subset is "
            "not supported by the device and emulation is not turned on.\nIf you wish to emulate "
            "VK_KHR_portability_subset, please set environment variable %s to 1.\n",
            kEnvarProfilesEmulatePortability);
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, constantAlphaColorBlendFactors, WarnIfNotEqual);
        GET_VALUE_WARN(member, events, WarnIfNotEqual);
        GET_VALUE_WARN(member, imageViewFormatReinterpretation, WarnIfNotEqual);
        GET_VALUE_WARN(member, imageViewFormatSwizzle, WarnIfNotEqual);
        GET_VALUE_WARN(member, imageView2DOn3DImage, WarnIfNotEqual);
        GET_VALUE_WARN(member, multisampleArrayImage, WarnIfNotEqual);
        GET_VALUE_WARN(member, mutableComparisonSamplers, WarnIfNotEqual);
        GET_VALUE_WARN(member, pointPolygons, WarnIfNotEqual);
        GET_VALUE_WARN(member, samplerMipLodBias, WarnIfNotEqual);
        GET_VALUE_WARN(member, separateStencilMaskRef, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderSampleRateInterpolationFunctions, WarnIfNotEqual);
        GET_VALUE_WARN(member, tessellationIsolines, WarnIfNotEqual);
        GET_VALUE_WARN(member, tessellationPointMode, WarnIfNotEqual);
        GET_VALUE_WARN(member, triangleFans, WarnIfNotEqual);
        GET_VALUE_WARN(member, vertexAttributeAccessBeyondStride, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceProtectedMemoryFeatures *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceProtectedMemoryFeatures)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, protectedMemory, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, samplerYcbcrConversion, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceScalarBlockLayoutFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceScalarBlockLayoutFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, scalarBlockLayout, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_SEPARATE_DEPTH_STENCIL_LAYOUTS_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, separateDepthStencilLayouts, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderAtomicInt64FeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderAtomicInt64FeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_SHADER_ATOMIC_INT64_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderBufferInt64Atomics, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderSharedInt64Atomics, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderDrawParametersFeatures *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderAtomicInt64FeaturesKHR)\n");
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderDrawParameters, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderFloat16Int8FeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderFloat16Int8FeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderFloat16, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderInt8, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceFeatures)\n");
    if (!CheckExtensionSupport(VK_KHR_SHADER_SUBGROUP_EXTENDED_TYPES_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderSubgroupExtendedTypes, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceTimelineSemaphoreFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceTimelineSemaphoreFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, timelineSemaphore, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, uniformBufferStandardLayout, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceVariablePointersFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceVariablePointersFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, variablePointersStorageBuffer, WarnIfNotEqual);
        GET_VALUE_WARN(member, variablePointers, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceVulkanMemoryModelFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceVulkanMemoryModelFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_VULKAN_MEMORY_MODEL_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, vulkanMemoryModel, WarnIfNotEqual);
        GET_VALUE_WARN(member, vulkanMemoryModelDeviceScope, WarnIfNotEqual);
        GET_VALUE_WARN(member, vulkanMemoryModelAvailabilityVisibilityChains, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_ZERO_INITIALIZE_WORKGROUP_MEMORY_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderZeroInitializeWorkgroupMemory, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceAccelerationStructureFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceAccelerationStructureFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, accelerationStructure, WarnIfNotEqual);
        GET_VALUE_WARN(member, accelerationStructureCaptureReplay, WarnIfNotEqual);
        GET_VALUE_WARN(member, accelerationStructureIndirectBuild, WarnIfNotEqual);
        GET_VALUE_WARN(member, accelerationStructureHostCommands, WarnIfNotEqual);
        GET_VALUE_WARN(member, descriptorBindingAccelerationStructureUpdateAfterBind, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceAccelerationStructurePropertiesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceAccelerationStructurePropertiesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, maxGeometryCount, WarnIfGreater);
        GET_VALUE_WARN(prop, maxInstanceCount, WarnIfGreater);
        GET_VALUE_WARN(prop, maxPrimitiveCount, WarnIfGreater);
        GET_VALUE_WARN(prop, maxPerStageDescriptorAccelerationStructures, WarnIfGreater);
        GET_VALUE_WARN(prop, maxPerStageDescriptorUpdateAfterBindAccelerationStructures, WarnIfGreater);
        GET_VALUE_WARN(prop, maxDescriptorSetAccelerationStructures, WarnIfGreater);
        GET_VALUE_WARN(prop, maxDescriptorSetUpdateAfterBindAccelerationStructures, WarnIfGreater);
        GET_VALUE_WARN(prop, minAccelerationStructureScratchOffsetAlignment, WarnIfLesser);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePerformanceQueryFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePerformanceQueryFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_PERFORMANCE_QUERY_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, performanceCounterQueryPools, WarnIfNotEqual);
        GET_VALUE_WARN(member, performanceCounterMultipleQueryPools, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePerformanceQueryPropertiesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePerformanceQueryPropertiesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_PERFORMANCE_QUERY_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, allowCommandBufferQueryCopies, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_PIPELINE_EXECUTABLE_PROPERTIES_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, pipelineExecutableInfo, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePresentIdFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePresentIdFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_PRESENT_ID_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, presentId, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePresentWaitFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePresentWaitFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_PRESENT_WAIT_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, presentWait, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePushDescriptorPropertiesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePushDescriptorPropertiesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, maxPushDescriptors, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceRayQueryFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceRayQueryFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_RAY_QUERY_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, rayQuery, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceRayTracingPipelineFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceRayTracingPipelineFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, rayTracingPipeline, WarnIfNotEqual);
        GET_VALUE_WARN(member, rayTracingPipelineShaderGroupHandleCaptureReplay, WarnIfNotEqual);
        GET_VALUE_WARN(member, rayTracingPipelineShaderGroupHandleCaptureReplayMixed, WarnIfNotEqual);
        GET_VALUE_WARN(member, rayTracingPipelineTraceRaysIndirect, WarnIfNotEqual);
        GET_VALUE_WARN(member, rayTraversalPrimitiveCulling, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceRayTracingPipelinePropertiesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceRayTracingPipelinePropertiesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, shaderGroupHandleSize, WarnIfGreater);
        GET_VALUE_WARN(prop, maxRayRecursionDepth, WarnIfGreater);
        GET_VALUE_WARN(prop, maxShaderGroupStride, WarnIfGreater);
        GET_VALUE_WARN(prop, shaderGroupBaseAlignment, WarnIfGreater);
        GET_VALUE_WARN(prop, shaderGroupHandleCaptureReplaySize, WarnIfGreater);
        GET_VALUE_WARN(prop, maxRayDispatchInvocationCount, WarnIfGreater);
        GET_VALUE_WARN(prop, shaderGroupHandleAlignment, WarnIfGreater);
        GET_VALUE_WARN(prop, maxRayHitAttributeSize, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderClockFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderClockFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_SHADER_CLOCK_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderSubgroupClock, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderDeviceClock, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_SHADER_INTEGER_DOT_PRODUCT_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderIntegerDotProduct, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_SHADER_INTEGER_DOT_PRODUCT_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, integerDotProduct8BitUnsignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct8BitSignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct8BitMixedSignednessAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct4x8BitPackedUnsignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct4x8BitPackedSignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct4x8BitPackedMixedSignednessAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct16BitUnsignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct16BitSignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct16BitMixedSignednessAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct32BitUnsignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct32BitSignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct32BitMixedSignednessAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct64BitUnsignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct64BitSignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct64BitMixedSignednessAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating8BitUnsignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating8BitSignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating8BitMixedSignednessAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating4x8BitPackedUnsignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating4x8BitPackedSignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating4x8BitPackedMixedSignednessAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating16BitUnsignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating16BitSignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating16BitMixedSignednessAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating32BitUnsignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating32BitSignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating32BitMixedSignednessAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating64BitUnsignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating64BitSignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating64BitMixedSignednessAccelerated, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderSubgroupUniformControlFlow, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_SHADER_TERMINATE_INVOCATION_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderTerminateInvocation, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceSynchronization2FeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceSynchronization2FeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, synchronization2, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, workgroupMemoryExplicitLayout, WarnIfNotEqual);
        GET_VALUE_WARN(member, workgroupMemoryExplicitLayoutScalarBlockLayout, WarnIfNotEqual);
        GET_VALUE_WARN(member, workgroupMemoryExplicitLayout8BitAccess, WarnIfNotEqual);
        GET_VALUE_WARN(member, workgroupMemoryExplicitLayout16BitAccess, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevice4444FormatsFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_EXT_4444_FORMATS_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, formatA4R4G4B4, WarnIfNotEqual);
        GET_VALUE_WARN(member, formatA4B4G4R4, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceASTCDecodeFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceASTCDecodeFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_ASTC_DECODE_MODE_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, decodeModeSharedExponent, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, advancedBlendCoherentOperations, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, advancedBlendMaxColorAttachments, WarnIfGreater);
        GET_VALUE_WARN(prop, advancedBlendIndependentBlend, WarnIfNotEqual);
        GET_VALUE_WARN(prop, advancedBlendNonPremultipliedSrcColor, WarnIfNotEqual);
        GET_VALUE_WARN(prop, advancedBlendNonPremultipliedDstColor, WarnIfNotEqual);
        GET_VALUE_WARN(prop, advancedBlendCorrelatedOverlap, WarnIfNotEqual);
        GET_VALUE_WARN(prop, advancedBlendAllOperations, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceBorderColorSwizzleFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceBorderColorSwizzleFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_BORDER_COLOR_SWIZZLE_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, borderColorSwizzle, WarnIfNotEqual);
        GET_VALUE_WARN(member, borderColorSwizzleFromImage, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceColorWriteEnableFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceColorWriteEnableFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_COLOR_WRITE_ENABLE_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, colorWriteEnable, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceConditionalRenderingFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceConditionalRenderingFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_CONDITIONAL_RENDERING_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, conditionalRendering, WarnIfNotEqual);
        GET_VALUE_WARN(member, inheritedConditionalRendering, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceConservativeRasterizationPropertiesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceConservativeRasterizationPropertiesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, primitiveOverestimationSize, WarnIfGreaterFloat);
        GET_VALUE_WARN(prop, maxExtraPrimitiveOverestimationSize, WarnIfGreaterFloat);
        GET_VALUE_WARN(prop, extraPrimitiveOverestimationSizeGranularity, WarnIfGreaterFloat);
        GET_VALUE_WARN(prop, primitiveUnderestimation, WarnIfNotEqual);
        GET_VALUE_WARN(prop, conservativePointAndLineRasterization, WarnIfNotEqual);
        GET_VALUE_WARN(prop, degenerateTrianglesRasterized, WarnIfNotEqual);
        GET_VALUE_WARN(prop, degenerateLinesRasterized, WarnIfNotEqual);
        GET_VALUE_WARN(prop, fullyCoveredFragmentShaderInputVariable, WarnIfNotEqual);
        GET_VALUE_WARN(prop, conservativeRasterizationPostDepthCoverage, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceCustomBorderColorFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceCustomBorderColorFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_CUSTOM_BORDER_COLOR_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, customBorderColors, WarnIfNotEqual);
        GET_VALUE_WARN(member, customBorderColorWithoutFormat, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceCustomBorderColorPropertiesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceCustomBorderColorPropertiesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_CUSTOM_BORDER_COLOR_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, maxCustomBorderColorSamplers, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDepthClipEnableFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceDepthClipEnableFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_DEPTH_CLIP_ENABLE_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, depthClipEnable, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDeviceMemoryReportFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceDeviceMemoryReportFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_DEVICE_MEMORY_REPORT_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, deviceMemoryReport, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDiscardRectanglePropertiesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceDiscardRectanglePropertiesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_DISCARD_RECTANGLES_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, maxDiscardRectangles, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceExtendedDynamicStateFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceExtendedDynamicStateFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, extendedDynamicState, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceExtendedDynamicState2FeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceExtendedDynamicState2FeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, extendedDynamicState2, WarnIfNotEqual);
        GET_VALUE_WARN(member, extendedDynamicState2LogicOp, WarnIfNotEqual);
        GET_VALUE_WARN(member, extendedDynamicState2PatchControlPoints, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceExternalMemoryHostPropertiesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceExternalMemoryHostPropertiesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_EXTERNAL_MEMORY_HOST_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, minImportedHostPointerAlignment, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentDensityMapFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceFragmentDensityMapFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_FRAGMENT_DENSITY_MAP_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, fragmentDensityMap, WarnIfNotEqual);
        GET_VALUE_WARN(member, fragmentDensityMapDynamic, WarnIfNotEqual);
        GET_VALUE_WARN(member, fragmentDensityMapNonSubsampledImages, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentDensityMapPropertiesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceFragmentDensityMapPropertiesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_FRAGMENT_DENSITY_MAP_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, minFragmentDensityTexelSize, WarnIfLesser);
        GET_VALUE_WARN(prop, maxFragmentDensityTexelSize, WarnIfGreater);
        GET_VALUE_WARN(prop, fragmentDensityInvocations, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, fragmentShaderSampleInterlock, WarnIfNotEqual);
        GET_VALUE_WARN(member, fragmentShaderPixelInterlock, WarnIfNotEqual);
        GET_VALUE_WARN(member, fragmentShaderShadingRateInterlock, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_GLOBAL_PRIORITY_QUERY_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, globalPriorityQuery, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceImageRobustnessFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceImageRobustnessFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_IMAGE_ROBUSTNESS_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, robustImageAccess, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceIndexTypeUint8FeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceIndexTypeUint8FeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_INDEX_TYPE_UINT8_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, indexTypeUint8, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceInlineUniformBlockFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceInlineUniformBlockFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, inlineUniformBlock, WarnIfNotEqual);
        GET_VALUE_WARN(member, descriptorBindingInlineUniformBlockUpdateAfterBind, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceInlineUniformBlockPropertiesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceInlineUniformBlockPropertiesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, maxInlineUniformBlockSize, WarnIfGreater);
        GET_VALUE_WARN(prop, maxPerStageDescriptorInlineUniformBlocks, WarnIfGreater);
        GET_VALUE_WARN(prop, maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks, WarnIfGreater);
        GET_VALUE_WARN(prop, maxDescriptorSetInlineUniformBlocks, WarnIfGreater);
        GET_VALUE_WARN(prop, maxDescriptorSetUpdateAfterBindInlineUniformBlocks, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceLineRasterizationFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceLineRasterizationFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, rectangularLines, WarnIfNotEqual);
        GET_VALUE_WARN(member, bresenhamLines, WarnIfNotEqual);
        GET_VALUE_WARN(member, smoothLines, WarnIfNotEqual);
        GET_VALUE_WARN(member, stippledRectangularLines, WarnIfNotEqual);
        GET_VALUE_WARN(member, stippledBresenhamLines, WarnIfNotEqual);
        GET_VALUE_WARN(member, stippledSmoothLines, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceLineRasterizationPropertiesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceLineRasterizationPropertiesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, lineSubPixelPrecisionBits, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceMemoryPriorityFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceMemoryPriorityFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, memoryPriority, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceMultiDrawFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceMultiDrawFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_MULTI_DRAW_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, multiDraw, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceMultiDrawPropertiesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceMultiDrawPropertiesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_MULTI_DRAW_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, maxMultiDrawCount, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_PAGEABLE_DEVICE_LOCAL_MEMORY_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, pageableDeviceLocalMemory, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_PIPELINE_CREATION_CACHE_CONTROL_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, pipelineCreationCacheControl, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_PRIMITIVE_TOPOLOGY_LIST_RESTART_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, primitiveTopologyListRestart, WarnIfNotEqual);
        GET_VALUE_WARN(member, primitiveTopologyPatchListRestart, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDevicePrivateDataFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePrivateDataFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_PRIVATE_DATA_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, privateData, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceProvokingVertexFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceProvokingVertexFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_PROVOKING_VERTEX_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, provokingVertexLast, WarnIfNotEqual);
        GET_VALUE_WARN(member, transformFeedbackPreservesProvokingVertex, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceProvokingVertexPropertiesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceProvokingVertexPropertiesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_PROVOKING_VERTEX_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, provokingVertexModePerPipeline, WarnIfNotEqual);
        GET_VALUE_WARN(prop, transformFeedbackPreservesTriangleFanProvokingVertex, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_RGBA10X6_FORMATS_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, formatRgba10x6WithoutYCbCrSampler, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceRobustness2FeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceRobustness2FeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_ROBUSTNESS_2_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, robustBufferAccess2, WarnIfNotEqual);
        GET_VALUE_WARN(member, robustImageAccess2, WarnIfNotEqual);
        GET_VALUE_WARN(member, nullDescriptor, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceRobustness2PropertiesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceRobustness2PropertiesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_ROBUSTNESS_2_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, robustStorageBufferAccessSizeAlignment, WarnIfGreater);
        GET_VALUE_WARN(prop, robustUniformBufferAccessSizeAlignment, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceSampleLocationsPropertiesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceSampleLocationsPropertiesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_SAMPLE_LOCATIONS_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, sampleLocationSampleCounts, WarnIfGreater);
        GET_VALUE_WARN(prop, maxSampleLocationGridSize, WarnIfGreater);
        GET_ARRAY(sampleLocationCoordinateRange);
        GET_VALUE_WARN(prop, sampleLocationSubPixelBits, WarnIfGreater);
        GET_VALUE_WARN(prop, variableSampleLocations, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderAtomicFloatFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderAtomicFloatFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderBufferFloat32Atomics, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderBufferFloat32AtomicAdd, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderBufferFloat64Atomics, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderBufferFloat64AtomicAdd, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderSharedFloat32Atomics, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderSharedFloat32AtomicAdd, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderSharedFloat64Atomics, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderSharedFloat64AtomicAdd, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderImageFloat32Atomics, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderImageFloat32AtomicAdd, WarnIfNotEqual);
        GET_VALUE_WARN(member, sparseImageFloat32Atomics, WarnIfNotEqual);
        GET_VALUE_WARN(member, sparseImageFloat32AtomicAdd, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_SHADER_ATOMIC_FLOAT_2_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderBufferFloat16Atomics, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderBufferFloat16AtomicAdd, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderBufferFloat16AtomicMinMax, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderBufferFloat32AtomicMinMax, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderBufferFloat64AtomicMinMax, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderSharedFloat16Atomics, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderSharedFloat16AtomicAdd, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderSharedFloat16AtomicMinMax, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderSharedFloat32AtomicMinMax, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderSharedFloat64AtomicMinMax, WarnIfNotEqual);
        GET_VALUE_WARN(member, shaderImageFloat32AtomicMinMax, WarnIfNotEqual);
        GET_VALUE_WARN(member, sparseImageFloat32AtomicMinMax, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderDemoteToHelperInvocation, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_SHADER_IMAGE_ATOMIC_INT64_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderImageInt64Atomics, WarnIfNotEqual);
        GET_VALUE_WARN(member, sparseImageInt64Atomics, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceSubgroupSizeControlFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceSubgroupSizeControlFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, subgroupSizeControl, WarnIfNotEqual);
        GET_VALUE_WARN(member, computeFullSubgroups, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceSubgroupSizeControlPropertiesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceSubgroupSizeControlPropertiesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, minSubgroupSize, WarnIfLesser);
        GET_VALUE_WARN(prop, maxSubgroupSize, WarnIfGreater);
        GET_VALUE_WARN(prop, maxComputeWorkgroupSubgroups, WarnIfGreater);
        GET_VALUE_WARN(prop, requiredSubgroupSizeStages, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, texelBufferAlignment, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, storageTexelBufferOffsetAlignmentBytes, WarnIfGreater);
        GET_VALUE_WARN(prop, storageTexelBufferOffsetSingleTexelAlignment, WarnIfNotEqual);
        GET_VALUE_WARN(prop, uniformTexelBufferOffsetAlignmentBytes, WarnIfGreater);
        GET_VALUE_WARN(prop, uniformTexelBufferOffsetSingleTexelAlignment, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_TEXTURE_COMPRESSION_ASTC_HDR_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, textureCompressionASTC_HDR, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceTransformFeedbackFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceTransformFeedbackFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, transformFeedback, WarnIfNotEqual);
        GET_VALUE_WARN(member, geometryStreams, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceTransformFeedbackPropertiesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceTransformFeedbackPropertiesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, maxTransformFeedbackStreams, WarnIfGreater);
        GET_VALUE_WARN(prop, maxTransformFeedbackBuffers, WarnIfGreater);
        GET_VALUE_WARN(prop, maxTransformFeedbackBufferSize, WarnIfGreater);
        GET_VALUE_WARN(prop, maxTransformFeedbackStreamDataSize, WarnIfGreater);
        GET_VALUE_WARN(prop, maxTransformFeedbackBufferDataSize, WarnIfGreater);
        GET_VALUE_WARN(prop, maxTransformFeedbackBufferDataStride, WarnIfGreater);
        GET_VALUE_WARN(prop, transformFeedbackQueries, WarnIfNotEqual);
        GET_VALUE_WARN(prop, transformFeedbackStreamsLinesTriangles, WarnIfNotEqual);
        GET_VALUE_WARN(prop, transformFeedbackRasterizationStreamSelect, WarnIfNotEqual);
        GET_VALUE_WARN(prop, transformFeedbackDraw, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, vertexAttributeInstanceRateDivisor, WarnIfNotEqual);
        GET_VALUE_WARN(member, vertexAttributeInstanceRateZeroDivisor, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, maxVertexAttribDivisor, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, vertexInputDynamicState, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_YCBCR_2PLANE_444_FORMATS_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, ycbcr2plane444Formats, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceYcbcrImageArraysFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceYcbcrImageArraysFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_YCBCR_IMAGE_ARRAYS_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, ycbcrImageArrays, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentShadingRateFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceFragmentShadingRateFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, pipelineFragmentShadingRate, WarnIfNotEqual);
        GET_VALUE_WARN(member, primitiveFragmentShadingRate, WarnIfNotEqual);
        GET_VALUE_WARN(member, attachmentFragmentShadingRate, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentShadingRatePropertiesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceFragmentShadingRatePropertiesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, minFragmentShadingRateAttachmentTexelSize, WarnIfLesser);
        GET_VALUE_WARN(prop, maxFragmentShadingRateAttachmentTexelSize, WarnIfGreater);
        GET_VALUE_WARN(prop, maxFragmentShadingRateAttachmentTexelSizeAspectRatio, WarnIfGreater);
        GET_VALUE_WARN(prop, primitiveFragmentShadingRateWithMultipleViewports, WarnIfNotEqual);
        GET_VALUE_WARN(prop, layeredShadingRateAttachments, WarnIfNotEqual);
        GET_VALUE_WARN(prop, fragmentShadingRateNonTrivialCombinerOps, WarnIfNotEqual);
        GET_VALUE_WARN(prop, maxFragmentSize, WarnIfGreater);
        GET_VALUE_WARN(prop, maxFragmentSizeAspectRatio, WarnIfGreater);
        GET_VALUE_WARN(prop, maxFragmentShadingRateCoverageSamples, WarnIfGreater);
        GET_VALUE(prop, maxFragmentShadingRateRasterizationSamples);
        GET_VALUE_WARN(prop, fragmentShadingRateWithShaderDepthStencilWrites, WarnIfNotEqual);
        GET_VALUE_WARN(prop, fragmentShadingRateWithSampleMask, WarnIfNotEqual);
        GET_VALUE_WARN(prop, fragmentShadingRateWithShaderSampleMask, WarnIfGreater);
        GET_VALUE_WARN(prop, fragmentShadingRateWithConservativeRasterization, WarnIfNotEqual);
        GET_VALUE_WARN(prop, fragmentShadingRateWithFragmentShaderInterlock, WarnIfNotEqual);
        GET_VALUE_WARN(prop, fragmentShadingRateWithCustomSampleLocations, WarnIfNotEqual);
        GET_VALUE_WARN(prop, fragmentShadingRateStrictMultiplyCombiner, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceCoherentMemoryFeaturesAMD *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceCoherentMemoryFeaturesAMD)\n");
    if (!CheckExtensionSupport(VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, deviceCoherentMemory, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderCorePropertiesAMD *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderCorePropertiesAMD)\n");
    if (!CheckExtensionSupport(VK_AMD_SHADER_CORE_PROPERTIES_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, shaderEngineCount, WarnIfGreater);
        GET_VALUE_WARN(prop, shaderArraysPerEngineCount, WarnIfGreater);
        GET_VALUE_WARN(prop, computeUnitsPerShaderArray, WarnIfGreater);
        GET_VALUE_WARN(prop, simdPerComputeUnit, WarnIfGreater);
        GET_VALUE_WARN(prop, wavefrontsPerSimd, WarnIfGreater);
        GET_VALUE_WARN(prop, wavefrontSize, WarnIfGreater);
        GET_VALUE_WARN(prop, sgprsPerSimd, WarnIfGreater);
        GET_VALUE_WARN(prop, minSgprAllocation, WarnIfLesser);
        GET_VALUE_WARN(prop, maxSgprAllocation, WarnIfGreater);
        GET_VALUE_WARN(prop, sgprAllocationGranularity, WarnIfGreater);
        GET_VALUE_WARN(prop, vgprsPerSimd, WarnIfGreater);
        GET_VALUE_WARN(prop, minVgprAllocation, WarnIfLesser);
        GET_VALUE_WARN(prop, maxVgprAllocation, WarnIfGreater);
        GET_VALUE_WARN(prop, vgprAllocationGranularity, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderCoreProperties2AMD *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderCoreProperties2AMD)\n");
    if (!CheckExtensionSupport(VK_AMD_SHADER_CORE_PROPERTIES_2_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, shaderCoreFeatures, WarnIfGreater);
        GET_VALUE_WARN(prop, activeComputeUnitCount, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceInvocationMaskFeaturesHUAWEI *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceInvocationMaskFeaturesHUAWEI)\n");
    if (!CheckExtensionSupport(VK_HUAWEI_INVOCATION_MASK_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, invocationMask, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceSubpassShadingFeaturesHUAWEI *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceSubpassShadingFeaturesHUAWEI)\n");
    if (!CheckExtensionSupport(VK_HUAWEI_SUBPASS_SHADING_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, subpassShading, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceSubpassShadingPropertiesHUAWEI *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceSubpassShadingPropertiesHUAWEI)\n");
    if (!CheckExtensionSupport(VK_HUAWEI_SUBPASS_SHADING_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, maxSubpassShadingWorkgroupSizeAspectRatio, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL)\n");
    if (!CheckExtensionSupport(VK_INTEL_SHADER_INTEGER_FUNCTIONS_2_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderIntegerFunctions2, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceComputeShaderDerivativesFeaturesNV *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceComputeShaderDerivativesFeaturesNV)\n");
    if (!CheckExtensionSupport(VK_NV_COMPUTE_SHADER_DERIVATIVES_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, computeDerivativeGroupQuads, WarnIfNotEqual);
        GET_VALUE_WARN(member, computeDerivativeGroupLinear, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceCooperativeMatrixFeaturesNV *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceCooperativeMatrixFeaturesNV)\n");
    if (!CheckExtensionSupport(VK_NV_COOPERATIVE_MATRIX_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, cooperativeMatrix, WarnIfNotEqual);
        GET_VALUE_WARN(member, cooperativeMatrixRobustBufferAccess, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceCooperativeMatrixPropertiesNV *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceCooperativeMatrixPropertiesNV)\n");
    if (!CheckExtensionSupport(VK_NV_COOPERATIVE_MATRIX_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, cooperativeMatrixSupportedStages, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceCornerSampledImageFeaturesNV *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceCornerSampledImageFeaturesNV)\n");
    if (!CheckExtensionSupport(VK_NV_CORNER_SAMPLED_IMAGE_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, cornerSampledImage, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceCoverageReductionModeFeaturesNV *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceCoverageReductionModeFeaturesNV)\n");
    if (!CheckExtensionSupport(VK_NV_COVERAGE_REDUCTION_MODE_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, coverageReductionMode, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV)\n");
    if (!CheckExtensionSupport(VK_NV_DEDICATED_ALLOCATION_IMAGE_ALIASING_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, dedicatedAllocationImageAliasing, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDiagnosticsConfigFeaturesNV *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceDiagnosticsConfigFeaturesNV)\n");
    if (!CheckExtensionSupport(VK_NV_DEVICE_DIAGNOSTICS_CONFIG_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, diagnosticsConfig, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV)\n");
    if (!CheckExtensionSupport(VK_NV_DEVICE_GENERATED_COMMANDS_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, deviceGeneratedCommands, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDeviceGeneratedCommandsPropertiesNV *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceDeviceGeneratedCommandsPropertiesNV)\n");
    if (!CheckExtensionSupport(VK_NV_DEVICE_GENERATED_COMMANDS_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, maxGraphicsShaderGroupCount, WarnIfGreater);
        GET_VALUE_WARN(prop, maxIndirectSequenceCount, WarnIfGreater);
        GET_VALUE_WARN(prop, maxIndirectCommandsTokenCount, WarnIfGreater);
        GET_VALUE_WARN(prop, maxIndirectCommandsStreamCount, WarnIfGreater);
        GET_VALUE_WARN(prop, maxIndirectCommandsTokenOffset, WarnIfGreater);
        GET_VALUE_WARN(prop, maxIndirectCommandsStreamStride, WarnIfGreater);
        GET_VALUE_WARN(prop, minSequencesCountBufferOffsetAlignment, WarnIfGreater);
        GET_VALUE_WARN(prop, minSequencesIndexBufferOffsetAlignment, WarnIfGreater);
        GET_VALUE_WARN(prop, minIndirectCommandsBufferOffsetAlignment, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceExternalMemoryRDMAFeaturesNV *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceExternalMemoryRDMAFeaturesNV)\n");
    if (!CheckExtensionSupport(VK_NV_EXTERNAL_MEMORY_RDMA_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, externalMemoryRDMA, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV)\n");
    if (!CheckExtensionSupport(VK_NV_FRAGMENT_SHADER_BARYCENTRIC_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, fragmentShaderBarycentric, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV)\n");
    if (!CheckExtensionSupport(VK_NV_FRAGMENT_SHADING_RATE_ENUMS_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, fragmentShadingRateEnums, WarnIfNotEqual);
        GET_VALUE_WARN(member, supersampleFragmentShadingRates, WarnIfNotEqual);
        GET_VALUE_WARN(member, noInvocationFragmentShadingRates, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentShadingRateEnumsPropertiesNV *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceFragmentShadingRateEnumsPropertiesNV)\n");
    if (!CheckExtensionSupport(VK_NV_FRAGMENT_SHADING_RATE_ENUMS_EXTENSION_NAME)) {
        return false;
    }
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE(prop, maxFragmentShadingRateInvocationCount);
    }
    return true;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceInheritedViewportScissorFeaturesNV *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceInheritedViewportScissorFeaturesNV)\n");
    if (!CheckExtensionSupport(VK_NV_INHERITED_VIEWPORT_SCISSOR_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, inheritedViewportScissor2D, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceMeshShaderFeaturesNV *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceMeshShaderFeaturesNV)\n");
    if (!CheckExtensionSupport(VK_NV_MESH_SHADER_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, taskShader, WarnIfNotEqual);
        GET_VALUE_WARN(member, meshShader, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceMeshShaderPropertiesNV *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceMeshShaderPropertiesNV)\n");
    if (!CheckExtensionSupport(VK_NV_MESH_SHADER_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, maxDrawMeshTasksCount, WarnIfGreater);
        GET_VALUE_WARN(prop, maxTaskWorkGroupInvocations, WarnIfGreater);
        GET_ARRAY(maxTaskWorkGroupSize);
        GET_VALUE_WARN(prop, maxTaskTotalMemorySize, WarnIfGreater);
        GET_VALUE_WARN(prop, maxTaskOutputCount, WarnIfGreater);
        GET_VALUE_WARN(prop, maxMeshWorkGroupInvocations, WarnIfGreater);
        GET_ARRAY(maxMeshWorkGroupSize);
        GET_VALUE_WARN(prop, maxMeshTotalMemorySize, WarnIfGreater);
        GET_VALUE_WARN(prop, maxMeshOutputVertices, WarnIfGreater);
        GET_VALUE_WARN(prop, maxMeshOutputPrimitives, WarnIfGreater);
        GET_VALUE_WARN(prop, maxMeshMultiviewViewCount, WarnIfGreater);
        GET_VALUE_WARN(prop, meshOutputPerVertexGranularity, WarnIfGreater);
        GET_VALUE_WARN(prop, meshOutputPerPrimitiveGranularity, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceRayTracingPropertiesNV *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceRayTracingPropertiesNV)\n");
    if (!CheckExtensionSupport(VK_NV_RAY_TRACING_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, shaderGroupHandleSize, WarnIfGreater);
        GET_VALUE_WARN(prop, maxRecursionDepth, WarnIfGreater);
        GET_VALUE_WARN(prop, maxShaderGroupStride, WarnIfGreater);
        GET_VALUE_WARN(prop, shaderGroupBaseAlignment, WarnIfGreater);
        GET_VALUE_WARN(prop, maxGeometryCount, WarnIfGreater);
        GET_VALUE_WARN(prop, maxInstanceCount, WarnIfGreater);
        GET_VALUE_WARN(prop, maxTriangleCount, WarnIfGreater);
        GET_VALUE_WARN(prop, maxDescriptorSetAccelerationStructures, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceRayTracingMotionBlurFeaturesNV *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceRayTracingMotionBlurFeaturesNV)\n");
    if (!CheckExtensionSupport(VK_NV_RAY_TRACING_MOTION_BLUR_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, rayTracingMotionBlur, WarnIfNotEqual);
        GET_VALUE_WARN(member, rayTracingMotionBlurPipelineTraceRaysIndirect, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV)\n");
    if (!CheckExtensionSupport(VK_NV_REPRESENTATIVE_FRAGMENT_TEST_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, representativeFragmentTest, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceExclusiveScissorFeaturesNV *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceExclusiveScissorFeaturesNV)\n");
    if (!CheckExtensionSupport(VK_NV_SCISSOR_EXCLUSIVE_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, exclusiveScissor, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderImageFootprintFeaturesNV *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderImageFootprintFeaturesNV)\n");
    if (!CheckExtensionSupport(VK_NV_SHADER_IMAGE_FOOTPRINT_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, imageFootprint, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderSMBuiltinsFeaturesNV *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderSMBuiltinsFeaturesNV)\n");
    if (!CheckExtensionSupport(VK_NV_SHADER_SM_BUILTINS_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shaderSMBuiltins, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShaderSMBuiltinsPropertiesNV *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderSMBuiltinsPropertiesNV)\n");
    if (!CheckExtensionSupport(VK_NV_SHADER_SM_BUILTINS_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, shaderSMCount, WarnIfGreater);
        GET_VALUE_WARN(prop, shaderWarpsPerSM, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShadingRateImageFeaturesNV *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShadingRateImageFeaturesNV)\n");
    if (!CheckExtensionSupport(VK_NV_SHADING_RATE_IMAGE_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, shadingRateImage, WarnIfNotEqual);
        GET_VALUE_WARN(member, shadingRateCoarseSampleOrder, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceShadingRateImagePropertiesNV *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShadingRateImagePropertiesNV)\n");
    if (!CheckExtensionSupport(VK_NV_SHADING_RATE_IMAGE_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, shadingRateTexelSize, WarnIfGreater);
        GET_VALUE_WARN(prop, shadingRatePaletteSize, WarnIfGreater);
        GET_VALUE_WARN(prop, shadingRateMaxCoarseSamples, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE)\n");
    if (!CheckExtensionSupport(VK_VALVE_MUTABLE_DESCRIPTOR_TYPE_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, mutableDescriptorType, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDynamicRenderingFeaturesKHR *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceDynamicRenderingFeaturesKHR)\n");
    if (!CheckExtensionSupport(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, dynamicRendering, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceImageViewMinLodFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceImageViewMinLodFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_IMAGE_VIEW_MIN_LOD_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, minLod, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentDensityMap2FeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceFragmentDensityMap2FeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_FRAGMENT_DENSITY_MAP_2_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, fragmentDensityMapDeferred, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentDensityMap2PropertiesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceFragmentDensityMap2PropertiesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_FRAGMENT_DENSITY_MAP_2_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, subsampledLoads, WarnIfNotEqual);
        GET_VALUE_WARN(prop, subsampledCoarseReconstructionEarlyAccess, WarnIfNotEqual);
        GET_VALUE(prop, maxSubsampledArrayLayers);
        GET_VALUE(prop, maxDescriptorSetSubsampledSamplers);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM)\n");
    if (!CheckExtensionSupport(VK_QCOM_FRAGMENT_DENSITY_MAP_OFFSET_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, fragmentDensityMapOffset, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceFragmentDensityMapOffsetPropertiesQCOM *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceFragmentDensityMapOffsetPropertiesQCOM)\n");
    if (!CheckExtensionSupport(VK_QCOM_FRAGMENT_DENSITY_MAP_OFFSET_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, fragmentDensityOffsetGranularity, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceDepthClipControlFeaturesEXT *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceDepthClipControlFeaturesEXT)\n");
    if (!CheckExtensionSupport(VK_EXT_DEPTH_CLIP_CONTROL_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, depthClipControl, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM)\n");
    if (!CheckExtensionSupport(VK_ARM_RASTERIZATION_ORDER_ATTACHMENT_ACCESS_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, rasterizationOrderColorAttachmentAccess, WarnIfNotEqual);
        GET_VALUE_WARN(prop, rasterizationOrderDepthAttachmentAccess, WarnIfNotEqual);
        GET_VALUE_WARN(prop, rasterizationOrderStencilAttachmentAccess, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceLinearColorAttachmentFeaturesNV *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceLinearColorAttachmentFeaturesNV)\n");
    if (!CheckExtensionSupport(VK_NV_LINEAR_COLOR_ATTACHMENT_EXTENSION_NAME)) {
        return false;
    }
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, linearColorAttachment, WarnIfNotEqual);
    }
    return valid;
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

void JsonLoader::GetValue(const Json::Value &parent, int index, VkQueueFamilyProperties *dest) {
    const Json::Value value = parent[index];
    if (value.type() != Json::objectValue) {
        return;
    }
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE(prop, queueFlags);
        GET_VALUE(prop, queueCount);
        GET_VALUE(prop, timestampValidBits);
        GET_VALUE(prop, minImageTransferGranularity);
    }
}

void JsonLoader::GetValue(const Json::Value &parent, int index, VkMemoryType *dest) {
    const Json::Value value = parent[index];
    if (value.type() != Json::objectValue) {
        return;
    }
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE(prop, propertyFlags);
        GET_VALUE(prop, heapIndex);
    }
}

bool JsonLoader::GetValue(const Json::Value &parent, int index, VkMemoryHeap *dest) {
    const Json::Value value = parent[index];
    if (value.type() != Json::objectValue) {
        return true;
    }
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, size, WarnIfGreater);
        GET_MEMBER_VALUE(member, flags);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceMemoryProperties *dest) {
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceMemoryProperties)\n");
    const int heap_count = GET_ARRAY(memoryHeaps);  // size <= VK_MAX_MEMORY_HEAPS
    if (heap_count >= 0) {
        dest->memoryHeapCount = heap_count;
    }
    const int type_count = GET_ARRAY(memoryTypes);  // size <= VK_MAX_MEMORY_TYPES
    if (type_count >= 0) {
        dest->memoryTypeCount = type_count;
        for (int i = 0; i < type_count; ++i) {
            if (dest->memoryTypes[i].heapIndex >= dest->memoryHeapCount) {
                DebugPrintf("WARN \"memoryType[%" PRIu32 "].heapIndex\" (%" PRIu32 ") exceeds memoryHeapCount (%" PRIu32 ")\n", i,
                            dest->memoryTypes[i].heapIndex, dest->memoryHeapCount);
            }
        }
    }
    return true;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkSurfaceCapabilitiesKHR *dest) {
    bool valid = true;
    for (const auto &member : parent.getMemberNames()) {
        GET_VALUE_WARN(member, minImageCount, WarnIfLesser);
        GET_VALUE_WARN(member, maxImageCount, WarnIfGreater);
        GET_VALUE_WARN(member, minImageExtent, WarnIfLesser);
        GET_VALUE_WARN(member, maxImageExtent, WarnIfGreater);
        GET_VALUE_WARN(member, maxImageArrayLayers, WarnIfGreater);
        GET_VALUE(member, supportedTransforms);
        GET_VALUE(member, supportedCompositeAlpha);
        GET_VALUE(member, supportedUsageFlags);
    }
    return valid;
}

void JsonLoader::GetValue(const Json::Value &parent, int index, VkLayerProperties *dest) {
    const Json::Value value = parent[index];
    if (value.type() != Json::objectValue) {
        return;
    }
    for (const auto &prop : parent.getMemberNames()) {
        GET_ARRAY(layerName);  // size < VK_MAX_EXTENSION_NAME_SIZE
        GET_VALUE(prop, specVersion);
        GET_VALUE(prop, implementationVersion);
        GET_ARRAY(description);  // size < VK_MAX_DESCRIPTION_SIZE
    }
}

void JsonLoader::GetValue(const Json::Value &parent, int index, VkExtensionProperties *dest) {
    const Json::Value value = parent[index];
    if (value.type() != Json::objectValue) {
        return;
    }
    for (const auto &prop : parent.getMemberNames()) {
        GET_ARRAY(extensionName);  // size < VK_MAX_EXTENSION_NAME_SIZE
        GET_VALUE(prop, specVersion);
    }
}

void JsonLoader::GetValue(const Json::Value &parent, int index, VkSurfaceFormatKHR *dest) {
    const Json::Value value = parent[index];
    if (value.type() != Json::objectValue) {
        return;
    }
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE(prop, format);
        GET_VALUE(prop, colorSpace);
    }
}

void JsonLoader::GetValue(const Json::Value &parent, int index, VkPresentModeKHR *dest) {
    const Json::Value value = parent[index];
    if (!value.isInt()) {
        return;
    }
    *dest = static_cast<VkPresentModeKHR>(value.asInt());
}

void JsonLoader::GetValue(const Json::Value &parent, int index, VkPhysicalDevice *dest) {
    const Json::Value value = parent[index];
    if (!value.isUInt64()) {
        return;
    }
    *dest = reinterpret_cast<VkPhysicalDevice>(value.asUInt64());
}

void JsonLoader::GetValue(const Json::Value &parent, int index, VkDeviceSize *dest) {
    const Json::Value value = parent[index];
    if (!value.isUInt64()) {
        return;
    }
    *dest = static_cast<VkDeviceSize>(value.asUInt64());
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceVulkan12Properties *dest) {
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, framebufferIntegerColorSampleCounts, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceVulkan13Properties *dest) {
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, minSubgroupSize, WarnIfGreater);
        GET_VALUE_WARN(prop, maxSubgroupSize, WarnIfGreater);
        GET_VALUE_WARN(prop, maxComputeWorkgroupSubgroups, WarnIfGreater);
        GET_VALUE_WARN(prop, requiredSubgroupSizeStages, WarnIfGreater);
        GET_VALUE_WARN(prop, maxInlineUniformBlockSize, WarnIfGreater);
        GET_VALUE_WARN(prop, maxPerStageDescriptorInlineUniformBlocks, WarnIfGreater);
        GET_VALUE_WARN(prop, maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks, WarnIfGreater);
        GET_VALUE_WARN(prop, maxDescriptorSetInlineUniformBlocks, WarnIfGreater);
        GET_VALUE_WARN(prop, maxDescriptorSetUpdateAfterBindInlineUniformBlocks, WarnIfGreater);
        GET_VALUE_WARN(prop, maxInlineUniformTotalSize, WarnIfGreater);
        GET_VALUE_WARN(prop, integerDotProduct8BitUnsignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct8BitSignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct8BitMixedSignednessAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct4x8BitPackedUnsignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct4x8BitPackedSignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct4x8BitPackedMixedSignednessAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct16BitUnsignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct16BitSignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct16BitMixedSignednessAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct32BitUnsignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct32BitSignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct32BitMixedSignednessAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct64BitUnsignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct64BitSignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProduct64BitMixedSignednessAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating8BitUnsignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating8BitSignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating8BitMixedSignednessAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating4x8BitPackedUnsignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating4x8BitPackedSignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating4x8BitPackedMixedSignednessAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating16BitUnsignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating16BitSignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating16BitMixedSignednessAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating32BitUnsignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating32BitSignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating32BitMixedSignednessAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating64BitUnsignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating64BitSignedAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, integerDotProductAccumulatingSaturating64BitMixedSignednessAccelerated, WarnIfNotEqual);
        GET_VALUE_WARN(prop, storageTexelBufferOffsetAlignmentBytes, WarnIfGreater);
        GET_VALUE_WARN(prop, storageTexelBufferOffsetSingleTexelAlignment, WarnIfNotEqual);
        GET_VALUE_WARN(prop, uniformTexelBufferOffsetAlignmentBytes, WarnIfGreater);
        GET_VALUE_WARN(prop, uniformTexelBufferOffsetSingleTexelAlignment, WarnIfNotEqual);
        GET_VALUE_WARN(prop, maxBufferSize, WarnIfGreater);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceVulkan12Features *dest) {
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, samplerMirrorClampToEdge, WarnIfNotEqual);
        GET_VALUE_WARN(prop, shaderOutputViewportIndex, WarnIfNotEqual);
        GET_VALUE_WARN(prop, shaderOutputLayer, WarnIfNotEqual);
        GET_VALUE_WARN(prop, subgroupBroadcastDynamicId, WarnIfNotEqual);
        GET_VALUE_WARN(prop, drawIndirectCount, WarnIfNotEqual);
        GET_VALUE_WARN(prop, descriptorIndexing, WarnIfNotEqual);
        GET_VALUE_WARN(prop, samplerFilterMinmax, WarnIfNotEqual);
    }
    return valid;
}

bool JsonLoader::GetValue(const Json::Value &parent, VkPhysicalDeviceVulkan13Features *dest) {
    bool valid = true;
    for (const auto &prop : parent.getMemberNames()) {
        GET_VALUE_WARN(prop, robustImageAccess, WarnIfNotEqual);
        GET_VALUE_WARN(prop, inlineUniformBlock, WarnIfNotEqual);
        GET_VALUE_WARN(prop, descriptorBindingInlineUniformBlockUpdateAfterBind, WarnIfNotEqual);
        GET_VALUE_WARN(prop, pipelineCreationCacheControl, WarnIfNotEqual);
        GET_VALUE_WARN(prop, privateData, WarnIfNotEqual);
        GET_VALUE_WARN(prop, shaderDemoteToHelperInvocation, WarnIfNotEqual);
        GET_VALUE_WARN(prop, shaderTerminateInvocation, WarnIfNotEqual);
        GET_VALUE_WARN(prop, subgroupSizeControl, WarnIfNotEqual);
        GET_VALUE_WARN(prop, computeFullSubgroups, WarnIfNotEqual);
        GET_VALUE_WARN(prop, synchronization2, WarnIfNotEqual);
        GET_VALUE_WARN(prop, textureCompressionASTC_HDR, WarnIfNotEqual);
        GET_VALUE_WARN(prop, shaderZeroInitializeWorkgroupMemory, WarnIfNotEqual);
        GET_VALUE_WARN(prop, dynamicRendering, WarnIfNotEqual);
        GET_VALUE_WARN(prop, shaderIntegerDotProduct, WarnIfNotEqual);
        GET_VALUE_WARN(prop, maintenance4, WarnIfNotEqual);
    }
    return valid;
}

void JsonLoader::GetValueGPUinfoCore11(const Json::Value &parent) {
    const Json::Value core11 = parent["core11"];
    if (core11.type() != Json::objectValue) {
        return;
    }

    const Json::Value core11_features = core11["features"];
    if (core11_features.type() == Json::objectValue) {
        GetValue(core11_features, "storageBuffer16BitAccess", "storageBuffer16BitAccess",
                 &pdd_.physical_device_16bit_storage_features_.storageBuffer16BitAccess);
        GetValue(core11_features, "uniformAndStorageBuffer16BitAccess",
                 &pdd_.physical_device_16bit_storage_features_.uniformAndStorageBuffer16BitAccess);
        GetValue(core11_features, "storagePushConstant16", &pdd_.physical_device_16bit_storage_features_.storagePushConstant16);
        GetValue(core11_features, "storageInputOutput16", &pdd_.physical_device_16bit_storage_features_.storageInputOutput16);

        GetValue(core11_features, "multiview", &pdd_.physical_device_multiview_features_.multiview);
        GetValue(core11_features, "multiviewGeometryShader", &pdd_.physical_device_multiview_features_.multiviewGeometryShader);
        GetValue(core11_features, "multiviewTessellationShader",
                 &pdd_.physical_device_multiview_features_.multiviewTessellationShader);

        GetValue(core11_features, "variablePointersStorageBuffer",
                 &pdd_.physical_device_variable_pointers_features_.variablePointersStorageBuffer);
        GetValue(core11_features, "variablePointers", &pdd_.physical_device_variable_pointers_features_.variablePointers);

        GetValue(core11_features, "protectedMemory", &pdd_.physical_device_protected_memory_features_.protectedMemory);

        GetValue(core11_features, "samplerYcbcrConversion",
                 &pdd_.physical_device_sampler_ycbcr_conversion_features_.samplerYcbcrConversion);

        GetValue(core11_features, "shaderDrawParameters",
                 &pdd_.physical_device_shader_draw_parameters_features_.shaderDrawParameters);
    }

    const Json::Value core11_properties = core11["properties"];
    if (core11_properties.type() == Json::objectValue) {
        GetValue(core11_properties, "subgroupSize", &pdd_.physical_device_subgroup_properties_.subgroupSize);
        GetValue(core11_properties, "subgroupSupportedStages", &pdd_.physical_device_subgroup_properties_.supportedStages);
        GetValue(core11_properties, "subgroupSupportedOperations", &pdd_.physical_device_subgroup_properties_.supportedOperations);
        GetValue(core11_properties, "subgroupQuadOperationsInAllStages",
                 &pdd_.physical_device_subgroup_properties_.quadOperationsInAllStages);

        GetValue(core11_properties, "maxMultiviewViewCount", &pdd_.physical_device_multiview_properties_.maxMultiviewViewCount);
        GetValue(core11_properties, "maxMultiviewInstanceIndex",
                 &pdd_.physical_device_multiview_properties_.maxMultiviewInstanceIndex);

        GetValue(core11_properties, "protectedNoFault", &pdd_.physical_device_protected_memory_properties_.protectedNoFault);

        GetValue(core11_properties, "maxPerSetDescriptors", &pdd_.physical_device_maintenance_3_properties_.maxPerSetDescriptors);
        GetValue(core11_properties, "maxMemoryAllocationSize",
                 &pdd_.physical_device_maintenance_3_properties_.maxMemoryAllocationSize);
    }

    const Json::Value device_props = parent["VkPhysicalDeviceProperties"];
    if (device_props.type() == Json::objectValue) {
        GetValue(device_props, &pdd_.physical_device_subgroup_properties_);
    }
}

void JsonLoader::GetValueGPUinfoCore12(const Json::Value &parent) {
    const Json::Value core12 = parent["core12"];
    if (core12.type() != Json::objectValue) {
        return;
    }

    const Json::Value core12_features = core12["features"];
    if (core12_features.type() == Json::objectValue) {
        GetValue(core12_features, "samplerMirrorClampToEdge", &pdd_.physical_device_vulkan_1_2_features_.samplerMirrorClampToEdge);
        GetValue(core12_features, "shaderOutputViewportIndex",
                 &pdd_.physical_device_vulkan_1_2_features_.shaderOutputViewportIndex);
        GetValue(core12_features, "shaderOutputLayer", &pdd_.physical_device_vulkan_1_2_features_.shaderOutputLayer);
        GetValue(core12_features, "subgroupBroadcastDynamicId",
                 &pdd_.physical_device_vulkan_1_2_features_.subgroupBroadcastDynamicId);
        GetValue(core12_features, "drawIndirectCount", &pdd_.physical_device_vulkan_1_2_features_.drawIndirectCount);
        GetValue(core12_features, "descriptorIndexing", &pdd_.physical_device_vulkan_1_2_features_.descriptorIndexing);
        GetValue(core12_features, "samplerFilterMinmax", &pdd_.physical_device_vulkan_1_2_features_.samplerFilterMinmax);

        GetValue(core12_features, "storageBuffer8BitAccess", &pdd_.physical_device_8bit_storage_features_.storageBuffer8BitAccess);
        GetValue(core12_features, "uniformAndStorageBuffer8BitAccess",
                 &pdd_.physical_device_8bit_storage_features_.uniformAndStorageBuffer8BitAccess);
        GetValue(core12_features, "storagePushConstant8", &pdd_.physical_device_8bit_storage_features_.storagePushConstant8);

        GetValue(core12_features, "shaderBufferInt64Atomics",
                 &pdd_.physical_device_shader_atomic_int64_features_.shaderBufferInt64Atomics);
        GetValue(core12_features, "shaderSharedInt64Atomics",
                 &pdd_.physical_device_shader_atomic_int64_features_.shaderSharedInt64Atomics);

        GetValue(core12_features, "shaderFloat16", &pdd_.physical_device_shader_float16_int8_features_.shaderFloat16);
        GetValue(core12_features, "shaderInt8", &pdd_.physical_device_shader_float16_int8_features_.shaderInt8);

        GetValue(core12_features, "shaderInputAttachmentArrayDynamicIndexing",
                 &pdd_.physical_device_descriptor_indexing_features_.shaderInputAttachmentArrayDynamicIndexing);
        GetValue(core12_features, "shaderUniformTexelBufferArrayDynamicIndexing",
                 &pdd_.physical_device_descriptor_indexing_features_.shaderUniformTexelBufferArrayDynamicIndexing);
        GetValue(core12_features, "shaderStorageTexelBufferArrayDynamicIndexing",
                 &pdd_.physical_device_descriptor_indexing_features_.shaderStorageTexelBufferArrayDynamicIndexing);
        GetValue(core12_features, "shaderUniformBufferArrayNonUniformIndexing",
                 &pdd_.physical_device_descriptor_indexing_features_.shaderUniformBufferArrayNonUniformIndexing);
        GetValue(core12_features, "shaderSampledImageArrayNonUniformIndexing",
                 &pdd_.physical_device_descriptor_indexing_features_.shaderSampledImageArrayNonUniformIndexing);
        GetValue(core12_features, "shaderStorageBufferArrayNonUniformIndexing",
                 &pdd_.physical_device_descriptor_indexing_features_.shaderStorageBufferArrayNonUniformIndexing);
        GetValue(core12_features, "shaderStorageImageArrayNonUniformIndexing",
                 &pdd_.physical_device_descriptor_indexing_features_.shaderStorageImageArrayNonUniformIndexing);
        GetValue(core12_features, "shaderInputAttachmentArrayNonUniformIndexing",
                 &pdd_.physical_device_descriptor_indexing_features_.shaderInputAttachmentArrayNonUniformIndexing);
        GetValue(core12_features, "shaderUniformTexelBufferArrayNonUniformIndexing",
                 &pdd_.physical_device_descriptor_indexing_features_.shaderUniformTexelBufferArrayNonUniformIndexing);
        GetValue(core12_features, "shaderStorageTexelBufferArrayNonUniformIndexing",
                 &pdd_.physical_device_descriptor_indexing_features_.shaderStorageTexelBufferArrayNonUniformIndexing);
        GetValue(core12_features, "descriptorBindingUniformBufferUpdateAfterBind",
                 &pdd_.physical_device_descriptor_indexing_features_.descriptorBindingUniformBufferUpdateAfterBind);
        GetValue(core12_features, "descriptorBindingSampledImageUpdateAfterBind",
                 &pdd_.physical_device_descriptor_indexing_features_.descriptorBindingSampledImageUpdateAfterBind);
        GetValue(core12_features, "descriptorBindingStorageImageUpdateAfterBind",
                 &pdd_.physical_device_descriptor_indexing_features_.descriptorBindingStorageImageUpdateAfterBind);
        GetValue(core12_features, "descriptorBindingStorageBufferUpdateAfterBind",
                 &pdd_.physical_device_descriptor_indexing_features_.descriptorBindingStorageBufferUpdateAfterBind);
        GetValue(core12_features, "descriptorBindingUniformTexelBufferUpdateAfterBind",
                 &pdd_.physical_device_descriptor_indexing_features_.descriptorBindingUniformTexelBufferUpdateAfterBind);
        GetValue(core12_features, "descriptorBindingStorageTexelBufferUpdateAfterBind",
                 &pdd_.physical_device_descriptor_indexing_features_.descriptorBindingStorageTexelBufferUpdateAfterBind);
        GetValue(core12_features, "descriptorBindingUpdateUnusedWhilePending",
                 &pdd_.physical_device_descriptor_indexing_features_.descriptorBindingUpdateUnusedWhilePending);
        GetValue(core12_features, "descriptorBindingPartiallyBound",
                 &pdd_.physical_device_descriptor_indexing_features_.descriptorBindingPartiallyBound);
        GetValue(core12_features, "descriptorBindingVariableDescriptorCount",
                 &pdd_.physical_device_descriptor_indexing_features_.descriptorBindingVariableDescriptorCount);
        GetValue(core12_features, "runtimeDescriptorArray",
                 &pdd_.physical_device_descriptor_indexing_features_.runtimeDescriptorArray);

        GetValue(core12_features, "scalarBlockLayout", &pdd_.physical_device_scalar_block_layout_features_.scalarBlockLayout);

        GetValue(core12_features, "imagelessFramebuffer",
                 &pdd_.physical_device_imageless_framebuffer_features_.imagelessFramebuffer);

        GetValue(core12_features, "uniformBufferStandardLayout",
                 &pdd_.physical_device_uniform_buffer_standard_layout_features_.uniformBufferStandardLayout);

        GetValue(core12_features, "shaderSubgroupExtendedTypes",
                 &pdd_.physical_device_shader_subgroup_extended_types_features_.shaderSubgroupExtendedTypes);

        GetValue(core12_features, "separateDepthStencilLayouts",
                 &pdd_.physical_device_separate_depth_stencil_layouts_features_.separateDepthStencilLayouts);

        GetValue(core12_features, "hostQueryReset", &pdd_.physical_device_host_query_reset_features_.hostQueryReset);

        GetValue(core12_features, "timelineSemaphore", &pdd_.physical_device_timeline_semaphore_features_.timelineSemaphore);

        GetValue(core12_features, "bufferDeviceAddress", &pdd_.physical_device_buffer_device_address_features_.bufferDeviceAddress);
        GetValue(core12_features, "bufferDeviceAddressCaptureReplay",
                 &pdd_.physical_device_buffer_device_address_features_.bufferDeviceAddressCaptureReplay);
        GetValue(core12_features, "bufferDeviceAddressMultiDevice",
                 &pdd_.physical_device_buffer_device_address_features_.bufferDeviceAddressMultiDevice);

        GetValue(core12_features, "vulkanMemoryModel", &pdd_.physical_device_vulkan_memory_model_features_.vulkanMemoryModel);
        GetValue(core12_features, "vulkanMemoryModelDeviceScope",
                 &pdd_.physical_device_vulkan_memory_model_features_.vulkanMemoryModelDeviceScope);
        GetValue(core12_features, "vulkanMemoryModelAvailabilityVisibilityChains",
                 &pdd_.physical_device_vulkan_memory_model_features_.vulkanMemoryModelAvailabilityVisibilityChains);
    }

    const Json::Value core12_properties = core12["properties"];
    if (core12_properties.type() == Json::objectValue) {
        GetValue(core12_properties, "denormBehaviorIndependence", "denormBehaviorIndependence",
                 &pdd_.physical_device_float_controls_properties_.denormBehaviorIndependence);
        GetValue(core12_properties, "roundingModeIndependence",
                 &pdd_.physical_device_float_controls_properties_.roundingModeIndependence);
        GetValue(core12_properties, "shaderSignedZeroInfNanPreserveFloat16",
                 &pdd_.physical_device_float_controls_properties_.shaderSignedZeroInfNanPreserveFloat16);
        GetValue(core12_properties, "shaderSignedZeroInfNanPreserveFloat32",
                 &pdd_.physical_device_float_controls_properties_.shaderSignedZeroInfNanPreserveFloat32);
        GetValue(core12_properties, "shaderSignedZeroInfNanPreserveFloat64",
                 &pdd_.physical_device_float_controls_properties_.shaderSignedZeroInfNanPreserveFloat64);
        GetValue(core12_properties, "shaderDenormPreserveFloat16",
                 &pdd_.physical_device_float_controls_properties_.shaderDenormPreserveFloat16);
        GetValue(core12_properties, "shaderDenormPreserveFloat32",
                 &pdd_.physical_device_float_controls_properties_.shaderDenormPreserveFloat32);
        GetValue(core12_properties, "shaderDenormPreserveFloat64",
                 &pdd_.physical_device_float_controls_properties_.shaderDenormPreserveFloat64);
        GetValue(core12_properties, "shaderDenormFlushToZeroFloat16",
                 &pdd_.physical_device_float_controls_properties_.shaderDenormFlushToZeroFloat16);
        GetValue(core12_properties, "shaderDenormFlushToZeroFloat32",
                 &pdd_.physical_device_float_controls_properties_.shaderDenormFlushToZeroFloat32);
        GetValue(core12_properties, "shaderDenormFlushToZeroFloat64",
                 &pdd_.physical_device_float_controls_properties_.shaderDenormFlushToZeroFloat64);
        GetValue(core12_properties, "shaderRoundingModeRTEFloat16",
                 &pdd_.physical_device_float_controls_properties_.shaderRoundingModeRTEFloat16);
        GetValue(core12_properties, "shaderRoundingModeRTEFloat32",
                 &pdd_.physical_device_float_controls_properties_.shaderRoundingModeRTEFloat32);
        GetValue(core12_properties, "shaderRoundingModeRTEFloat64",
                 &pdd_.physical_device_float_controls_properties_.shaderRoundingModeRTEFloat64);
        GetValue(core12_properties, "shaderRoundingModeRTZFloat16",
                 &pdd_.physical_device_float_controls_properties_.shaderRoundingModeRTZFloat16);
        GetValue(core12_properties, "shaderRoundingModeRTZFloat32",
                 &pdd_.physical_device_float_controls_properties_.shaderRoundingModeRTZFloat32);
        GetValue(core12_properties, "shaderRoundingModeRTZFloat64",
                 &pdd_.physical_device_float_controls_properties_.shaderRoundingModeRTZFloat64);

        GetValue(core12_properties, "maxUpdateAfterBindDescriptorsInAllPools",
                 &pdd_.physical_device_descriptor_indexing_properties_.maxUpdateAfterBindDescriptorsInAllPools);
        GetValue(core12_properties, "shaderUniformBufferArrayNonUniformIndexingNative",
                 &pdd_.physical_device_descriptor_indexing_properties_.shaderUniformBufferArrayNonUniformIndexingNative);
        GetValue(core12_properties, "shaderSampledImageArrayNonUniformIndexingNative",
                 &pdd_.physical_device_descriptor_indexing_properties_.shaderSampledImageArrayNonUniformIndexingNative);
        GetValue(core12_properties, "shaderStorageBufferArrayNonUniformIndexingNative",
                 &pdd_.physical_device_descriptor_indexing_properties_.shaderStorageBufferArrayNonUniformIndexingNative);
        GetValue(core12_properties, "shaderStorageImageArrayNonUniformIndexingNative",
                 &pdd_.physical_device_descriptor_indexing_properties_.shaderStorageImageArrayNonUniformIndexingNative);
        GetValue(core12_properties, "shaderInputAttachmentArrayNonUniformIndexingNative",
                 &pdd_.physical_device_descriptor_indexing_properties_.shaderInputAttachmentArrayNonUniformIndexingNative);
        GetValue(core12_properties, "robustBufferAccessUpdateAfterBind",
                 &pdd_.physical_device_descriptor_indexing_properties_.robustBufferAccessUpdateAfterBind);
        GetValue(core12_properties, "quadDivergentImplicitLod",
                 &pdd_.physical_device_descriptor_indexing_properties_.quadDivergentImplicitLod);
        GetValue(core12_properties, "maxPerStageDescriptorUpdateAfterBindSamplers",
                 &pdd_.physical_device_descriptor_indexing_properties_.maxPerStageDescriptorUpdateAfterBindSamplers);
        GetValue(core12_properties, "maxPerStageDescriptorUpdateAfterBindUniformBuffers",
                 &pdd_.physical_device_descriptor_indexing_properties_.maxPerStageDescriptorUpdateAfterBindUniformBuffers);
        GetValue(core12_properties, "maxPerStageDescriptorUpdateAfterBindStorageBuffers",
                 &pdd_.physical_device_descriptor_indexing_properties_.maxPerStageDescriptorUpdateAfterBindStorageBuffers);
        GetValue(core12_properties, "maxPerStageDescriptorUpdateAfterBindSampledImages",
                 &pdd_.physical_device_descriptor_indexing_properties_.maxPerStageDescriptorUpdateAfterBindSampledImages);
        GetValue(core12_properties, "maxPerStageDescriptorUpdateAfterBindStorageImages",
                 &pdd_.physical_device_descriptor_indexing_properties_.maxPerStageDescriptorUpdateAfterBindStorageImages);
        GetValue(core12_properties, "maxPerStageDescriptorUpdateAfterBindInputAttachments",
                 &pdd_.physical_device_descriptor_indexing_properties_.maxPerStageDescriptorUpdateAfterBindInputAttachments);
        GetValue(core12_properties, "maxPerStageUpdateAfterBindResources",
                 &pdd_.physical_device_descriptor_indexing_properties_.maxPerStageUpdateAfterBindResources);
        GetValue(core12_properties, "maxDescriptorSetUpdateAfterBindSamplers",
                 &pdd_.physical_device_descriptor_indexing_properties_.maxDescriptorSetUpdateAfterBindSamplers);
        GetValue(core12_properties, "maxDescriptorSetUpdateAfterBindUniformBuffers",
                 &pdd_.physical_device_descriptor_indexing_properties_.maxDescriptorSetUpdateAfterBindUniformBuffers);
        GetValue(core12_properties, "maxDescriptorSetUpdateAfterBindUniformBuffersDynamic",
                 &pdd_.physical_device_descriptor_indexing_properties_.maxDescriptorSetUpdateAfterBindUniformBuffersDynamic);
        GetValue(core12_properties, "maxDescriptorSetUpdateAfterBindStorageBuffers",
                 &pdd_.physical_device_descriptor_indexing_properties_.maxDescriptorSetUpdateAfterBindStorageBuffers);
        GetValue(core12_properties, "maxDescriptorSetUpdateAfterBindStorageBuffersDynamic",
                 &pdd_.physical_device_descriptor_indexing_properties_.maxDescriptorSetUpdateAfterBindStorageBuffersDynamic);
        GetValue(core12_properties, "maxDescriptorSetUpdateAfterBindSampledImages",
                 &pdd_.physical_device_descriptor_indexing_properties_.maxDescriptorSetUpdateAfterBindSampledImages);
        GetValue(core12_properties, "maxDescriptorSetUpdateAfterBindStorageImages",
                 &pdd_.physical_device_descriptor_indexing_properties_.maxDescriptorSetUpdateAfterBindStorageImages);
        GetValue(core12_properties, "maxDescriptorSetUpdateAfterBindInputAttachments",
                 &pdd_.physical_device_descriptor_indexing_properties_.maxDescriptorSetUpdateAfterBindInputAttachments);

        GetValue(core12_properties, "supportedDepthResolveModes",
                 &pdd_.physical_device_depth_stencil_resolve_properties_.supportedDepthResolveModes);
        GetValue(core12_properties, "supportedStencilResolveModes",
                 &pdd_.physical_device_depth_stencil_resolve_properties_.supportedStencilResolveModes);
        GetValue(core12_properties, "independentResolveNone",
                 &pdd_.physical_device_depth_stencil_resolve_properties_.independentResolveNone);
        GetValue(core12_properties, "independentResolve",
                 &pdd_.physical_device_depth_stencil_resolve_properties_.independentResolve);

        GetValue(core12_properties, "filterMinmaxSingleComponentFormats",
                 &pdd_.physical_device_sampler_filter_minmax_properties_.filterMinmaxSingleComponentFormats);
        GetValue(core12_properties, "filterMinmaxImageComponentMapping",
                 &pdd_.physical_device_sampler_filter_minmax_properties_.filterMinmaxImageComponentMapping);

        GetValue(core12_properties, "maxTimelineSemaphoreValueDifference",
                 &pdd_.physical_device_timeline_semaphore_properties_.maxTimelineSemaphoreValueDifference);

        GetValue(core12_properties, "framebufferIntegerColorSampleCounts",
                 &pdd_.physical_device_vulkan_1_2_properties_.framebufferIntegerColorSampleCounts);
    }
}

void JsonLoader::GetValueGPUinfoSurfaceCapabilities(const Json::Value &parent) {
    const Json::Value surface_capabilities =
        parent["surfacecapabilites"];  // You may notice "surfacecapabilites" is missing an 'i'.  This is how it is spelled in the
                                       // GPUinfo format.
    if (surface_capabilities.type() != Json::objectValue) {
        return;
    }

    /*GetValue(surface_capabilities, "minImageCount", &pdd_.surface_capabilities_.minImageCount);
    GetValue(surface_capabilities, "maxImageCount", &pdd_.surface_capabilities_.maxImageCount);
    GetValue(surface_capabilities, "minImageExtent", &pdd_.surface_capabilities_.minImageExtent);
    GetValue(surface_capabilities, "maxImageExtent", &pdd_.surface_capabilities_.maxImageExtent);
    GetValue(surface_capabilities, "maxImageArrayLayers", &pdd_.surface_capabilities_.maxImageArrayLayers);
    GetValue(surface_capabilities, "supportedTransforms", &pdd_.surface_capabilities_.supportedTransforms);
    GetValue(surface_capabilities, "supportedCompositeAlpha", &pdd_.surface_capabilities_.supportedCompositeAlpha);
    GetValue(surface_capabilities, "supportedUsageFlags", &pdd_.surface_capabilities_.supportedUsageFlags);

    GetArray(surface_capabilities, "surfaceformats", &pdd_.arrayof_surface_formats_);
    GetArray(surface_capabilities, "presentmodes", &pdd_.arrayof_present_modes_);*/
}

#undef GET_VALUE
#undef GET_ARRAY

// Layer-specific wrappers for Vulkan functions, accessed via vkGet*ProcAddr() ///////////////////////////////////////////////////

// Fill the inputFilename variable with a value from either vk_layer_settings.txt or environment variables.
// Environment variables get priority.
static void GetProfilesFilename() {
    std::string env_var = GetEnvarValue(kEnvarProfilesFilename);
    if (!env_var.empty()) {
        inputFilename.str = env_var;
        inputFilename.fromEnvVar = true;
    }

    if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsProfileFile)) {
        inputFilename.fromEnvVar = false;
        inputFilename.str = vku::GetLayerSettingString(kOurLayerName, kLayerSettingsProfileFile);
    }
}

static int GetBooleanValue(const std::string &value) {
    std::string temp = value;
    std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);

    if (value.empty())
        return 0;
    else if (temp == "true")
        return 1;
    else if (temp == "false")
        return 0;
    else
#if defined(__ANDROID__)
        return atoi(temp.c_str());
#else
        return std::atoi(temp.c_str());
#endif
}

static SetCombinationMode GetSetCombinationModeValue(const std::string &value) {
    std::string temp = value;
    std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);

    if (temp == "check_support") {
        return SET_CHECK_SUPPORT;
    } else if (temp == "from_device") {
        return SET_FROM_DEVICE;
    } else if (temp == "from_profile") {
        return SET_FROM_PROFILE;
    } else if (temp == "from_profile_override") {
        return SET_FROM_PROFILE_OVERRIDE;
    }

    return SET_CHECK_SUPPORT;
}
// Fill the debugLevel variable with a value from either vk_layer_settings.txt or environment variables.
// Environment variables get priority.
static void GetProfilesDebugLevel() {
    std::string env_var = GetEnvarValue(kEnvarProfilesDebugEnable);
    if (!env_var.empty()) {
        debugLevel.fromEnvVar = true;
        debugLevel.num = GetBooleanValue(env_var);
    }

    if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsDebugOutput)) {
        debugLevel.fromEnvVar = false;
        debugLevel.num = vku::GetLayerSettingBool(kOurLayerName, kLayerSettingsDebugOutput);
    }
}

// Fill the emulatePortability variable with a value from either vk_layer_settings.txt or environment variables.
// Environment variables get priority.
static void GetProfilesEmulatePortability() {
    std::string env_var = GetEnvarValue(kEnvarProfilesEmulatePortability);
    if (!env_var.empty()) {
        emulatePortability.num = GetBooleanValue(env_var);
        emulatePortability.fromEnvVar = true;
    }

    if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsEmulatePortability)) {
        emulatePortability.fromEnvVar = false;
        emulatePortability.num = vku::GetLayerSettingBool(kOurLayerName, kLayerSettingsEmulatePortability);
    }
}

// Fill the modifyExtensionList variable with a value from either vk_layer_settings.txt or environment variables.
// Environment variables get priority.
static void GetProfilesModifyExtensionList() {
    std::string modify_extension_list = "";
    std::string env_var = GetEnvarValue(kEnvarProfilesModifyExtensionList);
    if (!env_var.empty()) {
        modify_extension_list = env_var;
        modifyExtensionList.fromEnvVar = true;
    }

    if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsModifyExtensionList)) {
        modify_extension_list = vku::GetLayerSettingString(kOurLayerName, kLayerSettingsModifyExtensionList);
        modifyExtensionList.fromEnvVar = false;
    }
    modifyExtensionList.mode = GetSetCombinationModeValue(modify_extension_list);
}

// Fill the modifyFormatList variable with a value from either vk_layer_settings.txt or environment variables.
// Environment variables get priority.
static void GetProfilesModifyFormatList() {
    std::string modify_format_list = "";
    std::string env_var = GetEnvarValue(kEnvarProfilesModifyFormatList);
    if (!env_var.empty()) {
        modify_format_list = env_var;
        modifyFormatList.fromEnvVar = true;
    }

    if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsModifyFormatList)) {
        modifyFormatList.fromEnvVar = false;
        modify_format_list = vku::GetLayerSettingString(kOurLayerName, kLayerSettingsModifyFormatList);
    }
    modifyFormatList.mode = GetSetCombinationModeValue(modify_format_list);
}

// Fill the modifyFormatProperties variable with a value from either vk_layer_settings.txt or environment variables.
// Environment variables get priority.
static void GetProfilesModifyFormatProperties() {
    std::string modify_format_properties = "";
    std::string env_var = GetEnvarValue(kEnvarProfilesModifyFormatProperties);
    if (!env_var.empty()) {
        modify_format_properties = env_var;
        modifyFormatProperties.fromEnvVar = true;
    }

    if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsModifyFormatProperties)) {
        modifyFormatProperties.fromEnvVar = false;
        modify_format_properties = vku::GetLayerSettingString(kOurLayerName, kLayerSettingsModifyFormatProperties);
    }
    modifyFormatProperties.mode = GetSetCombinationModeValue(modify_format_properties);
}

static void GetProfileName() {
    std::string env_var = GetEnvarValue(kEnvarProfilesProfileName);
    if (!env_var.empty()) {
        profileName.str = env_var;
        profileName.fromEnvVar = true;
    }

    if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsProfileName)) {
        profileName.fromEnvVar = false;
        profileName.str = vku::GetLayerSettingString(kOurLayerName, kLayerSettingsProfileName);
    }
}

// Generic layer dispatch table setup, see [LALI].
static VkResult LayerSetupCreateInstance(const VkInstanceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator,
                                         VkInstance *pInstance) {
    GetProfilesEmulatePortability();
    GetProfilesFilename();
    GetProfilesDebugLevel();
    GetProfilesModifyExtensionList();
    GetProfilesModifyFormatList();
    GetProfilesModifyFormatProperties();
    GetProfileName();

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
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL CreateInstance(const VkInstanceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator,
                                              VkInstance *pInstance) {
    DebugPrintf("CreateInstance ========================================\n");
    DebugPrintf("%s version %d.%d.%d\n", kOurLayerName, kVersionProfilesMajor, kVersionProfilesMinor, kVersionProfilesPatch);
    DebugPrintf("JsonCpp version %s\n", JSONCPP_VERSION_STRING);

    const VkApplicationInfo *app_info = pCreateInfo->pApplicationInfo;
    const uint32_t requested_version = (app_info && app_info->apiVersion) ? app_info->apiVersion : VK_API_VERSION_1_0;
    if (requested_version > VK_API_VERSION_1_3) {
        DebugPrintf("%s currently only supports VK_API_VERSION_1_3 and lower.\n", kOurLayerName);
    }

    std::lock_guard<std::recursive_mutex> lock(global_lock);

    if (VK_VERSION_MINOR(requested_version) > 0) {
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

    return LayerSetupCreateInstance(pCreateInfo, pAllocator, pInstance);
}

VKAPI_ATTR void VKAPI_CALL DestroyInstance(VkInstance instance, const VkAllocationCallbacks *pAllocator) {
    DebugPrintf("DestroyInstance\n");

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
    }
}

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

// Utility function for iterating through the pNext chain of certain Vulkan structs.
void FillPNextChain(PhysicalDeviceData *physicalDeviceData, void *place) {
    while (place) {
        VkBaseOutStructure *structure = (VkBaseOutStructure *)place;

        // These switch statements check which struct is in the pNext chain and, if the physical device has the proper extension,
        // fill the struct with any override data provided by the PhysicalDeviceData object.

        switch (structure->sType) {
            // VK_KHR_portability_subset is a special case since it can also be emulated by the Profiles layer.
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME) ||
                    emulatePortability.num > 0) {
                    VkPhysicalDevicePortabilitySubsetPropertiesKHR *psp = (VkPhysicalDevicePortabilitySubsetPropertiesKHR *)place;
                    void *pNext = psp->pNext;
                    *psp = physicalDeviceData->physical_device_portability_subset_properties_;
                    psp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR:
                if ((PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME) ||
                     emulatePortability.num > 0)) {
                    VkPhysicalDevicePortabilitySubsetFeaturesKHR *psf = (VkPhysicalDevicePortabilitySubsetFeaturesKHR *)place;
                    void *pNext = psf->pNext;
                    *psf = physicalDeviceData->physical_device_portability_subset_features_;
                    psf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_8BIT_STORAGE_EXTENSION_NAME)) {
                    VkPhysicalDevice8BitStorageFeaturesKHR *ebsf = (VkPhysicalDevice8BitStorageFeaturesKHR *)place;
                    void *pNext = ebsf->pNext;
                    *ebsf = physicalDeviceData->physical_device_8bit_storage_features_;
                    ebsf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_16BIT_STORAGE_EXTENSION_NAME)) {
                    VkPhysicalDevice16BitStorageFeaturesKHR *sbsf = (VkPhysicalDevice16BitStorageFeaturesKHR *)place;
                    void *pNext = sbsf->pNext;
                    *sbsf = physicalDeviceData->physical_device_16bit_storage_features_;
                    sbsf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR:
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME)) {
                    VkPhysicalDeviceBufferDeviceAddressFeaturesKHR *bdaf = (VkPhysicalDeviceBufferDeviceAddressFeaturesKHR *)place;
                    void *pNext = bdaf->pNext;
                    *bdaf = physicalDeviceData->physical_device_buffer_device_address_features_;
                    bdaf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME)) {
                    VkPhysicalDeviceDepthStencilResolvePropertiesKHR *dsrp =
                        (VkPhysicalDeviceDepthStencilResolvePropertiesKHR *)place;
                    void *pNext = dsrp->pNext;
                    *dsrp = physicalDeviceData->physical_device_depth_stencil_resolve_properties_;
                    dsrp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)) {
                    VkPhysicalDeviceDescriptorIndexingPropertiesEXT *dip = (VkPhysicalDeviceDescriptorIndexingPropertiesEXT *)place;
                    void *pNext = dip->pNext;
                    *dip = physicalDeviceData->physical_device_descriptor_indexing_properties_;
                    dip->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)) {
                    VkPhysicalDeviceDescriptorIndexingFeaturesEXT *dif = (VkPhysicalDeviceDescriptorIndexingFeaturesEXT *)place;
                    void *pNext = dif->pNext;
                    *dif = physicalDeviceData->physical_device_descriptor_indexing_features_;
                    dif->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME)) {
                    VkPhysicalDeviceHostQueryResetFeaturesEXT *hqrf = (VkPhysicalDeviceHostQueryResetFeaturesEXT *)place;
                    void *pNext = hqrf->pNext;
                    *hqrf = physicalDeviceData->physical_device_host_query_reset_features_;
                    hqrf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME)) {
                    VkPhysicalDeviceImagelessFramebufferFeaturesKHR *iff = (VkPhysicalDeviceImagelessFramebufferFeaturesKHR *)place;
                    void *pNext = iff->pNext;
                    *iff = physicalDeviceData->physical_device_imageless_framebuffer_features_;
                    iff->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_MAINTENANCE3_EXTENSION_NAME)) {
                    VkPhysicalDeviceMaintenance3PropertiesKHR *pcp = (VkPhysicalDeviceMaintenance3PropertiesKHR *)place;
                    void *pNext = pcp->pNext;
                    *pcp = physicalDeviceData->physical_device_maintenance_3_properties_;
                    pcp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_MAINTENANCE_4_EXTENSION_NAME)) {
                    VkPhysicalDeviceMaintenance4FeaturesKHR *m4f = (VkPhysicalDeviceMaintenance4FeaturesKHR *)place;
                    void *pNext = m4f->pNext;
                    *m4f = physicalDeviceData->physical_device_maintenance_4_features_;
                    m4f->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_MAINTENANCE_4_EXTENSION_NAME)) {
                    VkPhysicalDeviceMaintenance4PropertiesKHR *m4p = (VkPhysicalDeviceMaintenance4PropertiesKHR *)place;
                    void *pNext = m4p->pNext;
                    *m4p = physicalDeviceData->physical_device_maintenance_4_properties_;
                    m4p->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_MULTIVIEW_EXTENSION_NAME)) {
                    VkPhysicalDeviceMultiviewPropertiesKHR *mp = (VkPhysicalDeviceMultiviewPropertiesKHR *)place;
                    void *pNext = mp->pNext;
                    *mp = physicalDeviceData->physical_device_multiview_properties_;
                    mp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_MULTIVIEW_EXTENSION_NAME)) {
                    VkPhysicalDeviceMultiviewFeaturesKHR *mf = (VkPhysicalDeviceMultiviewFeaturesKHR *)place;
                    void *pNext = mf->pNext;
                    *mf = physicalDeviceData->physical_device_multiview_features_;
                    mf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_FILTER_MINMAX_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME)) {
                    VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT *sfmp =
                        (VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT *)place;
                    void *pNext = sfmp->pNext;
                    *sfmp = physicalDeviceData->physical_device_sampler_filter_minmax_properties_;
                    sfmp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME)) {
                    VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR *sycf =
                        (VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR *)place;
                    void *pNext = sycf->pNext;
                    *sycf = physicalDeviceData->physical_device_sampler_ycbcr_conversion_features_;
                    sycf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME)) {
                    VkPhysicalDeviceScalarBlockLayoutFeaturesEXT *sblf = (VkPhysicalDeviceScalarBlockLayoutFeaturesEXT *)place;
                    void *pNext = sblf->pNext;
                    *sblf = physicalDeviceData->physical_device_scalar_block_layout_features_;
                    sblf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_KHR_SEPARATE_DEPTH_STENCIL_LAYOUTS_EXTENSION_NAME)) {
                    VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR *sdslf =
                        (VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR *)place;
                    void *pNext = sdslf->pNext;
                    *sdslf = physicalDeviceData->physical_device_separate_depth_stencil_layouts_features_;
                    sdslf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_SHADER_ATOMIC_INT64_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderAtomicInt64FeaturesKHR *saisf = (VkPhysicalDeviceShaderAtomicInt64FeaturesKHR *)place;
                    void *pNext = saisf->pNext;
                    *saisf = physicalDeviceData->physical_device_shader_atomic_int64_features_;
                    saisf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT_CONTROLS_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME)) {
                    VkPhysicalDeviceFloatControlsPropertiesKHR *fcp = (VkPhysicalDeviceFloatControlsPropertiesKHR *)place;
                    void *pNext = fcp->pNext;
                    *fcp = physicalDeviceData->physical_device_float_controls_properties_;
                    fcp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderFloat16Int8FeaturesKHR *sfsief = (VkPhysicalDeviceShaderFloat16Int8FeaturesKHR *)place;
                    void *pNext = sfsief->pNext;
                    *sfsief = physicalDeviceData->physical_device_shader_float16_int8_features_;
                    sfsief->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_KHR_SHADER_SUBGROUP_EXTENDED_TYPES_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR *ssetf =
                        (VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR *)place;
                    void *pNext = ssetf->pNext;
                    *ssetf = physicalDeviceData->physical_device_shader_subgroup_extended_types_features_;
                    ssetf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME)) {
                    VkPhysicalDeviceTimelineSemaphorePropertiesKHR *tsp = (VkPhysicalDeviceTimelineSemaphorePropertiesKHR *)place;
                    void *pNext = tsp->pNext;
                    *tsp = physicalDeviceData->physical_device_timeline_semaphore_properties_;
                    tsp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME)) {
                    VkPhysicalDeviceTimelineSemaphoreFeaturesKHR *tsf = (VkPhysicalDeviceTimelineSemaphoreFeaturesKHR *)place;
                    void *pNext = tsf->pNext;
                    *tsf = physicalDeviceData->physical_device_timeline_semaphore_features_;
                    tsf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT_EXTENSION_NAME)) {
                    VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR *ubslf =
                        (VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR *)place;
                    void *pNext = ubslf->pNext;
                    *ubslf = physicalDeviceData->physical_device_uniform_buffer_standard_layout_features_;
                    ubslf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME)) {
                    VkPhysicalDeviceVariablePointersFeaturesKHR *vpf = (VkPhysicalDeviceVariablePointersFeaturesKHR *)place;
                    void *pNext = vpf->pNext;
                    *vpf = physicalDeviceData->physical_device_variable_pointers_features_;
                    vpf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_VULKAN_MEMORY_MODEL_EXTENSION_NAME)) {
                    VkPhysicalDeviceVulkanMemoryModelFeaturesKHR *vmmf = (VkPhysicalDeviceVulkanMemoryModelFeaturesKHR *)place;
                    void *pNext = vmmf->pNext;
                    *vmmf = physicalDeviceData->physical_device_vulkan_memory_model_features_;
                    vmmf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_WORKGROUP_MEMORY_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_KHR_ZERO_INITIALIZE_WORKGROUP_MEMORY_EXTENSION_NAME)) {
                    VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR *ziwmf =
                        (VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR *)place;
                    void *pNext = ziwmf->pNext;
                    *ziwmf = physicalDeviceData->physical_device_zero_initialize_workgroup_memory_features_;
                    ziwmf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME)) {
                    VkPhysicalDeviceAccelerationStructureFeaturesKHR *asf =
                        (VkPhysicalDeviceAccelerationStructureFeaturesKHR *)place;
                    void *pNext = asf->pNext;
                    *asf = physicalDeviceData->physical_device_acceleration_structure_features_;
                    asf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME)) {
                    VkPhysicalDeviceAccelerationStructurePropertiesKHR *asp =
                        (VkPhysicalDeviceAccelerationStructurePropertiesKHR *)place;
                    void *pNext = asp->pNext;
                    *asp = physicalDeviceData->physical_device_acceleration_structure_properties_;
                    asp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_PERFORMANCE_QUERY_EXTENSION_NAME)) {
                    VkPhysicalDevicePerformanceQueryFeaturesKHR *pqf = (VkPhysicalDevicePerformanceQueryFeaturesKHR *)place;
                    void *pNext = pqf->pNext;
                    *pqf = physicalDeviceData->physical_device_performance_query_features_;
                    pqf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_PERFORMANCE_QUERY_EXTENSION_NAME)) {
                    VkPhysicalDevicePerformanceQueryPropertiesKHR *pqp = (VkPhysicalDevicePerformanceQueryPropertiesKHR *)place;
                    void *pNext = pqp->pNext;
                    *pqp = physicalDeviceData->physical_device_performance_query_properties_;
                    pqp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_EXECUTABLE_PROPERTIES_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_KHR_PIPELINE_EXECUTABLE_PROPERTIES_EXTENSION_NAME)) {
                    VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR *pepf =
                        (VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR *)place;
                    void *pNext = pepf->pNext;
                    *pepf = physicalDeviceData->physical_device_pipeline_executable_properties_features_;
                    pepf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_ID_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_PRESENT_ID_EXTENSION_NAME)) {
                    VkPhysicalDevicePresentIdFeaturesKHR *pidf = (VkPhysicalDevicePresentIdFeaturesKHR *)place;
                    void *pNext = pidf->pNext;
                    *pidf = physicalDeviceData->physical_device_present_id_features_;
                    pidf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_WAIT_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_PRESENT_WAIT_EXTENSION_NAME)) {
                    VkPhysicalDevicePresentWaitFeaturesKHR *pwf = (VkPhysicalDevicePresentWaitFeaturesKHR *)place;
                    void *pNext = pwf->pNext;
                    *pwf = physicalDeviceData->physical_device_present_wait_features_;
                    pwf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME)) {
                    VkPhysicalDevicePushDescriptorPropertiesKHR *pdp = (VkPhysicalDevicePushDescriptorPropertiesKHR *)place;
                    void *pNext = pdp->pNext;
                    *pdp = physicalDeviceData->physical_device_push_descriptor_properites_;
                    pdp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_RAY_QUERY_EXTENSION_NAME)) {
                    VkPhysicalDeviceRayQueryFeaturesKHR *rqf = (VkPhysicalDeviceRayQueryFeaturesKHR *)place;
                    void *pNext = rqf->pNext;
                    *rqf = physicalDeviceData->physical_device_ray_query_features_;
                    rqf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME)) {
                    VkPhysicalDeviceRayTracingPipelineFeaturesKHR *rtpf = (VkPhysicalDeviceRayTracingPipelineFeaturesKHR *)place;
                    void *pNext = rtpf->pNext;
                    *rtpf = physicalDeviceData->physical_device_ray_tracing_pipeline_features_;
                    rtpf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME)) {
                    VkPhysicalDeviceRayTracingPipelinePropertiesKHR *rtpp =
                        (VkPhysicalDeviceRayTracingPipelinePropertiesKHR *)place;
                    void *pNext = rtpp->pNext;
                    *rtpp = physicalDeviceData->physical_device_ray_tracing_pipeline_properties_;
                    rtpp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_SHADER_CLOCK_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderClockFeaturesKHR *scf = (VkPhysicalDeviceShaderClockFeaturesKHR *)place;
                    void *pNext = scf->pNext;
                    *scf = physicalDeviceData->physical_device_shader_clock_features_;
                    scf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_KHR_SHADER_INTEGER_DOT_PRODUCT_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR *sidpf =
                        (VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR *)place;
                    void *pNext = sidpf->pNext;
                    *sidpf = physicalDeviceData->physical_device_shader_integer_dot_product_features_;
                    sidpf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_KHR_SHADER_INTEGER_DOT_PRODUCT_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR *sidpp =
                        (VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR *)place;
                    void *pNext = sidpp->pNext;
                    *sidpp = physicalDeviceData->physical_device_shader_integer_dot_products_properties_;
                    sidpp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_KHR_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR *ssucff =
                        (VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR *)place;
                    void *pNext = ssucff->pNext;
                    *ssucff = physicalDeviceData->physical_device_shader_subgroup_uniform_control_flow_features_;
                    ssucff->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_KHR_SHADER_TERMINATE_INVOCATION_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR *stif =
                        (VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR *)place;
                    void *pNext = stif->pNext;
                    *stif = physicalDeviceData->physical_device_shader_terminate_invocation_features_;
                    stif->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_KHR_SHADER_TERMINATE_INVOCATION_EXTENSION_NAME)) {
                    VkPhysicalDeviceSynchronization2FeaturesKHR *s2f = (VkPhysicalDeviceSynchronization2FeaturesKHR *)place;
                    void *pNext = s2f->pNext;
                    *s2f = physicalDeviceData->physical_device_synchronization2_features_;
                    s2f->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_KHR_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_EXTENSION_NAME)) {
                    VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR *wmelf =
                        (VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR *)place;
                    void *pNext = wmelf->pNext;
                    *wmelf = physicalDeviceData->physical_device_workgroup_memory_explicit_layout_features_;
                    wmelf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_4444_FORMATS_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_4444_FORMATS_EXTENSION_NAME)) {
                    VkPhysicalDevice4444FormatsFeaturesEXT *ff = (VkPhysicalDevice4444FormatsFeaturesEXT *)place;
                    void *pNext = ff->pNext;
                    *ff = physicalDeviceData->physical_device_4444_formats_features_;
                    ff->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ASTC_DECODE_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_ASTC_DECODE_MODE_EXTENSION_NAME)) {
                    VkPhysicalDeviceASTCDecodeFeaturesEXT *astcdf = (VkPhysicalDeviceASTCDecodeFeaturesEXT *)place;
                    void *pNext = astcdf->pNext;
                    *astcdf = physicalDeviceData->physical_device_astc_decode_features_;
                    astcdf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME)) {
                    VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT *boaf =
                        (VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT *)place;
                    void *pNext = boaf->pNext;
                    *boaf = physicalDeviceData->physical_device_blend_operation_advanced_features_;
                    boaf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME)) {
                    VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT *boap =
                        (VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT *)place;
                    void *pNext = boap->pNext;
                    *boap = physicalDeviceData->physical_device_blend_operation_advanced_properties_;
                    boap->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BORDER_COLOR_SWIZZLE_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_BORDER_COLOR_SWIZZLE_EXTENSION_NAME)) {
                    VkPhysicalDeviceBorderColorSwizzleFeaturesEXT *bcsf = (VkPhysicalDeviceBorderColorSwizzleFeaturesEXT *)place;
                    void *pNext = bcsf->pNext;
                    *bcsf = physicalDeviceData->physical_device_border_color_swizzle_features_;
                    bcsf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COLOR_WRITE_ENABLE_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_COLOR_WRITE_ENABLE_EXTENSION_NAME)) {
                    VkPhysicalDeviceColorWriteEnableFeaturesEXT *cwef = (VkPhysicalDeviceColorWriteEnableFeaturesEXT *)place;
                    void *pNext = cwef->pNext;
                    *cwef = physicalDeviceData->physical_device_color_write_enable_features_;
                    cwef->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONDITIONAL_RENDERING_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_CONDITIONAL_RENDERING_EXTENSION_NAME)) {
                    VkPhysicalDeviceConditionalRenderingFeaturesEXT *crf = (VkPhysicalDeviceConditionalRenderingFeaturesEXT *)place;
                    void *pNext = crf->pNext;
                    *crf = physicalDeviceData->physical_device_conditional_rendering_features_;
                    crf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONSERVATIVE_RASTERIZATION_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME)) {
                    VkPhysicalDeviceConservativeRasterizationPropertiesEXT *crp =
                        (VkPhysicalDeviceConservativeRasterizationPropertiesEXT *)place;
                    void *pNext = crp->pNext;
                    *crp = physicalDeviceData->physical_device_conservative_rasterization_properties_;
                    crp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_CUSTOM_BORDER_COLOR_EXTENSION_NAME)) {
                    VkPhysicalDeviceCustomBorderColorFeaturesEXT *cbcf = (VkPhysicalDeviceCustomBorderColorFeaturesEXT *)place;
                    void *pNext = cbcf->pNext;
                    *cbcf = physicalDeviceData->physical_device_custom_border_color_features_;
                    cbcf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_CUSTOM_BORDER_COLOR_EXTENSION_NAME)) {
                    VkPhysicalDeviceCustomBorderColorPropertiesEXT *cbcp = (VkPhysicalDeviceCustomBorderColorPropertiesEXT *)place;
                    void *pNext = cbcp->pNext;
                    *cbcp = physicalDeviceData->physical_device_custom_border_color_properties_;
                    cbcp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_DEPTH_CLIP_ENABLE_EXTENSION_NAME)) {
                    VkPhysicalDeviceDepthClipEnableFeaturesEXT *dcef = (VkPhysicalDeviceDepthClipEnableFeaturesEXT *)place;
                    void *pNext = dcef->pNext;
                    *dcef = physicalDeviceData->physical_device_depth_clip_enable_features_ext_;
                    dcef->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_MEMORY_REPORT_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_DEVICE_MEMORY_REPORT_EXTENSION_NAME)) {
                    VkPhysicalDeviceDeviceMemoryReportFeaturesEXT *dmrf = (VkPhysicalDeviceDeviceMemoryReportFeaturesEXT *)place;
                    void *pNext = dmrf->pNext;
                    *dmrf = physicalDeviceData->physical_device_device_memory_report_features_;
                    dmrf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DISCARD_RECTANGLE_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_DISCARD_RECTANGLES_EXTENSION_NAME)) {
                    VkPhysicalDeviceDiscardRectanglePropertiesEXT *drp = (VkPhysicalDeviceDiscardRectanglePropertiesEXT *)place;
                    void *pNext = drp->pNext;
                    *drp = physicalDeviceData->physical_device_discard_rectangle_properties_;
                    drp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME)) {
                    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT *edsf =
                        (VkPhysicalDeviceExtendedDynamicStateFeaturesEXT *)place;
                    void *pNext = edsf->pNext;
                    *edsf = physicalDeviceData->physical_device_extended_dynamic_state_features_;
                    edsf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME)) {
                    VkPhysicalDeviceExtendedDynamicState2FeaturesEXT *eds2f =
                        (VkPhysicalDeviceExtendedDynamicState2FeaturesEXT *)place;
                    void *pNext = eds2f->pNext;
                    *eds2f = physicalDeviceData->physical_device_extended_dynamic_state2_features_;
                    eds2f->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_HOST_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_EXTERNAL_MEMORY_HOST_EXTENSION_NAME)) {
                    VkPhysicalDeviceExternalMemoryHostPropertiesEXT *emhp =
                        (VkPhysicalDeviceExternalMemoryHostPropertiesEXT *)place;
                    void *pNext = emhp->pNext;
                    *emhp = physicalDeviceData->physical_device_external_memory_host_properties_;
                    emhp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_FRAGMENT_DENSITY_MAP_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentDensityMapFeaturesEXT *fdmf = (VkPhysicalDeviceFragmentDensityMapFeaturesEXT *)place;
                    void *pNext = fdmf->pNext;
                    *fdmf = physicalDeviceData->physical_device_fragment_density_map_features_;
                    fdmf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_FRAGMENT_DENSITY_MAP_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentDensityMapPropertiesEXT *fdmp =
                        (VkPhysicalDeviceFragmentDensityMapPropertiesEXT *)place;
                    void *pNext = fdmp->pNext;
                    *fdmp = physicalDeviceData->physical_device_fragment_density_map_properties_;
                    fdmp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT *fsif =
                        (VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT *)place;
                    void *pNext = fsif->pNext;
                    *fsif = physicalDeviceData->physical_device_fragment_shader_interlock_features_;
                    fsif->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GLOBAL_PRIORITY_QUERY_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_GLOBAL_PRIORITY_QUERY_EXTENSION_NAME)) {
                    VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT *gpqf = (VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT *)place;
                    void *pNext = gpqf->pNext;
                    *gpqf = physicalDeviceData->physical_device_global_priority_query_features_;
                    gpqf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_IMAGE_ROBUSTNESS_EXTENSION_NAME)) {
                    VkPhysicalDeviceImageRobustnessFeaturesEXT *irf = (VkPhysicalDeviceImageRobustnessFeaturesEXT *)place;
                    void *pNext = irf->pNext;
                    *irf = physicalDeviceData->physical_device_image_robustness_features_;
                    irf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_INDEX_TYPE_UINT8_EXTENSION_NAME)) {
                    VkPhysicalDeviceIndexTypeUint8FeaturesEXT *itu8f = (VkPhysicalDeviceIndexTypeUint8FeaturesEXT *)place;
                    void *pNext = itu8f->pNext;
                    *itu8f = physicalDeviceData->physical_device_index_type_uint8_features_;
                    itu8f->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME)) {
                    VkPhysicalDeviceInlineUniformBlockFeaturesEXT *iubf = (VkPhysicalDeviceInlineUniformBlockFeaturesEXT *)place;
                    void *pNext = iubf->pNext;
                    *iubf = physicalDeviceData->physical_device_inline_uniform_block_features_;
                    iubf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME)) {
                    VkPhysicalDeviceInlineUniformBlockPropertiesEXT *iubp =
                        (VkPhysicalDeviceInlineUniformBlockPropertiesEXT *)place;
                    void *pNext = iubp->pNext;
                    *iubp = physicalDeviceData->physical_device_inline_uniform_block_properties_;
                    iubp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME)) {
                    VkPhysicalDeviceLineRasterizationFeaturesEXT *lrf = (VkPhysicalDeviceLineRasterizationFeaturesEXT *)place;
                    void *pNext = lrf->pNext;
                    *lrf = physicalDeviceData->physical_device_line_rasterization_features_;
                    lrf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME)) {
                    VkPhysicalDeviceLineRasterizationPropertiesEXT *lrp = (VkPhysicalDeviceLineRasterizationPropertiesEXT *)place;
                    void *pNext = lrp->pNext;
                    *lrp = physicalDeviceData->physical_device_line_rasterization_properties_;
                    lrp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PRIORITY_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME)) {
                    VkPhysicalDeviceMemoryPriorityFeaturesEXT *mpf = (VkPhysicalDeviceMemoryPriorityFeaturesEXT *)place;
                    void *pNext = mpf->pNext;
                    *mpf = physicalDeviceData->physical_device_memory_priority_features_;
                    mpf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_MULTI_DRAW_EXTENSION_NAME)) {
                    VkPhysicalDeviceMultiDrawFeaturesEXT *mdf = (VkPhysicalDeviceMultiDrawFeaturesEXT *)place;
                    void *pNext = mdf->pNext;
                    *mdf = physicalDeviceData->physical_device_multi_draw_features_;
                    mdf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_MULTI_DRAW_EXTENSION_NAME)) {
                    VkPhysicalDeviceMultiDrawPropertiesEXT *mdp = (VkPhysicalDeviceMultiDrawPropertiesEXT *)place;
                    void *pNext = mdp->pNext;
                    *mdp = physicalDeviceData->physical_device_multi_draw_properties_;
                    mdp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PAGEABLE_DEVICE_LOCAL_MEMORY_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_EXT_PAGEABLE_DEVICE_LOCAL_MEMORY_EXTENSION_NAME)) {
                    VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT *pdlmf =
                        (VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT *)place;
                    void *pNext = pdlmf->pNext;
                    *pdlmf = physicalDeviceData->physical_device_pageable_device_local_memory_features_;
                    pdlmf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_EXT_PIPELINE_CREATION_CACHE_CONTROL_EXTENSION_NAME)) {
                    VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT *pcccf =
                        (VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT *)place;
                    void *pNext = pcccf->pNext;
                    *pcccf = physicalDeviceData->physical_device_pipeline_creation_cache_control_features_;
                    pcccf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVE_TOPOLOGY_LIST_RESTART_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_EXT_PRIMITIVE_TOPOLOGY_LIST_RESTART_EXTENSION_NAME)) {
                    VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT *ptlrf =
                        (VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT *)place;
                    void *pNext = ptlrf->pNext;
                    *ptlrf = physicalDeviceData->physical_device_primitive_topology_list_restart_features_;
                    ptlrf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_PRIVATE_DATA_EXTENSION_NAME)) {
                    VkPhysicalDevicePrivateDataFeaturesEXT *pdf = (VkPhysicalDevicePrivateDataFeaturesEXT *)place;
                    void *pNext = pdf->pNext;
                    *pdf = physicalDeviceData->physical_device_private_data_features_;
                    pdf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_PROVOKING_VERTEX_EXTENSION_NAME)) {
                    VkPhysicalDeviceProvokingVertexFeaturesEXT *pvf = (VkPhysicalDeviceProvokingVertexFeaturesEXT *)place;
                    void *pNext = pvf->pNext;
                    *pvf = physicalDeviceData->physical_device_provoking_vertex_features_;
                    pvf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_PROVOKING_VERTEX_EXTENSION_NAME)) {
                    VkPhysicalDeviceProvokingVertexPropertiesEXT *pvp = (VkPhysicalDeviceProvokingVertexPropertiesEXT *)place;
                    void *pNext = pvp->pNext;
                    *pvp = physicalDeviceData->physical_device_provoking_vertex_properties_;
                    pvp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RGBA10X6_FORMATS_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_RGBA10X6_FORMATS_EXTENSION_NAME)) {
                    VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT *rgba10x6ff = (VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT *)place;
                    void *pNext = rgba10x6ff->pNext;
                    *rgba10x6ff = physicalDeviceData->physical_device_rgba10x6_formats_features_;
                    rgba10x6ff->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_ROBUSTNESS_2_EXTENSION_NAME)) {
                    VkPhysicalDeviceRobustness2FeaturesEXT *r2f = (VkPhysicalDeviceRobustness2FeaturesEXT *)place;
                    void *pNext = r2f->pNext;
                    *r2f = physicalDeviceData->physical_device_robustness_2_features_;
                    r2f->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_ROBUSTNESS_2_EXTENSION_NAME)) {
                    VkPhysicalDeviceRobustness2PropertiesEXT *r2p = (VkPhysicalDeviceRobustness2PropertiesEXT *)place;
                    void *pNext = r2p->pNext;
                    *r2p = physicalDeviceData->physical_device_robustness_2_properties_;
                    r2p->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLE_LOCATIONS_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_SAMPLE_LOCATIONS_EXTENSION_NAME)) {
                    VkPhysicalDeviceSampleLocationsPropertiesEXT *slp = (VkPhysicalDeviceSampleLocationsPropertiesEXT *)place;
                    void *pNext = slp->pNext;
                    *slp = physicalDeviceData->physical_device_sample_locations_properties_;
                    slp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderAtomicFloatFeaturesEXT *saff = (VkPhysicalDeviceShaderAtomicFloatFeaturesEXT *)place;
                    void *pNext = saff->pNext;
                    *saff = physicalDeviceData->physical_device_shader_atomic_float_features_;
                    saff->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_2_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_SHADER_ATOMIC_FLOAT_2_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT *saf2f = (VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT *)place;
                    void *pNext = saf2f->pNext;
                    *saf2f = physicalDeviceData->physical_device_shader_atomic_float2_features_;
                    saf2f->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT *sdthif =
                        (VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT *)place;
                    void *pNext = sdthif->pNext;
                    *sdthif = physicalDeviceData->physical_device_shader_demote_to_helper_invocation_features_;
                    sdthif->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_ATOMIC_INT64_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_EXT_SHADER_IMAGE_ATOMIC_INT64_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT *siai64f =
                        (VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT *)place;
                    void *pNext = siai64f->pNext;
                    *siai64f = physicalDeviceData->physical_device_shader_image_atomic_int64_features_;
                    siai64f->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME)) {
                    VkPhysicalDeviceSubgroupSizeControlFeaturesEXT *sscf = (VkPhysicalDeviceSubgroupSizeControlFeaturesEXT *)place;
                    void *pNext = sscf->pNext;
                    *sscf = physicalDeviceData->physical_device_subgroup_size_control_features_;
                    sscf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME)) {
                    VkPhysicalDeviceSubgroupSizeControlPropertiesEXT *sscp =
                        (VkPhysicalDeviceSubgroupSizeControlPropertiesEXT *)place;
                    void *pNext = sscp->pNext;
                    *sscp = physicalDeviceData->physical_device_subgroup_size_control_properties_;
                    sscp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME)) {
                    VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT *tbaf =
                        (VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT *)place;
                    void *pNext = tbaf->pNext;
                    *tbaf = physicalDeviceData->physical_device_texel_buffer_alignment_features_;
                    tbaf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME)) {
                    VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT *tbap =
                        (VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT *)place;
                    void *pNext = tbap->pNext;
                    *tbap = physicalDeviceData->physical_device_texel_buffer_alignment_properties_;
                    tbap->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_HDR_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_EXT_TEXTURE_COMPRESSION_ASTC_HDR_EXTENSION_NAME)) {
                    VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT *tcastchdrf =
                        (VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT *)place;
                    void *pNext = tcastchdrf->pNext;
                    *tcastchdrf = physicalDeviceData->physical_device_texture_compression_astc_hdr_features_;
                    tcastchdrf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME)) {
                    VkPhysicalDeviceTransformFeedbackFeaturesEXT *tff = (VkPhysicalDeviceTransformFeedbackFeaturesEXT *)place;
                    void *pNext = tff->pNext;
                    *tff = physicalDeviceData->physical_device_transform_feedback_features_;
                    tff->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME)) {
                    VkPhysicalDeviceTransformFeedbackPropertiesEXT *tfp = (VkPhysicalDeviceTransformFeedbackPropertiesEXT *)place;
                    void *pNext = tfp->pNext;
                    *tfp = physicalDeviceData->physical_device_transform_feedback_properties_;
                    tfp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME)) {
                    VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT *vadf =
                        (VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT *)place;
                    void *pNext = vadf->pNext;
                    *vadf = physicalDeviceData->physical_device_vertex_attribute_divisor_features_;
                    vadf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME)) {
                    VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT *vadp =
                        (VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT *)place;
                    void *pNext = vadp->pNext;
                    *vadp = physicalDeviceData->physical_device_vertex_attirbute_divisor_properties_;
                    vadp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_INPUT_DYNAMIC_STATE_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME)) {
                    VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT *vidsf =
                        (VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT *)place;
                    void *pNext = vidsf->pNext;
                    *vidsf = physicalDeviceData->physical_device_vertex_input_dynamic_state_features_;
                    vidsf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_2_PLANE_444_FORMATS_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_YCBCR_2PLANE_444_FORMATS_EXTENSION_NAME)) {
                    VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT *y2pff =
                        (VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT *)place;
                    void *pNext = y2pff->pNext;
                    *y2pff = physicalDeviceData->physical_device_ycbcr_2plane_444_formats_features_;
                    y2pff->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_IMAGE_ARRAYS_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_YCBCR_IMAGE_ARRAYS_EXTENSION_NAME)) {
                    VkPhysicalDeviceYcbcrImageArraysFeaturesEXT *yiaf = (VkPhysicalDeviceYcbcrImageArraysFeaturesEXT *)place;
                    void *pNext = yiaf->pNext;
                    *yiaf = physicalDeviceData->physical_device_ycbcr_image_arrays_features_;
                    yiaf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentShadingRateFeaturesKHR *fsrf = (VkPhysicalDeviceFragmentShadingRateFeaturesKHR *)place;
                    void *pNext = fsrf->pNext;
                    *fsrf = physicalDeviceData->physical_device_fragment_shading_rate_features_;
                    fsrf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentShadingRatePropertiesKHR *fsrp =
                        (VkPhysicalDeviceFragmentShadingRatePropertiesKHR *)place;
                    void *pNext = fsrp->pNext;
                    *fsrp = physicalDeviceData->physical_device_fragment_shading_rate_properties_;
                    fsrp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COHERENT_MEMORY_FEATURES_AMD:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME)) {
                    VkPhysicalDeviceCoherentMemoryFeaturesAMD *cmf = (VkPhysicalDeviceCoherentMemoryFeaturesAMD *)place;
                    void *pNext = cmf->pNext;
                    *cmf = physicalDeviceData->physical_device_coherent_memory_features_;
                    cmf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_AMD:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_AMD_SHADER_CORE_PROPERTIES_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderCorePropertiesAMD *scp = (VkPhysicalDeviceShaderCorePropertiesAMD *)place;
                    void *pNext = scp->pNext;
                    *scp = physicalDeviceData->physical_device_shader_core_properties_;
                    scp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_2_AMD:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_AMD_SHADER_CORE_PROPERTIES_2_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderCoreProperties2AMD *scp2 = (VkPhysicalDeviceShaderCoreProperties2AMD *)place;
                    void *pNext = scp2->pNext;
                    *scp2 = physicalDeviceData->physical_device_shader_core_properties_2_;
                    scp2->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INVOCATION_MASK_FEATURES_HUAWEI:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_HUAWEI_INVOCATION_MASK_EXTENSION_NAME)) {
                    VkPhysicalDeviceInvocationMaskFeaturesHUAWEI *imf = (VkPhysicalDeviceInvocationMaskFeaturesHUAWEI *)place;
                    void *pNext = imf->pNext;
                    *imf = physicalDeviceData->physical_device_invocation_mask_features_;
                    imf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_SHADING_FEATURES_HUAWEI:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_HUAWEI_INVOCATION_MASK_EXTENSION_NAME)) {
                    VkPhysicalDeviceSubpassShadingFeaturesHUAWEI *shf = (VkPhysicalDeviceSubpassShadingFeaturesHUAWEI *)place;
                    void *pNext = shf->pNext;
                    *shf = physicalDeviceData->physical_device_subpass_shading_features_;
                    shf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_SHADING_PROPERTIES_HUAWEI:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_HUAWEI_INVOCATION_MASK_EXTENSION_NAME)) {
                    VkPhysicalDeviceSubpassShadingPropertiesHUAWEI *shp = (VkPhysicalDeviceSubpassShadingPropertiesHUAWEI *)place;
                    void *pNext = shp->pNext;
                    *shp = physicalDeviceData->physical_device_subpass_shading_properties_;
                    shp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_FUNCTIONS_2_FEATURES_INTEL:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_INTEL_SHADER_INTEGER_FUNCTIONS_2_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL *sif2f =
                        (VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL *)place;
                    void *pNext = sif2f->pNext;
                    *sif2f = physicalDeviceData->physical_device_shader_integer_functions_2_features_;
                    sif2f->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMPUTE_SHADER_DERIVATIVES_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_NV_COMPUTE_SHADER_DERIVATIVES_EXTENSION_NAME)) {
                    VkPhysicalDeviceComputeShaderDerivativesFeaturesNV *csdf =
                        (VkPhysicalDeviceComputeShaderDerivativesFeaturesNV *)place;
                    void *pNext = csdf->pNext;
                    *csdf = physicalDeviceData->physical_device_compute_shader_derivatives_features_;
                    csdf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_COOPERATIVE_MATRIX_EXTENSION_NAME)) {
                    VkPhysicalDeviceCooperativeMatrixFeaturesNV *cmf = (VkPhysicalDeviceCooperativeMatrixFeaturesNV *)place;
                    void *pNext = cmf->pNext;
                    *cmf = physicalDeviceData->physical_device_cooperative_matrix_features_;
                    cmf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_PROPERTIES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_COOPERATIVE_MATRIX_EXTENSION_NAME)) {
                    VkPhysicalDeviceCooperativeMatrixPropertiesNV *cmp = (VkPhysicalDeviceCooperativeMatrixPropertiesNV *)place;
                    void *pNext = cmp->pNext;
                    *cmp = physicalDeviceData->physical_device_cooperative_matrix_properties_;
                    cmp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CORNER_SAMPLED_IMAGE_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_CORNER_SAMPLED_IMAGE_EXTENSION_NAME)) {
                    VkPhysicalDeviceCornerSampledImageFeaturesNV *csif = (VkPhysicalDeviceCornerSampledImageFeaturesNV *)place;
                    void *pNext = csif->pNext;
                    *csif = physicalDeviceData->physical_device_corner_sampled_image_features_;
                    csif->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COVERAGE_REDUCTION_MODE_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_COVERAGE_REDUCTION_MODE_EXTENSION_NAME)) {
                    VkPhysicalDeviceCoverageReductionModeFeaturesNV *crmf =
                        (VkPhysicalDeviceCoverageReductionModeFeaturesNV *)place;
                    void *pNext = crmf->pNext;
                    *crmf = physicalDeviceData->physical_device_coverage_reduction_mode_features_;
                    crmf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEDICATED_ALLOCATION_IMAGE_ALIASING_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_NV_DEDICATED_ALLOCATION_IMAGE_ALIASING_EXTENSION_NAME)) {
                    VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV *daiaf =
                        (VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV *)place;
                    void *pNext = daiaf->pNext;
                    *daiaf = physicalDeviceData->physical_device_dedicated_allocation_image_aliasing_features_;
                    daiaf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DIAGNOSTICS_CONFIG_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_DEVICE_DIAGNOSTICS_CONFIG_EXTENSION_NAME)) {
                    VkPhysicalDeviceDiagnosticsConfigFeaturesNV *dcf = (VkPhysicalDeviceDiagnosticsConfigFeaturesNV *)place;
                    void *pNext = dcf->pNext;
                    *dcf = physicalDeviceData->physical_device_diagnostics_config_features_;
                    dcf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_DEVICE_GENERATED_COMMANDS_EXTENSION_NAME)) {
                    VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV *dgcf =
                        (VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV *)place;
                    void *pNext = dgcf->pNext;
                    *dgcf = physicalDeviceData->physical_device_device_generated_commands_features_;
                    dgcf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_PROPERTIES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_DEVICE_GENERATED_COMMANDS_EXTENSION_NAME)) {
                    VkPhysicalDeviceDeviceGeneratedCommandsPropertiesNV *dgcp =
                        (VkPhysicalDeviceDeviceGeneratedCommandsPropertiesNV *)place;
                    void *pNext = dgcp->pNext;
                    *dgcp = physicalDeviceData->physical_device_device_generated_commands_properties_;
                    dgcp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_RDMA_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_EXTERNAL_MEMORY_RDMA_EXTENSION_NAME)) {
                    VkPhysicalDeviceExternalMemoryRDMAFeaturesNV *emrf = (VkPhysicalDeviceExternalMemoryRDMAFeaturesNV *)place;
                    void *pNext = emrf->pNext;
                    *emrf = physicalDeviceData->physical_device_external_memory_rdma_features_;
                    emrf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_BARYCENTRIC_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_NV_FRAGMENT_SHADER_BARYCENTRIC_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV *fsbf =
                        (VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV *)place;
                    void *pNext = fsbf->pNext;
                    *fsbf = physicalDeviceData->physical_device_fragment_shader_barycentric_features_;
                    fsbf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_ENUMS_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_NV_FRAGMENT_SHADING_RATE_ENUMS_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV *fsref =
                        (VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV *)place;
                    void *pNext = fsref->pNext;
                    *fsref = physicalDeviceData->physical_device_fragment_shading_rate_enums_features_;
                    fsref->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_ENUMS_PROPERTIES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_NV_FRAGMENT_SHADING_RATE_ENUMS_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentShadingRateEnumsPropertiesNV *fsrep =
                        (VkPhysicalDeviceFragmentShadingRateEnumsPropertiesNV *)place;
                    void *pNext = fsrep->pNext;
                    *fsrep = physicalDeviceData->physical_device_fragment_shading_rate_enums_properties_;
                    fsrep->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INHERITED_VIEWPORT_SCISSOR_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_NV_INHERITED_VIEWPORT_SCISSOR_EXTENSION_NAME)) {
                    VkPhysicalDeviceInheritedViewportScissorFeaturesNV *ivsf =
                        (VkPhysicalDeviceInheritedViewportScissorFeaturesNV *)place;
                    void *pNext = ivsf->pNext;
                    *ivsf = physicalDeviceData->physical_device_inherited_viewport_scissor_features_;
                    ivsf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_MESH_SHADER_EXTENSION_NAME)) {
                    VkPhysicalDeviceMeshShaderFeaturesNV *msf = (VkPhysicalDeviceMeshShaderFeaturesNV *)place;
                    void *pNext = msf->pNext;
                    *msf = physicalDeviceData->physical_device_mesh_shader_features_;
                    msf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_MESH_SHADER_EXTENSION_NAME)) {
                    VkPhysicalDeviceMeshShaderPropertiesNV *msp = (VkPhysicalDeviceMeshShaderPropertiesNV *)place;
                    void *pNext = msp->pNext;
                    *msp = physicalDeviceData->physical_device_mesh_shader_properties_;
                    msp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_RAY_TRACING_EXTENSION_NAME)) {
                    VkPhysicalDeviceRayTracingPropertiesNV *rtp = (VkPhysicalDeviceRayTracingPropertiesNV *)place;
                    void *pNext = rtp->pNext;
                    *rtp = physicalDeviceData->physical_device_ray_tracing_properties_;
                    rtp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_MOTION_BLUR_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_RAY_TRACING_MOTION_BLUR_EXTENSION_NAME)) {
                    VkPhysicalDeviceRayTracingMotionBlurFeaturesNV *rtmbf = (VkPhysicalDeviceRayTracingMotionBlurFeaturesNV *)place;
                    void *pNext = rtmbf->pNext;
                    *rtmbf = physicalDeviceData->physical_device_ray_tracing_motiuon_blur_features_;
                    rtmbf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_REPRESENTATIVE_FRAGMENT_TEST_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_NV_REPRESENTATIVE_FRAGMENT_TEST_EXTENSION_NAME)) {
                    VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV *rftf =
                        (VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV *)place;
                    void *pNext = rftf->pNext;
                    *rftf = physicalDeviceData->physical_device_representative_fragment_test_features_;
                    rftf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXCLUSIVE_SCISSOR_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_SCISSOR_EXCLUSIVE_EXTENSION_NAME)) {
                    VkPhysicalDeviceExclusiveScissorFeaturesNV *esf = (VkPhysicalDeviceExclusiveScissorFeaturesNV *)place;
                    void *pNext = esf->pNext;
                    *esf = physicalDeviceData->physical_device_exclusive_scissor_features_;
                    esf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_FOOTPRINT_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_SHADER_IMAGE_FOOTPRINT_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderImageFootprintFeaturesNV *siff = (VkPhysicalDeviceShaderImageFootprintFeaturesNV *)place;
                    void *pNext = siff->pNext;
                    *siff = physicalDeviceData->physical_device_shader_image_footprint_features_;
                    siff->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_SHADER_SM_BUILTINS_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderSMBuiltinsFeaturesNV *ssmbf = (VkPhysicalDeviceShaderSMBuiltinsFeaturesNV *)place;
                    void *pNext = ssmbf->pNext;
                    *ssmbf = physicalDeviceData->physical_device_shader_sm_builtins_features_;
                    ssmbf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_PROPERTIES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_SHADER_SM_BUILTINS_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderSMBuiltinsPropertiesNV *ssmbp = (VkPhysicalDeviceShaderSMBuiltinsPropertiesNV *)place;
                    void *pNext = ssmbp->pNext;
                    *ssmbp = physicalDeviceData->physical_device_shader_sm_builtins_properties_;
                    ssmbp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_SHADING_RATE_IMAGE_EXTENSION_NAME)) {
                    VkPhysicalDeviceShadingRateImageFeaturesNV *srif = (VkPhysicalDeviceShadingRateImageFeaturesNV *)place;
                    void *pNext = srif->pNext;
                    *srif = physicalDeviceData->physical_device_shading_rate_image_features_;
                    srif->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_PROPERTIES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_SHADING_RATE_IMAGE_EXTENSION_NAME)) {
                    VkPhysicalDeviceShadingRateImagePropertiesNV *srip = (VkPhysicalDeviceShadingRateImagePropertiesNV *)place;
                    void *pNext = srip->pNext;
                    *srip = physicalDeviceData->physical_device_shading_rate_image_properties_;
                    srip->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MUTABLE_DESCRIPTOR_TYPE_FEATURES_VALVE:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_VALVE_MUTABLE_DESCRIPTOR_TYPE_EXTENSION_NAME)) {
                    VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE *mdtf =
                        (VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE *)place;
                    void *pNext = mdtf->pNext;
                    *mdtf = physicalDeviceData->physical_device_mutable_descriptor_type_features_;
                    mdtf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME)) {
                    VkPhysicalDeviceDynamicRenderingFeaturesKHR *drf = (VkPhysicalDeviceDynamicRenderingFeaturesKHR *)place;
                    void *pNext = drf->pNext;
                    *drf = physicalDeviceData->physical_device_dynamic_rendering_features_;
                    drf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_VIEW_MIN_LOD_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_IMAGE_VIEW_MIN_LOD_EXTENSION_NAME)) {
                    VkPhysicalDeviceImageViewMinLodFeaturesEXT *ivmlf = (VkPhysicalDeviceImageViewMinLodFeaturesEXT *)place;
                    void *pNext = ivmlf->pNext;
                    *ivmlf = physicalDeviceData->physical_device_image_view_min_lod_features_;
                    ivmlf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_FRAGMENT_DENSITY_MAP_2_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentDensityMap2FeaturesEXT *fdm2f = (VkPhysicalDeviceFragmentDensityMap2FeaturesEXT *)place;
                    void *pNext = fdm2f->pNext;
                    *fdm2f = physicalDeviceData->physical_device_fragment_density_map_2_features_;
                    fdm2f->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_OFFSET_FEATURES_QCOM:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_QCOM_FRAGMENT_DENSITY_MAP_OFFSET_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM *fdmof =
                        (VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM *)place;
                    void *pNext = fdmof->pNext;
                    *fdmof = physicalDeviceData->physical_device_fragment_density_map_offset_features_;
                    fdmof->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_OFFSET_PROPERTIES_QCOM:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_QCOM_FRAGMENT_DENSITY_MAP_OFFSET_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentDensityMapOffsetPropertiesQCOM *fdmop =
                        (VkPhysicalDeviceFragmentDensityMapOffsetPropertiesQCOM *)place;
                    void *pNext = fdmop->pNext;
                    *fdmop = physicalDeviceData->physical_device_fragment_density_map_offset_properties_;
                    fdmop->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_CONTROL_FEATURES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_DEPTH_CLIP_CONTROL_EXTENSION_NAME)) {
                    VkPhysicalDeviceDepthClipControlFeaturesEXT *dccf = (VkPhysicalDeviceDepthClipControlFeaturesEXT *)place;
                    void *pNext = dccf->pNext;
                    *dccf = physicalDeviceData->physical_device_depth_clip_control_features_;
                    dccf->pNext = pNext;
                }
                break;
            // Todo: needs spec fix on VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM::pNext
            /*case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RASTERIZATION_ORDER_ATTACHMENT_ACCESS_FEATURES_ARM:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData,
                                                              VK_ARM_RASTERIZATION_ORDER_ATTACHMENT_ACCESS_EXTENSION_NAME)) {
                    VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM *roaaf =
                        (VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM *)place;
                    void *pNext = roaaf->pNext;
                    *roaaf = physicalDeviceData->physical_device_rasterization_order_attachment_access_features_;
                    roaaf->pNext = pNext;
                }
                break;*/
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINEAR_COLOR_ATTACHMENT_FEATURES_NV:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_NV_LINEAR_COLOR_ATTACHMENT_EXTENSION_NAME)) {
                    VkPhysicalDeviceLinearColorAttachmentFeaturesNV *lcaf =
                        (VkPhysicalDeviceLinearColorAttachmentFeaturesNV *)place;
                    void *pNext = lcaf->pNext;
                    *lcaf = physicalDeviceData->physical_device_linear_color_attachment_features_;
                    lcaf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasSimulatedExtension(physicalDeviceData, VK_EXT_FRAGMENT_DENSITY_MAP_2_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentDensityMap2PropertiesEXT *fdm2p =
                        (VkPhysicalDeviceFragmentDensityMap2PropertiesEXT *)place;
                    void *pNext = fdm2p->pNext;
                    *fdm2p = physicalDeviceData->physical_device_fragment_density_map_2_properties_;
                    fdm2p->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_PROPERTIES:
                if (physicalDeviceData->physical_device_properties_.apiVersion >= VK_API_VERSION_1_1) {
                    VkPhysicalDeviceProtectedMemoryProperties *pmp = (VkPhysicalDeviceProtectedMemoryProperties *)place;
                    void *pNext = pmp->pNext;
                    *pmp = physicalDeviceData->physical_device_protected_memory_properties_;
                    pmp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES:
                if (physicalDeviceData->physical_device_properties_.apiVersion >= VK_API_VERSION_1_1) {
                    VkPhysicalDeviceProtectedMemoryFeatures *pmf = (VkPhysicalDeviceProtectedMemoryFeatures *)place;
                    void *pNext = pmf->pNext;
                    *pmf = physicalDeviceData->physical_device_protected_memory_features_;
                    pmf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES:
                if (physicalDeviceData->physical_device_properties_.apiVersion >= VK_API_VERSION_1_1) {
                    VkPhysicalDeviceShaderDrawParametersFeatures *sdpf = (VkPhysicalDeviceShaderDrawParametersFeatures *)place;
                    void *pNext = sdpf->pNext;
                    *sdpf = physicalDeviceData->physical_device_shader_draw_parameters_features_;
                    sdpf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES:
                if (physicalDeviceData->physical_device_properties_.apiVersion >= VK_API_VERSION_1_1) {
                    VkPhysicalDeviceSubgroupProperties *sp = (VkPhysicalDeviceSubgroupProperties *)place;
                    void *pNext = sp->pNext;
                    *sp = physicalDeviceData->physical_device_subgroup_properties_;
                    sp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES:
                if (physicalDeviceData->physical_device_properties_.apiVersion >= VK_API_VERSION_1_2) {
                    VkPhysicalDeviceVulkan11Properties *v11p = (VkPhysicalDeviceVulkan11Properties *)place;
                    void *pNext = v11p->pNext;
                    *v11p = physicalDeviceData->physical_device_vulkan_1_1_properties_;
                    v11p->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES:
                if (physicalDeviceData->physical_device_properties_.apiVersion >= VK_API_VERSION_1_2) {
                    VkPhysicalDeviceVulkan11Features *v11f = (VkPhysicalDeviceVulkan11Features *)place;
                    void *pNext = v11f->pNext;
                    *v11f = physicalDeviceData->physical_device_vulkan_1_1_features_;
                    v11f->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES:
                if (physicalDeviceData->physical_device_properties_.apiVersion >= VK_API_VERSION_1_2) {
                    VkPhysicalDeviceVulkan12Properties *v12p = (VkPhysicalDeviceVulkan12Properties *)place;
                    void *pNext = v12p->pNext;
                    *v12p = physicalDeviceData->physical_device_vulkan_1_2_properties_;
                    v12p->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES:
                if (physicalDeviceData->physical_device_properties_.apiVersion >= VK_API_VERSION_1_2) {
                    VkPhysicalDeviceVulkan12Features *v12f = (VkPhysicalDeviceVulkan12Features *)place;
                    void *pNext = v12f->pNext;
                    *v12f = physicalDeviceData->physical_device_vulkan_1_2_features_;
                    v12f->pNext = pNext;
                }
                break;
            default:
                break;
        }

        place = structure->pNext;
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
    DebugPrintf("vkEnumerateInstanceLayerProperties %s n", (pProperties ? "VALUES" : "COUNT"));
    return EnumerateProperties(kLayerPropertiesCount, kLayerProperties, pCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL EnumerateInstanceExtensionProperties(const char *pLayerName, uint32_t *pCount,
                                                                    VkExtensionProperties *pProperties) {
    DebugPrintf("vkEnumerateInstanceExtensionProperties \"%s\" %s n", (pLayerName ? pLayerName : ""),
                (pProperties ? "VALUES" : "COUNT"));
    if (pLayerName && !strcmp(pLayerName, kOurLayerName)) {
        return EnumerateProperties(kInstanceExtensionPropertiesCount, kInstanceExtensionProperties.data(), pCount, pProperties);
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
    const uint32_t src_count = (pdd) ? static_cast<uint32_t>(pdd->simulation_extensions_.size()) : 0;
    if (pLayerName) {
        if (strcmp(pLayerName, kOurLayerName) == 0)
            result = EnumerateProperties(kDeviceExtensionPropertiesCount, kDeviceExtensionProperties.data(), pCount, pProperties);
        else
            result = dt->EnumerateDeviceExtensionProperties(physicalDevice, pLayerName, pCount, pProperties);
    } else if (src_count == 0 || (pdd->extension_list_combination_mode_ == SET_CHECK_SUPPORT ||
                                  pdd->extension_list_combination_mode_ == SET_FROM_DEVICE)) {
        result = dt->EnumerateDeviceExtensionProperties(physicalDevice, pLayerName, pCount, pProperties);
    } else {
        result = EnumerateProperties(src_count, pdd->simulation_extensions_.data(), pCount, pProperties);
    }

    if (result == VK_SUCCESS && !pLayerName && emulatePortability.num > 0 &&
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

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format,
                                                             VkFormatProperties *pFormatProperties) {
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

        switch (pdd->format_list_combination_mode_) {
            case SET_CHECK_SUPPORT:
            case SET_FROM_DEVICE:
                *pFormatProperties = device_format;
                break;
            case SET_FROM_PROFILE:
            case SET_FROM_PROFILE_OVERRIDE:
                *pFormatProperties = (iter != pdd->arrayof_format_properties_.end()) ? iter->second : VkFormatProperties{};
                break;
                break;
            default:
                *pFormatProperties = device_format;
        }

        if (IsFormatSupported(*pFormatProperties) && iter != pdd->arrayof_format_properties_.end()) {
            switch (pdd->format_properties_combination_mode_) {
                case SET_CHECK_SUPPORT:
                case SET_FROM_DEVICE:
                    *pFormatProperties = device_format;
                    break;
                case SET_FROM_PROFILE:
                case SET_FROM_PROFILE_OVERRIDE:
                    *pFormatProperties = iter->second;
                    break;
                default:
                    *pFormatProperties = device_format;
            }
            if ((*pFormatProperties).linearTilingFeatures > device_format.linearTilingFeatures ||
                (*pFormatProperties).optimalTilingFeatures > device_format.optimalTilingFeatures ||
                (*pFormatProperties).bufferFeatures > device_format.bufferFeatures)
                DebugPrintf("WARN format %s may be simulating unsupported features!\n", vkFormatToString(format).c_str());
        }
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFormatProperties2(VkPhysicalDevice physicalDevice, VkFormat format,
                                                              VkFormatProperties2KHR *pFormatProperties) {
    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(physicalDevice);
    dt->GetPhysicalDeviceFormatProperties2(physicalDevice, format, pFormatProperties);
    GetPhysicalDeviceFormatProperties(physicalDevice, format, &pFormatProperties->formatProperties);
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

    // Are there JSON overrides, or should we call down to return the original values?
    PhysicalDeviceData *pdd = PhysicalDeviceData::Find(physicalDevice);
    if (pdd->format_list_combination_mode_ ==
        (pdd->extension_list_combination_mode_ == SET_CHECK_SUPPORT || pdd->extension_list_combination_mode_ == SET_FROM_DEVICE)) {
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
    std::stringstream version_stream;
    version_stream << kVersionProfilesMajor << "." << kVersionProfilesMinor << "." << kVersionProfilesPatch;
    std::string version_string(version_stream.str());

    static VkPhysicalDeviceToolPropertiesEXT profiles_layer_tool_props = {};
    profiles_layer_tool_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TOOL_PROPERTIES_EXT;
    profiles_layer_tool_props.pNext = nullptr;
    strcpy(profiles_layer_tool_props.name, "Profiles Layer");
    strcpy(profiles_layer_tool_props.version, version_string.c_str());
    profiles_layer_tool_props.purposes = VK_TOOL_PURPOSE_MODIFYING_FEATURES_BIT_EXT;
    strcpy(profiles_layer_tool_props.description, "Khronos profiles layer");
    strcpy(profiles_layer_tool_props.layer, "VK_LAYER_KHRONOS_profiles");

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

#define TRANSFER_VALUE(name) dest->name = src->name
#define TRANSFER_ARRAY(name) std::copy(std::begin(src->name), std::end(src->name), std::begin(dest->name));
#define TRANSFER_CHAR_ARRAY(name, size) strncpy(dest->name, src->name, size);

// VK_VULKAN_1_1

// Properties
void TransferValue(VkPhysicalDeviceVulkan11Properties *dest, VkPhysicalDevicePointClippingPropertiesKHR *src) {
    TRANSFER_VALUE(pointClippingBehavior);
}

void TransferValue(VkPhysicalDeviceVulkan11Properties *dest, VkPhysicalDeviceMultiviewPropertiesKHR *src) {
    TRANSFER_VALUE(maxMultiviewViewCount);
    TRANSFER_VALUE(maxMultiviewInstanceIndex);
}

void TransferValue(VkPhysicalDeviceVulkan11Properties *dest, VkPhysicalDeviceMaintenance3PropertiesKHR *src) {
    TRANSFER_VALUE(maxPerSetDescriptors);
    TRANSFER_VALUE(maxMemoryAllocationSize);
}

void TransferValue(VkPhysicalDeviceVulkan11Properties *dest, VkPhysicalDeviceProtectedMemoryProperties *src) {
    TRANSFER_VALUE(protectedNoFault);
}

// Features
void TransferValue(VkPhysicalDeviceVulkan11Features *dest, VkPhysicalDevice16BitStorageFeaturesKHR *src) {
    TRANSFER_VALUE(storageBuffer16BitAccess);
    TRANSFER_VALUE(uniformAndStorageBuffer16BitAccess);
    TRANSFER_VALUE(storagePushConstant16);
    TRANSFER_VALUE(storageInputOutput16);
}

void TransferValue(VkPhysicalDeviceVulkan11Features *dest, VkPhysicalDeviceMultiviewFeaturesKHR *src) {
    TRANSFER_VALUE(multiview);
    TRANSFER_VALUE(multiviewGeometryShader);
    TRANSFER_VALUE(multiviewTessellationShader);
}

void TransferValue(VkPhysicalDeviceVulkan11Features *dest, VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR *src) {
    TRANSFER_VALUE(samplerYcbcrConversion);
}

void TransferValue(VkPhysicalDeviceVulkan11Features *dest, VkPhysicalDeviceVariablePointersFeaturesKHR *src) {
    TRANSFER_VALUE(variablePointersStorageBuffer);
    TRANSFER_VALUE(variablePointers);
}

void TransferValue(VkPhysicalDeviceVulkan11Features *dest, VkPhysicalDeviceProtectedMemoryFeatures *src) {
    TRANSFER_VALUE(protectedMemory);
}

void TransferValue(VkPhysicalDeviceVulkan11Features *dest, VkPhysicalDeviceShaderDrawParametersFeatures *src) {
    TRANSFER_VALUE(shaderDrawParameters);
}

// VK_VULKAN_1_2

// Properties
void TransferValue(VkPhysicalDeviceVulkan12Properties *dest, VkPhysicalDeviceDepthStencilResolvePropertiesKHR *src) {
    TRANSFER_VALUE(supportedDepthResolveModes);
    TRANSFER_VALUE(supportedStencilResolveModes);
    TRANSFER_VALUE(independentResolveNone);
    TRANSFER_VALUE(independentResolve);
}

void TransferValue(VkPhysicalDeviceVulkan12Properties *dest, VkPhysicalDeviceDescriptorIndexingPropertiesEXT *src) {
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

void TransferValue(VkPhysicalDeviceVulkan12Properties *dest, VkPhysicalDeviceFloatControlsPropertiesKHR *src) {
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

void TransferValue(VkPhysicalDeviceVulkan12Properties *dest, VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT *src) {
    TRANSFER_VALUE(filterMinmaxSingleComponentFormats);
    TRANSFER_VALUE(filterMinmaxImageComponentMapping);
}

void TransferValue(VkPhysicalDeviceVulkan12Properties *dest, VkPhysicalDeviceTimelineSemaphorePropertiesKHR *src) {
    TRANSFER_VALUE(maxTimelineSemaphoreValueDifference);
}

// Features
void TransferValue(VkPhysicalDeviceVulkan12Features *dest, VkPhysicalDevice8BitStorageFeaturesKHR *src) {
    TRANSFER_VALUE(storageBuffer8BitAccess);
    TRANSFER_VALUE(uniformAndStorageBuffer8BitAccess);
    TRANSFER_VALUE(storagePushConstant8);
}

void TransferValue(VkPhysicalDeviceVulkan12Features *dest, VkPhysicalDeviceBufferDeviceAddressFeaturesKHR *src) {
    TRANSFER_VALUE(bufferDeviceAddress);
    TRANSFER_VALUE(bufferDeviceAddressCaptureReplay);
    TRANSFER_VALUE(bufferDeviceAddressMultiDevice);
}

void TransferValue(VkPhysicalDeviceVulkan12Features *dest, VkPhysicalDeviceDescriptorIndexingFeaturesEXT *src) {
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

void TransferValue(VkPhysicalDeviceVulkan12Features *dest, VkPhysicalDeviceHostQueryResetFeaturesEXT *src) {
    TRANSFER_VALUE(hostQueryReset);
}

void TransferValue(VkPhysicalDeviceVulkan12Features *dest, VkPhysicalDeviceImagelessFramebufferFeaturesKHR *src) {
    TRANSFER_VALUE(imagelessFramebuffer);
}

void TransferValue(VkPhysicalDeviceVulkan12Features *dest, VkPhysicalDeviceScalarBlockLayoutFeaturesEXT *src) {
    TRANSFER_VALUE(scalarBlockLayout);
}

void TransferValue(VkPhysicalDeviceVulkan12Features *dest, VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR *src) {
    TRANSFER_VALUE(separateDepthStencilLayouts);
}

void TransferValue(VkPhysicalDeviceVulkan12Features *dest, VkPhysicalDeviceShaderAtomicInt64FeaturesKHR *src) {
    TRANSFER_VALUE(shaderBufferInt64Atomics);
    TRANSFER_VALUE(shaderSharedInt64Atomics);
}

void TransferValue(VkPhysicalDeviceVulkan12Features *dest, VkPhysicalDeviceShaderFloat16Int8FeaturesKHR *src) {
    TRANSFER_VALUE(shaderFloat16);
    TRANSFER_VALUE(shaderInt8);
}

void TransferValue(VkPhysicalDeviceVulkan12Features *dest, VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR *src) {
    TRANSFER_VALUE(shaderSubgroupExtendedTypes);
}

void TransferValue(VkPhysicalDeviceVulkan12Features *dest, VkPhysicalDeviceTimelineSemaphoreFeaturesKHR *src) {
    TRANSFER_VALUE(timelineSemaphore);
}

void TransferValue(VkPhysicalDeviceVulkan12Features *dest, VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR *src) {
    TRANSFER_VALUE(uniformBufferStandardLayout);
}

void TransferValue(VkPhysicalDeviceVulkan12Features *dest, VkPhysicalDeviceVulkanMemoryModelFeaturesKHR *src) {
    TRANSFER_VALUE(vulkanMemoryModel);
    TRANSFER_VALUE(vulkanMemoryModelDeviceScope);
    TRANSFER_VALUE(vulkanMemoryModelAvailabilityVisibilityChains);
}

// VK_VULKAN_1_3

// Properties
void TransferValue(VkPhysicalDeviceVulkan13Properties *dest, VkPhysicalDeviceSubgroupSizeControlProperties *src) {
    TRANSFER_VALUE(minSubgroupSize);
    TRANSFER_VALUE(maxSubgroupSize);
    TRANSFER_VALUE(maxComputeWorkgroupSubgroups);
    TRANSFER_VALUE(requiredSubgroupSizeStages);
}

void TransferValue(VkPhysicalDeviceVulkan13Properties *dest, VkPhysicalDeviceInlineUniformBlockProperties *src) {
    TRANSFER_VALUE(maxInlineUniformBlockSize);
    TRANSFER_VALUE(maxPerStageDescriptorInlineUniformBlocks);
    TRANSFER_VALUE(maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks);
    TRANSFER_VALUE(maxDescriptorSetInlineUniformBlocks);
    TRANSFER_VALUE(maxDescriptorSetUpdateAfterBindInlineUniformBlocks);
}

void TransferValue(VkPhysicalDeviceVulkan13Properties *dest, VkPhysicalDeviceShaderIntegerDotProductProperties *src) {
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

void TransferValue(VkPhysicalDeviceVulkan13Properties *dest, VkPhysicalDeviceTexelBufferAlignmentProperties *src) {
    TRANSFER_VALUE(storageTexelBufferOffsetAlignmentBytes);
    TRANSFER_VALUE(storageTexelBufferOffsetSingleTexelAlignment);
    TRANSFER_VALUE(uniformTexelBufferOffsetAlignmentBytes);
    TRANSFER_VALUE(uniformTexelBufferOffsetSingleTexelAlignment);
}

void TransferValue(VkPhysicalDeviceVulkan13Properties *dest, VkPhysicalDeviceMaintenance4Properties *src) {
    TRANSFER_VALUE(maxBufferSize);
}

// Features
void TransferValue(VkPhysicalDeviceVulkan13Features *dest, VkPhysicalDeviceImageRobustnessFeatures *src) {
    TRANSFER_VALUE(robustImageAccess);
}

void TransferValue(VkPhysicalDeviceVulkan13Features *dest, VkPhysicalDeviceInlineUniformBlockFeatures *src) {
    TRANSFER_VALUE(inlineUniformBlock);
    TRANSFER_VALUE(descriptorBindingInlineUniformBlockUpdateAfterBind);
}

void TransferValue(VkPhysicalDeviceVulkan13Features *dest, VkPhysicalDevicePipelineCreationCacheControlFeatures *src) {
    TRANSFER_VALUE(pipelineCreationCacheControl);
}

void TransferValue(VkPhysicalDeviceVulkan13Features *dest, VkPhysicalDevicePrivateDataFeatures *src) {
    TRANSFER_VALUE(privateData);
}

void TransferValue(VkPhysicalDeviceVulkan13Features *dest, VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures *src) {
    TRANSFER_VALUE(shaderDemoteToHelperInvocation);
}

void TransferValue(VkPhysicalDeviceVulkan13Features *dest, VkPhysicalDeviceShaderTerminateInvocationFeatures *src) {
    TRANSFER_VALUE(shaderTerminateInvocation);
}

void TransferValue(VkPhysicalDeviceVulkan13Features *dest, VkPhysicalDeviceSubgroupSizeControlFeatures *src) {
    TRANSFER_VALUE(subgroupSizeControl);
    TRANSFER_VALUE(computeFullSubgroups);
}

void TransferValue(VkPhysicalDeviceVulkan13Features *dest, VkPhysicalDeviceSynchronization2Features *src) {
    TRANSFER_VALUE(synchronization2);
}

void TransferValue(VkPhysicalDeviceVulkan13Features *dest, VkPhysicalDeviceTextureCompressionASTCHDRFeatures *src) {
    TRANSFER_VALUE(textureCompressionASTC_HDR);
}

void TransferValue(VkPhysicalDeviceVulkan13Features *dest, VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures *src) {
    TRANSFER_VALUE(shaderZeroInitializeWorkgroupMemory);
}

void TransferValue(VkPhysicalDeviceVulkan13Features *dest, VkPhysicalDeviceDynamicRenderingFeatures *src) {
    TRANSFER_VALUE(dynamicRendering);
}

void TransferValue(VkPhysicalDeviceVulkan13Features *dest, VkPhysicalDeviceShaderIntegerDotProductFeatures *src) {
    TRANSFER_VALUE(shaderIntegerDotProduct);
}

void TransferValue(VkPhysicalDeviceVulkan13Features *dest, VkPhysicalDeviceMaintenance4Features *src) {
    TRANSFER_VALUE(maintenance4);
}

#undef TRANSFER_VALUE

void LoadDeviceFormats(VkInstance instance, VkPhysicalDevice pd, ArrayOfVkFormatProperties *dest) {
    std::vector<VkFormat> formats = {
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
        VkFormatProperties format_properties;
        dt->GetPhysicalDeviceFormatProperties(pd, format, &format_properties);
        (*dest)[format] = format_properties;
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

            pdd.extension_list_combination_mode_ = modifyExtensionList.mode;
            pdd.format_list_combination_mode_ = modifyFormatList.mode;
            pdd.format_properties_combination_mode_ = modifyFormatProperties.mode;

            EnumerateAll<VkExtensionProperties>(&(pdd.device_extensions_), [&](uint32_t *count, VkExtensionProperties *results) {
                return dt->EnumerateDeviceExtensionProperties(physical_device, nullptr, count, results);
            });

            pdd.simulation_extensions_ = pdd.device_extensions_;

            dt->GetPhysicalDeviceProperties(physical_device, &pdd.physical_device_properties_);
            bool api_version_above_1_1 = pdd.physical_device_properties_.apiVersion >= VK_API_VERSION_1_1;
            bool api_version_above_1_2 = pdd.physical_device_properties_.apiVersion >= VK_API_VERSION_1_2;
            bool api_version_above_1_3 = pdd.physical_device_properties_.apiVersion >= VK_API_VERSION_1_3;

            // Initialize PDD members to the actual Vulkan implementation's defaults.
            if (get_physical_device_properties2_active) {
                VkPhysicalDeviceProperties2KHR property_chain = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR};
                VkPhysicalDeviceFeatures2KHR feature_chain = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR};
                VkPhysicalDeviceMemoryProperties2KHR memory_chain = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2_KHR};

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME)) {
                    property_chain.pNext = &(pdd.physical_device_portability_subset_properties_);
                    feature_chain.pNext = &(pdd.physical_device_portability_subset_features_);
                } else if (emulatePortability.num > 0) {
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

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_8BIT_STORAGE_EXTENSION_NAME)) {
                    pdd.physical_device_8bit_storage_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_8bit_storage_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_16BIT_STORAGE_EXTENSION_NAME)) {
                    pdd.physical_device_16bit_storage_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_16bit_storage_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME)) {
                    pdd.physical_device_buffer_device_address_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_buffer_device_address_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME)) {
                    pdd.physical_device_depth_stencil_resolve_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_depth_stencil_resolve_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)) {
                    pdd.physical_device_descriptor_indexing_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_descriptor_indexing_properties_);

                    pdd.physical_device_descriptor_indexing_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_descriptor_indexing_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME)) {
                    pdd.physical_device_host_query_reset_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_host_query_reset_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME)) {
                    pdd.physical_device_imageless_framebuffer_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_imageless_framebuffer_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_MAINTENANCE3_EXTENSION_NAME)) {
                    pdd.physical_device_maintenance_3_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_maintenance_3_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_MAINTENANCE_4_EXTENSION_NAME)) {
                    pdd.physical_device_maintenance_4_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_maintenance_4_features_);

                    pdd.physical_device_maintenance_4_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_maintenance_4_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_MULTIVIEW_EXTENSION_NAME)) {
                    pdd.physical_device_multiview_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_multiview_properties_);

                    pdd.physical_device_multiview_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_multiview_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME)) {
                    pdd.physical_device_sampler_filter_minmax_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_sampler_filter_minmax_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME)) {
                    pdd.physical_device_sampler_ycbcr_conversion_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_sampler_ycbcr_conversion_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME)) {
                    pdd.physical_device_scalar_block_layout_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_scalar_block_layout_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device,
                                                              VK_KHR_SEPARATE_DEPTH_STENCIL_LAYOUTS_EXTENSION_NAME)) {
                    pdd.physical_device_separate_depth_stencil_layouts_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_separate_depth_stencil_layouts_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_SHADER_ATOMIC_INT64_EXTENSION_NAME)) {
                    pdd.physical_device_shader_atomic_int64_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_atomic_int64_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME)) {
                    pdd.physical_device_float_controls_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_float_controls_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME)) {
                    pdd.physical_device_shader_float16_int8_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_float16_int8_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device,
                                                              VK_KHR_SHADER_SUBGROUP_EXTENDED_TYPES_EXTENSION_NAME)) {
                    pdd.physical_device_shader_subgroup_extended_types_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_subgroup_extended_types_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME)) {
                    pdd.physical_device_timeline_semaphore_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_timeline_semaphore_properties_);

                    pdd.physical_device_timeline_semaphore_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_timeline_semaphore_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device,
                                                              VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT_EXTENSION_NAME)) {
                    pdd.physical_device_uniform_buffer_standard_layout_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_uniform_buffer_standard_layout_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME)) {
                    pdd.physical_device_variable_pointers_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_variable_pointers_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_VULKAN_MEMORY_MODEL_EXTENSION_NAME)) {
                    pdd.physical_device_vulkan_memory_model_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_vulkan_memory_model_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device,
                                                              VK_KHR_ZERO_INITIALIZE_WORKGROUP_MEMORY_EXTENSION_NAME)) {
                    pdd.physical_device_zero_initialize_workgroup_memory_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_zero_initialize_workgroup_memory_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME)) {
                    pdd.physical_device_acceleration_structure_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_acceleration_structure_features_);

                    pdd.physical_device_acceleration_structure_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_acceleration_structure_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_PERFORMANCE_QUERY_EXTENSION_NAME)) {
                    pdd.physical_device_performance_query_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_performance_query_features_);

                    pdd.physical_device_performance_query_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_performance_query_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device,
                                                              VK_KHR_PIPELINE_EXECUTABLE_PROPERTIES_EXTENSION_NAME)) {
                    pdd.physical_device_pipeline_executable_properties_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_pipeline_executable_properties_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_PRESENT_ID_EXTENSION_NAME)) {
                    pdd.physical_device_present_id_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_present_id_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_PRESENT_WAIT_EXTENSION_NAME)) {
                    pdd.physical_device_present_wait_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_present_wait_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME)) {
                    pdd.physical_device_push_descriptor_properites_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_push_descriptor_properites_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_RAY_QUERY_EXTENSION_NAME)) {
                    pdd.physical_device_ray_query_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_ray_query_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME)) {
                    pdd.physical_device_ray_tracing_pipeline_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_ray_tracing_pipeline_features_);

                    pdd.physical_device_ray_tracing_pipeline_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_ray_tracing_pipeline_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_SHADER_CLOCK_EXTENSION_NAME)) {
                    pdd.physical_device_shader_clock_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_clock_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_SHADER_INTEGER_DOT_PRODUCT_EXTENSION_NAME)) {
                    pdd.physical_device_shader_integer_dot_product_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_integer_dot_product_features_);

                    pdd.physical_device_shader_integer_dot_products_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_shader_integer_dot_products_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device,
                                                              VK_KHR_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_EXTENSION_NAME)) {
                    pdd.physical_device_shader_subgroup_uniform_control_flow_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_subgroup_uniform_control_flow_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_SHADER_TERMINATE_INVOCATION_EXTENSION_NAME)) {
                    pdd.physical_device_shader_terminate_invocation_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_terminate_invocation_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME)) {
                    pdd.physical_device_synchronization2_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_synchronization2_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device,
                                                              VK_KHR_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_EXTENSION_NAME)) {
                    pdd.physical_device_workgroup_memory_explicit_layout_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_workgroup_memory_explicit_layout_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_4444_FORMATS_EXTENSION_NAME)) {
                    pdd.physical_device_4444_formats_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_4444_formats_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_ASTC_DECODE_MODE_EXTENSION_NAME)) {
                    pdd.physical_device_astc_decode_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_astc_decode_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME)) {
                    pdd.physical_device_blend_operation_advanced_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_blend_operation_advanced_features_);

                    pdd.physical_device_blend_operation_advanced_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_blend_operation_advanced_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_BORDER_COLOR_SWIZZLE_EXTENSION_NAME)) {
                    pdd.physical_device_border_color_swizzle_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_border_color_swizzle_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_COLOR_WRITE_ENABLE_EXTENSION_NAME)) {
                    pdd.physical_device_color_write_enable_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_color_write_enable_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_CONDITIONAL_RENDERING_EXTENSION_NAME)) {
                    pdd.physical_device_conditional_rendering_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_conditional_rendering_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME)) {
                    pdd.physical_device_conservative_rasterization_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_conservative_rasterization_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_CUSTOM_BORDER_COLOR_EXTENSION_NAME)) {
                    pdd.physical_device_custom_border_color_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_custom_border_color_features_);

                    pdd.physical_device_custom_border_color_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_custom_border_color_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_DEPTH_CLIP_ENABLE_EXTENSION_NAME)) {
                    pdd.physical_device_depth_clip_enable_features_ext_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_depth_clip_enable_features_ext_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_DEVICE_MEMORY_REPORT_EXTENSION_NAME)) {
                    pdd.physical_device_device_memory_report_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_device_memory_report_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_DISCARD_RECTANGLES_EXTENSION_NAME)) {
                    pdd.physical_device_discard_rectangle_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_discard_rectangle_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME)) {
                    pdd.physical_device_extended_dynamic_state_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_extended_dynamic_state_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME)) {
                    pdd.physical_device_extended_dynamic_state2_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_extended_dynamic_state2_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_EXTERNAL_MEMORY_HOST_EXTENSION_NAME)) {
                    pdd.physical_device_external_memory_host_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_external_memory_host_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_FRAGMENT_DENSITY_MAP_EXTENSION_NAME)) {
                    pdd.physical_device_fragment_density_map_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_fragment_density_map_features_);

                    pdd.physical_device_fragment_density_map_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_fragment_density_map_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME)) {
                    pdd.physical_device_fragment_shader_interlock_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_fragment_shader_interlock_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_GLOBAL_PRIORITY_QUERY_EXTENSION_NAME)) {
                    pdd.physical_device_global_priority_query_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_global_priority_query_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_IMAGE_ROBUSTNESS_EXTENSION_NAME)) {
                    pdd.physical_device_image_robustness_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_image_robustness_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_INDEX_TYPE_UINT8_EXTENSION_NAME)) {
                    pdd.physical_device_index_type_uint8_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_index_type_uint8_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME)) {
                    pdd.physical_device_inline_uniform_block_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_inline_uniform_block_features_);

                    pdd.physical_device_inline_uniform_block_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_inline_uniform_block_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME)) {
                    pdd.physical_device_line_rasterization_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_line_rasterization_features_);

                    pdd.physical_device_line_rasterization_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_line_rasterization_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME)) {
                    pdd.physical_device_memory_priority_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_memory_priority_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_MULTI_DRAW_EXTENSION_NAME)) {
                    pdd.physical_device_multi_draw_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_multi_draw_features_);

                    pdd.physical_device_multi_draw_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_multi_draw_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device,
                                                              VK_EXT_PAGEABLE_DEVICE_LOCAL_MEMORY_EXTENSION_NAME)) {
                    pdd.physical_device_pageable_device_local_memory_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_pageable_device_local_memory_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device,
                                                              VK_EXT_PIPELINE_CREATION_CACHE_CONTROL_EXTENSION_NAME)) {
                    pdd.physical_device_pipeline_creation_cache_control_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_pipeline_creation_cache_control_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device,
                                                              VK_EXT_PRIMITIVE_TOPOLOGY_LIST_RESTART_EXTENSION_NAME)) {
                    pdd.physical_device_primitive_topology_list_restart_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_primitive_topology_list_restart_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_PRIVATE_DATA_EXTENSION_NAME)) {
                    pdd.physical_device_private_data_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_private_data_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_PROVOKING_VERTEX_EXTENSION_NAME)) {
                    pdd.physical_device_provoking_vertex_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_provoking_vertex_features_);

                    pdd.physical_device_provoking_vertex_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_provoking_vertex_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_RGBA10X6_FORMATS_EXTENSION_NAME)) {
                    pdd.physical_device_rgba10x6_formats_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_rgba10x6_formats_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_ROBUSTNESS_2_EXTENSION_NAME)) {
                    pdd.physical_device_robustness_2_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_robustness_2_features_);

                    pdd.physical_device_robustness_2_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_robustness_2_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_SAMPLE_LOCATIONS_EXTENSION_NAME)) {
                    pdd.physical_device_sample_locations_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_sample_locations_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME)) {
                    pdd.physical_device_shader_atomic_float_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_atomic_float_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_SHADER_ATOMIC_FLOAT_2_EXTENSION_NAME)) {
                    pdd.physical_device_shader_atomic_float2_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_atomic_float2_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device,
                                                              VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME)) {
                    pdd.physical_device_shader_demote_to_helper_invocation_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_demote_to_helper_invocation_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_SHADER_IMAGE_ATOMIC_INT64_EXTENSION_NAME)) {
                    pdd.physical_device_shader_image_atomic_int64_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_image_atomic_int64_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME)) {
                    pdd.physical_device_subgroup_size_control_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_subgroup_size_control_features_);

                    pdd.physical_device_subgroup_size_control_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_subgroup_size_control_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME)) {
                    pdd.physical_device_texel_buffer_alignment_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_texel_buffer_alignment_features_);

                    pdd.physical_device_texel_buffer_alignment_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_texel_buffer_alignment_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device,
                                                              VK_EXT_TEXTURE_COMPRESSION_ASTC_HDR_EXTENSION_NAME)) {
                    pdd.physical_device_texture_compression_astc_hdr_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_texture_compression_astc_hdr_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME)) {
                    pdd.physical_device_transform_feedback_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_transform_feedback_features_);

                    pdd.physical_device_transform_feedback_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_transform_feedback_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME)) {
                    pdd.physical_device_vertex_attribute_divisor_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_vertex_attribute_divisor_features_);

                    pdd.physical_device_vertex_attirbute_divisor_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_vertex_attirbute_divisor_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME)) {
                    pdd.physical_device_vertex_input_dynamic_state_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_vertex_input_dynamic_state_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_YCBCR_2PLANE_444_FORMATS_EXTENSION_NAME)) {
                    pdd.physical_device_ycbcr_2plane_444_formats_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_ycbcr_2plane_444_formats_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_YCBCR_IMAGE_ARRAYS_EXTENSION_NAME)) {
                    pdd.physical_device_ycbcr_image_arrays_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_ycbcr_image_arrays_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME)) {
                    pdd.physical_device_fragment_shading_rate_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_fragment_shading_rate_features_);

                    pdd.physical_device_fragment_shading_rate_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_fragment_shading_rate_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME)) {
                    pdd.physical_device_coherent_memory_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_coherent_memory_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_AMD_SHADER_CORE_PROPERTIES_EXTENSION_NAME)) {
                    pdd.physical_device_shader_core_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_shader_core_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_AMD_SHADER_CORE_PROPERTIES_2_EXTENSION_NAME)) {
                    pdd.physical_device_shader_core_properties_2_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_shader_core_properties_2_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_HUAWEI_INVOCATION_MASK_EXTENSION_NAME)) {
                    pdd.physical_device_invocation_mask_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_invocation_mask_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_HUAWEI_SUBPASS_SHADING_EXTENSION_NAME)) {
                    pdd.physical_device_subpass_shading_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_subpass_shading_features_);

                    pdd.physical_device_subpass_shading_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_subpass_shading_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device,
                                                              VK_INTEL_SHADER_INTEGER_FUNCTIONS_2_EXTENSION_NAME)) {
                    pdd.physical_device_shader_integer_functions_2_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_integer_functions_2_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_NV_COMPUTE_SHADER_DERIVATIVES_EXTENSION_NAME)) {
                    pdd.physical_device_compute_shader_derivatives_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_compute_shader_derivatives_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_NV_COOPERATIVE_MATRIX_EXTENSION_NAME)) {
                    pdd.physical_device_cooperative_matrix_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_cooperative_matrix_features_);

                    pdd.physical_device_cooperative_matrix_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_cooperative_matrix_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_NV_CORNER_SAMPLED_IMAGE_EXTENSION_NAME)) {
                    pdd.physical_device_corner_sampled_image_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_corner_sampled_image_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_NV_COVERAGE_REDUCTION_MODE_EXTENSION_NAME)) {
                    pdd.physical_device_coverage_reduction_mode_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_coverage_reduction_mode_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device,
                                                              VK_NV_DEDICATED_ALLOCATION_IMAGE_ALIASING_EXTENSION_NAME)) {
                    pdd.physical_device_dedicated_allocation_image_aliasing_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_dedicated_allocation_image_aliasing_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_NV_DEVICE_DIAGNOSTICS_CONFIG_EXTENSION_NAME)) {
                    pdd.physical_device_diagnostics_config_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_diagnostics_config_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_NV_DEVICE_GENERATED_COMMANDS_EXTENSION_NAME)) {
                    pdd.physical_device_device_generated_commands_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_device_generated_commands_features_);

                    pdd.physical_device_device_generated_commands_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_device_generated_commands_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_NV_EXTERNAL_MEMORY_RDMA_EXTENSION_NAME)) {
                    pdd.physical_device_external_memory_rdma_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_external_memory_rdma_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_NV_FRAGMENT_SHADER_BARYCENTRIC_EXTENSION_NAME)) {
                    pdd.physical_device_fragment_shader_barycentric_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_fragment_shader_barycentric_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_NV_FRAGMENT_SHADING_RATE_ENUMS_EXTENSION_NAME)) {
                    pdd.physical_device_fragment_shading_rate_enums_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_fragment_shading_rate_enums_features_);

                    pdd.physical_device_fragment_shading_rate_enums_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_fragment_shading_rate_enums_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_NV_INHERITED_VIEWPORT_SCISSOR_EXTENSION_NAME)) {
                    pdd.physical_device_inherited_viewport_scissor_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_inherited_viewport_scissor_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_NV_MESH_SHADER_EXTENSION_NAME)) {
                    pdd.physical_device_mesh_shader_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_mesh_shader_features_);

                    pdd.physical_device_mesh_shader_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_mesh_shader_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_NV_RAY_TRACING_EXTENSION_NAME)) {
                    pdd.physical_device_ray_tracing_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_ray_tracing_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_NV_RAY_TRACING_MOTION_BLUR_EXTENSION_NAME)) {
                    pdd.physical_device_ray_tracing_motiuon_blur_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_ray_tracing_motiuon_blur_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_NV_REPRESENTATIVE_FRAGMENT_TEST_EXTENSION_NAME)) {
                    pdd.physical_device_representative_fragment_test_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_representative_fragment_test_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_NV_SCISSOR_EXCLUSIVE_EXTENSION_NAME)) {
                    pdd.physical_device_exclusive_scissor_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_exclusive_scissor_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_NV_SHADER_IMAGE_FOOTPRINT_EXTENSION_NAME)) {
                    pdd.physical_device_shader_image_footprint_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_image_footprint_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_NV_SHADER_SM_BUILTINS_EXTENSION_NAME)) {
                    pdd.physical_device_shader_sm_builtins_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_sm_builtins_features_);

                    pdd.physical_device_shader_sm_builtins_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_shader_sm_builtins_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_NV_SHADING_RATE_IMAGE_EXTENSION_NAME)) {
                    pdd.physical_device_shading_rate_image_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shading_rate_image_features_);

                    pdd.physical_device_shading_rate_image_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_shading_rate_image_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_VALVE_MUTABLE_DESCRIPTOR_TYPE_EXTENSION_NAME)) {
                    pdd.physical_device_mutable_descriptor_type_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_mutable_descriptor_type_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME)) {
                    pdd.physical_device_dynamic_rendering_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_dynamic_rendering_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_IMAGE_VIEW_MIN_LOD_EXTENSION_NAME)) {
                    pdd.physical_device_image_view_min_lod_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_image_view_min_lod_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_FRAGMENT_DENSITY_MAP_2_EXTENSION_NAME)) {
                    pdd.physical_device_fragment_density_map_2_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_fragment_density_map_2_features_);

                    pdd.physical_device_fragment_density_map_2_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_fragment_density_map_2_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device,
                                                              VK_QCOM_FRAGMENT_DENSITY_MAP_OFFSET_EXTENSION_NAME)) {
                    pdd.physical_device_fragment_density_map_offset_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_fragment_density_map_offset_features_);

                    pdd.physical_device_fragment_density_map_offset_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_fragment_density_map_offset_properties_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_EXT_DEPTH_CLIP_CONTROL_EXTENSION_NAME)) {
                    pdd.physical_device_depth_clip_control_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_depth_clip_control_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device,
                                                              VK_ARM_RASTERIZATION_ORDER_ATTACHMENT_ACCESS_EXTENSION_NAME)) {
                    pdd.physical_device_rasterization_order_attachment_access_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_rasterization_order_attachment_access_features_);
                }

                if (PhysicalDeviceData::HasSimulatedExtension(physical_device, VK_NV_LINEAR_COLOR_ATTACHMENT_EXTENSION_NAME)) {
                    pdd.physical_device_linear_color_attachment_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_linear_color_attachment_features_);
                }

                if (api_version_above_1_1) {
                    pdd.physical_device_protected_memory_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_protected_memory_properties_);

                    pdd.physical_device_protected_memory_features_.pNext = feature_chain.pNext;

                    pdd.physical_device_shader_draw_parameters_features_.pNext = &(pdd.physical_device_protected_memory_features_);

                    feature_chain.pNext = &(pdd.physical_device_shader_draw_parameters_features_);
                }

                if (api_version_above_1_2) {
                    // VK_VULKAN_1_1
                    pdd.physical_device_vulkan_1_1_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_vulkan_1_1_properties_);

                    pdd.physical_device_vulkan_1_1_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_vulkan_1_1_features_);

                    // VK_VULKAN_1_2
                    pdd.physical_device_vulkan_1_2_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_vulkan_1_2_properties_);

                    pdd.physical_device_vulkan_1_2_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_vulkan_1_2_features_);
                }

                if (api_version_above_1_3) {
                    // VK_VULKAN_1_3
                    pdd.physical_device_vulkan_1_3_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_vulkan_1_3_properties_);

                    pdd.physical_device_vulkan_1_3_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_vulkan_1_3_features_);
                }

                dt->GetPhysicalDeviceProperties2KHR(physical_device, &property_chain);
                dt->GetPhysicalDeviceFeatures2KHR(physical_device, &feature_chain);
                dt->GetPhysicalDeviceMemoryProperties2KHR(physical_device, &memory_chain);

                pdd.physical_device_properties_ = property_chain.properties;
                pdd.physical_device_features_ = feature_chain.features;
                pdd.physical_device_memory_properties_ = memory_chain.memoryProperties;
            } else {
                dt->GetPhysicalDeviceFeatures(physical_device, &pdd.physical_device_features_);
                dt->GetPhysicalDeviceMemoryProperties(physical_device, &pdd.physical_device_memory_properties_);
            }

            LoadDeviceFormats(instance, physical_device, &pdd.device_formats_);

            DebugPrintf("\tdeviceName \"%s\"\n", pdd.physical_device_properties_.deviceName);

            // Override PDD members with values from configuration file(s).
            JsonLoader json_loader(pdd);
            result = json_loader.LoadFiles();

            pdd.combineExtensionLists();

            // VK_VULKAN_1_1
            TransferValue(&(pdd.physical_device_vulkan_1_1_properties_), &(pdd.physical_device_multiview_properties_));
            TransferValue(&(pdd.physical_device_vulkan_1_1_properties_), &(pdd.physical_device_maintenance_3_properties_));
            TransferValue(&(pdd.physical_device_vulkan_1_1_properties_), &(pdd.physical_device_protected_memory_properties_));

            TransferValue(&(pdd.physical_device_vulkan_1_1_features_), &(pdd.physical_device_16bit_storage_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_1_features_), &(pdd.physical_device_multiview_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_1_features_), &(pdd.physical_device_sampler_ycbcr_conversion_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_1_features_), &(pdd.physical_device_variable_pointers_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_1_features_), &(pdd.physical_device_protected_memory_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_1_features_), &(pdd.physical_device_shader_draw_parameters_features_));

            // VK_VULKAN_1_2
            TransferValue(&(pdd.physical_device_vulkan_1_2_properties_), &(pdd.physical_device_depth_stencil_resolve_properties_));
            TransferValue(&(pdd.physical_device_vulkan_1_2_properties_), &(pdd.physical_device_descriptor_indexing_properties_));
            TransferValue(&(pdd.physical_device_vulkan_1_2_properties_), &(pdd.physical_device_float_controls_properties_));
            TransferValue(&(pdd.physical_device_vulkan_1_2_properties_), &(pdd.physical_device_sampler_filter_minmax_properties_));
            TransferValue(&(pdd.physical_device_vulkan_1_2_properties_), &(pdd.physical_device_timeline_semaphore_properties_));

            TransferValue(&(pdd.physical_device_vulkan_1_2_features_), &(pdd.physical_device_8bit_storage_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_2_features_), &(pdd.physical_device_buffer_device_address_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_2_features_), &(pdd.physical_device_descriptor_indexing_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_2_features_), &(pdd.physical_device_host_query_reset_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_2_features_), &(pdd.physical_device_imageless_framebuffer_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_2_features_), &(pdd.physical_device_scalar_block_layout_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_2_features_),
                          &(pdd.physical_device_separate_depth_stencil_layouts_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_2_features_), &(pdd.physical_device_shader_atomic_int64_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_2_features_), &(pdd.physical_device_shader_float16_int8_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_2_features_),
                          &(pdd.physical_device_shader_subgroup_extended_types_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_2_features_), &(pdd.physical_device_timeline_semaphore_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_2_features_),
                          &(pdd.physical_device_uniform_buffer_standard_layout_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_2_features_), &(pdd.physical_device_vulkan_memory_model_features_));

            // VK_VULKAN_1_3
            TransferValue(&(pdd.physical_device_vulkan_1_3_properties_), &(pdd.physical_device_subgroup_size_control_properties_));
            TransferValue(&(pdd.physical_device_vulkan_1_3_properties_), &(pdd.physical_device_inline_uniform_block_properties_));
            TransferValue(&(pdd.physical_device_vulkan_1_3_properties_),
                          &(pdd.physical_device_shader_integer_dot_products_properties_));
            TransferValue(&(pdd.physical_device_vulkan_1_3_properties_), &(pdd.physical_device_texel_buffer_alignment_properties_));
            TransferValue(&(pdd.physical_device_vulkan_1_3_properties_), &(pdd.physical_device_maintenance_4_properties_));

            TransferValue(&(pdd.physical_device_vulkan_1_3_features_), &(pdd.physical_device_image_robustness_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_3_features_), &(pdd.physical_device_inline_uniform_block_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_3_features_),
                          &(pdd.physical_device_pipeline_creation_cache_control_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_3_features_), &(pdd.physical_device_private_data_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_3_features_),
                          &(pdd.physical_device_shader_demote_to_helper_invocation_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_3_features_),
                          &(pdd.physical_device_shader_terminate_invocation_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_3_features_), &(pdd.physical_device_subgroup_size_control_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_3_features_), &(pdd.physical_device_synchronization2_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_3_features_),
                          &(pdd.physical_device_texture_compression_astc_hdr_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_3_features_),
                          &(pdd.physical_device_zero_initialize_workgroup_memory_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_3_features_), &(pdd.physical_device_dynamic_rendering_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_3_features_), &(pdd.physical_device_shader_integer_dot_product_features_));
            TransferValue(&(pdd.physical_device_vulkan_1_3_features_), &(pdd.physical_device_maintenance_4_features_));
        }
    }

    fflush(stdout);

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
    /*GET_PROC_ADDR(GetPhysicalDeviceMemoryProperties);
    GET_PROC_ADDR(GetPhysicalDeviceMemoryProperties2);
    GET_PROC_ADDR(GetPhysicalDeviceMemoryProperties2KHR);
    GET_PROC_ADDR(GetPhysicalDeviceQueueFamilyProperties);
    GET_PROC_ADDR(GetPhysicalDeviceQueueFamilyProperties2);
    GET_PROC_ADDR(GetPhysicalDeviceQueueFamilyProperties2KHR);*/
    GET_PROC_ADDR(GetPhysicalDeviceFormatProperties);
    GET_PROC_ADDR(GetPhysicalDeviceFormatProperties2);
    GET_PROC_ADDR(GetPhysicalDeviceFormatProperties2KHR);
    GET_PROC_ADDR(GetPhysicalDeviceImageFormatProperties);
    GET_PROC_ADDR(GetPhysicalDeviceImageFormatProperties2);
    GET_PROC_ADDR(GetPhysicalDeviceImageFormatProperties2KHR);
    /*GET_PROC_ADDR(GetPhysicalDeviceSurfaceCapabilitiesKHR);
    GET_PROC_ADDR(GetPhysicalDeviceSurfaceCapabilities2KHR);
    GET_PROC_ADDR(GetPhysicalDeviceSurfaceFormatsKHR);
    GET_PROC_ADDR(GetPhysicalDeviceSurfaceFormats2KHR);
    GET_PROC_ADDR(GetPhysicalDeviceSurfacePresentModesKHR);
    GET_PROC_ADDR(GetPhysicalDeviceToolPropertiesEXT);*/
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
// Keep synchronized with VisualStudio's VkLayer_device_simulation.def

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
