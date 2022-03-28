<!-- markdownlint-disable MD041 -->
<p align="left"><img src="https://vulkan.lunarg.com/img/NewLunarGLogoBlack.png" alt="LunarG" width=263 height=113 /></p>
<p align="left">Copyright (c) 2021-2022 LunarG, Inc.</p>

[![Creative Commons][3]][4]

[3]: https://i.creativecommons.org/l/by-nd/4.0/88x31.png "Creative Commons License"
[4]: https://creativecommons.org/licenses/by-nd/4.0/

<p align="center"><img src="./images/logo.png" width=400 /></p>

# Vulkan Profiles Toolset Overview

The *Vulkan Profiles Toolset* is a collection of components for Vulkan application developers to build **portable** Vulkan applications using *Vulkan Profiles*.

*Vulkan Profiles* have multiple use cases, including:
- *Roadmap profiles*: To express guidance on the future direction of Vulkan devices.
- *Platform profiles*: To express the Vulkan support available on different platforms.
- *Device profiles*: To express the Vulkan support of a single Vulkan driver for a Vulkan device.
- *Engine profiles*: To express some rendering code paths requirements of an engine.
- Etc.

Conceptually, *Vulkan Profiles* can be understood as the explicit expression and formalization of Vulkan requirements and provides clear communication of these requirements within the *Vulkan Community*.

