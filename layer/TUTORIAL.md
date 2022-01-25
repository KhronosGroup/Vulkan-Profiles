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
The Profiles layer supports a flexible file format using JSON syntax. The profiles file format is defined by a formal JSON schema, so any profile file may be verified to be correct using freely available JSON validators. Browsing through the schema file, you can see the extent of parameters that are available for your configuration.

### Android
To enable, use a setting with the path of profiles file to load:
```
adb shell settings put global debug.vulkan.khronos_profiles.profile_file <path/to/profiles/JSON/file>
```
Example of a Profiles layer JSON profiles file: [tiny1.json](https://github.com/KhronosGroup/Vulkan-Profiles/blob/master/profiles/VP_LUNARG_desktop_portability_2021.json)

Optional: use settings to enable debugging output and exit-on-error:
```
adb shell settings put global debug.vulkan.khronos_profiles.debug_reports DEBUG_REPORT_DEBUG_BIT
adb shell settings put global debug.vulkan.khronos_profiles.debug_fail_on_error true
```

# Technical Details

The Profiles Layer is a Vulkan layer that can modify the results of Vulkan PhysicalDevice queries based on a JSON profiles file, thus simulating some of the capabilities of device by overriding the capabilities of the actual device under test.

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

Please report issues to [Khronos's Vulkan-Profiles GitHub repository](https://github.com/KhronosGroup/Vulkan-Profiles/issues).

### Profiles Layer operation and profiles file
At application startup, during `vkEnumeratePhysicalDevices()`, the Profiles layer initializes its internal tables from the actual physical device in the system, then loads its profiles file, which specifies override values to apply to those internal tables.

JSON file formats consumed by the Profiles layer are specified by the following JSON schema https://schema.khronos.org/vulkan/profiles-1.3.204.json#

The schema permits additional top-level sections to be optionally included in profiles files;
any additional top-level sections will be ignored by Profiles layer.

The schemas define basic range checking for common Vulkan data types, but they cannot detect whether a particular profile is ilogical.
If a profile defines capabilities beyond what the actual device is natively capable of providing and the `fail_on_error` option is not used, the results are undefined.
The Profiles layer has some simple checking of profile values and writes debug messages (if enabled) for values that are incompatible with the capabilities of the actual device.

This version of Profiles layer currently supports Vulkan v1.3 and below including all Vulkan extensions.
If the application requests an unsupported version of the Vulkan API, Profiles layer will emit an error message.

### `VK_KHR_portability_subset` Emulation

The Profiles layer provides the ability to emulate the `VK_KHR_portability_subset` extension on devices that do not implement this extension.
This feature allows users to test their application with limitations found on non-conformant Vulkan implementations.
To turn on this feature, set the `VK_KHRONOS_PROFILES_EMULATE_PORTABILITY_SUBSET_EXTENSION` environment variable (or the corresponding vk_layer_settings.txt option `khronos_profiles.emulate_portability`) to a positive integer.

### Profiles Layer Options

#### Profiles JSON file
- Environment Variable: `VK_KHRONOS_PROFILES_PROFILE_FILE`
- `vk_layer_settings.txt` Option: `khronos_profiles.profile_file`
- Android Option: `debug.vulkan.khronos_profiles.profile_file`
- Default Value: Not set

Name of the profiles file to load. 

#### Selecting profile
- Environment Variable: `VK_KHRONOS_PROFILES_PROFILE_NAME`
- `vk_layer_settings.txt` Option: `khronos_profiles.profile_name`
- Android Option: `debug.vulkan.khronos_profiles.profile_name`
- Default Value: Not set

Name of the profile to load from the profiles file.

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

# Specify the simulated device's profiles file.
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
