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
 * layersvt/device_simulation.cpp - The VK_LAYER_LUNARG_device_simulation layer.
 * This DevSim layer simulates a device by loading a JSON configuration file to override values that would normally be returned
 * from a Vulkan implementation.  Configuration files must validate with the DevSim schema; this layer does not redundantly
 * check for configuration errors that would be caught by schema validation.
 * See JsonLoader::IdentifySchema() for the URIs of supported schemas.
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

const uint32_t kVersionDevsimMajor = 1;
const uint32_t kVersionDevsimMinor = 9;
const uint32_t kVersionDevsimPatch = 1;
const uint32_t kVersionDevsimImplementation = VK_MAKE_VERSION(kVersionDevsimMajor, kVersionDevsimMinor, kVersionDevsimPatch);

// Properties of this layer:
const VkLayerProperties kLayerProperties[] = {{
    "VK_LAYER_LUNARG_device_simulation",  // layerName
    VK_MAKE_VERSION(1, 0, 68),            // specVersion (clamped to final 1.0 spec version)
    kVersionDevsimImplementation,         // implementationVersion
    "LunarG device simulation layer"      // description
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

// The "standard" core VkFormat enum values:
const VkFormat StandardVkFormatEnumList[] = {
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
};

enum ArrayCombinationMode {
    ARRAY_COMBINATION_MODE_NONE,
    ARRAY_COMBINATION_MODE_REPLACE,
    ARRAY_COMBINATION_MODE_WHITELIST,
    ARRAY_COMBINATION_MODE_BLACKLIST,
    ARRAY_COMBINATION_MODE_INTERSECT
};

// Environment variables defined by this layer ///////////////////////////////////////////////////////////////////////////////////

#if defined(__ANDROID__)
const char *const kEnvarDevsimFilename = "debug.vulkan.devsim.filepath";        // path of the configuration file(s) to load.
const char *const kEnvarDevsimDebugEnable = "debug.vulkan.devsim.debugenable";  // a non-zero integer will enable debugging output.
const char *const kEnvarDevsimExitOnError = "debug.vulkan.devsim.exitonerror";  // a non-zero integer will enable exit-on-error.
const char *const kEnvarDevsimEmulatePortability =
    "debug.vulkan.devsim.emulateportability";  // a non-zero integer will enable emulation of the VK_KHR_portability_subset
                                               // extension.
const char *const kEnvarDevsimModifyExtensionList =
    "debug.vulkan.devsim.modifyextensionlist";  // a non-zero integer will enable modifying device extensions list.
const char *const kEnvarDevsimModifyMemoryFlags =
    "debug.vulkan.devsim.modifymemoryflags";  // a non-zero integer will enable modifying device memory flags.
const char *const kEnvarDevsimModifyFormatList =
    "debug.vulkan.devsim.modifyformatlist";  // an ArrayCombinationMode value sets how the device and config format lists are
                                             // combined.
const char *const kEnvarDevsimModifyFormatProperties =
    "debug.vulkan.devsim.modifyformatproperties";  // an ArrayCombinationMode value sets how the device and config format properties
                                                   // are combined.
const char *const kEnvarDevsimModifySurfaceFormats =
    "debug.vulkan.devsim.modifysurfaceformats";  // an ArrayCombinationMode value sets how the device and config surface format
                                                 // lists are combined.
const char *const kEnvarDevsimModifyPresentModes =
    "debug.vulkan.devsim.modifypresentmodes";  // an ArrayCombinationMode value sets how the device and config present modes are
                                               // combined.
#else
const char *const kEnvarDevsimFilename = "VK_DEVSIM_FILENAME";          // path of the configuration file(s) to load.
const char *const kEnvarDevsimDebugEnable = "VK_DEVSIM_DEBUG_ENABLE";   // a non-zero integer will enable debugging output.
const char *const kEnvarDevsimExitOnError = "VK_DEVSIM_EXIT_ON_ERROR";  // a non-zero integer will enable exit-on-error.
const char *const kEnvarDevsimEmulatePortability =
    "VK_DEVSIM_EMULATE_PORTABILITY_SUBSET_EXTENSION";  // a non-zero integer will enable emulation of the VK_KHR_portability_subset
                                                       // extension.
const char *const kEnvarDevsimModifyExtensionList =
    "VK_DEVSIM_MODIFY_EXTENSION_LIST";  // an ArrayCombinationMode value sets how the device and config extension lists are
                                        // combined.
const char *const kEnvarDevsimModifyMemoryFlags =
    "VK_DEVSIM_MODIFY_MEMORY_FLAGS";  // a non-zero integer will enable modifying device memory flags.
const char *const kEnvarDevsimModifyFormatList =
    "VK_DEVSIM_MODIFY_FORMAT_LIST";  // an ArrayCombinationMode value sets how the device and config format lists are combined.
const char *const kEnvarDevsimModifyFormatProperties =
    "VK_DEVSIM_MODIFY_FORMAT_PROPERTIES";  // an ArrayCombinationMode value sets how the device and config format properties are
                                           // combined.
const char *const kEnvarDevsimModifySurfaceFormats =
    "VK_DEVSIM_MODIFY_SURFACE_FORMATS";  // an ArrayCombinationMode value sets how the device and config surface format lists are
                                         // combined.
const char *const kEnvarDevsimModifyPresentModes =
    "VK_DEVSIM_MODIFY_PRESENT_MODES";  // an ArrayCombinationMode value sets how the device and config present modes are combined.
#endif

const char *const kLayerSettingsDevsimFilename = "filename";         // vk_layer_settings.txt equivalent for kEnvarDevsimFilename
const char *const kLayerSettingsDevsimDebugEnable = "debug_enable";  // vk_layer_settings.txt equivalent for kEnvarDevsimDebugEnable
const char *const kLayerSettingsDevsimExitOnError =
    "exit_on_error";  // vk_layer_settings.txt equivalent for kEnvarDevsimExitOnError
const char *const kLayerSettingsDevsimEmulatePortability =
    "emulate_portability";  // vk_layer_settings.txt equivalent for kEnvarDevsimEmulatePortability
const char *const kLayerSettingsDevsimModifyExtensionList =
    "modify_extension_list";  // vk_layer_settings.txt equivalent for kEnvarDevsimModifyExtensionList
const char *const kLayerSettingsDevsimModifyMemoryFlags =
    "modify_memory_flags";  // vk_layer_settings.txt equivalent for kEnvarDevsimModifyMemoryFlags
const char *const kLayerSettingsDevsimModifyFormatList =
    "modify_format_list";  // an ArrayCombinationMode value sets how the device and config format lists are combined.
const char *const kLayerSettingsDevsimModifyFormatProperties =
    "modify_format_properties";  // an ArrayCombinationMode value sets how the device and config format properties are combined.
const char *const kLayerSettingsDevsimModifySurfaceFormats =
    "modify_surface_formats";  // an ArrayCombinationMode value sets how the device and config surface format lists are combined.
const char *const kLayerSettingsDevsimModifyPresentModes =
    "modify_present_modes";  // an ArrayCombinationMode value sets how the device and config present modes are combined.

struct ArrayCombinationModeSetting {
    ArrayCombinationMode mode;
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
struct IntSetting debugLevel;
struct IntSetting errorLevel;
struct IntSetting emulatePortability;
struct ArrayCombinationModeSetting modifyExtensionList;
struct IntSetting modifyMemoryFlags;
struct ArrayCombinationModeSetting modifyFormatList;
struct ArrayCombinationModeSetting modifyFormatProperties;
struct ArrayCombinationModeSetting modifySurfaceFormats;
struct ArrayCombinationModeSetting modifyPresentModes;

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
        __android_log_print(ANDROID_LOG_INFO, "devsim", "Vulkan device simulation layer %s: %s", command.c_str(),
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
            __android_log_print(ANDROID_LOG_DEBUG, "devsim", "%s", message);
            break;
        case VK_LOG_ERROR:
            __android_log_print(ANDROID_LOG_ERROR, "devsim", "%s", message);
            break;
        default:
            __android_log_print(ANDROID_LOG_INFO, "devsim", "%s", message);
            break;
    }
    free(message);
}
#endif

void DebugPrintf(const char *fmt, ...) {
    if (debugLevel.num > 0) {
#if !defined(__ANDROID__)
        printf("\tDEBUG devsim ");
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
    fprintf(stderr, "\tERROR devsim ");
#endif
    va_list args;
    va_start(args, fmt);
#if defined(__ANDROID__)
    AndroidPrintf(VK_LOG_ERROR, fmt, args);
#else
    vfprintf(stderr, fmt, args);
#endif
    va_end(args);
    if (errorLevel.num > 0) {
#if defined(__ANDROID__)
        __android_log_print(ANDROID_LOG_ERROR, "devsim", "devsim exiting on error as requested");
#else
        fprintf(stderr, "\ndevsim exiting on error as requested\n\n");
#endif
        exit(1);
    }
}

std::string vkFormatToString(VkFormat fmt) {
    switch (fmt) {
        case VK_FORMAT_UNDEFINED:
            return "VK_FORMAT_UNDEFINED";
            break;
        case VK_FORMAT_R4G4_UNORM_PACK8:
            return "VK_FORMAT_R4G4_UNORM_PACK8";
            break;
        case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
            return "VK_FORMAT_R4G4B4A4_UNORM_PACK16";
            break;
        case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
            return "VK_FORMAT_B4G4R4A4_UNORM_PACK16";
            break;
        case VK_FORMAT_R5G6B5_UNORM_PACK16:
            return "VK_FORMAT_R5G6B5_UNORM_PACK16";
            break;
        case VK_FORMAT_B5G6R5_UNORM_PACK16:
            return "VK_FORMAT_B5G6R5_UNORM_PACK16";
            break;
        case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
            return "VK_FORMAT_R5G5B5A1_UNORM_PACK16";
            break;
        case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
            return "VK_FORMAT_B5G5R5A1_UNORM_PACK16";
            break;
        case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
            return "VK_FORMAT_A1R5G5B5_UNORM_PACK16";
            break;
        case VK_FORMAT_R8_UNORM:
            return "VK_FORMAT_R8_UNORM";
            break;
        case VK_FORMAT_R8_SNORM:
            return "VK_FORMAT_R8_SNORM";
            break;
        case VK_FORMAT_R8_USCALED:
            return "VK_FORMAT_R8_USCALED";
            break;
        case VK_FORMAT_R8_SSCALED:
            return "VK_FORMAT_R8_SSCALED";
            break;
        case VK_FORMAT_R8_UINT:
            return "VK_FORMAT_R8_UINT";
            break;
        case VK_FORMAT_R8_SINT:
            return "VK_FORMAT_R8_SINT";
            break;
        case VK_FORMAT_R8_SRGB:
            return "VK_FORMAT_R8_SRGB";
            break;
        case VK_FORMAT_R8G8_UNORM:
            return "VK_FORMAT_R8G8_UNORM";
            break;
        case VK_FORMAT_R8G8_SNORM:
            return "VK_FORMAT_R8G8_SNORM";
            break;
        case VK_FORMAT_R8G8_USCALED:
            return "VK_FORMAT_R8G8_USCALED";
            break;
        case VK_FORMAT_R8G8_SSCALED:
            return "VK_FORMAT_R8G8_SSCALED";
            break;
        case VK_FORMAT_R8G8_UINT:
            return "VK_FORMAT_R8G8_UINT";
            break;
        case VK_FORMAT_R8G8_SINT:
            return "VK_FORMAT_R8G8_SINT";
            break;
        case VK_FORMAT_R8G8_SRGB:
            return "VK_FORMAT_R8G8_SRGB";
            break;
        case VK_FORMAT_R8G8B8_UNORM:
            return "VK_FORMAT_R8G8B8_UNORM";
            break;
        case VK_FORMAT_R8G8B8_SNORM:
            return "VK_FORMAT_R8G8B8_SNORM";
            break;
        case VK_FORMAT_R8G8B8_USCALED:
            return "VK_FORMAT_R8G8B8_USCALED";
            break;
        case VK_FORMAT_R8G8B8_SSCALED:
            return "VK_FORMAT_R8G8B8_SSCALED";
            break;
        case VK_FORMAT_R8G8B8_UINT:
            return "VK_FORMAT_R8G8B8_UINT";
            break;
        case VK_FORMAT_R8G8B8_SINT:
            return "VK_FORMAT_R8G8B8_SINT";
            break;
        case VK_FORMAT_R8G8B8_SRGB:
            return "VK_FORMAT_R8G8B8_SRGB";
            break;
        case VK_FORMAT_B8G8R8_UNORM:
            return "VK_FORMAT_B8G8R8_UNORM";
            break;
        case VK_FORMAT_B8G8R8_SNORM:
            return "VK_FORMAT_B8G8R8_SNORM";
            break;
        case VK_FORMAT_B8G8R8_USCALED:
            return "VK_FORMAT_B8G8R8_USCALED";
            break;
        case VK_FORMAT_B8G8R8_SSCALED:
            return "VK_FORMAT_B8G8R8_SSCALED";
            break;
        case VK_FORMAT_B8G8R8_UINT:
            return "VK_FORMAT_B8G8R8_UINT";
            break;
        case VK_FORMAT_B8G8R8_SINT:
            return "VK_FORMAT_B8G8R8_SINT";
            break;
        case VK_FORMAT_B8G8R8_SRGB:
            return "VK_FORMAT_B8G8R8_SRGB";
            break;
        case VK_FORMAT_R8G8B8A8_UNORM:
            return "VK_FORMAT_R8G8B8A8_UNORM";
            break;
        case VK_FORMAT_R8G8B8A8_SNORM:
            return "VK_FORMAT_R8G8B8A8_SNORM";
            break;
        case VK_FORMAT_R8G8B8A8_USCALED:
            return "VK_FORMAT_R8G8B8A8_USCALED";
            break;
        case VK_FORMAT_R8G8B8A8_SSCALED:
            return "VK_FORMAT_R8G8B8A8_SSCALED";
            break;
        case VK_FORMAT_R8G8B8A8_UINT:
            return "VK_FORMAT_R8G8B8A8_UINT";
            break;
        case VK_FORMAT_R8G8B8A8_SINT:
            return "VK_FORMAT_R8G8B8A8_SINT";
            break;
        case VK_FORMAT_R8G8B8A8_SRGB:
            return "VK_FORMAT_R8G8B8A8_SRGB";
            break;
        case VK_FORMAT_B8G8R8A8_UNORM:
            return "VK_FORMAT_B8G8R8A8_UNORM";
            break;
        case VK_FORMAT_B8G8R8A8_SNORM:
            return "VK_FORMAT_B8G8R8A8_SNORM";
            break;
        case VK_FORMAT_B8G8R8A8_USCALED:
            return "VK_FORMAT_B8G8R8A8_USCALED";
            break;
        case VK_FORMAT_B8G8R8A8_SSCALED:
            return "VK_FORMAT_B8G8R8A8_SSCALED";
            break;
        case VK_FORMAT_B8G8R8A8_UINT:
            return "VK_FORMAT_B8G8R8A8_UINT";
            break;
        case VK_FORMAT_B8G8R8A8_SINT:
            return "VK_FORMAT_B8G8R8A8_SINT";
            break;
        case VK_FORMAT_B8G8R8A8_SRGB:
            return "VK_FORMAT_B8G8R8A8_SRGB";
            break;
        case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
            return "VK_FORMAT_A8B8G8R8_UNORM_PACK32";
            break;
        case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
            return "VK_FORMAT_A8B8G8R8_SNORM_PACK32";
            break;
        case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
            return "VK_FORMAT_A8B8G8R8_USCALED_PACK32";
            break;
        case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
            return "VK_FORMAT_A8B8G8R8_SSCALED_PACK32";
            break;
        case VK_FORMAT_A8B8G8R8_UINT_PACK32:
            return "VK_FORMAT_A8B8G8R8_UINT_PACK32";
            break;
        case VK_FORMAT_A8B8G8R8_SINT_PACK32:
            return "VK_FORMAT_A8B8G8R8_SINT_PACK32";
            break;
        case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
            return "VK_FORMAT_A8B8G8R8_SRGB_PACK32";
            break;
        case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
            return "VK_FORMAT_A2R10G10B10_UNORM_PACK32";
            break;
        case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
            return "VK_FORMAT_A2R10G10B10_SNORM_PACK32";
            break;
        case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
            return "VK_FORMAT_A2R10G10B10_USCALED_PACK32";
            break;
        case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
            return "VK_FORMAT_A2R10G10B10_SSCALED_PACK32";
            break;
        case VK_FORMAT_A2R10G10B10_UINT_PACK32:
            return "VK_FORMAT_A2R10G10B10_UINT_PACK32";
            break;
        case VK_FORMAT_A2R10G10B10_SINT_PACK32:
            return "VK_FORMAT_A2R10G10B10_SINT_PACK32";
            break;
        case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
            return "VK_FORMAT_A2B10G10R10_UNORM_PACK32";
            break;
        case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
            return "VK_FORMAT_A2B10G10R10_SNORM_PACK32";
            break;
        case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
            return "VK_FORMAT_A2B10G10R10_USCALED_PACK32";
            break;
        case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
            return "VK_FORMAT_A2B10G10R10_SSCALED_PACK32";
            break;
        case VK_FORMAT_A2B10G10R10_UINT_PACK32:
            return "VK_FORMAT_A2B10G10R10_UINT_PACK32";
            break;
        case VK_FORMAT_A2B10G10R10_SINT_PACK32:
            return "VK_FORMAT_A2B10G10R10_SINT_PACK32";
            break;
        case VK_FORMAT_R16_UNORM:
            return "VK_FORMAT_R16_UNORM";
            break;
        case VK_FORMAT_R16_SNORM:
            return "VK_FORMAT_R16_SNORM";
            break;
        case VK_FORMAT_R16_USCALED:
            return "VK_FORMAT_R16_USCALED";
            break;
        case VK_FORMAT_R16_SSCALED:
            return "VK_FORMAT_R16_SSCALED";
            break;
        case VK_FORMAT_R16_UINT:
            return "VK_FORMAT_R16_UINT";
            break;
        case VK_FORMAT_R16_SINT:
            return "VK_FORMAT_R16_SINT";
            break;
        case VK_FORMAT_R16_SFLOAT:
            return "VK_FORMAT_R16_SFLOAT";
            break;
        case VK_FORMAT_R16G16_UNORM:
            return "VK_FORMAT_R16G16_UNORM";
            break;
        case VK_FORMAT_R16G16_SNORM:
            return "VK_FORMAT_R16G16_SNORM";
            break;
        case VK_FORMAT_R16G16_USCALED:
            return "VK_FORMAT_R16G16_USCALED";
            break;
        case VK_FORMAT_R16G16_SSCALED:
            return "VK_FORMAT_R16G16_SSCALED";
            break;
        case VK_FORMAT_R16G16_UINT:
            return "VK_FORMAT_R16G16_UINT";
            break;
        case VK_FORMAT_R16G16_SINT:
            return "VK_FORMAT_R16G16_SINT";
            break;
        case VK_FORMAT_R16G16_SFLOAT:
            return "VK_FORMAT_R16G16_SFLOAT";
            break;
        case VK_FORMAT_R16G16B16_UNORM:
            return "VK_FORMAT_R16G16B16_UNORM";
            break;
        case VK_FORMAT_R16G16B16_SNORM:
            return "VK_FORMAT_R16G16B16_SNORM";
            break;
        case VK_FORMAT_R16G16B16_USCALED:
            return "VK_FORMAT_R16G16B16_USCALED";
            break;
        case VK_FORMAT_R16G16B16_SSCALED:
            return "VK_FORMAT_R16G16B16_SSCALED";
            break;
        case VK_FORMAT_R16G16B16_UINT:
            return "VK_FORMAT_R16G16B16_UINT";
            break;
        case VK_FORMAT_R16G16B16_SINT:
            return "VK_FORMAT_R16G16B16_SINT";
            break;
        case VK_FORMAT_R16G16B16_SFLOAT:
            return "VK_FORMAT_R16G16B16_SFLOAT";
            break;
        case VK_FORMAT_R16G16B16A16_UNORM:
            return "VK_FORMAT_R16G16B16A16_UNORM";
            break;
        case VK_FORMAT_R16G16B16A16_SNORM:
            return "VK_FORMAT_R16G16B16A16_SNORM";
            break;
        case VK_FORMAT_R16G16B16A16_USCALED:
            return "VK_FORMAT_R16G16B16A16_USCALED";
            break;
        case VK_FORMAT_R16G16B16A16_SSCALED:
            return "VK_FORMAT_R16G16B16A16_SSCALED";
            break;
        case VK_FORMAT_R16G16B16A16_UINT:
            return "VK_FORMAT_R16G16B16A16_UINT";
            break;
        case VK_FORMAT_R16G16B16A16_SINT:
            return "VK_FORMAT_R16G16B16A16_SINT";
            break;
        case VK_FORMAT_R16G16B16A16_SFLOAT:
            return "VK_FORMAT_R16G16B16A16_SFLOAT";
            break;
        case VK_FORMAT_R32_UINT:
            return "VK_FORMAT_R32_UINT";
            break;
        case VK_FORMAT_R32_SINT:
            return "VK_FORMAT_R32_SINT";
            break;
        case VK_FORMAT_R32_SFLOAT:
            return "VK_FORMAT_R32_SFLOAT";
            break;
        case VK_FORMAT_R32G32_UINT:
            return "VK_FORMAT_R32G32_UINT";
            break;
        case VK_FORMAT_R32G32_SINT:
            return "VK_FORMAT_R32G32_SINT";
            break;
        case VK_FORMAT_R32G32_SFLOAT:
            return "VK_FORMAT_R32G32_SFLOAT";
            break;
        case VK_FORMAT_R32G32B32_UINT:
            return "VK_FORMAT_R32G32B32_UINT";
            break;
        case VK_FORMAT_R32G32B32_SINT:
            return "VK_FORMAT_R32G32B32_SINT";
            break;
        case VK_FORMAT_R32G32B32_SFLOAT:
            return "VK_FORMAT_R32G32B32_SFLOAT";
            break;
        case VK_FORMAT_R32G32B32A32_UINT:
            return "VK_FORMAT_R32G32B32A32_UINT";
            break;
        case VK_FORMAT_R32G32B32A32_SINT:
            return "VK_FORMAT_R32G32B32A32_SINT";
            break;
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            return "VK_FORMAT_R32G32B32A32_SFLOAT";
            break;
        case VK_FORMAT_R64_UINT:
            return "VK_FORMAT_R64_UINT";
            break;
        case VK_FORMAT_R64_SINT:
            return "VK_FORMAT_R64_SINT";
            break;
        case VK_FORMAT_R64_SFLOAT:
            return "VK_FORMAT_R64_SFLOAT";
            break;
        case VK_FORMAT_R64G64_UINT:
            return "VK_FORMAT_R64G64_UINT";
            break;
        case VK_FORMAT_R64G64_SINT:
            return "VK_FORMAT_R64G64_SINT";
            break;
        case VK_FORMAT_R64G64_SFLOAT:
            return "VK_FORMAT_R64G64_SFLOAT";
            break;
        case VK_FORMAT_R64G64B64_UINT:
            return "VK_FORMAT_R64G64B64_UINT";
            break;
        case VK_FORMAT_R64G64B64_SINT:
            return "VK_FORMAT_R64G64B64_SINT";
            break;
        case VK_FORMAT_R64G64B64_SFLOAT:
            return "VK_FORMAT_R64G64B64_SFLOAT";
            break;
        case VK_FORMAT_R64G64B64A64_UINT:
            return "VK_FORMAT_R64G64B64A64_UINT";
            break;
        case VK_FORMAT_R64G64B64A64_SINT:
            return "VK_FORMAT_R64G64B64A64_SINT";
            break;
        case VK_FORMAT_R64G64B64A64_SFLOAT:
            return "VK_FORMAT_R64G64B64A64_SFLOAT";
            break;
        case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
            return "VK_FORMAT_B10G11R11_UFLOAT_PACK32";
            break;
        case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
            return "VK_FORMAT_E5B9G9R9_UFLOAT_PACK32";
            break;
        case VK_FORMAT_D16_UNORM:
            return "VK_FORMAT_D16_UNORM";
            break;
        case VK_FORMAT_X8_D24_UNORM_PACK32:
            return "VK_FORMAT_X8_D24_UNORM_PACK32";
            break;
        case VK_FORMAT_D32_SFLOAT:
            return "VK_FORMAT_D32_SFLOAT";
            break;
        case VK_FORMAT_S8_UINT:
            return "VK_FORMAT_S8_UINT";
            break;
        case VK_FORMAT_D16_UNORM_S8_UINT:
            return "VK_FORMAT_D16_UNORM_S8_UINT";
            break;
        case VK_FORMAT_D24_UNORM_S8_UINT:
            return "VK_FORMAT_D24_UNORM_S8_UINT";
            break;
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return "VK_FORMAT_D32_SFLOAT_S8_UINT";
            break;
        case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
            return "VK_FORMAT_BC1_RGB_UNORM_BLOCK";
            break;
        case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
            return "VK_FORMAT_BC1_RGB_SRGB_BLOCK";
            break;
        case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
            return "VK_FORMAT_BC1_RGBA_UNORM_BLOCK";
            break;
        case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
            return "VK_FORMAT_BC1_RGBA_SRGB_BLOCK";
            break;
        case VK_FORMAT_BC2_UNORM_BLOCK:
            return "VK_FORMAT_BC2_UNORM_BLOCK";
            break;
        case VK_FORMAT_BC2_SRGB_BLOCK:
            return "VK_FORMAT_BC2_SRGB_BLOCK";
            break;
        case VK_FORMAT_BC3_UNORM_BLOCK:
            return "VK_FORMAT_BC3_UNORM_BLOCK";
            break;
        case VK_FORMAT_BC3_SRGB_BLOCK:
            return "VK_FORMAT_BC3_SRGB_BLOCK";
            break;
        case VK_FORMAT_BC4_UNORM_BLOCK:
            return "VK_FORMAT_BC4_UNORM_BLOCK";
            break;
        case VK_FORMAT_BC4_SNORM_BLOCK:
            return "VK_FORMAT_BC4_SNORM_BLOCK";
            break;
        case VK_FORMAT_BC5_UNORM_BLOCK:
            return "VK_FORMAT_BC5_UNORM_BLOCK";
            break;
        case VK_FORMAT_BC5_SNORM_BLOCK:
            return "VK_FORMAT_BC5_SNORM_BLOCK";
            break;
        case VK_FORMAT_BC6H_UFLOAT_BLOCK:
            return "VK_FORMAT_BC6H_UFLOAT_BLOCK";
            break;
        case VK_FORMAT_BC6H_SFLOAT_BLOCK:
            return "VK_FORMAT_BC6H_SFLOAT_BLOCK";
            break;
        case VK_FORMAT_BC7_UNORM_BLOCK:
            return "VK_FORMAT_BC7_UNORM_BLOCK";
            break;
        case VK_FORMAT_BC7_SRGB_BLOCK:
            return "VK_FORMAT_BC7_SRGB_BLOCK";
            break;
        case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
            return "VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK";
            break;
        case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
            return "VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK";
            break;
        case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
            return "VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK";
            break;
        case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
            return "VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK";
            break;
        case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
            return "VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK";
            break;
        case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
            return "VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK";
            break;
        case VK_FORMAT_EAC_R11_UNORM_BLOCK:
            return "VK_FORMAT_EAC_R11_UNORM_BLOCK";
            break;
        case VK_FORMAT_EAC_R11_SNORM_BLOCK:
            return "VK_FORMAT_EAC_R11_SNORM_BLOCK";
            break;
        case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
            return "VK_FORMAT_EAC_R11G11_UNORM_BLOCK";
            break;
        case VK_FORMAT_EAC_R11G11_SNORM_BLOCK:
            return "VK_FORMAT_EAC_R11G11_SNORM_BLOCK";
            break;
        case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_4x4_UNORM_BLOCK";
            break;
        case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_4x4_SRGB_BLOCK";
            break;
        case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_5x4_UNORM_BLOCK";
            break;
        case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_5x4_SRGB_BLOCK";
            break;
        case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_5x5_UNORM_BLOCK";
            break;
        case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_5x5_SRGB_BLOCK";
            break;
        case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_6x5_UNORM_BLOCK";
            break;
        case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_6x5_SRGB_BLOCK";
            break;
        case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_6x6_UNORM_BLOCK";
            break;
        case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_6x6_SRGB_BLOCK";
            break;
        case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_8x5_UNORM_BLOCK";
            break;
        case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_8x5_SRGB_BLOCK";
            break;
        case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_8x6_UNORM_BLOCK";
            break;
        case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_8x6_SRGB_BLOCK";
            break;
        case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_8x8_UNORM_BLOCK";
            break;
        case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_8x8_SRGB_BLOCK";
            break;
        case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_10x5_UNORM_BLOCK";
            break;
        case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_10x5_SRGB_BLOCK";
            break;
        case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_10x6_UNORM_BLOCK";
            break;
        case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_10x6_SRGB_BLOCK";
            break;
        case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_10x8_UNORM_BLOCK";
            break;
        case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_10x8_SRGB_BLOCK";
            break;
        case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_10x10_UNORM_BLOCK";
            break;
        case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_10x10_SRGB_BLOCK";
            break;
        case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_12x10_UNORM_BLOCK";
            break;
        case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_12x10_SRGB_BLOCK";
            break;
        case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
            return "VK_FORMAT_ASTC_12x12_UNORM_BLOCK";
            break;
        case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:
            return "VK_FORMAT_ASTC_12x12_SRGB_BLOCK";
            break;
        case VK_FORMAT_G8B8G8R8_422_UNORM:
            return "VK_FORMAT_G8B8G8R8_422_UNORM";
            break;
        case VK_FORMAT_B8G8R8G8_422_UNORM:
            return "VK_FORMAT_B8G8R8G8_422_UNORM";
            break;
        case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
            return "VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM";
            break;
        case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:
            return "VK_FORMAT_G8_B8R8_2PLANE_420_UNORM";
            break;
        case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM:
            return "VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM";
            break;
        case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM:
            return "VK_FORMAT_G8_B8R8_2PLANE_422_UNORM";
            break;
        case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM:
            return "VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM";
            break;
        case VK_FORMAT_R10X6_UNORM_PACK16:
            return "VK_FORMAT_R10X6_UNORM_PACK16";
            break;
        case VK_FORMAT_R10X6G10X6_UNORM_2PACK16:
            return "VK_FORMAT_R10X6G10X6_UNORM_2PACK16";
            break;
        case VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16:
            return "VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16";
            break;
        case VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16:
            return "VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16";
            break;
        case VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16:
            return "VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16";
            break;
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
            return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16";
            break;
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
            return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16";
            break;
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
            return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16";
            break;
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
            return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16";
            break;
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
            return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16";
            break;
        case VK_FORMAT_R12X4_UNORM_PACK16:
            return "VK_FORMAT_R12X4_UNORM_PACK16";
            break;
        case VK_FORMAT_R12X4G12X4_UNORM_2PACK16:
            return "VK_FORMAT_R12X4G12X4_UNORM_2PACK16";
            break;
        case VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16:
            return "VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16";
            break;
        case VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16:
            return "VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16";
            break;
        case VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16:
            return "VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16";
            break;
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
            return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16";
            break;
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
            return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16";
            break;
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
            return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16";
            break;
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
            return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16";
            break;
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
            return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16";
            break;
        case VK_FORMAT_G16B16G16R16_422_UNORM:
            return "VK_FORMAT_G16B16G16R16_422_UNORM";
            break;
        case VK_FORMAT_B16G16R16G16_422_UNORM:
            return "VK_FORMAT_B16G16R16G16_422_UNORM";
            break;
        case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM:
            return "VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM";
            break;
        case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM:
            return "VK_FORMAT_G16_B16R16_2PLANE_420_UNORM";
            break;
        case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM:
            return "VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM";
            break;
        case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM:
            return "VK_FORMAT_G16_B16R16_2PLANE_422_UNORM";
            break;
        case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM:
            return "VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM";
            break;
        case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG:
            return "VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG";
            break;
        case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG:
            return "VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG";
            break;
        case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG:
            return "VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG";
            break;
        case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG:
            return "VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG";
            break;
        case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG:
            return "VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG";
            break;
        case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG:
            return "VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG";
            break;
        case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG:
            return "VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG";
            break;
        case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG:
            return "VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG";
            break;
        case VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT";
            break;
        case VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT";
            break;
        case VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT";
            break;
        case VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT";
            break;
        case VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT";
            break;
        case VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT";
            break;
        case VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT";
            break;
        case VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT";
            break;
        case VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT";
            break;
        case VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT";
            break;
        case VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT";
            break;
        case VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT";
            break;
        case VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT";
            break;
        case VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT:
            return "VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT";
            break;
        case VK_FORMAT_G8_B8R8_2PLANE_444_UNORM_EXT:
            return "VK_FORMAT_G8_B8R8_2PLANE_444_UNORM_EXT";
            break;
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT:
            return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT";
            break;
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT:
            return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT";
            break;
        case VK_FORMAT_G16_B16R16_2PLANE_444_UNORM_EXT:
            return "VK_FORMAT_G16_B16R16_2PLANE_444_UNORM_EXT";
            break;
        case VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT:
            return "VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT";
            break;
        case VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT:
            return "VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT";
            break;
        default:
            return "VK_FORMAT_UNDEFINED";
    }
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

typedef std::vector<VkQueueFamilyProperties> ArrayOfVkQueueFamilyProperties;
typedef std::unordered_map<uint32_t /*VkFormat*/, VkFormatProperties> ArrayOfVkFormatProperties;
typedef std::vector<VkLayerProperties> ArrayOfVkLayerProperties;
typedef std::vector<VkExtensionProperties> ArrayOfVkExtensionProperties;
typedef std::vector<VkSurfaceFormatKHR> ArrayOfVkSurfaceFormats;
typedef std::vector<VkPresentModeKHR> ArrayOfVkPresentModes;
typedef std::vector<VkDeviceSize> ArrayOfVkDeviceSize;

// FormatProperties utilities ////////////////////////////////////////////////////////////////////////////////////////////////////

// This is the JSON representation of VkFormat property data, as defined by the Devsim schema.
// It will be split to create a VkFormat value and a VkFormatProperties structure after loading from JSON.
struct DevsimFormatProperties {
    VkFormat formatID;
    VkFormatFeatureFlags linearTilingFeatures;
    VkFormatFeatureFlags optimalTilingFeatures;
    VkFormatFeatureFlags bufferFeatures;
};

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
        assert(!Find(pd));                        // Verify this instance does not already exist.
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

    static bool HasExtension(VkPhysicalDevice pd, const char *extension_name) { return HasExtension(Find(pd), extension_name); }

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
            case ARRAY_COMBINATION_MODE_NONE:
                simulation_extensions_ = device_extensions_;
                break;
            case ARRAY_COMBINATION_MODE_REPLACE:
                simulation_extensions_ = arrayof_extension_properties_;
                break;
            case ARRAY_COMBINATION_MODE_WHITELIST:
                for (VkExtensionProperties dev_props : device_extensions_) {
                    for (VkExtensionProperties file_props : arrayof_extension_properties_) {
                        if (strncmp(dev_props.extensionName, file_props.extensionName, VK_MAX_EXTENSION_NAME_SIZE) == 0) {
                            simulation_extensions_.push_back(dev_props);
                            break;
                        }
                    }
                }
                break;
            case ARRAY_COMBINATION_MODE_BLACKLIST:
                for (VkExtensionProperties dev_props : device_extensions_) {
                    bool black_listed = false;
                    for (VkExtensionProperties file_props : arrayof_extension_properties_) {
                        if (strncmp(dev_props.extensionName, file_props.extensionName, VK_MAX_EXTENSION_NAME_SIZE) == 0) {
                            black_listed = true;
                            break;
                        }
                    }
                    if (!black_listed) simulation_extensions_.push_back(dev_props);
                }
                break;
            case ARRAY_COMBINATION_MODE_INTERSECT:
                for (VkExtensionProperties dev_props : device_extensions_) {
                    simulation_extensions_.push_back(dev_props);
                }

                for (VkExtensionProperties file_props : arrayof_extension_properties_) {
                    bool intersection = false;
                    for (VkExtensionProperties dev_props : device_extensions_) {
                        if (strncmp(dev_props.extensionName, file_props.extensionName, VK_MAX_EXTENSION_NAME_SIZE) == 0) {
                            intersection = true;
                            break;
                        }
                    }
                    if (!intersection) simulation_extensions_.push_back(file_props);
                }
                break;
            default:
                simulation_extensions_ = device_extensions_;
        }

        return static_cast<uint32_t>(simulation_extensions_.size());
    }

    VkInstance instance() const { return instance_; }

    ArrayOfVkExtensionProperties device_extensions_;

    ArrayCombinationMode extension_list_combination_mode_;
    ArrayOfVkExtensionProperties simulation_extensions_;

    ArrayCombinationMode format_list_combination_mode_;
    ArrayCombinationMode format_properties_combination_mode_;

    VkPhysicalDeviceProperties physical_device_properties_;
    VkPhysicalDeviceFeatures physical_device_features_;
    VkPhysicalDeviceMemoryProperties physical_device_memory_properties_;
    VkSurfaceCapabilitiesKHR surface_capabilities_;
    ArrayOfVkPresentModes arrayof_present_modes_;
    ArrayOfVkQueueFamilyProperties arrayof_queue_family_properties_;
    ArrayOfVkFormatProperties arrayof_format_properties_;
    ArrayOfVkLayerProperties arrayof_layer_properties_;
    ArrayOfVkExtensionProperties arrayof_extension_properties_;
    ArrayOfVkSurfaceFormats arrayof_surface_formats_;
    ArrayOfVkDeviceSize arrayof_heap_budgets_;
    ArrayOfVkDeviceSize arrayof_heap_usages_;

    // Vulkan 1.2 structs for summarizing core extension properties and features
    VkPhysicalDeviceVulkan11Properties physical_device_vulkan_1_1_properties_;
    VkPhysicalDeviceVulkan11Features physical_device_vulkan_1_1_features_;

    VkPhysicalDeviceVulkan12Properties physical_device_vulkan_1_2_properties_;
    VkPhysicalDeviceVulkan12Features physical_device_vulkan_1_2_features_;

    // Vulkan 1.1 structs
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

    // VK_KHR_maintenance2 structs
    VkPhysicalDevicePointClippingPropertiesKHR physical_device_point_clipping_properties_;

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

   private:
    PhysicalDeviceData() = delete;
    PhysicalDeviceData &operator=(const PhysicalDeviceData &) = delete;
    PhysicalDeviceData(VkInstance instance) : instance_(instance) {
        extension_list_combination_mode_ = ARRAY_COMBINATION_MODE_NONE;

        format_list_combination_mode_ = ARRAY_COMBINATION_MODE_NONE;
        format_properties_combination_mode_ = ARRAY_COMBINATION_MODE_NONE;

        physical_device_properties_ = {};
        physical_device_features_ = {};
        physical_device_memory_properties_ = {};
        surface_capabilities_ = {};

        // Vulkan 1.2 structs for summarizing core extension properties and features
        physical_device_vulkan_1_1_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES};
        physical_device_vulkan_1_1_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES};

        physical_device_vulkan_1_2_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES};
        physical_device_vulkan_1_2_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};

        // Vulkan 1.1 structs
        physical_device_protected_memory_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_PROPERTIES};
        physical_device_protected_memory_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES};

        physical_device_shader_draw_parameters_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES};

        physical_device_subgroup_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES};

        // VK_KHR_8bit_storage structs
        physical_device_8bit_storage_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR};

        // VK_KHR_16bit_storage structs
        physical_device_16bit_storage_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES_KHR};

        // VK_KHR_buffer_device_address structs
        physical_device_buffer_device_address_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR};

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

        // VK_KHR_maintenance2 structs
        physical_device_point_clipping_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_POINT_CLIPPING_PROPERTIES_KHR};

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
        physical_device_workgroup_memory_explicit_layout_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_FEATURES_KHR};

        // VK_EXT_4444_formats structs
        physical_device_4444_formats_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_4444_FORMATS_FEATURES_EXT};

        // VK_EXT_astc_decode_mode structs
        physical_device_astc_decode_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ASTC_DECODE_FEATURES_EXT};

        // VK_EXT_blend_operation_advanced structs
        physical_device_blend_operation_advanced_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT};
        physical_device_blend_operation_advanced_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_PROPERTIES_EXT};

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
        physical_device_external_memory_host_properties_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_HOST_PROPERTIES_EXT};

        // VK_EXT_fragment_density_map structs
        physical_device_fragment_density_map_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_FEATURES_EXT};
        physical_device_fragment_density_map_properties_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_PROPERTIES_EXT};

        // VK_EXT_fragment_shader_interlock structs
        physical_device_fragment_shader_interlock_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT};

        // VK_EXT_global_priority_query structs
        physical_device_global_priority_query_features_ = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GLOBAL_PRIORITY_QUERY_FEATURES_EXT};

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
        physical_device_pageable_device_local_memory_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PAGEABLE_DEVICE_LOCAL_MEMORY_FEATURES_EXT};

        // VK_EXT_pipeline_creation_cache_control structs
        physical_device_pipeline_creation_cache_control_features_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES_EXT};

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
        physical_device_texel_buffer_alignment_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_PROPERTIES_EXT};

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
        physical_device_fragment_shading_rate_properties_ = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR};

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
    }

    const VkInstance instance_;

    typedef std::unordered_map<VkPhysicalDevice, PhysicalDeviceData> Map;
    static Map map_;
};

