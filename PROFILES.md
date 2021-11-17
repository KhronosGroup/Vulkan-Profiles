<!-- markdownlint-disable MD041 -->
<p align="left"><img src="https://vulkan.lunarg.com/img/NewLunarGLogoBlack.png" alt="LunarG" width=263 height=113 /></p>
<p align="left">Copyright © 2021-2022 LunarG, Inc.</p>

<p align="center"><img src="./images/logo.png" width=400 /></p>

[![Creative Commons][3]][4]

[3]: https://i.creativecommons.org/l/by-nd/4.0/88x31.png "Creative Commons License"
[4]: https://creativecommons.org/licenses/by-nd/4.0/

# Vulkan Profiles definitions

## Vulkan Profiles API Version

| Profiles | VP_KHR_roadmap_2022 | VP_LUNARG_desktop_portability_2022 | VP_ANDROID_angle_es31 | VP_ANDROID_baseline_2022 |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| API version | 1.3 | 1.1  | 1.1 | 1.0 |

## Vulkan Profiles Extensions

| Profiles | VP_KHR_roadmap_2022 | VP_LUNARG_desktop_portability_2022 | VP_ANDROID_angle_es31 | VP_ANDROID_baseline_2022 |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| VK_KHR_8bit_storage | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_create_renderpass2 | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_depth_stencil_resolve | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_driver_properties | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_image_format_list | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_imageless_framebuffer | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_sampler_mirror_clamp_to_edge | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_shader_subgroup_extended_types | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_timeline_semaphore | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_uniform_buffer_standard_layout | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_EXT_descriptor_indexing | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_EXT_host_query_reset | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_EXT_scalar_block_layout | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_swapchain | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_get_surface_capabilities2 | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_swapchain_mutable_format | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_sampler_ycbcr_conversion | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_EXT_inline_uniform_block | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_EXT_robustness2 | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_EXT_subgroup_size_control | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_EXT_texel_buffer_alignment | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_EXT_vertex_attribute_divisor | 1.2 Core | :heavy_check_mark: | :x: | :x: |

## Vulkan Profiles Features

| Profiles | VP_KHR_roadmap_2022 (VkPhysicalDeviceFeatures) | VP_LUNARG_desktop_portability_2022 (VkPhysicalDeviceFeatures) | VP_ANDROID_angle_es31 (VkPhysicalDeviceFeatures) | VP_ANDROID_baseline_2022 (VkPhysicalDeviceFeatures) |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| depthBiasClamp | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| depthClamp | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| drawIndirectFirstInstance | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| dualSrcBlend | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| fillModeNonSolid | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| fragmentStoresAndAtomics | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| fullDrawIndexUint32 | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| imageCubeArray | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| independentBlend | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| inheritedQueries | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| largePoints | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| multiDrawIndirect | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| multiViewport | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| occlusionQueryPrecise | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| robustBufferAccess | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| sampleRateShading | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| samplerAnisotropy | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| shaderClipDistance | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| shaderImageGatherExtended | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| shaderSampledImageArrayDynamicIndexing | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| shaderStorageBufferArrayDynamicIndexing | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| shaderStorageImageArrayDynamicIndexing | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| shaderStorageImageExtendedFormats | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| shaderStorageImageWriteWithoutFormat | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| shaderTessellationAndGeometryPointSize | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| shaderUniformBufferArrayDynamicIndexing | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| tessellationShader | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| textureCompressionBC | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| vertexPipelineStoresAndAtomics | 1.2 Core | :heavy_check_mark: | :x: | :x: |

| Profiles | VP_KHR_roadmap_2022 (VkPhysicalDeviceVulkan12Features) | VP_LUNARG_desktop_portability_2022 (VkPhysicalDeviceImagelessFramebufferFeatures) | VP_ANDROID_angle_es31 | VP_ANDROID_baseline_2022 |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| imagelessFramebuffer | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |

| Profiles | VP_KHR_roadmap_2022 (VkPhysicalDeviceVulkan12Features) | VP_LUNARG_desktop_portability_2022 (VkPhysicalDevice16BitStorageFeatures) | VP_ANDROID_angle_es31 | VP_ANDROID_baseline_2022 |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| storageBuffer16BitAccess | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| uniformAndStorageBuffer16BitAccess | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |

| Profiles | VP_KHR_roadmap_2022 (VkPhysicalDeviceVulkan11Features) | VP_LUNARG_desktop_portability_2022 (VkPhysicalDeviceMultiviewFeatures) | VP_ANDROID_angle_es31 | VP_ANDROID_baseline_2022 |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| multiview | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |

