
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
| depthBiasClamp | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| depthClamp | :heavy_check_mark: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| drawIndirectFirstInstance | :heavy_check_mark: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| dualSrcBlend | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| fillModeNonSolid | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| fragmentStoresAndAtomics | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| fullDrawIndexUint32 | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| imageCubeArray | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| independentBlend | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| inheritedQueries | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| largePoints | :x: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| multiDrawIndirect | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| multiViewport | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| occlusionQueryPrecise | :heavy_check_mark: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| robustBufferAccess | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| sampleRateShading | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| samplerAnisotropy | :heavy_check_mark: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| shaderClipDistance | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| shaderImageGatherExtended | :x: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| shaderSampledImageArrayDynamicIndexing | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| shaderStorageBufferArrayDynamicIndexing | :heavy_check_mark: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| shaderStorageImageArrayDynamicIndexing | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| shaderStorageImageExtendedFormats | :heavy_check_mark: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| shaderStorageImageWriteWithoutFormat | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| shaderTessellationAndGeometryPointSize | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| shaderUniformBufferArrayDynamicIndexing | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| tessellationShader | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| textureCompressionASTC_LDR | :x: | :heavy_check_mark: | :x: | :x: |
| textureCompressionBC | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| textureCompressionETC2 | :x: | :heavy_check_mark: | :x: | :x: |
| vertexPipelineStoresAndAtomics | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| **Vulkan 1.1** |
| multiview | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| samplerYcbcrConversion | :heavy_check_mark: | :x: | :x: | :x: |
| shaderDrawParameters | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| storageBuffer16BitAccess | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| uniformAndStorageBuffer16BitAccess | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| variablePointers | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| variablePointersStorageBuffer | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| **Vulkan 1.2** |
| bufferDeviceAddress | :heavy_check_mark: | :x: | :x: | :x: |
| descriptorBindingPartiallyBound | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| descriptorBindingSampledImageUpdateAfterBind | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| descriptorBindingStorageBufferUpdateAfterBind | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| descriptorBindingStorageImageUpdateAfterBind | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| descriptorBindingStorageTexelBufferUpdateAfterBind | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| descriptorBindingUniformBufferUpdateAfterBind | :heavy_check_mark: | :x: | :x: | :x: |
| descriptorBindingUniformTexelBufferUpdateAfterBind | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| descriptorBindingUpdateUnusedWhilePending | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| descriptorBindingVariableDescriptorCount | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| descriptorIndexing | :heavy_check_mark: | :x: | :x: | :x: |
| hostQueryReset | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| imagelessFramebuffer | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| runtimeDescriptorArray | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| samplerMirrorClampToEdge | :heavy_check_mark: | :x: | :x: | :x: |
| scalarBlockLayout | :heavy_check_mark: | :x: | :x: | :x: |
| separateDepthStencilLayouts | :heavy_check_mark: | :x: | :x: | :x: |
| shaderInt8 | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| shaderSampledImageArrayNonUniformIndexing | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| shaderStorageBufferArrayNonUniformIndexing | :heavy_check_mark: | :x: | :x: | :x: |
| shaderStorageImageArrayNonUniformIndexing | :heavy_check_mark: | :x: | :x: | :x: |
| shaderStorageTexelBufferArrayDynamicIndexing | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| shaderStorageTexelBufferArrayNonUniformIndexing | :heavy_check_mark: | :x: | :x: | :x: |
| shaderSubgroupExtendedTypes | :heavy_check_mark: | :x: | :x: | :x: |
| shaderUniformBufferArrayNonUniformIndexing | :heavy_check_mark: | :x: | :x: | :x: |
| shaderUniformTexelBufferArrayDynamicIndexing | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| shaderUniformTexelBufferArrayNonUniformIndexing | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| storageBuffer8BitAccess | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| subgroupBroadcastDynamicId | :heavy_check_mark: | :x: | :x: | :x: |
| timelineSemaphore | :heavy_check_mark: | :x: | :x: | :x: |
| uniformAndStorageBuffer8BitAccess | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| uniformBufferStandardLayout | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| vulkanMemoryModel | :heavy_check_mark: | :x: | :x: | :x: |
| vulkanMemoryModelAvailabilityVisibilityChains | :heavy_check_mark: | :x: | :x: | :x: |
| vulkanMemoryModelDeviceScope | :heavy_check_mark: | :x: | :x: | :x: |
| **Vulkan 1.3** |
| computeFullSubgroups | :heavy_check_mark: | :x: | :x: | :x: |
| descriptorBindingInlineUniformBlockUpdateAfterBind | :heavy_check_mark: | :x: | :x: | :x: |
| inlineUniformBlock | :heavy_check_mark: | :x: | :x: | :x: |
| maintenance4 | :heavy_check_mark: | :x: | :x: | :x: |
| pipelineCreationCacheControl | :heavy_check_mark: | :x: | :x: | :x: |
| robustImageAccess | :heavy_check_mark: | :x: | :x: | :x: |
| shaderDemoteToHelperInvocation | :heavy_check_mark: | :x: | :x: | :x: |
| shaderIntegerDotProduct | :heavy_check_mark: | :x: | :x: | :x: |
| shaderTerminateInvocation | :heavy_check_mark: | :x: | :x: | :x: |
| shaderZeroInitializeWorkgroupMemory | :heavy_check_mark: | :x: | :x: | :x: |
| subgroupSizeControl | :heavy_check_mark: | :x: | :x: | :x: |
| synchronization2 | :heavy_check_mark: | :x: | :x: | :x: |
| **VK_KHR_16bit_storage** |
| storageBuffer16BitAccess | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| uniformAndStorageBuffer16BitAccess | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| **VK_KHR_8bit_storage** |
| storageBuffer8BitAccess | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| uniformAndStorageBuffer8BitAccess | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| **VK_KHR_buffer_device_address** |
| bufferDeviceAddress | :heavy_check_mark: | :x: | :x: | :x: |
| **VK_KHR_imageless_framebuffer** |
| imagelessFramebuffer | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| **VK_KHR_maintenance4** |
| maintenance4 | :heavy_check_mark: | :x: | :x: | :x: |
| **VK_KHR_multiview** |
| multiview | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| **VK_KHR_portability_subset** |
| constantAlphaColorBlendFactors | :x: | :x: | :x: | :heavy_check_mark: |
| events | :x: | :x: | :x: | :heavy_check_mark: |
| imageViewFormatReinterpretation | :x: | :x: | :x: | :heavy_check_mark: |
| imageViewFormatSwizzle | :x: | :x: | :x: | :heavy_check_mark: |
| multisampleArrayImage | :x: | :x: | :x: | :heavy_check_mark: |
| mutableComparisonSamplers | :x: | :x: | :x: | :heavy_check_mark: |
| separateStencilMaskRef | :x: | :x: | :x: | :heavy_check_mark: |
| vertexAttributeAccessBeyondStride | :x: | :x: | :x: | :heavy_check_mark: |
| **VK_KHR_sampler_ycbcr_conversion** |
| samplerYcbcrConversion | :heavy_check_mark: | :x: | :x: | :x: |
| **VK_KHR_separate_depth_stencil_layouts** |
| separateDepthStencilLayouts | :heavy_check_mark: | :x: | :x: | :x: |
| **VK_KHR_shader_float16_int8** |
| shaderInt8 | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| **VK_KHR_shader_integer_dot_product** |
| shaderIntegerDotProduct | :heavy_check_mark: | :x: | :x: | :x: |
| **VK_KHR_shader_subgroup_extended_types** |
| shaderSubgroupExtendedTypes | :heavy_check_mark: | :x: | :x: | :x: |
| **VK_KHR_shader_terminate_invocation** |
| shaderTerminateInvocation | :heavy_check_mark: | :x: | :x: | :x: |
| **VK_KHR_synchronization2** |
| synchronization2 | :heavy_check_mark: | :x: | :x: | :x: |
| **VK_KHR_timeline_semaphore** |
| timelineSemaphore | :heavy_check_mark: | :x: | :x: | :x: |
| **VK_KHR_uniform_buffer_standard_layout** |
| uniformBufferStandardLayout | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| **VK_KHR_variable_pointers** |
| variablePointers | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| variablePointersStorageBuffer | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| **VK_KHR_vulkan_memory_model** |
| vulkanMemoryModel | :heavy_check_mark: | :x: | :x: | :x: |
| vulkanMemoryModelAvailabilityVisibilityChains | :heavy_check_mark: | :x: | :x: | :x: |
| vulkanMemoryModelDeviceScope | :heavy_check_mark: | :x: | :x: | :x: |
| **VK_KHR_zero_initialize_workgroup_memory** |
| shaderZeroInitializeWorkgroupMemory | :heavy_check_mark: | :x: | :x: | :x: |
| **VK_EXT_descriptor_indexing** |
| descriptorBindingPartiallyBound | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| descriptorBindingSampledImageUpdateAfterBind | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| descriptorBindingStorageBufferUpdateAfterBind | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| descriptorBindingStorageImageUpdateAfterBind | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| descriptorBindingStorageTexelBufferUpdateAfterBind | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| descriptorBindingUniformBufferUpdateAfterBind | :heavy_check_mark: | :x: | :x: | :x: |
| descriptorBindingUniformTexelBufferUpdateAfterBind | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| descriptorBindingUpdateUnusedWhilePending | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| descriptorBindingVariableDescriptorCount | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| runtimeDescriptorArray | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| shaderSampledImageArrayNonUniformIndexing | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| shaderStorageBufferArrayNonUniformIndexing | :heavy_check_mark: | :x: | :x: | :x: |
| shaderStorageImageArrayNonUniformIndexing | :heavy_check_mark: | :x: | :x: | :x: |
| shaderStorageTexelBufferArrayDynamicIndexing | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| shaderStorageTexelBufferArrayNonUniformIndexing | :heavy_check_mark: | :x: | :x: | :x: |
| shaderUniformBufferArrayNonUniformIndexing | :heavy_check_mark: | :x: | :x: | :x: |
| shaderUniformTexelBufferArrayDynamicIndexing | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| shaderUniformTexelBufferArrayNonUniformIndexing | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| **VK_EXT_host_query_reset** |
| hostQueryReset | :x: | :x: | :heavy_check_mark: | :heavy_check_mark: |
| **VK_EXT_image_robustness** |
| robustImageAccess | :heavy_check_mark: | :x: | :x: | :x: |
| **VK_EXT_inline_uniform_block** |
| descriptorBindingInlineUniformBlockUpdateAfterBind | :heavy_check_mark: | :x: | :x: | :x: |
| inlineUniformBlock | :heavy_check_mark: | :x: | :x: | :x: |
| **VK_EXT_pipeline_creation_cache_control** |
| pipelineCreationCacheControl | :heavy_check_mark: | :x: | :x: | :x: |
| **VK_EXT_scalar_block_layout** |
| scalarBlockLayout | :heavy_check_mark: | :x: | :x: | :x: |
| **VK_EXT_shader_demote_to_helper_invocation** |
| shaderDemoteToHelperInvocation | :heavy_check_mark: | :x: | :x: | :x: |
| **VK_EXT_subgroup_size_control** |
| computeFullSubgroups | :heavy_check_mark: | :x: | :x: | :x: |
| subgroupSizeControl | :heavy_check_mark: | :x: | :x: | :x: |

