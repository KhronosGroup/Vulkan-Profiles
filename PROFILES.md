
<!-- markdownlint-disable MD041 -->
<p align="left"><img src="https://vulkan.lunarg.com/img/NewLunarGLogoBlack.png" alt="LunarG" width=263 height=113 /></p>
<p align="left">Copyright (c) 2021-2022 LunarG, Inc.</p>

<p align="center"><img src="./images/logo.png" width=400 /></p>

[![Creative Commons][3]][4]

[3]: https://i.creativecommons.org/l/by-nd/4.0/88x31.png "Creative Commons License"
[4]: https://creativecommons.org/licenses/by-nd/4.0/

# Vulkan Profiles Definitions

## Vulkan Profiles List

| Profiles | VP_KHR_roadmap_2022 | VP_ANDROID_baseline_2021 | VP_LUNARG_desktop_portability_2021 | VP_LUNARG_desktop_portability_2021_subset |
|----------|---------------------|--------------------------|------------------------------------|-------------------------------------------|
| Label | Khronos Vulkan Roadmap 2022 profile | Android Vulkan Baseline 2021 profile | LunarG Vulkan Desktop Portability 2021 profile | LunarG Vulkan Desktop Portability 2021 subset profile |
| Description | This roadmap profile is intended to be supported by newer devices shipping in 2022 across mainstream smartphone, tablet, laptops, console and desktop devices. | Collection of functionality that is broadly supported on Android | Common desktop platforms with drivers supporting Vulkan 1.1 on Windows and Linux. | Common desktop platforms with drivers supporting Vulkan 1.1 on macOS. |
| Version | 1 | 1 | 1 | 1 |
| Required API version | 1.3.203 | 1.0.68 | 1.1.142 | 1.1.142 |
| Fallback profiles | - | - | - | VP_LUNARG_desktop_portability_2021 |

## Vulkan Profiles Extensions

| Profiles | VP_KHR_roadmap_2022 | VP_ANDROID_baseline_2021 | VP_LUNARG_desktop_portability_2021 | VP_LUNARG_desktop_portability_2021_subset |
|----------|---------------------|--------------------------|------------------------------------|-------------------------------------------|
| **Instance extensions** |
| VK_KHR_android_surface | :x: | :heavy_check_mark: | :x: | :x: |
| VK_KHR_device_group_creation | 1.1 Core | :x: | 1.1 Core | 1.1 Core |
| VK_KHR_external_fence_capabilities | 1.1 Core | :heavy_check_mark: | 1.1 Core | 1.1 Core |
| VK_KHR_external_memory_capabilities | 1.1 Core | :heavy_check_mark: | 1.1 Core | 1.1 Core |
| VK_KHR_external_semaphore_capabilities | 1.1 Core | :heavy_check_mark: | 1.1 Core | 1.1 Core |
| VK_KHR_get_physical_device_properties2 | 1.1 Core | :heavy_check_mark: | 1.1 Core | 1.1 Core |
| VK_KHR_get_surface_capabilities2 | :x: | :heavy_check_mark: | :x: | :x: |
| VK_KHR_surface | :x: | :heavy_check_mark: | :x: | :x: |
| VK_EXT_swapchain_colorspace | :x: | :heavy_check_mark: | :x: | :x: |
| **Device extensions** |
| VK_KHR_16bit_storage | 1.1 Core | :x: | 1.1 Core | 1.1 Core |
| VK_KHR_8bit_storage | 1.2 Core | :x: | :heavy_check_mark: | :heavy_check_mark: |
| VK_KHR_bind_memory2 | 1.1 Core | :x: | 1.1 Core | 1.1 Core |
| VK_KHR_buffer_device_address | 1.2 Core | :x: | :x: | :x: |
| VK_KHR_copy_commands2 | 1.3 Core | :x: | :x: | :x: |
| VK_KHR_create_renderpass2 | 1.2 Core | :x: | :heavy_check_mark: | :heavy_check_mark: |
| VK_KHR_dedicated_allocation | 1.1 Core | :heavy_check_mark: | 1.1 Core | 1.1 Core |
| VK_KHR_depth_stencil_resolve | 1.2 Core | :x: | :heavy_check_mark: | :heavy_check_mark: |
| VK_KHR_descriptor_update_template | 1.1 Core | :heavy_check_mark: | 1.1 Core | 1.1 Core |
| VK_KHR_device_group | 1.1 Core | :x: | 1.1 Core | 1.1 Core |
| VK_KHR_draw_indirect_count | 1.2 Core | :x: | :x: | :x: |
| VK_KHR_driver_properties | 1.2 Core | :x: | :heavy_check_mark: | :heavy_check_mark: |
| VK_KHR_dynamic_rendering | 1.3 Core | :x: | :x: | :x: |
| VK_KHR_external_fence | 1.1 Core | :heavy_check_mark: | 1.1 Core | 1.1 Core |
| VK_KHR_external_fence_fd | :x: | :heavy_check_mark: | :x: | :x: |
| VK_KHR_external_memory | 1.1 Core | :heavy_check_mark: | 1.1 Core | 1.1 Core |
| VK_KHR_external_semaphore | 1.1 Core | :heavy_check_mark: | 1.1 Core | 1.1 Core |
| VK_KHR_external_semaphore_fd | :x: | :heavy_check_mark: | :x: | :x: |
| VK_KHR_format_feature_flags2 | 1.3 Core | :x: | :x: | :x: |
| VK_KHR_get_memory_requirements2 | 1.1 Core | :heavy_check_mark: | 1.1 Core | 1.1 Core |
| VK_KHR_global_priority | :heavy_check_mark: | :x: | :x: | :x: |
| VK_KHR_image_format_list | 1.2 Core | :x: | :heavy_check_mark: | :heavy_check_mark: |
| VK_KHR_imageless_framebuffer | 1.2 Core | :x: | :heavy_check_mark: | :heavy_check_mark: |
| VK_KHR_incremental_present | :x: | :heavy_check_mark: | :x: | :x: |
| VK_KHR_maintenance1 | 1.1 Core | :heavy_check_mark: | 1.1 Core | 1.1 Core |
| VK_KHR_maintenance2 | 1.1 Core | :x: | 1.1 Core | 1.1 Core |
| VK_KHR_maintenance3 | 1.1 Core | :x: | 1.1 Core | 1.1 Core |
| VK_KHR_maintenance4 | 1.3 Core | :x: | :x: | :x: |
| VK_KHR_multiview | 1.1 Core | :x: | 1.1 Core | 1.1 Core |
| VK_KHR_portability_subset | :x: | :x: | :x: | :heavy_check_mark: |
| VK_KHR_relaxed_block_layout | 1.1 Core | :x: | 1.1 Core | 1.1 Core |
| VK_KHR_sampler_mirror_clamp_to_edge | 1.2 Core | :x: | :heavy_check_mark: | :heavy_check_mark: |
| VK_KHR_sampler_ycbcr_conversion | 1.1 Core | :x: | 1.1 Core | 1.1 Core |
| VK_KHR_separate_depth_stencil_layouts | 1.2 Core | :x: | :x: | :x: |
| VK_KHR_shader_atomic_int64 | 1.2 Core | :x: | :x: | :x: |
| VK_KHR_shader_draw_parameters | 1.1 Core | :x: | 1.1 Core | 1.1 Core |
| VK_KHR_shader_float16_int8 | 1.2 Core | :x: | :heavy_check_mark: | :heavy_check_mark: |
| VK_KHR_shader_float_controls | 1.2 Core | :x: | :x: | :x: |
| VK_KHR_shader_integer_dot_product | 1.3 Core | :x: | :x: | :x: |
| VK_KHR_shader_non_semantic_info | 1.3 Core | :x: | :x: | :x: |
| VK_KHR_shader_subgroup_extended_types | 1.2 Core | :x: | :x: | :x: |
| VK_KHR_shader_terminate_invocation | 1.3 Core | :x: | :x: | :x: |
| VK_KHR_spirv_1_4 | 1.2 Core | :x: | :x: | :x: |
| VK_KHR_storage_buffer_storage_class | 1.1 Core | :heavy_check_mark: | 1.1 Core | 1.1 Core |
| VK_KHR_swapchain | :x: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| VK_KHR_swapchain_mutable_format | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| VK_KHR_synchronization2 | 1.3 Core | :x: | :x: | :x: |
| VK_KHR_timeline_semaphore | 1.2 Core | :x: | :heavy_check_mark: | :heavy_check_mark: |
| VK_KHR_uniform_buffer_standard_layout | 1.2 Core | :x: | :heavy_check_mark: | :heavy_check_mark: |
| VK_KHR_variable_pointers | 1.1 Core | :heavy_check_mark: | 1.1 Core | 1.1 Core |
| VK_KHR_vulkan_memory_model | 1.2 Core | :x: | :x: | :x: |
| VK_KHR_zero_initialize_workgroup_memory | 1.3 Core | :x: | :x: | :x: |
| VK_EXT_4444_formats | 1.3 Core | :x: | :x: | :x: |
| VK_EXT_descriptor_indexing | 1.2 Core | :x: | :heavy_check_mark: | :heavy_check_mark: |
| VK_EXT_extended_dynamic_state | 1.3 Core | :x: | :x: | :x: |
| VK_EXT_extended_dynamic_state2 | 1.3 Core | :x: | :x: | :x: |
| VK_EXT_host_query_reset | 1.2 Core | :x: | :heavy_check_mark: | :heavy_check_mark: |
| VK_EXT_image_robustness | 1.3 Core | :x: | :x: | :x: |
| VK_EXT_inline_uniform_block | 1.3 Core | :x: | :x: | :x: |
| VK_EXT_pipeline_creation_cache_control | 1.3 Core | :x: | :x: | :x: |
| VK_EXT_pipeline_creation_feedback | 1.3 Core | :x: | :x: | :x: |
| VK_EXT_private_data | 1.3 Core | :x: | :x: | :x: |
| VK_EXT_robustness2 | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| VK_EXT_sampler_filter_minmax | 1.2 Core | :x: | :x: | :x: |
| VK_EXT_scalar_block_layout | 1.2 Core | :x: | :heavy_check_mark: | :heavy_check_mark: |
| VK_EXT_separate_stencil_usage | 1.2 Core | :x: | :x: | :x: |
| VK_EXT_shader_demote_to_helper_invocation | 1.3 Core | :x: | :x: | :x: |
| VK_EXT_shader_viewport_index_layer | 1.2 Core | :x: | :x: | :x: |
| VK_EXT_subgroup_size_control | 1.3 Core | :x: | :heavy_check_mark: | :heavy_check_mark: |
| VK_EXT_texel_buffer_alignment | 1.3 Core | :x: | :heavy_check_mark: | :heavy_check_mark: |
| VK_EXT_texture_compression_astc_hdr | 1.3 Core | :x: | :x: | :x: |
| VK_EXT_tooling_info | 1.3 Core | :x: | :x: | :x: |
| VK_EXT_vertex_attribute_divisor | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| VK_EXT_ycbcr_2plane_444_formats | 1.3 Core | :x: | :x: | :x: |
| VK_AMD_draw_indirect_count | 1.2 Core | :x: | :x: | :x: |
| VK_GOOGLE_display_timing | :x: | :heavy_check_mark: | :x: | :x: |