PhysicalDeviceData::Map PhysicalDeviceData::map_;

// Loader for DevSim JSON configuration files ////////////////////////////////////////////////////////////////////////////////////

class JsonLoader {
   public:
    JsonLoader(PhysicalDeviceData &pdd) : pdd_(pdd) {}
    JsonLoader() = delete;
    JsonLoader(const JsonLoader &) = delete;
    JsonLoader &operator=(const JsonLoader &) = delete;

    bool LoadFiles();
    bool LoadFiles(const char *filename_list);
    bool LoadFile(const char *filename);

   private:
    enum class SchemaId {
        kUnknown = 0,
        kDevsim100,
        kDevsim110,
        kDevsim120,
        kDevsim8BitStorageKHR,
        kDevsim16BitStorageKHR,
        kDevsimBufferDeviceAddressKHR,
        kDevsimDepthStencilResolveKHR,
        kDevsimDescriptorIndexingEXT,
        kDevsimHostQueryResetEXT,
        kDevsimImagelessFramebufferKHR,
        kDevsimMaintenance2KHR,
        kDevsimMaintenance3KHR,
        kDevsimMultiviewKHR,
        kDevsimPortabilitySubsetKHR,
        kDevsimSamplerFilterMinmaxEXT,
        kDevsimSamplerYcbcrConversionKHR,
        kDevsimScalarBlockLayoutEXT,
        kDevsimSeparateDepthStencilLayoutsKHR,
        kDevsimShaderAtomicInt64KHR,
        kDevsimShaderFloatControlsKHR,
        kDevsimShaderFloat16Int8KHR,
        kDevsimShaderSubgroupExtendedTypesKHR,
        kDevsimTimelineSemaphoreKHR,
        kDevsimUniformBufferStandardLayoutKHR,
        kDevsimVariablePointersKHR,
        kDevsimVulkanMemoryModelKHR
    };

