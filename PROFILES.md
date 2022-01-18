<!-- markdownlint-disable MD041 -->
<p align="left"><img src="https://vulkan.lunarg.com/img/NewLunarGLogoBlack.png" alt="LunarG" width=263 height=113 /></p>
<p align="left">Copyright © 2021-2022 LunarG, Inc.</p>

<p align="center"><img src="./images/logo.png" width=400 /></p>

[![Creative Commons][3]][4]

[3]: https://i.creativecommons.org/l/by-nd/4.0/88x31.png "Creative Commons License"
[4]: https://creativecommons.org/licenses/by-nd/4.0/

# Vulkan Profiles definitions

## Vulkan Profiles List

| Profiles | VP_KHR_roadmap_2022 | VP_LUNARG_desktop_portability_2022 | VP_ANDROID_angle_es31 | VP_ANDROID_baseline_2022 |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| Description | This roadmap profile is intended to be supported by newer devices shipping in 2022 across mainstream smartphone, tablet, laptops, console and desktop devices. | Common desktop platforms with drivers supporting Vulkan 1.1 on Windows, Linux and macOS. | ANGLE Layer Support for GLES 3.1 | Collection on features supported in the majority of devices |
| Required API version | 1.3.XXX | 1.1.142  | 1.1.131 | 1.0.68 |

## Vulkan Profiles Extensions

| Profiles | VP_KHR_roadmap_2022 | VP_LUNARG_desktop_portability_2022 | VP_ANDROID_angle_es31 | VP_ANDROID_baseline_2022 |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| VK_KHR_8bit_storage | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_buffer_device_address | 1.2 Core | :x: | :x: | :x: |
| VK_KHR_create_renderpass2 | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_depth_stencil_resolve | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_draw_indirect_count | 1.2 Core | :x: | :x: | :x: |
| VK_KHR_driver_properties | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_image_format_list | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_imageless_framebuffer | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_sampler_mirror_clamp_to_edge | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_separate_depth_stencil_layouts | 1.2 Core | :x: | :x: | :x: |
| VK_KHR_shader_atomic_int64 | 1.2 Core | :x: | :x: | :x: |
| VK_KHR_shader_float16_int8 | 1.2 Core | :x: | :x: | :x: |
| VK_KHR_shader_float_controls | 1.2 Core | :x: | :x: | :x: |
| VK_KHR_shader_subgroup_extended_types | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_spirv_1_4 | 1.2 Core | :x: | :x: | :x: |
| VK_KHR_timeline_semaphore | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_uniform_buffer_standard_layout | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_vulkan_memory_model | 1.2 Core | :x: | :x: | :x: |
| VK_EXT_descriptor_indexing | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_EXT_host_query_reset | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_EXT_sampler_filter_minmax | 1.2 Core | :x: | :x: | :x: |
| VK_EXT_scalar_block_layout | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_EXT_separate_stencil_usage | 1.2 Core | :x: | :x: | :x: |
| VK_EXT_shader_viewport_index_layer | 1.2 Core | :x: | :x: | :x: |
| VK_KHR_swapchain | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_get_surface_capabilities2 | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_swapchain_mutable_format | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_sampler_ycbcr_conversion | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_EXT_inline_uniform_block | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_EXT_robustness2 | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_EXT_subgroup_size_control | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_EXT_texel_buffer_alignment | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_EXT_vertex_attribute_divisor | 1.2 Core | :heavy_check_mark: | :x: | :x: |
| VK_KHR_16bit_storage | 1.1 Core | 1.1 Core | 1.1 Core | :x: |
| VK_KHR_bind_memory2 | 1.1 Core | 1.1 Core | 1.1 Core | :x: |
| VK_KHR_dedicated_allocation | 1.1 Core | 1.1 Core | 1.1 Core | :x: |
| VK_KHR_descriptor_update_template | 1.1 Core | 1.1 Core | 1.1 Core | :x: |
| VK_KHR_device_group | 1.1 Core | 1.1 Core | 1.1 Core | :x: |
| VK_KHR_device_group_creation | 1.1 Core | 1.1 Core | 1.1 Core | :x: |
| VK_KHR_external_fence | 1.1 Core | 1.1 Core | 1.1 Core | :x: |
| VK_KHR_external_fence_capabilities | 1.1 Core | 1.1 Core | 1.1 Core | :x: |
| VK_KHR_external_memory | 1.1 Core | 1.1 Core | 1.1 Core | :x: |
| VK_KHR_surface | :x: | :x: | :x: | :heavy_check_mark: |
| VK_KHR_android_surface | :x: | :x: | :x: | :heavy_check_mark: |
| VK_KHR_swapchain | :x: | :x: | :x: | :heavy_check_mark: |
| VK_KHR_get_physical_device_properties2 | :x: | :x: | :x: | :heavy_check_mark: |
| VK_KHR_maintenance1 | :x: | :x: | :x: | :heavy_check_mark: |
| VK_EXT_swapchain_colorspace | :x: | :x: | :x: | :heavy_check_mark: |
| VK_KHR_get_surface_capabilities2 | :x: | :x: | :x: | :heavy_check_mark: |
| VK_KHR_incremental_present | :x: | :x: | :x: | :heavy_check_mark: |
| VK_GOOGLE_display_timing | :x: | :x: | :x: | :heavy_check_mark: |
| VK_KHR_shader_draw_parameters | :x: | :x: | :x: | :heavy_check_mark: |
| VK_KHR_descriptor_update_template | :x: | :x: | :x: | :heavy_check_mark: |
| VK_KHR_get_memory_requirements | :x: | :x: | :x: | :heavy_check_mark: |
| VK_KHR_dedicated_allocation | :x: | :x: | :x: | :heavy_check_mark: |
| VK_KHR_storage_buffer_storage_class | :x: | :x: | :x: | :heavy_check_mark: |
| VK_KHR_external_semaphore_capabilities | :x: | :x: | :x: | :heavy_check_mark: |
| VK_KHR_external_semaphore | :x: | :x: | :x: | :heavy_check_mark: |
| VK_KHR_external_memory_capbilities | :x: | :x: | :x: | :heavy_check_mark: |
| VK_KHR_external_memory | :x: | :x: | :x: | :heavy_check_mark: |
| VK_KHR_external_fence_capabilities | :x: | :x: | :x: | :heavy_check_mark: |
| VK_KHR_external_semaphore_fd | :x: | :x: | :x: | :heavy_check_mark: |
| VK_KHR_external_fence | :x: | :x: | :x: | :heavy_check_mark: |
| VK_KHR_external_fence_fd | :x: | :x: | :x: | :heavy_check_mark: |
| VK_KHR_variable_pointers | :x: | :x: | :x: | :heavy_check_mark: |
| VK_KHR_maintenance2 | :x: | :x: | :x: | :heavy_check_mark: |