## Vulkan Profile Features

> **NOTE**: The table below only contains features explicitly defined by the corresponding profile. Further features may be supported by the profiles in accordance to the requirements defined in the "Feature Requirements" section of the appropriate version of the Vulkan API Specification.

| Profiles | VP_KHR_roadmap_2022 | VP_ANDROID_baseline_2021 | VP_LUNARG_desktop_portability_2021 | VP_LUNARG_desktop_portability_2021_subset |
|----------|---------------------|--------------------------|------------------------------------|-------------------------------------------|
| **Vulkan 1.0** |
| depthBiasClamp | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| depthClamp | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :x: | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| drawIndirectFirstInstance | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :x: | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| dualSrcBlend | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| fillModeNonSolid | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| fragmentStoresAndAtomics | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| fullDrawIndexUint32 | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| imageCubeArray | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| independentBlend | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| inheritedQueries | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| largePoints | :x: | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| multiDrawIndirect | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| multiViewport | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| occlusionQueryPrecise | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :x: | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| robustBufferAccess | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| sampleRateShading | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| samplerAnisotropy | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :x: | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| shaderClipDistance | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| shaderImageGatherExtended | :x: | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| shaderSampledImageArrayDynamicIndexing | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| shaderStorageBufferArrayDynamicIndexing | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :x: | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| shaderStorageImageArrayDynamicIndexing | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| shaderStorageImageExtendedFormats | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :x: | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| shaderStorageImageWriteWithoutFormat | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| shaderTessellationAndGeometryPointSize | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| shaderUniformBufferArrayDynamicIndexing | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| tessellationShader | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| textureCompressionASTC_LDR | :x: | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :x: | :x: |
| textureCompressionBC | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| textureCompressionETC2 | :x: | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :x: | :x: |
| vertexPipelineStoresAndAtomics | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceFeatures) | :heavy_check_mark: (in VkPhysicalDeviceFeatures) |
| **Vulkan 1.1** |
| multiview | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceMultiviewFeatures) | :heavy_check_mark: (in VkPhysicalDeviceMultiviewFeatures) |
| samplerYcbcrConversion | :heavy_check_mark: (in VkPhysicalDeviceVulkan11Features) | :x: | :x: | :x: |
| shaderDrawParameters | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceShaderDrawParametersFeatures) | :heavy_check_mark: (in VkPhysicalDeviceShaderDrawParametersFeatures) |
| storageBuffer16BitAccess | :x: | :x: | :heavy_check_mark: (in VkPhysicalDevice16BitStorageFeatures) | :heavy_check_mark: (in VkPhysicalDevice16BitStorageFeatures) |
| uniformAndStorageBuffer16BitAccess | :x: | :x: | :heavy_check_mark: (in VkPhysicalDevice16BitStorageFeatures) | :heavy_check_mark: (in VkPhysicalDevice16BitStorageFeatures) |
| variablePointers | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceVariablePointersFeatures) | :heavy_check_mark: (in VkPhysicalDeviceVariablePointersFeatures) |
| variablePointersStorageBuffer | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceVariablePointersFeatures) | :heavy_check_mark: (in VkPhysicalDeviceVariablePointersFeatures) |
| **Vulkan 1.2** |
| bufferDeviceAddress | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| descriptorBindingPartiallyBound | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) |
| descriptorBindingSampledImageUpdateAfterBind | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) |
| descriptorBindingStorageBufferUpdateAfterBind | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) |
| descriptorBindingStorageImageUpdateAfterBind | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) |
| descriptorBindingStorageTexelBufferUpdateAfterBind | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) |
| descriptorBindingUniformBufferUpdateAfterBind | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| descriptorBindingUniformTexelBufferUpdateAfterBind | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) |
| descriptorBindingUpdateUnusedWhilePending | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) |
| descriptorBindingVariableDescriptorCount | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) |
| descriptorIndexing | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| hostQueryReset | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceHostQueryResetFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceHostQueryResetFeaturesEXT) |
| imagelessFramebuffer | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceImagelessFramebufferFeaturesKHR) | :heavy_check_mark: (in VkPhysicalDeviceImagelessFramebufferFeaturesKHR) |
| runtimeDescriptorArray | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) |
| samplerMirrorClampToEdge | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| scalarBlockLayout | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| separateDepthStencilLayouts | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| shaderInt8 | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceShaderFloat16Int8FeaturesKHR) | :heavy_check_mark: (in VkPhysicalDeviceShaderFloat16Int8FeaturesKHR) |
| shaderSampledImageArrayNonUniformIndexing | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) |
| shaderStorageBufferArrayNonUniformIndexing | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| shaderStorageImageArrayNonUniformIndexing | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| shaderStorageTexelBufferArrayDynamicIndexing | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) |
| shaderStorageTexelBufferArrayNonUniformIndexing | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| shaderSubgroupExtendedTypes | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| shaderUniformBufferArrayNonUniformIndexing | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| shaderUniformTexelBufferArrayDynamicIndexing | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) |
| shaderUniformTexelBufferArrayNonUniformIndexing | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) |
| storageBuffer8BitAccess | :x: | :x: | :heavy_check_mark: (in VkPhysicalDevice8BitStorageFeaturesKHR) | :heavy_check_mark: (in VkPhysicalDevice8BitStorageFeaturesKHR) |
| subgroupBroadcastDynamicId | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| timelineSemaphore | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| uniformAndStorageBuffer8BitAccess | :x: | :x: | :heavy_check_mark: (in VkPhysicalDevice8BitStorageFeaturesKHR) | :heavy_check_mark: (in VkPhysicalDevice8BitStorageFeaturesKHR) |
| uniformBufferStandardLayout | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR) | :heavy_check_mark: (in VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR) |
| vulkanMemoryModel | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| vulkanMemoryModelAvailabilityVisibilityChains | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| vulkanMemoryModelDeviceScope | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| **Vulkan 1.3** |
| computeFullSubgroups | :heavy_check_mark: (in VkPhysicalDeviceVulkan13Features) | :x: | :x: | :x: |
| descriptorBindingInlineUniformBlockUpdateAfterBind | :heavy_check_mark: (in VkPhysicalDeviceVulkan13Features) | :x: | :x: | :x: |
| inlineUniformBlock | :heavy_check_mark: (in VkPhysicalDeviceVulkan13Features) | :x: | :x: | :x: |
| maintenance4 | :heavy_check_mark: (in VkPhysicalDeviceVulkan13Features) | :x: | :x: | :x: |
| pipelineCreationCacheControl | :heavy_check_mark: (in VkPhysicalDeviceVulkan13Features) | :x: | :x: | :x: |
| robustImageAccess | :heavy_check_mark: (in VkPhysicalDeviceVulkan13Features) | :x: | :x: | :x: |
| shaderDemoteToHelperInvocation | :heavy_check_mark: (in VkPhysicalDeviceVulkan13Features) | :x: | :x: | :x: |
| shaderIntegerDotProduct | :heavy_check_mark: (in VkPhysicalDeviceVulkan13Features) | :x: | :x: | :x: |
| shaderTerminateInvocation | :heavy_check_mark: (in VkPhysicalDeviceVulkan13Features) | :x: | :x: | :x: |
| shaderZeroInitializeWorkgroupMemory | :heavy_check_mark: (in VkPhysicalDeviceVulkan13Features) | :x: | :x: | :x: |
| subgroupSizeControl | :heavy_check_mark: (in VkPhysicalDeviceVulkan13Features) | :x: | :x: | :x: |
| synchronization2 | :heavy_check_mark: (in VkPhysicalDeviceVulkan13Features) | :x: | :x: | :x: |
| **VK_KHR_16bit_storage** |
| storageBuffer16BitAccess | :x: | :x: | :heavy_check_mark: (in VkPhysicalDevice16BitStorageFeatures) | :heavy_check_mark: (in VkPhysicalDevice16BitStorageFeatures) |
| uniformAndStorageBuffer16BitAccess | :x: | :x: | :heavy_check_mark: (in VkPhysicalDevice16BitStorageFeatures) | :heavy_check_mark: (in VkPhysicalDevice16BitStorageFeatures) |
| **VK_KHR_8bit_storage** |
| storageBuffer8BitAccess | :x: | :x: | :heavy_check_mark: (in VkPhysicalDevice8BitStorageFeaturesKHR) | :heavy_check_mark: (in VkPhysicalDevice8BitStorageFeaturesKHR) |
| uniformAndStorageBuffer8BitAccess | :x: | :x: | :heavy_check_mark: (in VkPhysicalDevice8BitStorageFeaturesKHR) | :heavy_check_mark: (in VkPhysicalDevice8BitStorageFeaturesKHR) |
| **VK_KHR_buffer_device_address** |
| bufferDeviceAddress | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| **VK_KHR_imageless_framebuffer** |
| imagelessFramebuffer | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceImagelessFramebufferFeaturesKHR) | :heavy_check_mark: (in VkPhysicalDeviceImagelessFramebufferFeaturesKHR) |
| **VK_KHR_maintenance4** |
| maintenance4 | :heavy_check_mark: (in VkPhysicalDeviceVulkan13Features) | :x: | :x: | :x: |
| **VK_KHR_multiview** |
| multiview | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceMultiviewFeatures) | :heavy_check_mark: (in VkPhysicalDeviceMultiviewFeatures) |
| **VK_KHR_portability_subset** |
| constantAlphaColorBlendFactors | :x: | :x: | :x: | :heavy_check_mark: (in VkPhysicalDevicePortabilitySubsetFeaturesKHR) |
| events | :x: | :x: | :x: | :heavy_check_mark: (in VkPhysicalDevicePortabilitySubsetFeaturesKHR) |
| imageViewFormatReinterpretation | :x: | :x: | :x: | :heavy_check_mark: (in VkPhysicalDevicePortabilitySubsetFeaturesKHR) |
| imageViewFormatSwizzle | :x: | :x: | :x: | :heavy_check_mark: (in VkPhysicalDevicePortabilitySubsetFeaturesKHR) |
| multisampleArrayImage | :x: | :x: | :x: | :heavy_check_mark: (in VkPhysicalDevicePortabilitySubsetFeaturesKHR) |
| mutableComparisonSamplers | :x: | :x: | :x: | :heavy_check_mark: (in VkPhysicalDevicePortabilitySubsetFeaturesKHR) |
| separateStencilMaskRef | :x: | :x: | :x: | :heavy_check_mark: (in VkPhysicalDevicePortabilitySubsetFeaturesKHR) |
| vertexAttributeAccessBeyondStride | :x: | :x: | :x: | :heavy_check_mark: (in VkPhysicalDevicePortabilitySubsetFeaturesKHR) |
| **VK_KHR_sampler_ycbcr_conversion** |
| samplerYcbcrConversion | :heavy_check_mark: (in VkPhysicalDeviceVulkan11Features) | :x: | :x: | :x: |
| **VK_KHR_separate_depth_stencil_layouts** |
| separateDepthStencilLayouts | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| **VK_KHR_shader_float16_int8** |
| shaderInt8 | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceShaderFloat16Int8FeaturesKHR) | :heavy_check_mark: (in VkPhysicalDeviceShaderFloat16Int8FeaturesKHR) |
| **VK_KHR_shader_integer_dot_product** |
| shaderIntegerDotProduct | :heavy_check_mark: (in VkPhysicalDeviceVulkan13Features) | :x: | :x: | :x: |
| **VK_KHR_shader_subgroup_extended_types** |
| shaderSubgroupExtendedTypes | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| **VK_KHR_shader_terminate_invocation** |
| shaderTerminateInvocation | :heavy_check_mark: (in VkPhysicalDeviceVulkan13Features) | :x: | :x: | :x: |
| **VK_KHR_synchronization2** |
| synchronization2 | :heavy_check_mark: (in VkPhysicalDeviceVulkan13Features) | :x: | :x: | :x: |
| **VK_KHR_timeline_semaphore** |
| timelineSemaphore | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| **VK_KHR_uniform_buffer_standard_layout** |
| uniformBufferStandardLayout | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR) | :heavy_check_mark: (in VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR) |
| **VK_KHR_variable_pointers** |
| variablePointers | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceVariablePointersFeatures) | :heavy_check_mark: (in VkPhysicalDeviceVariablePointersFeatures) |
| variablePointersStorageBuffer | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceVariablePointersFeatures) | :heavy_check_mark: (in VkPhysicalDeviceVariablePointersFeatures) |
| **VK_KHR_vulkan_memory_model** |
| vulkanMemoryModel | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| vulkanMemoryModelAvailabilityVisibilityChains | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| vulkanMemoryModelDeviceScope | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| **VK_KHR_zero_initialize_workgroup_memory** |
| shaderZeroInitializeWorkgroupMemory | :heavy_check_mark: (in VkPhysicalDeviceVulkan13Features) | :x: | :x: | :x: |
| **VK_EXT_descriptor_indexing** |
| descriptorBindingPartiallyBound | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) |
| descriptorBindingSampledImageUpdateAfterBind | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) |
| descriptorBindingStorageBufferUpdateAfterBind | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) |
| descriptorBindingStorageImageUpdateAfterBind | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) |
| descriptorBindingStorageTexelBufferUpdateAfterBind | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) |
| descriptorBindingUniformBufferUpdateAfterBind | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| descriptorBindingUniformTexelBufferUpdateAfterBind | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) |
| descriptorBindingUpdateUnusedWhilePending | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) |
| descriptorBindingVariableDescriptorCount | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) |
| runtimeDescriptorArray | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) |
| shaderSampledImageArrayNonUniformIndexing | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) |
| shaderStorageBufferArrayNonUniformIndexing | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| shaderStorageImageArrayNonUniformIndexing | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| shaderStorageTexelBufferArrayDynamicIndexing | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) |
| shaderStorageTexelBufferArrayNonUniformIndexing | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| shaderUniformBufferArrayNonUniformIndexing | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| shaderUniformTexelBufferArrayDynamicIndexing | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) |
| shaderUniformTexelBufferArrayNonUniformIndexing | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceDescriptorIndexingFeaturesEXT) |
| **VK_EXT_host_query_reset** |
| hostQueryReset | :x: | :x: | :heavy_check_mark: (in VkPhysicalDeviceHostQueryResetFeaturesEXT) | :heavy_check_mark: (in VkPhysicalDeviceHostQueryResetFeaturesEXT) |
| **VK_EXT_image_robustness** |
| robustImageAccess | :heavy_check_mark: (in VkPhysicalDeviceVulkan13Features) | :x: | :x: | :x: |
| **VK_EXT_inline_uniform_block** |
| descriptorBindingInlineUniformBlockUpdateAfterBind | :heavy_check_mark: (in VkPhysicalDeviceVulkan13Features) | :x: | :x: | :x: |
| inlineUniformBlock | :heavy_check_mark: (in VkPhysicalDeviceVulkan13Features) | :x: | :x: | :x: |
| **VK_EXT_pipeline_creation_cache_control** |
| pipelineCreationCacheControl | :heavy_check_mark: (in VkPhysicalDeviceVulkan13Features) | :x: | :x: | :x: |
| **VK_EXT_scalar_block_layout** |
| scalarBlockLayout | :heavy_check_mark: (in VkPhysicalDeviceVulkan12Features) | :x: | :x: | :x: |
| **VK_EXT_shader_demote_to_helper_invocation** |
| shaderDemoteToHelperInvocation | :heavy_check_mark: (in VkPhysicalDeviceVulkan13Features) | :x: | :x: | :x: |
| **VK_EXT_subgroup_size_control** |
| computeFullSubgroups | :heavy_check_mark: (in VkPhysicalDeviceVulkan13Features) | :x: | :x: | :x: |
| subgroupSizeControl | :heavy_check_mark: (in VkPhysicalDeviceVulkan13Features) | :x: | :x: | :x: |