    SchemaId IdentifySchema(const Json::Value &value);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceProperties *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceDepthStencilResolveProperties *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceDescriptorIndexingPropertiesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceFloatControlsPropertiesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceMaintenance3PropertiesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceMaintenance4FeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceMaintenance4PropertiesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceMultiviewPropertiesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDevicePointClippingPropertiesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDevicePortabilitySubsetPropertiesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceProtectedMemoryProperties *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceTimelineSemaphorePropertiesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceLimits *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceSparseProperties *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceSubgroupProperties *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceFeatures *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDevice8BitStorageFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDevice16BitStorageFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceBufferDeviceAddressFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceDescriptorIndexingFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceHostQueryResetFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceImagelessFramebufferFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceMultiviewFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDevicePortabilitySubsetFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceProtectedMemoryFeatures *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceScalarBlockLayoutFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderAtomicInt64FeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderDrawParametersFeatures *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderFloat16Int8FeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceTimelineSemaphoreFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceVariablePointersFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceVulkanMemoryModelFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceAccelerationStructureFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceAccelerationStructurePropertiesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDevicePerformanceQueryFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDevicePerformanceQueryPropertiesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDevicePresentIdFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDevicePresentWaitFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDevicePushDescriptorPropertiesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceRayQueryFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceRayTracingPipelineFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceRayTracingPipelinePropertiesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderClockFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceSynchronization2FeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDevice4444FormatsFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceASTCDecodeFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceBorderColorSwizzleFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceColorWriteEnableFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceConditionalRenderingFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceConservativeRasterizationPropertiesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceCustomBorderColorFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceCustomBorderColorPropertiesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceDepthClipEnableFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceDeviceMemoryReportFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceDiscardRectanglePropertiesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceExtendedDynamicStateFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceExtendedDynamicState2FeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceExternalMemoryHostPropertiesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceFragmentDensityMapFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceFragmentDensityMapPropertiesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceImageRobustnessFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceIndexTypeUint8FeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceInlineUniformBlockFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceInlineUniformBlockPropertiesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceLineRasterizationFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceLineRasterizationPropertiesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceMemoryPriorityFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceMultiDrawFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceMultiDrawPropertiesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDevicePrivateDataFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceProvokingVertexFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceProvokingVertexPropertiesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceRobustness2FeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceRobustness2PropertiesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceSampleLocationsPropertiesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderAtomicFloatFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceSubgroupSizeControlFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceSubgroupSizeControlPropertiesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceTransformFeedbackFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceTransformFeedbackPropertiesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceYcbcrImageArraysFeaturesEXT *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceFragmentShadingRateFeaturesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceFragmentShadingRatePropertiesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceCoherentMemoryFeaturesAMD *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderCorePropertiesAMD *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderCoreProperties2AMD *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceInvocationMaskFeaturesHUAWEI *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceSubpassShadingFeaturesHUAWEI *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceSubpassShadingPropertiesHUAWEI *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceComputeShaderDerivativesFeaturesNV *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceCooperativeMatrixFeaturesNV *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceCooperativeMatrixPropertiesNV *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceCornerSampledImageFeaturesNV *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceCoverageReductionModeFeaturesNV *dest);
    void GetValue(const Json::Value &parent, int index, VkMemoryType *dest);
    void GetValue(const Json::Value &parent, int index, VkMemoryHeap *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceMemoryProperties *dest);
    void GetValue(const Json::Value &parent, const char *name, VkSurfaceCapabilitiesKHR *dest);
    void GetValue(const Json::Value &parent, const char *name, VkExtent2D *dest);
    void GetValue(const Json::Value &parent, const char *name, VkExtent3D *dest);
    void GetValue(const Json::Value &parent, int index, VkQueueFamilyProperties *dest);
    void GetValue(const Json::Value &parent, int index, DevsimFormatProperties *dest);
    void GetValue(const Json::Value &parent, int index, VkLayerProperties *dest);
    void GetValue(const Json::Value &parent, int index, VkExtensionProperties *dest);
    void GetValue(const Json::Value &parent, int index, VkSurfaceFormatKHR *dest);
    void GetValue(const Json::Value &parent, int index, VkPresentModeKHR *dest);
    void GetValue(const Json::Value &parent, int index, VkPhysicalDevice *dest);
    void GetValue(const Json::Value &parent, int index, VkDeviceSize *dest);

    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceVulkan12Properties *dest);
    void GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceVulkan12Features *dest);

    // GPUinfo config file format
    void GetValueGPUinfoCore11(const Json::Value &parent);
    void GetValueGPUinfoCore12(const Json::Value &parent);
    void GetValueGPUinfoSurfaceCapabilities(const Json::Value &parent);

    // For use as warn_func in GET_VALUE_WARN().  Return true if warning occurred.
    static bool WarnIfGreater(const char *name, const uint64_t new_value, const uint64_t old_value) {
        if (new_value > old_value) {
            DebugPrintf("WARN \"%s\" JSON value (%" PRIu64 ") is greater than existing value (%" PRIu64 ")\n", name, new_value,
                        old_value);
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

    void GetValue(const Json::Value &parent, const char *name, float *dest,
                  std::function<bool(const char *, float, float)> warn_func = nullptr) {
        const Json::Value value = parent[name];
        if (!value.isDouble()) {
            return;
        }
        const float new_value = value.asFloat();
        if (warn_func) {
            warn_func(name, new_value, *dest);
        }
        *dest = new_value;
    }

    void GetValue(const Json::Value &parent, const char *name, int32_t *dest,
                  std::function<bool(const char *, int32_t, int32_t)> warn_func = nullptr) {
        const Json::Value value = parent[name];
        if (!value.isInt()) {
            return;
        }
        const uint32_t new_value = value.asInt();
        if (warn_func) {
            warn_func(name, new_value, *dest);
        }
        *dest = new_value;
    }

    void GetValue(const Json::Value &parent, const char *name, uint32_t *dest,
                  std::function<bool(const char *, uint32_t, uint32_t)> warn_func = nullptr) {
        const Json::Value value = parent[name];
        if (!value.isUInt()) {
            return;
        }
        const uint32_t new_value = value.asUInt();
        if (warn_func) {
            warn_func(name, new_value, *dest);
        }
        *dest = new_value;
    }

    void GetValue(const Json::Value &parent, const char *name, uint64_t *dest,
                  std::function<bool(const char *, uint64_t, uint64_t)> warn_func = nullptr) {
        const Json::Value value = parent[name];
        if (!value.isUInt64()) {
            return;
        }
        const uint64_t new_value = value.asUInt64();
        if (warn_func) {
            warn_func(name, new_value, *dest);
        }
        *dest = new_value;
    }

    void GetValue(const Json::Value &parent, const char *name, int64_t *dest,
                  std::function<bool(const char *, int64_t, int64_t)> warn_func = nullptr) {
        const Json::Value value = parent[name];
        if (!value.isInt64()) {
            return;
        }
        const int64_t new_value = value.asInt64();
        if (warn_func) {
            warn_func(name, new_value, *dest);
        }
        *dest = new_value;
    }

    void GetValue(const Json::Value &parent, const char *name, uint8_t *dest,
                  std::function<bool(const char *, uint8_t, uint8_t)> warn_func = nullptr) {
        const Json::Value value = parent[name];
        if (!value.isUInt()) {
            return;
        }
        const uint8_t new_value = value.asUInt();
        if (warn_func) {
            warn_func(name, new_value, *dest);
        }
        *dest = new_value;
    }

    template <typename T>  // for Vulkan enum types
    void GetValue(const Json::Value &parent, const char *name, T *dest,
                  std::function<bool(const char *, T, T)> warn_func = nullptr) {
        const Json::Value value = parent[name];
        if (!value.isInt()) {
            return;
        }
        const T new_value = static_cast<T>(value.asInt());
        if (warn_func) {
            warn_func(name, new_value, *dest);
        }
        *dest = new_value;
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

    int GetArray(const Json::Value &parent, const char *name, ArrayOfVkQueueFamilyProperties *dest) {
        const Json::Value value = parent[name];
        if (value.type() != Json::arrayValue) {
            return -1;
        }
        DebugPrintf("\t\tJsonLoader::GetArray(ArrayOfVkQueueFamilyProperties)\n");
        dest->clear();
        const int count = static_cast<int>(value.size());
        for (int i = 0; i < count; ++i) {
            VkQueueFamilyProperties queue_family_properties = {};
            GetValue(value, i, &queue_family_properties);
            dest->push_back(queue_family_properties);
        }
        return static_cast<int>(dest->size());
    }

    int GetArray(const Json::Value &parent, const char *name, ArrayOfVkFormatProperties *dest) {
        const Json::Value value = parent[name];
        if (value.type() != Json::arrayValue) {
            return -1;
        }
        DebugPrintf("\t\tJsonLoader::GetArray(ArrayOfVkFormatProperties)\n");
        dest->clear();
        const int count = static_cast<int>(value.size());
        for (int i = 0; i < count; ++i) {
            // Get a format structure from JSON.
            DevsimFormatProperties devsim_format_properties = {};
            GetValue(value, i, &devsim_format_properties);
            // Split the JSON-acquired data into VkFormat and VkFormatProperties.
            const VkFormat format = devsim_format_properties.formatID;
            VkFormatProperties vk_format_properties = {};
            vk_format_properties.linearTilingFeatures = devsim_format_properties.linearTilingFeatures;
            vk_format_properties.optimalTilingFeatures = devsim_format_properties.optimalTilingFeatures;
            vk_format_properties.bufferFeatures = devsim_format_properties.bufferFeatures;
            if (IsFormatSupported(vk_format_properties)) {
                dest->insert({format, vk_format_properties});
            }
        }
        return static_cast<int>(dest->size());
    }

    int GetArray(const Json::Value &parent, const char *name, ArrayOfVkLayerProperties *dest) {
        const Json::Value value = parent[name];
        if (value.type() != Json::arrayValue) {
            return -1;
        }
        DebugPrintf("\t\tJsonLoader::GetArray(ArrayOfVkLayerProperties)\n");
        dest->clear();
        const int count = static_cast<int>(value.size());
        for (int i = 0; i < count; ++i) {
            VkLayerProperties layer_properties = {};
            GetValue(value, i, &layer_properties);
            dest->push_back(layer_properties);
        }
        return static_cast<int>(dest->size());
    }

    int GetArray(const Json::Value &parent, const char *name, ArrayOfVkExtensionProperties *dest) {
        const Json::Value value = parent[name];
        if (value.type() != Json::arrayValue) {
            return -1;
        }
        DebugPrintf("\t\tJsonLoader::GetArray(ArrayOfVkExtensionProperties)\n");
        dest->clear();
        const int count = static_cast<int>(value.size());
        for (int i = 0; i < count; ++i) {
            VkExtensionProperties extension_properties = {};
            GetValue(value, i, &extension_properties);
            dest->push_back(extension_properties);
        }
        return static_cast<int>(dest->size());
    }

    int GetArray(const Json::Value &parent, const char *name, ArrayOfVkSurfaceFormats *dest) {
        const Json::Value value = parent[name];
        if (value.type() != Json::arrayValue) {
            return -1;
        }
        DebugPrintf("\t\tJsonLoader::GetArray(ArrayOfVkSurfaceFormats)\n");
        dest->clear();
        const int count = static_cast<int>(value.size());
        for (int i = 0; i < count; ++i) {
            VkSurfaceFormatKHR surface_format = {};
            GetValue(value, i, &surface_format);
            dest->push_back(surface_format);
        }
        return static_cast<int>(dest->size());
    }

    int GetArray(const Json::Value &parent, const char *name, ArrayOfVkPresentModes *dest) {
        const Json::Value value = parent[name];
        if (value.type() != Json::arrayValue) {
            return -1;
        }
        DebugPrintf("\t\tJsonLoader::GetArray(ArrayOfVkPresentModes)\n");
        dest->clear();
        const int count = static_cast<int>(value.size());
        for (int i = 0; i < count; ++i) {
            VkPresentModeKHR present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
            GetValue(value, i, &present_mode);
            dest->push_back(present_mode);
        }
        return static_cast<int>(dest->size());
    }

    int GetArray(const Json::Value &parent, const char *name, ArrayOfVkDeviceSize *dest) {
        const Json::Value value = parent[name];
        if (value.type() != Json::arrayValue) {
            return -1;
        }
        DebugPrintf("\t\tJsonLoader::GetArray(ArrayOfVkDeviceSize)\n");
        dest->clear();
        const int count = static_cast<int>(value.size());
        for (int i = 0; i < count; ++i) {
            VkDeviceSize device_size = {};
            GetValue(value, i, &device_size);
            dest->push_back(device_size);
        }
        return static_cast<int>(dest->size());
    }

    void WarnDeprecated(const Json::Value &parent, const char *name) {
        const Json::Value value = parent[name];
        if (value.type() != Json::nullValue) {
            DebugPrintf("WARN JSON section %s is deprecated and ignored.\n", name);
        }
    }

    PhysicalDeviceData &pdd_;
};

bool JsonLoader::LoadFiles() {
    if (inputFilename.str.empty()) {
        ErrorPrintf("envar %s and %s in vk_layer_settings.txt are unset\n", kEnvarDevsimFilename, kLayerSettingsDevsimFilename);
        return false;
    }

    const char *filename_list = inputFilename.str.c_str();
    if (inputFilename.fromEnvVar) {
        DebugPrintf("envar %s = \"%s\"\n", kEnvarDevsimFilename, filename_list);
    } else {
        DebugPrintf("vk_layer_settings.txt setting %s = \"%s\"\n", kLayerSettingsDevsimFilename, filename_list);
    }
    return LoadFiles(filename_list);
}

bool JsonLoader::LoadFiles(const char *filename_list) {
#if defined(_WIN32)
    const char delimiter = ';';
#else
    const char delimiter = ':';
#endif
    std::stringstream ss_list(filename_list);
    std::string filename;

    while (std::getline(ss_list, filename, delimiter)) {
        if (!filename.empty()) {
            if (!LoadFile(filename.c_str())) {
                return false;
            }
        }
    }
    return true;
}

bool JsonLoader::LoadFile(const char *filename) {
    std::ifstream json_file(filename);
    if (!json_file) {
        ErrorPrintf("JsonLoader failed to open file \"%s\"\n", filename);
        return false;
    }

    DebugPrintf("JsonLoader::LoadFile(\"%s\")\n", filename);
    Json::Reader reader;
    Json::Value root = Json::nullValue;
    bool success = reader.parse(json_file, root, false);
    if (!success) {
        ErrorPrintf("Json::Reader failed {\n%s}\n", reader.getFormattedErrorMessages().c_str());
        return false;
    }
    json_file.close();

    if (root.type() != Json::objectValue) {
        ErrorPrintf("Json document root is not an object\n");
        return false;
    }

    DebugPrintf("{\n");
    bool result = false;
    const Json::Value schema_value = root["$schema"];
    const SchemaId schema_id = IdentifySchema(schema_value);
    if (schema_id == SchemaId::kUnknown) {
        DebugPrintf("}\n");
        return result;
    }

    result = true;

    GetValue(root, "VkPhysicalDeviceProperties", &pdd_.physical_device_properties_);
    GetValue(root, "VkPhysicalDeviceDepthStencilResolveProperties", &pdd_.physical_device_depth_stencil_resolve_properties_);
    GetValue(root, "VkPhysicalDeviceDepthStencilResolvePropertiesKHR", &pdd_.physical_device_depth_stencil_resolve_properties_);
    GetValue(root, "VkPhysicalDeviceSubgroupProperties", &pdd_.physical_device_subgroup_properties_);
    GetValue(root, "VkPhysicalDeviceDescriptorIndexingProperties", &pdd_.physical_device_descriptor_indexing_properties_);
    GetValue(root, "VkPhysicalDeviceDescriptorIndexingPropertiesEXT", &pdd_.physical_device_descriptor_indexing_properties_);
    GetValue(root, "VkPhysicalDeviceFloatControlsProperties", &pdd_.physical_device_float_controls_properties_);
    GetValue(root, "VkPhysicalDeviceFloatControlsPropertiesKHR", &pdd_.physical_device_float_controls_properties_);
    GetValue(root, "VkPhysicalDeviceHostQueryResetFeatures", &pdd_.physical_device_host_query_reset_features_);
    GetValue(root, "VkPhysicalDeviceHostQueryResetFeaturesEXT", &pdd_.physical_device_host_query_reset_features_);
    GetValue(root, "VkPhysicalDeviceMaintenance3Properties", &pdd_.physical_device_maintenance_3_properties_);
    GetValue(root, "VkPhysicalDeviceMaintenance3PropertiesKHR", &pdd_.physical_device_maintenance_3_properties_);
    GetValue(root, "VkPhysicalDeviceMaintenance4FeaturesKHR", &pdd_.physical_device_maintenance_4_features_);
    GetValue(root, "VkPhysicalDeviceMaintenance4PropertiesKHR", &pdd_.physical_device_maintenance_4_properties_);
    GetValue(root, "VkPhysicalDeviceMultiviewProperties", &pdd_.physical_device_multiview_properties_);
    GetValue(root, "VkPhysicalDeviceMultiviewPropertiesKHR", &pdd_.physical_device_multiview_properties_);
    GetValue(root, "VkPhysicalDevicePointClippingProperties", &pdd_.physical_device_point_clipping_properties_);
    GetValue(root, "VkPhysicalDevicePointClippingPropertiesKHR", &pdd_.physical_device_point_clipping_properties_);
    GetValue(root, "VkPhysicalDeviceProtectedMemoryProperties", &pdd_.physical_device_protected_memory_properties_);
    GetValue(root, "VkPhysicalDeviceTimelineSemaphoreProperties", &pdd_.physical_device_timeline_semaphore_properties_);
    GetValue(root, "VkPhysicalDeviceTimelineSemaphorePropertiesKHR", &pdd_.physical_device_timeline_semaphore_properties_);
    GetValue(root, "VkPhysicalDeviceFeatures", &pdd_.physical_device_features_);
    GetValue(root, "VkPhysicalDevice16BitStorageFeatures", &pdd_.physical_device_16bit_storage_features_);
    GetValue(root, "VkPhysicalDevice16BitStorageFeaturesKHR", &pdd_.physical_device_16bit_storage_features_);
    GetValue(root, "VkPhysicalDevice8BitStorageFeatures", &pdd_.physical_device_8bit_storage_features_);
    GetValue(root, "VkPhysicalDevice8BitStorageFeaturesKHR", &pdd_.physical_device_8bit_storage_features_);
    GetValue(root, "VkPhysicalDeviceBufferDeviceAddressFeatures", &pdd_.physical_device_buffer_device_address_features_);
    GetValue(root, "VkPhysicalDeviceBufferDeviceAddressFeaturesKHR", &pdd_.physical_device_buffer_device_address_features_);
    GetValue(root, "VkPhysicalDeviceDescriptorIndexingFeatures", &pdd_.physical_device_descriptor_indexing_features_);
    GetValue(root, "VkPhysicalDeviceDescriptorIndexingFeaturesEXT", &pdd_.physical_device_descriptor_indexing_features_);
    GetValue(root, "VkPhysicalDeviceImagelessFramebufferFeatures", &pdd_.physical_device_imageless_framebuffer_features_);
    GetValue(root, "VkPhysicalDeviceImagelessFramebufferFeaturesKHR", &pdd_.physical_device_imageless_framebuffer_features_);
    GetValue(root, "VkPhysicalDeviceMultiviewFeatures", &pdd_.physical_device_multiview_features_);
    GetValue(root, "VkPhysicalDeviceMultiviewFeaturesKHR", &pdd_.physical_device_multiview_features_);
    GetValue(root, "VkPhysicalDeviceProtectedMemoryFeatures", &pdd_.physical_device_protected_memory_features_);
    GetValue(root, "VkPhysicalDeviceSamplerFilterMinmaxProperties", &pdd_.physical_device_sampler_filter_minmax_properties_);
    GetValue(root, "VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT", &pdd_.physical_device_sampler_filter_minmax_properties_);
    GetValue(root, "VkPhysicalDeviceSamplerYcbcrConversionFeatures", &pdd_.physical_device_sampler_ycbcr_conversion_features_);
    GetValue(root, "VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR", &pdd_.physical_device_sampler_ycbcr_conversion_features_);
    GetValue(root, "VkPhysicalDeviceScalarBlockLayoutFeatures", &pdd_.physical_device_scalar_block_layout_features_);
    GetValue(root, "VkPhysicalDeviceScalarBlockLayoutFeaturesEXT", &pdd_.physical_device_scalar_block_layout_features_);
    GetValue(root, "VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures",
             &pdd_.physical_device_separate_depth_stencil_layouts_features_);
    GetValue(root, "VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR",
             &pdd_.physical_device_separate_depth_stencil_layouts_features_);
    GetValue(root, "VkPhysicalDeviceShaderAtomicInt64Features", &pdd_.physical_device_shader_atomic_int64_features_);
    GetValue(root, "VkPhysicalDeviceShaderAtomicInt64FeaturesKHR", &pdd_.physical_device_shader_atomic_int64_features_);
    GetValue(root, "VkPhysicalDeviceShaderDrawParametersFeatures", &pdd_.physical_device_shader_draw_parameters_features_);
    GetValue(root, "VkPhysicalDeviceShaderFloat16Int8Features", &pdd_.physical_device_shader_float16_int8_features_);
    GetValue(root, "VkPhysicalDeviceShaderFloat16Int8FeaturesKHR", &pdd_.physical_device_shader_float16_int8_features_);
    GetValue(root, "VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures",
             &pdd_.physical_device_shader_subgroup_extended_types_features_);
    GetValue(root, "VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR",
             &pdd_.physical_device_shader_subgroup_extended_types_features_);
    GetValue(root, "VkPhysicalDeviceTimelineSemaphoreFeatures", &pdd_.physical_device_timeline_semaphore_features_);
    GetValue(root, "VkPhysicalDeviceTimelineSemaphoreFeaturesKHR", &pdd_.physical_device_timeline_semaphore_features_);
    GetValue(root, "VkPhysicalDeviceUniformBufferStandardLayoutFeatures",
             &pdd_.physical_device_uniform_buffer_standard_layout_features_);
    GetValue(root, "VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR",
             &pdd_.physical_device_uniform_buffer_standard_layout_features_);
    GetValue(root, "VkPhysicalDeviceVariablePointersFeatures", &pdd_.physical_device_variable_pointers_features_);
    GetValue(root, "VkPhysicalDeviceVariablePointersFeaturesKHR", &pdd_.physical_device_variable_pointers_features_);
    GetValue(root, "VkPhysicalDeviceVulkanMemoryModelFeatures", &pdd_.physical_device_vulkan_memory_model_features_);
    GetValue(root, "VkPhysicalDeviceVulkanMemoryModelFeaturesKHR", &pdd_.physical_device_vulkan_memory_model_features_);
    GetValue(root, "VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR",
             &pdd_.physical_device_zero_initialize_workgroup_memory_features_);
    GetValue(root, "VkPhysicalDeviceAccelerationStructureFeaturesKHR", &pdd_.physical_device_acceleration_structure_features_);
    GetValue(root, "VkPhysicalDeviceAccelerationStructurePropertiesKHR", &pdd_.physical_device_acceleration_structure_properties_);
    GetValue(root, "VkPhysicalDevicePerformanceQueryFeaturesKHR", &pdd_.physical_device_performance_query_features_);
    GetValue(root, "VkPhysicalDevicePerformanceQueryPropertiesKHR", &pdd_.physical_device_performance_query_properties_);
    GetValue(root, "VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR",
             &pdd_.physical_device_pipeline_executable_properties_features_);
    GetValue(root, "VkPhysicalDevicePresentIdFeaturesKHR", &pdd_.physical_device_present_id_features_);
    GetValue(root, "VkPhysicalDevicePresentWaitFeaturesKHR", &pdd_.physical_device_present_wait_features_);
    GetValue(root, "VkPhysicalDevicePushDescriptorPropertiesKHR", &pdd_.physical_device_push_descriptor_properites_);
    GetValue(root, "VkPhysicalDeviceRayQueryFeaturesKHR", &pdd_.physical_device_ray_query_features_);
    GetValue(root, "VkPhysicalDeviceRayTracingPipelineFeaturesKHR", &pdd_.physical_device_ray_tracing_pipeline_features_);
    GetValue(root, "VkPhysicalDeviceRayTracingPipelinePropertiesKHR", &pdd_.physical_device_ray_tracing_pipeline_properties_);
    GetValue(root, "VkPhysicalDeviceShaderClockFeaturesKHR", &pdd_.physical_device_shader_clock_features_);
    GetValue(root, "VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR",
             &pdd_.physical_device_shader_integer_dot_product_features_);
    GetValue(root, "VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR",
             &pdd_.physical_device_shader_integer_dot_products_properties_);
    GetValue(root, "VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR",
             &pdd_.physical_device_shader_subgroup_uniform_control_flow_features_);
    GetValue(root, "VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR",
             &pdd_.physical_device_shader_terminate_invocation_features_);
    GetValue(root, "VkPhysicalDeviceSynchronization2FeaturesKHR", &pdd_.physical_device_synchronization2_features_);
    GetValue(root, "VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR",
             &pdd_.physical_device_workgroup_memory_explicit_layout_features_);
    GetValue(root, "VkPhysicalDevice4444FormatsFeaturesEXT", &pdd_.physical_device_4444_formats_features_);
    GetValue(root, "VkPhysicalDeviceASTCDecodeFeaturesEXT", &pdd_.physical_device_astc_decode_features_);
    GetValue(root, "VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT", &pdd_.physical_device_blend_operation_advanced_features_);
    GetValue(root, "VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT",
             &pdd_.physical_device_blend_operation_advanced_properties_);
    GetValue(root, "VkPhysicalDeviceBorderColorSwizzleFeaturesEXT", &pdd_.physical_device_border_color_swizzle_features_);
    GetValue(root, "VkPhysicalDeviceColorWriteEnableFeaturesEXT", &pdd_.physical_device_color_write_enable_features_);
    GetValue(root, "VkPhysicalDeviceConditionalRenderingFeaturesEXT", &pdd_.physical_device_conditional_rendering_features_);
    GetValue(root, "VkPhysicalDeviceConservativeRasterizationPropertiesEXT",
             &pdd_.physical_device_conservative_rasterization_properties_);
    GetValue(root, "VkPhysicalDeviceCustomBorderColorFeaturesEXT", &pdd_.physical_device_custom_border_color_features_);
    GetValue(root, "VkPhysicalDeviceCustomBorderColorPropertiesEXT", &pdd_.physical_device_custom_border_color_properties_);
    GetValue(root, "VkPhysicalDeviceDepthClipEnableFeaturesEXT", &pdd_.physical_device_depth_clip_enable_features_ext_);
    GetValue(root, "VkPhysicalDeviceDeviceMemoryReportFeaturesEXT", &pdd_.physical_device_device_memory_report_features_);
    GetValue(root, "VkPhysicalDeviceDiscardRectanglePropertiesEXT", &pdd_.physical_device_discard_rectangle_properties_);
    GetValue(root, "VkPhysicalDeviceExtendedDynamicStateFeaturesEXT", &pdd_.physical_device_extended_dynamic_state_features_);
    GetValue(root, "VkPhysicalDeviceExtendedDynamicState2FeaturesEXT", &pdd_.physical_device_extended_dynamic_state2_features_);
    GetValue(root, "VkPhysicalDeviceExternalMemoryHostPropertiesEXT", &pdd_.physical_device_external_memory_host_properties_);
    GetValue(root, "VkPhysicalDeviceFragmentDensityMapFeaturesEXT", &pdd_.physical_device_fragment_density_map_features_);
    GetValue(root, "VkPhysicalDeviceFragmentDensityMapPropertiesEXT", &pdd_.physical_device_fragment_density_map_properties_);
    GetValue(root, "VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT", &pdd_.physical_device_fragment_shader_interlock_features_);
    GetValue(root, "VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT", &pdd_.physical_device_global_priority_query_features_);
    GetValue(root, "VkPhysicalDeviceImageRobustnessFeaturesEXT", &pdd_.physical_device_image_robustness_features_);
    GetValue(root, "VkPhysicalDeviceIndexTypeUint8FeaturesEXT", &pdd_.physical_device_index_type_uint8_features_);
    GetValue(root, "VkPhysicalDeviceInlineUniformBlockFeaturesEXT", &pdd_.physical_device_inline_uniform_block_features_);
    GetValue(root, "VkPhysicalDeviceInlineUniformBlockPropertiesEXT", &pdd_.physical_device_inline_uniform_block_properties_);
    GetValue(root, "VkPhysicalDeviceLineRasterizationFeaturesEXT", &pdd_.physical_device_line_rasterization_features_);
    GetValue(root, "VkPhysicalDeviceLineRasterizationPropertiesEXT", &pdd_.physical_device_line_rasterization_properties_);
    GetValue(root, "VkPhysicalDeviceMemoryPriorityFeaturesEXT", &pdd_.physical_device_memory_priority_features_);
    GetValue(root, "VkPhysicalDeviceMultiDrawFeaturesEXT", &pdd_.physical_device_multi_draw_features_);
    GetValue(root, "VkPhysicalDeviceMultiDrawPropertiesEXT", &pdd_.physical_device_multi_draw_properties_);
    GetValue(root, "VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT",
             &pdd_.physical_device_pageable_device_local_memory_features_);
    GetValue(root, "VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT",
             &pdd_.physical_device_pipeline_creation_cache_control_features_);
    GetValue(root, "VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT",
             &pdd_.physical_device_primitive_topology_list_restart_features_);
    GetValue(root, "VkPhysicalDevicePrivateDataFeaturesEXT", &pdd_.physical_device_private_data_features_);
    GetValue(root, "VkPhysicalDeviceProvokingVertexFeaturesEXT", &pdd_.physical_device_provoking_vertex_features_);
    GetValue(root, "VkPhysicalDeviceProvokingVertexPropertiesEXT", &pdd_.physical_device_provoking_vertex_properties_);
    GetValue(root, "VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT", &pdd_.physical_device_rgba10x6_formats_features_);
    GetValue(root, "VkPhysicalDeviceRobustness2FeaturesEXT", &pdd_.physical_device_robustness_2_features_);
    GetValue(root, "VkPhysicalDeviceRobustness2PropertiesEXT", &pdd_.physical_device_robustness_2_properties_);
    GetValue(root, "VkPhysicalDeviceSampleLocationsPropertiesEXT", &pdd_.physical_device_sample_locations_properties_);
    GetValue(root, "VkPhysicalDeviceShaderAtomicFloatFeaturesEXT", &pdd_.physical_device_shader_atomic_float_features_);
    GetValue(root, "VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT", &pdd_.physical_device_shader_atomic_float2_features_);
    GetValue(root, "VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT",
             &pdd_.physical_device_shader_demote_to_helper_invocation_features_);
    GetValue(root, "VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT", &pdd_.physical_device_shader_image_atomic_int64_features_);
    GetValue(root, "VkPhysicalDeviceSubgroupSizeControlFeaturesEXT", &pdd_.physical_device_subgroup_size_control_features_);
    GetValue(root, "VkPhysicalDeviceSubgroupSizeControlPropertiesEXT", &pdd_.physical_device_subgroup_size_control_properties_);
    GetValue(root, "VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT", &pdd_.physical_device_texel_buffer_alignment_features_);
    GetValue(root, "VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT", &pdd_.physical_device_texel_buffer_alignment_properties_);
    GetValue(root, "VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT",
             &pdd_.physical_device_texture_compression_astc_hdr_features_);
    GetValue(root, "VkPhysicalDeviceTransformFeedbackFeaturesEXT", &pdd_.physical_device_transform_feedback_features_);
    GetValue(root, "VkPhysicalDeviceTransformFeedbackPropertiesEXT", &pdd_.physical_device_transform_feedback_properties_);
    GetValue(root, "VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT", &pdd_.physical_device_vertex_attribute_divisor_features_);
    GetValue(root, "VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT",
             &pdd_.physical_device_vertex_attirbute_divisor_properties_);
    GetValue(root, "VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT",
             &pdd_.physical_device_vertex_input_dynamic_state_features_);
    GetValue(root, "VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT", &pdd_.physical_device_ycbcr_2plane_444_formats_features_);
    GetValue(root, "VkPhysicalDeviceYcbcrImageArraysFeaturesEXT", &pdd_.physical_device_ycbcr_image_arrays_features_);
    GetValue(root, "VkPhysicalDeviceFragmentShadingRateFeaturesKHR", &pdd_.physical_device_fragment_shading_rate_features_);
    GetValue(root, "VkPhysicalDeviceFragmentShadingRatePropertiesKHR", &pdd_.physical_device_fragment_shading_rate_properties_);
    GetValue(root, "VkPhysicalDeviceCoherentMemoryFeaturesAMD", &pdd_.physical_device_coherent_memory_features_);
    GetValue(root, "VkPhysicalDeviceShaderCorePropertiesAMD", &pdd_.physical_device_shader_core_properties_);
    GetValue(root, "VkPhysicalDeviceShaderCoreProperties2AMD", &pdd_.physical_device_shader_core_properties_2_);
    GetValue(root, "VkPhysicalDeviceInvocationMaskFeaturesHUAWEI", &pdd_.physical_device_invocation_mask_features_);
    GetValue(root, "VkPhysicalDeviceSubpassShadingFeaturesHUAWEI", &pdd_.physical_device_subpass_shading_features_);
    GetValue(root, "VkPhysicalDeviceSubpassShadingPropertiesHUAWEI", &pdd_.physical_device_subpass_shading_properties_);
    GetValue(root, "VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL",
             &pdd_.physical_device_shader_integer_functions_2_features_);
    GetValue(root, "VkPhysicalDeviceComputeShaderDerivativesFeaturesNV",
             &pdd_.physical_device_compute_shader_derivatives_features_);
    GetValue(root, "VkPhysicalDeviceCooperativeMatrixFeaturesNV", &pdd_.physical_device_cooperative_matrix_features_);
    GetValue(root, "VkPhysicalDeviceCooperativeMatrixPropertiesNV", &pdd_.physical_device_cooperative_matrix_properties_);
    GetValue(root, "VkPhysicalDeviceCornerSampledImageFeaturesNV", &pdd_.physical_device_corner_sampled_image_features_);
    GetValue(root, "VkPhysicalDeviceCoverageReductionModeFeaturesNV", &pdd_.physical_device_coverage_reduction_mode_features_);
    GetValue(root, "VkPhysicalDeviceMemoryProperties", &pdd_.physical_device_memory_properties_);
    GetValue(root, "VkSurfaceCapabilitiesKHR", &pdd_.surface_capabilities_);
    GetArray(root, "ArrayOfVkQueueFamilyProperties", &pdd_.arrayof_queue_family_properties_);
    GetArray(root, "ArrayOfVkFormatProperties", &pdd_.arrayof_format_properties_);
    GetArray(root, "ArrayOfVkLayerProperties", &pdd_.arrayof_layer_properties_);
    GetArray(root, "ArrayOfVkExtensionProperties", &pdd_.arrayof_extension_properties_);
    GetArray(root, "ArrayOfVkSurfaceFormats", &pdd_.arrayof_surface_formats_);
    GetArray(root, "ArrayOfVkPresentModes", &pdd_.arrayof_present_modes_);

    GetValue(root, "Vulkan12Features", &pdd_.physical_device_vulkan_1_2_features_);
    GetValue(root, "Vulkan12Properties", &pdd_.physical_device_vulkan_1_2_properties_);

    GetValue(root, "VkPhysicalDevicePortabilitySubsetPropertiesKHR", &pdd_.physical_device_portability_subset_properties_);
    GetValue(root, "VkPhysicalDevicePortabilitySubsetFeaturesKHR", &pdd_.physical_device_portability_subset_features_);

    // GPUinfo structures
    GetValueGPUinfoCore11(root);
    GetValueGPUinfoCore12(root);
    GetValueGPUinfoSurfaceCapabilities(root);

    DebugPrintf("}\n");

    return result;
}

JsonLoader::SchemaId JsonLoader::IdentifySchema(const Json::Value &value) {
    if (!value.isString()) {
        ErrorPrintf("JSON element \"$schema\" is not a string\n");
        return SchemaId::kUnknown;
    }

    SchemaId schema_id = SchemaId::kUnknown;
    const char *schema_string = value.asCString();
    if (strcmp(schema_string, "https://schema.khronos.org/vulkan/devsim_1_0_0.json#") == 0) {
        schema_id = SchemaId::kDevsim100;
    } else if (strcmp(schema_string, "https://schema.khronos.org/vulkan/devsim_1_1_0.json#") == 0) {
        schema_id = SchemaId::kDevsim110;
    } else if (strcmp(schema_string, "https://schema.khronos.org/vulkan/devsim_1_2_0.json#") == 0) {
        schema_id = SchemaId::kDevsim120;
    } else if (strcmp(schema_string, "https://schema.khronos.org/vulkan/devsim_VK_KHR_8bit_storage_1.json#") == 0) {
        schema_id = SchemaId::kDevsim8BitStorageKHR;
    } else if (strcmp(schema_string, "https://schema.khronos.org/vulkan/devsim_VK_KHR_16bit_storage_1.json#") == 0) {
        schema_id = SchemaId::kDevsim16BitStorageKHR;
    } else if (strcmp(schema_string, "https://schema.khronos.org/vulkan/devsim_VK_KHR_buffer_device_address_1.json#") == 0) {
        schema_id = SchemaId::kDevsimBufferDeviceAddressKHR;
    } else if (strcmp(schema_string, "https://schema.khronos.org/vulkan/devsim_VK_KHR_depth_stencil_resolve_1.json#") == 0) {
        schema_id = SchemaId::kDevsimDepthStencilResolveKHR;
    } else if (strcmp(schema_string, "https://schema.khronos.org/vulkan/devsim_VK_EXT_descriptor_indexing_1.json#") == 0) {
        schema_id = SchemaId::kDevsimDescriptorIndexingEXT;
    } else if (strcmp(schema_string, "https://schema.khronos.org/vulkan/devsim_VK_EXT_host_query_reset_1.json#") == 0) {
        schema_id = SchemaId::kDevsimHostQueryResetEXT;
    } else if (strcmp(schema_string, "https://schema.khronos.org/vulkan/devsim_VK_KHR_imageless_framebuffer_1.json#") == 0) {
        schema_id = SchemaId::kDevsimImagelessFramebufferKHR;
    } else if (strcmp(schema_string, "https://schema.khronos.org/vulkan/devsim_VK_KHR_maintenance2_1.json#") == 0) {
        schema_id = SchemaId::kDevsimMaintenance2KHR;
    } else if (strcmp(schema_string, "https://schema.khronos.org/vulkan/devsim_VK_KHR_maintenance3_1.json#") == 0) {
        schema_id = SchemaId::kDevsimMaintenance3KHR;
    } else if (strcmp(schema_string, "https://schema.khronos.org/vulkan/devsim_VK_KHR_multiview_1.json#") == 0) {
        schema_id = SchemaId::kDevsimMultiviewKHR;
    } else if (strcmp(schema_string, "https://schema.khronos.org/vulkan/devsim_VK_KHR_portability_subset-provisional-1.json#") ==
               0) {
        schema_id = SchemaId::kDevsimPortabilitySubsetKHR;
    } else if (strcmp(schema_string, "https://schema.khronos.org/vulkan/devsim_VK_EXT_sampler_filter_minmax_1.json#") == 0) {
        schema_id = SchemaId::kDevsimSamplerFilterMinmaxEXT;
    } else if (strcmp(schema_string, "https://schema.khronos.org/vulkan/devsim_VK_KHR_sampler_ycbcr_conversion_1.json#") == 0) {
        schema_id = SchemaId::kDevsimSamplerYcbcrConversionKHR;
    } else if (strcmp(schema_string, "https://schema.khronos.org/vulkan/devsim_VK_EXT_scalar_block_layout_1.json#") == 0) {
        schema_id = SchemaId::kDevsimScalarBlockLayoutEXT;
    } else if (strcmp(schema_string, "https://schema.khronos.org/vulkan/devsim_VK_KHR_separate_depth_stencil_layouts_1.json#") ==
               0) {
        schema_id = SchemaId::kDevsimSeparateDepthStencilLayoutsKHR;
    } else if (strcmp(schema_string, "https://schema.khronos.org/vulkan/devsim_VK_KHR_shader_atomic_int64_1.json#") == 0) {
        schema_id = SchemaId::kDevsimShaderAtomicInt64KHR;
    } else if (strcmp(schema_string, "https://schema.khronos.org/vulkan/devsim_VK_KHR_shader_float_controls_1.json#") == 0) {
        schema_id = SchemaId::kDevsimShaderFloatControlsKHR;
    } else if (strcmp(schema_string, "https://schema.khronos.org/vulkan/devsim_VK_KHR_shader_float16_int8_1.json#") == 0) {
        schema_id = SchemaId::kDevsimShaderFloat16Int8KHR;
    } else if (strcmp(schema_string, "https://schema.khronos.org/vulkan/devsim_VK_KHR_shader_subgroup_extended_types_1.json#") ==
               0) {
        schema_id = SchemaId::kDevsimShaderSubgroupExtendedTypesKHR;
    } else if (strcmp(schema_string, "https://schema.khronos.org/vulkan/devsim_VK_KHR_timeline_semaphore_1.json#") == 0) {
        schema_id = SchemaId::kDevsimTimelineSemaphoreKHR;
    } else if (strcmp(schema_string, "https://schema.khronos.org/vulkan/devsim_VK_KHR_uniform_buffer_standard_layout_1.json#") ==
               0) {
        schema_id = SchemaId::kDevsimUniformBufferStandardLayoutKHR;
    } else if (strcmp(schema_string, "https://schema.khronos.org/vulkan/devsim_VK_KHR_variable_pointers_1.json#") == 0) {
        schema_id = SchemaId::kDevsimVariablePointersKHR;
    } else if (strcmp(schema_string, "https://schema.khronos.org/vulkan/devsim_VK_KHR_vulkan_memory_model_1.json#") == 0) {
        schema_id = SchemaId::kDevsimVulkanMemoryModelKHR;
    }

    if (schema_id != SchemaId::kUnknown) {
        DebugPrintf("\tDocument schema \"%s\" is schema_id %d\n", schema_string, schema_id);
    } else {
        ErrorPrintf("Document schema \"%s\" not supported by %s\n", schema_string, kOurLayerName);
    }
    return schema_id;
}

// Apply the DRY principle, see https://en.wikipedia.org/wiki/Don%27t_repeat_yourself
#define GET_VALUE(name) GetValue(value, #name, &dest->name)
#define GET_ARRAY(name) GetArray(value, #name, dest->name)
#define GET_VALUE_WARN(name, warn_func) GetValue(value, #name, &dest->name, warn_func)

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceProperties *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceProperties)\n");
    GET_VALUE(apiVersion);
    GET_VALUE(driverVersion);
    GET_VALUE(vendorID);
    GET_VALUE(deviceID);
    GET_VALUE(deviceType);
    GET_ARRAY(deviceName);         // size < VK_MAX_PHYSICAL_DEVICE_NAME_SIZE
    GET_ARRAY(pipelineCacheUUID);  // size == VK_UUID_SIZE
    GET_VALUE(limits);
    GET_VALUE(sparseProperties);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceDepthStencilResolveProperties *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceDepthStencilResolveProperties)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_depth_stencil_resolve, but "
            "VK_KHR_depth_stencil_resolve is "
            "not supported by the device.\n");
    }
    GET_VALUE(supportedDepthResolveModes);
    GET_VALUE(supportedStencilResolveModes);
    GET_VALUE_WARN(independentResolveNone, WarnIfGreater);
    GET_VALUE_WARN(independentResolve, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceDescriptorIndexingPropertiesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceDescriptorIndexingPropertiesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_descriptor_indexing, but "
            "VK_EXT_descriptor_indexing is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(maxUpdateAfterBindDescriptorsInAllPools, WarnIfGreater);
    GET_VALUE_WARN(shaderUniformBufferArrayNonUniformIndexingNative, WarnIfGreater);
    GET_VALUE_WARN(shaderSampledImageArrayNonUniformIndexingNative, WarnIfGreater);
    GET_VALUE_WARN(shaderStorageBufferArrayNonUniformIndexingNative, WarnIfGreater);
    GET_VALUE_WARN(shaderStorageImageArrayNonUniformIndexingNative, WarnIfGreater);
    GET_VALUE_WARN(shaderInputAttachmentArrayNonUniformIndexingNative, WarnIfGreater);
    GET_VALUE_WARN(robustBufferAccessUpdateAfterBind, WarnIfGreater);
    GET_VALUE_WARN(quadDivergentImplicitLod, WarnIfGreater);
    GET_VALUE_WARN(maxPerStageDescriptorUpdateAfterBindSamplers, WarnIfGreater);
    GET_VALUE_WARN(maxPerStageDescriptorUpdateAfterBindUniformBuffers, WarnIfGreater);
    GET_VALUE_WARN(maxPerStageDescriptorUpdateAfterBindStorageBuffers, WarnIfGreater);
    GET_VALUE_WARN(maxPerStageDescriptorUpdateAfterBindSampledImages, WarnIfGreater);
    GET_VALUE_WARN(maxPerStageDescriptorUpdateAfterBindStorageImages, WarnIfGreater);
    GET_VALUE_WARN(maxPerStageDescriptorUpdateAfterBindInputAttachments, WarnIfGreater);
    GET_VALUE_WARN(maxPerStageUpdateAfterBindResources, WarnIfGreater);
    GET_VALUE_WARN(maxDescriptorSetUpdateAfterBindSamplers, WarnIfGreater);
    GET_VALUE_WARN(maxDescriptorSetUpdateAfterBindUniformBuffers, WarnIfGreater);
    GET_VALUE_WARN(maxDescriptorSetUpdateAfterBindUniformBuffersDynamic, WarnIfGreater);
    GET_VALUE_WARN(maxDescriptorSetUpdateAfterBindStorageBuffers, WarnIfGreater);
    GET_VALUE_WARN(maxDescriptorSetUpdateAfterBindStorageBuffersDynamic, WarnIfGreater);
    GET_VALUE_WARN(maxDescriptorSetUpdateAfterBindSampledImages, WarnIfGreater);
    GET_VALUE_WARN(maxDescriptorSetUpdateAfterBindStorageImages, WarnIfGreater);
    GET_VALUE_WARN(maxDescriptorSetUpdateAfterBindInputAttachments, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceFloatControlsPropertiesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceFloatControlsPropertiesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_shader_float_controls, but "
            "VK_KHR_shader_float_controls is "
            "not supported by the device.\n");
    }
    GET_VALUE(denormBehaviorIndependence);
    GET_VALUE(roundingModeIndependence);
    GET_VALUE_WARN(shaderSignedZeroInfNanPreserveFloat16, WarnIfGreater);
    GET_VALUE_WARN(shaderSignedZeroInfNanPreserveFloat32, WarnIfGreater);
    GET_VALUE_WARN(shaderSignedZeroInfNanPreserveFloat64, WarnIfGreater);
    GET_VALUE_WARN(shaderDenormPreserveFloat16, WarnIfGreater);
    GET_VALUE_WARN(shaderDenormPreserveFloat32, WarnIfGreater);
    GET_VALUE_WARN(shaderDenormPreserveFloat64, WarnIfGreater);
    GET_VALUE_WARN(shaderDenormFlushToZeroFloat16, WarnIfGreater);
    GET_VALUE_WARN(shaderDenormFlushToZeroFloat32, WarnIfGreater);
    GET_VALUE_WARN(shaderDenormFlushToZeroFloat64, WarnIfGreater);
    GET_VALUE_WARN(shaderRoundingModeRTEFloat16, WarnIfGreater);
    GET_VALUE_WARN(shaderRoundingModeRTEFloat32, WarnIfGreater);
    GET_VALUE_WARN(shaderRoundingModeRTEFloat64, WarnIfGreater);
    GET_VALUE_WARN(shaderRoundingModeRTZFloat16, WarnIfGreater);
    GET_VALUE_WARN(shaderRoundingModeRTZFloat32, WarnIfGreater);
    GET_VALUE_WARN(shaderRoundingModeRTZFloat64, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceMaintenance3PropertiesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceMaintenance3PropertiesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_MAINTENANCE3_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_maintenance3, but VK_KHR_maintenance3 is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(maxPerSetDescriptors, WarnIfGreater);
    GET_VALUE_WARN(maxMemoryAllocationSize, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceMaintenance4FeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceMaintenance4FeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_MAINTENANCE_4_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_maintenance4, but VK_KHR_maintenance4 is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(maintenance4, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceMaintenance4PropertiesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceMaintenance4PropertiesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_MAINTENANCE_4_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_maintenance4, but VK_KHR_maintenance4 is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(maxBufferSize, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceMultiviewPropertiesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceMultiviewPropertiesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_MULTIVIEW_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_multiview, but VK_KHR_multiview is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(maxMultiviewViewCount, WarnIfGreater);
    GET_VALUE_WARN(maxMultiviewInstanceIndex, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDevicePointClippingPropertiesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePointClippingPropertiesKHR)\n");
    DebugPrintf("WARN VkPhysicalDevicePointClippingProperties only reports how a device functions and will not be set.");
    // GET_VALUE(pointClippingBehavior);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDevicePortabilitySubsetPropertiesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePortabilitySubsetPropertiesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME) && emulatePortability.num <= 0) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_portability_subset, but VK_KHR_portability_subset is "
            "not supported by the device and emulation is not turned on.\nIf you wish to emulate "
            "VK_KHR_portability_subset, please set environment variable %s to 1.\n",
            kEnvarDevsimEmulatePortability);
    }
    GET_VALUE_WARN(minVertexInputBindingStrideAlignment, WarnIfLesser);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceProtectedMemoryProperties *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceProtectedMemoryProperties)\n");
    GET_VALUE_WARN(protectedNoFault, WarnIfLesser);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_sampler_filter_minmax, but "
            "VK_EXT_sampler_filter_minmax is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(filterMinmaxSingleComponentFormats, WarnIfGreater);
    GET_VALUE_WARN(filterMinmaxImageComponentMapping, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceTimelineSemaphorePropertiesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceTimelineSemaphorePropertiesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_timeline_semaphore, but "
            "VK_KHR_timeline_semaphore is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(maxTimelineSemaphoreValueDifference, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceLimits *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    GET_VALUE(maxImageDimension1D);
    GET_VALUE(maxImageDimension2D);
    GET_VALUE(maxImageDimension3D);
    GET_VALUE(maxImageDimensionCube);
    GET_VALUE(maxImageArrayLayers);
    GET_VALUE(maxTexelBufferElements);
    GET_VALUE(maxUniformBufferRange);
    GET_VALUE(maxStorageBufferRange);
    GET_VALUE(maxPushConstantsSize);
    GET_VALUE(maxMemoryAllocationCount);
    GET_VALUE(maxSamplerAllocationCount);
    GET_VALUE(bufferImageGranularity);
    GET_VALUE(sparseAddressSpaceSize);
    GET_VALUE_WARN(maxBoundDescriptorSets, WarnIfGreater);
    GET_VALUE_WARN(maxPerStageDescriptorSamplers, WarnIfGreater);
    GET_VALUE_WARN(maxPerStageDescriptorUniformBuffers, WarnIfGreater);
    GET_VALUE_WARN(maxPerStageDescriptorStorageBuffers, WarnIfGreater);
    GET_VALUE_WARN(maxPerStageDescriptorSampledImages, WarnIfGreater);
    GET_VALUE_WARN(maxPerStageDescriptorStorageImages, WarnIfGreater);
    GET_VALUE_WARN(maxPerStageDescriptorInputAttachments, WarnIfGreater);
    GET_VALUE_WARN(maxPerStageResources, WarnIfGreater);
    GET_VALUE_WARN(maxDescriptorSetSamplers, WarnIfGreater);
    GET_VALUE_WARN(maxDescriptorSetUniformBuffers, WarnIfGreater);
    GET_VALUE_WARN(maxDescriptorSetUniformBuffersDynamic, WarnIfGreater);
    GET_VALUE_WARN(maxDescriptorSetStorageBuffers, WarnIfGreater);
    GET_VALUE_WARN(maxDescriptorSetStorageBuffersDynamic, WarnIfGreater);
    GET_VALUE_WARN(maxDescriptorSetSampledImages, WarnIfGreater);
    GET_VALUE_WARN(maxDescriptorSetStorageImages, WarnIfGreater);
    GET_VALUE_WARN(maxDescriptorSetInputAttachments, WarnIfGreater);
    GET_VALUE(maxVertexInputAttributes);
    GET_VALUE(maxVertexInputBindings);
    GET_VALUE(maxVertexInputAttributeOffset);
    GET_VALUE(maxVertexInputBindingStride);
    GET_VALUE(maxVertexOutputComponents);
    GET_VALUE(maxTessellationGenerationLevel);
    GET_VALUE(maxTessellationPatchSize);
    GET_VALUE(maxTessellationControlPerVertexInputComponents);
    GET_VALUE(maxTessellationControlPerVertexOutputComponents);
    GET_VALUE(maxTessellationControlPerPatchOutputComponents);
    GET_VALUE(maxTessellationControlTotalOutputComponents);
    GET_VALUE(maxTessellationEvaluationInputComponents);
    GET_VALUE(maxTessellationEvaluationOutputComponents);
    GET_VALUE(maxGeometryShaderInvocations);
    GET_VALUE(maxGeometryInputComponents);
    GET_VALUE(maxGeometryOutputComponents);
    GET_VALUE(maxGeometryOutputVertices);
    GET_VALUE(maxGeometryTotalOutputComponents);
    GET_VALUE(maxFragmentInputComponents);
    GET_VALUE(maxFragmentOutputAttachments);
    GET_VALUE(maxFragmentDualSrcAttachments);
    GET_VALUE(maxFragmentCombinedOutputResources);
    GET_VALUE(maxComputeSharedMemorySize);
    GET_ARRAY(maxComputeWorkGroupCount);  // size == 3
    GET_VALUE(maxComputeWorkGroupInvocations);
    GET_ARRAY(maxComputeWorkGroupSize);  // size == 3
    GET_VALUE(subPixelPrecisionBits);
    GET_VALUE(subTexelPrecisionBits);
    GET_VALUE(mipmapPrecisionBits);
    GET_VALUE(maxDrawIndexedIndexValue);
    GET_VALUE(maxDrawIndirectCount);
    GET_VALUE(maxSamplerLodBias);
    GET_VALUE(maxSamplerAnisotropy);
    GET_VALUE(maxViewports);
    GET_ARRAY(maxViewportDimensions);  // size == 2
    GET_ARRAY(viewportBoundsRange);    // size == 2
    GET_VALUE(viewportSubPixelBits);
    GET_VALUE(minMemoryMapAlignment);
    GET_VALUE(minTexelBufferOffsetAlignment);
    GET_VALUE(minUniformBufferOffsetAlignment);
    GET_VALUE(minStorageBufferOffsetAlignment);
    GET_VALUE(minTexelOffset);
    GET_VALUE(maxTexelOffset);
    GET_VALUE(minTexelGatherOffset);
    GET_VALUE(maxTexelGatherOffset);
    GET_VALUE(minInterpolationOffset);
    GET_VALUE(maxInterpolationOffset);
    GET_VALUE(subPixelInterpolationOffsetBits);
    GET_VALUE(maxFramebufferWidth);
    GET_VALUE(maxFramebufferHeight);
    GET_VALUE(maxFramebufferLayers);
    GET_VALUE(framebufferColorSampleCounts);
    GET_VALUE(framebufferDepthSampleCounts);
    GET_VALUE(framebufferStencilSampleCounts);
    GET_VALUE(framebufferNoAttachmentsSampleCounts);
    GET_VALUE(maxColorAttachments);
    GET_VALUE(sampledImageColorSampleCounts);
    GET_VALUE(sampledImageIntegerSampleCounts);
    GET_VALUE(sampledImageDepthSampleCounts);
    GET_VALUE(sampledImageStencilSampleCounts);
    GET_VALUE(storageImageSampleCounts);
    GET_VALUE(maxSampleMaskWords);
    GET_VALUE(timestampComputeAndGraphics);
    GET_VALUE(timestampPeriod);
    GET_VALUE(maxClipDistances);
    GET_VALUE(maxCullDistances);
    GET_VALUE(maxCombinedClipAndCullDistances);
    GET_VALUE(discreteQueuePriorities);
    GET_ARRAY(pointSizeRange);  // size == 2
    GET_ARRAY(lineWidthRange);  // size == 2
    GET_VALUE(pointSizeGranularity);
    GET_VALUE(lineWidthGranularity);
    GET_VALUE(strictLines);
    GET_VALUE(standardSampleLocations);
    GET_VALUE(optimalBufferCopyOffsetAlignment);
    GET_VALUE(optimalBufferCopyRowPitchAlignment);
    GET_VALUE(nonCoherentAtomSize);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceSparseProperties *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    GET_VALUE(residencyStandard2DBlockShape);
    GET_VALUE(residencyStandard2DMultisampleBlockShape);
    GET_VALUE(residencyStandard3DBlockShape);
    GET_VALUE(residencyAlignedMipSize);
    GET_VALUE(residencyNonResidentStrict);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceSubgroupProperties *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    GET_VALUE(subgroupSize);
    GET_VALUE(supportedStages);
    GET_VALUE(supportedOperations);
    GET_VALUE(quadOperationsInAllStages);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceFeatures *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceFeatures)\n");
    GET_VALUE(robustBufferAccess);
    GET_VALUE(fullDrawIndexUint32);
    GET_VALUE(imageCubeArray);
    GET_VALUE(independentBlend);
    GET_VALUE(geometryShader);
    GET_VALUE(tessellationShader);
    GET_VALUE(sampleRateShading);
    GET_VALUE(dualSrcBlend);
    GET_VALUE(logicOp);
    GET_VALUE(multiDrawIndirect);
    GET_VALUE(drawIndirectFirstInstance);
    GET_VALUE(depthClamp);
    GET_VALUE(depthBiasClamp);
    GET_VALUE(fillModeNonSolid);
    GET_VALUE(depthBounds);
    GET_VALUE(wideLines);
    GET_VALUE(largePoints);
    GET_VALUE(alphaToOne);
    GET_VALUE(multiViewport);
    GET_VALUE(samplerAnisotropy);
    GET_VALUE(textureCompressionETC2);
    GET_VALUE(textureCompressionASTC_LDR);
    GET_VALUE(textureCompressionBC);
    GET_VALUE(occlusionQueryPrecise);
    GET_VALUE(pipelineStatisticsQuery);
    GET_VALUE(vertexPipelineStoresAndAtomics);
    GET_VALUE(fragmentStoresAndAtomics);
    GET_VALUE(shaderTessellationAndGeometryPointSize);
    GET_VALUE(shaderImageGatherExtended);
    GET_VALUE(shaderStorageImageExtendedFormats);
    GET_VALUE(shaderStorageImageMultisample);
    GET_VALUE(shaderStorageImageReadWithoutFormat);
    GET_VALUE(shaderStorageImageWriteWithoutFormat);
    GET_VALUE(shaderUniformBufferArrayDynamicIndexing);
    GET_VALUE(shaderSampledImageArrayDynamicIndexing);
    GET_VALUE(shaderStorageBufferArrayDynamicIndexing);
    GET_VALUE(shaderStorageImageArrayDynamicIndexing);
    GET_VALUE(shaderClipDistance);
    GET_VALUE(shaderCullDistance);
    GET_VALUE(shaderFloat64);
    GET_VALUE(shaderInt64);
    GET_VALUE(shaderInt16);
    GET_VALUE(shaderResourceResidency);
    GET_VALUE(shaderResourceMinLod);
    GET_VALUE(sparseBinding);
    GET_VALUE(sparseResidencyBuffer);
    GET_VALUE(sparseResidencyImage2D);
    GET_VALUE(sparseResidencyImage3D);
    GET_VALUE(sparseResidency2Samples);
    GET_VALUE(sparseResidency4Samples);
    GET_VALUE(sparseResidency8Samples);
    GET_VALUE(sparseResidency16Samples);
    GET_VALUE(sparseResidencyAliased);
    GET_VALUE(variableMultisampleRate);
    GET_VALUE(inheritedQueries);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDevice8BitStorageFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevice8BitStorageFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_8BIT_STORAGE_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_8bit_storage, but VK_KHR_8bit_storage is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(storageBuffer8BitAccess, WarnIfGreater);
    GET_VALUE_WARN(uniformAndStorageBuffer8BitAccess, WarnIfGreater);
    GET_VALUE_WARN(storagePushConstant8, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDevice16BitStorageFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevice16BitStorageFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_16BIT_STORAGE_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_16bit_storage, but VK_KHR_16bit_storage is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(storageBuffer16BitAccess, WarnIfGreater);
    GET_VALUE_WARN(uniformAndStorageBuffer16BitAccess, WarnIfGreater);
    GET_VALUE_WARN(storagePushConstant16, WarnIfGreater);
    GET_VALUE_WARN(storageInputOutput16, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceBufferDeviceAddressFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceBufferDeviceAddressFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_buffer_device_address, but "
            "VK_KHR_buffer_device_address is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(bufferDeviceAddress, WarnIfGreater);
    GET_VALUE_WARN(bufferDeviceAddressCaptureReplay, WarnIfGreater);
    GET_VALUE_WARN(bufferDeviceAddressMultiDevice, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceDescriptorIndexingFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceDescriptorIndexingFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_descriptor_indexing, but "
            "VK_EXT_descriptor_indexing is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(shaderInputAttachmentArrayDynamicIndexing, WarnIfGreater);
    GET_VALUE_WARN(shaderUniformTexelBufferArrayDynamicIndexing, WarnIfGreater);
    GET_VALUE_WARN(shaderStorageTexelBufferArrayDynamicIndexing, WarnIfGreater);
    GET_VALUE_WARN(shaderUniformBufferArrayNonUniformIndexing, WarnIfGreater);
    GET_VALUE_WARN(shaderSampledImageArrayNonUniformIndexing, WarnIfGreater);
    GET_VALUE_WARN(shaderStorageBufferArrayNonUniformIndexing, WarnIfGreater);
    GET_VALUE_WARN(shaderStorageImageArrayNonUniformIndexing, WarnIfGreater);
    GET_VALUE_WARN(shaderInputAttachmentArrayNonUniformIndexing, WarnIfGreater);
    GET_VALUE_WARN(shaderUniformTexelBufferArrayNonUniformIndexing, WarnIfGreater);
    GET_VALUE_WARN(shaderStorageTexelBufferArrayNonUniformIndexing, WarnIfGreater);
    GET_VALUE_WARN(descriptorBindingUniformBufferUpdateAfterBind, WarnIfGreater);
    GET_VALUE_WARN(descriptorBindingSampledImageUpdateAfterBind, WarnIfGreater);
    GET_VALUE_WARN(descriptorBindingStorageImageUpdateAfterBind, WarnIfGreater);
    GET_VALUE_WARN(descriptorBindingStorageBufferUpdateAfterBind, WarnIfGreater);
    GET_VALUE_WARN(descriptorBindingUniformTexelBufferUpdateAfterBind, WarnIfGreater);
    GET_VALUE_WARN(descriptorBindingStorageTexelBufferUpdateAfterBind, WarnIfGreater);
    GET_VALUE_WARN(descriptorBindingUpdateUnusedWhilePending, WarnIfGreater);
    GET_VALUE_WARN(descriptorBindingPartiallyBound, WarnIfGreater);
    GET_VALUE_WARN(descriptorBindingVariableDescriptorCount, WarnIfGreater);
    GET_VALUE_WARN(runtimeDescriptorArray, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceHostQueryResetFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceHostQueryResetFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_host_query_reset, but "
            "VK_EXT_host_query_reset is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(hostQueryReset, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceImagelessFramebufferFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceImagelessFramebufferFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_imageless_framebuffer, but "
            "VK_KHR_imageless_framebuffer is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(imagelessFramebuffer, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceMultiviewFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceMultiviewFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_MULTIVIEW_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_multiview, but VK_KHR_multiview is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(multiview, WarnIfGreater);
    GET_VALUE_WARN(multiviewGeometryShader, WarnIfGreater);
    GET_VALUE_WARN(multiviewTessellationShader, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDevicePortabilitySubsetFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePortabilitySubsetFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME) && emulatePortability.num <= 0) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_portability_subset, but VK_KHR_portability_subset is "
            "not supported by the device and emulation is not turned on.\nIf you wish to emulate "
            "VK_KHR_portability_subset, please set environment variable %s to 1.\n",
            kEnvarDevsimEmulatePortability);
    }
    GET_VALUE_WARN(constantAlphaColorBlendFactors, WarnIfGreater);
    GET_VALUE_WARN(events, WarnIfGreater);
    GET_VALUE_WARN(imageViewFormatReinterpretation, WarnIfGreater);
    GET_VALUE_WARN(imageViewFormatSwizzle, WarnIfGreater);
    GET_VALUE_WARN(imageView2DOn3DImage, WarnIfGreater);
    GET_VALUE_WARN(multisampleArrayImage, WarnIfGreater);
    GET_VALUE_WARN(mutableComparisonSamplers, WarnIfGreater);
    GET_VALUE_WARN(pointPolygons, WarnIfGreater);
    GET_VALUE_WARN(samplerMipLodBias, WarnIfGreater);
    GET_VALUE_WARN(separateStencilMaskRef, WarnIfGreater);
    GET_VALUE_WARN(shaderSampleRateInterpolationFunctions, WarnIfGreater);
    GET_VALUE_WARN(tessellationIsolines, WarnIfGreater);
    GET_VALUE_WARN(tessellationPointMode, WarnIfGreater);
    GET_VALUE_WARN(triangleFans, WarnIfGreater);
    GET_VALUE_WARN(vertexAttributeAccessBeyondStride, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceProtectedMemoryFeatures *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceProtectedMemoryFeatures)\n");
    GET_VALUE_WARN(protectedMemory, WarnIfLesser);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_sampler_ycbcr_conversion, but "
            "VK_KHR_sampler_ycbcr_conversion is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(samplerYcbcrConversion, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceScalarBlockLayoutFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceScalarBlockLayoutFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_scalar_block_layout, but "
            "VK_EXT_scalar_block_layout is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(scalarBlockLayout, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name,
                          VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_SEPARATE_DEPTH_STENCIL_LAYOUTS_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_separate_depth_stencil_layouts, but "
            "VK_KHR_separate_depth_stencil_layouts is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(separateDepthStencilLayouts, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderAtomicInt64FeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderAtomicInt64FeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_SHADER_ATOMIC_INT64_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_shader_atomic_int64, but "
            "VK_KHR_shader_atomic_int64 is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(shaderBufferInt64Atomics, WarnIfGreater);
    GET_VALUE_WARN(shaderSharedInt64Atomics, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderDrawParametersFeatures *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderAtomicInt64FeaturesKHR)\n");
    GET_VALUE_WARN(shaderDrawParameters, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderFloat16Int8FeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderFloat16Int8FeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_shader_float16_int8, but "
            "VK_KHR_shader_float16_int8 is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(shaderFloat16, WarnIfGreater);
    GET_VALUE_WARN(shaderInt8, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name,
                          VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceFeatures)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_SHADER_SUBGROUP_EXTENDED_TYPES_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_shader_subgroup_extended_types, but "
            "VK_KHR_shader_subgroup_extended_types is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(shaderSubgroupExtendedTypes, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceTimelineSemaphoreFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceTimelineSemaphoreFeaturesKHR)\n");
    GET_VALUE_WARN(timelineSemaphore, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name,
                          VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_uniform_buffer_standard_layout, but"
            "VK_KHR_unifrom_buffer_standard_layout is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(uniformBufferStandardLayout, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceVariablePointersFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceVariablePointersFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_variable_pointers, but VK_KHR_variable_pointers is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(variablePointersStorageBuffer, WarnIfGreater);
    GET_VALUE_WARN(variablePointers, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceVulkanMemoryModelFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceVulkanMemoryModelFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_VULKAN_MEMORY_MODEL_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_vulkan_memory_model, but VK_KHR_vulkan_memory_model "
            "is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(vulkanMemoryModel, WarnIfGreater);
    GET_VALUE_WARN(vulkanMemoryModelDeviceScope, WarnIfGreater);
    GET_VALUE_WARN(vulkanMemoryModelAvailabilityVisibilityChains, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name,
                          VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_ZERO_INITIALIZE_WORKGROUP_MEMORY_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_zero_initialize_workgroup_memory, but "
            "VK_KHR_zero_initialize_workgroup_memory is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(shaderZeroInitializeWorkgroupMemory, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceAccelerationStructureFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceAccelerationStructureFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_acceleration_structure, but "
            "VK_KHR_acceleration_structure is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(accelerationStructure, WarnIfGreater);
    GET_VALUE_WARN(accelerationStructureCaptureReplay, WarnIfGreater);
    GET_VALUE_WARN(accelerationStructureIndirectBuild, WarnIfGreater);
    GET_VALUE_WARN(accelerationStructureHostCommands, WarnIfGreater);
    GET_VALUE_WARN(descriptorBindingAccelerationStructureUpdateAfterBind, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceAccelerationStructurePropertiesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceAccelerationStructurePropertiesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_acceleration_structure, but "
            "VK_KHR_acceleration_structure is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(maxGeometryCount, WarnIfGreater);
    GET_VALUE_WARN(maxInstanceCount, WarnIfGreater);
    GET_VALUE_WARN(maxPrimitiveCount, WarnIfGreater);
    GET_VALUE_WARN(maxPerStageDescriptorAccelerationStructures, WarnIfGreater);
    GET_VALUE_WARN(maxPerStageDescriptorUpdateAfterBindAccelerationStructures, WarnIfGreater);
    GET_VALUE_WARN(maxDescriptorSetAccelerationStructures, WarnIfGreater);
    GET_VALUE_WARN(maxDescriptorSetUpdateAfterBindAccelerationStructures, WarnIfGreater);
    GET_VALUE_WARN(minAccelerationStructureScratchOffsetAlignment, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDevicePerformanceQueryFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePerformanceQueryFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_PERFORMANCE_QUERY_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_performance_query, but "
            "VK_KHR_performance_query is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(performanceCounterQueryPools, WarnIfGreater);
    GET_VALUE_WARN(performanceCounterMultipleQueryPools, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDevicePerformanceQueryPropertiesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePerformanceQueryPropertiesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_PERFORMANCE_QUERY_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_performance_query, but "
            "VK_KHR_performance_query is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(allowCommandBufferQueryCopies, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name,
                          VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_PIPELINE_EXECUTABLE_PROPERTIES_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_pipeline_executable_properties, but "
            "VK_KHR_pipeline_executable_properties is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(pipelineExecutableInfo, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDevicePresentIdFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePresentIdFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_PRESENT_ID_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_present_id, but "
            "VK_KHR_present_id is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(presentId, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDevicePresentWaitFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePresentWaitFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_PRESENT_WAIT_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_present_wait, but "
            "VK_KHR_present_wait is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(presentWait, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDevicePushDescriptorPropertiesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePushDescriptorPropertiesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_push_descriptor, but "
            "VK_KHR_push_descriptor is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(maxPushDescriptors, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceRayQueryFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceRayQueryFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_RAY_QUERY_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_ray_query, but "
            "VK_KHR_ray_query is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(rayQuery, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceRayTracingPipelineFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceRayTracingPipelineFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_ray_tracing_pipeline, but "
            "VK_KHR_ray_tracing_pipeline is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(rayTracingPipeline, WarnIfGreater);
    GET_VALUE_WARN(rayTracingPipelineShaderGroupHandleCaptureReplay, WarnIfGreater);
    GET_VALUE_WARN(rayTracingPipelineShaderGroupHandleCaptureReplayMixed, WarnIfGreater);
    GET_VALUE_WARN(rayTracingPipelineTraceRaysIndirect, WarnIfGreater);
    GET_VALUE_WARN(rayTraversalPrimitiveCulling, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceRayTracingPipelinePropertiesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceRayTracingPipelinePropertiesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_ray_tracing_pipeline, but "
            "VK_KHR_ray_tracing_pipeline is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(shaderGroupHandleSize, WarnIfGreater);
    GET_VALUE_WARN(maxRayRecursionDepth, WarnIfGreater);
    GET_VALUE_WARN(maxShaderGroupStride, WarnIfGreater);
    GET_VALUE_WARN(shaderGroupBaseAlignment, WarnIfGreater);
    GET_VALUE_WARN(shaderGroupHandleCaptureReplaySize, WarnIfGreater);
    GET_VALUE_WARN(maxRayDispatchInvocationCount, WarnIfGreater);
    GET_VALUE_WARN(shaderGroupHandleAlignment, WarnIfGreater);
    GET_VALUE_WARN(maxRayHitAttributeSize, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderClockFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderClockFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_SHADER_CLOCK_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_shader_clock, but "
            "VK_KHR_shader_clock is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(shaderSubgroupClock, WarnIfGreater);
    GET_VALUE_WARN(shaderDeviceClock, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_SHADER_INTEGER_DOT_PRODUCT_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_shader_integer_dot_product, but "
            "VK_KHR_shader_integer_dot_product is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(shaderIntegerDotProduct, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_SHADER_INTEGER_DOT_PRODUCT_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_shader_integer_dot_product, but "
            "VK_KHR_shader_integer_dot_product is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(integerDotProduct8BitUnsignedAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProduct8BitSignedAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProduct8BitMixedSignednessAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProduct4x8BitPackedUnsignedAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProduct4x8BitPackedSignedAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProduct4x8BitPackedMixedSignednessAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProduct16BitUnsignedAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProduct16BitSignedAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProduct16BitMixedSignednessAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProduct32BitUnsignedAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProduct32BitSignedAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProduct32BitMixedSignednessAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProduct64BitUnsignedAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProduct64BitSignedAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProduct64BitMixedSignednessAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProductAccumulatingSaturating8BitUnsignedAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProductAccumulatingSaturating8BitSignedAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProductAccumulatingSaturating8BitMixedSignednessAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProductAccumulatingSaturating4x8BitPackedUnsignedAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProductAccumulatingSaturating4x8BitPackedSignedAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProductAccumulatingSaturating4x8BitPackedMixedSignednessAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProductAccumulatingSaturating16BitUnsignedAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProductAccumulatingSaturating16BitSignedAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProductAccumulatingSaturating16BitMixedSignednessAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProductAccumulatingSaturating32BitUnsignedAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProductAccumulatingSaturating32BitSignedAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProductAccumulatingSaturating32BitMixedSignednessAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProductAccumulatingSaturating64BitUnsignedAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProductAccumulatingSaturating64BitSignedAccelerated, WarnIfGreater);
    GET_VALUE_WARN(integerDotProductAccumulatingSaturating64BitMixedSignednessAccelerated, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name,
                          VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_shader_subgroup_uniform_control_flow, but "
            "VK_KHR_shader_subgroup_uniform_control_flow is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(shaderSubgroupUniformControlFlow, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_SHADER_TERMINATE_INVOCATION_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_shader_terminate_invocation, but "
            "VK_KHR_shader_terminate_invocation is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(shaderTerminateInvocation, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceSynchronization2FeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceSynchronization2FeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_synchronization2, but "
            "VK_KHR_synchronization2 is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(synchronization2, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name,
                          VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_workgroup_memory_explicit_layout, but "
            "VK_KHR_workgroup_memory_explicit_layout is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(workgroupMemoryExplicitLayout, WarnIfGreater);
    GET_VALUE_WARN(workgroupMemoryExplicitLayoutScalarBlockLayout, WarnIfGreater);
    GET_VALUE_WARN(workgroupMemoryExplicitLayout8BitAccess, WarnIfGreater);
    GET_VALUE_WARN(workgroupMemoryExplicitLayout16BitAccess, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDevice4444FormatsFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_4444_FORMATS_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_4444_formats, but "
            "VK_EXT_4444_formats is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(formatA4R4G4B4, WarnIfGreater);
    GET_VALUE_WARN(formatA4B4G4R4, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceASTCDecodeFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceASTCDecodeFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_ASTC_DECODE_MODE_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_astc_decode_mode, but "
            "VK_EXT_astc_decode_mode is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(decodeModeSharedExponent, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_blend_operation_advanced, but "
            "VK_EXT_blend_operation_advanced is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(advancedBlendCoherentOperations, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_blend_operation_advanced, but "
            "VK_EXT_blend_operation_advanced is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(advancedBlendMaxColorAttachments, WarnIfGreater);
    GET_VALUE_WARN(advancedBlendIndependentBlend, WarnIfGreater);
    GET_VALUE_WARN(advancedBlendNonPremultipliedSrcColor, WarnIfGreater);
    GET_VALUE_WARN(advancedBlendNonPremultipliedDstColor, WarnIfGreater);
    GET_VALUE_WARN(advancedBlendCorrelatedOverlap, WarnIfGreater);
    GET_VALUE_WARN(advancedBlendAllOperations, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceBorderColorSwizzleFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceBorderColorSwizzleFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_BORDER_COLOR_SWIZZLE_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_border_color_swizzle, but "
            "VK_EXT_border_color_swizzle is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(borderColorSwizzle, WarnIfGreater);
    GET_VALUE_WARN(borderColorSwizzleFromImage, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceColorWriteEnableFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceColorWriteEnableFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_COLOR_WRITE_ENABLE_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_color_write_enable, but "
            "VK_EXT_color_write_enable is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(colorWriteEnable, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceConditionalRenderingFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceConditionalRenderingFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_CONDITIONAL_RENDERING_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_conditional_rendering, but "
            "VK_EXT_conditional_rendering is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(conditionalRendering, WarnIfGreater);
    GET_VALUE_WARN(inheritedConditionalRendering, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name,
                          VkPhysicalDeviceConservativeRasterizationPropertiesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceConservativeRasterizationPropertiesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_conservative_rasterization, but "
            "VK_EXT_conservative_rasterization is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(primitiveOverestimationSize, WarnIfGreater);
    GET_VALUE_WARN(maxExtraPrimitiveOverestimationSize, WarnIfGreater);
    GET_VALUE_WARN(extraPrimitiveOverestimationSizeGranularity, WarnIfGreater);
    GET_VALUE_WARN(primitiveUnderestimation, WarnIfGreater);
    GET_VALUE_WARN(conservativePointAndLineRasterization, WarnIfGreater);
    GET_VALUE_WARN(degenerateTrianglesRasterized, WarnIfGreater);
    GET_VALUE_WARN(degenerateLinesRasterized, WarnIfGreater);
    GET_VALUE_WARN(fullyCoveredFragmentShaderInputVariable, WarnIfGreater);
    GET_VALUE_WARN(conservativeRasterizationPostDepthCoverage, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceCustomBorderColorFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceCustomBorderColorFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_CUSTOM_BORDER_COLOR_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_custom_border_color, but "
            "VK_EXT_custom_border_color is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(customBorderColors, WarnIfGreater);
    GET_VALUE_WARN(customBorderColorWithoutFormat, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceCustomBorderColorPropertiesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceCustomBorderColorPropertiesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_CUSTOM_BORDER_COLOR_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_custom_border_color, but "
            "VK_EXT_custom_border_color is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(maxCustomBorderColorSamplers, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceDepthClipEnableFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceDepthClipEnableFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_DEPTH_CLIP_ENABLE_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_depth_clip_enable, but "
            "VK_EXT_depth_clip_enable is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(depthClipEnable, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceDeviceMemoryReportFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceDeviceMemoryReportFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_DEVICE_MEMORY_REPORT_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_device_memory_report, but "
            "VK_EXT_device_memory_report is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(deviceMemoryReport, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceDiscardRectanglePropertiesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceDiscardRectanglePropertiesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_DISCARD_RECTANGLES_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_discard_rectangles, but "
            "VK_EXT_discard_rectangles is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(maxDiscardRectangles, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceExtendedDynamicStateFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceExtendedDynamicStateFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_extended_dynamic_state, but "
            "VK_EXT_extended_dynamic_state is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(extendedDynamicState, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceExtendedDynamicState2FeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceExtendedDynamicState2FeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_extended_dynamic_state2, but "
            "VK_EXT_extended_dynamic_state2 is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(extendedDynamicState2, WarnIfGreater);
    GET_VALUE_WARN(extendedDynamicState2LogicOp, WarnIfGreater);
    GET_VALUE_WARN(extendedDynamicState2PatchControlPoints, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceExternalMemoryHostPropertiesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceExternalMemoryHostPropertiesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_EXTERNAL_MEMORY_HOST_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_external_memory_host, but "
            "VK_EXT_external_memory_host is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(minImportedHostPointerAlignment, WarnIfLesser);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceFragmentDensityMapFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceFragmentDensityMapFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_FRAGMENT_DENSITY_MAP_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_fragment_density_map, but "
            "VK_EXT_fragment_density_map is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(fragmentDensityMap, WarnIfGreater);
    GET_VALUE_WARN(fragmentDensityMapDynamic, WarnIfGreater);
    GET_VALUE_WARN(fragmentDensityMapNonSubsampledImages, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceFragmentDensityMapPropertiesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceFragmentDensityMapPropertiesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_FRAGMENT_DENSITY_MAP_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_fragment_density_map, but "
            "VK_EXT_fragment_density_map is "
            "not supported by the device.\n");
    }
    GET_VALUE(minFragmentDensityTexelSize);
    GET_VALUE(maxFragmentDensityTexelSize);
    GET_VALUE_WARN(fragmentDensityInvocations, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_fragment_shader_interlock, but "
            "VK_EXT_fragment_shader_interlock is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(fragmentShaderSampleInterlock, WarnIfGreater);
    GET_VALUE_WARN(fragmentShaderPixelInterlock, WarnIfGreater);
    GET_VALUE_WARN(fragmentShaderShadingRateInterlock, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_GLOBAL_PRIORITY_QUERY_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_global_priority_query, but "
            "VK_EXT_global_priority_query is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(globalPriorityQuery, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceImageRobustnessFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceImageRobustnessFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_IMAGE_ROBUSTNESS_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_image_robustness, but "
            "VK_EXT_image_robustness is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(robustImageAccess, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceIndexTypeUint8FeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceIndexTypeUint8FeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_INDEX_TYPE_UINT8_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_index_type_uint8, but "
            "VK_EXT_index_type_uint8 is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(indexTypeUint8, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceInlineUniformBlockFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceInlineUniformBlockFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_inline_uniform_block, but "
            "VK_EXT_inline_uniform_block is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(inlineUniformBlock, WarnIfGreater);
    GET_VALUE_WARN(descriptorBindingInlineUniformBlockUpdateAfterBind, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceInlineUniformBlockPropertiesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceInlineUniformBlockPropertiesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_inline_uniform_block, but "
            "VK_EXT_inline_uniform_block is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(maxInlineUniformBlockSize, WarnIfGreater);
    GET_VALUE_WARN(maxPerStageDescriptorInlineUniformBlocks, WarnIfGreater);
    GET_VALUE_WARN(maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks, WarnIfGreater);
    GET_VALUE_WARN(maxDescriptorSetInlineUniformBlocks, WarnIfGreater);
    GET_VALUE_WARN(maxDescriptorSetUpdateAfterBindInlineUniformBlocks, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceLineRasterizationFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceLineRasterizationFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_line_rasterization, but "
            "VK_EXT_line_rasterization is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(rectangularLines, WarnIfGreater);
    GET_VALUE_WARN(bresenhamLines, WarnIfGreater);
    GET_VALUE_WARN(smoothLines, WarnIfGreater);
    GET_VALUE_WARN(stippledRectangularLines, WarnIfGreater);
    GET_VALUE_WARN(stippledBresenhamLines, WarnIfGreater);
    GET_VALUE_WARN(stippledSmoothLines, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceLineRasterizationPropertiesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceLineRasterizationPropertiesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_line_rasterization, but "
            "VK_EXT_line_rasterization is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(lineSubPixelPrecisionBits, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceMemoryPriorityFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceMemoryPriorityFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_memory_priority, but "
            "VK_EXT_memory_priority is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(memoryPriority, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceMultiDrawFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceMultiDrawFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_MULTI_DRAW_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_multi_draw, but "
            "VK_EXT_multi_draw is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(multiDraw, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceMultiDrawPropertiesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceMultiDrawPropertiesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_MULTI_DRAW_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_multi_draw, but "
            "VK_EXT_multi_draw is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(maxMultiDrawCount, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_PAGEABLE_DEVICE_LOCAL_MEMORY_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_pageable_device_local_memory, but "
            "VK_EXT_pageable_device_local_memory is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(pageableDeviceLocalMemory, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name,
                          VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_PIPELINE_CREATION_CACHE_CONTROL_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_pipeline_creation_cache_control, but "
            "VK_EXT_pipeline_creation_cache_control is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(pipelineCreationCacheControl, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name,
                          VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_PRIMITIVE_TOPOLOGY_LIST_RESTART_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_primitive_topology_list_restart, but "
            "VK_EXT_primitive_topology_list_restart is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(primitiveTopologyListRestart, WarnIfGreater);
    GET_VALUE_WARN(primitiveTopologyPatchListRestart, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDevicePrivateDataFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDevicePrivateDataFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_PRIVATE_DATA_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_private_data, but "
            "VK_EXT_private_data is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(privateData, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceProvokingVertexFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceProvokingVertexFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_PROVOKING_VERTEX_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_provoking_vertex, but "
            "VK_EXT_provoking_vertex is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(provokingVertexLast, WarnIfGreater);
    GET_VALUE_WARN(transformFeedbackPreservesProvokingVertex, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceProvokingVertexPropertiesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceProvokingVertexPropertiesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_PROVOKING_VERTEX_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_provoking_vertex, but "
            "VK_EXT_provoking_vertex is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(provokingVertexModePerPipeline, WarnIfGreater);
    GET_VALUE_WARN(transformFeedbackPreservesTriangleFanProvokingVertex, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_RGBA10X6_FORMATS_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_rgba10x6_formats, but "
            "VK_EXT_rgba10x6_formats is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(formatRgba10x6WithoutYCbCrSampler, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceRobustness2FeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceRobustness2FeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_ROBUSTNESS_2_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_robustness2, but "
            "VK_EXT_robustness2 is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(robustBufferAccess2, WarnIfGreater);
    GET_VALUE_WARN(robustImageAccess2, WarnIfGreater);
    GET_VALUE_WARN(nullDescriptor, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceRobustness2PropertiesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceRobustness2PropertiesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_ROBUSTNESS_2_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_robustness2, but "
            "VK_EXT_robustness2 is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(robustStorageBufferAccessSizeAlignment, WarnIfGreater);
    GET_VALUE_WARN(robustUniformBufferAccessSizeAlignment, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceSampleLocationsPropertiesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceSampleLocationsPropertiesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_SAMPLE_LOCATIONS_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_sample_locations, but "
            "VK_EXT_sample_locations is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(sampleLocationSampleCounts, WarnIfGreater);
    GET_VALUE(maxSampleLocationGridSize);
    GET_ARRAY(sampleLocationCoordinateRange);
    GET_VALUE_WARN(sampleLocationSubPixelBits, WarnIfGreater);
    GET_VALUE_WARN(variableSampleLocations, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderAtomicFloatFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderAtomicFloatFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_shader_atomic_float, but "
            "VK_EXT_shader_atomic_float is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(shaderBufferFloat32Atomics, WarnIfGreater);
    GET_VALUE_WARN(shaderBufferFloat32AtomicAdd, WarnIfGreater);
    GET_VALUE_WARN(shaderBufferFloat64Atomics, WarnIfGreater);
    GET_VALUE_WARN(shaderBufferFloat64AtomicAdd, WarnIfGreater);
    GET_VALUE_WARN(shaderSharedFloat32Atomics, WarnIfGreater);
    GET_VALUE_WARN(shaderSharedFloat32AtomicAdd, WarnIfGreater);
    GET_VALUE_WARN(shaderSharedFloat64Atomics, WarnIfGreater);
    GET_VALUE_WARN(shaderSharedFloat64AtomicAdd, WarnIfGreater);
    GET_VALUE_WARN(shaderImageFloat32Atomics, WarnIfGreater);
    GET_VALUE_WARN(shaderImageFloat32AtomicAdd, WarnIfGreater);
    GET_VALUE_WARN(sparseImageFloat32Atomics, WarnIfGreater);
    GET_VALUE_WARN(sparseImageFloat32AtomicAdd, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_SHADER_ATOMIC_FLOAT_2_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_shader_atomic_float2, but "
            "VK_EXT_shader_atomic_float2 is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(shaderBufferFloat16Atomics, WarnIfGreater);
    GET_VALUE_WARN(shaderBufferFloat16AtomicAdd, WarnIfGreater);
    GET_VALUE_WARN(shaderBufferFloat16AtomicMinMax, WarnIfGreater);
    GET_VALUE_WARN(shaderBufferFloat32AtomicMinMax, WarnIfGreater);
    GET_VALUE_WARN(shaderBufferFloat64AtomicMinMax, WarnIfGreater);
    GET_VALUE_WARN(shaderSharedFloat16Atomics, WarnIfGreater);
    GET_VALUE_WARN(shaderSharedFloat16AtomicAdd, WarnIfGreater);
    GET_VALUE_WARN(shaderSharedFloat16AtomicMinMax, WarnIfGreater);
    GET_VALUE_WARN(shaderSharedFloat32AtomicMinMax, WarnIfGreater);
    GET_VALUE_WARN(shaderSharedFloat64AtomicMinMax, WarnIfGreater);
    GET_VALUE_WARN(shaderImageFloat32AtomicMinMax, WarnIfGreater);
    GET_VALUE_WARN(sparseImageFloat32AtomicMinMax, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name,
                          VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_shader_demote_to_helper_invocation, but "
            "VK_EXT_shader_demote_to_helper_invocation is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(shaderDemoteToHelperInvocation, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_SHADER_IMAGE_ATOMIC_INT64_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_shader_image_atomic_int64, but "
            "VK_EXT_shader_image_atomic_int64 is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(shaderImageInt64Atomics, WarnIfGreater);
    GET_VALUE_WARN(sparseImageInt64Atomics, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceSubgroupSizeControlFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceSubgroupSizeControlFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_subgroup_size_control, but "
            "VK_EXT_subgroup_size_control is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(subgroupSizeControl, WarnIfGreater);
    GET_VALUE_WARN(computeFullSubgroups, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceSubgroupSizeControlPropertiesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceSubgroupSizeControlPropertiesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_subgroup_size_control, but "
            "VK_EXT_subgroup_size_control is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(minSubgroupSize, WarnIfLesser);
    GET_VALUE_WARN(maxSubgroupSize, WarnIfGreater);
    GET_VALUE_WARN(maxComputeWorkgroupSubgroups, WarnIfGreater);
    GET_VALUE_WARN(requiredSubgroupSizeStages, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_texel_buffer_alignment, but "
            "VK_EXT_texel_buffer_alignment is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(texelBufferAlignment, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_texel_buffer_alignment, but "
            "VK_EXT_texel_buffer_alignment is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(storageTexelBufferOffsetAlignmentBytes, WarnIfGreater);
    GET_VALUE_WARN(storageTexelBufferOffsetSingleTexelAlignment, WarnIfGreater);
    GET_VALUE_WARN(uniformTexelBufferOffsetAlignmentBytes, WarnIfGreater);
    GET_VALUE_WARN(uniformTexelBufferOffsetSingleTexelAlignment, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_TEXTURE_COMPRESSION_ASTC_HDR_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_texture_compression_astc_hdr, but "
            "VK_EXT_texture_compression_astc_hdr is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(textureCompressionASTC_HDR, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceTransformFeedbackFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceTransformFeedbackFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_transform_feedback, but "
            "VK_EXT_transform_feedback is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(transformFeedback, WarnIfGreater);
    GET_VALUE_WARN(geometryStreams, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceTransformFeedbackPropertiesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceTransformFeedbackPropertiesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_transform_feedback, but "
            "VK_EXT_transform_feedback is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(maxTransformFeedbackStreams, WarnIfGreater);
    GET_VALUE_WARN(maxTransformFeedbackBuffers, WarnIfGreater);
    GET_VALUE_WARN(maxTransformFeedbackBufferSize, WarnIfGreater);
    GET_VALUE_WARN(maxTransformFeedbackStreamDataSize, WarnIfGreater);
    GET_VALUE_WARN(maxTransformFeedbackBufferDataSize, WarnIfGreater);
    GET_VALUE_WARN(maxTransformFeedbackBufferDataStride, WarnIfGreater);
    GET_VALUE_WARN(transformFeedbackQueries, WarnIfGreater);
    GET_VALUE_WARN(transformFeedbackStreamsLinesTriangles, WarnIfGreater);
    GET_VALUE_WARN(transformFeedbackRasterizationStreamSelect, WarnIfGreater);
    GET_VALUE_WARN(transformFeedbackDraw, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_vertex_attribute_divisor, but "
            "VK_EXT_vertex_attribute_divisor is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(vertexAttributeInstanceRateDivisor, WarnIfGreater);
    GET_VALUE_WARN(vertexAttributeInstanceRateZeroDivisor, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_vertex_attribute_divisor, but "
            "VK_EXT_vertex_attribute_divisor is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(maxVertexAttribDivisor, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_vertex_input_dynamic_state, but "
            "VK_EXT_vertex_input_dynamic_state is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(vertexInputDynamicState, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_YCBCR_2PLANE_444_FORMATS_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_ycbcr_2plane_444_formats, but "
            "VK_EXT_ycbcr_2plane_444_formats is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(ycbcr2plane444Formats, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceYcbcrImageArraysFeaturesEXT *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceYcbcrImageArraysFeaturesEXT)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_EXT_YCBCR_IMAGE_ARRAYS_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_EXT_ycbcr_image_arrays, but "
            "VK_EXT_ycbcr_image_arrays is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(ycbcrImageArrays, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceFragmentShadingRateFeaturesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceFragmentShadingRateFeaturesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_fragment_shading_rate, but "
            "VK_KHR_fragment_shading_rate is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(pipelineFragmentShadingRate, WarnIfGreater);
    GET_VALUE_WARN(primitiveFragmentShadingRate, WarnIfGreater);
    GET_VALUE_WARN(attachmentFragmentShadingRate, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceFragmentShadingRatePropertiesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceFragmentShadingRatePropertiesKHR)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_KHR_fragment_shading_rate, but "
            "VK_KHR_fragment_shading_rate is "
            "not supported by the device.\n");
    }
    GET_VALUE(minFragmentShadingRateAttachmentTexelSize);
    GET_VALUE(maxFragmentShadingRateAttachmentTexelSize);
    GET_VALUE_WARN(maxFragmentShadingRateAttachmentTexelSizeAspectRatio, WarnIfGreater);
    GET_VALUE_WARN(primitiveFragmentShadingRateWithMultipleViewports, WarnIfGreater);
    GET_VALUE_WARN(layeredShadingRateAttachments, WarnIfGreater);
    GET_VALUE_WARN(fragmentShadingRateNonTrivialCombinerOps, WarnIfGreater);
    GET_VALUE(maxFragmentSize);
    GET_VALUE_WARN(maxFragmentSizeAspectRatio, WarnIfGreater);
    GET_VALUE_WARN(maxFragmentShadingRateCoverageSamples, WarnIfGreater);
    GET_VALUE(maxFragmentShadingRateRasterizationSamples);
    GET_VALUE_WARN(fragmentShadingRateWithShaderDepthStencilWrites, WarnIfGreater);
    GET_VALUE_WARN(fragmentShadingRateWithSampleMask, WarnIfGreater);
    GET_VALUE_WARN(fragmentShadingRateWithShaderSampleMask, WarnIfGreater);
    GET_VALUE_WARN(fragmentShadingRateWithConservativeRasterization, WarnIfGreater);
    GET_VALUE_WARN(fragmentShadingRateWithFragmentShaderInterlock, WarnIfGreater);
    GET_VALUE_WARN(fragmentShadingRateWithCustomSampleLocations, WarnIfGreater);
    GET_VALUE_WARN(fragmentShadingRateStrictMultiplyCombiner, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceCoherentMemoryFeaturesAMD *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceCoherentMemoryFeaturesAMD)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_AMD_device_coherent_memory, but "
            "VK_AMD_device_coherent_memory is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(deviceCoherentMemory, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderCorePropertiesAMD *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderCorePropertiesAMD)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_AMD_SHADER_CORE_PROPERTIES_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_AMD_shader_core_properties, but "
            "VK_AMD_shader_core_properties is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(shaderEngineCount, WarnIfGreater);
    GET_VALUE_WARN(shaderArraysPerEngineCount, WarnIfGreater);
    GET_VALUE_WARN(computeUnitsPerShaderArray, WarnIfGreater);
    GET_VALUE_WARN(simdPerComputeUnit, WarnIfGreater);
    GET_VALUE_WARN(wavefrontsPerSimd, WarnIfGreater);
    GET_VALUE_WARN(wavefrontSize, WarnIfGreater);
    GET_VALUE_WARN(sgprsPerSimd, WarnIfGreater);
    GET_VALUE_WARN(minSgprAllocation, WarnIfGreater);
    GET_VALUE_WARN(maxSgprAllocation, WarnIfGreater);
    GET_VALUE_WARN(sgprAllocationGranularity, WarnIfGreater);
    GET_VALUE_WARN(vgprsPerSimd, WarnIfGreater);
    GET_VALUE_WARN(minVgprAllocation, WarnIfGreater);
    GET_VALUE_WARN(maxVgprAllocation, WarnIfGreater);
    GET_VALUE_WARN(vgprAllocationGranularity, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderCoreProperties2AMD *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderCoreProperties2AMD)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_AMD_SHADER_CORE_PROPERTIES_2_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_AMD_shader_core_properties2, but "
            "VK_AMD_shader_core_properties2 is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(shaderCoreFeatures, WarnIfGreater);
    GET_VALUE_WARN(activeComputeUnitCount, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceInvocationMaskFeaturesHUAWEI *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceInvocationMaskFeaturesHUAWEI)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_HUAWEI_INVOCATION_MASK_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_HUAWEI_invocation_mask, but "
            "VK_HUAWEI_invocation_mask is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(invocationMask, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceSubpassShadingFeaturesHUAWEI *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceSubpassShadingFeaturesHUAWEI)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_HUAWEI_SUBPASS_SHADING_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_HUAWEI_subpass_shading, but "
            "VK_HUAWEI_subpass_shading is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(subpassShading, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceSubpassShadingPropertiesHUAWEI *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceSubpassShadingPropertiesHUAWEI)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_HUAWEI_SUBPASS_SHADING_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_HUAWEI_subpass_shading, but "
            "VK_HUAWEI_subpass_shading is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(maxSubpassShadingWorkgroupSizeAspectRatio, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_INTEL_SHADER_INTEGER_FUNCTIONS_2_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_INTEL_shader_integer_functions2, but "
            "VK_INTEL_shader_integer_functions2 is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(shaderIntegerFunctions2, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceComputeShaderDerivativesFeaturesNV *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceComputeShaderDerivativesFeaturesNV)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_NV_COMPUTE_SHADER_DERIVATIVES_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_NV_compute_shader_derivatives, but "
            "VK_NV_compute_shader_derivatives is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(computeDerivativeGroupQuads, WarnIfGreater);
    GET_VALUE_WARN(computeDerivativeGroupLinear, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceCooperativeMatrixFeaturesNV *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceCooperativeMatrixFeaturesNV)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_NV_COOPERATIVE_MATRIX_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_NV_cooperative_matrix, but "
            "VK_NV_cooperative_matrix is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(cooperativeMatrix, WarnIfGreater);
    GET_VALUE_WARN(cooperativeMatrixRobustBufferAccess, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceCooperativeMatrixPropertiesNV *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceCooperativeMatrixPropertiesNV)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_NV_COOPERATIVE_MATRIX_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_NV_cooperative_matrix, but "
            "VK_NV_cooperative_matrix is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(cooperativeMatrixSupportedStages, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceCornerSampledImageFeaturesNV *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceCornerSampledImageFeaturesNV)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_NV_CORNER_SAMPLED_IMAGE_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_NV_corner_sampled_image, but "
            "VK_NV_corner_sampled_image is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(cornerSampledImage, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceCoverageReductionModeFeaturesNV *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    DebugPrintf("\t\tJsonLoader::GetValue(VkPhysicalDeviceCoverageReductionModeFeaturesNV)\n");
    if (!PhysicalDeviceData::HasExtension(&pdd_, VK_NV_COVERAGE_REDUCTION_MODE_EXTENSION_NAME)) {
        ErrorPrintf(
            "JSON file sets variables for structs provided by VK_NV_coverage_reduction_mode, but "
            "VK_NV_coverage_reduction_mode is "
            "not supported by the device.\n");
    }
    GET_VALUE_WARN(coverageReductionMode, WarnIfGreater);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkExtent2D *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    GET_VALUE(width);
    GET_VALUE(height);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkExtent3D *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    GET_VALUE(width);
    GET_VALUE(height);
    GET_VALUE(depth);
}

void JsonLoader::GetValue(const Json::Value &parent, int index, VkQueueFamilyProperties *dest) {
    const Json::Value value = parent[index];
    if (value.type() != Json::objectValue) {
        return;
    }
    GET_VALUE(queueFlags);
    GET_VALUE(queueCount);
    GET_VALUE(timestampValidBits);
    GET_VALUE(minImageTransferGranularity);
}

void JsonLoader::GetValue(const Json::Value &parent, int index, VkMemoryType *dest) {
    const Json::Value value = parent[index];
    if (value.type() != Json::objectValue) {
        return;
    }
    GET_VALUE(propertyFlags);
    GET_VALUE(heapIndex);
}

void JsonLoader::GetValue(const Json::Value &parent, int index, VkMemoryHeap *dest) {
    const Json::Value value = parent[index];
    if (value.type() != Json::objectValue) {
        return;
    }
    GET_VALUE_WARN(size, WarnIfGreater);
    GET_VALUE(flags);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceMemoryProperties *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
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
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkSurfaceCapabilitiesKHR *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    GET_VALUE_WARN(minImageCount, WarnIfLesser);
    GET_VALUE_WARN(maxImageCount, WarnIfGreater);
    GET_VALUE(minImageExtent);
    GET_VALUE(maxImageExtent);
    GET_VALUE_WARN(maxImageArrayLayers, WarnIfGreater);
    GET_VALUE(supportedTransforms);
    GET_VALUE(supportedCompositeAlpha);
    GET_VALUE(supportedUsageFlags);
}

void JsonLoader::GetValue(const Json::Value &parent, int index, DevsimFormatProperties *dest) {
    const Json::Value value = parent[index];
    if (value.type() != Json::objectValue) {
        return;
    }
    GET_VALUE(formatID);
    GET_VALUE(linearTilingFeatures);
    GET_VALUE(optimalTilingFeatures);
    GET_VALUE(bufferFeatures);
}

void JsonLoader::GetValue(const Json::Value &parent, int index, VkLayerProperties *dest) {
    const Json::Value value = parent[index];
    if (value.type() != Json::objectValue) {
        return;
    }
    GET_ARRAY(layerName);  // size < VK_MAX_EXTENSION_NAME_SIZE
    GET_VALUE(specVersion);
    GET_VALUE(implementationVersion);
    GET_ARRAY(description);  // size < VK_MAX_DESCRIPTION_SIZE
}

void JsonLoader::GetValue(const Json::Value &parent, int index, VkExtensionProperties *dest) {
    const Json::Value value = parent[index];
    if (value.type() != Json::objectValue) {
        return;
    }
    GET_ARRAY(extensionName);  // size < VK_MAX_EXTENSION_NAME_SIZE
    GET_VALUE(specVersion);
}

void JsonLoader::GetValue(const Json::Value &parent, int index, VkSurfaceFormatKHR *dest) {
    const Json::Value value = parent[index];
    if (value.type() != Json::objectValue) {
        return;
    }
    GET_VALUE(format);
    GET_VALUE(colorSpace);
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

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceVulkan12Properties *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    GET_VALUE(framebufferIntegerColorSampleCounts);
}

void JsonLoader::GetValue(const Json::Value &parent, const char *name, VkPhysicalDeviceVulkan12Features *dest) {
    const Json::Value value = parent[name];
    if (value.type() != Json::objectValue) {
        return;
    }
    GET_VALUE_WARN(samplerMirrorClampToEdge, WarnIfGreater);
    GET_VALUE_WARN(shaderOutputViewportIndex, WarnIfGreater);
    GET_VALUE_WARN(shaderOutputLayer, WarnIfGreater);
    GET_VALUE_WARN(subgroupBroadcastDynamicId, WarnIfGreater);
    GET_VALUE_WARN(drawIndirectCount, WarnIfGreater);
    GET_VALUE_WARN(descriptorIndexing, WarnIfGreater);
    GET_VALUE_WARN(samplerFilterMinmax, WarnIfGreater);
}

void JsonLoader::GetValueGPUinfoCore11(const Json::Value &parent) {
    const Json::Value core11 = parent["core11"];
    if (core11.type() != Json::objectValue) {
        return;
    }

    const Json::Value core11_features = core11["features"];
    if (core11_features.type() == Json::objectValue) {
        GetValue(core11_features, "storageBuffer16BitAccess",
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
        GetValue(device_props, "subgroupProperties", &pdd_.physical_device_subgroup_properties_);
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
        GetValue(core12_properties, "denormBehaviorIndependence",
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

    GetValue(surface_capabilities, "minImageCount", &pdd_.surface_capabilities_.minImageCount);
    GetValue(surface_capabilities, "maxImageCount", &pdd_.surface_capabilities_.maxImageCount);
    GetValue(surface_capabilities, "minImageExtent", &pdd_.surface_capabilities_.minImageExtent);
    GetValue(surface_capabilities, "maxImageExtent", &pdd_.surface_capabilities_.maxImageExtent);
    GetValue(surface_capabilities, "maxImageArrayLayers", &pdd_.surface_capabilities_.maxImageArrayLayers);
    GetValue(surface_capabilities, "supportedTransforms", &pdd_.surface_capabilities_.supportedTransforms);
    GetValue(surface_capabilities, "supportedCompositeAlpha", &pdd_.surface_capabilities_.supportedCompositeAlpha);
    GetValue(surface_capabilities, "supportedUsageFlags", &pdd_.surface_capabilities_.supportedUsageFlags);

    GetArray(surface_capabilities, "surfaceformats", &pdd_.arrayof_surface_formats_);
    GetArray(surface_capabilities, "presentmodes", &pdd_.arrayof_present_modes_);
}

#undef GET_VALUE
#undef GET_ARRAY

// Layer-specific wrappers for Vulkan functions, accessed via vkGet*ProcAddr() ///////////////////////////////////////////////////

// Fill the inputFilename variable with a value from either vk_layer_settings.txt or environment variables.
// Environment variables get priority.
static void GetDevSimFilename() {
    std::string env_var = GetEnvarValue(kEnvarDevsimFilename);
    if (!env_var.empty()) {
        inputFilename.str = env_var;
        inputFilename.fromEnvVar = true;
    }

    if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsDevsimFilename)) {
        inputFilename.fromEnvVar = false;
        inputFilename.str = vku::GetLayerSettingString(kOurLayerName, kLayerSettingsDevsimFilename);
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

static ArrayCombinationMode GetArrayCombinationModeValue(const std::string &value) {
    std::string temp = value;
    std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);

    if (value.empty())
        return ARRAY_COMBINATION_MODE_NONE;
    else if (temp == "none" || temp == "0")
        return ARRAY_COMBINATION_MODE_NONE;
    else if (temp == "replace" || temp == "1")
        return ARRAY_COMBINATION_MODE_REPLACE;
    else if (temp == "whitelist" || temp == "2")
        return ARRAY_COMBINATION_MODE_WHITELIST;
    else if (temp == "blacklist" || temp == "3")
        return ARRAY_COMBINATION_MODE_BLACKLIST;
    else if (temp == "intersect" || temp == "4")
        return ARRAY_COMBINATION_MODE_INTERSECT;
    else
        return ARRAY_COMBINATION_MODE_NONE;
}
// Fill the debugLevel variable with a value from either vk_layer_settings.txt or environment variables.
// Environment variables get priority.
static void GetDevSimDebugLevel() {
    std::string env_var = GetEnvarValue(kEnvarDevsimDebugEnable);
    if (!env_var.empty()) {
        debugLevel.fromEnvVar = true;
        debugLevel.num = GetBooleanValue(env_var);
    }

    if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsDevsimDebugEnable)) {
        debugLevel.fromEnvVar = false;
        debugLevel.num = vku::GetLayerSettingBool(kOurLayerName, kLayerSettingsDevsimDebugEnable);
    }
}

// Fill the errorLevel variable with a value from either vk_layer_settings.txt or environment variables.
// Environment variables get priority.
static void GetDevSimErrorLevel() {
    std::string env_var = GetEnvarValue(kEnvarDevsimExitOnError);
    if (!env_var.empty()) {
        errorLevel.num = GetBooleanValue(env_var);
        errorLevel.fromEnvVar = true;
    }

    if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsDevsimExitOnError)) {
        errorLevel.fromEnvVar = false;
        errorLevel.num = vku::GetLayerSettingBool(kOurLayerName, kLayerSettingsDevsimExitOnError);
    }
}

// Fill the emulatePortability variable with a value from either vk_layer_settings.txt or environment variables.
// Environment variables get priority.
static void GetDevSimEmulatePortability() {
    std::string env_var = GetEnvarValue(kEnvarDevsimEmulatePortability);
    if (!env_var.empty()) {
        emulatePortability.num = GetBooleanValue(env_var);
        emulatePortability.fromEnvVar = true;
    }

    if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsDevsimEmulatePortability)) {
        emulatePortability.fromEnvVar = false;
        emulatePortability.num = vku::GetLayerSettingBool(kOurLayerName, kLayerSettingsDevsimEmulatePortability);
    }
}

// Fill the modifyExtensionList variable with a value from either vk_layer_settings.txt or environment variables.
// Environment variables get priority.
static void GetDevSimModifyExtensionList() {
    std::string modify_extension_list = "";
    std::string env_var = GetEnvarValue(kEnvarDevsimModifyExtensionList);
    if (!env_var.empty()) {
        modify_extension_list = env_var;
        modifyExtensionList.fromEnvVar = true;
    }

    if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsDevsimModifyExtensionList)) {
        modify_extension_list = vku::GetLayerSettingString(kOurLayerName, kLayerSettingsDevsimModifyExtensionList);
        modifyExtensionList.fromEnvVar = false;
    }
    modifyExtensionList.mode = GetArrayCombinationModeValue(modify_extension_list);
}

// Fill the modifyMemoryFlags variable with a value from either vk_layer_settings.txt or environment variables.
// Environment variables get priority.
static void GetDevSimModifyMemoryFlags() {
    std::string env_var = GetEnvarValue(kEnvarDevsimModifyMemoryFlags);
    if (!env_var.empty()) {
        modifyMemoryFlags.num = GetBooleanValue(env_var);
        modifyMemoryFlags.fromEnvVar = true;
    }

    if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsDevsimModifyMemoryFlags)) {
        modifyMemoryFlags.fromEnvVar = false;
        modifyMemoryFlags.num = vku::GetLayerSettingBool(kOurLayerName, kLayerSettingsDevsimModifyMemoryFlags);
    }
}

// Fill the modifyFormatList variable with a value from either vk_layer_settings.txt or environment variables.
// Environment variables get priority.
static void GetDevSimModifyFormatList() {
    std::string modify_format_list = "";
    std::string env_var = GetEnvarValue(kEnvarDevsimModifyFormatList);
    if (!env_var.empty()) {
        modify_format_list = env_var;
        modifyFormatList.fromEnvVar = true;
    }

    if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsDevsimModifyFormatList)) {
        modifyFormatList.fromEnvVar = false;
        modify_format_list = vku::GetLayerSettingString(kOurLayerName, kLayerSettingsDevsimModifyFormatList);
    }
    modifyFormatList.mode = GetArrayCombinationModeValue(modify_format_list);
}

// Fill the modifyFormatProperties variable with a value from either vk_layer_settings.txt or environment variables.
// Environment variables get priority.
static void GetDevSimModifyFormatProperties() {
    std::string modify_format_properties = "";
    std::string env_var = GetEnvarValue(kEnvarDevsimModifyFormatProperties);
    if (!env_var.empty()) {
        modify_format_properties = env_var;
        modifyFormatProperties.fromEnvVar = true;
    }

    if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsDevsimModifyFormatProperties)) {
        modifyFormatProperties.fromEnvVar = false;
        modify_format_properties = vku::GetLayerSettingString(kOurLayerName, kLayerSettingsDevsimModifyFormatProperties);
    }
    modifyFormatProperties.mode = GetArrayCombinationModeValue(modify_format_properties);
}

// Fill the modifySurfaceFormats variable with a value from either vk_layer_settings.txt or environment variables.
// Environment variables get priority.
static void GetDevSimModifySurfaceFormats() {
    std::string modify_surface_formats = "";
    std::string env_var = GetEnvarValue(kEnvarDevsimModifySurfaceFormats);
    if (!env_var.empty()) {
        modify_surface_formats = env_var;
        modifySurfaceFormats.fromEnvVar = true;
    }

    if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsDevsimModifySurfaceFormats)) {
        modifySurfaceFormats.fromEnvVar = false;
        modify_surface_formats = vku::GetLayerSettingString(kOurLayerName, kLayerSettingsDevsimModifySurfaceFormats);
    }
    modifySurfaceFormats.mode = GetArrayCombinationModeValue(modify_surface_formats);
}

// Fill the modifyPresentModes variable with a value from either vk_layer_settings.txt or environment variables.
// Environment variables get priority.
static void GetDevSimModifyPresentModes() {
    std::string modify_present_modes = "";
    std::string env_var = GetEnvarValue(kEnvarDevsimModifyPresentModes);
    if (!env_var.empty()) {
        modify_present_modes = env_var;
        modifyPresentModes.fromEnvVar = true;
    }

    if (vku::IsLayerSetting(kOurLayerName, kLayerSettingsDevsimModifyPresentModes)) {
        modifyPresentModes.fromEnvVar = false;
        modify_present_modes = vku::GetLayerSettingString(kOurLayerName, kLayerSettingsDevsimModifyPresentModes);
    }
    modifyPresentModes.mode = GetArrayCombinationModeValue(modify_present_modes);
}

// Generic layer dispatch table setup, see [LALI].
static VkResult LayerSetupCreateInstance(const VkInstanceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator,
                                         VkInstance *pInstance) {
    GetDevSimEmulatePortability();
    GetDevSimFilename();
    GetDevSimDebugLevel();
    GetDevSimErrorLevel();
    GetDevSimModifyExtensionList();
    GetDevSimModifyMemoryFlags();
    GetDevSimModifyFormatList();
    GetDevSimModifyFormatProperties();
    GetDevSimModifySurfaceFormats();
    GetDevSimModifyPresentModes();

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
    DebugPrintf("%s version %d.%d.%d\n", kOurLayerName, kVersionDevsimMajor, kVersionDevsimMinor, kVersionDevsimPatch);
    DebugPrintf("JsonCpp version %s\n", JSONCPP_VERSION_STRING);

    const VkApplicationInfo *app_info = pCreateInfo->pApplicationInfo;
    const uint32_t requested_version = (app_info && app_info->apiVersion) ? app_info->apiVersion : VK_API_VERSION_1_0;
    if (requested_version > VK_API_VERSION_1_2) {
        DebugPrintf("%s currently only supports VK_API_VERSION_1_2 and lower.\n", kOurLayerName);
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
            // VK_KHR_portability_subset is a special case since it can also be emulated by the DevSim layer.
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME) ||
                    emulatePortability.num > 0) {
                    VkPhysicalDevicePortabilitySubsetPropertiesKHR *psp = (VkPhysicalDevicePortabilitySubsetPropertiesKHR *)place;
                    void *pNext = psp->pNext;
                    *psp = physicalDeviceData->physical_device_portability_subset_properties_;
                    psp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR:
                if ((PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME) ||
                     emulatePortability.num > 0)) {
                    VkPhysicalDevicePortabilitySubsetFeaturesKHR *psf = (VkPhysicalDevicePortabilitySubsetFeaturesKHR *)place;
                    void *pNext = psf->pNext;
                    *psf = physicalDeviceData->physical_device_portability_subset_features_;
                    psf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_8BIT_STORAGE_EXTENSION_NAME)) {
                    VkPhysicalDevice8BitStorageFeaturesKHR *ebsf = (VkPhysicalDevice8BitStorageFeaturesKHR *)place;
                    void *pNext = ebsf->pNext;
                    *ebsf = physicalDeviceData->physical_device_8bit_storage_features_;
                    ebsf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_16BIT_STORAGE_EXTENSION_NAME)) {
                    VkPhysicalDevice16BitStorageFeaturesKHR *sbsf = (VkPhysicalDevice16BitStorageFeaturesKHR *)place;
                    void *pNext = sbsf->pNext;
                    *sbsf = physicalDeviceData->physical_device_16bit_storage_features_;
                    sbsf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME)) {
                    VkPhysicalDeviceBufferDeviceAddressFeaturesKHR *bdaf = (VkPhysicalDeviceBufferDeviceAddressFeaturesKHR *)place;
                    void *pNext = bdaf->pNext;
                    *bdaf = physicalDeviceData->physical_device_buffer_device_address_features_;
                    bdaf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME)) {
                    VkPhysicalDeviceDepthStencilResolvePropertiesKHR *dsrp =
                        (VkPhysicalDeviceDepthStencilResolvePropertiesKHR *)place;
                    void *pNext = dsrp->pNext;
                    *dsrp = physicalDeviceData->physical_device_depth_stencil_resolve_properties_;
                    dsrp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)) {
                    VkPhysicalDeviceDescriptorIndexingPropertiesEXT *dip = (VkPhysicalDeviceDescriptorIndexingPropertiesEXT *)place;
                    void *pNext = dip->pNext;
                    *dip = physicalDeviceData->physical_device_descriptor_indexing_properties_;
                    dip->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)) {
                    VkPhysicalDeviceDescriptorIndexingFeaturesEXT *dif = (VkPhysicalDeviceDescriptorIndexingFeaturesEXT *)place;
                    void *pNext = dif->pNext;
                    *dif = physicalDeviceData->physical_device_descriptor_indexing_features_;
                    dif->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME)) {
                    VkPhysicalDeviceHostQueryResetFeaturesEXT *hqrf = (VkPhysicalDeviceHostQueryResetFeaturesEXT *)place;
                    void *pNext = hqrf->pNext;
                    *hqrf = physicalDeviceData->physical_device_host_query_reset_features_;
                    hqrf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME)) {
                    VkPhysicalDeviceImagelessFramebufferFeaturesKHR *iff = (VkPhysicalDeviceImagelessFramebufferFeaturesKHR *)place;
                    void *pNext = iff->pNext;
                    *iff = physicalDeviceData->physical_device_imageless_framebuffer_features_;
                    iff->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_POINT_CLIPPING_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_MAINTENANCE2_EXTENSION_NAME)) {
                    VkPhysicalDevicePointClippingPropertiesKHR *pcp = (VkPhysicalDevicePointClippingPropertiesKHR *)place;
                    void *pNext = pcp->pNext;
                    *pcp = physicalDeviceData->physical_device_point_clipping_properties_;
                    pcp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_MAINTENANCE3_EXTENSION_NAME)) {
                    VkPhysicalDeviceMaintenance3PropertiesKHR *pcp = (VkPhysicalDeviceMaintenance3PropertiesKHR *)place;
                    void *pNext = pcp->pNext;
                    *pcp = physicalDeviceData->physical_device_maintenance_3_properties_;
                    pcp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_MAINTENANCE_4_EXTENSION_NAME)) {
                    VkPhysicalDeviceMaintenance4FeaturesKHR *m4f = (VkPhysicalDeviceMaintenance4FeaturesKHR *)place;
                    void *pNext = m4f->pNext;
                    *m4f = physicalDeviceData->physical_device_maintenance_4_features_;
                    m4f->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_MAINTENANCE_4_EXTENSION_NAME)) {
                    VkPhysicalDeviceMaintenance4PropertiesKHR *m4p = (VkPhysicalDeviceMaintenance4PropertiesKHR *)place;
                    void *pNext = m4p->pNext;
                    *m4p = physicalDeviceData->physical_device_maintenance_4_properties_;
                    m4p->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_MULTIVIEW_EXTENSION_NAME)) {
                    VkPhysicalDeviceMultiviewPropertiesKHR *mp = (VkPhysicalDeviceMultiviewPropertiesKHR *)place;
                    void *pNext = mp->pNext;
                    *mp = physicalDeviceData->physical_device_multiview_properties_;
                    mp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_MULTIVIEW_EXTENSION_NAME)) {
                    VkPhysicalDeviceMultiviewFeaturesKHR *mf = (VkPhysicalDeviceMultiviewFeaturesKHR *)place;
                    void *pNext = mf->pNext;
                    *mf = physicalDeviceData->physical_device_multiview_features_;
                    mf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_FILTER_MINMAX_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME)) {
                    VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT *sfmp =
                        (VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT *)place;
                    void *pNext = sfmp->pNext;
                    *sfmp = physicalDeviceData->physical_device_sampler_filter_minmax_properties_;
                    sfmp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME)) {
                    VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR *sycf =
                        (VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR *)place;
                    void *pNext = sycf->pNext;
                    *sycf = physicalDeviceData->physical_device_sampler_ycbcr_conversion_features_;
                    sycf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME)) {
                    VkPhysicalDeviceScalarBlockLayoutFeaturesEXT *sblf = (VkPhysicalDeviceScalarBlockLayoutFeaturesEXT *)place;
                    void *pNext = sblf->pNext;
                    *sblf = physicalDeviceData->physical_device_scalar_block_layout_features_;
                    sblf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_SEPARATE_DEPTH_STENCIL_LAYOUTS_EXTENSION_NAME)) {
                    VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR *sdslf =
                        (VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR *)place;
                    void *pNext = sdslf->pNext;
                    *sdslf = physicalDeviceData->physical_device_separate_depth_stencil_layouts_features_;
                    sdslf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_SHADER_ATOMIC_INT64_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderAtomicInt64FeaturesKHR *saisf = (VkPhysicalDeviceShaderAtomicInt64FeaturesKHR *)place;
                    void *pNext = saisf->pNext;
                    *saisf = physicalDeviceData->physical_device_shader_atomic_int64_features_;
                    saisf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT_CONTROLS_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME)) {
                    VkPhysicalDeviceFloatControlsPropertiesKHR *fcp = (VkPhysicalDeviceFloatControlsPropertiesKHR *)place;
                    void *pNext = fcp->pNext;
                    *fcp = physicalDeviceData->physical_device_float_controls_properties_;
                    fcp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderFloat16Int8FeaturesKHR *sfsief = (VkPhysicalDeviceShaderFloat16Int8FeaturesKHR *)place;
                    void *pNext = sfsief->pNext;
                    *sfsief = physicalDeviceData->physical_device_shader_float16_int8_features_;
                    sfsief->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_SHADER_SUBGROUP_EXTENDED_TYPES_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR *ssetf =
                        (VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR *)place;
                    void *pNext = ssetf->pNext;
                    *ssetf = physicalDeviceData->physical_device_shader_subgroup_extended_types_features_;
                    ssetf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME)) {
                    VkPhysicalDeviceTimelineSemaphorePropertiesKHR *tsp = (VkPhysicalDeviceTimelineSemaphorePropertiesKHR *)place;
                    void *pNext = tsp->pNext;
                    *tsp = physicalDeviceData->physical_device_timeline_semaphore_properties_;
                    tsp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME)) {
                    VkPhysicalDeviceTimelineSemaphoreFeaturesKHR *tsf = (VkPhysicalDeviceTimelineSemaphoreFeaturesKHR *)place;
                    void *pNext = tsf->pNext;
                    *tsf = physicalDeviceData->physical_device_timeline_semaphore_features_;
                    tsf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT_EXTENSION_NAME)) {
                    VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR *ubslf =
                        (VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR *)place;
                    void *pNext = ubslf->pNext;
                    *ubslf = physicalDeviceData->physical_device_uniform_buffer_standard_layout_features_;
                    ubslf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME)) {
                    VkPhysicalDeviceVariablePointersFeaturesKHR *vpf = (VkPhysicalDeviceVariablePointersFeaturesKHR *)place;
                    void *pNext = vpf->pNext;
                    *vpf = physicalDeviceData->physical_device_variable_pointers_features_;
                    vpf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_VULKAN_MEMORY_MODEL_EXTENSION_NAME)) {
                    VkPhysicalDeviceVulkanMemoryModelFeaturesKHR *vmmf = (VkPhysicalDeviceVulkanMemoryModelFeaturesKHR *)place;
                    void *pNext = vmmf->pNext;
                    *vmmf = physicalDeviceData->physical_device_vulkan_memory_model_features_;
                    vmmf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_WORKGROUP_MEMORY_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_ZERO_INITIALIZE_WORKGROUP_MEMORY_EXTENSION_NAME)) {
                    VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR *ziwmf =
                        (VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR *)place;
                    void *pNext = ziwmf->pNext;
                    *ziwmf = physicalDeviceData->physical_device_zero_initialize_workgroup_memory_features_;
                    ziwmf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME)) {
                    VkPhysicalDeviceAccelerationStructureFeaturesKHR *asf =
                        (VkPhysicalDeviceAccelerationStructureFeaturesKHR *)place;
                    void *pNext = asf->pNext;
                    *asf = physicalDeviceData->physical_device_acceleration_structure_features_;
                    asf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME)) {
                    VkPhysicalDeviceAccelerationStructurePropertiesKHR *asp =
                        (VkPhysicalDeviceAccelerationStructurePropertiesKHR *)place;
                    void *pNext = asp->pNext;
                    *asp = physicalDeviceData->physical_device_acceleration_structure_properties_;
                    asp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_PERFORMANCE_QUERY_EXTENSION_NAME)) {
                    VkPhysicalDevicePerformanceQueryFeaturesKHR *pqf = (VkPhysicalDevicePerformanceQueryFeaturesKHR *)place;
                    void *pNext = pqf->pNext;
                    *pqf = physicalDeviceData->physical_device_performance_query_features_;
                    pqf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_PERFORMANCE_QUERY_EXTENSION_NAME)) {
                    VkPhysicalDevicePerformanceQueryPropertiesKHR *pqp = (VkPhysicalDevicePerformanceQueryPropertiesKHR *)place;
                    void *pNext = pqp->pNext;
                    *pqp = physicalDeviceData->physical_device_performance_query_properties_;
                    pqp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_EXECUTABLE_PROPERTIES_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_PIPELINE_EXECUTABLE_PROPERTIES_EXTENSION_NAME)) {
                    VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR *pepf =
                        (VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR *)place;
                    void *pNext = pepf->pNext;
                    *pepf = physicalDeviceData->physical_device_pipeline_executable_properties_features_;
                    pepf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_ID_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_PRESENT_ID_EXTENSION_NAME)) {
                    VkPhysicalDevicePresentIdFeaturesKHR *pidf = (VkPhysicalDevicePresentIdFeaturesKHR *)place;
                    void *pNext = pidf->pNext;
                    *pidf = physicalDeviceData->physical_device_present_id_features_;
                    pidf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_WAIT_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_PRESENT_WAIT_EXTENSION_NAME)) {
                    VkPhysicalDevicePresentWaitFeaturesKHR *pwf = (VkPhysicalDevicePresentWaitFeaturesKHR *)place;
                    void *pNext = pwf->pNext;
                    *pwf = physicalDeviceData->physical_device_present_wait_features_;
                    pwf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME)) {
                    VkPhysicalDevicePushDescriptorPropertiesKHR *pdp = (VkPhysicalDevicePushDescriptorPropertiesKHR *)place;
                    void *pNext = pdp->pNext;
                    *pdp = physicalDeviceData->physical_device_push_descriptor_properites_;
                    pdp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_RAY_QUERY_EXTENSION_NAME)) {
                    VkPhysicalDeviceRayQueryFeaturesKHR *rqf = (VkPhysicalDeviceRayQueryFeaturesKHR *)place;
                    void *pNext = rqf->pNext;
                    *rqf = physicalDeviceData->physical_device_ray_query_features_;
                    rqf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME)) {
                    VkPhysicalDeviceRayTracingPipelineFeaturesKHR *rtpf = (VkPhysicalDeviceRayTracingPipelineFeaturesKHR *)place;
                    void *pNext = rtpf->pNext;
                    *rtpf = physicalDeviceData->physical_device_ray_tracing_pipeline_features_;
                    rtpf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME)) {
                    VkPhysicalDeviceRayTracingPipelinePropertiesKHR *rtpp =
                        (VkPhysicalDeviceRayTracingPipelinePropertiesKHR *)place;
                    void *pNext = rtpp->pNext;
                    *rtpp = physicalDeviceData->physical_device_ray_tracing_pipeline_properties_;
                    rtpp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_SHADER_CLOCK_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderClockFeaturesKHR *scf = (VkPhysicalDeviceShaderClockFeaturesKHR *)place;
                    void *pNext = scf->pNext;
                    *scf = physicalDeviceData->physical_device_shader_clock_features_;
                    scf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_SHADER_INTEGER_DOT_PRODUCT_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR *sidpf =
                        (VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR *)place;
                    void *pNext = sidpf->pNext;
                    *sidpf = physicalDeviceData->physical_device_shader_integer_dot_product_features_;
                    sidpf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_SHADER_INTEGER_DOT_PRODUCT_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR *sidpp =
                        (VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR *)place;
                    void *pNext = sidpp->pNext;
                    *sidpp = physicalDeviceData->physical_device_shader_integer_dot_products_properties_;
                    sidpp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData,
                                                     VK_KHR_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR *ssucff =
                        (VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR *)place;
                    void *pNext = ssucff->pNext;
                    *ssucff = physicalDeviceData->physical_device_shader_subgroup_uniform_control_flow_features_;
                    ssucff->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_SHADER_TERMINATE_INVOCATION_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR *stif =
                        (VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR *)place;
                    void *pNext = stif->pNext;
                    *stif = physicalDeviceData->physical_device_shader_terminate_invocation_features_;
                    stif->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_SHADER_TERMINATE_INVOCATION_EXTENSION_NAME)) {
                    VkPhysicalDeviceSynchronization2FeaturesKHR *s2f = (VkPhysicalDeviceSynchronization2FeaturesKHR *)place;
                    void *pNext = s2f->pNext;
                    *s2f = physicalDeviceData->physical_device_synchronization2_features_;
                    s2f->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_EXTENSION_NAME)) {
                    VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR *wmelf =
                        (VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR *)place;
                    void *pNext = wmelf->pNext;
                    *wmelf = physicalDeviceData->physical_device_workgroup_memory_explicit_layout_features_;
                    wmelf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_4444_FORMATS_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_4444_FORMATS_EXTENSION_NAME)) {
                    VkPhysicalDevice4444FormatsFeaturesEXT *ff = (VkPhysicalDevice4444FormatsFeaturesEXT *)place;
                    void *pNext = ff->pNext;
                    *ff = physicalDeviceData->physical_device_4444_formats_features_;
                    ff->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ASTC_DECODE_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_ASTC_DECODE_MODE_EXTENSION_NAME)) {
                    VkPhysicalDeviceASTCDecodeFeaturesEXT *astcdf = (VkPhysicalDeviceASTCDecodeFeaturesEXT *)place;
                    void *pNext = astcdf->pNext;
                    *astcdf = physicalDeviceData->physical_device_astc_decode_features_;
                    astcdf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME)) {
                    VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT *boaf =
                        (VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT *)place;
                    void *pNext = boaf->pNext;
                    *boaf = physicalDeviceData->physical_device_blend_operation_advanced_features_;
                    boaf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME)) {
                    VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT *boap =
                        (VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT *)place;
                    void *pNext = boap->pNext;
                    *boap = physicalDeviceData->physical_device_blend_operation_advanced_properties_;
                    boap->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BORDER_COLOR_SWIZZLE_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_BORDER_COLOR_SWIZZLE_EXTENSION_NAME)) {
                    VkPhysicalDeviceBorderColorSwizzleFeaturesEXT *bcsf = (VkPhysicalDeviceBorderColorSwizzleFeaturesEXT *)place;
                    void *pNext = bcsf->pNext;
                    *bcsf = physicalDeviceData->physical_device_border_color_swizzle_features_;
                    bcsf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COLOR_WRITE_ENABLE_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_COLOR_WRITE_ENABLE_EXTENSION_NAME)) {
                    VkPhysicalDeviceColorWriteEnableFeaturesEXT *cwef = (VkPhysicalDeviceColorWriteEnableFeaturesEXT *)place;
                    void *pNext = cwef->pNext;
                    *cwef = physicalDeviceData->physical_device_color_write_enable_features_;
                    cwef->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONDITIONAL_RENDERING_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_CONDITIONAL_RENDERING_EXTENSION_NAME)) {
                    VkPhysicalDeviceConditionalRenderingFeaturesEXT *crf = (VkPhysicalDeviceConditionalRenderingFeaturesEXT *)place;
                    void *pNext = crf->pNext;
                    *crf = physicalDeviceData->physical_device_conditional_rendering_features_;
                    crf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONSERVATIVE_RASTERIZATION_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME)) {
                    VkPhysicalDeviceConservativeRasterizationPropertiesEXT *crp =
                        (VkPhysicalDeviceConservativeRasterizationPropertiesEXT *)place;
                    void *pNext = crp->pNext;
                    *crp = physicalDeviceData->physical_device_conservative_rasterization_properties_;
                    crp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_CUSTOM_BORDER_COLOR_EXTENSION_NAME)) {
                    VkPhysicalDeviceCustomBorderColorFeaturesEXT *cbcf = (VkPhysicalDeviceCustomBorderColorFeaturesEXT *)place;
                    void *pNext = cbcf->pNext;
                    *cbcf = physicalDeviceData->physical_device_custom_border_color_features_;
                    cbcf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_CUSTOM_BORDER_COLOR_EXTENSION_NAME)) {
                    VkPhysicalDeviceCustomBorderColorPropertiesEXT *cbcp = (VkPhysicalDeviceCustomBorderColorPropertiesEXT *)place;
                    void *pNext = cbcp->pNext;
                    *cbcp = physicalDeviceData->physical_device_custom_border_color_properties_;
                    cbcp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_DEPTH_CLIP_ENABLE_EXTENSION_NAME)) {
                    VkPhysicalDeviceDepthClipEnableFeaturesEXT *dcef = (VkPhysicalDeviceDepthClipEnableFeaturesEXT *)place;
                    void *pNext = dcef->pNext;
                    *dcef = physicalDeviceData->physical_device_depth_clip_enable_features_ext_;
                    dcef->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_MEMORY_REPORT_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_DEVICE_MEMORY_REPORT_EXTENSION_NAME)) {
                    VkPhysicalDeviceDeviceMemoryReportFeaturesEXT *dmrf = (VkPhysicalDeviceDeviceMemoryReportFeaturesEXT *)place;
                    void *pNext = dmrf->pNext;
                    *dmrf = physicalDeviceData->physical_device_device_memory_report_features_;
                    dmrf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DISCARD_RECTANGLE_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_DISCARD_RECTANGLES_EXTENSION_NAME)) {
                    VkPhysicalDeviceDiscardRectanglePropertiesEXT *drp = (VkPhysicalDeviceDiscardRectanglePropertiesEXT *)place;
                    void *pNext = drp->pNext;
                    *drp = physicalDeviceData->physical_device_discard_rectangle_properties_;
                    drp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME)) {
                    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT *edsf =
                        (VkPhysicalDeviceExtendedDynamicStateFeaturesEXT *)place;
                    void *pNext = edsf->pNext;
                    *edsf = physicalDeviceData->physical_device_extended_dynamic_state_features_;
                    edsf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME)) {
                    VkPhysicalDeviceExtendedDynamicState2FeaturesEXT *eds2f =
                        (VkPhysicalDeviceExtendedDynamicState2FeaturesEXT *)place;
                    void *pNext = eds2f->pNext;
                    *eds2f = physicalDeviceData->physical_device_extended_dynamic_state2_features_;
                    eds2f->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_HOST_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_EXTERNAL_MEMORY_HOST_EXTENSION_NAME)) {
                    VkPhysicalDeviceExternalMemoryHostPropertiesEXT *emhp =
                        (VkPhysicalDeviceExternalMemoryHostPropertiesEXT *)place;
                    void *pNext = emhp->pNext;
                    *emhp = physicalDeviceData->physical_device_external_memory_host_properties_;
                    emhp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_FRAGMENT_DENSITY_MAP_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentDensityMapFeaturesEXT *fdmf = (VkPhysicalDeviceFragmentDensityMapFeaturesEXT *)place;
                    void *pNext = fdmf->pNext;
                    *fdmf = physicalDeviceData->physical_device_fragment_density_map_features_;
                    fdmf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_FRAGMENT_DENSITY_MAP_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentDensityMapPropertiesEXT *fdmp =
                        (VkPhysicalDeviceFragmentDensityMapPropertiesEXT *)place;
                    void *pNext = fdmp->pNext;
                    *fdmp = physicalDeviceData->physical_device_fragment_density_map_properties_;
                    fdmp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT *fsif =
                        (VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT *)place;
                    void *pNext = fsif->pNext;
                    *fsif = physicalDeviceData->physical_device_fragment_shader_interlock_features_;
                    fsif->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GLOBAL_PRIORITY_QUERY_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_GLOBAL_PRIORITY_QUERY_EXTENSION_NAME)) {
                    VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT *gpqf = (VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT *)place;
                    void *pNext = gpqf->pNext;
                    *gpqf = physicalDeviceData->physical_device_global_priority_query_features_;
                    gpqf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_IMAGE_ROBUSTNESS_EXTENSION_NAME)) {
                    VkPhysicalDeviceImageRobustnessFeaturesEXT *irf = (VkPhysicalDeviceImageRobustnessFeaturesEXT *)place;
                    void *pNext = irf->pNext;
                    *irf = physicalDeviceData->physical_device_image_robustness_features_;
                    irf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_INDEX_TYPE_UINT8_EXTENSION_NAME)) {
                    VkPhysicalDeviceIndexTypeUint8FeaturesEXT *itu8f = (VkPhysicalDeviceIndexTypeUint8FeaturesEXT *)place;
                    void *pNext = itu8f->pNext;
                    *itu8f = physicalDeviceData->physical_device_index_type_uint8_features_;
                    itu8f->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME)) {
                    VkPhysicalDeviceInlineUniformBlockFeaturesEXT *iubf = (VkPhysicalDeviceInlineUniformBlockFeaturesEXT *)place;
                    void *pNext = iubf->pNext;
                    *iubf = physicalDeviceData->physical_device_inline_uniform_block_features_;
                    iubf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME)) {
                    VkPhysicalDeviceInlineUniformBlockPropertiesEXT *iubp =
                        (VkPhysicalDeviceInlineUniformBlockPropertiesEXT *)place;
                    void *pNext = iubp->pNext;
                    *iubp = physicalDeviceData->physical_device_inline_uniform_block_properties_;
                    iubp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME)) {
                    VkPhysicalDeviceLineRasterizationFeaturesEXT *lrf = (VkPhysicalDeviceLineRasterizationFeaturesEXT *)place;
                    void *pNext = lrf->pNext;
                    *lrf = physicalDeviceData->physical_device_line_rasterization_features_;
                    lrf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME)) {
                    VkPhysicalDeviceLineRasterizationPropertiesEXT *lrp = (VkPhysicalDeviceLineRasterizationPropertiesEXT *)place;
                    void *pNext = lrp->pNext;
                    *lrp = physicalDeviceData->physical_device_line_rasterization_properties_;
                    lrp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PRIORITY_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME)) {
                    VkPhysicalDeviceMemoryPriorityFeaturesEXT *mpf = (VkPhysicalDeviceMemoryPriorityFeaturesEXT *)place;
                    void *pNext = mpf->pNext;
                    *mpf = physicalDeviceData->physical_device_memory_priority_features_;
                    mpf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_MULTI_DRAW_EXTENSION_NAME)) {
                    VkPhysicalDeviceMultiDrawFeaturesEXT *mdf = (VkPhysicalDeviceMultiDrawFeaturesEXT *)place;
                    void *pNext = mdf->pNext;
                    *mdf = physicalDeviceData->physical_device_multi_draw_features_;
                    mdf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_MULTI_DRAW_EXTENSION_NAME)) {
                    VkPhysicalDeviceMultiDrawPropertiesEXT *mdp = (VkPhysicalDeviceMultiDrawPropertiesEXT *)place;
                    void *pNext = mdp->pNext;
                    *mdp = physicalDeviceData->physical_device_multi_draw_properties_;
                    mdp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PAGEABLE_DEVICE_LOCAL_MEMORY_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_PAGEABLE_DEVICE_LOCAL_MEMORY_EXTENSION_NAME)) {
                    VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT *pdlmf =
                        (VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT *)place;
                    void *pNext = pdlmf->pNext;
                    *pdlmf = physicalDeviceData->physical_device_pageable_device_local_memory_features_;
                    pdlmf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_PIPELINE_CREATION_CACHE_CONTROL_EXTENSION_NAME)) {
                    VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT *pcccf =
                        (VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT *)place;
                    void *pNext = pcccf->pNext;
                    *pcccf = physicalDeviceData->physical_device_pipeline_creation_cache_control_features_;
                    pcccf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVE_TOPOLOGY_LIST_RESTART_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_PRIMITIVE_TOPOLOGY_LIST_RESTART_EXTENSION_NAME)) {
                    VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT *ptlrf =
                        (VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT *)place;
                    void *pNext = ptlrf->pNext;
                    *ptlrf = physicalDeviceData->physical_device_primitive_topology_list_restart_features_;
                    ptlrf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_PRIVATE_DATA_EXTENSION_NAME)) {
                    VkPhysicalDevicePrivateDataFeaturesEXT *pdf = (VkPhysicalDevicePrivateDataFeaturesEXT *)place;
                    void *pNext = pdf->pNext;
                    *pdf = physicalDeviceData->physical_device_private_data_features_;
                    pdf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_PROVOKING_VERTEX_EXTENSION_NAME)) {
                    VkPhysicalDeviceProvokingVertexFeaturesEXT *pvf = (VkPhysicalDeviceProvokingVertexFeaturesEXT *)place;
                    void *pNext = pvf->pNext;
                    *pvf = physicalDeviceData->physical_device_provoking_vertex_features_;
                    pvf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_PROVOKING_VERTEX_EXTENSION_NAME)) {
                    VkPhysicalDeviceProvokingVertexPropertiesEXT *pvp = (VkPhysicalDeviceProvokingVertexPropertiesEXT *)place;
                    void *pNext = pvp->pNext;
                    *pvp = physicalDeviceData->physical_device_provoking_vertex_properties_;
                    pvp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RGBA10X6_FORMATS_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_RGBA10X6_FORMATS_EXTENSION_NAME)) {
                    VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT *rgba10x6ff = (VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT *)place;
                    void *pNext = rgba10x6ff->pNext;
                    *rgba10x6ff = physicalDeviceData->physical_device_rgba10x6_formats_features_;
                    rgba10x6ff->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_ROBUSTNESS_2_EXTENSION_NAME)) {
                    VkPhysicalDeviceRobustness2FeaturesEXT *r2f = (VkPhysicalDeviceRobustness2FeaturesEXT *)place;
                    void *pNext = r2f->pNext;
                    *r2f = physicalDeviceData->physical_device_robustness_2_features_;
                    r2f->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_ROBUSTNESS_2_EXTENSION_NAME)) {
                    VkPhysicalDeviceRobustness2PropertiesEXT *r2p = (VkPhysicalDeviceRobustness2PropertiesEXT *)place;
                    void *pNext = r2p->pNext;
                    *r2p = physicalDeviceData->physical_device_robustness_2_properties_;
                    r2p->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLE_LOCATIONS_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_SAMPLE_LOCATIONS_EXTENSION_NAME)) {
                    VkPhysicalDeviceSampleLocationsPropertiesEXT *slp = (VkPhysicalDeviceSampleLocationsPropertiesEXT *)place;
                    void *pNext = slp->pNext;
                    *slp = physicalDeviceData->physical_device_sample_locations_properties_;
                    slp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderAtomicFloatFeaturesEXT *saff = (VkPhysicalDeviceShaderAtomicFloatFeaturesEXT *)place;
                    void *pNext = saff->pNext;
                    *saff = physicalDeviceData->physical_device_shader_atomic_float_features_;
                    saff->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_2_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_SHADER_ATOMIC_FLOAT_2_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT *saf2f = (VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT *)place;
                    void *pNext = saf2f->pNext;
                    *saf2f = physicalDeviceData->physical_device_shader_atomic_float2_features_;
                    saf2f->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData,
                                                     VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT *sdthif =
                        (VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT *)place;
                    void *pNext = sdthif->pNext;
                    *sdthif = physicalDeviceData->physical_device_shader_demote_to_helper_invocation_features_;
                    sdthif->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_ATOMIC_INT64_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_SHADER_IMAGE_ATOMIC_INT64_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT *siai64f =
                        (VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT *)place;
                    void *pNext = siai64f->pNext;
                    *siai64f = physicalDeviceData->physical_device_shader_image_atomic_int64_features_;
                    siai64f->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME)) {
                    VkPhysicalDeviceSubgroupSizeControlFeaturesEXT *sscf = (VkPhysicalDeviceSubgroupSizeControlFeaturesEXT *)place;
                    void *pNext = sscf->pNext;
                    *sscf = physicalDeviceData->physical_device_subgroup_size_control_features_;
                    sscf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME)) {
                    VkPhysicalDeviceSubgroupSizeControlPropertiesEXT *sscp =
                        (VkPhysicalDeviceSubgroupSizeControlPropertiesEXT *)place;
                    void *pNext = sscp->pNext;
                    *sscp = physicalDeviceData->physical_device_subgroup_size_control_properties_;
                    sscp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME)) {
                    VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT *tbaf =
                        (VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT *)place;
                    void *pNext = tbaf->pNext;
                    *tbaf = physicalDeviceData->physical_device_texel_buffer_alignment_features_;
                    tbaf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME)) {
                    VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT *tbap =
                        (VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT *)place;
                    void *pNext = tbap->pNext;
                    *tbap = physicalDeviceData->physical_device_texel_buffer_alignment_properties_;
                    tbap->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_HDR_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_TEXTURE_COMPRESSION_ASTC_HDR_EXTENSION_NAME)) {
                    VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT *tcastchdrf =
                        (VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT *)place;
                    void *pNext = tcastchdrf->pNext;
                    *tcastchdrf = physicalDeviceData->physical_device_texture_compression_astc_hdr_features_;
                    tcastchdrf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME)) {
                    VkPhysicalDeviceTransformFeedbackFeaturesEXT *tff = (VkPhysicalDeviceTransformFeedbackFeaturesEXT *)place;
                    void *pNext = tff->pNext;
                    *tff = physicalDeviceData->physical_device_transform_feedback_features_;
                    tff->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME)) {
                    VkPhysicalDeviceTransformFeedbackPropertiesEXT *tfp = (VkPhysicalDeviceTransformFeedbackPropertiesEXT *)place;
                    void *pNext = tfp->pNext;
                    *tfp = physicalDeviceData->physical_device_transform_feedback_properties_;
                    tfp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME)) {
                    VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT *vadf =
                        (VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT *)place;
                    void *pNext = vadf->pNext;
                    *vadf = physicalDeviceData->physical_device_vertex_attribute_divisor_features_;
                    vadf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_PROPERTIES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME)) {
                    VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT *vadp =
                        (VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT *)place;
                    void *pNext = vadp->pNext;
                    *vadp = physicalDeviceData->physical_device_vertex_attirbute_divisor_properties_;
                    vadp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_INPUT_DYNAMIC_STATE_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME)) {
                    VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT *vidsf =
                        (VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT *)place;
                    void *pNext = vidsf->pNext;
                    *vidsf = physicalDeviceData->physical_device_vertex_input_dynamic_state_features_;
                    vidsf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_2_PLANE_444_FORMATS_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_YCBCR_2PLANE_444_FORMATS_EXTENSION_NAME)) {
                    VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT *y2pff =
                        (VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT *)place;
                    void *pNext = y2pff->pNext;
                    *y2pff = physicalDeviceData->physical_device_ycbcr_2plane_444_formats_features_;
                    y2pff->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_IMAGE_ARRAYS_FEATURES_EXT:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_EXT_YCBCR_IMAGE_ARRAYS_EXTENSION_NAME)) {
                    VkPhysicalDeviceYcbcrImageArraysFeaturesEXT *yiaf = (VkPhysicalDeviceYcbcrImageArraysFeaturesEXT *)place;
                    void *pNext = yiaf->pNext;
                    *yiaf = physicalDeviceData->physical_device_ycbcr_image_arrays_features_;
                    yiaf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentShadingRateFeaturesKHR *fsrf = (VkPhysicalDeviceFragmentShadingRateFeaturesKHR *)place;
                    void *pNext = fsrf->pNext;
                    *fsrf = physicalDeviceData->physical_device_fragment_shading_rate_features_;
                    fsrf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME)) {
                    VkPhysicalDeviceFragmentShadingRatePropertiesKHR *fsrp =
                        (VkPhysicalDeviceFragmentShadingRatePropertiesKHR *)place;
                    void *pNext = fsrp->pNext;
                    *fsrp = physicalDeviceData->physical_device_fragment_shading_rate_properties_;
                    fsrp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COHERENT_MEMORY_FEATURES_AMD:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME)) {
                    VkPhysicalDeviceCoherentMemoryFeaturesAMD *cmf = (VkPhysicalDeviceCoherentMemoryFeaturesAMD *)place;
                    void *pNext = cmf->pNext;
                    *cmf = physicalDeviceData->physical_device_coherent_memory_features_;
                    cmf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_AMD:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_AMD_SHADER_CORE_PROPERTIES_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderCorePropertiesAMD *scp = (VkPhysicalDeviceShaderCorePropertiesAMD *)place;
                    void *pNext = scp->pNext;
                    *scp = physicalDeviceData->physical_device_shader_core_properties_;
                    scp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_2_AMD:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_AMD_SHADER_CORE_PROPERTIES_2_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderCoreProperties2AMD *scp2 = (VkPhysicalDeviceShaderCoreProperties2AMD *)place;
                    void *pNext = scp2->pNext;
                    *scp2 = physicalDeviceData->physical_device_shader_core_properties_2_;
                    scp2->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INVOCATION_MASK_FEATURES_HUAWEI:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_HUAWEI_INVOCATION_MASK_EXTENSION_NAME)) {
                    VkPhysicalDeviceInvocationMaskFeaturesHUAWEI *imf = (VkPhysicalDeviceInvocationMaskFeaturesHUAWEI *)place;
                    void *pNext = imf->pNext;
                    *imf = physicalDeviceData->physical_device_invocation_mask_features_;
                    imf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_SHADING_FEATURES_HUAWEI:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_HUAWEI_INVOCATION_MASK_EXTENSION_NAME)) {
                    VkPhysicalDeviceSubpassShadingFeaturesHUAWEI *shf = (VkPhysicalDeviceSubpassShadingFeaturesHUAWEI *)place;
                    void *pNext = shf->pNext;
                    *shf= physicalDeviceData->physical_device_subpass_shading_features_;
                    shf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_SHADING_PROPERTIES_HUAWEI:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_HUAWEI_INVOCATION_MASK_EXTENSION_NAME)) {
                    VkPhysicalDeviceSubpassShadingPropertiesHUAWEI *shp = (VkPhysicalDeviceSubpassShadingPropertiesHUAWEI *)place;
                    void *pNext = shp->pNext;
                    *shp = physicalDeviceData->physical_device_subpass_shading_properties_;
                    shp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_FUNCTIONS_2_FEATURES_INTEL:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_INTEL_SHADER_INTEGER_FUNCTIONS_2_EXTENSION_NAME)) {
                    VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL *sif2f =
                        (VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL *)place;
                    void *pNext = sif2f->pNext;
                    *sif2f = physicalDeviceData->physical_device_shader_integer_functions_2_features_;
                    sif2f->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMPUTE_SHADER_DERIVATIVES_FEATURES_NV:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_NV_COMPUTE_SHADER_DERIVATIVES_EXTENSION_NAME)) {
                    VkPhysicalDeviceComputeShaderDerivativesFeaturesNV *csdf =
                        (VkPhysicalDeviceComputeShaderDerivativesFeaturesNV *)place;
                    void *pNext = csdf->pNext;
                    *csdf = physicalDeviceData->physical_device_compute_shader_derivatives_features_;
                    csdf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_NV:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_NV_COOPERATIVE_MATRIX_EXTENSION_NAME)) {
                    VkPhysicalDeviceCooperativeMatrixFeaturesNV *cmf = (VkPhysicalDeviceCooperativeMatrixFeaturesNV *)place;
                    void *pNext = cmf->pNext;
                    *cmf = physicalDeviceData->physical_device_cooperative_matrix_features_;
                    cmf->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_PROPERTIES_NV:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_NV_COOPERATIVE_MATRIX_EXTENSION_NAME)) {
                    VkPhysicalDeviceCooperativeMatrixPropertiesNV *cmp = (VkPhysicalDeviceCooperativeMatrixPropertiesNV *)place;
                    void *pNext = cmp->pNext;
                    *cmp = physicalDeviceData->physical_device_cooperative_matrix_properties_;
                    cmp->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CORNER_SAMPLED_IMAGE_FEATURES_NV:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_NV_CORNER_SAMPLED_IMAGE_EXTENSION_NAME)) {
                    VkPhysicalDeviceCornerSampledImageFeaturesNV *csif = (VkPhysicalDeviceCornerSampledImageFeaturesNV *)place;
                    void *pNext = csif->pNext;
                    *csif = physicalDeviceData->physical_device_corner_sampled_image_features_;
                    csif->pNext = pNext;
                }
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COVERAGE_REDUCTION_MODE_FEATURES_NV:
                if (PhysicalDeviceData::HasExtension(physicalDeviceData, VK_NV_COVERAGE_REDUCTION_MODE_EXTENSION_NAME)) {
                    VkPhysicalDeviceCoverageReductionModeFeaturesNV *crmf =
                        (VkPhysicalDeviceCoverageReductionModeFeaturesNV *)place;
                    void *pNext = crmf->pNext;
                    *crmf = physicalDeviceData->physical_device_coverage_reduction_mode_features_;
                    crmf->pNext = pNext;
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

    PhysicalDeviceData *pdd = PhysicalDeviceData::Find(physicalDevice);
    const uint32_t src_count = (pdd) ? static_cast<uint32_t>(pdd->simulation_extensions_.size()) : 0;
    if (pLayerName) {
        if (strcmp(pLayerName, kOurLayerName) == 0)
            result = EnumerateProperties(kDeviceExtensionPropertiesCount, kDeviceExtensionProperties.data(), pCount, pProperties);
        else
            result = dt->EnumerateDeviceExtensionProperties(physicalDevice, pLayerName, pCount, pProperties);
    } else if (src_count == 0 || pdd->extension_list_combination_mode_ == ARRAY_COMBINATION_MODE_NONE) {
        result = dt->EnumerateDeviceExtensionProperties(physicalDevice, pLayerName, pCount, pProperties);
    } else {
        result = EnumerateProperties(src_count, pdd->simulation_extensions_.data(), pCount, pProperties);
    }

    if (result == VK_SUCCESS && !pLayerName && emulatePortability.num > 0 &&
        !PhysicalDeviceData::HasSimulatedOrRealExtension(physicalDevice, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME)) {
        if (pProperties) {
            strncpy(pProperties[(*pCount) - 1].extensionName, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME, VK_MAX_EXTENSION_NAME_SIZE);
            pProperties[(*pCount) - 1].specVersion = VK_KHR_PORTABILITY_SUBSET_SPEC_VERSION;
        } else {
            *pCount += 1;
        }
    }

    return result;
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice,
                                                             VkPhysicalDeviceMemoryProperties *pMemoryProperties) {
    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(physicalDevice);

    // Are there JSON overrides, or should we call down to return the original values?
    PhysicalDeviceData *pdd = PhysicalDeviceData::Find(physicalDevice);
    if (pdd) {
        if (modifyMemoryFlags.num > 0) {
            *pMemoryProperties = pdd->physical_device_memory_properties_;
        } else {
            dt->GetPhysicalDeviceMemoryProperties(physicalDevice, pMemoryProperties);
            uint32_t min_memory_heap_count =
                pMemoryProperties->memoryHeapCount < pdd->physical_device_memory_properties_.memoryHeapCount
                    ? pMemoryProperties->memoryHeapCount
                    : pdd->physical_device_memory_properties_.memoryHeapCount;
            pMemoryProperties->memoryHeapCount = min_memory_heap_count;
            for (uint32_t i = 0; i < min_memory_heap_count; i++) {
                pMemoryProperties->memoryHeaps[i].size = pdd->physical_device_memory_properties_.memoryHeaps[i].size;
            }
        }
    } else {
        dt->GetPhysicalDeviceMemoryProperties(physicalDevice, pMemoryProperties);
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceMemoryProperties2(VkPhysicalDevice physicalDevice,
                                                              VkPhysicalDeviceMemoryProperties2KHR *pMemoryProperties) {
    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(physicalDevice);
    dt->GetPhysicalDeviceMemoryProperties2(physicalDevice, pMemoryProperties);
    GetPhysicalDeviceMemoryProperties(physicalDevice, &pMemoryProperties->memoryProperties);
    if (modifyMemoryFlags.num > 0) {
        PhysicalDeviceData *pdd = PhysicalDeviceData::Find(physicalDevice);
        FillPNextChain(pdd, pMemoryProperties->pNext);
    }
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceMemoryProperties2KHR(VkPhysicalDevice physicalDevice,
                                                                 VkPhysicalDeviceMemoryProperties2KHR *pMemoryProperties) {
    GetPhysicalDeviceMemoryProperties2(physicalDevice, pMemoryProperties);
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
    } else {
        EnumerateProperties(src_count, pdd->arrayof_queue_family_properties_.data(), pQueueFamilyPropertyCount,
                            pQueueFamilyProperties);
    }
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
    const VkQueueFamilyProperties *src_props = pdd->arrayof_queue_family_properties_.data();
    for (uint32_t i = 0; i < copy_count; ++i) {
        pQueueFamilyProperties2[i].queueFamilyProperties = src_props[i];
    }
    *pQueueFamilyPropertyCount = copy_count;
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceQueueFamilyProperties2(VkPhysicalDevice physicalDevice,
                                                                   uint32_t *pQueueFamilyPropertyCount,
                                                                   VkQueueFamilyProperties2KHR *pQueueFamilyProperties2) {
    GetPhysicalDeviceQueueFamilyProperties2KHR(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties2);
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
            case ARRAY_COMBINATION_MODE_NONE:
                *pFormatProperties = device_format;
                break;
            case ARRAY_COMBINATION_MODE_REPLACE:
                *pFormatProperties = (iter != pdd->arrayof_format_properties_.end()) ? iter->second : VkFormatProperties{};
                break;
            case ARRAY_COMBINATION_MODE_WHITELIST:
                *pFormatProperties = (iter != pdd->arrayof_format_properties_.end()) ? device_format : VkFormatProperties{};
                break;
            case ARRAY_COMBINATION_MODE_BLACKLIST:
                *pFormatProperties = (iter != pdd->arrayof_format_properties_.end()) ? VkFormatProperties{} : device_format;
                break;
            case ARRAY_COMBINATION_MODE_INTERSECT:
                *pFormatProperties = IsFormatSupported(device_format)
                                         ? device_format
                                         : (iter != pdd->arrayof_format_properties_.end()) ? iter->second : VkFormatProperties{};
                break;
            default:
                *pFormatProperties = device_format;
        }

        if (IsFormatSupported(*pFormatProperties) && iter != pdd->arrayof_format_properties_.end()) {
            switch (pdd->format_properties_combination_mode_) {
                case ARRAY_COMBINATION_MODE_NONE:
                    *pFormatProperties = device_format;
                    break;
                case ARRAY_COMBINATION_MODE_REPLACE:
                    *pFormatProperties = iter->second;
                    break;
                case ARRAY_COMBINATION_MODE_WHITELIST:
                    (*pFormatProperties).optimalTilingFeatures =
                        device_format.optimalTilingFeatures & iter->second.optimalTilingFeatures;
                    (*pFormatProperties).linearTilingFeatures =
                        device_format.linearTilingFeatures & iter->second.linearTilingFeatures;
                    (*pFormatProperties).bufferFeatures = device_format.bufferFeatures & iter->second.bufferFeatures;
                    break;
                case ARRAY_COMBINATION_MODE_BLACKLIST:
                    (*pFormatProperties).optimalTilingFeatures =
                        device_format.optimalTilingFeatures ^ iter->second.optimalTilingFeatures;
                    (*pFormatProperties).linearTilingFeatures =
                        device_format.linearTilingFeatures ^ iter->second.linearTilingFeatures;
                    (*pFormatProperties).bufferFeatures = device_format.bufferFeatures ^ iter->second.bufferFeatures;

                    (*pFormatProperties).optimalTilingFeatures &= device_format.optimalTilingFeatures;
                    (*pFormatProperties).linearTilingFeatures &= device_format.linearTilingFeatures;
                    (*pFormatProperties).bufferFeatures &= device_format.bufferFeatures;
                    break;
                case ARRAY_COMBINATION_MODE_INTERSECT:
                    (*pFormatProperties).optimalTilingFeatures =
                        device_format.optimalTilingFeatures | iter->second.optimalTilingFeatures;
                    (*pFormatProperties).linearTilingFeatures =
                        device_format.linearTilingFeatures | iter->second.linearTilingFeatures;
                    (*pFormatProperties).bufferFeatures = device_format.bufferFeatures | iter->second.bufferFeatures;
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
    if (pdd->format_list_combination_mode_ == ARRAY_COMBINATION_MODE_NONE) {
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

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                                                                       VkSurfaceCapabilitiesKHR *pSurfaceCapabilities) {
    VkResult result = VK_SUCCESS;
    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(physicalDevice);

    result = dt->GetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, pSurfaceCapabilities);
    if (result != VK_SUCCESS) return result;

    PhysicalDeviceData *pdd = PhysicalDeviceData::Find(physicalDevice);
    if (pdd) {
        VkSurfaceCapabilitiesKHR surf_caps = pdd->surface_capabilities_;
        surf_caps.currentExtent = pSurfaceCapabilities->currentExtent;
        surf_caps.currentTransform = pSurfaceCapabilities->currentTransform;

        if (surf_caps.minImageCount == 0) surf_caps.minImageCount = pSurfaceCapabilities->minImageCount;
        if (surf_caps.maxImageCount == 0) surf_caps.maxImageCount = pSurfaceCapabilities->maxImageCount;
        if (surf_caps.minImageExtent.width == 0 && surf_caps.minImageExtent.height == 0)
            surf_caps.minImageExtent = pSurfaceCapabilities->minImageExtent;
        if (surf_caps.maxImageExtent.width == 0 && surf_caps.maxImageExtent.height == 0)
            surf_caps.maxImageExtent = pSurfaceCapabilities->maxImageExtent;
        if (surf_caps.maxImageArrayLayers == 0) surf_caps.maxImageArrayLayers = pSurfaceCapabilities->maxImageArrayLayers;
        if (surf_caps.supportedTransforms == 0) surf_caps.supportedTransforms = pSurfaceCapabilities->supportedTransforms;
        if (surf_caps.supportedCompositeAlpha == 0)
            surf_caps.supportedCompositeAlpha = pSurfaceCapabilities->supportedCompositeAlpha;
        if (surf_caps.supportedUsageFlags == 0) surf_caps.supportedUsageFlags = pSurfaceCapabilities->supportedUsageFlags;

        *pSurfaceCapabilities = surf_caps;
    }

    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceCapabilities2KHR(VkPhysicalDevice physicalDevice,
                                                                        VkPhysicalDeviceSurfaceInfo2KHR *pSurfaceInfo,
                                                                        VkSurfaceCapabilities2KHR *pSurfaceCapabilities) {
    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(physicalDevice);
    dt->GetPhysicalDeviceSurfaceCapabilities2KHR(physicalDevice, pSurfaceInfo, pSurfaceCapabilities);
    return GetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, pSurfaceInfo->surface,
                                                   &pSurfaceCapabilities->surfaceCapabilities);
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                                                                  uint32_t *pSurfaceFormatCount,
                                                                  VkSurfaceFormatKHR *pSurfaceFormats) {
    VkResult result = VK_SUCCESS;
    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(physicalDevice);

    PhysicalDeviceData *pdd = PhysicalDeviceData::Find(physicalDevice);
    uint32_t src_count = (pdd) ? static_cast<uint32_t>(pdd->arrayof_surface_formats_.size()) : 0;
    if (src_count == 0) {
        result = dt->GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, pSurfaceFormatCount, pSurfaceFormats);
    } else {
        uint32_t sf_count = 0;
        dt->GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &sf_count, nullptr);
        std::vector<VkSurfaceFormatKHR> device_surface_formats(sf_count);
        dt->GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &sf_count, device_surface_formats.data());

        std::vector<VkSurfaceFormatKHR> simulation_surface_formats;

        switch (modifySurfaceFormats.mode) {
            case ARRAY_COMBINATION_MODE_NONE:
                return dt->GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, pSurfaceFormatCount, pSurfaceFormats);
                break;
            case ARRAY_COMBINATION_MODE_REPLACE:
                return EnumerateProperties(src_count, pdd->arrayof_surface_formats_.data(), pSurfaceFormatCount, pSurfaceFormats);
                break;
            case ARRAY_COMBINATION_MODE_WHITELIST:
                for (VkSurfaceFormatKHR &dev_surf_form : device_surface_formats) {
                    for (VkSurfaceFormatKHR &file_surf_form : pdd->arrayof_surface_formats_) {
                        if (dev_surf_form.format == file_surf_form.format &&
                            dev_surf_form.colorSpace == file_surf_form.colorSpace) {
                            simulation_surface_formats.push_back(dev_surf_form);
                            break;
                        }
                    }
                }
                break;
            case ARRAY_COMBINATION_MODE_BLACKLIST:
                for (VkSurfaceFormatKHR &dev_surf_form : device_surface_formats) {
                    bool blacklisted = false;
                    for (VkSurfaceFormatKHR &file_surf_form : pdd->arrayof_surface_formats_) {
                        if (dev_surf_form.format == file_surf_form.format &&
                            dev_surf_form.colorSpace == file_surf_form.colorSpace) {
                            blacklisted = true;
                            break;
                        }
                    }
                    if (!blacklisted) {
                        simulation_surface_formats.push_back(dev_surf_form);
                    }
                }
                break;
            case ARRAY_COMBINATION_MODE_INTERSECT:
                for (VkSurfaceFormatKHR &dev_surf_form : device_surface_formats) {
                    simulation_surface_formats.push_back(dev_surf_form);
                }
                for (VkSurfaceFormatKHR &file_surf_form : pdd->arrayof_surface_formats_) {
                    bool blacklisted = false;
                    for (VkSurfaceFormatKHR &sim_surf_form : simulation_surface_formats) {
                        if (sim_surf_form.format == file_surf_form.format &&
                            sim_surf_form.colorSpace == file_surf_form.colorSpace) {
                            blacklisted = true;
                            break;
                        }
                    }
                    if (!blacklisted) {
                        simulation_surface_formats.push_back(file_surf_form);
                    }
                }
                break;
            default:
                return dt->GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, pSurfaceFormatCount, pSurfaceFormats);
        }

        result = EnumerateProperties(static_cast<uint32_t>(simulation_surface_formats.size()), simulation_surface_formats.data(),
                                     pSurfaceFormatCount, pSurfaceFormats);
    }

    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceFormats2KHR(VkPhysicalDevice physicalDevice,
                                                                   const VkPhysicalDeviceSurfaceInfo2KHR *pSurfaceInfo,
                                                                   uint32_t *pSurfaceFormatCount,
                                                                   VkSurfaceFormat2KHR *pSurfaceFormats) {
    VkResult result = VK_SUCCESS;
    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(physicalDevice);

    // Are there JSON overrides, or should we call down to return the original values?
    PhysicalDeviceData *pdd = PhysicalDeviceData::Find(physicalDevice);
    uint32_t src_count = (pdd) ? static_cast<uint32_t>(pdd->arrayof_surface_formats_.size()) : 0;
    if (src_count == 0) {
        result = dt->GetPhysicalDeviceSurfaceFormats2KHR(physicalDevice, pSurfaceInfo, pSurfaceFormatCount, pSurfaceFormats);
        return result;
    }

    uint32_t sf_count = 0;
    dt->GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, pSurfaceInfo->surface, &sf_count, nullptr);
    std::vector<VkSurfaceFormatKHR> device_surface_formats(sf_count);
    dt->GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, pSurfaceInfo->surface, &sf_count, device_surface_formats.data());

    std::vector<VkSurfaceFormatKHR> simulation_surface_formats;

    switch (modifySurfaceFormats.mode) {
        case ARRAY_COMBINATION_MODE_NONE:
            result = dt->GetPhysicalDeviceSurfaceFormats2KHR(physicalDevice, pSurfaceInfo, pSurfaceFormatCount, pSurfaceFormats);
            return result;
            break;
        case ARRAY_COMBINATION_MODE_REPLACE:
            if (!pSurfaceFormats) {
                *pSurfaceFormatCount = src_count;
                return result;
            }

            // You can't directly initialize a variable in a switch case because of scope issues, so we explicitly add a scope here.
            {
                if (*pSurfaceFormatCount < src_count) result = VK_INCOMPLETE;
                const uint32_t copy_count = (*pSurfaceFormatCount < src_count) ? *pSurfaceFormatCount : src_count;
                const VkSurfaceFormatKHR *src_props = pdd->arrayof_surface_formats_.data();
                for (uint32_t i = 0; i < copy_count; ++i) {
                    pSurfaceFormats[i].surfaceFormat = src_props[i];
                }
                *pSurfaceFormatCount = copy_count;
            }

            return result;
            break;
        case ARRAY_COMBINATION_MODE_WHITELIST:
            for (VkSurfaceFormatKHR &dev_surf_form : device_surface_formats) {
                for (VkSurfaceFormatKHR &file_surf_form : pdd->arrayof_surface_formats_) {
                    if (dev_surf_form.format == file_surf_form.format && dev_surf_form.colorSpace == file_surf_form.colorSpace) {
                        simulation_surface_formats.push_back(dev_surf_form);
                        break;
                    }
                }
            }
            break;
        case ARRAY_COMBINATION_MODE_BLACKLIST:
            for (VkSurfaceFormatKHR &dev_surf_form : device_surface_formats) {
                bool blacklisted = false;
                for (VkSurfaceFormatKHR &file_surf_form : pdd->arrayof_surface_formats_) {
                    if (dev_surf_form.format == file_surf_form.format && dev_surf_form.colorSpace == file_surf_form.colorSpace) {
                        blacklisted = true;
                        break;
                    }
                }
                if (!blacklisted) {
                    simulation_surface_formats.push_back(dev_surf_form);
                }
            }
            break;
        case ARRAY_COMBINATION_MODE_INTERSECT:
            for (VkSurfaceFormatKHR &dev_surf_form : device_surface_formats) {
                simulation_surface_formats.push_back(dev_surf_form);
            }
            for (VkSurfaceFormatKHR &file_surf_form : pdd->arrayof_surface_formats_) {
                bool blacklisted = false;
                for (VkSurfaceFormatKHR &sim_surf_form : simulation_surface_formats) {
                    if (sim_surf_form.format == file_surf_form.format && sim_surf_form.colorSpace == file_surf_form.colorSpace) {
                        blacklisted = true;
                        break;
                    }
                }
                if (!blacklisted) {
                    simulation_surface_formats.push_back(file_surf_form);
                }
            }
            break;
        default:
            result = dt->GetPhysicalDeviceSurfaceFormats2KHR(physicalDevice, pSurfaceInfo, pSurfaceFormatCount, pSurfaceFormats);
            return result;
    }

    src_count = static_cast<uint32_t>(simulation_surface_formats.size());

    if (!pSurfaceFormats) {
        *pSurfaceFormatCount = src_count;
        return result;
    }

    // Careful: cannot use EnumerateProperties() here! (because src and dst structs are not the same type)
    if (*pSurfaceFormatCount < src_count) result = VK_INCOMPLETE;
    const uint32_t copy_count = (*pSurfaceFormatCount < src_count) ? *pSurfaceFormatCount : src_count;
    const VkSurfaceFormatKHR *src_props = simulation_surface_formats.data();
    for (uint32_t i = 0; i < copy_count; ++i) {
        pSurfaceFormats[i].surfaceFormat = src_props[i];
    }
    *pSurfaceFormatCount = copy_count;

    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                                                                       uint32_t *pPresentModeCount,
                                                                       VkPresentModeKHR *pPresentModes) {
    VkResult result = VK_SUCCESS;
    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(physicalDevice);

    PhysicalDeviceData *pdd = PhysicalDeviceData::Find(physicalDevice);
    const uint32_t src_count = (pdd) ? static_cast<uint32_t>(pdd->arrayof_present_modes_.size()) : 0;
    if (src_count == 0) {
        return dt->GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, pPresentModeCount, pPresentModes);
    }

    uint32_t present_mode_count = 0;
    dt->GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &present_mode_count, nullptr);
    std::vector<VkPresentModeKHR> device_present_modes(present_mode_count);
    dt->GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &present_mode_count, device_present_modes.data());

    std::vector<VkPresentModeKHR> simulation_present_modes;

    switch (modifyPresentModes.mode) {
        case ARRAY_COMBINATION_MODE_NONE:
            result = dt->GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, pPresentModeCount, pPresentModes);
            return result;
            break;
        case ARRAY_COMBINATION_MODE_REPLACE:
            result = EnumerateProperties(src_count, pdd->arrayof_present_modes_.data(), pPresentModeCount, pPresentModes);
            return result;
            break;
        case ARRAY_COMBINATION_MODE_WHITELIST:
            for (VkPresentModeKHR &dev_present_mode : device_present_modes) {
                for (VkPresentModeKHR &file_present_mode : pdd->arrayof_present_modes_) {
                    if (dev_present_mode == file_present_mode) {
                        simulation_present_modes.push_back(dev_present_mode);
                        break;
                    }
                }
            }
            break;
        case ARRAY_COMBINATION_MODE_BLACKLIST:
            for (VkPresentModeKHR &dev_present_mode : device_present_modes) {
                bool blacklist = false;
                for (VkPresentModeKHR &file_present_mode : pdd->arrayof_present_modes_) {
                    if (dev_present_mode == file_present_mode) {
                        blacklist = true;
                        break;
                    }
                }
                if (!blacklist) {
                    simulation_present_modes.push_back(dev_present_mode);
                }
            }
            break;
        case ARRAY_COMBINATION_MODE_INTERSECT:
            for (VkPresentModeKHR &dev_present_mode : device_present_modes) {
                simulation_present_modes.push_back(dev_present_mode);
            }
            for (VkPresentModeKHR &file_present_mode : pdd->arrayof_present_modes_) {
                bool blacklist = false;
                for (VkPresentModeKHR &sim_present_mode : simulation_present_modes) {
                    if (sim_present_mode == file_present_mode) {
                        blacklist = true;
                        break;
                    }
                }
                if (!blacklist) {
                    simulation_present_modes.push_back(file_present_mode);
                }
            }
            break;
        default:
            result = dt->GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, pPresentModeCount, pPresentModes);
            return result;
    }

    return EnumerateProperties(static_cast<uint32_t>(simulation_present_modes.size()), simulation_present_modes.data(),
                               pPresentModeCount, pPresentModes);
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceToolPropertiesEXT(VkPhysicalDevice physicalDevice, uint32_t *pToolCount,
                                                                  VkPhysicalDeviceToolPropertiesEXT *pToolProperties) {
    std::stringstream version_stream;
    version_stream << kVersionDevsimMajor << "." << kVersionDevsimMinor << "." << kVersionDevsimPatch;
    std::string version_string(version_stream.str());

    static VkPhysicalDeviceToolPropertiesEXT devsim_layer_tool_props = {};
    devsim_layer_tool_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TOOL_PROPERTIES_EXT;
    devsim_layer_tool_props.pNext = nullptr;
    strcpy(devsim_layer_tool_props.name, "Device Simulation Layer");
    strcpy(devsim_layer_tool_props.version, version_string.c_str());
    devsim_layer_tool_props.purposes = VK_TOOL_PURPOSE_MODIFYING_FEATURES_BIT_EXT;
    strcpy(devsim_layer_tool_props.description, "LunarG device simulation layer");
    strcpy(devsim_layer_tool_props.layer, "VK_LAYER_LUNARG_device_simulation");

    auto original_pToolProperties = pToolProperties;
    if (pToolProperties != nullptr) {
        *pToolProperties = devsim_layer_tool_props;
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

#undef TRANSFER_VALUE

VKAPI_ATTR VkResult VKAPI_CALL EnumeratePhysicalDevices(VkInstance instance, uint32_t *pPhysicalDeviceCount,
                                                        VkPhysicalDevice *pPhysicalDevices) {
    // Our layer-specific initialization...

    std::lock_guard<std::recursive_mutex> lock(global_lock);
    const auto dt = instance_dispatch_table(instance);
    VkResult result = dt->EnumeratePhysicalDevices(instance, pPhysicalDeviceCount, pPhysicalDevices);

    // HACK!! epd_count is used to ensure the following code only gets called _after_ vkCreateInstance finishes *in the "vkcube +
    // devsim" use case*
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

            dt->GetPhysicalDeviceProperties(physical_device, &pdd.physical_device_properties_);
            bool api_version_above_1_1 = pdd.physical_device_properties_.apiVersion >= VK_API_VERSION_1_1;
            bool api_version_above_1_2 = pdd.physical_device_properties_.apiVersion >= VK_API_VERSION_1_2;

            // Initialize PDD members to the actual Vulkan implementation's defaults.
            if (get_physical_device_properties2_active) {
                VkPhysicalDeviceProperties2KHR property_chain = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR};
                VkPhysicalDeviceFeatures2KHR feature_chain = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR};
                VkPhysicalDeviceMemoryProperties2KHR memory_chain = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2_KHR};

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME)) {
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

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_8BIT_STORAGE_EXTENSION_NAME)) {
                    pdd.physical_device_8bit_storage_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_8bit_storage_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_16BIT_STORAGE_EXTENSION_NAME)) {
                    pdd.physical_device_16bit_storage_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_16bit_storage_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME)) {
                    pdd.physical_device_buffer_device_address_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_buffer_device_address_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME)) {
                    pdd.physical_device_depth_stencil_resolve_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_depth_stencil_resolve_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)) {
                    pdd.physical_device_descriptor_indexing_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_descriptor_indexing_properties_);

                    pdd.physical_device_descriptor_indexing_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_descriptor_indexing_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME)) {
                    pdd.physical_device_host_query_reset_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_host_query_reset_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME)) {
                    pdd.physical_device_imageless_framebuffer_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_imageless_framebuffer_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_MAINTENANCE2_EXTENSION_NAME)) {
                    pdd.physical_device_point_clipping_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_point_clipping_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_MAINTENANCE3_EXTENSION_NAME)) {
                    pdd.physical_device_maintenance_3_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_maintenance_3_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_MAINTENANCE_4_EXTENSION_NAME)) {
                    pdd.physical_device_maintenance_4_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_maintenance_4_features_);

                    pdd.physical_device_maintenance_4_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_maintenance_4_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_MULTIVIEW_EXTENSION_NAME)) {
                    pdd.physical_device_multiview_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_multiview_properties_);

                    pdd.physical_device_multiview_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_multiview_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME)) {
                    pdd.physical_device_sampler_filter_minmax_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_sampler_filter_minmax_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME)) {
                    pdd.physical_device_sampler_ycbcr_conversion_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_sampler_ycbcr_conversion_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME)) {
                    pdd.physical_device_scalar_block_layout_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_scalar_block_layout_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_SEPARATE_DEPTH_STENCIL_LAYOUTS_EXTENSION_NAME)) {
                    pdd.physical_device_separate_depth_stencil_layouts_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_separate_depth_stencil_layouts_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_SHADER_ATOMIC_INT64_EXTENSION_NAME)) {
                    pdd.physical_device_shader_atomic_int64_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_atomic_int64_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME)) {
                    pdd.physical_device_float_controls_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_float_controls_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME)) {
                    pdd.physical_device_shader_float16_int8_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_float16_int8_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_SHADER_SUBGROUP_EXTENDED_TYPES_EXTENSION_NAME)) {
                    pdd.physical_device_shader_subgroup_extended_types_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_subgroup_extended_types_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME)) {
                    pdd.physical_device_timeline_semaphore_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_timeline_semaphore_properties_);

                    pdd.physical_device_timeline_semaphore_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_timeline_semaphore_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT_EXTENSION_NAME)) {
                    pdd.physical_device_uniform_buffer_standard_layout_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_uniform_buffer_standard_layout_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME)) {
                    pdd.physical_device_variable_pointers_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_variable_pointers_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_VULKAN_MEMORY_MODEL_EXTENSION_NAME)) {
                    pdd.physical_device_vulkan_memory_model_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_vulkan_memory_model_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_ZERO_INITIALIZE_WORKGROUP_MEMORY_EXTENSION_NAME)) {
                    pdd.physical_device_zero_initialize_workgroup_memory_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_zero_initialize_workgroup_memory_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME)) {
                    pdd.physical_device_acceleration_structure_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_acceleration_structure_features_);

                    pdd.physical_device_acceleration_structure_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_acceleration_structure_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_PERFORMANCE_QUERY_EXTENSION_NAME)) {
                    pdd.physical_device_performance_query_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_performance_query_features_);

                    pdd.physical_device_performance_query_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_performance_query_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_PIPELINE_EXECUTABLE_PROPERTIES_EXTENSION_NAME)) {
                    pdd.physical_device_pipeline_executable_properties_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_pipeline_executable_properties_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_PRESENT_ID_EXTENSION_NAME)) {
                    pdd.physical_device_present_id_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_present_id_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_PRESENT_WAIT_EXTENSION_NAME)) {
                    pdd.physical_device_present_wait_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_present_wait_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME)) {
                    pdd.physical_device_push_descriptor_properites_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_push_descriptor_properites_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_RAY_QUERY_EXTENSION_NAME)) {
                    pdd.physical_device_ray_query_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_ray_query_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME)) {
                    pdd.physical_device_ray_tracing_pipeline_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_ray_tracing_pipeline_features_);

                    pdd.physical_device_ray_tracing_pipeline_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_ray_tracing_pipeline_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_SHADER_CLOCK_EXTENSION_NAME)) {
                    pdd.physical_device_shader_clock_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_clock_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_SHADER_INTEGER_DOT_PRODUCT_EXTENSION_NAME)) {
                    pdd.physical_device_shader_integer_dot_product_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_integer_dot_product_features_);

                    pdd.physical_device_shader_integer_dot_products_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_shader_integer_dot_products_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_EXTENSION_NAME)) {
                    pdd.physical_device_shader_subgroup_uniform_control_flow_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_subgroup_uniform_control_flow_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_SHADER_TERMINATE_INVOCATION_EXTENSION_NAME)) {
                    pdd.physical_device_shader_terminate_invocation_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_terminate_invocation_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME)) {
                    pdd.physical_device_synchronization2_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_synchronization2_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_EXTENSION_NAME)) {
                    pdd.physical_device_workgroup_memory_explicit_layout_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_workgroup_memory_explicit_layout_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_4444_FORMATS_EXTENSION_NAME)) {
                    pdd.physical_device_4444_formats_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_4444_formats_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_ASTC_DECODE_MODE_EXTENSION_NAME)) {
                    pdd.physical_device_astc_decode_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_astc_decode_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME)) {
                    pdd.physical_device_blend_operation_advanced_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_blend_operation_advanced_features_);

                    pdd.physical_device_blend_operation_advanced_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_blend_operation_advanced_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_BORDER_COLOR_SWIZZLE_EXTENSION_NAME)) {
                    pdd.physical_device_border_color_swizzle_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_border_color_swizzle_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_COLOR_WRITE_ENABLE_EXTENSION_NAME)) {
                    pdd.physical_device_color_write_enable_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_color_write_enable_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_CONDITIONAL_RENDERING_EXTENSION_NAME)) {
                    pdd.physical_device_conditional_rendering_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_conditional_rendering_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME)) {
                    pdd.physical_device_conservative_rasterization_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_conservative_rasterization_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_CUSTOM_BORDER_COLOR_EXTENSION_NAME)) {
                    pdd.physical_device_custom_border_color_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_custom_border_color_features_);

                    pdd.physical_device_custom_border_color_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_custom_border_color_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_DEPTH_CLIP_ENABLE_EXTENSION_NAME)) {
                    pdd.physical_device_depth_clip_enable_features_ext_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_depth_clip_enable_features_ext_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_DEVICE_MEMORY_REPORT_EXTENSION_NAME)) {
                    pdd.physical_device_device_memory_report_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_device_memory_report_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_DISCARD_RECTANGLES_EXTENSION_NAME)) {
                    pdd.physical_device_discard_rectangle_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_discard_rectangle_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME)) {
                    pdd.physical_device_extended_dynamic_state_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_extended_dynamic_state_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME)) {
                    pdd.physical_device_extended_dynamic_state2_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_extended_dynamic_state2_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_EXTERNAL_MEMORY_HOST_EXTENSION_NAME)) {
                    pdd.physical_device_external_memory_host_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_external_memory_host_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_FRAGMENT_DENSITY_MAP_EXTENSION_NAME)) {
                    pdd.physical_device_fragment_density_map_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_fragment_density_map_features_);

                    pdd.physical_device_fragment_density_map_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_fragment_density_map_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME)) {
                    pdd.physical_device_fragment_shader_interlock_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_fragment_shader_interlock_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_GLOBAL_PRIORITY_QUERY_EXTENSION_NAME)) {
                    pdd.physical_device_global_priority_query_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_global_priority_query_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_IMAGE_ROBUSTNESS_EXTENSION_NAME)) {
                    pdd.physical_device_image_robustness_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_image_robustness_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_INDEX_TYPE_UINT8_EXTENSION_NAME)) {
                    pdd.physical_device_index_type_uint8_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_index_type_uint8_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME)) {
                    pdd.physical_device_inline_uniform_block_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_inline_uniform_block_features_);

                    pdd.physical_device_inline_uniform_block_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_inline_uniform_block_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME)) {
                    pdd.physical_device_line_rasterization_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_line_rasterization_features_);

                    pdd.physical_device_line_rasterization_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_line_rasterization_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME)) {
                    pdd.physical_device_memory_priority_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_memory_priority_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_MULTI_DRAW_EXTENSION_NAME)) {
                    pdd.physical_device_multi_draw_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_multi_draw_features_);

                    pdd.physical_device_multi_draw_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_multi_draw_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_PAGEABLE_DEVICE_LOCAL_MEMORY_EXTENSION_NAME)) {
                    pdd.physical_device_pageable_device_local_memory_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_pageable_device_local_memory_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_PIPELINE_CREATION_CACHE_CONTROL_EXTENSION_NAME)) {
                    pdd.physical_device_pipeline_creation_cache_control_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_pipeline_creation_cache_control_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_PRIMITIVE_TOPOLOGY_LIST_RESTART_EXTENSION_NAME)) {
                    pdd.physical_device_primitive_topology_list_restart_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_primitive_topology_list_restart_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_PRIVATE_DATA_EXTENSION_NAME)) {
                    pdd.physical_device_private_data_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_private_data_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_PROVOKING_VERTEX_EXTENSION_NAME)) {
                    pdd.physical_device_provoking_vertex_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_provoking_vertex_features_);

                    pdd.physical_device_provoking_vertex_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_provoking_vertex_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_RGBA10X6_FORMATS_EXTENSION_NAME)) {
                    pdd.physical_device_rgba10x6_formats_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_rgba10x6_formats_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_ROBUSTNESS_2_EXTENSION_NAME)) {
                    pdd.physical_device_robustness_2_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_robustness_2_features_);

                    pdd.physical_device_robustness_2_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_robustness_2_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_SAMPLE_LOCATIONS_EXTENSION_NAME)) {
                    pdd.physical_device_sample_locations_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_sample_locations_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME)) {
                    pdd.physical_device_shader_atomic_float_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_atomic_float_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_SHADER_ATOMIC_FLOAT_2_EXTENSION_NAME)) {
                    pdd.physical_device_shader_atomic_float2_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_atomic_float2_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME)) {
                    pdd.physical_device_shader_demote_to_helper_invocation_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_demote_to_helper_invocation_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_SHADER_IMAGE_ATOMIC_INT64_EXTENSION_NAME)) {
                    pdd.physical_device_shader_image_atomic_int64_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_image_atomic_int64_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME)) {
                    pdd.physical_device_subgroup_size_control_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_subgroup_size_control_features_);

                    pdd.physical_device_subgroup_size_control_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_subgroup_size_control_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_TEXEL_BUFFER_ALIGNMENT_EXTENSION_NAME)) {
                    pdd.physical_device_texel_buffer_alignment_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_texel_buffer_alignment_features_);

                    pdd.physical_device_texel_buffer_alignment_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_texel_buffer_alignment_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_TEXTURE_COMPRESSION_ASTC_HDR_EXTENSION_NAME)) {
                    pdd.physical_device_texture_compression_astc_hdr_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_texture_compression_astc_hdr_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME)) {
                    pdd.physical_device_transform_feedback_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_transform_feedback_features_);

                    pdd.physical_device_transform_feedback_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_transform_feedback_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME)) {
                    pdd.physical_device_vertex_attribute_divisor_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_vertex_attribute_divisor_features_);

                    pdd.physical_device_vertex_attirbute_divisor_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_vertex_attirbute_divisor_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME)) {
                    pdd.physical_device_vertex_input_dynamic_state_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_vertex_input_dynamic_state_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_YCBCR_2PLANE_444_FORMATS_EXTENSION_NAME)) {
                    pdd.physical_device_ycbcr_2plane_444_formats_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_ycbcr_2plane_444_formats_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_EXT_YCBCR_IMAGE_ARRAYS_EXTENSION_NAME)) {
                    pdd.physical_device_ycbcr_image_arrays_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_ycbcr_image_arrays_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME)) {
                    pdd.physical_device_fragment_shading_rate_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_fragment_shading_rate_features_);

                    pdd.physical_device_fragment_shading_rate_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_fragment_shading_rate_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME)) {
                    pdd.physical_device_coherent_memory_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_coherent_memory_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_AMD_SHADER_CORE_PROPERTIES_EXTENSION_NAME)) {
                    pdd.physical_device_shader_core_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_shader_core_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_AMD_SHADER_CORE_PROPERTIES_2_EXTENSION_NAME)) {
                    pdd.physical_device_shader_core_properties_2_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_shader_core_properties_2_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_HUAWEI_INVOCATION_MASK_EXTENSION_NAME)) {
                    pdd.physical_device_invocation_mask_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_invocation_mask_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_HUAWEI_SUBPASS_SHADING_EXTENSION_NAME)) {
                    pdd.physical_device_subpass_shading_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_subpass_shading_features_);

                    pdd.physical_device_subpass_shading_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_subpass_shading_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_INTEL_SHADER_INTEGER_FUNCTIONS_2_EXTENSION_NAME)) {
                    pdd.physical_device_shader_integer_functions_2_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_shader_integer_functions_2_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_NV_COMPUTE_SHADER_DERIVATIVES_EXTENSION_NAME)) {
                    pdd.physical_device_compute_shader_derivatives_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_compute_shader_derivatives_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_NV_COOPERATIVE_MATRIX_EXTENSION_NAME)) {
                    pdd.physical_device_cooperative_matrix_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_cooperative_matrix_features_);

                    pdd.physical_device_cooperative_matrix_properties_.pNext = property_chain.pNext;

                    property_chain.pNext = &(pdd.physical_device_cooperative_matrix_properties_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_NV_CORNER_SAMPLED_IMAGE_EXTENSION_NAME)) {
                    pdd.physical_device_corner_sampled_image_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_corner_sampled_image_features_);
                }

                if (PhysicalDeviceData::HasExtension(physical_device, VK_NV_COVERAGE_REDUCTION_MODE_EXTENSION_NAME)) {
                    pdd.physical_device_coverage_reduction_mode_features_.pNext = feature_chain.pNext;

                    feature_chain.pNext = &(pdd.physical_device_coverage_reduction_mode_features_);
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

            DebugPrintf("\tdeviceName \"%s\"\n", pdd.physical_device_properties_.deviceName);

            // Override PDD members with values from configuration file(s).
            JsonLoader json_loader(pdd);
            json_loader.LoadFiles();

            pdd.combineExtensionLists();

            // VK_VULKAN_1_1
            TransferValue(&(pdd.physical_device_vulkan_1_1_properties_), &(pdd.physical_device_point_clipping_properties_));
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
        }
    }
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
    GET_PROC_ADDR(GetPhysicalDeviceMemoryProperties);
    GET_PROC_ADDR(GetPhysicalDeviceMemoryProperties2);
    GET_PROC_ADDR(GetPhysicalDeviceMemoryProperties2KHR);
    GET_PROC_ADDR(GetPhysicalDeviceQueueFamilyProperties);
    GET_PROC_ADDR(GetPhysicalDeviceQueueFamilyProperties2);
    GET_PROC_ADDR(GetPhysicalDeviceQueueFamilyProperties2KHR);
    GET_PROC_ADDR(GetPhysicalDeviceFormatProperties);
    GET_PROC_ADDR(GetPhysicalDeviceFormatProperties2);
    GET_PROC_ADDR(GetPhysicalDeviceFormatProperties2KHR);
    GET_PROC_ADDR(GetPhysicalDeviceImageFormatProperties);
    GET_PROC_ADDR(GetPhysicalDeviceImageFormatProperties2);
    GET_PROC_ADDR(GetPhysicalDeviceImageFormatProperties2KHR);
    GET_PROC_ADDR(GetPhysicalDeviceSurfaceCapabilitiesKHR);
    GET_PROC_ADDR(GetPhysicalDeviceSurfaceCapabilities2KHR);
    GET_PROC_ADDR(GetPhysicalDeviceSurfaceFormatsKHR);
    GET_PROC_ADDR(GetPhysicalDeviceSurfaceFormats2KHR);
    GET_PROC_ADDR(GetPhysicalDeviceSurfacePresentModesKHR);
    GET_PROC_ADDR(GetPhysicalDeviceToolPropertiesEXT);
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