| Profiles | VP_KHR_roadmap_2022 (VkPhysicalDeviceVulkan11Features) | VP_LUNARG_desktop_portability_2022 (VkPhysicalDevice8BitStorageFeatures) | VP_ANDROID_angle_es31 | VP_ANDROID_baseline_2022 |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| multiview | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |

| Profiles | VP_KHR_roadmap_2022 (VkPhysicalDeviceVulkan11Features) | VP_LUNARG_desktop_portability_2022 (VkPhysicalDeviceDescriptorIndexingFeatures) | VP_ANDROID_angle_es31 | VP_ANDROID_baseline_2022 |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| shaderUniformTexelBufferArrayDynamicIndexing | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| shaderStorageTexelBufferArrayDynamicIndexing | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| shaderSampledImageArrayNonUniformIndexing | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| shaderStorageBufferArrayNonUniformIndexing | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| shaderUniformTexelBufferArrayNonUniformIndexing | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| descriptorBindingSampledImageUpdateAfterBind | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| descriptorBindingStorageImageUpdateAfterBind | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| descriptorBindingStorageBufferUpdateAfterBind | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| descriptorBindingUniformTexelBufferUpdateAfterBind | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| descriptorBindingStorageTexelBufferUpdateAfterBind | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| descriptorBindingUpdateUnusedWhilePending | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| descriptorBindingPartiallyBound | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| descriptorBindingVariableDescriptorCount | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| runtimeDescriptorArray | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |

| Profiles | VP_KHR_roadmap_2022 (VkPhysicalDeviceVulkan11Features) | VP_LUNARG_desktop_portability_2022 (VkPhysicalDeviceHostQueryResetFeatures) | VP_ANDROID_angle_es31 | VP_ANDROID_baseline_2022 |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| hostQueryReset | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |

| Profiles | VP_KHR_roadmap_2022 (VkPhysicalDeviceVulkan11Features) | VP_LUNARG_desktop_portability_2022 (VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures) | VP_ANDROID_angle_es31 | VP_ANDROID_baseline_2022 |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| shaderSubgroupExtendedTypes | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |

| Profiles | VP_KHR_roadmap_2022 (VkPhysicalDeviceVulkan11Features) | VP_LUNARG_desktop_portability_2022 (VkPhysicalDeviceUniformBufferStandardLayoutFeatures) | VP_ANDROID_angle_es31 | VP_ANDROID_baseline_2022 |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| uniformBufferStandardLayout | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |

| Profiles | VP_KHR_roadmap_2022 (VkPhysicalDeviceVulkan11Features) | VP_LUNARG_desktop_portability_2022 (VkPhysicalDeviceShaderDrawParametersFeatures) | VP_ANDROID_angle_es31 | VP_ANDROID_baseline_2022 |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| shaderDrawParameters | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |

| Profiles | VP_KHR_roadmap_2022 (VkPhysicalDeviceVulkan11Features) | VP_LUNARG_desktop_portability_2022 (VkPhysicalDevice8BitStorageFeatures) | VP_ANDROID_angle_es31 | VP_ANDROID_baseline_2022 |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| storageBuffer8BitAccess | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| storagePushConstant8 | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| uniformAndStorageBuffer8BitAccess | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |

| Profiles | VP_KHR_roadmap_2022 (VkPhysicalDeviceVulkan11Features) | VP_LUNARG_desktop_portability_2022 (VkPhysicalDeviceShaderFloat16Int8Features) | VP_ANDROID_angle_es31 | VP_ANDROID_baseline_2022 |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| shaderInt8 | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |

| Profiles | VP_KHR_roadmap_2022 (VkPhysicalDeviceVulkan11Features) | VP_LUNARG_desktop_portability_2022 (VkPhysicalDeviceSamplerYcbcrConversionFeatures) | VP_ANDROID_angle_es31 | VP_ANDROID_baseline_2022 |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| samplerYcbcrConversion | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |

| Profiles | VP_KHR_roadmap_2022 (VkPhysicalDeviceVulkan11Features) | VP_LUNARG_desktop_portability_2022 (VkPhysicalDeviceVariablePointersFeatures) | VP_ANDROID_angle_es31 | VP_ANDROID_baseline_2022 |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| variablePointersStorageBuffer | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| variablePointers | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |

## Vulkan Profiles Properties

## Vulkan Profiles Formats