## Vulkan Profile Limits (Properties)

> **NOTE**: The table below only contains properties/limits explicitly defined by the corresponding profile. Further properties/limits may be supported by the profiles in accordance to the requirements defined in the "Limit Requirements" section of the appropriate version of the Vulkan API Specification.

| Profiles | VP_KHR_roadmap_2022 | VP_ANDROID_baseline_2021 | VP_LUNARG_desktop_portability_2021 | VP_LUNARG_desktop_portability_2021_subset |
|----------|---------------------|--------------------------|------------------------------------|-------------------------------------------|
| **Vulkan 1.0** |
| bufferImageGranularity (min) | 4096 (in VkPhysicalDeviceProperties) | - | 1024 (in VkPhysicalDeviceProperties) | 1024 (in VkPhysicalDeviceProperties) |
| discreteQueuePriorities (max) | - | 2 (in VkPhysicalDeviceProperties) | 2 (in VkPhysicalDeviceProperties) | 2 (in VkPhysicalDeviceProperties) |
| framebufferColorSampleCounts (min) | - | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) (in VkPhysicalDeviceProperties) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) (in VkPhysicalDeviceProperties) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) (in VkPhysicalDeviceProperties) |
| framebufferDepthSampleCounts (min) | - | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) (in VkPhysicalDeviceProperties) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) (in VkPhysicalDeviceProperties) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) (in VkPhysicalDeviceProperties) |
| framebufferStencilSampleCounts (min) | - | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) (in VkPhysicalDeviceProperties) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) (in VkPhysicalDeviceProperties) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) (in VkPhysicalDeviceProperties) |
| lineWidthGranularity (min) | 0.5 (in VkPhysicalDeviceProperties) | 1 (in VkPhysicalDeviceProperties) | 0.5 (in VkPhysicalDeviceProperties) | 0.5 (in VkPhysicalDeviceProperties) |
| lineWidthRange (min,max) | - | (1.0,1.0) (in VkPhysicalDeviceProperties) | (1.0,1.0) (in VkPhysicalDeviceProperties) | (1.0,1.0) (in VkPhysicalDeviceProperties) |
| maxBoundDescriptorSets (max) | - | 4 (in VkPhysicalDeviceProperties) | 8 (in VkPhysicalDeviceProperties) | 8 (in VkPhysicalDeviceProperties) |
| maxClipDistances (max) | - | - | 8 (in VkPhysicalDeviceProperties) | 8 (in VkPhysicalDeviceProperties) |
| maxColorAttachments (max) | 7 (in VkPhysicalDeviceProperties) | 4 (in VkPhysicalDeviceProperties) | 8 (in VkPhysicalDeviceProperties) | 8 (in VkPhysicalDeviceProperties) |
| maxCombinedClipAndCullDistances (max) | - | - | 8 (in VkPhysicalDeviceProperties) | 8 (in VkPhysicalDeviceProperties) |
| maxComputeSharedMemorySize (max) | - | 16384 (in VkPhysicalDeviceProperties) | 32768 (in VkPhysicalDeviceProperties) | 32768 (in VkPhysicalDeviceProperties) |
| maxComputeWorkGroupCount (max) | - | (65535,65535,65535) (in VkPhysicalDeviceProperties) | (65535,65535,65535) (in VkPhysicalDeviceProperties) | (65535,65535,65535) (in VkPhysicalDeviceProperties) |
| maxComputeWorkGroupInvocations (max) | 256 (in VkPhysicalDeviceProperties) | 128 (in VkPhysicalDeviceProperties) | 1024 (in VkPhysicalDeviceProperties) | 1024 (in VkPhysicalDeviceProperties) |
| maxComputeWorkGroupSize (max) | (1024,1024,64) (in VkPhysicalDeviceProperties) | (128,128,64) (in VkPhysicalDeviceProperties) | (1024,1024,64) (in VkPhysicalDeviceProperties) | (1024,1024,64) (in VkPhysicalDeviceProperties) |
| maxDescriptorSetInputAttachments (max) | - | 4 (in VkPhysicalDeviceProperties) | 8 (in VkPhysicalDeviceProperties) | 8 (in VkPhysicalDeviceProperties) |
| maxDescriptorSetSampledImages (max) | 1800 (in VkPhysicalDeviceProperties) | 96 (in VkPhysicalDeviceProperties) | 256 (in VkPhysicalDeviceProperties) | 256 (in VkPhysicalDeviceProperties) |
| maxDescriptorSetSamplers (max) | 576 (in VkPhysicalDeviceProperties) | 96 (in VkPhysicalDeviceProperties) | 80 (in VkPhysicalDeviceProperties) | 80 (in VkPhysicalDeviceProperties) |
| maxDescriptorSetStorageBuffers (max) | 96 (in VkPhysicalDeviceProperties) | 24 (in VkPhysicalDeviceProperties) | 155 (in VkPhysicalDeviceProperties) | 155 (in VkPhysicalDeviceProperties) |
| maxDescriptorSetStorageBuffersDynamic (max) | - | 4 (in VkPhysicalDeviceProperties) | 8 (in VkPhysicalDeviceProperties) | 8 (in VkPhysicalDeviceProperties) |
| maxDescriptorSetStorageImages (max) | 144 (in VkPhysicalDeviceProperties) | 24 (in VkPhysicalDeviceProperties) | 40 (in VkPhysicalDeviceProperties) | 40 (in VkPhysicalDeviceProperties) |
| maxDescriptorSetUniformBuffers (max) | 90 (in VkPhysicalDeviceProperties) | 72 (in VkPhysicalDeviceProperties) | 90 (in VkPhysicalDeviceProperties) | 90 (in VkPhysicalDeviceProperties) |
| maxDescriptorSetUniformBuffersDynamic (max) | - | 8 (in VkPhysicalDeviceProperties) | 8 (in VkPhysicalDeviceProperties) | 8 (in VkPhysicalDeviceProperties) |
| maxDrawIndexedIndexValue (max) | - | 4294967295 (in VkPhysicalDeviceProperties) | 4294967295 (in VkPhysicalDeviceProperties) | 4294967295 (in VkPhysicalDeviceProperties) |
| maxDrawIndirectCount (max) | - | 1 (in VkPhysicalDeviceProperties) | 1073740000 (in VkPhysicalDeviceProperties) | 1073740000 (in VkPhysicalDeviceProperties) |
| maxFragmentCombinedOutputResources (max) | 16 (in VkPhysicalDeviceProperties) | 8 (in VkPhysicalDeviceProperties) | 8 (in VkPhysicalDeviceProperties) | 8 (in VkPhysicalDeviceProperties) |
| maxFragmentDualSrcAttachments (max) | - | - | 1 (in VkPhysicalDeviceProperties) | 1 (in VkPhysicalDeviceProperties) |
| maxFragmentInputComponents (max) | - | 64 (in VkPhysicalDeviceProperties) | 116 (in VkPhysicalDeviceProperties) | 116 (in VkPhysicalDeviceProperties) |
| maxFragmentOutputAttachments (max) | - | 4 (in VkPhysicalDeviceProperties) | 8 (in VkPhysicalDeviceProperties) | 8 (in VkPhysicalDeviceProperties) |
| maxFramebufferHeight (max) | - | 8192 (in VkPhysicalDeviceProperties) | 16384 (in VkPhysicalDeviceProperties) | 16384 (in VkPhysicalDeviceProperties) |
| maxFramebufferLayers (max) | - | 256 (in VkPhysicalDeviceProperties) | 1024 (in VkPhysicalDeviceProperties) | 1024 (in VkPhysicalDeviceProperties) |
| maxFramebufferWidth (max) | - | 8192 (in VkPhysicalDeviceProperties) | 16384 (in VkPhysicalDeviceProperties) | 16384 (in VkPhysicalDeviceProperties) |
| maxImageArrayLayers (max) | 2048 (in VkPhysicalDeviceProperties) | 256 (in VkPhysicalDeviceProperties) | 2048 (in VkPhysicalDeviceProperties) | 2048 (in VkPhysicalDeviceProperties) |
| maxImageDimension1D (max) | 8192 (in VkPhysicalDeviceProperties) | 8192 (in VkPhysicalDeviceProperties) | 16384 (in VkPhysicalDeviceProperties) | 16384 (in VkPhysicalDeviceProperties) |
| maxImageDimension2D (max) | 8192 (in VkPhysicalDeviceProperties) | 8192 (in VkPhysicalDeviceProperties) | 16384 (in VkPhysicalDeviceProperties) | 16384 (in VkPhysicalDeviceProperties) |
| maxImageDimension3D (max) | - | 2048 (in VkPhysicalDeviceProperties) | 2048 (in VkPhysicalDeviceProperties) | 2048 (in VkPhysicalDeviceProperties) |
| maxImageDimensionCube (max) | 8192 (in VkPhysicalDeviceProperties) | 8192 (in VkPhysicalDeviceProperties) | 16384 (in VkPhysicalDeviceProperties) | 16384 (in VkPhysicalDeviceProperties) |
| maxInterpolationOffset (max) | - | 0.4375 (in VkPhysicalDeviceProperties) | 0.4375 (in VkPhysicalDeviceProperties) | 0.4375 (in VkPhysicalDeviceProperties) |
| maxMemoryAllocationCount (max) | - | 4096 (in VkPhysicalDeviceProperties) | 4096 (in VkPhysicalDeviceProperties) | 4096 (in VkPhysicalDeviceProperties) |
| maxPerStageDescriptorInputAttachments (max) | - | 4 (in VkPhysicalDeviceProperties) | 8 (in VkPhysicalDeviceProperties) | 8 (in VkPhysicalDeviceProperties) |
| maxPerStageDescriptorSampledImages (max) | 200 (in VkPhysicalDeviceProperties) | 16 (in VkPhysicalDeviceProperties) | 128 (in VkPhysicalDeviceProperties) | 128 (in VkPhysicalDeviceProperties) |
| maxPerStageDescriptorSamplers (max) | 64 (in VkPhysicalDeviceProperties) | 16 (in VkPhysicalDeviceProperties) | 16 (in VkPhysicalDeviceProperties) | 16 (in VkPhysicalDeviceProperties) |
| maxPerStageDescriptorStorageBuffers (max) | 30 (in VkPhysicalDeviceProperties) | 4 (in VkPhysicalDeviceProperties) | 16 (in VkPhysicalDeviceProperties) | 16 (in VkPhysicalDeviceProperties) |
| maxPerStageDescriptorStorageImages (max) | 16 (in VkPhysicalDeviceProperties) | 4 (in VkPhysicalDeviceProperties) | 8 (in VkPhysicalDeviceProperties) | 8 (in VkPhysicalDeviceProperties) |
| maxPerStageDescriptorUniformBuffers (max) | 15 (in VkPhysicalDeviceProperties) | 12 (in VkPhysicalDeviceProperties) | 15 (in VkPhysicalDeviceProperties) | 15 (in VkPhysicalDeviceProperties) |
| maxPerStageResources (max) | 200 (in VkPhysicalDeviceProperties) | 44 (in VkPhysicalDeviceProperties) | 128 (in VkPhysicalDeviceProperties) | 128 (in VkPhysicalDeviceProperties) |
| maxPushConstantsSize (max) | - | 128 (in VkPhysicalDeviceProperties) | 128 (in VkPhysicalDeviceProperties) | 128 (in VkPhysicalDeviceProperties) |
| maxSampleMaskWords (max) | - | 1 (in VkPhysicalDeviceProperties) | 1 (in VkPhysicalDeviceProperties) | 1 (in VkPhysicalDeviceProperties) |
| maxSamplerAllocationCount (max) | - | 4000 (in VkPhysicalDeviceProperties) | 1024 (in VkPhysicalDeviceProperties) | 1024 (in VkPhysicalDeviceProperties) |
| maxSamplerAnisotropy (max) | - | 1.0 (in VkPhysicalDeviceProperties) | 16.0 (in VkPhysicalDeviceProperties) | 16.0 (in VkPhysicalDeviceProperties) |
| maxSamplerLodBias (max) | 14 (in VkPhysicalDeviceProperties) | 2.0 (in VkPhysicalDeviceProperties) | 4.0 (in VkPhysicalDeviceProperties) | 4.0 (in VkPhysicalDeviceProperties) |
| maxStorageBufferRange (max) | - | 134217728 (in VkPhysicalDeviceProperties) | 134217728 (in VkPhysicalDeviceProperties) | 134217728 (in VkPhysicalDeviceProperties) |
| maxTessellationControlPerPatchOutputComponents (max) | - | - | 120 (in VkPhysicalDeviceProperties) | 120 (in VkPhysicalDeviceProperties) |
| maxTessellationControlPerVertexInputComponents (max) | - | - | 124 (in VkPhysicalDeviceProperties) | 124 (in VkPhysicalDeviceProperties) |
| maxTessellationControlPerVertexOutputComponents (max) | - | - | 124 (in VkPhysicalDeviceProperties) | 124 (in VkPhysicalDeviceProperties) |
| maxTessellationControlTotalOutputComponents (max) | - | - | 2048 (in VkPhysicalDeviceProperties) | 2048 (in VkPhysicalDeviceProperties) |
| maxTessellationEvaluationInputComponents (max) | - | - | 124 (in VkPhysicalDeviceProperties) | 124 (in VkPhysicalDeviceProperties) |
| maxTessellationEvaluationOutputComponents (max) | - | - | 124 (in VkPhysicalDeviceProperties) | 124 (in VkPhysicalDeviceProperties) |
| maxTessellationGenerationLevel (max) | - | - | 64 (in VkPhysicalDeviceProperties) | 64 (in VkPhysicalDeviceProperties) |
| maxTessellationPatchSize (max) | - | - | 32 (in VkPhysicalDeviceProperties) | 32 (in VkPhysicalDeviceProperties) |
| maxTexelBufferElements (max) | - | 65536 (in VkPhysicalDeviceProperties) | 67108864 (in VkPhysicalDeviceProperties) | 67108864 (in VkPhysicalDeviceProperties) |
| maxTexelGatherOffset (max) | - | 7 (in VkPhysicalDeviceProperties) | 7 (in VkPhysicalDeviceProperties) | 7 (in VkPhysicalDeviceProperties) |
| maxTexelOffset (max) | - | 7 (in VkPhysicalDeviceProperties) | 7 (in VkPhysicalDeviceProperties) | 7 (in VkPhysicalDeviceProperties) |
| maxUniformBufferRange (max) | 65536 (in VkPhysicalDeviceProperties) | 16384 (in VkPhysicalDeviceProperties) | 65536 (in VkPhysicalDeviceProperties) | 65536 (in VkPhysicalDeviceProperties) |
| maxVertexInputAttributeOffset (max) | - | 2047 (in VkPhysicalDeviceProperties) | 2047 (in VkPhysicalDeviceProperties) | 2047 (in VkPhysicalDeviceProperties) |
| maxVertexInputAttributes (max) | - | 16 (in VkPhysicalDeviceProperties) | 28 (in VkPhysicalDeviceProperties) | 28 (in VkPhysicalDeviceProperties) |
| maxVertexInputBindingStride (max) | - | 2048 (in VkPhysicalDeviceProperties) | 2048 (in VkPhysicalDeviceProperties) | 2048 (in VkPhysicalDeviceProperties) |
| maxVertexInputBindings (max) | - | 16 (in VkPhysicalDeviceProperties) | 28 (in VkPhysicalDeviceProperties) | 28 (in VkPhysicalDeviceProperties) |
| maxVertexOutputComponents (max) | - | 64 (in VkPhysicalDeviceProperties) | 124 (in VkPhysicalDeviceProperties) | 124 (in VkPhysicalDeviceProperties) |
| maxViewportDimensions (max) | - | (8192,8192) (in VkPhysicalDeviceProperties) | (16384,16384) (in VkPhysicalDeviceProperties) | (16384,16384) (in VkPhysicalDeviceProperties) |
| maxViewports (max) | - | 1 (in VkPhysicalDeviceProperties) | 16 (in VkPhysicalDeviceProperties) | 16 (in VkPhysicalDeviceProperties) |
| minInterpolationOffset (min) | - | -0.5 (in VkPhysicalDeviceProperties) | -0.5 (in VkPhysicalDeviceProperties) | -0.5 (in VkPhysicalDeviceProperties) |
| minMemoryMapAlignment (min) | - | 4096 (in VkPhysicalDeviceProperties) | 4096 (in VkPhysicalDeviceProperties) | 4096 (in VkPhysicalDeviceProperties) |
| minStorageBufferOffsetAlignment (min) | - | 256 (in VkPhysicalDeviceProperties) | 64 (in VkPhysicalDeviceProperties) | 64 (in VkPhysicalDeviceProperties) |
| minTexelBufferOffsetAlignment (min) | - | 256 (in VkPhysicalDeviceProperties) | 64 (in VkPhysicalDeviceProperties) | 64 (in VkPhysicalDeviceProperties) |
| minTexelGatherOffset (min) | - | -8 (in VkPhysicalDeviceProperties) | -8 (in VkPhysicalDeviceProperties) | -8 (in VkPhysicalDeviceProperties) |
| minTexelOffset (min) | - | -8 (in VkPhysicalDeviceProperties) | -8 (in VkPhysicalDeviceProperties) | -8 (in VkPhysicalDeviceProperties) |
| minUniformBufferOffsetAlignment (min) | - | 256 (in VkPhysicalDeviceProperties) | 256 (in VkPhysicalDeviceProperties) | 256 (in VkPhysicalDeviceProperties) |
| mipmapPrecisionBits (max) | 6 (in VkPhysicalDeviceProperties) | 4 (in VkPhysicalDeviceProperties) | 4 (in VkPhysicalDeviceProperties) | 4 (in VkPhysicalDeviceProperties) |
| pointSizeGranularity (min) | 0.125 (in VkPhysicalDeviceProperties) | 1 (in VkPhysicalDeviceProperties) | 1.0 (in VkPhysicalDeviceProperties) | 1.0 (in VkPhysicalDeviceProperties) |
| pointSizeRange (min,max) | - | (1.0,1.0) (in VkPhysicalDeviceProperties) | (1.0,64.0) (in VkPhysicalDeviceProperties) | (1.0,64.0) (in VkPhysicalDeviceProperties) |
| sampledImageColorSampleCounts (min) | - | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) (in VkPhysicalDeviceProperties) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) (in VkPhysicalDeviceProperties) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) (in VkPhysicalDeviceProperties) |
| sampledImageDepthSampleCounts (min) | - | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) (in VkPhysicalDeviceProperties) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) (in VkPhysicalDeviceProperties) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) (in VkPhysicalDeviceProperties) |
| sampledImageIntegerSampleCounts (min) | - | (VK_SAMPLE_COUNT_1_BIT) (in VkPhysicalDeviceProperties) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) (in VkPhysicalDeviceProperties) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) (in VkPhysicalDeviceProperties) |
| sampledImageStencilSampleCounts (min) | - | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) (in VkPhysicalDeviceProperties) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) (in VkPhysicalDeviceProperties) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) (in VkPhysicalDeviceProperties) |
| storageImageSampleCounts (min) | - | (VK_SAMPLE_COUNT_1_BIT) (in VkPhysicalDeviceProperties) | (VK_SAMPLE_COUNT_1_BIT) (in VkPhysicalDeviceProperties) | (VK_SAMPLE_COUNT_1_BIT) (in VkPhysicalDeviceProperties) |
| subPixelInterpolationOffsetBits (max) | - | 4 (in VkPhysicalDeviceProperties) | 4 (in VkPhysicalDeviceProperties) | 4 (in VkPhysicalDeviceProperties) |
| subPixelPrecisionBits (max) | - | 4 (in VkPhysicalDeviceProperties) | 4 (in VkPhysicalDeviceProperties) | 4 (in VkPhysicalDeviceProperties) |
| subTexelPrecisionBits (max) | 8 (in VkPhysicalDeviceProperties) | 4 (in VkPhysicalDeviceProperties) | 4 (in VkPhysicalDeviceProperties) | 4 (in VkPhysicalDeviceProperties) |
| viewportBoundsRange (min,max) | - | (-8192,8192) (in VkPhysicalDeviceProperties) | (-32768,32767) (in VkPhysicalDeviceProperties) | (-32768,32767) (in VkPhysicalDeviceProperties) |
| **Vulkan 1.1** |
| maxMemoryAllocationSize (max) | - | - | 2147483648 (in VkPhysicalDeviceMaintenance3Properties) | 2147483648 (in VkPhysicalDeviceMaintenance3Properties) |
| maxMultiviewInstanceIndex (max) | - | - | 134217727 (in VkPhysicalDeviceMultiviewProperties) | 134217727 (in VkPhysicalDeviceMultiviewProperties) |
| maxMultiviewViewCount (max) | - | - | 6 (in VkPhysicalDeviceMultiviewProperties) | 6 (in VkPhysicalDeviceMultiviewProperties) |
| maxPerSetDescriptors (max) | - | - | 700 (in VkPhysicalDeviceMaintenance3Properties) | 700 (in VkPhysicalDeviceMaintenance3Properties) |
| subgroupSize (max) | 4 (in VkPhysicalDeviceVulkan11Properties) | - | - | - |
| subgroupSupportedOperations (min) | (VK_SUBGROUP_FEATURE_BASIC_BIT \| VK_SUBGROUP_FEATURE_VOTE_BIT \| VK_SUBGROUP_FEATURE_ARITHMETIC_BIT \| VK_SUBGROUP_FEATURE_BALLOT_BIT \| VK_SUBGROUP_FEATURE_SHUFFLE_BIT \| VK_SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT \| VK_SUBGROUP_FEATURE_QUAD_BIT) (in VkPhysicalDeviceVulkan11Properties) | - | - | - |
| subgroupSupportedStages (min) | (VK_SHADER_STAGE_COMPUTE_BIT \| VK_SHADER_STAGE_FRAGMENT_BIT) (in VkPhysicalDeviceVulkan11Properties) | - | - | - |
| **Vulkan 1.2** |
| independentResolve | - | - | VK_TRUE (in VkPhysicalDeviceDepthStencilResolvePropertiesKHR) | VK_TRUE (in VkPhysicalDeviceDepthStencilResolvePropertiesKHR) |
| independentResolveNone | - | - | VK_TRUE (in VkPhysicalDeviceDepthStencilResolvePropertiesKHR) | VK_TRUE (in VkPhysicalDeviceDepthStencilResolvePropertiesKHR) |
| maxDescriptorSetUpdateAfterBindInputAttachments (max) | - | - | 8 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 8 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxDescriptorSetUpdateAfterBindSampledImages (max) | - | - | 640 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 640 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxDescriptorSetUpdateAfterBindSamplers (max) | - | - | 80 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 80 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxDescriptorSetUpdateAfterBindStorageBuffers (max) | - | - | 155 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 155 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxDescriptorSetUpdateAfterBindStorageBuffersDynamic (max) | - | - | 8 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 8 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxDescriptorSetUpdateAfterBindStorageImages (max) | - | - | 40 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 40 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxDescriptorSetUpdateAfterBindUniformBuffers (max) | - | - | 90 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 90 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxDescriptorSetUpdateAfterBindUniformBuffersDynamic (max) | - | - | 8 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 8 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxPerStageDescriptorUpdateAfterBindInputAttachments (max) | - | - | 128 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 128 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxPerStageDescriptorUpdateAfterBindSampledImages (max) | - | - | 128 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 128 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxPerStageDescriptorUpdateAfterBindSamplers (max) | - | - | 16 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 16 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxPerStageDescriptorUpdateAfterBindStorageBuffers (max) | - | - | 31 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 31 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxPerStageDescriptorUpdateAfterBindStorageImages (max) | - | - | 8 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 8 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxPerStageDescriptorUpdateAfterBindUniformBuffers (max) | - | - | 15 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 15 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxPerStageUpdateAfterBindResources (max) | - | - | 159 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 159 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxTimelineSemaphoreValueDifference (max) | 2147483647 (in VkPhysicalDeviceVulkan12Properties) | - | - | - |
| maxUpdateAfterBindDescriptorsInAllPools (max) | - | - | 1048576 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 1048576 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| shaderSignedZeroInfNanPreserveFloat16 | VK_TRUE (in VkPhysicalDeviceVulkan12Properties) | - | - | - |
| shaderSignedZeroInfNanPreserveFloat32 | VK_TRUE (in VkPhysicalDeviceVulkan12Properties) | - | - | - |
| supportedDepthResolveModes (min) | - | - | (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT) (in VkPhysicalDeviceDepthStencilResolvePropertiesKHR) | (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT) (in VkPhysicalDeviceDepthStencilResolvePropertiesKHR) |
| supportedStencilResolveModes (min) | - | - | (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT) (in VkPhysicalDeviceDepthStencilResolvePropertiesKHR) | (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT) (in VkPhysicalDeviceDepthStencilResolvePropertiesKHR) |
| **Vulkan 1.3** |
| maxBufferSize (max) | 1073741824 (in VkPhysicalDeviceVulkan13Properties) | - | - | - |
| maxDescriptorSetInlineUniformBlocks (max) | 4 (in VkPhysicalDeviceVulkan13Properties) | - | - | - |
| maxDescriptorSetUpdateAfterBindInlineUniformBlocks (max) | 4 (in VkPhysicalDeviceVulkan13Properties) | - | - | - |
| maxInlineUniformBlockSize (max) | 256 (in VkPhysicalDeviceVulkan13Properties) | - | - | - |
| maxInlineUniformTotalSize (max) | 4 (in VkPhysicalDeviceVulkan13Properties) | - | - | - |
| maxPerStageDescriptorInlineUniformBlocks (max) | 4 (in VkPhysicalDeviceVulkan13Properties) | - | - | - |
| maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks (max) | 4 (in VkPhysicalDeviceVulkan13Properties) | - | - | - |
| **VK_KHR_depth_stencil_resolve** |
| independentResolve | - | - | VK_TRUE (in VkPhysicalDeviceDepthStencilResolvePropertiesKHR) | VK_TRUE (in VkPhysicalDeviceDepthStencilResolvePropertiesKHR) |
| independentResolveNone | - | - | VK_TRUE (in VkPhysicalDeviceDepthStencilResolvePropertiesKHR) | VK_TRUE (in VkPhysicalDeviceDepthStencilResolvePropertiesKHR) |
| supportedDepthResolveModes (min) | - | - | (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT) (in VkPhysicalDeviceDepthStencilResolvePropertiesKHR) | (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT) (in VkPhysicalDeviceDepthStencilResolvePropertiesKHR) |
| supportedStencilResolveModes (min) | - | - | (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT) (in VkPhysicalDeviceDepthStencilResolvePropertiesKHR) | (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT) (in VkPhysicalDeviceDepthStencilResolvePropertiesKHR) |
| **VK_KHR_maintenance3** |
| maxMemoryAllocationSize (max) | - | - | 2147483648 (in VkPhysicalDeviceMaintenance3Properties) | 2147483648 (in VkPhysicalDeviceMaintenance3Properties) |
| maxPerSetDescriptors (max) | - | - | 700 (in VkPhysicalDeviceMaintenance3Properties) | 700 (in VkPhysicalDeviceMaintenance3Properties) |
| **VK_KHR_maintenance4** |
| maxBufferSize (max) | 1073741824 (in VkPhysicalDeviceVulkan13Properties) | - | - | - |
| **VK_KHR_multiview** |
| maxMultiviewInstanceIndex (max) | - | - | 134217727 (in VkPhysicalDeviceMultiviewProperties) | 134217727 (in VkPhysicalDeviceMultiviewProperties) |
| maxMultiviewViewCount (max) | - | - | 6 (in VkPhysicalDeviceMultiviewProperties) | 6 (in VkPhysicalDeviceMultiviewProperties) |
| **VK_KHR_portability_subset** |
| minVertexInputBindingStrideAlignment (min) | - | - | - | 4 (in VkPhysicalDevicePortabilitySubsetPropertiesKHR) |
| **VK_KHR_shader_float_controls** |
| shaderSignedZeroInfNanPreserveFloat16 | VK_TRUE (in VkPhysicalDeviceVulkan12Properties) | - | - | - |
| shaderSignedZeroInfNanPreserveFloat32 | VK_TRUE (in VkPhysicalDeviceVulkan12Properties) | - | - | - |
| **VK_KHR_timeline_semaphore** |
| maxTimelineSemaphoreValueDifference (max) | 2147483647 (in VkPhysicalDeviceVulkan12Properties) | - | - | - |
| **VK_EXT_descriptor_indexing** |
| maxDescriptorSetUpdateAfterBindInputAttachments (max) | - | - | 8 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 8 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxDescriptorSetUpdateAfterBindSampledImages (max) | - | - | 640 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 640 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxDescriptorSetUpdateAfterBindSamplers (max) | - | - | 80 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 80 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxDescriptorSetUpdateAfterBindStorageBuffers (max) | - | - | 155 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 155 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxDescriptorSetUpdateAfterBindStorageBuffersDynamic (max) | - | - | 8 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 8 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxDescriptorSetUpdateAfterBindStorageImages (max) | - | - | 40 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 40 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxDescriptorSetUpdateAfterBindUniformBuffers (max) | - | - | 90 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 90 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxDescriptorSetUpdateAfterBindUniformBuffersDynamic (max) | - | - | 8 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 8 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxPerStageDescriptorUpdateAfterBindInputAttachments (max) | - | - | 128 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 128 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxPerStageDescriptorUpdateAfterBindSampledImages (max) | - | - | 128 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 128 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxPerStageDescriptorUpdateAfterBindSamplers (max) | - | - | 16 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 16 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxPerStageDescriptorUpdateAfterBindStorageBuffers (max) | - | - | 31 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 31 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxPerStageDescriptorUpdateAfterBindStorageImages (max) | - | - | 8 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 8 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxPerStageDescriptorUpdateAfterBindUniformBuffers (max) | - | - | 15 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 15 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxPerStageUpdateAfterBindResources (max) | - | - | 159 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 159 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| maxUpdateAfterBindDescriptorsInAllPools (max) | - | - | 1048576 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) | 1048576 (in VkPhysicalDeviceDescriptorIndexingPropertiesEXT) |
| **VK_EXT_inline_uniform_block** |
| maxDescriptorSetInlineUniformBlocks (max) | 4 (in VkPhysicalDeviceVulkan13Properties) | - | - | - |
| maxDescriptorSetUpdateAfterBindInlineUniformBlocks (max) | 4 (in VkPhysicalDeviceVulkan13Properties) | - | - | - |
| maxInlineUniformBlockSize (max) | 256 (in VkPhysicalDeviceVulkan13Properties) | - | - | - |
| maxPerStageDescriptorInlineUniformBlocks (max) | 4 (in VkPhysicalDeviceVulkan13Properties) | - | - | - |
| maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks (max) | 4 (in VkPhysicalDeviceVulkan13Properties) | - | - | - |
