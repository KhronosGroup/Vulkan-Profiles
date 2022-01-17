<!-- markdownlint-disable MD041 -->
<p align="left"><img src="https://vulkan.lunarg.com/img/NewLunarGLogoBlack.png" alt="LunarG" width=263 height=113 /></p>
<p align="left">Copyright © 2015-2022 LunarG, Inc.</p>

[![Creative Commons][3]][4]

[3]: https://i.creativecommons.org/l/by-nd/4.0/88x31.png "Creative Commons License"
[4]: https://creativecommons.org/licenses/by-nd/4.0/

# VK\_LAYER\_KHRONOS\_profiles

## Overview

### Extend your Vulkan test coverage with the Khronos Profiles Layer
The Khronos Profiles Layer helps test across a wide range of hardware capabilities without requiring a physical copy of every device. It can be applied without modifying any application binaries, and in a fully automated fashion. The Profiles layer is a Vulkan layer that can override the values returned by your application’s queries of the GPU. Profiles layer uses a JSON text configuration file to make your application see a different driver/GPU than is actually in your system. This capability is useful to verify that your application both a) properly queries the limits from Vulkan, and b) obeys those limits.

The Profiles layer library is available pre-built in the Vulkan SDK, and continues to evolve. It works for all Vulkan platforms (Linux, Windows, macOS, and Android), and is open-source software hosted on GitHub. The Profiles layer can be enabled and configured using the [Vulkan Configurator](https://vulkan.lunarg.com/doc/sdk/latest/windows/vkconfig.html) included with the Vulkan SDK.

The role of the Profiles layer is to "simulate" a Vulkan implementation by modifying the features and resources of a more-capable implementation. Profiles layer does not add capabilities to your existing Vulkan implementation by "emulating" additional capabilities with software; e.g. Profiles layer cannot add geometry shader capability to an actual device that doesn’t already provide it. Also, Profiles layer does not "enforce" the features being simulated. For enforcement, you would continue to use the Validation Layers as usual, in conjunction with Profiles layer.

### Using Profiles layer
The Profiles layer supports a flexible configuration file format using JSON syntax. The configuration file format is defined by a formal JSON schema, so any configuration file may be verified to be correct using freely available JSON validators. Browsing through the schema file, you can see the extent of parameters that are available for your configuration.

### Android
To enable, use a setting with the path of configuration file to load:
```
adb shell settings put global debug.vulkan.khronos_profiles.profile_file <path/to/profiles/JSON/configuration/file>
```
Example of a Profiles layer JSON configuration file: [tiny1.json](https://github.com/LunarG/VulkanTools/blob/master/layersvt/device_simulation_examples/tiny1.json)

Optional: use settings to enable debugging output and exit-on-error:
```
adb shell settings put global debug.vulkan.khronos_profiles.debug_reports DEBUG_REPORT_DEBUG_BIT
adb shell settings put global debug.vulkan.khronos_profiles.debug_fail_on_error true
```

### How Profiles layer works
The Profiles layer builds its internal data tables by querying the capabilities of the underlying actual device, then applying each of the configuration files “on top of” those tables. Therefore you only need to specify the features you wish to modify from the actual device; tweaking a single feature is easy. Here’s an example of  a valid configuration file for changing only the maximum permitted viewport size:

```json
{
    "$schema": "https://schema.khronos.org/vulkan/profiles-1.3.201.json#",
    "capabilities": {
        "baseline": {
            "extensions": {},
            "features": {},
            "properties": {
                "VkPhysicalDeviceProperties": {
                    "limits": {
                        "maxViewportDimensions": [ 169, 170 ],
					}
				}
			},
            "formats": {},
            "queueFamiliesProperties": []
        }
    },
    "profiles": {
        "VP_LUNARG_test_structure_complex": {
            "version": 1,
            "api-version": "1.2.198",
            "label": "LunarG Profiles Structure Complex unit test",
            "description": "For schema unit test on C.I.",
            "contributors": {
                "Christophe Riccio": {
                    "company": "LunarG",
                    "email": "christophe@lunarg.com",
                    "github": "christophe-lunarg",
                    "contact": true
                }
            },
            "history": [
                {
                    "revision": 1,
                    "date": "2021-12-08",
                    "author": "Christophe Riccio",
                    "comment": "Initial revision"
                }
            ],
            "capabilities": [
                "baseline"
            ]
        }
    }
}
```

### Simulating Entire Real-World Devices
If you instead wish to simulate entire real-world devices, LunarG has collaborated with the [Vulkan Hardware Database](https://vulkan.gpuinfo.org/) to make their data compatible with the Profiles schema. You can download device configurations from the website in JSON format, and use those configuration files directly with Profiles layer.

# Technical Details

The Profiles Layer is a Vulkan layer that can modify the results of Vulkan PhysicalDevice queries based on a JSON configuration file, thus simulating some of the capabilities of device by overriding the capabilities of the actual device under test.

Please note that this device simulation layer "simulates", rather than "emulates", another device.
By that we mean that the layer cannot add emulated capabilities that do not already exist in the system's underlying actual device;
Profiles layer will not enable a less-capable device to emulate a more-capable device.

Application code can be tested to verify it responds correctly to the capabilities reported by the simulated device.
That could include:
* Properly querying the capabilities of the device.
* Properly complying with the limits reported from the device.
* Verifying all necessary capabilities are reported present, rather than assuming they are available.
* Exercising fall-back code paths, if optional capabilities are not available.

The `fail_on_error` option can be used to make sure the device supports the requested capabilities. 
In this case if an application erroneously attempts to overcommit a resource, or use a disabled feature, the Profiles layer will return `VK_ERROR_INITIALIZATION_FAILED` from `vkEnumeratePhysicalDevices()`.

The Profiles layer will work together with other Vulkan layers, such as the Validation layer.
When configuring the order of the layers list, the Profiles layer should be "last";
i.e.: closest to the driver, farthest from the application.
That allows the Validation layer to see the results of the Profiles layer, and permit Validation to enforce the simulated capabilities.

Please report issues to [LunarG's VulkanTools GitHub repository](https://github.com/LunarG/VulkanTools/issues) and include "Profiles" in the title text.

### Layer name
`VK_LAYER_KHRONOS_profiles`

### Profiles Layer operation and configuration
At application startup, during `vkEnumeratePhysicalDevices()`, the Profiles layer initializes its internal tables from the actual physical device in the system, then loads its configuration file, which specifies override values to apply to those internal tables.

JSON file formats consumed by the Profiles layer are specified by the following JSON schema https://schema.khronos.org/vulkan/profiles-1.3.203.json#

The top-level sections of such configuration files are processed as follows:
* `$schema` - Optional.  Must be the URI string referencing the JSON schema.
* `capabilities` - Required.  Specifies the list of capabilities sets.
* `profiles` - Required.  Specifies the list of profile definitions.

`capabilities` contains the following sections:
* `extensions` - Optional. List of all required extensions.
* `features` - Optional. List of all required features.
* `properties` - Optional. List of all required properties.
* `formats` - Optional. List of all required formats.

`profiles` contains the following sections:
* `version` - Required. The revision of the profile.
* `label` - Required. The label used to present the profile to the Vulkan developer.
* `description` - Required. The description of the profile.
* `status` - Optional. The developmet status of the profile.
* `api-version` - Required. The Vulkan API version against which the profile is written.
* `contributors` - Required. The list of contributors of the profile.
* `history` - Required. The version history of the profile file.
* `capabilities` - Required. The list of capability sets that can be reference by a profile.
* `fallback` - Optional. The list of profiles recommended if the checked profile is not supported by the platform.

The schema permits additional top-level sections to be optionally included in configuration files;
any additional top-level sections will be ignored by Profiles layer.

The schemas define basic range checking for common Vulkan data types, but they cannot detect whether a particular configuration is ilogical.
If a configuration defines capabilities beyond what the actual device is natively capable of providing and the `fail_on_error` option is not used, the results are undefined.
The Profiles layer has some simple checking of configuration values and writes debug messages (if enabled) for values that are incompatible with the capabilities of the actual device.

This version of Profiles layer currently supports Vulkan v1.3 and below.
If the application requests an unsupported version of the Vulkan API, Profiles layer will emit an error message.

### Example of a Profiles JSON configuration file
```json
{
    "$schema": "https://schema.khronos.org/vulkan/profiles-1.3.201.json#",
    "capabilities": {
        "baseline": {
            "extensions": {},
            "features": {},
            "properties": {
                "VkPhysicalDeviceProperties": {
                    "limits": {
						"maxViewports": 1,
						"maxViewportDimensions": [1024, 1024]
					}
				}
			},
            "formats": {},
            "queueFamiliesProperties": []
        }
    },
    "profiles": {
        "VP_LUNARG_test_structure_complex": {
            "version": 1,
            "api-version": "1.2.198",
            "label": "LunarG Profiles Structure Complex unit test",
            "description": "For schema unit test on C.I.",
            "contributors": {
                "Christophe Riccio": {
                    "company": "LunarG",
                    "email": "christophe@lunarg.com",
                    "github": "christophe-lunarg",
                    "contact": true
                }
            },
            "history": [
                {
                    "revision": 1,
                    "date": "2021-12-08",
                    "author": "Christophe Riccio",
                    "comment": "Initial revision"
                }
            ],
            "capabilities": [
                "baseline"
            ]
        }
    }
}
```

### `VK_KHR_portability_subset` Emulation

The Profiles layer provides the ability to emulate the `VK_KHR_portability_subset` extension on devices that do not implement this extension.
This feature allows users to test their application with limitations found on non-conformant Vulkan implementations.
To turn on this feature, set the `VK_KHRONOS_PROFILES_EMULATE_PORTABILITY_SUBSET_EXTENSION` environment variable (or the corresponding vk_layer_settings.txt option `khronos_profiles.emulate_portability`) to a positive integer.

### Profiles Layer Options

#### Profiles JSON configuration file
- Environment Variable: `VK_KHRONOS_PROFILES_PROFILE_FILE`
- `vk_layer_settings.txt` Option: `khronos_profiles.profile_file`
- Android Option: `debug.vulkan.khronos_profiles.profile_file`
- Default Value: Not set

Name of the configuration file to load. 

#### Selecting profile
- Environment Variable: `VK_KHRONOS_PROFILES_PROFILE_NAME`
- `vk_layer_settings.txt` Option: `khronos_profiles.profile_name`
- Android Option: `debug.vulkan.khronos_profiles.profile_name`
- Default Value: Not set

Name of the profile to load from the configuration file.

#### Emulate `VK_KHR_portability_subset`
- Environment Variable: `VK_KHRONOS_PROFILES_EMULATE_PORTABILITY`
- `vk_layer_settings.txt` Option: `khronos_profiles.emulate_portability`
- Android Option: `debug.vulkan.khronos_profiles.emulate_portability`
- Default Value: false

Enables emulation of the `VK_KHR_portability_subset` extension.

#### Simulating capabilities
- Environment Variable: `VK_KHRONOS_PROFILES_SIMULATE_CAPABILITIES`
- `vk_layer_settings.txt` Option: `khronos_profiles.simulate_capabilities`
- Android Option: `debug.vulkan.khronos_profiles.simulate_capabilities`
- Options: SIMULATE_API_VERSION_BIT, SIMULATE_FEATURES_BIT, SIMULATE_PROPERTIES_BIT, SIMULATE_EXTENSIONS_BIT, SIMULATE_FORMATS_BIT, SIMULATE_FORMAT_PROPERTIES_BIT
- Default Value: SIMULATE_API_VERSION_BIT, SIMULATE_FEATURES_BIT, SIMULATE_PROPERTIES_BIT

Enables modification of device capabilities.

#### Debug Actions
- Environment Variable: `VK_KHRONOS_PROFILES_DEBUG_ACTIONS`
- `vk_layer_settings.txt` Option: `khronos_profiles.debug_actions`
- Android Option: `debug.vulkan.khronos_profiles.debug_actions`
- Options: DEBUG_ACTION_LOG_BIT, DEBUG_ACTION_OUTPUT_BIT, DEBUG_ACTION_BREAKPOINT_BIT
- Default Value: LOG

Enables different debugging actions.

#### Debug Filename
- Environment Variable: `VK_KHRONOS_PROFILES_DEBUG_FILENAME`
- `vk_layer_settings.txt` Option: `khronos_profiles.debug_filename`
- Android Option: `debug.vulkan.khronos_profiles.debug_filename`
- Default Value: stdout

Sets the output location.

#### Debug fail on error
- Environment Variable: `VK_KHRONOS_PROFILES_DEBUG_FAIL_ON_ERROR`
- `vk_layer_settings.txt` Option: `khronos_profiles.debug_fail_on_error`
- Android Option: `debug.vulkan.khronos_profiles.debug_fail_on_error`
- Default Value: Off

Enabled failing if an error occurs.

#### Debug reports
- Environment Variable: `VK_KHRONOS_PROFILES_DEBUG_REPORTS`
- `vk_layer_settings.txt` Option: `khronos_profiles.debug_reports`
- Android Option: `debug.vulkan.khronos_profiles.debug_reports`
- Options: DEBUG_REPORT_NOTIFICATION_BIT, DEBUG_REPORT_WARNING_BIT, DEBUG_REPORT_ERROR_BIT, DEBUG_REPORT_DEBUG_BIT
- Default Value: 0

Enabled reports level.

**Note:** Environment variables take precedence over `vk_layer_settings.txt` options.

### Example using the Profiles layer using Linux environment variables
```bash
# Configure bash to find the Vulkan SDK.
source $VKSDK/setup-env.sh

# Set loader parameters to find and load the Profiles layer from your local VulkanTools build.
export `VK_LAYER_PATH="${VulkanTools}/build/layersvt"`
export `VK_INSTANCE_LAYERS="VK_LAYER_KHRONOS_profiles`"`

# Specify the simulated device's configuration file.
export `VK_KHRONOS_PROFILES_PROFILE_FILE="${VulkanTools}/layersvt/device_simulation_examples/tiny1.json"`

# Enable notification and debug messages from the Profiles layer.
export `VK_KHRONOS_PROFILES_DEBUG_REPORTS="DEBUG_REPORT_NOTIFICATION_BIT,DEBUG_REPORT_DEBUG_BIT"`

# Enable capabilities to simulate
export `VK_KHRONOS_PROFILES_SIMULATE_CAPABILITIES="SIMULATE_API_VERSION_BIT,SIMULATE_FEATURES_BIT,SIMULATE_PROPERTIES_BIT,SIMULATE_EXTENSIONS_BIT"`

# Run a Vulkan application through the Profiles layer.
vulkaninfo
# Compare the results with that app running without the Profiles layer.
```
See also
* [${VulkanTools}/tests/devsim_layer_test.sh](https://github.com/LunarG/VulkanTools/blob/master/tests/devsim_layer_test.sh) - a test runner script.
* [${VulkanTools}/tests/devsim_test1.json](https://github.com/LunarG/VulkanTools/blob/master/tests/devsim_test1_in.json) - an example configuration file, containing bogus test data.

### Device configuration data from vulkan.gpuinfo.org
A large and growing database of device capabilities is available at https://vulkan.gpuinfo.org/

That device data can be downloaded in JSON format, compatible with the Profiles JSON schema.

A JSON index of the available device records can be queried with https://vulkan.gpuinfo.org/api/v2/devsim/getreportlist.php

That index includes URLs to download the specific device records in Profiles-compatible format, for example https://vulkan.gpuinfo.org/api/v2/devsim/getreport.php?id=1456

As mentioned above, attempting to use a configuration file that does not fit within the capabilities of the underlying device may produce undefined results.
Downloaded device records should be reviewed to determine that their capabilities can be simulated by the underlying device.

### Device configuration data from the local system
Vulkan Info can write its output in a format compatible the Profiles JSON schema,
so the configuration of the local system can be captured.
Use `vulkaninfo -j` to generate output in JSON format and redirect to a file, which can be consumed directly by Profiles layer.
See the Vulkan Info documentation for further details.

### Device configuration data from an iOS device
Vulkan Info can be built and run for iOS using the source and XCode project available from the [Vulkan-Tools repository](https://github.com/KhronosGroup/Vulkan-Tools/tree/master/vulkaninfo). For details on running and extracting the json files see the [Vulkan Info documentation](https://vulkan.lunarg.com/doc/sdk/latest/mac/vulkaninfo.html). An [Apple Developer Program](https://developer.apple.com/programs/) membership is required to deploy Vulkan Info to an iOS hardware device.

### JSON validation
The Profiles layer itself does very little sanity-checking of the configuration file, so those files should be validated to the schema using a separate tool, such as the following web-based validators.
1. http://www.jsonschemavalidator.net/
2. https://json-schema-validator.herokuapp.com/
3. https://jsonschemalint.com/#/version/draft-04/markup/json/

### Other Resources
1. http://json.org/
2. http://json-schema.org/
