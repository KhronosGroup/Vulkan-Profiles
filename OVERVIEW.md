# Vulkan Profiles Tools Overview

The *Vulkan Profiles Tools* is a collection of components for Vulkan application developers to build **portable** Vulkan applications using *Vulkan Profiles*.

*Vulkan Profiles* have multiple use cases, including:

* *Roadmap profiles*: To express guidance on the future direction of Vulkan devices.
* *Platform profiles*: To express the Vulkan support available on different platforms.
* *Device profiles*: To express the Vulkan support of a single Vulkan driver for a Vulkan device.
* *Engine profiles*: To express some rendering code paths requirements of an engine.
* Etc.

Conceptually, *Vulkan Profiles* can be understood as the explicit expression and formalization of Vulkan requirements and provides clear communication of these requirements within the *Vulkan Community*.

* The Vulkan Profiles tools require Vulkan 1.1.

*The Vulkan Profiles Tools* delivered in the ***[Vulkan SDK](https://www.lunarg.com/vulkan-sdk/)***.

## Vulkan Profiles Tools Components

The *Vulkan Profiles Tools* includes the following components:

* ***[The `vkprofiles` command line tool](./scripts/README.md)***
* A command-line tool to process Vulkan profiles files and generate code and documentation:
  * Validates Vulkan profiles files and analysis the sementics of profiles file data.
  * Supports profiles combining via `union`, `intersection` and `difference` of Vulkan capabilities.
  * Supports transformation of Vulkan profiles file to help developers to produce exhaustive Vulkan capabilities set and process the data to be more relavant for Vulkan Profiles layer users and generating Vulkan Profiles API libraries.

* ***[The Vulkan Profiles JSON Schema](https://github.com/KhronosGroup/Khronos-Schemas/tree/main/vulkan)***
* A data format to exchange Vulkan capabilities: extensions, features, properties, formats and queue properties.
* Each revision of Vulkan API is represented by a schema that supersedes older versions of Vulkan API.
* The schema covers Vulkan capabilities and all extensions.

* ***[The Vulkan Profiles API library](https://vulkan.lunarg.com/doc/sdk/latest/windows/profiles_api_library.html)***
* A header-only C++ library to use *Vulkan Profiles* in Vulkan applications.
* The library allows checking Profiles support on a device and creating a `VkDevice` instance with the profile features and extensions enabled.

* ***[The Vulkan Profiles Layer](https://vulkan.lunarg.com/doc/sdk/latest/windows/profiles_layer.html)***
* A layer used during application development to ensure adherence to the requirements of a chosen *Vulkan Profile*.
* [It simulates but doesn't emulate](https://vulkan.lunarg.com/doc/sdk/latest/windows/profiles_layer.html%23technical-details) Vulkan capabilities. Hence, it is used with the [Validation layer](https://github.com/KhronosGroup/Vulkan-ValidationLayers) to clamp available capabilities on the Vulkan developer system.

* ***[The Vulkan Profiles Comparison Table](https://vulkan.lunarg.com/doc/sdk/latest/windows/profiles_definitions.html)***
* Human-readable formats, HTML and markdown of *Vulkan Profiles* in a table to enable comparison.

* **[Vulkan Profiles Tools Whitepaper](./doc/Vulkan-Profiles-Tools-Whitepaper.pdf)**: Detailed whitepaper about the Vulkan Profiles Tools.

## Vulkan Profiles Files

The repository contains implementations of some *Vulkan Profiles* using the *Vulkan Profiles JSON Schema*.

The Khronos Roadmap profiles expose advanced features initially found in higher-end GPUs, but will become widely available in mid-to-high-end GPUs during the lifetime of that specification:

* [VP_KHR_roadmap_2026](https://github.com/KhronosGroup/Vulkan-Headers/blob/main/registry/profiles/VP_KHR_roadmap.json) (Vulkan 1.4)
* [VP_KHR_roadmap_2024](https://github.com/KhronosGroup/Vulkan-Headers/blob/main/registry/profiles/VP_KHR_roadmap.json) (Vulkan 1.3)
* [VP_KHR_roadmap_2022](https://github.com/KhronosGroup/Vulkan-Headers/blob/main/registry/profiles/VP_KHR_roadmap.json) (Vulkan 1.3)

The Android Vulkan Profiles are collection of functionality that is broadly supported on Android designed from Vulkan application developers to target a large amount of devices:

* [VP_ANDROID_vulkan_profile_2025](https://github.com/KhronosGroup/Vulkan-Profiles/blob/main/profiles/Android/VP_ANDROID_vulkan_profile_2025.json) (Vulkan 1.1)
* [VP_ANDROID_vulkan_profile_2022](https://github.com/KhronosGroup/Vulkan-Profiles/blob/main/profiles/Android/VP_ANDROID_vulkan_profile_2022.json) (Vulkan 1.1)
* [VP_ANDROID_vulkan_profile_2021](https://github.com/KhronosGroup/Vulkan-Profiles/blob/main/profiles/Android/VP_ANDROID_vulkan_profile_2021.json) (Vulkan 1.0)

The VP_ANDROID_baseline_2022 profile has been deprecated by Android, rebranded into the VP_ANDROID_vulkan_profile_2022 profile.
The VP_ANDROID_baseline_2021 profile has been deprecated by Android, rebranded into the VP_ANDROID_vulkan_profile_2021 profile.

The Vulkan Requirement for Android profiles collection of functionality that is mandated for chipsets that launch (or renew Google Requirements Freeze) on Android:

* [VP_ANDROID_17_requirements](https://github.com/KhronosGroup/Vulkan-Profiles/blob/main/profiles/Android/VP_ANDROID_17_requirements.json) (Vulkan 1.4)
* [VP_ANDROID_16_requirements](https://github.com/KhronosGroup/Vulkan-Profiles/blob/main/profiles/Android/VP_ANDROID_16_requirements.json) (Vulkan 1.3)
* [VP_ANDROID_15_requirements](https://github.com/KhronosGroup/Vulkan-Profiles/blob/main/profiles/Android/VP_ANDROID_15_requirements.json) (Vulkan 1.3)

The VP_ANDROID_16_minimums profile has been deprecated by Android, rebranded into the VP_ANDROID_16_requirements profile.
The VP_ANDROID_15_minimums profile has been deprecated by Android, rebranded into the VP_ANDROID_15_requirements profile.

The `VP_LUNARG_desktop_baseline` profiles are generated arbritarily as examples of *Platform* profiles from hand picked Vulkan devices profiles. They are not expected to be used by Vulkan applications as defined. Instead, Vulkan application developers should create *Platform* profiles dedicated to their Vulkan applications that match the ecosystem of devices they want the Vulkan applications to run on.

* [VP_LUNARG_desktop_baseline_2026](https://github.com/KhronosGroup/Vulkan-Profiles/blob/main/profiles/VP_LUNARG_desktop_baseline_2026) (Vulkan 1.4)
* [VP_LUNARG_desktop_baseline_2024](https://github.com/KhronosGroup/Vulkan-Profiles/blob/main/profiles/VP_LUNARG_desktop_baseline_2024) (Vulkan 1.2)
* [VP_LUNARG_desktop_baseline_2023](https://github.com/KhronosGroup/Vulkan-Profiles/blob/main/profiles/VP_LUNARG_desktop_baseline_2023) (Vulkan 1.2)
* [VP_LUNARG_desktop_baseline_2022](https://github.com/KhronosGroup/Vulkan-Profiles/blob/main/profiles/VP_LUNARG_desktop_baseline_2022) (Vulkan 1.1)

The Vulkan Profiles files shipping within the *Vulkan SDK* are located at `$(VULKAN_SDK)/Config/VK_LAYER_KHRONOS_profiles`.

## Vulkan Profiles Tools Code Generation

Considering the complexity of the Vulkan ecosystem, there isn't a single *Vulkan Profile* that can fit all needs. As a result, on top of the predefined *Vulkan Profiles*, the *Vulkan Profiles Toolset* solution is designed around the idea of code generation.

The *Vulkan Profiles Tools* is generated against [`vk.xml`](https://github.com/KhronosGroup/Vulkan-Headers/blob/main/registry/vk.xml)(the canonical representation of the Vulkan specification) and *[Vulkan Profiles](https://github.com/KhronosGroup/Vulkan-Profiles/tree/main/profiles)*.
This design guarantees that any Vulkan developer can regenerate the entire *Vulkan Profiles Tools* solution with any new [Vulkan Header](https://github.com/KhronosGroup/Vulkan-Headers) update or any set of *Vulkan Profiles*.

The following diagram shows the Vulkan Profiles Toolset generation pipeline with every produced components:

Note: The *Profiles layer* is not yet code-generated but it will be in the future.

## The Vulkan Profiles JSON Schema

The Profiles layer builds its internal data tables by querying the capabilities of the underlying actual device, then applying the selected Vulkan Profile "on top of" of those tables. Therefore we only need to specify the features we wish to modify from the actual device; tweaking a single feature is easy. Here's an example of a valid *Vulkan Profiles* file for changing only the maximum permitted viewport size:

```json
{
    "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.2-280.json#",
    "capabilities": {
        "baseline": {
            "extensions": {},
            "features": {},
            "properties": {
                "VkPhysicalDeviceProperties": {
                    "limits": {
                        "maxViewportDimensions": [ 4096, 2048 ]
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
            "capabilities": [
                "baseline"
            ]
        }
    },
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
    ]
}

```

The top-level sections of such profiles files are processed as follows:

* `$schema` - Required. Must be the `URI` string referencing the JSON schema.
* `capabilities` - Required. Specifies the list of capabilities sets.
* `profiles` - Required. Specifies the list of profile definitions.
* `contributors` - Optional. The list of contributors of the profile file.
* `history` - Optional. The version history of the profile file.

Each entry in `capabilities` includes a reference name and a dictionary containing the following sections:

* `extensions` - Optional. List of all required extensions.
* `features` - Optional. List of all required features.
* `properties` - Optional. List of all required properties.
* `formats` - Optional. List of all required formats.
* `queueFamiliesProperties` - Optional. List of all queue families properties.
* `videoProfiles` - Optional. List of all required video profilies.

Each entry in `profiles` includes a reference name and a dictionary containing the following sections:

* `version` - Required. The revision of the profile.
* `api-version` - Required. The Vulkan API version against which the profile is written.
* `status` - Optional. The development stage of the profile. It can be either `ALPHA`, `BETA`, `STABLE` or `DEPRECATED`.
* `label` - Required. The label used to present the profile to the Vulkan developer.
* `description` - Required. The description of the profile.
* `contributors` - Optional. The list of contributors of the profile.
* `history` - Optional. The version history of the profile.
* `profiles` - Optional. The list of profiles required by the profile.
* `capabilities` - Required. The list of capability sets that can be referenced by a profile.
* `fallback` - Optional. The list of profiles recommended if the checked profile is not supported by the platform.

`profiles` may reference multiple `capabilities` which may define the same capability differently. In this case, the rule is that the last reference of that capability overrides any previously set values.

## Vulkan Profiles File Processing (`vkprofiles`)

The *Vulkan Profiles Tools* include the `vkprofiles` command line tool to transform, combine and validate profile files. This tool aims at helping Vulkan developers to create their own Vulkan profiles to reflect their Vulkan applications requirements and the Vulkan platforms they want to support.

For combinng profile files, it supports:

* Union of Vulkan Capabilities: To accumulate Vulkan capabilities from multiple profiles
* Intersection of Vulkan Capabilities: To find the common set of Vulkan capabilities of multiple profiles
* Difference of Vulkan Capabilities: To identify the specific capabilities some profiles contains but some other don't.

[GPUInfo.org](https://vulkan.gpuinfo.org/) allows downloading `device` profiles in the form of JSON files. We can use these files to create *platform* profiles by calculating their intersection.

This is the approach used to create the `VP_LUNARG_desktop_baseline` profiles in this repository.

Example:

```bash
vkprofiles combine --registry vk.xml --mode intersection --config profiles/LunarG/VP_LUNARG_desktop_baseline_config.json --output profiles/LunarG/VP_LUNARG_desktop_baseline.json

```

For detailed usage documentation on all available `vkprofiles` subcommands (`validate`, `transform`, `combine`, `schema`, `library`, `doc`), see the **[`vkprofiles` command line reference](./scripts/README.md)**.

## Vulkan Profiles JSON Validation

The JSON schema allows validating that a profiles JSON file is correctly formatted. Profile files can be validated directly with the `vkprofiles` tool:

```bash
vkprofiles validate --registry vk.xml --input path/to/profiles

```

Alternatively, web-based validators can also be used:

* [http://www.jsonschemavalidator.net/](http://www.jsonschemavalidator.net/)
* [https://json-schema-validator.herokuapp.com/](https://json-schema-validator.herokuapp.com/)
* [https://jsonschemalint.com/#/version/draft-04/markup/json/](https://jsonschemalint.com/%23/version/draft-04/markup/json/)
