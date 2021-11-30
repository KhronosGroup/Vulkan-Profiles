<!-- markdownlint-disable MD041 -->
<p align="left"><img src="https://vulkan.lunarg.com/img/NewLunarGLogoBlack.png" alt="LunarG" width=263 height=113 /></p>
<p align="left">Copyright © 2021-2022 LunarG, Inc.</p>

<p align="center"><img src="./images/logo.png" width=400 /></p>

[![Creative Commons][3]][4]

[3]: https://i.creativecommons.org/l/by-nd/4.0/88x31.png "Creative Commons License"
[4]: https://creativecommons.org/licenses/by-nd/4.0/

*Vulkan Profiles Tools* is a collection of tools for Vulkan application developers to leverage *Vulkan Profiles* while developing a Vulkan application.

* **[Change Log](./CHANGELOG.md)**: The history of *Vulkan Profiles Tools* releases.
* **[Bug reports](https://github.com/LunarG/VulkanProfiles/issues)**: Open a GitHub issue when you encounter a bug.
* **[Roadmap](https://github.com/LunarG/VulkanProfiles/projects)**: Follow *Vulkan Profiles Tools* future developments.
* **[Contributing](./PROFILES.md)**: The definitions of *Vulkan Profiles*.
* **[Contributing](./TUTORIAL.md)**: How to use the *Vulkan Profiles Tools*.

--------------
## Vulkan Profiles Library

The *Vulkan Profiles Library* provides an API to leverage *Vulkan Profiles* in Vulkan applications code with the following features:
- Checking whether the some *Vulkan Profiles* are supported
- Creating `VkDevice` instances with the features of a *Vulkan Profile* emabled
- Reflecting on *Vulkan Profiles* features

--------------
### Platforms Support

| Windows            | Linux               | macOS              | iOS                | Android            |
| ------------------ | ------------------- | ------------------ | ------------------ | ------------------ |
| :heavy_check_mark: | :heavy_check_mark:  | :heavy_check_mark: | :x:                | :heavy_check_mark: |

--------------
## VK_LAYER_KHRONOS_device_simulation

The validation layer assists developers in checking their code for improper use of Vulkan, but
these checks take into account only the limits of the test platform. To ensure an application
properly handles multiple platforms, these checks must be run on all platforms of interest to the
developer. Combinations of GPUs, ICDs, drivers, and operating systems to name a few factors
create an exponential number of possible test platforms, which is infeasible for a developer to
obtain and maintain.

The *Device Simulation Layer* (also called Devsim) seeks to mitigate this obstacle by providing a
method to simulate arbitrary property, feature, and limit combinations representing different
platforms for specific test cases. For example, it can be configured as though the application is
running on a device with the minimum properties, features, and limits defined by the Vulkan
specification.

--------------
### Platforms Support

| Windows            | Linux               | macOS              | iOS                | Android            |
| ------------------ | ------------------- | ------------------ | ------------------ | ------------------ |
| :heavy_check_mark: | :heavy_check_mark:  | :heavy_check_mark: | :x:                | :heavy_check_mark: |

--------------
### Vulkan Capabilities Schema

The *Vulkan Capabilities Schema* aims at providing a human readable format to store and share data representing properties, features, formats, etc.

This format is used to store *Vulkan Profiles*.

--------------
### Vulkan Profiles

This repository contains representation of profiles following the *Vulkan Capabilities Schema*:
- [VP_KHR_roadmap_2022](./profiles/VP_KHR_roadmap_2022.json)
- [VP_LUNARG_desktop_portability_2021](./profiles/VP_LUNARG_desktop_portability_2021.json)
- [VP_ANDROID_angle_es31](./profiles/VP_ANDROID_angle_es31.json)
- [VP_ANDROID_baseline_2022](./profiles/VP_ANDROID_baseline_2022.json)

--------------
## Downloads

*Vulkan Profiles Tools* is delivered with the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home).