## Vulkan Profile Limits (Properties)

> **NOTE**: The table below only contains properties/limits explicitly defined by the corresponding profile. Further properties/limits may be supported by the profiles in accordance to the requirements defined in the "Limit Requirements" section of the appropriate version of the Vulkan API Specification.

| Profiles | VP_KHR_roadmap_2022 | VP_ANDROID_baseline_2021 | VP_LUNARG_desktop_portability_2021 | VP_LUNARG_desktop_portability_2021_subset |
|----------|---------------------|--------------------------|------------------------------------|-------------------------------------------|
| **Vulkan 1.0** |
| bufferImageGranularity (min) | 4096 | - | 1024 | 1024 |
| discreteQueuePriorities (max) | - | 2 | 2 | 2 |
| framebufferColorSampleCounts (min) | - | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) |
| framebufferDepthSampleCounts (min) | - | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) |
| framebufferStencilSampleCounts (min) | - | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) |
| lineWidthGranularity (min) | 0.5 | 1 | 0.5 | 0.5 |
| lineWidthRange (min,max) | - | (1.0,1.0) | (1.0,1.0) | (1.0,1.0) |
| maxBoundDescriptorSets (max) | - | 4 | 8 | 8 |
| maxClipDistances (max) | - | - | 8 | 8 |
| maxColorAttachments (max) | 7 | 4 | 8 | 8 |
| maxCombinedClipAndCullDistances (max) | - | - | 8 | 8 |
| maxComputeSharedMemorySize (max) | - | 16384 | 32768 | 32768 |
| maxComputeWorkGroupCount (max) | - | (65535,65535,65535) | (65535,65535,65535) | (65535,65535,65535) |
| maxComputeWorkGroupInvocations (max) | 256 | 128 | 1024 | 1024 |
| maxComputeWorkGroupSize (max) | (1024,1024,64) | (128,128,64) | (1024,1024,64) | (1024,1024,64) |
| maxDescriptorSetInputAttachments (max) | - | 4 | 8 | 8 |
| maxDescriptorSetSampledImages (max) | 1800 | 96 | 256 | 256 |
| maxDescriptorSetSamplers (max) | 576 | 96 | 80 | 80 |
| maxDescriptorSetStorageBuffers (max) | 96 | 24 | 155 | 155 |
| maxDescriptorSetStorageBuffersDynamic (max) | - | 4 | 8 | 8 |
| maxDescriptorSetStorageImages (max) | 144 | 24 | 40 | 40 |
| maxDescriptorSetUniformBuffers (max) | 90 | 72 | 90 | 90 |
| maxDescriptorSetUniformBuffersDynamic (max) | - | 8 | 8 | 8 |
| maxDrawIndexedIndexValue (max) | - | 4294967295 | 4294967295 | 4294967295 |
| maxDrawIndirectCount (max) | - | 1 | 1073740000 | 1073740000 |
| maxFragmentCombinedOutputResources (max) | 16 | 8 | 8 | 8 |
| maxFragmentDualSrcAttachments (max) | - | - | 1 | 1 |
| maxFragmentInputComponents (max) | - | 64 | 116 | 116 |
| maxFragmentOutputAttachments (max) | - | 4 | 8 | 8 |
| maxFramebufferHeight (max) | - | 8192 | 16384 | 16384 |
| maxFramebufferLayers (max) | - | 256 | 1024 | 1024 |
| maxFramebufferWidth (max) | - | 8192 | 16384 | 16384 |
| maxImageArrayLayers (max) | 2048 | 256 | 2048 | 2048 |
| maxImageDimension1D (max) | 8192 | 8192 | 16384 | 16384 |
| maxImageDimension2D (max) | 8192 | 8192 | 16384 | 16384 |
| maxImageDimension3D (max) | - | 2048 | 2048 | 2048 |
| maxImageDimensionCube (max) | 8192 | 8192 | 16384 | 16384 |
| maxInterpolationOffset (max) | - | 0.4375 | 0.4375 | 0.4375 |
| maxMemoryAllocationCount (max) | - | 4096 | 4096 | 4096 |
| maxPerStageDescriptorInputAttachments (max) | - | 4 | 8 | 8 |
| maxPerStageDescriptorSampledImages (max) | 200 | 16 | 128 | 128 |
| maxPerStageDescriptorSamplers (max) | 64 | 16 | 16 | 16 |
| maxPerStageDescriptorStorageBuffers (max) | 30 | 4 | 16 | 16 |
| maxPerStageDescriptorStorageImages (max) | 16 | 4 | 8 | 8 |
| maxPerStageDescriptorUniformBuffers (max) | 15 | 12 | 15 | 15 |
| maxPerStageResources (max) | 200 | 44 | 128 | 128 |
| maxPushConstantsSize (max) | - | 128 | 128 | 128 |
| maxSampleMaskWords (max) | - | 1 | 1 | 1 |
| maxSamplerAllocationCount (max) | - | 4000 | 1024 | 1024 |
| maxSamplerAnisotropy (max) | - | 1.0 | 16.0 | 16.0 |
| maxSamplerLodBias (max) | 14 | 2.0 | 4.0 | 4.0 |
| maxStorageBufferRange (max) | - | 134217728 | 134217728 | 134217728 |
| maxTessellationControlPerPatchOutputComponents (max) | - | - | 120 | 120 |
| maxTessellationControlPerVertexInputComponents (max) | - | - | 124 | 124 |
| maxTessellationControlPerVertexOutputComponents (max) | - | - | 124 | 124 |
| maxTessellationControlTotalOutputComponents (max) | - | - | 2048 | 2048 |
| maxTessellationEvaluationInputComponents (max) | - | - | 124 | 124 |
| maxTessellationEvaluationOutputComponents (max) | - | - | 124 | 124 |
| maxTessellationGenerationLevel (max) | - | - | 64 | 64 |
| maxTessellationPatchSize (max) | - | - | 32 | 32 |
| maxTexelBufferElements (max) | - | 65536 | 67108864 | 67108864 |
| maxTexelGatherOffset (max) | - | 7 | 7 | 7 |
| maxTexelOffset (max) | - | 7 | 7 | 7 |
| maxUniformBufferRange (max) | 65536 | 16384 | 65536 | 65536 |
| maxVertexInputAttributeOffset (max) | - | 2047 | 2047 | 2047 |
| maxVertexInputAttributes (max) | - | 16 | 28 | 28 |
| maxVertexInputBindingStride (max) | - | 2048 | 2048 | 2048 |
| maxVertexInputBindings (max) | - | 16 | 28 | 28 |
| maxVertexOutputComponents (max) | - | 64 | 124 | 124 |
| maxViewportDimensions (max) | - | (8192,8192) | (16384,16384) | (16384,16384) |
| maxViewports (max) | - | 1 | 16 | 16 |
| minInterpolationOffset (min) | - | -0.5 | -0.5 | -0.5 |
| minMemoryMapAlignment (min) | - | 4096 | 4096 | 4096 |
| minStorageBufferOffsetAlignment (min) | - | 256 | 64 | 64 |
| minTexelBufferOffsetAlignment (min) | - | 256 | 64 | 64 |
| minTexelGatherOffset (min) | - | -8 | -8 | -8 |
| minTexelOffset (min) | - | -8 | -8 | -8 |
| minUniformBufferOffsetAlignment (min) | - | 256 | 256 | 256 |
| mipmapPrecisionBits (max) | 6 | 4 | 4 | 4 |
| pointSizeGranularity (min) | 0.125 | 1 | 1.0 | 1.0 |
| pointSizeRange (min,max) | - | (1.0,1.0) | (1.0,64.0) | (1.0,64.0) |
| sampledImageColorSampleCounts (min) | - | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) |
| sampledImageDepthSampleCounts (min) | - | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) |
| sampledImageIntegerSampleCounts (min) | - | (VK_SAMPLE_COUNT_1_BIT) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) |
| sampledImageStencilSampleCounts (min) | - | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) | (VK_SAMPLE_COUNT_1_BIT \| VK_SAMPLE_COUNT_4_BIT) |
| storageImageSampleCounts (min) | - | (VK_SAMPLE_COUNT_1_BIT) | (VK_SAMPLE_COUNT_1_BIT) | (VK_SAMPLE_COUNT_1_BIT) |
| subPixelInterpolationOffsetBits (max) | - | 4 | 4 | 4 |
| subPixelPrecisionBits (max) | - | 4 | 4 | 4 |
| subTexelPrecisionBits (max) | 8 | 4 | 4 | 4 |
| viewportBoundsRange (min,max) | - | (-8192,8192) | (-32768,32767) | (-32768,32767) |
| **Vulkan 1.1** |
| maxMemoryAllocationSize (max) | - | - | 2147483648 | 2147483648 |
| maxMultiviewInstanceIndex (max) | - | - | 134217727 | 134217727 |
| maxMultiviewViewCount (max) | - | - | 6 | 6 |
| maxPerSetDescriptors (max) | - | - | 700 | 700 |
| subgroupSize (max) | 4 | - | - | - |
| subgroupSupportedOperations (min) | (VK_SUBGROUP_FEATURE_BASIC_BIT \| VK_SUBGROUP_FEATURE_VOTE_BIT \| VK_SUBGROUP_FEATURE_ARITHMETIC_BIT \| VK_SUBGROUP_FEATURE_BALLOT_BIT \| VK_SUBGROUP_FEATURE_SHUFFLE_BIT \| VK_SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT \| VK_SUBGROUP_FEATURE_QUAD_BIT) | - | - | - |
| subgroupSupportedStages (min) | (VK_SHADER_STAGE_COMPUTE_BIT \| VK_SHADER_STAGE_FRAGMENT_BIT) | - | - | - |
| **Vulkan 1.2** |
| independentResolve | - | - | VK_TRUE | VK_TRUE |
| independentResolveNone | - | - | VK_TRUE | VK_TRUE |
| maxDescriptorSetUpdateAfterBindInputAttachments (max) | - | - | 8 | 8 |
| maxDescriptorSetUpdateAfterBindSampledImages (max) | - | - | 640 | 640 |
| maxDescriptorSetUpdateAfterBindSamplers (max) | - | - | 80 | 80 |
| maxDescriptorSetUpdateAfterBindStorageBuffers (max) | - | - | 155 | 155 |
| maxDescriptorSetUpdateAfterBindStorageBuffersDynamic (max) | - | - | 8 | 8 |
| maxDescriptorSetUpdateAfterBindStorageImages (max) | - | - | 40 | 40 |
| maxDescriptorSetUpdateAfterBindUniformBuffers (max) | - | - | 90 | 90 |
| maxDescriptorSetUpdateAfterBindUniformBuffersDynamic (max) | - | - | 8 | 8 |
| maxPerStageDescriptorUpdateAfterBindInputAttachments (max) | - | - | 128 | 128 |
| maxPerStageDescriptorUpdateAfterBindSampledImages (max) | - | - | 128 | 128 |
| maxPerStageDescriptorUpdateAfterBindSamplers (max) | - | - | 16 | 16 |
| maxPerStageDescriptorUpdateAfterBindStorageBuffers (max) | - | - | 31 | 31 |
| maxPerStageDescriptorUpdateAfterBindStorageImages (max) | - | - | 8 | 8 |
| maxPerStageDescriptorUpdateAfterBindUniformBuffers (max) | - | - | 15 | 15 |
| maxPerStageUpdateAfterBindResources (max) | - | - | 159 | 159 |
| maxTimelineSemaphoreValueDifference (max) | 2147483647 | - | - | - |
| maxUpdateAfterBindDescriptorsInAllPools (max) | - | - | 1048576 | 1048576 |
| shaderSignedZeroInfNanPreserveFloat16 | VK_TRUE | - | - | - |
| shaderSignedZeroInfNanPreserveFloat32 | VK_TRUE | - | - | - |
| supportedDepthResolveModes (min) | - | - | (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT) | (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT) |
| supportedStencilResolveModes (min) | - | - | (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT) | (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT) |
| **Vulkan 1.3** |
| maxBufferSize (max) | 1073741824 | - | - | - |
| maxDescriptorSetInlineUniformBlocks (max) | 4 | - | - | - |
| maxDescriptorSetUpdateAfterBindInlineUniformBlocks (max) | 4 | - | - | - |
| maxInlineUniformBlockSize (max) | 256 | - | - | - |
| maxInlineUniformTotalSize (max) | 4 | - | - | - |
| maxPerStageDescriptorInlineUniformBlocks (max) | 4 | - | - | - |
| maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks (max) | 4 | - | - | - |
| **VK_KHR_depth_stencil_resolve** |
| independentResolve | - | - | VK_TRUE | VK_TRUE |
| independentResolveNone | - | - | VK_TRUE | VK_TRUE |
| supportedDepthResolveModes (min) | - | - | (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT) | (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT) |
| supportedStencilResolveModes (min) | - | - | (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT) | (VK_RESOLVE_MODE_SAMPLE_ZERO_BIT) |
| **VK_KHR_maintenance3** |
| maxMemoryAllocationSize (max) | - | - | 2147483648 | 2147483648 |
| maxPerSetDescriptors (max) | - | - | 700 | 700 |
| **VK_KHR_maintenance4** |
| maxBufferSize (max) | 1073741824 | - | - | - |
| **VK_KHR_multiview** |
| maxMultiviewInstanceIndex (max) | - | - | 134217727 | 134217727 |
| maxMultiviewViewCount (max) | - | - | 6 | 6 |
| **VK_KHR_portability_subset** |
| minVertexInputBindingStrideAlignment (min) | - | - | - | 4 |
| **VK_KHR_shader_float_controls** |
| shaderSignedZeroInfNanPreserveFloat16 | VK_TRUE | - | - | - |
| shaderSignedZeroInfNanPreserveFloat32 | VK_TRUE | - | - | - |
| **VK_KHR_timeline_semaphore** |
| maxTimelineSemaphoreValueDifference (max) | 2147483647 | - | - | - |
| **VK_EXT_descriptor_indexing** |
| maxDescriptorSetUpdateAfterBindInputAttachments (max) | - | - | 8 | 8 |
| maxDescriptorSetUpdateAfterBindSampledImages (max) | - | - | 640 | 640 |
| maxDescriptorSetUpdateAfterBindSamplers (max) | - | - | 80 | 80 |
| maxDescriptorSetUpdateAfterBindStorageBuffers (max) | - | - | 155 | 155 |
| maxDescriptorSetUpdateAfterBindStorageBuffersDynamic (max) | - | - | 8 | 8 |
| maxDescriptorSetUpdateAfterBindStorageImages (max) | - | - | 40 | 40 |
| maxDescriptorSetUpdateAfterBindUniformBuffers (max) | - | - | 90 | 90 |
| maxDescriptorSetUpdateAfterBindUniformBuffersDynamic (max) | - | - | 8 | 8 |
| maxPerStageDescriptorUpdateAfterBindInputAttachments (max) | - | - | 128 | 128 |
| maxPerStageDescriptorUpdateAfterBindSampledImages (max) | - | - | 128 | 128 |
| maxPerStageDescriptorUpdateAfterBindSamplers (max) | - | - | 16 | 16 |
| maxPerStageDescriptorUpdateAfterBindStorageBuffers (max) | - | - | 31 | 31 |
| maxPerStageDescriptorUpdateAfterBindStorageImages (max) | - | - | 8 | 8 |
| maxPerStageDescriptorUpdateAfterBindUniformBuffers (max) | - | - | 15 | 15 |
| maxPerStageUpdateAfterBindResources (max) | - | - | 159 | 159 |
| maxUpdateAfterBindDescriptorsInAllPools (max) | - | - | 1048576 | 1048576 |
| **VK_EXT_inline_uniform_block** |
| maxDescriptorSetInlineUniformBlocks (max) | 4 | - | - | - |
| maxDescriptorSetUpdateAfterBindInlineUniformBlocks (max) | 4 | - | - | - |
| maxInlineUniformBlockSize (max) | 256 | - | - | - |
| maxPerStageDescriptorInlineUniformBlocks (max) | 4 | - | - | - |
| maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks (max) | 4 | - | - | - |