## Vulkan Profiles Features

| Profiles | VP_KHR_roadmap_2022 (VkPhysicalDeviceFeatures) | VP_LUNARG_desktop_portability_2022 (VkPhysicalDeviceFeatures) | VP_ANDROID_angle_es31 (VkPhysicalDeviceFeatures) | VP_ANDROID_baseline_2022 (VkPhysicalDeviceFeatures) |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| depthBiasClamp               | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| depthClamp                   | :x:  | :heavy_check_mark: | :x: | :x: |
| drawIndirectFirstInstance    | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| dualSrcBlend                 | :x:  | :heavy_check_mark: | :x: | :x: |
| fillModeNonSolid             | :x:  | :heavy_check_mark: | :x: | :x: |
| fragmentStoresAndAtomics     | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| fullDrawIndexUint32          | :x:  | :heavy_check_mark: | :x: | :x: |
| imageCubeArray               | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| independentBlend             | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :x: |
| inheritedQueries             | :x:  | :heavy_check_mark: | :x: | :x: |
| largePoints                  | :x:  | :heavy_check_mark: | :x: | :x: |
| multiDrawIndirect            | :x:  | :heavy_check_mark: | :x: | :x: |
| multiViewport                | :x:  | :heavy_check_mark: | :x: | :x: |
| occlusionQueryPrecise        | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| robustBufferAccess           | 1.3 Core | :heavy_check_mark: | :heavy_check_mark: | :x: |
| sampleRateShading            | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| samplerAnisotropy            | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| shaderClipDistance           | :x:  | :heavy_check_mark: | :x: | :x: |
| shaderImageGatherExtended    | :x:  | :heavy_check_mark: | :x: | :x: |
| shaderSampledImageArrayDynamicIndexing   | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| shaderStorageBufferArrayDynamicIndexing  | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| shaderStorageImageArrayDynamicIndexing   | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| shaderStorageImageExtendedFormats        | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| shaderStorageImageReadWithoutFormat      | :heavy_check_mark: | :x: | :x: | :x: |
| shaderStorageImageWriteWithoutFormat     | :x:  | :heavy_check_mark: | :x: | :x: |
| shaderTessellationAndGeometryPointSize   | :x:  | :heavy_check_mark: | :x: | :x: |
| shaderUniformBufferArrayDynamicIndexing  | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| tessellationShader                       | :x:  | :heavy_check_mark: | :x: | :x: |
| textureCompressionBC                     | :x:  | :heavy_check_mark: | :x: | :x: |
| vertexPipelineStoresAndAtomics           | :x:  | :heavy_check_mark: | :heavy_check_mark: | :x: |

