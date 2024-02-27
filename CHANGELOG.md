<!-- markdownlint-disable MD041 -->
<p align="left"><img src="https://vulkan.lunarg.com/img/NewLunarGLogoBlack.png" alt="LunarG" width="263" height="113" /></p>
<p align="left">Copyright © 2015-2023 LunarG, Inc.</p>

[![Creative Commons][3]][4]

[3]: https://i.creativecommons.org/l/by-nd/4.0/88x31.png "Creative Commons License"
[4]: https://creativecommons.org/licenses/by-nd/4.0/

<p align="center"><img src="./images/logo.png" width=400 /></p>

# Release Notes

## [Vulkan Profiles Toolset 1.3.XXX](https://github.com/KhronosGroup/Vulkan-Profiles/tree/main) - March 2024

### Features:
- Force device selection is not BETA anymore
- Add Khronos Roadmap 2024 profile
- Add merge script JSON config file using the new `--config` argument
- Add BETA implementation of `VULKAN_PROFILES_OBJECT` API enabling providing external Vulkan functions

### Improvements:
- Split generated documentation in three differents files:
  - `PROFILES.md` for profiles in Vulkan-Headers
  - `PROFILES_ANDROID.md` for profiles for Android platform
  - `PROFILES_ALL.md` for all profiles in the repository
- Add `--config` to `gen_profiles_solution.py` to select the API library generation configuration `Debug` (with debug message) or `Release`
- Make `--output-library-src` optional, when missing only the header-only library will be generated
- Store all Desktop Baseline profiles in `VP_LUNARG_desktop_baseline.json`
- Update `VP_LUNARG_desktop_baseline_2022` and `VP_LUNARG_desktop_baseline_2023` to version 2 with support of ARC GPUs

### Bugfixes:
- Fix features, properties and formats not listed in generated profiles table documentation
- Fix merge script change of behavior with limit outputed when all profiles don't support a capability
- Fix 'not' limitype not handle correctly by merge script
- Fix `VP_LUNARG_desktop_baseline_2024` profile, tweaked incorrect input files, add latest drivers input profiles
- Fix merge script `sparseProperties` support
- Fix `VP_DEBUG_MESSAGE_CALLBACK`

### Deprecation:
- Remove `VP_LUNARG_desktop_baseline_2024.json`, `VP_LUNARG_desktop_baseline_2023.json` and `VP_LUNARG_desktop_baseline_2022.json`

## [Vulkan Profiles Toolset 1.3.275](https://github.com/KhronosGroup/Vulkan-Profiles/tree/sdk-1.3.275.0) - January 2024

### Features:
- Add API library merging of additional application features and extensions
- Add API library support of checking the support of profiles with multiple variants
- Add API library support of creating VkDevice and VkInstance with multiple variants
- Add `vpGetProfileAPIVersion` profile API library
- Add `vpHasMultipleVariantsProfile` profile API library
- Add `vpGetProfileRequiredProfiles` profile API library and support of profiles with required profiles
- Add `VP_LUNARG_desktop_baseline_2024` profile
- Add `VP_LUNARG_minimum_requirements_1_3`, `VP_LUNARG_minimum_requirements_1_2`, `VP_LUNARG_minimum_requirements_1_1` and `VP_LUNARG_minimum_requirements_1_0` profiles
- Add `VP_ANDROID_15_minimums` profile
- Add `VP_HEADER_VERSION_COMPLETE` to identify the API library version

### Deprecation:
- Refactor API library to support profiles with multiple variants
- Remove API library overriding features and extensions modes
- Remove `VP_LUNARG_desktop_portability_2022` profile

## [Vulkan Profiles Toolset 1.3.261](https://github.com/KhronosGroup/Vulkan-Profiles/tree/sdk-1.3.261.0) - August 2023

### Bugfixes:
- Fix query of capabilities of aliased extensions
- Fix error message typo
- Fix library generation when using profiles with multiple variants #464

## [Vulkan Profiles Toolset 1.3.250](https://github.com/KhronosGroup/Vulkan-Profiles/tree/sdk-1.3.250.0) - June 2023

### Features:
- Add a setting to control how layer features are initialized when a feature is not specified in a selected profile.
- Add Profiles layer loading of profiles with multiple variants. All variants are emulated.

### Improvements:
- Indicate the corresponding device name to log message when a profile capability is not supported by the given device.

### Bugfixes:
- Fix invalid message when trying to override a supported capability to unsupported #393

## [Vulkan Profiles Toolset 1.3.243](https://github.com/KhronosGroup/Vulkan-Profiles/tree/sdk-1.3.243.0) - April 2023

### Features:
- Add LunarG Desktop Baseline 2023 profile (Vulkan 1.2)
- Update LunarG Desktop Baseline 2022 profile (Vulkan 1.1) to match updated MoltenVK support.

## [Vulkan Profiles Toolset 1.3.239](https://github.com/KhronosGroup/Vulkan-Profiles/tree/sdk-1.3.239.0) - February 2023

### Features:
- Add Android Baseline 2022 profile

### Improvements:
- Improve handling of `VkFormatProperties*` from profile data which can be query with any Vulkan functions

## [Vulkan Profiles Toolset 1.3.236](https://github.com/KhronosGroup/Vulkan-Profiles/tree/sdk-1.3.236.0) - December 2022

### Features:
- Add merge script support of `VkFormatProperties3` #304 #237

### Improvements:
- Switch `BETA` to `STABLE` development status.

### Bugfixes:
- Fix warning when emulating `VK_EXT_portability_subset`

## [Vulkan Profiles Toolset 1.3.231](https://github.com/KhronosGroup/Vulkan-Profiles/tree/sdk-1.3.231.0) - October 2022

### Features:
- Add layer settings to override portability subset. #295
- Add OR of capabilities to profile schema, version 0.8.1 #274
- Add optional capabilities to profile schema, version 0.8.1 #274

### Improvements:
- Improve message reports for unsupported capabilities by the device.
- Improve message reports for unmodifiable Vulkan capabilities.

### Bugfixes:
- Fix handling of 64 bits properties by the merge tool. #294
- Fix float build warnings. #291

## [Vulkan Profiles Toolset 1.3.224](https://github.com/KhronosGroup/Vulkan-Profiles/tree/sdk-1.3.224.0) - August 2022

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

