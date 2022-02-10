<!-- markdownlint-disable MD041 -->
<p align="left"><img src="https://vulkan.lunarg.com/img/NewLunarGLogoBlack.png" alt="LunarG" width=263 height=113 /></p>
<p align="left">Copyright © 2015-2022 LunarG, Inc.</p>

[![Creative Commons][3]][4]

[3]: https://i.creativecommons.org/l/by-nd/4.0/88x31.png "Creative Commons License"
[4]: https://creativecommons.org/licenses/by-nd/4.0/

# Vulkan Profiles API library

- [Vulkan Profiles library](#vulkan-profiles-library)
  - [Overview](#overview)
  - [Building](#building)
  - [Basic usage](#basic-usage)
  - [Advanced usage](#advanced-usage)
    - [Overriding API version](#overriding-api-version)
    - [Overriding extensions](#overriding-extensions)
    - [Merging extensions](#merging-extensions)
    - [Overriding features](#overriding-features)
    - [Merging/overriding individual features](#mergingoverriding-individual-features)
    - [Disabling robust access](#disabling-robust-access)
  - [API reference](#api-reference)
    - [Preprocessor definitions](#preprocessor-definitions)
    - [Profile support and usage](#profile-support-and-usage)
      - [Check instance level support](#check-instance-level-support)
      - [Create instance with profile](#create-instance-with-profile)
      - [Check device level support](#check-device-level-support)
      - [Create device with profile](#create-device-with-profile)
    - [Profile queries](#profile-queries)
      - [Query profiles](#query-profiles)
      - [Query profile fallbacks](#query-profile-fallbacks)
      - [Query profile instance extensions](#query-profile-instance-extensions)
      - [Query profile device extensions](#query-profile-device-extensions)
      - [Query profile features](#query-profile-features)
      - [Query profile device properties](#query-profile-device-properties)
      - [Query profile queue family properties](#query-profile-queue-family-properties)
      - [Query profile format properties](#query-profile-format-properties)

## Overview

The Vulkan Profiles library is a helper library that provides the following set of convenience APIs to developers:

* Capability introspection APIs to query the extensions, features, properties, formats, and queue families required by a particular Vulkan profile.
* APIs to verify instance-level and device-level support for a particular Vulkan profile.
* Instance and device creation APIs that automatically enable the extensions and features required by a particular Vulkan profile.

## Building

The Vulkan Profiles library is provided as a header-only C++ library (`vulkan/vulkan_profiles.hpp`) that is bundled with the Vulkan SDK. C++ applications thus can simply use the Vulkan Profiles library by including this header-only C++ library.

The library is primarily designed to be dynamically linked to the Vulkan implementation (loader or ICD). If applications want to dynamically load Vulkan then they have to make sure (one way or another) that the Vulkan API symbols seen by the Vulkan Profiles header-only library are valid and correspond to the dynamically loaded entry points.

In order to enable support for other language bindings, the library is also available in a header + source pair (`vulkan/vulkan_profiles.h` and `vulkan_profiles.cpp`). There is no build configuration for this variant of the library as it's not meant to be used as a standalone static or dynamic library. Instead, developers can drop the files into their own project to build the Vulkan profiles library into it. This may also come handy if the developer would like to optimize compilation times by not having to include the rather large header-only library in multiple source files.

The profile definitions are enabled depending on the pre-processor definitions coming from the Vulkan headers; thus the application has to make sure to configure the right set of pre-processor definitions. As an example, the `VP_ANDROID_baseline_2021` profile depends on the `VK_KHR_android_surface` instance extension; thus in order to use this profile the application must define `VK_USE_PLATFORM_ANDROID_KHR`.

## Basic usage

The typically expected usage of the Vulkan Profiles library is for applications to target a specific profile with their application and leave it to the Vulkan Profiles library to enable any necessary extensions and features required by that profile.

In order to do so, the application first has to make sure that the Vulkan implementation supports the selected profile as follows:

```C++
    VkResult result = VK_SUCCESS;
    VkBool32 supported = VK_FALSE;
    VpProfileProperties profile{
        VP_KHR_ROADMAP_2022_NAME,
        VP_KHR_ROADMAP_2022_SPEC_VERSION
    };

    result = vpGetInstanceProfileSupport(nullptr, &profile, &supported);
    if (result != VK_SUCCESS) {
        // something went wrong
        ...
    }
    else if (supported != VK_TRUE) {
        // profile is not supported at the instance level
        ...
    }
```

The above code example verifies the instance-level profile requirements of the `VP_KHR_roadmap_2022` profile, including required API version and instance extensions.

If the profile is supported by the Vulkan implementation at the instance level, then a Vulkan instance can be created with the instance extensions required by the profile as follows:

```C++
    VkApplicationInfo vkAppInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
    // Set API version to the minimum API version required by the profile
    vkAppInfo.apiVersion = VP_KHR_ROADMAP_2022_MIN_API_VERSION;
    VkInstanceCreateInfo vkCreateInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    vkCreateInfo.pApplicationInfo = &vkAppInfo;
    // set up your own instance creation parameters, except instance extensions
    // as those will come from the profile
    ...

    VpInstanceCreateInfo vpCreateInfo{};
    createInfo.pCreateInfo = &vkCreateInfo;
    createInfo.pProfile = &profile;

    VkInstance instance = VK_NULL_HANDLE;
    result = vpCreateInstance(&vpCreateInfo, nullptr, &instance);
    if (result != VK_SUCCESS) {
        // something went wrong
        ...
    }
```

The above code example will create a Vulkan instance with the API version and instance extensions required by the profile (unless the application overrides any of them, as explained later).

Make sure to set the `apiVersion` in the `VkApplicationInfo` structure at least to the minimum API version required by the profile, as seen above, to ensure the correct Vulkan API version is used.

Once a Vulkan instance is created, the application can check whether individual physical devices support the selected profile as follows:

```C++
    result = vpGetPhysicalDeviceProfileSupport(instance, physicalDevice,
                                               &profile, &supported);
    if (result != VK_SUCCESS) {
        // something went wrong
        ...
    }
    else if (supported != VK_TRUE) {
        // profile is not supported at the device level
        ...
    }
```

Finally, once a physical device supporting the profile is selected, a Vulkan device can be created with the device extensions and features required by the profile as follows:

```C++
    VkDeviceCreateInfo vkCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    // set up your own device creation parameters, except device extensions
    // and device features as those will come from the profile
    ...

    VpDeviceCreateInfo vpCreateInfo{};
    createInfo.pCreateInfo = &vkCreateInfo;
    createInfo.pProfile = &profile;

    VkDevice device = VK_NULL_HANDLE;
    result = vpCreateDevice(physicalDevice, &vpCreateInfo, nullptr, &device);
    if (result != VK_SUCCESS) {
        // something went wrong
        ...
    }
```

## Advanced usage

### Overriding API version

By default it's expected that the application either does not provide its own `VkApplicationInfo` when calling `vpCreateInstance` or it sets its `apiVersion` to the minimum API version required by the profile as defined by the corresponding `<VP_VENDOR_NAME>_MIN_API_VERSION` preprocessor definition.

If the application would like to use a different API version, then it should use its own `apiVersion` in `VkApplicationInfo` at instance creation time instead (e.g. to request a newer API version).

Using an older version than the one required by the profile may result in unexpected behavior if any of the features required by the profile are not present in the specified version of the Vulkan API.

### Overriding extensions

By default the application must not provide its own set of instance and/or device extensions when using `vpCreateDevice` and/or `vpCreateInstance`, as the list of enabled extensions comes from the profile definition.

If the application needs to provide its own set of instance or device extensions, then the `VP_INSTANCE_CREATE_OVERRIDE_EXTENSIONS_BIT` and/or `VP_DEVICE_CREATE_OVERRIDE_EXTENSIONS_BIT` flags can be used, as in the examples below:

```C++
    VkInstanceCreateInfo vkCreateInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    ...
    // Specify extension list to use, overriding profile extension list
    vkCreateInfo.enabledExtensionCount = ...
    vkCreateInfo.ppEnabledExtensionNames = ...

    VpInstanceCreateInfo vpCreateInfo{};
    createInfo.pCreateInfo = &vkCreateInfo;
    createInfo.pProfile = &profile;
    // Use extension override flag
    createInfo.flags = VP_INSTANCE_CREATE_OVERRIDE_EXTENSIONS_BIT;
    ...
```

```C++
    VkDeviceCreateInfo vkCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    ...
    // Specify extension list to use, overriding profile extension list
    vkCreateInfo.enabledExtensionCount = ...
    vkCreateInfo.ppEnabledExtensionNames = ...

    VpDeviceCreateInfo vpCreateInfo{};
    createInfo.pCreateInfo = &vkCreateInfo;
    createInfo.pProfile = &profile;
    // Use extension override flag
    createInfo.flags = VP_DEVICE_CREATE_OVERRIDE_EXTENSIONS_BIT;
    ...
```

### Merging extensions

If the application would like to specify additional extensions besides the ones defined by the profile, then the `VP_INSTANCE_CREATE_MERGE_EXTENSIONS_BIT` and/or `VP_DEVICE_CREATE_MERGE_EXTENSIONS_BIT` flags can be used to request the merging of the application-provided extension list with the profile extensions, as in the examples below:

```C++
    VkInstanceCreateInfo vkCreateInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    ...
    // Specify additional extensions to use beyond the profile-defined ones
    vkCreateInfo.enabledExtensionCount = ...
    vkCreateInfo.ppEnabledExtensionNames = ...

    VpInstanceCreateInfo vpCreateInfo{};
    createInfo.pCreateInfo = &vkCreateInfo;
    createInfo.pProfile = &profile;
    // Use extension merge flag
    createInfo.flags = VP_INSTANCE_CREATE_MERGE_EXTENSIONS_BIT;
    ...
```

```C++
    VkDeviceCreateInfo vkCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    ...
    // Specify additional extensions to use beyond the profile-defined ones
    vkCreateInfo.enabledExtensionCount = ...
    vkCreateInfo.ppEnabledExtensionNames = ...

    VpDeviceCreateInfo vpCreateInfo{};
    createInfo.pCreateInfo = &vkCreateInfo;
    createInfo.pProfile = &profile;
    // Use extension merge flag
    createInfo.flags = VP_DEVICE_CREATE_MERGE_EXTENSIONS_BIT;
    ...
```

### Overriding features

By default, profile-defined feature structures must not be specified as part of the `pNext` chain of `VkDeviceCreateInfo` when using `vpCreateDevice`, as the profile-defined feature structures are automatically added to the `pNext` chain, and duplicate feature structures would otherwise result in undefined behavior.

If the application needs to override any subset of the feature structures otherwise defined by the profile, then the `VP_DEVICE_CREATE_OVERRIDE_FEATURES_BIT` flag can be used, as in the example below:

```C++
    // We want to use our own VkPhysicalDeviceVulkan11Features structure,
    // overriding any feature enablement in that structure as defined by
    // the profile
    VkPhysicalDeviceVulkan11Features overriddenVulkan11Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES };
    ...

    VkDeviceCreateInfo vkCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    // Include overridden features in the pNext chain of VkDeviceCreateInfo
    vkCreateInfo.pNext = &overriddenVulkan11Features;
    ...

    VpDeviceCreateInfo vpCreateInfo{};
    createInfo.pCreateInfo = &vkCreateInfo;
    createInfo.pProfile = &profile;
    // Use feature override flag
    createInfo.flags = VP_DEVICE_CREATE_OVERRIDE_FEATURES_BIT;
    ...
```

In this case feature structures from the profile are only included in the final `pNext` chain passed to `vkCreateDevice` if a corresponding overriding feature structure isn't provided by the application.

Sometimes the application needs to override all feature structures with its own, i.e. no feature defined in the profile should be automatically enabled. This can be requested using the `VP_DEVICE_CREATE_OVERRIDE_ALL_FEATURES_BIT` flag, as in the example below:

```C++
    VpDeviceCreateInfo vpCreateInfo{};
    createInfo.pCreateInfo = &vkCreateInfo;
    createInfo.pProfile = &profile;
    // We want to enable only the features we specify, so we override
    // all profile-defined features
    createInfo.flags = VP_DEVICE_CREATE_OVERRIDE_ALL_FEATURES_BIT;
    ...
```

### Merging/overriding individual features

> **NOTE:** Currently there is no `VP_DEVICE_CREATE_MERGE_FEATURES_BIT` that could allow merging individual feature enablement within feature structures due to the constness of the `pNext` chain of `VkDeviceCreateInfo`, as reconstructing the `pNext` chain entirely would preclude supporting any structures in the `pNext` chain unknown to the Vulkan Profiles library.

While currently there's no way to request the automatic merging of individual feature enablements within feature structures, the application can merge features on its own, potentially disabling features that would be otherwise enabled by the profile, or enabling features that wouldn't be enabled by the profile.

In order to achieve this, any feature structure that the application intends to provide on its own can be pre-populated with the feature data of the profile using the `vpGetProfileFeatures` API, as in the example below:

```C++
    // We intend to enable/disable some features in the following feature 
    // structures but otherwise we want to keep the rest of the features
    // enabled/disabled according to the profile
    VkPhysicalDeviceVulkan11Features vulkan11Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES };
    VkPhysicalDeviceVulkan12Features vulkan12Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };

    // We pre-populate the structures with the feature data of the profile
    // by passing all structures as a pNext chain to vpGetProfileFeatures
    vulkan11Features.pNext = &vulkan12Features;
    vpGetProfileFeatures(&profile, &vulkan11Features);

    // We then set/override any features as we wish
    vulkan11Features.multiview = VK_FALSE;
    vulkan12Features.drawIndirectCount = VK_TRUE;
    ...

    VkDeviceCreateInfo vkCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    // We add our own feature structures to the pNext chain
    vkCreateInfo.pNext = &vulkan11Features;
    ...

    VpDeviceCreateInfo vpCreateInfo{};
    createInfo.pCreateInfo = &vkCreateInfo;
    createInfo.pProfile = &profile;
    // Finally, we specify the feature override flag so that the feature
    // structures provided will be used instead of the profile defaults
    createInfo.flags = VP_DEVICE_CREATE_OVERRIDE_FEATURES_BIT;
    ...
```

### Disabling robust access

Many profiles require robust buffer and/or image access; however, applications often prefer to opt out from these features for performance reasons. This can be requested by overriding individual features as explained in the previous section; however, as it's quite a common usage pattern, explicit device creation flags are provided to make it easier to do so.

If the `VP_DEVICE_CREATE_DISABLE_ROBUST_BUFFER_ACCESS_BIT` flag is specified, then the `robustBufferAccess` feature (defined by `VkPhysicalDeviceFeatures`) and the `robustBufferAccess2` feature (defined by `VkPhysicalDeviceRobustness2FeaturesEXT`) are disabled, unless the application provides its own overridden feature structures containing the features.

If the `VP_DEVICE_CREATE_DISABLE_ROBUST_IMAGE_ACCESS_BIT` flag is specified, then the `robustImageAccess` feature (defined by `VkPhysicalDeviceImageRobustnessFeatures` and `VkPhysicalDeviceVulkan13Features`) and the `robustImageAccess2` feature (defined by `VkPhysicalDeviceRobustness2FeaturesEXT`) are disabled, unless the application provides its own overridden feature structures containing the features.

Using `VP_DEVICE_CREATE_DISABLE_ROBUST_ACCESS_BIT` disables both buffer and image robustness features.

## API reference

### Preprocessor definitions

Each profile defines the following set of preprocessor definitions:

* `<VP_VENDOR_name>` - Defined to `1` in the presence of the profile
* `<VP_VENDOR_NAME>_NAME` - The character string name of the profile
* `<VP_VENDOR_NAME>_SPEC_VERSION` - The version of the profile definition
* `<VP_VENDOR_NAME>_MIN_API_VERSION` - The minimum Vulkan API version required by the profile

Where:
* `<VP_VENDOR_name>` is the name of the profile
* `<VP_VENDOR_NAME>` is the upper-case name of the profile

### Profile support and usage

The Vulkan Profile library offers a set of APIs to verify support for a particular Vulkan profile and to create Vulkan instances and devices using the extensions and features required by the profile.

#### Check instance level support

In order to query whether the Vulkan implementation supports the necessary instance level requirements (API version and instance extensions) of a particular profile, use the following command:

```C++
VkResult vpGetInstanceProfileSupport(
    const char*                     pLayerName,
    const VpProfileProperties*      pProfile,
    VkBool32*                       pSupported);
```

Where:
* `pLayerName` is either `NULL` or a pointer to a null-terminated UTF-8 string naming the layer to retrieve extensions from (analogous to the corresponding parameter of `vkEnumerateInstanceExtensionProperties`).
* `pProfile` is a pointer to the `VpProfileProperties` structure specifying the profile to check support for.
* `pSupported` is a pointer to a `VkBool32`, which is set to `VK_TRUE` to indicate support, and `VK_FALSE` otherwise.

The `VpProfileProperties` structure is defined as follows:

```C++
typedef struct VpProfileProperties {
    char                            profileName[VP_MAX_PROFILE_NAME_SIZE];
    uint32_t                        specVersion;
} VpProfileProperties;
```

Where:
* `profileName` is the name of the profile
* `specVersion` is the version of the profile specification

#### Create instance with profile

The Vulkan Profiles library provides the following helper function that enables easier adoption of profiles by automatically including profile requirements in the Vulkan instance creation process:

```C++
VkResult vpCreateInstance(
    const VpInstanceCreateInfo*     pCreateInfo,
    const VkAllocationCallbacks*    pAllocator,
    VkInstance*                     pInstance);
```

Where:
* `pCreateInfo` is a pointer to the `VpInstanceCreateInfo` structure specifying the instance creation info, as described below.
* `pAllocator` controls host memory allocation and is analogous to the corresponding parameter of `vkCreateInstance`.
* `pInstance` points to a `VkInstance` handle in which the resulting instance is returned.

The `VpInstanceCreateInfo` structure is defined as follows:

```C++
typedef struct VpInstanceCreateInfo {
    const VkInstanceCreateInfo*     pCreateInfo;
    const VpProfileProperties*      pProfile;
    VpInstanceCreateFlags           flags;
} VpInstanceCreateInfo;
```

Where:
* `pCreateInfo` is a pointer to the `VkInstanceCreateInfo` structure specifying the usual Vulkan instance creation info.
* `pProfile` is a pointer to the `VpProfileProperties` structure specifying the profile to enable.
* `flags` contains zero or more of the `VpInstanceCreateFlagBits` values, as described below.

The default behavior of `vpCreateInstance` is to enable only the instance extensions that are required by the profile; the `VkInstanceCreateInfo` provided by the application must not enable any instance extensions itself.

If the application specifies the `VP_INSTANCE_CREATE_OVERRIDE_EXTENSIONS_BIT` then only the extensions enabled in the `VkInstanceCreateInfo` provided by the application will be used.

If the application specifies the `VP_INSTANCE_CREATE_MERGE_EXTENSIONS_BIT` then the extension lists are merged and both the extensions required by the profile and those enabled by the application in the `VkInstanceCreateInfo` structure will be enabled.

If the application-provided `VkInstanceCreateInfo` does not specify a `VkApplicationInfo` structure then a `VkApplicationInfo` with an `apiVersion` field equaling the minimum required API version of the profile will be added. If the application does provide a `VkApplicationInfo` it has to make sure that its `apiVersion` field is greater than or equal to the minimum required API version of the profile (e.g. by using the corresponding preprocessor definition).

#### Check device level support

In order to query whether a Vulkan physical device supports the necessary device level requirements (API version, device extensions, device features and properties/limits) of a particular profile, use the following command:

```C++
VkResult vpGetPhysicalDeviceProfileSupport(
    VkInstance                      instance,
    VkPhysicalDevice                physicalDevice,
    const VpProfileProperties*      pProfile,
    VkBool32*                       pSupported);
```

Where:
* `instance` is the Vulkan instance.
* `physicalDevice` is the physical device to check support on.
* `pProfile` is a pointer to the `VpProfileProperties` structure specifying the profile to check support for.
* `pSupported` is a pointer to a `VkBool32`, which is set to `VK_TRUE` to indicate support, and `VK_FALSE` otherwise.

#### Create device with profile

The Vulkan Profiles library provides the following helper function that enables easier adoption of profiles by automatically including profile requirements in the Vulkan device creation process:

```C++
VkResult vpCreateDevice(
    VkPhysicalDevice                physicalDevice,
    const VpDeviceCreateInfo*       pCreateInfo,
    const VkAllocationCallbacks*    pAllocator,
    VkDevice*                       pDevice);
```

Where:
* `physicalDevice` is the physical device to create the logical device for.
* `pCreateInfo` is a pointer to the `VpDeviceCreateInfo` structure specifying the device creation info, as described below.
* `pAllocator` controls host memory allocation and is analogous to the corresponding parameter of `vkCreateDevice`.
* `pDevice` points to a `VkDevice` handle in which the resulting device is returned.

The default behavior of `vpCreateDevice` is to enable only the device extensions that are required by the profile and the `VkInstanceCreateInfo` provided by the application must not enable any instance extensions itself.

Similarly, the default behavior of `vpCreateDevice` is to enable features that are required by the profile and the `pNext` chain of `VkDeviceCreateInfo` must not contain any feature structures that are defined by the profile itself.

If the application specifies the `VP_DEVICE_CREATE_OVERRIDE_EXTENSIONS_BIT`, then only the extensions enabled in the `VkDeviceCreateInfo` provided by the application will be used.

If the application specifies the `VP_DEVICE_CREATE_MERGE_EXTENSIONS_BIT`, then the extension lists are merged and both the extensions required by the profile and those enabled by the application in the `VkDeviceCreateInfo` structure will be enabled.

If the application specifies the `VP_DEVICE_CREATE_OVERRIDE_FEATURES_BIT` and if a feature structure is defined by both the profile and the `pNext` chain of `VkDeviceCreateInfo`, then the one provided by the application is used, overriding the enable state of any feature that otherwise would come from the profile.

If the application specifies the `VP_DEVICE_CREATE_OVERRIDE_ALL_FEATURES_BIT`, then only the feature structures provided by the application in the `pNext` chain of `VkDeviceCreateInfo` are used, overriding all profile feature data.

> **NOTE:** Currently there is no `VP_DEVICE_CREATE_MERGE_FEATURES_BIT` that could allow merging individual feature enablement within feature structures due to the constness of the `pNext` chain of `VkDeviceCreateInfo`, as reconstructing the `pNext` chain entirely would preclude supporting any structures in the `pNext` chain unknown to the Vulkan Profiles library.

### Profile queries

The Vulkan Profile library offers a set of APIs to query the capabilities defined in a particular Vulkan profile, and may be used both for development-time checking of profile capabilities and for facilitating the construction of custom extension and feature configurations that use only a subset of the capabilities required by the profile.

#### Query profiles

In order to query the list of available profiles, use the following command:

```C++
VkResult vpGetProfiles(
    uint32_t*                       pPropertyCount,
    VpProfileProperties*            pProperties);
```

Where:
* `pPropertyCount` is a pointer to an integer related to the number of profiles available or queried, as described below.
* `pProperties` is either `NULL` or a pointer to an array of `VpProfileProperties` structures.

If `pProperties` is `NULL`, then the number of profiles available is returned in `pPropertyCount`. Otherwise, `pPropertyCount` must point to a variable set by the user to the number of elements in the `pProperties` array, and on return the variable is overwritten with the number of structures actually written to `pProperties`. If `pPropertyCount` is less than the number of profiles available, at most `pPropertyCount` structures will be written, and `VK_INCOMPLETE` will be returned instead of `VK_SUCCESS`, to indicate that not all the available properties were returned.

#### Query profile fallbacks

Some profiles have recommended fallback profiles, i.e. profiles to use as a fallback in case the original profile is not supported. In order to query the list of fallback profiles for a given profile, use the following command:

```C++
VkResult vpGetProfileFallbacks(
    const VpProfileProperties*      pProfile,
    uint32_t*                       pPropertyCount,
    VpProfileProperties*            pProperties);
```

Where:
* `pProfile` is a pointer to the `VpProfileProperties` structure specifying the profile whose fallback profiles are queried.
* `pPropertyCount` is a pointer to an integer related to the number of fallback profiles available or queried, as described below.
* `pProperties` is either `NULL` or a pointer to an array of `VpProfileProperties` structures.

If `pProperties` is `NULL`, then the number of fallback profiles available for the specified profile is returned in `pPropertyCount`. Otherwise, `pPropertyCount` must point to a variable set by the user to the number of elements in the `pProperties` array, and on return the variable is overwritten with the number of structures actually written to `pProperties`. If `pPropertyCount` is less than the number of fallback profiles available for the specified profile, at most `pPropertyCount` structures will be written, and `VK_INCOMPLETE` will be returned instead of `VK_SUCCESS`, to indicate that not all the available properties were returned.

#### Query profile instance extensions

In order to query the list of instance extensions required by a profile, use the following command:

```C++
VkResult vpGetProfileInstanceExtensionProperties(
    const VpProfileProperties*      pProfile,
    uint32_t*                       pPropertyCount,
    VkExtensionProperties*          pProperties);
```

Where:
* `pProfile` is a pointer to the `VpProfileProperties` structure specifying the queried profile.
* `pPropertyCount` is a pointer to an integer related to the number of instance extensions available or queried, as described below.
* `pProperties` is either `NULL` or a pointer to an array of `VkExtensionProperties` structures.

If `pProperties` is `NULL`, then the number of instance extensions required by the profile is returned in `pPropertyCount`. Otherwise, `pPropertyCount` must point to a variable set by the user to the number of elements in the `pProperties` array, and on return the variable is overwritten with the number of structures actually written to `pProperties`. If `pPropertyCount` is less than the number of instance extensions required by the profile, at most `pPropertyCount` structures will be written, and `VK_INCOMPLETE` will be returned instead of `VK_SUCCESS`, to indicate that not all the available properties were returned.

#### Query profile device extensions

In order to query the list of device extensions required by a profile, use the following command:

```C++
VkResult vpGetProfileDeviceExtensionProperties(
    const VpProfileProperties*      pProfile,
    uint32_t*                       pPropertyCount,
    VkExtensionProperties*          pProperties);
```

Where:
* `pProfile` is a pointer to the `VpProfileProperties` structure specifying the queried profile.
* `pPropertyCount` is a pointer to an integer related to the number of device extensions available or queried, as described below.
* `pProperties` is either `NULL` or a pointer to an array of `VkExtensionProperties` structures.

If `pProperties` is `NULL`, then the number of device extensions required by the profile is returned in `pPropertyCount`. Otherwise, `pPropertyCount` must point to a variable set by the user to the number of elements in the `pProperties` array, and on return the variable is overwritten with the number of structures actually written to `pProperties`. If `pPropertyCount` is less than the number of device extensions required by the profile, at most `pPropertyCount` structures will be written, and `VK_INCOMPLETE` will be returned instead of `VK_SUCCESS`, to indicate that not all the available properties were returned.

#### Query profile features

In order to query the structure types of the Vulkan device feature structures for which the profile defines requirements, use the following command:

```C++
VkResult vpGetProfileFeatureStructureTypes(
    const VpProfileProperties*      pProfile,
    uint32_t*                       pStructureTypeCount,
    VkStructureType*                pStructureTypes);
```

Where:
* `pProfile` is a pointer to the `VpProfileProperties` structure specifying the queried profile.
* `pStructureTypeCount` is a pointer to an integer related to the number of device feature structure types available or queried, as described below.
* `pStructureTypes` is either `NULL` or a pointer to an array of `VkStructureType` values.

If `pStructureTypes` is `NULL`, then the number of device feature structure types defined by the profile is returned in `pStructureTypeCount`. Otherwise, `pStructureTypeCount` must point to a variable set by the user to the number of elements in the `pStructureTypes` array, and on return the variable is overwritten with the number of values actually written to `pStructureTypes`. If `pStructureTypeCount` is less than the number of device feature structure types defined by the profile, at most `pStructureTypeCount` values will be written, and `VK_INCOMPLETE` will be returned instead of `VK_SUCCESS`, to indicate that not all the available values were returned.

In order to query the device features required by a profile, use the following command:

```C++
void vpGetProfileFeatures(
    const VpProfileProperties*      pProfile,
    void*                           pNext);
```

Where:
* `pProfile` is a pointer to the `VpProfileProperties` structure specifying the queried profile.
* `pNext` is a `pNext` chain of Vulkan device feature structures (with or without a `VkPhysicalDeviceFeatures2` feature structure).

If the `pNext` chain contains a Vulkan device feature structure for which the profile defines corresponding feature requirements, then the structure is modified to include those features. Other fields of the Vulkan device feature structure not defined by the profile will be left unmodified, and structures not defined in the profile will be ignored (left unmodified as a whole).

#### Query profile device properties

In order to query the structure types of the Vulkan device property structures for which the profile defines requirements, use the following command:

```C++
VkResult vpGetProfilePropertyStructureTypes(
    const VpProfileProperties*      pProfile,
    uint32_t*                       pStructureTypeCount,
    VkStructureType*                pStructureTypes);
```

Where:
* `pProfile` is a pointer to the `VpProfileProperties` structure specifying the queried profile.
* `pStructureTypeCount` is a pointer to an integer related to the number of device property structure types available or queried, as described below.
* `pStructureTypes` is either `NULL` or a pointer to an array of `VkStructureType` enums.

If `pStructureTypes` is `NULL`, then the number of device property structure types defined by the profile is returned in `pStructureTypeCount`. Otherwise, `pStructureTypeCount` must point to a variable set by the user to the number of elements in the `pStructureTypes` array, and on return the variable is overwritten with the number of values actually written to `pStructureTypes`. If `pStructureTypeCount` is less than the number of device property structure types defined by the profile, at most `pStructureTypeCount` values will be written, and `VK_INCOMPLETE` will be returned instead of `VK_SUCCESS`, to indicate that not all the available values were returned.

In order to query the device properties required by a profile, use the following command:

```C++
void vpGetProfileProperties(
    const VpProfileProperties*      pProfile,
    void*                           pNext);
```

Where:
* `pProfile` is a pointer to the `VpProfileProperties` structure specifying the queried profile.
* `pNext` is a `pNext` chain of Vulkan device property structures (with or without a `VkPhysicalDeviceProperties2` property structure).

If the `pNext` chain contains a Vulkan device property structure for which the profile defines corresponding property/limit requirements then the structure is modified to include those properties/limits. Other fields of the Vulkan device property structure not defined by the profile will be left unmodified and structures not defined in the profile will be ignored (left unmodified as a whole).

#### Query profile queue family properties

In order to query the structure types of the Vulkan queue family property structures for which the profile defines requirements, use the following command:

```C++
VkResult vpGetProfileQueueFamilyStructureTypes(
    const VpProfileProperties*      pProfile,
    uint32_t*                       pStructureTypeCount,
    VkStructureType*                pStructureTypes);
```

Where:
* `pProfile` is a pointer to the `VpProfileProperties` structure specifying the queried profile.
* `pStructureTypeCount` is a pointer to an integer related to the number of queue family property structure types available or queried, as described below.
* `pStructureTypes` is either `NULL` or a pointer to an array of `VkStructureType` enums.

If `pStructureTypes` is `NULL`, then the number of queue family property structure types defined by the profile is returned in `pStructureTypeCount`. Otherwise, `pStructureTypeCount` must point to a variable set by the user to the number of elements in the `pStructureTypes` array, and on return the variable is overwritten with the number of values actually written to `pStructureTypes`. If `pStructureTypeCount` is less than the number of queue family property structure types defined by the profile, at most `pStructureTypeCount` values will be written, and `VK_INCOMPLETE` will be returned instead of `VK_SUCCESS`, to indicate that not all the available values were returned.

In order to query the queue family properties required by a profile, use the following command:

```C++
void vpGetProfileProperties(
    const VpProfileProperties*      pProfile,
    uint32_t*                       pPropertyCount,
    VkQueueFamilyProperties2KHR*    pProperties);
```

Where:
* `pProfile` is a pointer to the `VpProfileProperties` structure specifying the queried profile.
* `pPropertyCount` is a pointer to an integer related to the number of queue families available or queried, as described below.
* `pProperties` is either `NULL` or a pointer to an array of `VkQueueFamilyProperties2KHR` structures and corresponding `pNext` chains.

The API is slightly more complex in this case as profiles may have requirements defined for multiple queue families, hence multiple queue family property structure chains may be populated.

If `pProperties` is `NULL`, then the number of queue families defined by the profile is returned in `pPropertyCount`. Otherwise, `pPropertyCount` must point to a variable set by the user to the number of elements in the `pProperties` array, and on return the variable is overwritten with the number of structures actually written to `pProperties`. If `pPropertyCount` is less than the number of queue families defined by the profile, at most `pPropertyCount` structures will be written, and `VK_INCOMPLETE` will be returned instead of `VK_SUCCESS`, to indicate that not all the available properties were returned.

If `pProperties` or its `pNext` chain contains a Vulkan queue family property structure for which the profile defines corresponding queue family property requirements then the structure is modified to include those properties. Other fields of the Vulkan queue family property structure not defined by the profile will be left unmodified and structures not defined in the profile will be ignored (left unmodified as a whole).

#### Query profile format properties

In order to query the structure types of the Vulkan format property structures for which the profile defines requirements, use the following command:

```C++
VkResult vpGetProfileFormatStructureTypes(
    const VpProfileProperties*      pProfile,
    uint32_t*                       pStructureTypeCount,
    VkStructureType*                pStructureTypes);
```

Where:
* `pProfile` is a pointer to the `VpProfileProperties` structure specifying the queried profile.
* `pStructureTypeCount` is a pointer to an integer related to the number of format property structure types available or queried, as described below.
* `pStructureTypes` is either `NULL` or a pointer to an array of `VkStructureType` enums.

If `pStructureTypes` is `NULL`, then the number of format property structure types defined by the profile is returned in `pStructureTypeCount`. Otherwise, `pStructureTypeCount` must point to a variable set by the user to the number of elements in the `pStructureTypes` array, and on return the variable is overwritten with the number of values actually written to `pStructureTypes`. If `pStructureTypeCount` is less than the number of format property structure types defined by the profile, at most `pStructureTypeCount` values will be written, and `VK_INCOMPLETE` will be returned instead of `VK_SUCCESS`, to indicate that not all the available values were returned.

In order to query the list of formats required by a profile, use the following command:

```C++
VkResult vpGetProfileFormats(
    const VpProfileProperties*      pProfile,
    uint32_t*                       pFormatCount,
    VkFormat*                       pFormats);
```

Where:
* `pProfile` is a pointer to the `VpProfileProperties` structure specifying the queried profile.
* `pFormatCount` is a pointer to an integer related to the number of formats available or queried, as described below.
* `pFormats` is either `NULL` or a pointer to an array of `VkFormat` values.

If `pFormats` is `NULL`, then the number of formats required by the profile is returned in `pFormatCount`. Otherwise, `pFormatCount` must point to a variable set by the user to the number of elements in the `pFormats` array, and on return the variable is overwritten with the number of values actually written to `pProperties`. If `pPropertyCount` is less than the number of device extensions required by the profile, at most `pPropertyCount` values will be written, and `VK_INCOMPLETE` will be returned instead of `VK_SUCCESS`, to indicate that not all the available values were returned.

In order to query the format properties required by a profile for a specific format, use the following command:

```C++
void vpGetProfileFormatProperties(
    const VpProfileProperties*      pProfile,
    VkFormat                        format,
    void*                           pNext);
```

Where:
* `pProfile` is a pointer to the `VpProfileProperties` structure specifying the queried profile.
* `format` is the format for which required properties are queried.
* `pNext` is a `pNext` chain of Vulkan device property structures (with or without `VkPhysicalDeviceFormatProperties2` or `VkPhysicalDeviceFormatProperties3` property structures).

If the `pNext` chain contains a Vulkan format property structure for which the profile defines corresponding requirements then the structure is modified to include those properties. Other fields of the Vulkan format property structure not defined by the profile will be left unmodified and structures not defined in the profile will be ignored (left unmodified as a whole).
