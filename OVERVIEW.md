<!-- markdownlint-disable MD041 -->
<p align="left"><img src="https://vulkan.lunarg.com/img/NewLunarGLogoBlack.png" alt="LunarG" width=263 height=113 /></p>
<p align="left">Copyright © 2021-2022 LunarG, Inc.</p>

<p align="center"><img src="./images/logo.png" width=400 /></p>

[![Creative Commons][3]][4]

[3]: https://i.creativecommons.org/l/by-nd/4.0/88x31.png "Creative Commons License"
[4]: https://creativecommons.org/licenses/by-nd/4.0/

# Vulkan Profiles Tools Overview

The *Vulkan Profiles Tools* are a collection of components for Vulkan application developers to build **portable** Vulkan applications thanks to *Vulkan Profiles*.

The *Vulkan Profiles Tools* interpret the concept of *Vulkan Profiles* as a concept that can be appropriated by any member of the Vulkan ecosystem.
As a result, *Vulkan Profiles* can serve multiple use case:
- *Roadmap profiles*: To express guidance on the future direction of Vulkan devices.
- *Platform profiles*: To express the Vulkan support available on different platforms.
- *Device profiles*: To express the Vulkan support of a single Vulkan driver for a Vulkan device.
- *Engine profiles*: To express some rendering code paths requirements of an engine.
- Etc.

Conceptually, *Vulkan Profiles* can be understood as the explicit expression and formalization of Vulkan requirements and supports by different members of the Vulkan ecosystem.

All *the Vulkan Profiles Tools* remains under development and are delivered as part of the [***Vulkan SDK***](https://www.lunarg.com/vulkan-sdk/) under the **BETA** status.

## Vulkan Profiles Tools Components

The *Vulkan Profiles Tools* are the following components:
- ***The Vulkan Profiles JSON Schemas***
  - A data format to exchange Vulkan Capabilities: extensions, features, properties and formats.
  - Each revision of Vulkan API is represented by a schema that supersed older version of Vulkan API.
  - The currently schema covers all Vulkan 1.3 and extensions.
- [***The Vulkan Profiles API library***](./library/TUTORIAL.md)
  -  A header-only C++ library to use Vulkan Profiles in Vulkan applications compatible with Vulkan 1.0 + `VK_KHR_get_physical_device_properties2` drivers.
- [***The Vulkan Profiles Layer***](./layer/TUTORIAL.md)
  - A layer to help developing Vulkan Applications which respect the requirements of a chosen *Vulkan Profile*.
- [***The Vulkan Profiles Documentation***](./PROFILES.md)
  - A table to compare *Vulkan Profiles* capabilities against each other.

Furthermore, the repository includes implementions of some *Vulkan Profiles* using the *Vulkan Profiles JSON Schema*, currently:
- [VP_KHR_roadmap_2022.json](./profiles/VP_KHR_roadmap_2022.json)
- [VP_ANDROID_baseline_2021.json](./profiles/VP_ANDROID_baseline_2021.json)
- [VP_LUNARG_desktop_portability_2021.json](./profiles/VP_LUNARG_desktop_portability_2021.json)

## Vulkan Profiles Tools Code Generation

Considering the complexcity of the Vulkan ecosystem, there isn't a single *Vulkan Profile* that can fit all needs. As a result, on top of the predefined *Vulkan Profiles*, the *Vulkan Profiles Tools* solution is designed around the idea of code generation.

The *Vulkan Profiles Tools* are generated against [`vk.xml`](https://github.com/KhronosGroup/Vulkan-Headers/blob/main/registry/vk.xml)(the canonical representation of the Vulkan specification) and [*Vulkan Profiles*](https://github.com/KhronosGroup/Vulkan-Profiles/tree/master/profiles) written against the Vulkan Profiles JSON Schemas.
This design allows any Vulkan developer to regenerate the entier *Vulkan Profiles Tools* solution with any [Vulkan Header](https://github.com/KhronosGroup/Vulkan-Headers) update and with Vulkan developer-defined profiles.

[BUILD.md](./BUILD.md) provides instructions to build the repository, which allows regenerating the *Vulkan Profiles Tools* with supports of new profiles or updated profiles.

The following diagram shows the Vulkan Profiles Tools generation pipeline with every produced components:
![Vulkan Profiles](./images/overview.png)

Note that the *Profiles layer* is not yet code generated but this is in the roadmap.

## Future Work:
  - [GPUInfo.org](https://gpuinfo.org/) will allow downloading profiles files for any Vulkan drivers report.
  - [`vulkaninfo`](https://vulkan.lunarg.com/doc/view/latest/windows/vulkaninfo.html) will allow generating profiles files.
  - The [Khronos Vulkan-Samples](https://github.com/KhronosGroup/Vulkan-Samples) repository will include a sample that use the *Vulkan Profiles API Library*.

## The Vulkan Profiles JSON Schema

The Profiles layer builds its internal data tables by querying the capabilities of the underlying actual device, then applying each of the configuration files “on top of” those tables. Therefore you only need to specify the features you wish to modify from the actual device; tweaking a single feature is easy. Here’s an example of a valid configuration file for changing only the maximum permitted viewport size:

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
* `$schema` - Required. Must be the URI string referencing the JSON schema.
* `capabilities` - Required. Specifies the list of capabilities sets.
* `profiles` - Required. Specifies the list of profile definitions.

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

`profiles` may reference multiple `capabilities` which may define differently the same capability. In this case, the rule is that last reference of that capability overwrite the previously set value.

## Vulkan Profiles JSON validation

The JSON schema allows validating that the profiles file are correctly defined. This can be done by many tools including the following web-based validators:
- http://www.jsonschemavalidator.net/
- https://json-schema-validator.herokuapp.com/
- https://jsonschemalint.com/#/version/draft-04/markup/json/