| Profiles | VP_KHR_roadmap_2022 (VkPhysicalDeviceVulkan12Features) | VP_LUNARG_desktop_portability_2022 (VkPhysicalDeviceImagelessFramebufferFeatures) | VP_ANDROID_angle_es31 | VP_ANDROID_baseline_2022 |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| imagelessFramebuffer | 1.2 Core | :heavy_check_mark: | :x: | :x: |

| Profiles | VP_KHR_roadmap_2022 (VkPhysicalDeviceVulkan12Features) | VP_LUNARG_desktop_portability_2022 (VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures) | VP_ANDROID_angle_es31 | VP_ANDROID_baseline_2022 |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| separateDepthStencilLayouts | 1.2 Core | :x: | :x: | :x: |

| Profiles | VP_KHR_roadmap_2022 (VkPhysicalDeviceVulkan12Features) | VP_LUNARG_desktop_portability_2022 (VkPhysicalDevice16BitStorageFeatures) | VP_ANDROID_angle_es31 | VP_ANDROID_baseline_2022 |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| storageBuffer16BitAccess | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| uniformAndStorageBuffer16BitAccess | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |

| Profiles | VP_KHR_roadmap_2022 (VkPhysicalDeviceVulkan11Features) | VP_LUNARG_desktop_portability_2022 (VkPhysicalDeviceMultiviewFeatures) | VP_ANDROID_angle_es31 | VP_ANDROID_baseline_2022 |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| multiview | 1.1 Core | 1.1 Core | 1.1 Core | :x: |

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

