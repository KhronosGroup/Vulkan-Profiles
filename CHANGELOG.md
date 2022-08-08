<!-- markdownlint-disable MD041 -->
<p align="left"><img src="https://vulkan.lunarg.com/img/NewLunarGLogoBlack.png" alt="LunarG" width="263" height="113" /></p>
<p align="left">Copyright © 2015-2022 LunarG, Inc.</p>

[![Creative Commons][3]][4]

[3]: https://i.creativecommons.org/l/by-nd/4.0/88x31.png "Creative Commons License"
[4]: https://creativecommons.org/licenses/by-nd/4.0/

<p align="center"><img src="./images/logo.png" width=400 /></p>

# Release Notes

## [Vulkan Profiles Toolset 1.3.XXX](https://github.com/LunarG/VulkanProfiles/tree/master) - August 2022

### Features:
- Add script to merge (union and intersection) multiple profiles
- Add support of the new `vk.xml` limittypes
- Add the generated `VP_LUNARG_desktop_baseline_2022` profile
- Add the generated `VP_LUNARG_desktop_portability_2022` profile

### Improvements:
- Add handling of many Vulkan capabilities
- Improve unit tests

### Bugfixes:
- Fix the impossibility to override extensions with no extension
- Fix Roadmap 2022 profile file that incorrecly required `vulkanMemoryModelAvailabilityVisibilityChains`

### Deprecation:
- Remove `VP_LUNARG_desktop_portability_2021` profile
- Remove `VP_LUNARG_desktop_portability_2021_subset` profile

## [Vulkan Profiles Toolset 1.3.216](https://github.com/KhronosGroup/Vulkan-Profiles/tree/sdk-1.3.216.0) - June 2022

### Features:
- Add `VK_KHR_portability_enumeration` support

### Bugfixes:
- Fix Roadmap 2022 JSON file definision

## [Vulkan Profiles Toolset 1.3.211](https://github.com/KhronosGroup/Vulkan-Profiles/tree/sdk-1.3.211.0) - April 2022

### Improvements:
- Add *Profiles layer* is generated with Python code

## [Vulkan Profiles Toolset 1.3.204](https://github.com/KhronosGroup/Vulkan-Profiles/tree/sdk-1.3.204.1) - February 2022

### Features:
- Add *Vulkan Profiles schema*
  - Add `VP_KHR_roadmap_2022.json` profiles file
  - Add `VP_LUNARG_desktop_portability_2021.json` profiles file
  - Add `VP_ANDROID_baseline_2021.json` profiles file
  - Add *Vulkan Profiles schema* generator using `vk.xml`
- Add *Profiles API library* to use profile in a Vulkan application code
  - Add API to check profile support by Vulkan devices
  - Add API to create `VkDevice` using a profile
  - Add APIs for profile reflection
  - Add *Profiles API library* generator using `vk.xml`
- Add *Profiles layer* to simulate profiles during application development
  - Add support of Vulkan 1.3 and all extensions
  - Add support of the *Vulkan Profiles JSON Schema*
- Add *Profiles Comparison Table*

