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