| Profiles | VP_KHR_roadmap_2022 (VkPhysicalDeviceLimits) | VP_LUNARG_desktop_portability_2022 (VkPhysicalDeviceLimits) | VP_ANDROID_angle_es31 (VkPhysicalDeviceLimits) | VP_ANDROID_baseline_2022 (VkPhysicalDeviceLimits) |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| maxImageDimension1D (min) | 16384 | 16384 | 8192 | 8192 |
| maxImageDimension2D (min) | 16384 | 16384 | 8192 | 8192 |
| maxImageDimension3D (min) | 2048 | 2048 | 2048 | 2048 |
| maxImageDimensionCube (min) | 16384 | 16384 | 8192 | 8192 |
| maxImageArrayLayers (min) | 2048 | 2048 | 2048 | 2048 |
| maxTexelBufferElements (min) | 67108900 | 67108900 | 67108900 | 67108900 |
| maxUniformBufferRange (min) | 65536 | 65536 | 65536 | 65536 |
| maxStorageBufferRange (min) | 134217728 | 134217728 | 134217728 | 134217728 |
| maxPushConstantsSize (min) | 128 | 128 | 128 | 128 |
| maxMemoryAllocationCount (min) | 4096 | 4096 | 4096 | 4096 |
| maxSamplerAllocationCount (min) | 1024 | 1024 | 1024 | 1024 |
| bufferImageGranularity (max) | 1024 | 1024 | 1024 | 1024 |
| sparseAddressSpaceSize (min) | 1073741824 | 1073741824 | 1073741824 | 1073741824 |
| maxBoundDescriptorSets (min) | 8 | 8 | 8 | 8 |
| maxPerStageDescriptorSamplers (min) | 16 | 16 | 16 | 16 |
| maxPerStageDescriptorUniformBuffers (min) | 15 | 15 | 15 | 15 |
| maxPerStageDescriptorStorageBuffers (min) | 16 | 16 | 16 | 16 |
| maxPerStageDescriptorSampledImages (min) | 128 | 128 | 128 | 128 |
| maxPerStageDescriptorStorageImages (min) | 8 | 8 | 8 | 8 |
| maxPerStageDescriptorInputAttachments (min) | 8 | 8 | 8 | 8 |
| maxPerStageResources (min) | 128 | 128 | 128 | 128 |
| maxDescriptorSetSamplers (min) | 80 | 80 | 80 | 80 |
| maxDescriptorSetUniformBuffers (min) | 90 | 90 | 90 | 90 |
| maxDescriptorSetUniformBuffersDynamic (min) | 8 | 8 | 8 | 8 |
| maxDescriptorSetStorageBuffers (min) | 155 | 155 | 155 | 155 
| maxDescriptorSetStorageBuffersDynamic (min) | 8 | 8 | 8 | 8 |
| maxDescriptorSetSampledImages (min) | 256 | 256 | 256 | 256 |
| maxDescriptorSetStorageImages (min) | 40 | 40 | 40 | 40 |
| maxDescriptorSetInputAttachments (min) | 8 | 8 | 8 | 8 |
| maxVertexInputAttributes (min) | 28 | 28 | 28 | 28 |
| maxVertexInputBindings (min) | 28 | 28 | 28 | 28 |
| maxVertexInputAttributeOffset (min) | 2047 | 2047 | 2047 | 2047 |
| maxVertexInputBindingStride (min) | 2048 | 2048 | 2048 | 2048 |
| maxVertexOutputComponents (min) | 124 | 124 | 124 | 124 |
| maxTessellationGenerationLevel (min) | 64 | 64 | 64 | 64 |
| maxTessellationPatchSize (min) | 32 | 32 | 32 | 32 |
| maxTessellationControlPerVertexInputComponents (min) | 124 | 124 | 124 | 124 |
| maxTessellationControlPerVertexOutputComponents (min) | 124 | 124 | 124 | 124 |
| maxTessellationControlPerPatchOutputComponents (min) | 120 | 120 | 120 | 120 |
| maxTessellationControlTotalOutputComponents (min) | 2048 | 2048 | 2048 | 2048 |
| maxTessellationEvaluationInputComponents (min) | 124 | 124 | 124 | 124 |
| maxTessellationEvaluationOutputComponents (min) | 124 | 124 | 124 | 124 |
| maxFragmentInputComponents (min) | 116 | 116 | 116 | 116 |
| maxFragmentOutputAttachments (min) | 8 | 8 | 8 | 8 |
| maxFragmentDualSrcAttachments (min) | 1 | 1 | 1 | 1 |
| maxFragmentCombinedOutputResources (min) | 8 | 8 | 8 | 8 |
| maxComputeSharedMemorySize (min) | 32768 | 32768 | 32768 | 32768 |
| maxComputeWorkGroupCount[0] (min) | 65535 | 65535 | 65535 | 65535 |
| maxComputeWorkGroupCount[1] (min) | 65535 | 65535 | 65535 | 65535 |
| maxComputeWorkGroupCount[2] (min) | 65535 | 65535 | 65535 | 65535 |
| maxComputeWorkGroupInvocations (min) | 1024 | 1024 | 1024 | 1024 |
| maxComputeWorkGroupSize[0] (min) | 1024 | 1024 | 1024 | 1024 |
| maxComputeWorkGroupSize[1] (min) | 1024 | 1024 | 1024 | 1024 |
| maxComputeWorkGroupSize[2] (min) | 64 | 64 | 64 | 64 |
| subPixelPrecisionBits (min) | 4 | 4 | 4 | 4 |
| subTexelPrecisionBits (min) | 4 | 4 | 4 | 4 |
| mipmapPrecisionBits (min) | 4 | 4 | 4 | 4 |
| maxDrawIndexedIndexValue (min) | 4294967295 | 4294967295 | 4294967295 | 4294967295 |
| maxDrawIndirectCount (min) | 1073740000 | 1073740000 | 1073740000 | 1073740000 |
| maxSamplerLodBias (min) | 14.0 | 14.0 | 14.0 | 14.0 |
| maxSamplerAnisotropy (min) | 16.0 | 16.0 | 16.0 | 16.0 |
| maxViewports (min) | 16 | 16 | 16 | 16 |
| maxViewportDimensions[0] (min) | 16384 | 16384 | 16384 | 16384 |
| maxViewportDimensions[1] (min) | 16384 | 16384 | 16384 | 16384 |
| viewportBoundsRange[0] (min) | -32768.0 | -32768.0 | -32768.0 | -32768.0 |
| viewportBoundsRange[1] (min) | 32767.0 | 32767.0 | 32767.0 | 32767.0 |
| minMemoryMapAlignment (min) | 64 | 64 | 64 | 64 |
| minTexelBufferOffsetAlignment (max) | 64 | 64 | 64 | 64 |
| minUniformBufferOffsetAlignment (max) | 256 | 256 | 256 | 256 |
| minStorageBufferOffsetAlignment (max) | 64 | 64 | 64 | 64 |
| minTexelOffset (max) | -8 | -8 | -8 | -8 |
| maxTexelOffset (min) | 7 | 7 | 7 | 7 |
| minTexelGatherOffset (max) | -8 | -8 | -8 | -8 |
| maxTexelGatherOffset (min) | 7 | 7 | 7 | 7 |
| minInterpolationOffset (max) | -0.5 | -0.5 | -0.5 | -0.5 |
| maxInterpolationOffset (min) | 0.4375 | 0.4375 | 0.4375 | 0.4375 |
| subPixelInterpolationOffsetBits (min) | 4 | 4 | 4 | 4 |
| maxFramebufferWidth (min) | 16384 | 16384 | 16384 | 16384 |
| maxFramebufferHeight (min) | 16384 | 16384 | 16384 | 16384 |
| maxFramebufferLayers (min) | 1024 | 1024 | 1024 | 1024 |
| framebufferColorSampleCounts (VkSampleCountFlags) | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT |
| framebufferDepthSampleCounts (VkSampleCountFlags) | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT |
| framebufferStencilSampleCounts (VkSampleCountFlags) | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT |
| maxColorAttachments (min) | 8 | 8 | 8 | 8 |
| sampledImageColorSampleCounts (VkSampleCountFlags) | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT |
| sampledImageIntegerSampleCounts (VkSampleCountFlags) | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT |
| sampledImageDepthSampleCounts (VkSampleCountFlags) | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT |
| sampledImageStencilSampleCounts (VkSampleCountFlags) | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_4_BIT |
| storageImageSampleCounts (VkSampleCountFlags) | VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_1_BIT |
| maxSampleMaskWords (max) | 1 | 1 | 1 | 1 |
| maxClipDistances (min) | 8 | 8 | 8 | 8 |
| maxCullDistances (min) | 8 | 8 | 8 | 8 |
| maxCombinedClipAndCullDistances (min) | 8 | 8 | 8 | 8 |
| discreteQueuePriorities (min) | 2 | 2 | 2 | 2 |
| pointSizeRange[0] (max) | 1.0 | 1.0 | 1.0 | 1.0 |
| pointSizeRange[1] (min) | 64.0 | 64.0 | 64.0 | 64.0 |
| lineWidthRange[0] (max) | 1.0 | 1.0 | 1.0 | 1.0 |
| lineWidthRange[1] (min) | 1.0 | 1.0 | 1.0 | 1.0 |
| pointSizeGranularity (max) | 0.125 | 0.125 | 0.125 | 0.125 |
| lineWidthGranularity (max) | 0.5 | 0.5 | 0.5 | 0.5 |