## Vulkan Profile Queue Families

| Profiles | VP_KHR_roadmap_2022 | VP_ANDROID_baseline_2021 | VP_LUNARG_desktop_portability_2021 | VP_LUNARG_desktop_portability_2021_subset |
|----------|---------------------|--------------------------|------------------------------------|-------------------------------------------|
| **Queue family #0** |
| minImageTransferGranularity (min) |  |  | (1,1,1) | (1,1,1) |
| queueCount (max) |  |  | 1 | 1 |
| queueFlags |  |  | (VK_QUEUE_GRAPHICS_BIT \| VK_QUEUE_COMPUTE_BIT \| VK_QUEUE_TRANSFER_BIT \| VK_QUEUE_SPARSE_BINDING_BIT) | (VK_QUEUE_GRAPHICS_BIT \| VK_QUEUE_COMPUTE_BIT \| VK_QUEUE_TRANSFER_BIT \| VK_QUEUE_SPARSE_BINDING_BIT) |
| timestampValidBits (max) |  |  | 36 | 36 |

## Vulkan Profile Formats

| Profiles | VP_KHR_roadmap_2022 | VP_ANDROID_baseline_2021 | VP_LUNARG_desktop_portability_2021 | VP_LUNARG_desktop_portability_2021_subset |
|----------|---------------------|--------------------------|------------------------------------|-------------------------------------------|
| **VK_FORMAT_A1R5G5B5_UNORM_PACK16** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_A2B10G10R10_UINT_PACK32** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_A2B10G10R10_UNORM_PACK32** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_A8B8G8R8_SINT_PACK32** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | - | - |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_A8B8G8R8_SNORM_PACK32** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_A8B8G8R8_SRGB_PACK32** |
| bufferFeatures |  | - | - | - |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_A8B8G8R8_UINT_PACK32** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | - | - |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_A8B8G8R8_UNORM_PACK32** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_ASTC_10x10_SRGB_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_10x10_UNORM_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_10x5_SRGB_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_10x5_UNORM_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_10x6_SRGB_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_10x6_UNORM_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_10x8_SRGB_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_10x8_UNORM_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_12x10_SRGB_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_12x10_UNORM_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_12x12_SRGB_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_12x12_UNORM_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_4x4_SRGB_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_4x4_UNORM_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_5x4_SRGB_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_5x4_UNORM_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_5x5_SRGB_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_5x5_UNORM_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_6x5_SRGB_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_6x5_UNORM_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_6x6_SRGB_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_6x6_UNORM_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_8x5_SRGB_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_8x5_UNORM_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_8x6_SRGB_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_8x6_UNORM_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_8x8_SRGB_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ASTC_8x8_UNORM_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_B10G11R11_UFLOAT_PACK32** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_B4G4R4A4_UNORM_PACK16** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_B8G8R8A8_SRGB** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_B8G8R8A8_UNORM** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_BC1_RGBA_SRGB_BLOCK** |
| bufferFeatures |  |  | - | - |
| linearTilingFeatures |  |  | - | - |
| optimalTilingFeatures |  |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_BC1_RGBA_UNORM_BLOCK** |
| bufferFeatures |  |  | - | - |
| linearTilingFeatures |  |  | - | - |
| optimalTilingFeatures |  |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_BC1_RGB_SRGB_BLOCK** |
| bufferFeatures |  |  | - | - |
| linearTilingFeatures |  |  | - | - |
| optimalTilingFeatures |  |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_BC1_RGB_UNORM_BLOCK** |
| bufferFeatures |  |  | - | - |
| linearTilingFeatures |  |  | - | - |
| optimalTilingFeatures |  |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_BC2_SRGB_BLOCK** |
| bufferFeatures |  |  | - | - |
| linearTilingFeatures |  |  | - | - |
| optimalTilingFeatures |  |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_BC2_UNORM_BLOCK** |
| bufferFeatures |  |  | - | - |
| linearTilingFeatures |  |  | - | - |
| optimalTilingFeatures |  |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_BC3_SRGB_BLOCK** |
| bufferFeatures |  |  | - | - |
| linearTilingFeatures |  |  | - | - |
| optimalTilingFeatures |  |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_BC3_UNORM_BLOCK** |
| bufferFeatures |  |  | - | - |
| linearTilingFeatures |  |  | - | - |
| optimalTilingFeatures |  |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_BC4_SNORM_BLOCK** |
| bufferFeatures |  |  | - | - |
| linearTilingFeatures |  |  | - | - |
| optimalTilingFeatures |  |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_BC4_UNORM_BLOCK** |
| bufferFeatures |  |  | - | - |
| linearTilingFeatures |  |  | - | - |
| optimalTilingFeatures |  |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_BC5_SNORM_BLOCK** |
| bufferFeatures |  |  | - | - |
| linearTilingFeatures |  |  | - | - |
| optimalTilingFeatures |  |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_BC5_UNORM_BLOCK** |
| bufferFeatures |  |  | - | - |
| linearTilingFeatures |  |  | - | - |
| optimalTilingFeatures |  |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_BC6H_SFLOAT_BLOCK** |
| bufferFeatures |  |  | - | - |
| linearTilingFeatures |  |  | - | - |
| optimalTilingFeatures |  |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_BC6H_UFLOAT_BLOCK** |
| bufferFeatures |  |  | - | - |
| linearTilingFeatures |  |  | - | - |
| optimalTilingFeatures |  |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_BC7_SRGB_BLOCK** |
| bufferFeatures |  |  | - | - |
| linearTilingFeatures |  |  | - | - |
| optimalTilingFeatures |  |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_BC7_UNORM_BLOCK** |
| bufferFeatures |  |  | - | - |
| linearTilingFeatures |  |  | - | - |
| optimalTilingFeatures |  |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_D16_UNORM** |
| bufferFeatures |  | - | - | - |
| linearTilingFeatures |  | - | - | - |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_D32_SFLOAT** |
| bufferFeatures |  | - | - | - |
| linearTilingFeatures |  | - | - | - |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_D32_SFLOAT_S8_UINT** |
| bufferFeatures |  |  | - | - |
| linearTilingFeatures |  |  | - | - |
| optimalTilingFeatures |  |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_E5B9G9R9_UFLOAT_PACK32** |
| bufferFeatures |  | - | - | - |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_EAC_R11G11_SNORM_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_EAC_R11G11_UNORM_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_EAC_R11_SNORM_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_EAC_R11_UNORM_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_R16G16B16A16_SFLOAT** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | - | - |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) |
| **VK_FORMAT_R16G16B16A16_SINT** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R16G16B16A16_SNORM** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | - | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | - | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R16G16B16A16_UINT** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R16G16B16A16_UNORM** |
| bufferFeatures |  |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R16G16_SFLOAT** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R16G16_SINT** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R16G16_SNORM** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | - | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | - | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R16G16_UINT** |
| bufferFeatures |  | - | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R16G16_UNORM** |
| bufferFeatures |  |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R16_SFLOAT** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R16_SINT** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R16_SNORM** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | - | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | - | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R16_UINT** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R16_UNORM** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | - | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | - | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R32G32B32A32_SFLOAT** |
| bufferFeatures |  | - | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R32G32B32A32_SINT** |
| bufferFeatures |  | - | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R32G32B32A32_UINT** |
| bufferFeatures |  | - | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R32G32_SFLOAT** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R32G32_SINT** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R32G32_UINT** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R32_SFLOAT** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R32_SINT** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R32_UINT** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R5G6B5_UNORM_PACK16** |
| bufferFeatures |  | - |  |  |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |  |  |
| **VK_FORMAT_R8G8B8A8_SINT** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R8G8B8A8_SNORM** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R8G8B8A8_SRGB** |
| bufferFeatures |  | - | - | - |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R8G8B8A8_UINT** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R8G8B8A8_UNORM** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R8G8_SINT** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R8G8_SNORM** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R8G8_UINT** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R8G8_UNORM** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R8_SINT** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R8_SNORM** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R8_UINT** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| **VK_FORMAT_R8_UNORM** |
| bufferFeatures |  | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) | (VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT \| VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) |
| linearTilingFeatures |  | (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
| optimalTilingFeatures |  | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) | (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT \| VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT \| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT \| VK_FORMAT_FEATURE_BLIT_SRC_BIT \| VK_FORMAT_FEATURE_BLIT_DST_BIT \| VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT \| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT \| VK_FORMAT_FEATURE_TRANSFER_DST_BIT) |