*The Vulkan Profiles Toolset* delivered in the [***Vulkan SDK***](https://www.lunarg.com/vulkan-sdk/) is still under development at the **BETA** stage.

## Vulkan Profiles Toolset Components

The *Vulkan Profiles Toolset* includes the following components:
- [***The Vulkan Profiles JSON Schema***](https://github.com/KhronosGroup/Khronos-Schemas/tree/main/vulkan)
  - A data format to exchange Vulkan capabilities: extensions, features, properties, formats and queue properties.
  - Each revision of Vulkan API is represented by a schema that supersedes older versions of Vulkan API.
  - The schema covers Vulkan 1.3 and all extensions.
- [***The Vulkan Profiles API library***](https://vulkan.lunarg.com/doc/sdk/latest/windows/profiles_api_library.html)
  - A header-only C++ library to use *Vulkan Profiles* in Vulkan applications.
  - The library allows checking Profiles support on a device and creating a `VkDevice` instance with the profile features and extensions enabled.
  - The library requires a Vulkan 1.0 driver that supports `VK_KHR_get_physical_device_properties2` extension.
- [***The Vulkan Profiles Layer***](https://vulkan.lunarg.com/doc/sdk/latest/windows/profiles_layer.html)
  - A layer used during application development to ensure adherence to the requirements of a chosen *Vulkan Profile*.
  - [It simulates but doesn't emulate](https://vulkan.lunarg.com/doc/sdk/latest/windows/profiles_layer.html#technical-details) Vulkan capabilities. Hence, it is used with the [Validation layer](https://github.com/KhronosGroup/Vulkan-ValidationLayers) to clamp available capabilities on the Vulkan developer system.
  - The layer requires a Vulkan 1.0 driver that supports `VK_KHR_get_physical_device_properties2` extension.
- [***The Vulkan Profiles Comparison Table***](https://vulkan.lunarg.com/doc/sdk/latest/windows/profiles_definitions.html)
  - Human-readable format of *Vulkan Profiles* in a table to enable comparison.

Furthermore, the *Vulkan SDK* includes implementations of some *Vulkan Profiles* using the *Vulkan Profiles JSON Schema*:
- [VP_KHR_roadmap_2022.json](https://github.com/KhronosGroup/Vulkan-Profiles/blob/master/profiles/VP_KHR_roadmap_2022.json)
- [VP_ANDROID_baseline_2021.json](https://github.com/KhronosGroup/Vulkan-Profiles/blob/master/profiles/VP_ANDROID_baseline_2021.json)
- [VP_LUNARG_desktop_portability_2021.json](https://github.com/KhronosGroup/Vulkan-Profiles/blob/master/profiles/VP_LUNARG_desktop_portability_2021.json)

Each profiles file may contain multiple variants of a profile. For example `VP_LUNARG_desktop_portability_2021.json` specifies `VP_LUNARG_desktop_portability_2021` profile for Windows and Linux but also `VP_LUNARG_desktop_portability_2021_subset` profile for macOS which has specific requirements.

Within the *Vulkan SDK*, the Vulkan Profiles files are located at `$(VULKAN_SDK)/Config/VK_LAYER_KHRONOS_profiles`.

## Vulkan Profiles Tools Code Generation

Considering the complexity of the Vulkan ecosystem, there isn't a single *Vulkan Profile* that can fit all needs. As a result, on top of the predefined *Vulkan Profiles*, the *Vulkan Profiles Toolset* solution is designed around the idea of code generation.

The *Vulkan Profiles Toolset* is generated against [`vk.xml`](https://github.com/KhronosGroup/Vulkan-Headers/blob/main/registry/vk.xml)(the canonical representation of the Vulkan specification) and [*Vulkan Profiles*](https://github.com/KhronosGroup/Vulkan-Profiles/tree/master/profiles).
This design guarantees that any Vulkan developer can regenerate the entire *Vulkan Profiles Toolset* solution with any new [Vulkan Header](https://github.com/KhronosGroup/Vulkan-Headers) update or any set of *Vulkan Profiles*.

The following diagram shows the Vulkan Profiles Toolset generation pipeline with every produced components:
![Vulkan Profiles](https://github.com/KhronosGroup/Vulkan-Profiles/blob/master/images/overview.png)

Note: The *Profiles layer* is not yet code-generated but it will be in the future.

## Future Work:
  - [GPUInfo.org](https://gpuinfo.org/) will allow downloading profiles JSON files for any Vulkan driver report.
  - [`vulkaninfo`](https://vulkan.lunarg.com/doc/view/latest/windows/vulkaninfo.html) will allow generating profiles JSON files.
  - The [Khronos Vulkan-Samples](https://github.com/KhronosGroup/Vulkan-Samples) repository will include a sample that uses the *Vulkan Profiles API Library*.

## The Vulkan Profiles JSON Schema

The Profiles layer builds its internal data tables by querying the capabilities of the underlying actual device, then applying the selected Vulkan Profile "on top of" of those tables. Therefore we only need to specify the features we wish to modify from the actual device; tweaking a single feature is easy. Here's an example of a valid *Vulkan Profiles* file for changing only the maximum permitted viewport size:

```json
{
    "$schema": "https://schema.khronos.org/vulkan/profiles-1.3.204.json#",
    "capabilities": {
        "baseline": {
            "extensions": {},
            "features": {},
            "properties": {
                "VkPhysicalDeviceProperties": {
                    "limits": {
                        "maxViewportDimensions": [ 4096, 2048 ],
                    }
                }
			 },
            "formats": {},
            "queueFamiliesProperties": []
        }
    },
    "profiles": {
        "VP_LUNARG_example": {
            "version": 1,
            "api-version": "1.2.198",
            "status": "STABLE",
            "label": "LunarG Example Profile",
            "description": "An example profile used for the overview documentation",
            "contributors": {
                "A Person": {
                    "company": "LunarG",
                    "email": "a.person@lunarg.com",
                    "github": "lunarg",
                    "contact": true
                }
            },
            "history": [
                {
                    "revision": 1,
                    "date": "2021-12-08",
                    "author": "A Person",
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

The top-level sections of such profiles files are processed as follows:
* `$schema` - Required. Must be the `URI` string referencing the JSON schema.
* `capabilities` - Required. Specifies the list of capabilities sets.
* `profiles` - Required. Specifies the list of profile definitions.

Each entry in `capabilities` includes a reference name and a dictionary containing the following sections:
* `extensions` - Optional. List of all required extensions.
* `features` - Optional. List of all required features.
* `properties` - Optional. List of all required properties.
* `formats` - Optional. List of all required formats.
* `queueFamiliesProperties` - Optional. List of all queue families properties.

Each entry in `profiles` includes a reference name and a dictionary containing the following sections:
* `version` - Required. The revision of the profile.
* `api-version` - Required. The Vulkan API version against which the profile is written.
* `status` - Optional. The development stage of the profile. It can be either `ALPHA`, `BETA`, `STABLE` or `DEPRECATED`.
* `label` - Required. The label used to present the profile to the Vulkan developer.
* `description` - Required. The description of the profile.
* `contributors` - Required. The list of contributors of the profile.
* `history` - Required. The version history of the profile file.
* `capabilities` - Required. The list of capability sets that can be referenced by a profile.
* `fallback` - Optional. The list of profiles recommended if the checked profile is not supported by the platform.

`profiles` may reference multiple `capabilities` which may define the same capability differently. In this case, the rule is that the last reference of that capability overrides any previously set values.

## Vulkan Profiles JSON validation

The JSON schema allows validating that the profiles JSON file is correctly defined. This can be done by many tools including the following web-based validators:
- http://www.jsonschemavalidator.net/
- https://json-schema-validator.herokuapp.com/
- https://jsonschemalint.com/#/version/draft-04/markup/json/