| Profiles | VP_KHR_roadmap_2022 (VkPhysicalDeviceVulkan12Properties) | VP_LUNARG_desktop_portability_2022 (VkPhysicalDeviceDescriptorIndexingProperties) | VP_ANDROID_angle_es31 (N/A) | VP_ANDROID_baseline_2022 (N/A) |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| maxUpdateAfterBindDescriptorsInAllPools (min) | 1048576 | 1048576 | 1048576 | 1048576 |
| maxPerStageDescriptorUpdateAfterBindSamplers (min) | 16 | 16 | 16 | 16 |
| maxPerStageDescriptorUpdateAfterBindUniformBuffers (min) | 15 | 15 | 15 | 15 |
| maxPerStageDescriptorUpdateAfterBindStorageBuffers (min) | 31 | 31 | 31 | 31 |
| maxPerStageDescriptorUpdateAfterBindSampledImages (min) | 128 | 128 | 128 | 128 |
| maxPerStageDescriptorUpdateAfterBindStorageImages (min) | 8 | 8 | 8 | 8 |
| maxPerStageDescriptorUpdateAfterBindInputAttachments (min) | 128 | 128 | 128 | 128 |
| maxPerStageUpdateAfterBindResources (min) | 159 | 159 | 159 | 159 |
| maxDescriptorSetUpdateAfterBindSamplers (min) | 80 | 80 | 80 | 80 |
| maxDescriptorSetUpdateAfterBindUniformBuffers (min) | 90 | 90 | 90 | 90 |
| maxDescriptorSetUpdateAfterBindUniformBuffersDynamic (min) | 8 | 8 | 8 | 8 |
| maxDescriptorSetUpdateAfterBindStorageBuffers (min) | 155 | 155 | 155 | 155 |
| maxDescriptorSetUpdateAfterBindStorageBuffersDynamic (min) | 8 | 8 | 8 | 8 |
| maxDescriptorSetUpdateAfterBindSampledImages (min) | 640 | 640 | 640 | 640 |
| maxDescriptorSetUpdateAfterBindStorageImages (min) | 40 | 40 | 40 | 40 |
| maxDescriptorSetUpdateAfterBindInputAttachments (min) | 640 | 640 | 640 | 640 |

| Profiles | VP_KHR_roadmap_2022 (VkPhysicalDeviceVulkan12Properties) | VP_LUNARG_desktop_portability_2022 (VkPhysicalDeviceMultiviewProperties) | VP_ANDROID_angle_es31 (N/A) | VP_ANDROID_baseline_2022 (N/A) |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| maxMultiviewViewCount (min) | 6 | 6 | 6 | 6 |
| maxMultiviewInstanceIndex (min) | 134217727 | 134217727 | 134217727 | 134217727 |

| Profiles | VP_KHR_roadmap_2022 (VkPhysicalDeviceVulkan12Properties) | VP_LUNARG_desktop_portability_2022 (VkPhysicalDeviceDepthStencilResolveProperties) | VP_ANDROID_angle_es31 (N/A) | VP_ANDROID_baseline_2022 (N/A) |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| independentResolve | true | true | true | true |
| independentResolveNone | true | true | true | true |
| supportedDepthResolveModes | VK_RESOLVE_MODE_SAMPLE_ZERO_BIT | VK_RESOLVE_MODE_SAMPLE_ZERO_BIT | VK_RESOLVE_MODE_SAMPLE_ZERO_BIT | VK_RESOLVE_MODE_SAMPLE_ZERO_BIT |
| supportedStencilResolveModes | VK_RESOLVE_MODE_SAMPLE_ZERO_BIT | VK_RESOLVE_MODE_SAMPLE_ZERO_BIT | VK_RESOLVE_MODE_SAMPLE_ZERO_BIT | VK_RESOLVE_MODE_SAMPLE_ZERO_BIT |

| Profiles | VP_KHR_roadmap_2022 (VkPhysicalDeviceVulkan12Properties) | VP_LUNARG_desktop_portability_2022 (VkPhysicalDeviceMaintenance3Properties) | VP_ANDROID_angle_es31 (N/A) | VP_ANDROID_baseline_2022 (N/A) |
| -------- | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| maxPerSetDescriptors (min) | 700 | 700 | 700 | 700 |
| maxMemoryAllocationSize (min) | 2147483648 | 2147483648 | 2147483648 | 2147483648 |

## Vulkan Profiles Formats

| VK_FORMAT_R8_UNORM | VP_KHR_roadmap_2022 (VkFormatProperties2) | VP_LUNARG_desktop_portability_2022 (VkFormatProperties2) | VP_ANDROID_angle_es31 (VkFormatProperties2) | VP_ANDROID_baseline_2022 (VkFormatProperties2) |
| ------------------ | ------------------- | ---------------------------------- | --------------------- | ------------------------ |
| linearTilingFeatures | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT, VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
| optimalTilingFeatures | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT, VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
| bufferFeatures | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT, VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |

