VP_INLINE VkResult vpGetProfiles(uint32_t *pPropertyCount, VpProfileProperties *pProperties) {
    static const VpProfileProperties table[] = {
        {VP_KHR_ROADMAP_2022_NAME, VP_KHR_ROADMAP_2022_SPEC_VERSION},
#ifdef VK_ENABLE_BETA_EXTENSIONS
        {VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME, VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_SPEC_VERSION},
#endif  // VK_ENABLE_BETA_EXTENSIONS
        {VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, VP_LUNARG_DESKTOP_PORTABILITY_2021_SPEC_VERSION}};

    if (pProperties == nullptr) {
        *pPropertyCount = _vpCountOf(table);
        return VK_SUCCESS;
    }

    *pPropertyCount = std::min<uint32_t>(_vpCountOf(table), *pPropertyCount);
    for (std::size_t i = 0, n = *pPropertyCount; i < n; ++i) {
        pProperties[i] = table[i];
    }

    return _vpCountOf(table) <= *pPropertyCount ? VK_SUCCESS : VK_INCOMPLETE;
}

VP_INLINE VkResult vpGetProfileFallbacks(const VpProfileProperties *pProfile, uint32_t *pPropertyCount,
                                         VpProfileProperties *pProperties) {
    static const VpProfileProperties LUNARG_desktop_portability_2021_subset_fallbacks[] = {
        {VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, VP_LUNARG_DESKTOP_PORTABILITY_2021_SPEC_VERSION}};

    if (pProperties == nullptr) {
#ifdef VK_ENABLE_BETA_EXTENSIONS
        if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME) == 0) {
            *pPropertyCount = _vpCountOf(LUNARG_desktop_portability_2021_subset_fallbacks);
        } else {
#else
        {
#endif  // VK_ENABLE_BETA_EXTENSIONS
            *pPropertyCount = 0;
        }

        return VK_SUCCESS;
    }

#ifdef VK_ENABLE_BETA_EXTENSIONS
    if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME) == 0) {
        *pPropertyCount = std::min<uint32_t>(_vpCountOf(LUNARG_desktop_portability_2021_subset_fallbacks), *pPropertyCount);
        for (uint32_t i = 0, n = *pPropertyCount; i < n; ++i) {
            pProperties[i] = LUNARG_desktop_portability_2021_subset_fallbacks[i];
        }
    }

    return _vpCountOf(LUNARG_desktop_portability_2021_subset_fallbacks) <= *pPropertyCount ? VK_SUCCESS : VK_INCOMPLETE;
#else
    return VK_SUCCESS;
#endif  // VK_ENABLE_BETA_EXTENSIONS
}

VP_INLINE void vpGetProfileStructures(const VpProfileProperties *pProfile, void *pNext) {
    if (pProfile == nullptr || pNext == nullptr) return;

    struct VkStruct {
        VkStructureType sType;
        void *pNext;
    };

    VkStruct *p = static_cast<VkStruct *>(pNext);

    if (strcmp(pProfile->profileName, VP_KHR_ROADMAP_2022_NAME) == 0) {
        while (p != nullptr) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2: {
                    VkPhysicalDeviceFeatures2 *features = (VkPhysicalDeviceFeatures2 *)p;
                    features->features.robustBufferAccess = VK_TRUE;
                    features->features.fullDrawIndexUint32 = VK_TRUE;
                    features->features.imageCubeArray = VK_TRUE;
                    features->features.independentBlend = VK_TRUE;
                    features->features.sampleRateShading = VK_TRUE;
                    features->features.drawIndirectFirstInstance = VK_TRUE;
                    features->features.depthClamp = VK_TRUE;
                    features->features.depthBiasClamp = VK_TRUE;
                    features->features.samplerAnisotropy = VK_TRUE;
                    features->features.occlusionQueryPrecise = VK_TRUE;
                    features->features.fragmentStoresAndAtomics = VK_TRUE;
                    features->features.shaderStorageImageExtendedFormats = VK_TRUE;
                    features->features.shaderUniformBufferArrayDynamicIndexing = VK_TRUE;
                    features->features.shaderSampledImageArrayDynamicIndexing = VK_TRUE;
                    features->features.shaderStorageBufferArrayDynamicIndexing = VK_TRUE;
                    features->features.shaderStorageImageArrayDynamicIndexing = VK_TRUE;
                } break;
#ifdef VK_VERSION_1_2
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES: {
                    VkPhysicalDeviceVulkan11Features *features = (VkPhysicalDeviceVulkan11Features *)p;
                    features->multiview = VK_TRUE;
                    features->samplerYcbcrConversion = VK_TRUE;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES: {
                    VkPhysicalDeviceVulkan12Features *features = (VkPhysicalDeviceVulkan12Features *)p;
                    features->uniformBufferStandardLayout = VK_TRUE;
                    features->subgroupBroadcastDynamicId = VK_TRUE;
                    features->imagelessFramebuffer = VK_TRUE;
                    features->separateDepthStencilLayouts = VK_TRUE;
                    features->hostQueryReset = VK_TRUE;
                    features->timelineSemaphore = VK_TRUE;
                    features->shaderSubgroupExtendedTypes = VK_TRUE;
                    features->vulkanMemoryModel = VK_TRUE;
                    features->vulkanMemoryModelDeviceScope = VK_TRUE;
                    features->vulkanMemoryModelAvailabilityVisibilityChains = VK_TRUE;
                    features->bufferDeviceAddress = VK_TRUE;
                    features->samplerMirrorClampToEdge = VK_TRUE;
                    features->descriptorIndexing = VK_TRUE;
                    features->shaderUniformTexelBufferArrayDynamicIndexing = VK_TRUE;
                    features->shaderUniformTexelBufferArrayDynamicIndexing = VK_TRUE;
                    features->shaderStorageTexelBufferArrayDynamicIndexing = VK_TRUE;
                    features->shaderUniformBufferArrayNonUniformIndexing = VK_TRUE;
                    features->shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
                    features->shaderStorageBufferArrayNonUniformIndexing = VK_TRUE;
                    features->shaderStorageImageArrayNonUniformIndexing = VK_TRUE;
                    features->shaderUniformTexelBufferArrayNonUniformIndexing = VK_TRUE;
                    features->shaderStorageTexelBufferArrayNonUniformIndexing = VK_TRUE;
                    features->descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE;
                    features->descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
                    features->descriptorBindingStorageImageUpdateAfterBind = VK_TRUE;
                    features->descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE;
                    features->descriptorBindingUniformTexelBufferUpdateAfterBind = VK_TRUE;
                    features->descriptorBindingStorageTexelBufferUpdateAfterBind = VK_TRUE;
                    features->descriptorBindingUpdateUnusedWhilePending = VK_TRUE;
                    features->descriptorBindingPartiallyBound = VK_TRUE;
                    features->descriptorBindingVariableDescriptorCount = VK_TRUE;
                    features->runtimeDescriptorArray = VK_TRUE;
                    features->scalarBlockLayout = VK_TRUE;
                } break;
#endif  // VK_VERSION_1_2
#ifdef VK_VERSION_1_3
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES: {
                    VkPhysicalDeviceVulkan13Features *features = (VkPhysicalDeviceVulkan13Features *)p;
                    features->robustImageAccess = VK_TRUE;
                    features->shaderTerminateInvocation = VK_TRUE;
                    features->shaderZeroInitializeWorkgroupMemory = VK_TRUE;
                    features->synchronization2 = VK_TRUE;
                    features->shaderIntegerDotProduct = VK_TRUE;
                    features->maintenance4 = VK_TRUE;
                    features->pipelineCreationCacheControl = VK_TRUE;
                    features->subgroupSizeControl = VK_TRUE;
                    features->computeFullSubgroups = VK_TRUE;
                    features->shaderDemoteToHelperInvocation = VK_TRUE;
                    features->inlineUniformBlock = VK_TRUE;
                    features->descriptorBindingInlineUniformBlockUpdateAfterBind = VK_TRUE;
                } break;
#endif  // VK_VERSION_1_3
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2: {
                    VkPhysicalDeviceProperties2 *properties = (VkPhysicalDeviceProperties2 *)p;
                    properties->properties.limits.maxImageDimension1D = 8192;
                    properties->properties.limits.maxImageDimension2D = 8192;
                    properties->properties.limits.maxImageDimensionCube = 8192;
                    properties->properties.limits.maxImageArrayLayers = 2048;
                    properties->properties.limits.maxUniformBufferRange = 65536;
                    properties->properties.limits.bufferImageGranularity = 4096;
                    properties->properties.limits.maxPerStageDescriptorSamplers = 64;
                    properties->properties.limits.maxPerStageDescriptorUniformBuffers = 15;
                    properties->properties.limits.maxPerStageDescriptorStorageBuffers = 30;
                    properties->properties.limits.maxPerStageDescriptorSampledImages = 200;
                    properties->properties.limits.maxPerStageDescriptorStorageImages = 16;
                    properties->properties.limits.maxPerStageResources = 200;
                    properties->properties.limits.maxDescriptorSetSamplers = 576;
                    properties->properties.limits.maxDescriptorSetUniformBuffers = 90;
                    properties->properties.limits.maxDescriptorSetStorageBuffers = 96;
                    properties->properties.limits.maxDescriptorSetSampledImages = 1800;
                    properties->properties.limits.maxDescriptorSetStorageImages = 144;
                    properties->properties.limits.maxFragmentCombinedOutputResources = 16;
                    properties->properties.limits.maxComputeWorkGroupInvocations = 256;
                    properties->properties.limits.maxComputeWorkGroupSize[0] = 256;
                    properties->properties.limits.maxComputeWorkGroupSize[1] = 256;
                    properties->properties.limits.maxComputeWorkGroupSize[2] = 64;
                    properties->properties.limits.subTexelPrecisionBits = 8;
                    properties->properties.limits.mipmapPrecisionBits = 6;
                    properties->properties.limits.maxSamplerLodBias = 14.0;
                    properties->properties.limits.pointSizeGranularity = 0.125;
                    properties->properties.limits.lineWidthGranularity = 0.5;
                    properties->properties.limits.standardSampleLocations = VK_TRUE;
                    properties->properties.limits.maxColorAttachments = 7;
                } break;
#ifdef VK_VERSION_1_2
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES: {
                    VkPhysicalDeviceVulkan11Properties *properties = (VkPhysicalDeviceVulkan11Properties *)p;
                    properties->subgroupSize = 4;
                    properties->subgroupSupportedStages = VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
                    properties->subgroupSupportedOperations =
                        VK_SUBGROUP_FEATURE_BASIC_BIT | VK_SUBGROUP_FEATURE_VOTE_BIT | VK_SUBGROUP_FEATURE_ARITHMETIC_BIT |
                        VK_SUBGROUP_FEATURE_BALLOT_BIT | VK_SUBGROUP_FEATURE_SHUFFLE_BIT |
                        VK_SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT | VK_SUBGROUP_FEATURE_QUAD_BIT;
                } break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES: {
                    VkPhysicalDeviceVulkan12Properties *properties = (VkPhysicalDeviceVulkan12Properties *)p;
                    properties->shaderSignedZeroInfNanPreserveFloat16 = VK_TRUE;
                    properties->shaderSignedZeroInfNanPreserveFloat32 = VK_TRUE;
                    properties->maxPerStageDescriptorUpdateAfterBindSamplers = 500000;
                    properties->maxPerStageDescriptorUpdateAfterBindUniformBuffers = 12;
                    properties->maxPerStageDescriptorUpdateAfterBindStorageBuffers = 500000;
                    properties->maxPerStageDescriptorUpdateAfterBindSampledImages = 500000;
                    properties->maxPerStageDescriptorUpdateAfterBindStorageImages = 500000;
                    properties->maxPerStageDescriptorUpdateAfterBindInputAttachments = 7;
                    properties->maxPerStageUpdateAfterBindResources = 500000;
                    properties->maxDescriptorSetUpdateAfterBindSamplers = 500000;
                    properties->maxDescriptorSetUpdateAfterBindUniformBuffers = 72;
                    properties->maxDescriptorSetUpdateAfterBindUniformBuffersDynamic = 8;
                    properties->maxDescriptorSetUpdateAfterBindStorageBuffers = 500000;
                    properties->maxDescriptorSetUpdateAfterBindStorageBuffersDynamic = 4;
                    properties->maxDescriptorSetUpdateAfterBindSampledImages = 500000;
                    properties->maxDescriptorSetUpdateAfterBindStorageImages = 500000;
                    properties->maxDescriptorSetUpdateAfterBindInputAttachments = 7;
                } break;
#endif  // VK_VERSION_1_2
#ifdef VK_VERSION_1_3
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES: {
                    VkPhysicalDeviceVulkan13Properties *properties = (VkPhysicalDeviceVulkan13Properties *)p;
                    properties->maxBufferSize = 1073741824;
                    properties->maxInlineUniformBlockSize = 256;
                    properties->maxPerStageDescriptorInlineUniformBlocks = 4;
                    properties->maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks = 4;
                    properties->maxDescriptorSetInlineUniformBlocks = 4;
                    properties->maxDescriptorSetUpdateAfterBindInlineUniformBlocks = 4;
                    properties->maxInlineUniformTotalSize = 256;
                } break;
#endif  // VK_VERSION_1_3
                default:
                    break;
            }
            p = static_cast<VkStruct *>(p->pNext);
        }
    } else if (strcmp(VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, pProfile->profileName) == 0
#ifdef VK_ENABLE_BETA_EXTENSIONS
               || strcmp(VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME, pProfile->profileName) == 0
#endif
    ) {
        while (p != nullptr) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2: {
                    VkPhysicalDeviceFeatures2 *features = (VkPhysicalDeviceFeatures2 *)p;
                    features->features.depthBiasClamp = VK_TRUE;
                    features->features.depthClamp = VK_TRUE;
                    features->features.drawIndirectFirstInstance = VK_TRUE;
                    features->features.dualSrcBlend = VK_TRUE;
                    features->features.fillModeNonSolid = VK_TRUE;
                    features->features.fragmentStoresAndAtomics = VK_TRUE;
                    features->features.fullDrawIndexUint32 = VK_TRUE;
                    features->features.imageCubeArray = VK_TRUE;
                    features->features.independentBlend = VK_TRUE;
                    features->features.inheritedQueries = VK_TRUE;
                    features->features.largePoints = VK_TRUE;
                    features->features.multiDrawIndirect = VK_TRUE;
                    features->features.multiViewport = VK_TRUE;
                    features->features.occlusionQueryPrecise = VK_TRUE;
                    features->features.robustBufferAccess = VK_TRUE;
                    features->features.sampleRateShading = VK_TRUE;
                    features->features.samplerAnisotropy = VK_TRUE;
                    features->features.shaderClipDistance = VK_TRUE;
                    features->features.shaderImageGatherExtended = VK_TRUE;
                    features->features.shaderSampledImageArrayDynamicIndexing = VK_TRUE;
                    features->features.shaderStorageBufferArrayDynamicIndexing = VK_TRUE;
                    features->features.shaderStorageImageArrayDynamicIndexing = VK_TRUE;
                    features->features.shaderStorageImageExtendedFormats = VK_TRUE;
                    features->features.shaderStorageImageWriteWithoutFormat = VK_TRUE;
                    features->features.shaderTessellationAndGeometryPointSize = VK_TRUE;
                    features->features.shaderUniformBufferArrayDynamicIndexing = VK_TRUE;
                    features->features.tessellationShader = VK_TRUE;
                    features->features.textureCompressionBC = VK_TRUE;
                    features->features.vertexPipelineStoresAndAtomics = VK_TRUE;
                } break;

#ifdef VK_KHR_multiview
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR: {
                    VkPhysicalDeviceMultiviewFeaturesKHR *features = (VkPhysicalDeviceMultiviewFeaturesKHR *)p;
                    features->multiview = VK_TRUE;
                } break;
#endif  // VK_KHR_multiview

#ifdef VK_KHR_imageless_framebuffer
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR: {
                    VkPhysicalDeviceImagelessFramebufferFeaturesKHR *features =
                        (VkPhysicalDeviceImagelessFramebufferFeaturesKHR *)p;
                    features->imagelessFramebuffer = VK_TRUE;
                } break;
#endif  // VK_KHR_imageless_framebuffer

#ifdef VK_KHR_16bit_storage
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES_KHR: {
                    VkPhysicalDevice16BitStorageFeaturesKHR *features = (VkPhysicalDevice16BitStorageFeaturesKHR *)p;
                    features->storageBuffer16BitAccess = VK_TRUE;
                    features->uniformAndStorageBuffer16BitAccess = VK_TRUE;
                } break;
#endif  // VK_KHR_16bit_storage

#ifdef VK_KHR_8bit_storage
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR: {
                    VkPhysicalDevice8BitStorageFeaturesKHR *features = (VkPhysicalDevice8BitStorageFeaturesKHR *)p;
                    features->storageBuffer8BitAccess = VK_TRUE;
                    features->storagePushConstant8 = VK_TRUE;
                    features->uniformAndStorageBuffer8BitAccess = VK_TRUE;
                } break;
#endif  // VK_KHR_8bit_storage

#ifdef VK_EXT_descriptor_indexing
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT: {
                    VkPhysicalDeviceDescriptorIndexingFeaturesEXT *features = (VkPhysicalDeviceDescriptorIndexingFeaturesEXT *)p;
                    features->shaderUniformTexelBufferArrayDynamicIndexing = VK_TRUE;
                    features->shaderStorageTexelBufferArrayDynamicIndexing = VK_TRUE;
                    features->shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
                    features->shaderStorageBufferArrayNonUniformIndexing = VK_TRUE;
                    features->shaderUniformTexelBufferArrayNonUniformIndexing = VK_TRUE;
                    features->descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
                    features->descriptorBindingStorageImageUpdateAfterBind = VK_TRUE;
                    features->descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE;
                    features->descriptorBindingUniformTexelBufferUpdateAfterBind = VK_TRUE;
                    features->descriptorBindingStorageTexelBufferUpdateAfterBind = VK_TRUE;
                    features->descriptorBindingUpdateUnusedWhilePending = VK_TRUE;
                    features->descriptorBindingPartiallyBound = VK_TRUE;
                    features->descriptorBindingVariableDescriptorCount = VK_TRUE;
                    features->runtimeDescriptorArray = VK_TRUE;
                } break;
#endif  // VK_EXT_descriptor_indexing

#ifdef VK_EXT_host_query_reset
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT: {
                    VkPhysicalDeviceHostQueryResetFeaturesEXT *features = (VkPhysicalDeviceHostQueryResetFeaturesEXT *)p;
                    features->hostQueryReset = VK_TRUE;
                } break;
#endif  // VK_EXT_host_query_reset

#ifdef VK_KHR_shader_subgroup_extended_types
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR: {
                    VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR *features =
                        (VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR *)p;
                    features->shaderSubgroupExtendedTypes = VK_TRUE;
                } break;
#endif  // VK_KHR_shader_subgroup_extended_types

#ifdef VK_KHR_uniform_buffer_standard_layout
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR: {
                    VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR *features =
                        (VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR *)p;
                    features->uniformBufferStandardLayout = VK_TRUE;
                } break;
#endif  // VK_KHR_uniform_buffer_standard_layout

#ifdef VK_KHR_shader_draw_parameters
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES: {
                    VkPhysicalDeviceShaderDrawParametersFeatures *features = (VkPhysicalDeviceShaderDrawParametersFeatures *)p;
                    features->shaderDrawParameters = VK_TRUE;
                } break;
#endif  // VK_KHR_shader_draw_parameters

#ifdef VK_KHR_shader_float16_int8
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR: {
                    VkPhysicalDeviceShaderFloat16Int8FeaturesKHR *features = (VkPhysicalDeviceShaderFloat16Int8FeaturesKHR *)p;
                    features->shaderInt8 = VK_TRUE;
                } break;
#endif  // VK_KHR_shader_float16_int8

#ifdef VK_KHR_sampler_ycbcr_conversion
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR: {
                    VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR *features =
                        (VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR *)p;
                    features->samplerYcbcrConversion = VK_TRUE;
                } break;
#endif  // VK_KHR_sampler_ycbcr_conversion

#ifdef VK_KHR_variable_pointers
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES_KHR: {
                    VkPhysicalDeviceVariablePointersFeaturesKHR *features = (VkPhysicalDeviceVariablePointersFeaturesKHR *)p;
                    features->variablePointersStorageBuffer = VK_TRUE;
                    features->variablePointers = VK_TRUE;
                } break;
#endif  // VK_KHR_variable_pointers

#ifdef VK_KHR_portability_subset
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR: {
                    VkPhysicalDevicePortabilitySubsetFeaturesKHR *features = (VkPhysicalDevicePortabilitySubsetFeaturesKHR *)p;
                    features->vertexAttributeAccessBeyondStride = VK_TRUE;
                    features->separateStencilMaskRef = VK_TRUE;
                    features->mutableComparisonSamplers = VK_TRUE;
                    features->multisampleArrayImage = VK_TRUE;
                    features->imageViewFormatSwizzle = VK_TRUE;
                    features->imageViewFormatReinterpretation = VK_TRUE;
                    features->events = VK_TRUE;
                    features->constantAlphaColorBlendFactors = VK_TRUE;
                } break;
#endif  // VK_KHR_portability_subset

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2: {
                    VkPhysicalDeviceProperties2 *properties = (VkPhysicalDeviceProperties2 *)p;
                    properties->properties.limits.maxImageDimension1D = 16384;
                    properties->properties.limits.maxImageDimension2D = 16384;
                    properties->properties.limits.maxImageDimension3D = 2048;
                    properties->properties.limits.maxImageDimensionCube = 16384;
                    properties->properties.limits.maxImageArrayLayers = 2048;
                    properties->properties.limits.maxTexelBufferElements = 67108900;
                    properties->properties.limits.maxUniformBufferRange = 65536;
                    properties->properties.limits.maxStorageBufferRange = 134217728;
                    properties->properties.limits.maxPushConstantsSize = 128;
                    properties->properties.limits.maxMemoryAllocationCount = 4096;
                    properties->properties.limits.maxSamplerAllocationCount = 1024;
                    properties->properties.limits.bufferImageGranularity = 1024;
                    properties->properties.limits.sparseAddressSpaceSize = 1073741824;
                    properties->properties.limits.maxBoundDescriptorSets = 8;
                    properties->properties.limits.maxPerStageDescriptorSamplers = 16;
                    properties->properties.limits.maxPerStageDescriptorUniformBuffers = 15;
                    properties->properties.limits.maxPerStageDescriptorStorageBuffers = 16;
                    properties->properties.limits.maxPerStageDescriptorSampledImages = 128;
                    properties->properties.limits.maxPerStageDescriptorStorageImages = 8;
                    properties->properties.limits.maxPerStageDescriptorInputAttachments = 8;
                    properties->properties.limits.maxPerStageResources = 128;
                    properties->properties.limits.maxDescriptorSetSamplers = 80;
                    properties->properties.limits.maxDescriptorSetUniformBuffers = 90;
                    properties->properties.limits.maxDescriptorSetUniformBuffersDynamic = 8;
                    properties->properties.limits.maxDescriptorSetStorageBuffers = 155;
                    properties->properties.limits.maxDescriptorSetStorageBuffersDynamic = 8;
                    properties->properties.limits.maxDescriptorSetSampledImages = 256;
                    properties->properties.limits.maxDescriptorSetStorageImages = 40;
                    properties->properties.limits.maxDescriptorSetInputAttachments = 8;
                    properties->properties.limits.maxVertexInputAttributes = 28;
                    properties->properties.limits.maxVertexInputBindings = 28;
                    properties->properties.limits.maxVertexInputAttributeOffset = 2047;
                    properties->properties.limits.maxVertexInputBindingStride = 2048;
                    properties->properties.limits.maxVertexOutputComponents = 124;
                    properties->properties.limits.maxTessellationGenerationLevel = 64;
                    properties->properties.limits.maxTessellationPatchSize = 32;
                    properties->properties.limits.maxTessellationControlPerVertexInputComponents = 124;
                    properties->properties.limits.maxTessellationControlPerVertexOutputComponents = 124;
                    properties->properties.limits.maxTessellationControlPerPatchOutputComponents = 120;
                    properties->properties.limits.maxTessellationControlTotalOutputComponents = 2048;
                    properties->properties.limits.maxTessellationEvaluationInputComponents = 124;
                    properties->properties.limits.maxTessellationEvaluationOutputComponents = 124;
                    properties->properties.limits.maxFragmentInputComponents = 116;
                    properties->properties.limits.maxFragmentOutputAttachments = 8;
                    properties->properties.limits.maxFragmentDualSrcAttachments = 1;
                    properties->properties.limits.maxFragmentCombinedOutputResources = 8;
                    properties->properties.limits.maxComputeSharedMemorySize = 32768;
                    properties->properties.limits.maxComputeWorkGroupCount[0] = 65535;
                    properties->properties.limits.maxComputeWorkGroupCount[1] = 65535;
                    properties->properties.limits.maxComputeWorkGroupCount[2] = 65535;
                    properties->properties.limits.maxComputeWorkGroupInvocations = 1024;
                    properties->properties.limits.maxComputeWorkGroupSize[0] = 1024;
                    properties->properties.limits.maxComputeWorkGroupSize[1] = 1024;
                    properties->properties.limits.maxComputeWorkGroupSize[2] = 64;
                    properties->properties.limits.subPixelPrecisionBits = 4;
                    properties->properties.limits.subTexelPrecisionBits = 4;
                    properties->properties.limits.mipmapPrecisionBits = 4;
                    properties->properties.limits.maxDrawIndexedIndexValue = 4294967295u;
                    properties->properties.limits.maxDrawIndirectCount = 1073740000;
                    properties->properties.limits.maxSamplerLodBias = 14;
                    properties->properties.limits.maxSamplerAnisotropy = 16;
                    properties->properties.limits.maxViewports = 16;
                    properties->properties.limits.maxViewportDimensions[0] = 16384;
                    properties->properties.limits.maxViewportDimensions[1] = 16384;
                    properties->properties.limits.viewportBoundsRange[0] = -32768;
                    properties->properties.limits.viewportBoundsRange[1] = 32767;
                    properties->properties.limits.minMemoryMapAlignment = 64;
                    properties->properties.limits.minTexelBufferOffsetAlignment = 64;
                    properties->properties.limits.minUniformBufferOffsetAlignment = 256;
                    properties->properties.limits.minStorageBufferOffsetAlignment = 64;
                    properties->properties.limits.minTexelOffset = -8;
                    properties->properties.limits.maxTexelOffset = 7;
                    properties->properties.limits.minTexelGatherOffset = -8;
                    properties->properties.limits.maxTexelGatherOffset = 7;
                    properties->properties.limits.minInterpolationOffset = -0.5;
                    properties->properties.limits.maxInterpolationOffset = 0.4375;
                    properties->properties.limits.subPixelInterpolationOffsetBits = 4;
                    properties->properties.limits.maxFramebufferWidth = 16384;
                    properties->properties.limits.maxFramebufferHeight = 16384;
                    properties->properties.limits.maxFramebufferLayers = 1024;
                    properties->properties.limits.framebufferColorSampleCounts = 9;
                    properties->properties.limits.framebufferDepthSampleCounts = 9;
                    properties->properties.limits.framebufferStencilSampleCounts = 9;
                    properties->properties.limits.framebufferNoAttachmentsSampleCounts = 9;
                    properties->properties.limits.maxColorAttachments = 8;
                    properties->properties.limits.sampledImageColorSampleCounts = 9;
                    properties->properties.limits.sampledImageIntegerSampleCounts = 9;
                    properties->properties.limits.sampledImageDepthSampleCounts = 9;
                    properties->properties.limits.sampledImageStencilSampleCounts = 9;
                    properties->properties.limits.storageImageSampleCounts = 1;
                    properties->properties.limits.maxSampleMaskWords = 1;
                    properties->properties.limits.maxClipDistances = 8;
                    properties->properties.limits.maxCullDistances = 8;
                    properties->properties.limits.maxCombinedClipAndCullDistances = 8;
                    properties->properties.limits.discreteQueuePriorities = 2;
                    properties->properties.limits.pointSizeRange[0] = 1.0;
                    properties->properties.limits.pointSizeRange[1] = 64.0;
                    properties->properties.limits.lineWidthRange[0] = 1.0;
                    properties->properties.limits.lineWidthRange[1] = 1.0;
                    properties->properties.limits.pointSizeGranularity = 0.125;
                    properties->properties.limits.lineWidthGranularity = 0.5;
                } break;

#ifdef VK_KHR_multiview
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR: {
                    VkPhysicalDeviceMultiviewPropertiesKHR *properties = (VkPhysicalDeviceMultiviewPropertiesKHR *)p;
                    properties->maxMultiviewInstanceIndex = 134217727;
                    properties->maxMultiviewViewCount = 6;
                } break;
#endif  // VK_KHR_multiview

#ifdef VK_EXT_descriptor_indexing
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT: {
                    VkPhysicalDeviceDescriptorIndexingPropertiesEXT *properties =
                        (VkPhysicalDeviceDescriptorIndexingPropertiesEXT *)p;
                    properties->maxUpdateAfterBindDescriptorsInAllPools = 1048576;
                    properties->maxPerStageDescriptorUpdateAfterBindSamplers = 16;
                    properties->maxPerStageDescriptorUpdateAfterBindUniformBuffers = 15;
                    properties->maxPerStageDescriptorUpdateAfterBindStorageBuffers = 31;
                    properties->maxPerStageDescriptorUpdateAfterBindSampledImages = 128;
                    properties->maxPerStageDescriptorUpdateAfterBindStorageImages = 8;
                    properties->maxPerStageDescriptorUpdateAfterBindInputAttachments = 128;
                    properties->maxPerStageUpdateAfterBindResources = 159;
                    properties->maxDescriptorSetUpdateAfterBindSamplers = 80;
                    properties->maxDescriptorSetUpdateAfterBindUniformBuffers = 90;
                    properties->maxDescriptorSetUpdateAfterBindUniformBuffersDynamic = 8;
                    properties->maxDescriptorSetUpdateAfterBindStorageBuffers = 155;
                    properties->maxDescriptorSetUpdateAfterBindStorageBuffersDynamic = 8;
                    properties->maxDescriptorSetUpdateAfterBindSampledImages = 640;
                    properties->maxDescriptorSetUpdateAfterBindStorageImages = 40;
                    properties->maxDescriptorSetUpdateAfterBindInputAttachments = 8;
                } break;
#endif  // VK_EXT_descriptor_indexing

#ifdef VK_KHR_depth_stencil_resolve
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR: {
                    VkPhysicalDeviceDepthStencilResolvePropertiesKHR *properties =
                        (VkPhysicalDeviceDepthStencilResolvePropertiesKHR *)p;
                    properties->independentResolve = VK_TRUE;
                    properties->independentResolveNone = VK_TRUE;
                    properties->supportedDepthResolveModes = VK_RESOLVE_MODE_SAMPLE_ZERO_BIT;
                    properties->supportedStencilResolveModes = VK_RESOLVE_MODE_SAMPLE_ZERO_BIT;
                } break;
#endif  // VK_KHR_depth_stencil_resolve

#ifdef VK_KHR_maintenance3
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES_KHR: {
                    VkPhysicalDeviceMaintenance3PropertiesKHR *properties = (VkPhysicalDeviceMaintenance3PropertiesKHR *)p;
                    properties->maxPerSetDescriptors = 700;
                    properties->maxMemoryAllocationSize = 2147483648;

                } break;
#endif  // VK_KHR_maintenance3

#ifdef VK_KHR_portability_subset
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES_KHR: {
                    if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME) == 0) {
                        VkPhysicalDevicePortabilitySubsetPropertiesKHR *properties =
                            (VkPhysicalDevicePortabilitySubsetPropertiesKHR *)p;
                        properties->minVertexInputBindingStrideAlignment = 4;
                    }
                } break;
#endif  // VK_KHR_portability_subset

                default:
                    break;
            }
            p = static_cast<VkStruct *>(p->pNext);
        }
    }
}

VP_INLINE VkResult vpCreateDevice(VkPhysicalDevice physicalDevice, const VpDeviceCreateInfo *pCreateInfo,
                                  const VkAllocationCallbacks *pAllocator, VkDevice *pDevice) {
    assert(pCreateInfo != nullptr);

    if (physicalDevice == VK_NULL_HANDLE || pCreateInfo == nullptr || pDevice == nullptr) {
        // Ensure the validation layer
        return vkCreateDevice(physicalDevice, pCreateInfo == nullptr ? nullptr : pCreateInfo->pCreateInfo, pAllocator, pDevice);
    } else if (strcmp(pCreateInfo->pProfile->profileName, "") == 0) {
        return vkCreateDevice(physicalDevice, pCreateInfo->pCreateInfo, pAllocator, pDevice);
    } else if (strcmp(VP_KHR_ROADMAP_2022_NAME, pCreateInfo->pProfile->profileName) == 0) {
        std::vector<const char *> extensions;
        _vpGetExtensions(pCreateInfo, _vpCountOf(_VP_KHR_ROADMAP_2022_EXTENSIONS), &_VP_KHR_ROADMAP_2022_EXTENSIONS[0], extensions);

        void *pProfileNext = nullptr;
        void *pRoot = const_cast<void *>(pCreateInfo->pCreateInfo->pNext);

        const VkPhysicalDeviceFeatures2 *requestedFeatures2 =
            (const VkPhysicalDeviceFeatures2 *)_vpGetStructure(pRoot, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2);

#ifdef VK_VERSION_1_2
        const VkPhysicalDeviceVulkan11Features *requestedVulkan11Features =
            (const VkPhysicalDeviceVulkan11Features *)_vpGetStructure(pRoot, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES);

        const VkPhysicalDeviceVulkan12Features *requestedVulkan12Features =
            (const VkPhysicalDeviceVulkan12Features *)_vpGetStructure(pRoot, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES);
#endif  // VK_VERSION_1_2

#ifdef VK_VERSION_1_3
        const VkPhysicalDeviceVulkan13Features *requestedVulkan13Features =
            (const VkPhysicalDeviceVulkan13Features *)_vpGetStructure(pRoot, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES);
#endif  // VK_VERSION_1_3

        VkPhysicalDeviceFeatures2 deviceFeatures2 = {};
        deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        deviceFeatures2.pNext = nullptr;
        pProfileNext = &deviceFeatures2;

#ifdef VK_VERSION_1_2
        VkPhysicalDeviceVulkan11Features deviceVulkan11Features = {};
        deviceVulkan11Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
        deviceVulkan11Features.pNext = pProfileNext;
        pProfileNext = &deviceVulkan11Features;

        VkPhysicalDeviceVulkan12Features deviceVulkan12Features = {};
        deviceVulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        deviceVulkan12Features.pNext = pProfileNext;
        pProfileNext = &deviceVulkan12Features;
#endif  // VK_VERSION_1_2

#ifdef VK_VERSION_1_3
        VkPhysicalDeviceVulkan13Features deviceVulkan13Features = {};
        deviceVulkan13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        deviceVulkan13Features.pNext = pProfileNext;
        pProfileNext = &deviceVulkan13Features;
#endif  // VK_VERSION_1_3

        vpGetProfileStructures(pCreateInfo->pProfile, pProfileNext);

        void *pNext = pRoot;

        if (pCreateInfo->pCreateInfo->pEnabledFeatures != nullptr) {
            deviceFeatures2.features = *pCreateInfo->pCreateInfo->pEnabledFeatures;
        }
        if (requestedFeatures2 == nullptr && pCreateInfo->pCreateInfo->pEnabledFeatures == nullptr) {
            deviceFeatures2.pNext = pNext;
            pNext = &deviceFeatures2;
        }
        if (pCreateInfo->flags & VP_DEVICE_CREATE_DISABLE_ROBUST_BUFFER_ACCESS_BIT) {
            deviceFeatures2.features.robustBufferAccess = VK_FALSE;
        }

#ifdef VK_VERSION_1_2
        if (requestedVulkan11Features == nullptr) {
            deviceVulkan11Features.pNext = pNext;
            pNext = &deviceVulkan11Features;
        }

        if (requestedVulkan12Features == nullptr) {
            deviceVulkan12Features.pNext = pNext;
            pNext = &deviceVulkan12Features;
        }
#endif  // VK_VERSION_1_2

#ifdef VK_VERSION_1_3
        if (requestedVulkan13Features == nullptr) {
            deviceVulkan13Features.pNext = pNext;
            pNext = &deviceVulkan13Features;
        }
        if (pCreateInfo->flags & VP_DEVICE_CREATE_DISABLE_ROBUST_IMAGE_ACCESS_BIT) {
            deviceVulkan13Features.robustImageAccess = VK_FALSE;
        }
#endif  // VK_VERSION_1_3

        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pNext = pNext;
        deviceCreateInfo.queueCreateInfoCount = pCreateInfo->pCreateInfo->queueCreateInfoCount;
        deviceCreateInfo.pQueueCreateInfos = pCreateInfo->pCreateInfo->pQueueCreateInfos;
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = static_cast<const char *const *>(extensions.data());
        deviceCreateInfo.pEnabledFeatures =
            pCreateInfo->pCreateInfo->pEnabledFeatures != nullptr ? &deviceFeatures2.features : nullptr;
        return vkCreateDevice(physicalDevice, &deviceCreateInfo, pAllocator, pDevice);
    } else if (strcmp(VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, pCreateInfo->pProfile->profileName) == 0
#ifdef VK_ENABLE_BETA_EXTENSIONS
               || strcmp(VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME, pCreateInfo->pProfile->profileName) == 0
#endif  // VK_ENABLE_BETA_EXTENSIONS
    ) {
        std::vector<const char *> extensions;
#ifdef VK_ENABLE_BETA_EXTENSIONS
        if (strcmp(VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME, pCreateInfo->pProfile->profileName) == 0) {
            _vpGetExtensions(pCreateInfo, _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_EXTENSIONS),
                             &_VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_EXTENSIONS[0], extensions);
        } else
#endif
        {
            _vpGetExtensions(pCreateInfo, _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_EXTENSIONS),
                             &_VP_LUNARG_DESKTOP_PORTABILITY_2021_EXTENSIONS[0], extensions);
        }

        void *pProfileNext = nullptr;
        void *pRoot = const_cast<void *>(pCreateInfo->pCreateInfo->pNext);

        const VkPhysicalDeviceFeatures2 *requestedFeatures2 =
            (const VkPhysicalDeviceFeatures2 *)_vpGetStructure(pRoot, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2);

#ifdef VK_KHR_multiview
        const VkPhysicalDeviceMultiviewFeaturesKHR *requestedMultiviewFeatures =
            (const VkPhysicalDeviceMultiviewFeaturesKHR *)_vpGetStructure(pRoot,
                                                                          VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR);
#endif  // VK_KHR_multiview

#ifdef VK_KHR_imageless_framebuffer
        const VkPhysicalDeviceImagelessFramebufferFeaturesKHR *requestedImagelessFeatures =
            (const VkPhysicalDeviceImagelessFramebufferFeaturesKHR *)_vpGetStructure(
                pRoot, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR);
#endif  // VK_KHR_imageless_framebuffer

#ifdef VK_KHR_16bit_storage
        const VkPhysicalDevice16BitStorageFeatures *requested16BitFeatures =
            (const VkPhysicalDevice16BitStorageFeatures *)_vpGetStructure(
                pRoot, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES_KHR);
#endif  // VK_KHR_16bit_storage

#ifdef VK_KHR_8bit_storage
        const VkPhysicalDevice8BitStorageFeatures *requested8BitStorageFeatures =
            (const VkPhysicalDevice8BitStorageFeatures *)_vpGetStructure(
                pRoot, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR);
#endif  // VK_KHR_8bit_storage

#ifdef VK_EXT_descriptor_indexing
        const VkPhysicalDeviceDescriptorIndexingFeatures *requestedDescriptorInxedingFeatures =
            (const VkPhysicalDeviceDescriptorIndexingFeatures *)_vpGetStructure(
                pRoot, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES);
#endif  // VK_EXT_descriptor_indexing

#ifdef VK_EXT_host_query_reset
        const VkPhysicalDeviceHostQueryResetFeatures *requestedHostQueryResetFeatures =
            (const VkPhysicalDeviceHostQueryResetFeatures *)_vpGetStructure(
                pRoot, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES);
#endif  // VK_EXT_host_query_reset

#ifdef VK_KHR_shader_subgroup_extended_types
        const VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures *requestedShaderSubgroupFeatures =
            (const VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures *)_vpGetStructure(
                pRoot, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES);
#endif  // VK_KHR_shader_subgroup_extended_types

#ifdef VK_KHR_uniform_buffer_standard_layout
        const VkPhysicalDeviceUniformBufferStandardLayoutFeatures *requestedUniformBufferFeatures =
            (const VkPhysicalDeviceUniformBufferStandardLayoutFeatures *)_vpGetStructure(
                pRoot, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES);
#endif  // VK_KHR_uniform_buffer_standard_layout

#ifdef VK_KHR_shader_draw_parameters
        const VkPhysicalDeviceShaderDrawParametersFeatures *requestedShaderDrawFeatures =
            (const VkPhysicalDeviceShaderDrawParametersFeatures *)_vpGetStructure(
                pRoot, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES);
#endif  // VK_KHR_shader_draw_parameters

#ifdef VK_KHR_shader_float16_int8
        const VkPhysicalDeviceShaderFloat16Int8FeaturesKHR *requestedShaderFloatFeatures =
            (const VkPhysicalDeviceShaderFloat16Int8FeaturesKHR *)_vpGetStructure(
                pRoot, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR);
#endif  // VK_KHR_shader_float16_int8

#ifdef VK_KHR_sampler_ycbcr_conversion
        const VkPhysicalDeviceSamplerYcbcrConversionFeatures *requestedSamplerYcbcrFeatures =
            (const VkPhysicalDeviceSamplerYcbcrConversionFeatures *)_vpGetStructure(
                pRoot, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES);
#endif  // VK_KHR_sampler_ycbcr_conversion

#ifdef VK_KHR_variable_pointers
        const VkPhysicalDeviceVariablePointersFeaturesKHR *requestedVariableFeatures =
            (const VkPhysicalDeviceVariablePointersFeaturesKHR *)_vpGetStructure(
                pRoot, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES_KHR);
#endif  // VK_KHR_variable_pointers

#ifdef VK_KHR_portability_subset
        const VkPhysicalDevicePortabilitySubsetFeaturesKHR *requestedPortabilityFeatures =
            (const VkPhysicalDevicePortabilitySubsetFeaturesKHR *)_vpGetStructure(
                pRoot, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR);
#endif  // VK_KHR_portability_subset

        VkPhysicalDeviceFeatures2 deviceFeatures2 = {};
        deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        deviceFeatures2.pNext = nullptr;
        pProfileNext = &deviceFeatures2;

#ifdef VK_KHR_multiview
        VkPhysicalDeviceMultiviewFeaturesKHR deviceMultiviewFeatures = {};
        deviceMultiviewFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR;
        deviceMultiviewFeatures.pNext = pProfileNext;
        pProfileNext = &deviceMultiviewFeatures;
#endif  // VK_KHR_multiview

#ifdef VK_KHR_imageless_framebuffer
        VkPhysicalDeviceImagelessFramebufferFeaturesKHR deviceImagelessFeatures = {};
        deviceImagelessFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR;
        deviceImagelessFeatures.pNext = pProfileNext;
        pProfileNext = &deviceImagelessFeatures;
#endif  // VK_KHR_imageless_framebuffer

#ifdef VK_KHR_16bit_storage
        VkPhysicalDevice16BitStorageFeatures device16BitFeatures = {};
        device16BitFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES;
        device16BitFeatures.pNext = pProfileNext;
        pProfileNext = &device16BitFeatures;
#endif  // VK_KHR_16bit_storage

#ifdef VK_KHR_8bit_storage
        VkPhysicalDevice8BitStorageFeatures device8BitFeatures = {};
        device8BitFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES;
        device8BitFeatures.pNext = pProfileNext;
        pProfileNext = &device8BitFeatures;
#endif  // VK_KHR_8bit_storage

#ifdef VK_EXT_descriptor_indexing
        VkPhysicalDeviceDescriptorIndexingFeatures deviceDescriptorInxedingFeatures = {};
        deviceDescriptorInxedingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
        deviceDescriptorInxedingFeatures.pNext = pProfileNext;
        pProfileNext = &deviceDescriptorInxedingFeatures;
#endif  // VK_EXT_descriptor_indexing

#ifdef VK_EXT_host_query_reset
        VkPhysicalDeviceHostQueryResetFeatures deviceHostQueryResetFeatures = {};
        deviceHostQueryResetFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES;
        deviceHostQueryResetFeatures.pNext = pProfileNext;
        pProfileNext = &deviceHostQueryResetFeatures;
#endif  // VK_EXT_host_query_reset

#ifdef VK_KHR_shader_subgroup_extended_types
        VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures deviceShaderSubgroupFeatures = {};
        deviceShaderSubgroupFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES;
        deviceShaderSubgroupFeatures.pNext = pProfileNext;
        pProfileNext = &deviceShaderSubgroupFeatures;
#endif  // VK_KHR_shader_subgroup_extended_types

#ifdef VK_KHR_uniform_buffer_standard_layout
        VkPhysicalDeviceUniformBufferStandardLayoutFeatures deviceUniformBufferFeatures = {};
        deviceUniformBufferFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES;
        deviceUniformBufferFeatures.pNext = pProfileNext;
        pProfileNext = &deviceUniformBufferFeatures;
#endif  // VK_KHR_uniform_buffer_standard_layout

#ifdef VK_KHR_shader_draw_parameters
        VkPhysicalDeviceShaderDrawParametersFeatures deviceShaderDrawFeatures = {};
        deviceShaderDrawFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;
        deviceShaderDrawFeatures.pNext = pProfileNext;
        pProfileNext = &deviceShaderDrawFeatures;
#endif  // VK_KHR_shader_draw_parameters

#ifdef VK_KHR_8bit_storage
        VkPhysicalDevice8BitStorageFeaturesKHR device8BitStorageFeatures = {};
        device8BitStorageFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR;
        device8BitStorageFeatures.pNext = pProfileNext;
        pProfileNext = &device8BitStorageFeatures;
#endif  // VK_KHR_8bit_storage

#ifdef VK_KHR_shader_float16_int8
        VkPhysicalDeviceShaderFloat16Int8FeaturesKHR deviceShaderFloatFeatures = {};
        deviceShaderFloatFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR;
        deviceShaderFloatFeatures.pNext = pProfileNext;
        pProfileNext = &deviceShaderFloatFeatures;
#endif  // VK_KHR_shader_float16_int8

#ifdef VK_KHR_sampler_ycbcr_conversion
        VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR deviceSamplerYcbcrFeatures = {};
        deviceSamplerYcbcrFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR;
        deviceSamplerYcbcrFeatures.pNext = pProfileNext;
        pProfileNext = &deviceSamplerYcbcrFeatures;
#endif  // VK_KHR_sampler_ycbcr_conversion

#ifdef VK_KHR_variable_pointers
        VkPhysicalDeviceVariablePointerFeaturesKHR deviceVariableFeatures = {};
        deviceVariableFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES_KHR;
        deviceVariableFeatures.pNext = pProfileNext;
        pProfileNext = &deviceVariableFeatures;
#endif  // VK_KHR_variable_pointers

#ifdef VK_KHR_portability_subset
        VkPhysicalDevicePortabilitySubsetFeaturesKHR devicePortabilityFeatures = {};
        devicePortabilityFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR;
        if (strcmp(pCreateInfo->pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME) == 0) {
            devicePortabilityFeatures.pNext = pProfileNext;
            pProfileNext = &devicePortabilityFeatures;
        }
#endif  // VK_KHR_portability_subset

        vpGetProfileStructures(pCreateInfo->pProfile, pProfileNext);

        void *pNext = pRoot;

        if (pCreateInfo->pCreateInfo->pEnabledFeatures != nullptr) {
            deviceFeatures2.features = *pCreateInfo->pCreateInfo->pEnabledFeatures;
        }
        if (requestedFeatures2 == nullptr && pCreateInfo->pCreateInfo->pEnabledFeatures == nullptr) {
            deviceFeatures2.pNext = pNext;
            pNext = &deviceFeatures2;
        }
        if (pCreateInfo->flags & VP_DEVICE_CREATE_DISABLE_ROBUST_BUFFER_ACCESS_BIT) {
            deviceFeatures2.features.robustBufferAccess = VK_FALSE;
        }

#ifdef VK_KHR_multiview
        if (requestedMultiviewFeatures == nullptr) {
            deviceMultiviewFeatures.pNext = pNext;
            pNext = &deviceMultiviewFeatures;
        }
#endif  // VK_KHR_multiview

#ifdef VK_KHR_imageless_framebuffer
        if (requestedImagelessFeatures == nullptr) {
            deviceImagelessFeatures.pNext = pNext;
            pNext = &deviceImagelessFeatures;
        }
#endif  // VK_KHR_imageless_framebuffer

#ifdef VK_KHR_16bit_storage
        if (requested16BitFeatures == nullptr) {
            device16BitFeatures.pNext = pNext;
            pNext = &device16BitFeatures;
        }
#endif  // VK_KHR_16bit_storage

#ifdef VK_KHR_8bit_storage
        if (requested8BitStorageFeatures == nullptr) {
            device8BitFeatures.pNext = pNext;
            pNext = &device8BitFeatures;
        }
#endif  // VK_KHR_8bit_storage

#ifdef VK_EXT_descriptor_indexing
        if (requestedDescriptorInxedingFeatures == nullptr) {
            deviceDescriptorInxedingFeatures.pNext = pNext;
            pNext = &deviceDescriptorInxedingFeatures;
        }
#endif  // VK_EXT_descriptor_indexing

#ifdef VK_EXT_host_query_reset
        if (requestedHostQueryResetFeatures == nullptr) {
            deviceHostQueryResetFeatures.pNext = pNext;
            pNext = &deviceHostQueryResetFeatures;
        }
#endif  // VK_EXT_host_query_reset

#ifdef VK_KHR_shader_subgroup_extended_types
        if (requestedShaderSubgroupFeatures == nullptr) {
            deviceShaderSubgroupFeatures.pNext = pNext;
            pNext = &deviceShaderSubgroupFeatures;
        }
#endif  // VK_KHR_shader_subgroup_extended_types

#ifdef VK_KHR_uniform_buffer_standard_layout
        if (requestedUniformBufferFeatures == nullptr) {
            deviceUniformBufferFeatures.pNext = pNext;
            pNext = &deviceUniformBufferFeatures;
        }
#endif  // VK_KHR_uniform_buffer_standard_layout

#ifdef VK_KHR_shader_draw_parameters
        if (requestedShaderDrawFeatures == nullptr) {
            deviceShaderDrawFeatures.pNext = pNext;
            pNext = &deviceShaderDrawFeatures;
        }
#endif  // VK_KHR_shader_draw_parameters

#ifdef VK_KHR_8bit_storage
        if (requested8BitStorageFeatures == nullptr) {
            device8BitStorageFeatures.pNext = pNext;
            pNext = &device8BitStorageFeatures;
        }
#endif  // VK_KHR_8bit_storage

#ifdef VK_KHR_shader_float16_int8
        if (requestedShaderFloatFeatures == nullptr) {
            deviceShaderFloatFeatures.pNext = pNext;
            pNext = &deviceShaderFloatFeatures;
        }
#endif  // VK_KHR_shader_float16_int8

#ifdef VK_KHR_sampler_ycbcr_conversion
        if (requestedSamplerYcbcrFeatures == nullptr) {
            deviceSamplerYcbcrFeatures.pNext = pNext;
            pNext = &deviceSamplerYcbcrFeatures;
        }
#endif  // VK_KHR_sampler_ycbcr_conversion

#ifdef VK_KHR_variable_pointers
        if (requestedVariableFeatures == nullptr) {
            deviceVariableFeatures.pNext = pNext;
            pNext = &deviceVariableFeatures;
        }
#endif  // VK_KHR_variable_pointers

#ifdef VK_KHR_portability_subset
        if (strcmp(pCreateInfo->pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME) == 0) {
            if (requestedPortabilityFeatures == nullptr) {
                devicePortabilityFeatures.pNext = pNext;
                pNext = &devicePortabilityFeatures;
            }
        }
#endif  // VK_KHR_portability_subset

        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pNext = pNext;
        deviceCreateInfo.queueCreateInfoCount = pCreateInfo->pCreateInfo->queueCreateInfoCount;
        deviceCreateInfo.pQueueCreateInfos = pCreateInfo->pCreateInfo->pQueueCreateInfos;
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = static_cast<const char *const *>(extensions.data());
        deviceCreateInfo.pEnabledFeatures =
            pCreateInfo->pCreateInfo->pEnabledFeatures != nullptr ? &deviceFeatures2.features : nullptr;
        return vkCreateDevice(physicalDevice, &deviceCreateInfo, pAllocator, pDevice);
    } else {
        return VK_ERROR_UNKNOWN;
    }
}

VP_INLINE VkResult vpGetDeviceProfileSupport(VkPhysicalDevice physicalDevice, const char *pLayerName,
                                             const VpProfileProperties *pProfile, VkBool32 *pSupported) {
    assert(pProfile != nullptr);
    assert(pSupported != nullptr);
    assert(physicalDevice != VK_NULL_HANDLE);

    VkResult result = VK_SUCCESS;

    uint32_t instanceExtensionCount;
    result = vkEnumerateInstanceExtensionProperties(pLayerName, &instanceExtensionCount, nullptr);
    if (result != VK_SUCCESS) return result;
    std::vector<VkExtensionProperties> instanceExtensions(instanceExtensionCount);
    result = vkEnumerateInstanceExtensionProperties(pLayerName, &instanceExtensionCount, instanceExtensions.data());
    if (result != VK_SUCCESS) return result;

    uint32_t deviceExtensionCount;
    result = vkEnumerateDeviceExtensionProperties(physicalDevice, pLayerName, &deviceExtensionCount, nullptr);
    if (result != VK_SUCCESS) return result;
    std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionCount);
    result = vkEnumerateDeviceExtensionProperties(physicalDevice, pLayerName, &deviceExtensionCount, deviceExtensions.data());
    if (result != VK_SUCCESS) return result;

    *pSupported = VK_FALSE;

    if (strcmp(VP_KHR_ROADMAP_2022_NAME, pProfile->profileName) == 0) {
        if (VP_KHR_ROADMAP_2022_SPEC_VERSION < pProfile->specVersion) {
            return result;
        }

        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);
        if (VK_VERSION_PATCH(properties.apiVersion) < VK_VERSION_PATCH(VP_KHR_ROADMAP_2022_MIN_API_VERSION)) return result;

        VkBool32 extensionSupported = VK_TRUE;
        for (std::size_t i = 0, n = _vpCountOf(_VP_KHR_ROADMAP_2022_EXTENSIONS); i < n && extensionSupported; ++i) {
            const bool supportedInstanceExt = _vpCheckExtension(instanceExtensions.data(), instanceExtensions.size(),
                                                                _VP_KHR_ROADMAP_2022_EXTENSIONS[i].extensionName);
            const bool supportedDeviceExt = _vpCheckExtension(deviceExtensions.data(), deviceExtensions.size(),
                                                              _VP_KHR_ROADMAP_2022_EXTENSIONS[i].extensionName);

            if (!supportedInstanceExt && !supportedDeviceExt) {
                extensionSupported = VK_FALSE;
            }
        }

        if (extensionSupported == VK_FALSE) {
            return result;
        }

        void *pDeviceFeaturesNext = nullptr;

        VkPhysicalDeviceVulkan13Features deviceFeatures13 = {};
        deviceFeatures13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        deviceFeatures13.pNext = pDeviceFeaturesNext;
        pDeviceFeaturesNext = &deviceFeatures13;

        VkPhysicalDeviceVulkan12Features deviceFeatures12 = {};
        deviceFeatures12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        deviceFeatures12.pNext = pDeviceFeaturesNext;
        pDeviceFeaturesNext = &deviceFeatures12;

        VkPhysicalDeviceVulkan11Features deviceFeatures11 = {};
        deviceFeatures11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
        deviceFeatures11.pNext = pDeviceFeaturesNext;
        pDeviceFeaturesNext = &deviceFeatures11;

        VkPhysicalDeviceFeatures2 deviceFeatures = {};
        deviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        deviceFeatures.pNext = pDeviceFeaturesNext;

        vkGetPhysicalDeviceFeatures2(physicalDevice, &deviceFeatures);

        void *pProfileFeaturesNext = nullptr;

        VkPhysicalDeviceVulkan13Features profileFeatures13 = {};
        profileFeatures13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        profileFeatures13.pNext = pProfileFeaturesNext;
        pProfileFeaturesNext = &profileFeatures13;

        VkPhysicalDeviceVulkan12Features profileFeatures12 = {};
        profileFeatures12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        profileFeatures12.pNext = pProfileFeaturesNext;
        pProfileFeaturesNext = &profileFeatures12;

        VkPhysicalDeviceVulkan11Features profileFeatures11 = {};
        profileFeatures11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
        profileFeatures11.pNext = pProfileFeaturesNext;
        pProfileFeaturesNext = &profileFeatures11;

        VkPhysicalDeviceFeatures2 profileFeatures = {};
        profileFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        profileFeatures.pNext = pProfileFeaturesNext;

        vpGetProfileStructures(pProfile, &profileFeatures);

        if (deviceFeatures.features.robustBufferAccess != profileFeatures.features.robustBufferAccess) {
            return result;
        } else if (deviceFeatures.features.fullDrawIndexUint32 != profileFeatures.features.fullDrawIndexUint32) {
            return result;
        } else if (deviceFeatures.features.imageCubeArray != profileFeatures.features.imageCubeArray) {
            return result;
        } else if (deviceFeatures.features.independentBlend != profileFeatures.features.independentBlend) {
            return result;
        } else if (deviceFeatures.features.sampleRateShading != profileFeatures.features.sampleRateShading) {
            return result;
        } else if (deviceFeatures.features.drawIndirectFirstInstance != profileFeatures.features.drawIndirectFirstInstance) {
            return result;
        } else if (deviceFeatures.features.depthClamp != profileFeatures.features.depthClamp) {
            return result;
        } else if (deviceFeatures.features.depthBiasClamp != profileFeatures.features.depthBiasClamp) {
            return result;
        } else if (deviceFeatures.features.samplerAnisotropy != profileFeatures.features.samplerAnisotropy) {
            return result;
        } else if (deviceFeatures.features.occlusionQueryPrecise != profileFeatures.features.occlusionQueryPrecise) {
            return result;
        } else if (deviceFeatures.features.fragmentStoresAndAtomics != profileFeatures.features.fragmentStoresAndAtomics) {
            return result;
        } else if (deviceFeatures.features.shaderStorageImageExtendedFormats !=
                   profileFeatures.features.shaderStorageImageExtendedFormats) {
            return result;
        } else if (deviceFeatures.features.shaderUniformBufferArrayDynamicIndexing !=
                   profileFeatures.features.shaderUniformBufferArrayDynamicIndexing) {
            return result;
        } else if (deviceFeatures.features.shaderSampledImageArrayDynamicIndexing !=
                   profileFeatures.features.shaderSampledImageArrayDynamicIndexing) {
            return result;
        } else if (deviceFeatures.features.shaderStorageBufferArrayDynamicIndexing !=
                   profileFeatures.features.shaderStorageBufferArrayDynamicIndexing) {
            return result;
        } else if (deviceFeatures.features.shaderStorageImageArrayDynamicIndexing !=
                   profileFeatures.features.shaderStorageImageArrayDynamicIndexing) {
            return result;
        }

        if (deviceFeatures11.multiview != profileFeatures11.multiview) {
            return result;
        } else if (deviceFeatures11.samplerYcbcrConversion != profileFeatures11.samplerYcbcrConversion) {
            return result;
        }

        if (deviceFeatures12.uniformBufferStandardLayout != profileFeatures12.uniformBufferStandardLayout) {
            return result;
        } else if (deviceFeatures12.subgroupBroadcastDynamicId != profileFeatures12.subgroupBroadcastDynamicId) {
            return result;
        } else if (deviceFeatures12.imagelessFramebuffer != profileFeatures12.imagelessFramebuffer) {
            return result;
        } else if (deviceFeatures12.separateDepthStencilLayouts != profileFeatures12.separateDepthStencilLayouts) {
            return result;
        } else if (deviceFeatures12.hostQueryReset != profileFeatures12.hostQueryReset) {
            return result;
        } else if (deviceFeatures12.timelineSemaphore != profileFeatures12.timelineSemaphore) {
            return result;
        } else if (deviceFeatures12.shaderSubgroupExtendedTypes != profileFeatures12.shaderSubgroupExtendedTypes) {
            return result;
        } else if (deviceFeatures12.vulkanMemoryModel != profileFeatures12.vulkanMemoryModel) {
            return result;
        } else if (deviceFeatures12.vulkanMemoryModelDeviceScope != profileFeatures12.vulkanMemoryModelDeviceScope) {
            return result;
        } else if (deviceFeatures12.vulkanMemoryModelAvailabilityVisibilityChains !=
                   profileFeatures12.vulkanMemoryModelAvailabilityVisibilityChains) {
            return result;
        } else if (deviceFeatures12.bufferDeviceAddress != profileFeatures12.bufferDeviceAddress) {
            return result;
        } else if (deviceFeatures12.samplerMirrorClampToEdge != profileFeatures12.samplerMirrorClampToEdge) {
            return result;
        } else if (deviceFeatures12.descriptorIndexing != profileFeatures12.descriptorIndexing) {
            return result;
        } else if (deviceFeatures12.shaderUniformTexelBufferArrayDynamicIndexing !=
                   profileFeatures12.shaderUniformTexelBufferArrayDynamicIndexing) {
            return result;
        } else if (deviceFeatures12.shaderStorageTexelBufferArrayDynamicIndexing !=
                   profileFeatures12.shaderStorageTexelBufferArrayDynamicIndexing) {
            return result;
        } else if (deviceFeatures12.shaderUniformBufferArrayNonUniformIndexing !=
                   profileFeatures12.shaderUniformBufferArrayNonUniformIndexing) {
            return result;
        } else if (deviceFeatures12.shaderSampledImageArrayNonUniformIndexing !=
                   profileFeatures12.shaderSampledImageArrayNonUniformIndexing) {
            return result;
        } else if (deviceFeatures12.shaderStorageBufferArrayNonUniformIndexing !=
                   profileFeatures12.shaderStorageBufferArrayNonUniformIndexing) {
            return result;
        } else if (deviceFeatures12.shaderStorageImageArrayNonUniformIndexing !=
                   profileFeatures12.shaderStorageImageArrayNonUniformIndexing) {
            return result;
        } else if (deviceFeatures12.shaderUniformTexelBufferArrayNonUniformIndexing !=
                   profileFeatures12.shaderUniformTexelBufferArrayNonUniformIndexing) {
            return result;
        } else if (deviceFeatures12.shaderStorageTexelBufferArrayNonUniformIndexing !=
                   profileFeatures12.shaderStorageTexelBufferArrayNonUniformIndexing) {
            return result;
        } else if (deviceFeatures12.descriptorBindingUniformBufferUpdateAfterBind !=
                   profileFeatures12.descriptorBindingUniformBufferUpdateAfterBind) {
            return result;
        } else if (deviceFeatures12.descriptorBindingSampledImageUpdateAfterBind !=
                   profileFeatures12.descriptorBindingSampledImageUpdateAfterBind) {
            return result;
        } else if (deviceFeatures12.descriptorBindingStorageImageUpdateAfterBind !=
                   profileFeatures12.descriptorBindingStorageImageUpdateAfterBind) {
            return result;
        } else if (deviceFeatures12.descriptorBindingStorageBufferUpdateAfterBind !=
                   profileFeatures12.descriptorBindingStorageBufferUpdateAfterBind) {
            return result;
        } else if (deviceFeatures12.descriptorBindingUniformTexelBufferUpdateAfterBind !=
                   profileFeatures12.descriptorBindingUniformTexelBufferUpdateAfterBind) {
            return result;
        } else if (deviceFeatures12.descriptorBindingStorageTexelBufferUpdateAfterBind !=
                   profileFeatures12.descriptorBindingStorageTexelBufferUpdateAfterBind) {
            return result;
        } else if (deviceFeatures12.descriptorBindingUpdateUnusedWhilePending !=
                   profileFeatures12.descriptorBindingUpdateUnusedWhilePending) {
            return result;
        } else if (deviceFeatures12.descriptorBindingPartiallyBound != profileFeatures12.descriptorBindingPartiallyBound) {
            return result;
        } else if (deviceFeatures12.descriptorBindingVariableDescriptorCount !=
                   profileFeatures12.descriptorBindingVariableDescriptorCount) {
            return result;
        } else if (deviceFeatures12.runtimeDescriptorArray != profileFeatures12.runtimeDescriptorArray) {
            return result;
        } else if (deviceFeatures12.scalarBlockLayout != profileFeatures12.scalarBlockLayout) {
            return result;
        }

        if (deviceFeatures13.robustImageAccess != profileFeatures13.robustImageAccess) {
            return result;
        } else if (deviceFeatures13.shaderTerminateInvocation != profileFeatures13.shaderTerminateInvocation) {
            return result;
        } else if (deviceFeatures13.shaderZeroInitializeWorkgroupMemory != profileFeatures13.shaderZeroInitializeWorkgroupMemory) {
            return result;
        } else if (deviceFeatures13.synchronization2 != profileFeatures13.synchronization2) {
            return result;
        } else if (deviceFeatures13.shaderIntegerDotProduct != profileFeatures13.shaderIntegerDotProduct) {
            return result;
        } else if (deviceFeatures13.maintenance4 != profileFeatures13.maintenance4) {
            return result;
        } else if (deviceFeatures13.pipelineCreationCacheControl != profileFeatures13.pipelineCreationCacheControl) {
            return result;
        } else if (deviceFeatures13.subgroupSizeControl != profileFeatures13.subgroupSizeControl) {
            return result;
        } else if (deviceFeatures13.computeFullSubgroups != profileFeatures13.computeFullSubgroups) {
            return result;
        } else if (deviceFeatures13.shaderDemoteToHelperInvocation != profileFeatures13.shaderDemoteToHelperInvocation) {
            return result;
        } else if (deviceFeatures13.inlineUniformBlock != profileFeatures13.inlineUniformBlock) {
            return result;
        } else if (deviceFeatures13.descriptorBindingInlineUniformBlockUpdateAfterBind !=
                   profileFeatures13.descriptorBindingInlineUniformBlockUpdateAfterBind) {
            return result;
        }

        void *pDevicePropertiesNext = nullptr;

        VkPhysicalDeviceVulkan13Properties devicePropertiesVulkan13 = {};
        devicePropertiesVulkan13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES;
        devicePropertiesVulkan13.pNext = pDevicePropertiesNext;
        pDevicePropertiesNext = &devicePropertiesVulkan13;

        VkPhysicalDeviceVulkan12Properties devicePropertiesVulkan12 = {};
        devicePropertiesVulkan12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;
        devicePropertiesVulkan12.pNext = pDevicePropertiesNext;
        pDevicePropertiesNext = &devicePropertiesVulkan12;

        VkPhysicalDeviceVulkan11Properties devicePropertiesVulkan11 = {};
        devicePropertiesVulkan11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
        devicePropertiesVulkan11.pNext = pDevicePropertiesNext;
        pDevicePropertiesNext = &devicePropertiesVulkan11;

        VkPhysicalDeviceProperties2 deviceProperties{};
        deviceProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        deviceProperties.pNext = pDevicePropertiesNext;

        vkGetPhysicalDeviceProperties2(physicalDevice, &deviceProperties);

        void *pProfilePropertiesNext = nullptr;

        VkPhysicalDeviceVulkan13Properties profilePropertiesVulkan13 = {};
        profilePropertiesVulkan13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES;
        profilePropertiesVulkan13.pNext = pProfilePropertiesNext;
        pProfilePropertiesNext = &profilePropertiesVulkan13;

        VkPhysicalDeviceVulkan12Properties profilePropertiesVulkan12 = {};
        profilePropertiesVulkan12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;
        profilePropertiesVulkan12.pNext = pProfilePropertiesNext;
        pProfilePropertiesNext = &profilePropertiesVulkan12;

        VkPhysicalDeviceVulkan11Properties profilePropertiesVulkan11 = {};
        profilePropertiesVulkan11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
        profilePropertiesVulkan11.pNext = pProfilePropertiesNext;
        pProfilePropertiesNext = &profilePropertiesVulkan11;

        VkPhysicalDeviceProperties2 profileProperties{};
        profileProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        profileProperties.pNext = pProfilePropertiesNext;

        vpGetProfileStructures(pProfile, &profileProperties);

        if (deviceProperties.properties.limits.maxImageDimension1D < profileProperties.properties.limits.maxImageDimension1D) {
            return result;
        } else if (deviceProperties.properties.limits.maxImageDimension2D <
                   profileProperties.properties.limits.maxImageDimension2D) {
            return result;
        } else if (deviceProperties.properties.limits.maxImageDimensionCube <
                   profileProperties.properties.limits.maxImageDimensionCube) {
            return result;
        } else if (deviceProperties.properties.limits.maxImageArrayLayers <
                   profileProperties.properties.limits.maxImageArrayLayers) {
            return result;
        } else if (deviceProperties.properties.limits.maxUniformBufferRange <
                   profileProperties.properties.limits.maxUniformBufferRange) {
            return result;
        } else if (deviceProperties.properties.limits.bufferImageGranularity >
                   profileProperties.properties.limits.bufferImageGranularity) {
            return result;
        } else if (deviceProperties.properties.limits.maxPerStageDescriptorSamplers <
                   profileProperties.properties.limits.maxPerStageDescriptorSamplers) {
            return result;
        } else if (deviceProperties.properties.limits.maxPerStageDescriptorUniformBuffers <
                   profileProperties.properties.limits.maxPerStageDescriptorUniformBuffers) {
            return result;
        } else if (deviceProperties.properties.limits.maxPerStageDescriptorStorageBuffers <
                   profileProperties.properties.limits.maxPerStageDescriptorStorageBuffers) {
            return result;
        } else if (deviceProperties.properties.limits.maxPerStageDescriptorSampledImages <
                   profileProperties.properties.limits.maxPerStageDescriptorSampledImages) {
            return result;
        } else if (deviceProperties.properties.limits.maxPerStageDescriptorStorageImages <
                   profileProperties.properties.limits.maxPerStageDescriptorStorageImages) {
            return result;
        } else if (deviceProperties.properties.limits.maxPerStageResources <
                   profileProperties.properties.limits.maxPerStageResources) {
            return result;
        } else if (deviceProperties.properties.limits.maxDescriptorSetSamplers <
                   profileProperties.properties.limits.maxDescriptorSetSamplers) {
            return result;
        } else if (deviceProperties.properties.limits.maxDescriptorSetUniformBuffers <
                   profileProperties.properties.limits.maxDescriptorSetUniformBuffers) {
            return result;
        } else if (deviceProperties.properties.limits.maxDescriptorSetStorageBuffers <
                   profileProperties.properties.limits.maxDescriptorSetStorageBuffers) {
            return result;
        } else if (deviceProperties.properties.limits.maxDescriptorSetSampledImages <
                   profileProperties.properties.limits.maxDescriptorSetSampledImages) {
            return result;
        } else if (deviceProperties.properties.limits.maxDescriptorSetStorageImages <
                   profileProperties.properties.limits.maxDescriptorSetStorageImages) {
            return result;
        } else if (deviceProperties.properties.limits.maxFragmentCombinedOutputResources <
                   profileProperties.properties.limits.maxFragmentCombinedOutputResources) {
            return result;
        } else if (deviceProperties.properties.limits.maxComputeWorkGroupInvocations <
                   profileProperties.properties.limits.maxComputeWorkGroupInvocations) {
            return result;
        } else if (deviceProperties.properties.limits.maxComputeWorkGroupSize[0] <
                       profileProperties.properties.limits.maxComputeWorkGroupSize[0] ||
                   deviceProperties.properties.limits.maxComputeWorkGroupSize[1] <
                       profileProperties.properties.limits.maxComputeWorkGroupSize[1] ||
                   deviceProperties.properties.limits.maxComputeWorkGroupSize[2] <
                       profileProperties.properties.limits.maxComputeWorkGroupSize[2]) {
            return result;
        } else if (deviceProperties.properties.limits.subTexelPrecisionBits <
                   profileProperties.properties.limits.subTexelPrecisionBits) {
            return result;
        } else if (deviceProperties.properties.limits.mipmapPrecisionBits <
                   profileProperties.properties.limits.mipmapPrecisionBits) {
            return result;
        } else if (deviceProperties.properties.limits.maxSamplerLodBias < profileProperties.properties.limits.maxSamplerLodBias) {
            return result;
        } else if (deviceProperties.properties.limits.pointSizeGranularity >
                   profileProperties.properties.limits.pointSizeGranularity) {
            return result;
        } else if (deviceProperties.properties.limits.lineWidthGranularity >
                   profileProperties.properties.limits.lineWidthGranularity) {
            return result;
        } else if (deviceProperties.properties.limits.standardSampleLocations !=
                   profileProperties.properties.limits.standardSampleLocations) {
            return result;
        } else if (deviceProperties.properties.limits.maxColorAttachments >
                   profileProperties.properties.limits.maxColorAttachments) {
            return result;
        }

        if (devicePropertiesVulkan11.maxMultiviewViewCount < profilePropertiesVulkan11.maxMultiviewViewCount) {
            return result;
        } else if (devicePropertiesVulkan11.maxMultiviewInstanceIndex < profilePropertiesVulkan11.maxMultiviewInstanceIndex) {
            return result;
        } else if (devicePropertiesVulkan11.subgroupSize < profilePropertiesVulkan11.subgroupSize) {
            return result;
        } else if ((devicePropertiesVulkan11.subgroupSupportedStages & profilePropertiesVulkan11.subgroupSupportedStages) !=
                   profilePropertiesVulkan11.subgroupSupportedStages) {
            return result;
        } else if ((devicePropertiesVulkan11.subgroupSupportedOperations & profilePropertiesVulkan11.subgroupSupportedOperations) !=
                   profilePropertiesVulkan11.subgroupSupportedOperations) {
            return result;
        }

        if (devicePropertiesVulkan12.maxTimelineSemaphoreValueDifference <
            profilePropertiesVulkan12.maxTimelineSemaphoreValueDifference) {
            return result;
        } else if (devicePropertiesVulkan12.shaderSignedZeroInfNanPreserveFloat16 !=
                   profilePropertiesVulkan12.shaderSignedZeroInfNanPreserveFloat16) {
            return result;
        } else if (devicePropertiesVulkan12.shaderSignedZeroInfNanPreserveFloat32 !=
                   profilePropertiesVulkan12.shaderSignedZeroInfNanPreserveFloat32) {
            return result;
        } else if (devicePropertiesVulkan12.maxPerStageDescriptorUpdateAfterBindSamplers <
                   profilePropertiesVulkan12.maxPerStageDescriptorUpdateAfterBindSamplers) {
            return result;
        } else if (devicePropertiesVulkan12.maxPerStageDescriptorUpdateAfterBindUniformBuffers <
                   profilePropertiesVulkan12.maxPerStageDescriptorUpdateAfterBindUniformBuffers) {
            return result;
        } else if (devicePropertiesVulkan12.maxPerStageDescriptorUpdateAfterBindStorageBuffers <
                   profilePropertiesVulkan12.maxPerStageDescriptorUpdateAfterBindStorageBuffers) {
            return result;
        } else if (devicePropertiesVulkan12.maxPerStageDescriptorUpdateAfterBindSampledImages <
                   profilePropertiesVulkan12.maxPerStageDescriptorUpdateAfterBindSampledImages) {
            return result;
        } else if (devicePropertiesVulkan12.maxPerStageDescriptorUpdateAfterBindStorageImages <
                   profilePropertiesVulkan12.maxPerStageDescriptorUpdateAfterBindStorageImages) {
            return result;
        } else if (devicePropertiesVulkan12.maxPerStageDescriptorUpdateAfterBindInputAttachments <
                   profilePropertiesVulkan12.maxPerStageDescriptorUpdateAfterBindInputAttachments) {
            return result;
        } else if (devicePropertiesVulkan12.maxPerStageUpdateAfterBindResources <
                   profilePropertiesVulkan12.maxPerStageUpdateAfterBindResources) {
            return result;
        } else if (devicePropertiesVulkan12.maxDescriptorSetUpdateAfterBindSamplers <
                   profilePropertiesVulkan12.maxDescriptorSetUpdateAfterBindSamplers) {
            return result;
        } else if (devicePropertiesVulkan12.maxDescriptorSetUpdateAfterBindUniformBuffers <
                   profilePropertiesVulkan12.maxDescriptorSetUpdateAfterBindUniformBuffers) {
            return result;
        } else if (devicePropertiesVulkan12.maxDescriptorSetUpdateAfterBindUniformBuffersDynamic <
                   profilePropertiesVulkan12.maxDescriptorSetUpdateAfterBindUniformBuffersDynamic) {
            return result;
        } else if (devicePropertiesVulkan12.maxDescriptorSetUpdateAfterBindStorageBuffers <
                   profilePropertiesVulkan12.maxDescriptorSetUpdateAfterBindStorageBuffers) {
            return result;
        } else if (devicePropertiesVulkan12.maxDescriptorSetUpdateAfterBindStorageBuffersDynamic <
                   profilePropertiesVulkan12.maxDescriptorSetUpdateAfterBindStorageBuffersDynamic) {
            return result;
        } else if (devicePropertiesVulkan12.maxDescriptorSetUpdateAfterBindSampledImages <
                   profilePropertiesVulkan12.maxDescriptorSetUpdateAfterBindSampledImages) {
            return result;
        } else if (devicePropertiesVulkan12.maxDescriptorSetUpdateAfterBindStorageImages <
                   profilePropertiesVulkan12.maxDescriptorSetUpdateAfterBindStorageImages) {
            return result;
        } else if (devicePropertiesVulkan12.maxDescriptorSetUpdateAfterBindInputAttachments <
                   profilePropertiesVulkan12.maxDescriptorSetUpdateAfterBindInputAttachments) {
            return result;
        }

        if (devicePropertiesVulkan13.maxBufferSize < profilePropertiesVulkan13.maxBufferSize) {
            return result;
        } else if (devicePropertiesVulkan13.maxInlineUniformBlockSize < profilePropertiesVulkan13.maxInlineUniformBlockSize) {
            return result;
        } else if (devicePropertiesVulkan13.maxPerStageDescriptorInlineUniformBlocks <
                   profilePropertiesVulkan13.maxPerStageDescriptorInlineUniformBlocks) {
            return result;
        } else if (devicePropertiesVulkan13.maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks <
                   profilePropertiesVulkan13.maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks) {
            return result;
        } else if (devicePropertiesVulkan13.maxDescriptorSetInlineUniformBlocks <
                   profilePropertiesVulkan13.maxDescriptorSetInlineUniformBlocks) {
            return result;
        } else if (devicePropertiesVulkan13.maxDescriptorSetUpdateAfterBindInlineUniformBlocks <
                   profilePropertiesVulkan13.maxDescriptorSetUpdateAfterBindInlineUniformBlocks) {
            return result;
        } else if (devicePropertiesVulkan13.maxInlineUniformTotalSize < profilePropertiesVulkan13.maxInlineUniformTotalSize) {
            return result;
        }

    } else if (strcmp(VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, pProfile->profileName) == 0
#ifdef VK_ENABLE_BETA_EXTENSIONS
               || strcmp(VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME, pProfile->profileName) == 0
#endif
    ) {
        if (strcmp(VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, pProfile->profileName) == 0) {
            if (VP_LUNARG_DESKTOP_PORTABILITY_2021_SPEC_VERSION < pProfile->specVersion) {
                return result;
            }
        }
#ifdef VK_ENABLE_BETA_EXTENSIONS
        else if (strcmp(VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME, pProfile->profileName) == 0) {
            if (VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_SPEC_VERSION < pProfile->specVersion) {
                return result;
            }
        }
#endif

        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);
        if (strcmp(VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, pProfile->profileName) == 0) {
            if (VK_VERSION_PATCH(properties.apiVersion) < VK_VERSION_PATCH(VP_LUNARG_DESKTOP_PORTABILITY_2021_MIN_API_VERSION)) {
                return result;
            }
        }
#ifdef VK_ENABLE_BETA_EXTENSIONS
        else if (strcmp(VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME, pProfile->profileName) == 0) {
            if (VK_VERSION_PATCH(properties.apiVersion) <
                VK_VERSION_PATCH(VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_MIN_API_VERSION)) {
                return result;
            }
        }
#endif

        VkBool32 extensionSupported = VK_TRUE;
        if (strcmp(VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME, pProfile->profileName) == 0) {
            for (std::size_t i = 0, n = _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_EXTENSIONS); i < n && extensionSupported;
                 ++i) {
                const VkExtensionProperties &extensionProperties = _VP_LUNARG_DESKTOP_PORTABILITY_2021_EXTENSIONS[i];

                const bool supportedInstanceExt =
                    _vpCheckExtension(instanceExtensions.data(), instanceExtensions.size(), extensionProperties.extensionName);
                const bool supportedDeviceExt =
                    _vpCheckExtension(deviceExtensions.data(), deviceExtensions.size(), extensionProperties.extensionName);

                if (!supportedInstanceExt && !supportedDeviceExt) {
                    extensionSupported = VK_FALSE;
                }
            }
        }

#ifdef VK_ENABLE_BETA_EXTENSIONS
        if (strcmp(VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME, pProfile->profileName) == 0) {
            for (std::size_t i = 0, n = _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_EXTENSIONS);
                 i < n && extensionSupported; ++i) {
                const VkExtensionProperties &extensionProperties = _VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_EXTENSIONS[i];

                const bool supportedInstanceExt =
                    _vpCheckExtension(instanceExtensions.data(), instanceExtensions.size(), extensionProperties.extensionName);
                const bool supportedDeviceExt =
                    _vpCheckExtension(deviceExtensions.data(), deviceExtensions.size(), extensionProperties.extensionName);

                if (!supportedInstanceExt && !supportedDeviceExt) {
                    extensionSupported = VK_FALSE;
                }
            }
        }
#endif  // VK_ENABLE_BETA_EXTENSIONS

        if (extensionSupported == VK_FALSE) {
            return result;
        }

        VkBool32 formatsSupported = VK_TRUE;
        for (std::size_t i = 0, n = _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_FORMATS); i < n && formatsSupported; ++i) {
            const VpFormatProperties &requiredProps = _VP_LUNARG_DESKTOP_PORTABILITY_2021_FORMATS[i];

            VkFormatProperties2 deviceProps = {};
            deviceProps.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
            vkGetPhysicalDeviceFormatProperties2(physicalDevice, requiredProps.format, &deviceProps);

            if (!_vpCheckFormatProperty(&deviceProps, requiredProps)) {
                formatsSupported = VK_FALSE;
            }
        }

        if (formatsSupported == VK_FALSE) {
            return result;
        }

        std::uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamily(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamily.data());

        VkBool32 queueSupported = VK_TRUE;
        for (uint32_t i = 0, n = _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_QUEUE_FAMILY_PROPERTIES); i < n && queueSupported;
             ++i) {
            if (!_vpCheckQueueFamilyProperty(&queueFamily[0], queueFamilyCount,
                                             _VP_LUNARG_DESKTOP_PORTABILITY_2021_QUEUE_FAMILY_PROPERTIES[i])) {
                queueSupported = VK_FALSE;
            }
        }

        if (queueSupported == VK_FALSE) {
            return result;
        }

        void *pDeviceFeaturesNext = nullptr;

#ifdef VK_KHR_multiview
        VkPhysicalDeviceMultiviewFeaturesKHR deviceMultiviewFeatures = {};
        deviceMultiviewFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR;
        deviceMultiviewFeatures.pNext = pDeviceFeaturesNext;
        pDeviceFeaturesNext = &deviceMultiviewFeatures;
#endif  // VK_KHR_multiview

#ifdef VK_KHR_imageless_framebuffer
        VkPhysicalDeviceImagelessFramebufferFeaturesKHR deviceImagelessFramebufferFeatures = {};
        deviceImagelessFramebufferFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR;
        deviceImagelessFramebufferFeatures.pNext = pDeviceFeaturesNext;
        pDeviceFeaturesNext = &deviceImagelessFramebufferFeatures;
#endif  // VK_KHR_imageless_framebuffer

#ifdef VK_KHR_16bit_storage
        VkPhysicalDevice16BitStorageFeaturesKHR deviceStorage16bit = {};
        deviceStorage16bit.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES_KHR;
        deviceStorage16bit.pNext = pDeviceFeaturesNext;
        pDeviceFeaturesNext = &deviceStorage16bit;
#endif  // VK_KHR_16bit_storage

#ifdef VK_KHR_8bit_storage
        VkPhysicalDevice8BitStorageFeaturesKHR device8BitStorageFeatures = {};
        device8BitStorageFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR;
        device8BitStorageFeatures.pNext = pDeviceFeaturesNext;
        pDeviceFeaturesNext = &device8BitStorageFeatures;
#endif  // VK_KHR_8bit_storage

#ifdef VK_EXT_descriptor_indexing
        VkPhysicalDeviceDescriptorIndexingFeaturesEXT deviceDescriptorIndexingFeatures = {};
        deviceDescriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
        deviceDescriptorIndexingFeatures.pNext = pDeviceFeaturesNext;
        pDeviceFeaturesNext = &deviceDescriptorIndexingFeatures;
#endif  // VK_EXT_descriptor_indexing

#ifdef VK_EXT_host_query_reset
        VkPhysicalDeviceHostQueryResetFeaturesEXT deviceQueryResetFeatures = {};
        deviceQueryResetFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT;
        deviceQueryResetFeatures.pNext = pDeviceFeaturesNext;
        pDeviceFeaturesNext = &deviceQueryResetFeatures;
#endif  // VK_EXT_host_query_reset

#ifdef VK_KHR_shader_subgroup_extended_types
        VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR deviceSubgroupExtendedTypesFeatures = {};
        deviceSubgroupExtendedTypesFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR;
        deviceSubgroupExtendedTypesFeatures.pNext = pDeviceFeaturesNext;
        pDeviceFeaturesNext = &deviceSubgroupExtendedTypesFeatures;
#endif  // VK_KHR_shader_subgroup_extended_types

#ifdef VK_KHR_uniform_buffer_standard_layout
        VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR deviceUniformBufferStandardLayoutFeatures = {};
        deviceUniformBufferStandardLayoutFeatures.sType =
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR;
        deviceUniformBufferStandardLayoutFeatures.pNext = pDeviceFeaturesNext;
        pDeviceFeaturesNext = &deviceUniformBufferStandardLayoutFeatures;
#endif  // VK_KHR_uniform_buffer_standard_layout

#ifdef VK_KHR_shader_draw_parameters
        VkPhysicalDeviceShaderDrawParametersFeatures deviceShaderDrawParametersFeatures = {};
        deviceShaderDrawParametersFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;
        deviceShaderDrawParametersFeatures.pNext = pDeviceFeaturesNext;
        pDeviceFeaturesNext = &deviceShaderDrawParametersFeatures;
#endif  // VK_KHR_shader_draw_parameters

#ifdef VK_KHR_shader_float16_int8
        VkPhysicalDeviceShaderFloat16Int8FeaturesKHR deviceShaderFloat16Int8Features = {};
        deviceShaderFloat16Int8Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR;
        deviceShaderFloat16Int8Features.pNext = pDeviceFeaturesNext;
        pDeviceFeaturesNext = &deviceShaderFloat16Int8Features;
#endif  // VK_KHR_shader_float16_int8

#ifdef VK_KHR_sampler_ycbcr_conversion
        VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR deviceSamplerYcbcrConversionFeatures = {};
        deviceSamplerYcbcrConversionFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR;
        deviceSamplerYcbcrConversionFeatures.pNext = pDeviceFeaturesNext;
        pDeviceFeaturesNext = &deviceSamplerYcbcrConversionFeatures;
#endif  // VK_KHR_sampler_ycbcr_conversion

#ifdef VK_KHR_variable_pointers
        VkPhysicalDeviceVariablePointerFeaturesKHR deviceVariablePointerFeatures = {};
        deviceVariablePointerFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES_KHR;
        deviceVariablePointerFeatures.pNext = pDeviceFeaturesNext;
        pDeviceFeaturesNext = &deviceVariablePointerFeatures;
#endif  // VK_KHR_variable_pointers

#ifdef VK_KHR_portability_subset
        VkPhysicalDevicePortabilitySubsetFeaturesKHR devicePortabilitySubset = {};
        devicePortabilitySubset.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR;
        if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME) == 0) {
            devicePortabilitySubset.pNext = pDeviceFeaturesNext;
            pDeviceFeaturesNext = &devicePortabilitySubset;
        }
#endif  // VK_KHR_portability_subset

        VkPhysicalDeviceFeatures2 deviceFeatures = {};
        deviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        deviceFeatures.pNext = pDeviceFeaturesNext;

        vkGetPhysicalDeviceFeatures2(physicalDevice, &deviceFeatures);

        void *pProfileFeaturesNext = nullptr;

#ifdef VK_KHR_multiview
        VkPhysicalDeviceMultiviewFeaturesKHR profileMultiviewFeatures = {};
        profileMultiviewFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR;
        profileMultiviewFeatures.pNext = pProfileFeaturesNext;
        pProfileFeaturesNext = &profileMultiviewFeatures;
#endif  // VK_KHR_multiview

#ifdef VK_KHR_imageless_framebuffer
        VkPhysicalDeviceImagelessFramebufferFeaturesKHR profileImagelessFramebufferFeatures = {};
        profileImagelessFramebufferFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR;
        profileImagelessFramebufferFeatures.pNext = pProfileFeaturesNext;
        pProfileFeaturesNext = &profileImagelessFramebufferFeatures;
#endif  // VK_KHR_imageless_framebuffer

#ifdef VK_KHR_16bit_storage
        VkPhysicalDevice16BitStorageFeaturesKHR profileStorage16bitFeatures = {};
        profileStorage16bitFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES_KHR;
        profileStorage16bitFeatures.pNext = pProfileFeaturesNext;
        pProfileFeaturesNext = &profileStorage16bitFeatures;
#endif  // VK_KHR_16bit_storage

#ifdef VK_KHR_8bit_storage
        VkPhysicalDevice8BitStorageFeaturesKHR profile8BitStorageFeatures = {};
        profile8BitStorageFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR;
        profile8BitStorageFeatures.pNext = pProfileFeaturesNext;
        pProfileFeaturesNext = &profile8BitStorageFeatures;
#endif  // VK_KHR_8bit_storage

#ifdef VK_EXT_descriptor_indexing
        VkPhysicalDeviceDescriptorIndexingFeaturesEXT profileDescriptorIndexingFeatures = {};
        profileDescriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
        profileDescriptorIndexingFeatures.pNext = pProfileFeaturesNext;
        pProfileFeaturesNext = &profileDescriptorIndexingFeatures;
#endif  // VK_EXT_descriptor_indexing

#ifdef VK_EXT_host_query_reset
        VkPhysicalDeviceHostQueryResetFeaturesEXT profileQueryResetFeatures = {};
        profileQueryResetFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT;
        profileQueryResetFeatures.pNext = pProfileFeaturesNext;
        pProfileFeaturesNext = &profileQueryResetFeatures;
#endif  // VK_EXT_host_query_reset

#ifdef VK_KHR_shader_subgroup_extended_types
        VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR profileSubgroupExtendedTypesFeatures = {};
        profileSubgroupExtendedTypesFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR;
        profileSubgroupExtendedTypesFeatures.pNext = pProfileFeaturesNext;
        pProfileFeaturesNext = &profileSubgroupExtendedTypesFeatures;
#endif  // VK_KHR_shader_subgroup_extended_types

#ifdef VK_KHR_uniform_buffer_standard_layout
        VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR profileUniformBufferStandardLayoutFeatures = {};
        profileUniformBufferStandardLayoutFeatures.sType =
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR;
        profileUniformBufferStandardLayoutFeatures.pNext = pProfileFeaturesNext;
        pProfileFeaturesNext = &profileUniformBufferStandardLayoutFeatures;
#endif  // VK_KHR_uniform_buffer_standard_layout

#ifdef VK_KHR_shader_draw_parameters
        VkPhysicalDeviceShaderDrawParametersFeatures profileShaderDrawParametersFeatures = {};
        profileShaderDrawParametersFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;
        profileShaderDrawParametersFeatures.pNext = pProfileFeaturesNext;
        pProfileFeaturesNext = &profileShaderDrawParametersFeatures;
#endif  // VK_KHR_shader_draw_parameters

#ifdef VK_KHR_shader_float16_int8
        VkPhysicalDeviceShaderFloat16Int8FeaturesKHR profileShaderFloat16Int8Features = {};
        profileShaderFloat16Int8Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR;
        profileShaderFloat16Int8Features.pNext = pProfileFeaturesNext;
        pProfileFeaturesNext = &profileShaderFloat16Int8Features;
#endif  // VK_KHR_shader_float16_int8

#ifdef VK_KHR_sampler_ycbcr_conversion
        VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR profileSamplerYcbcrConversionFeatures = {};
        profileSamplerYcbcrConversionFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR;
        profileSamplerYcbcrConversionFeatures.pNext = pProfileFeaturesNext;
        pProfileFeaturesNext = &profileSamplerYcbcrConversionFeatures;
#endif  // VK_KHR_sampler_ycbcr_conversion

#ifdef VK_KHR_variable_pointers
        VkPhysicalDeviceVariablePointersFeaturesKHR profileVariablePointersFeatures = {};
        profileVariablePointersFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES_KHR;
        profileVariablePointersFeatures.pNext = pProfileFeaturesNext;
        pProfileFeaturesNext = &profileVariablePointersFeatures;
#endif  // VK_KHR_variable_pointers

#ifdef VK_KHR_portability_subset
        VkPhysicalDevicePortabilitySubsetFeaturesKHR profilePortabilitySubset = {};
        profilePortabilitySubset.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR;
        if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME) == 0) {
            profilePortabilitySubset.pNext = pProfileFeaturesNext;
            pProfileFeaturesNext = &profilePortabilitySubset;
        }
#endif  // VK_KHR_portability_subset

        VkPhysicalDeviceFeatures2 profileFeatures = {};
        profileFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        profileFeatures.pNext = pProfileFeaturesNext;

        vpGetProfileStructures(pProfile, &profileFeatures);

        if (deviceFeatures.features.depthBiasClamp != profileFeatures.features.depthBiasClamp) {
            return result;
        } else if (deviceFeatures.features.depthClamp != profileFeatures.features.depthClamp) {
            return result;
        } else if (deviceFeatures.features.drawIndirectFirstInstance != profileFeatures.features.drawIndirectFirstInstance) {
            return result;
        } else if (deviceFeatures.features.dualSrcBlend != profileFeatures.features.dualSrcBlend) {
            return result;
        } else if (deviceFeatures.features.fillModeNonSolid != profileFeatures.features.fillModeNonSolid) {
            return result;
        } else if (deviceFeatures.features.fragmentStoresAndAtomics != profileFeatures.features.fragmentStoresAndAtomics) {
            return result;
        } else if (deviceFeatures.features.fullDrawIndexUint32 != profileFeatures.features.fullDrawIndexUint32) {
            return result;
        } else if (deviceFeatures.features.imageCubeArray != profileFeatures.features.imageCubeArray) {
            return result;
        } else if (deviceFeatures.features.independentBlend != profileFeatures.features.independentBlend) {
            return result;
        } else if (deviceFeatures.features.inheritedQueries != profileFeatures.features.inheritedQueries) {
            return result;
        } else if (deviceFeatures.features.largePoints != profileFeatures.features.largePoints) {
            return result;
        } else if (deviceFeatures.features.multiDrawIndirect != profileFeatures.features.multiDrawIndirect) {
            return result;
        } else if (deviceFeatures.features.multiViewport != profileFeatures.features.multiViewport) {
            return result;
        } else if (deviceFeatures.features.largePoints != profileFeatures.features.largePoints) {
            return result;
        } else if (deviceFeatures.features.occlusionQueryPrecise != profileFeatures.features.occlusionQueryPrecise) {
            return result;
        } else if (deviceFeatures.features.robustBufferAccess != profileFeatures.features.robustBufferAccess) {
            return result;
        } else if (deviceFeatures.features.sampleRateShading != profileFeatures.features.sampleRateShading) {
            return result;
        } else if (deviceFeatures.features.samplerAnisotropy != profileFeatures.features.samplerAnisotropy) {
            return result;
        } else if (deviceFeatures.features.shaderClipDistance != profileFeatures.features.shaderClipDistance) {
            return result;
        } else if (deviceFeatures.features.shaderImageGatherExtended != profileFeatures.features.shaderImageGatherExtended) {
            return result;
        } else if (deviceFeatures.features.shaderSampledImageArrayDynamicIndexing !=
                   profileFeatures.features.shaderSampledImageArrayDynamicIndexing) {
            return result;
        } else if (deviceFeatures.features.shaderStorageBufferArrayDynamicIndexing !=
                   profileFeatures.features.shaderStorageBufferArrayDynamicIndexing) {
            return result;
        } else if (deviceFeatures.features.shaderStorageImageArrayDynamicIndexing !=
                   profileFeatures.features.shaderStorageImageArrayDynamicIndexing) {
            return result;
        } else if (deviceFeatures.features.shaderStorageImageExtendedFormats !=
                   profileFeatures.features.shaderStorageImageExtendedFormats) {
            return result;
        } else if (deviceFeatures.features.shaderStorageImageWriteWithoutFormat !=
                   profileFeatures.features.shaderStorageImageWriteWithoutFormat) {
            return result;
        } else if (deviceFeatures.features.shaderTessellationAndGeometryPointSize !=
                   profileFeatures.features.shaderTessellationAndGeometryPointSize) {
            return result;
        } else if (deviceFeatures.features.shaderUniformBufferArrayDynamicIndexing !=
                   profileFeatures.features.shaderUniformBufferArrayDynamicIndexing) {
            return result;
        } else if (deviceFeatures.features.tessellationShader != profileFeatures.features.tessellationShader) {
            return result;
        } else if (deviceFeatures.features.textureCompressionBC != profileFeatures.features.textureCompressionBC) {
            return result;
        } else if (deviceFeatures.features.vertexPipelineStoresAndAtomics !=
                   profileFeatures.features.vertexPipelineStoresAndAtomics) {
            return result;
        }

#ifdef VK_KHR_multiview
        if (deviceMultiviewFeatures.multiview != profileMultiviewFeatures.multiview) {
            return result;
        }
#endif  // VK_KHR_multiview

#ifdef VK_KHR_imageless_framebuffer
        if (deviceImagelessFramebufferFeatures.imagelessFramebuffer != profileImagelessFramebufferFeatures.imagelessFramebuffer) {
            return result;
        }
#endif  // VK_KHR_imageless_framebuffer

#ifdef VK_KHR_16bit_storage
        if (deviceStorage16bit.storageBuffer16BitAccess != profileStorage16bitFeatures.storageBuffer16BitAccess) {
            return result;
        } else if (deviceStorage16bit.uniformAndStorageBuffer16BitAccess !=
                   profileStorage16bitFeatures.uniformAndStorageBuffer16BitAccess) {
            return result;
        }
#endif  // VK_KHR_16bit_storage

#ifdef VK_KHR_8bit_storage
        if (device8BitStorageFeatures.storageBuffer8BitAccess != profile8BitStorageFeatures.storageBuffer8BitAccess) {
            return result;
        } else if (device8BitStorageFeatures.storagePushConstant8 != profile8BitStorageFeatures.storagePushConstant8) {
            return result;
        } else if (device8BitStorageFeatures.uniformAndStorageBuffer8BitAccess !=
                   profile8BitStorageFeatures.uniformAndStorageBuffer8BitAccess) {
            return result;
        }
#endif  // VK_KHR_8bit_storage

#ifdef VK_EXT_descriptor_indexing
        if (deviceDescriptorIndexingFeatures.shaderUniformTexelBufferArrayDynamicIndexing !=
            profileDescriptorIndexingFeatures.shaderUniformTexelBufferArrayDynamicIndexing) {
            return result;
        } else if (deviceDescriptorIndexingFeatures.shaderStorageTexelBufferArrayDynamicIndexing !=
                   profileDescriptorIndexingFeatures.shaderStorageTexelBufferArrayDynamicIndexing) {
            return result;
        } else if (deviceDescriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing !=
                   profileDescriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing) {
            return result;
        } else if (deviceDescriptorIndexingFeatures.shaderStorageBufferArrayNonUniformIndexing !=
                   profileDescriptorIndexingFeatures.shaderStorageBufferArrayNonUniformIndexing) {
            return result;
        } else if (deviceDescriptorIndexingFeatures.shaderUniformTexelBufferArrayNonUniformIndexing !=
                   profileDescriptorIndexingFeatures.shaderUniformTexelBufferArrayNonUniformIndexing) {
            return result;
        } else if (deviceDescriptorIndexingFeatures.descriptorBindingSampledImageUpdateAfterBind !=
                   profileDescriptorIndexingFeatures.descriptorBindingSampledImageUpdateAfterBind) {
            return result;
        } else if (deviceDescriptorIndexingFeatures.descriptorBindingStorageImageUpdateAfterBind !=
                   profileDescriptorIndexingFeatures.descriptorBindingStorageImageUpdateAfterBind) {
            return result;
        } else if (deviceDescriptorIndexingFeatures.descriptorBindingStorageBufferUpdateAfterBind !=
                   profileDescriptorIndexingFeatures.descriptorBindingStorageBufferUpdateAfterBind) {
            return result;
        } else if (deviceDescriptorIndexingFeatures.descriptorBindingUniformTexelBufferUpdateAfterBind !=
                   profileDescriptorIndexingFeatures.descriptorBindingUniformTexelBufferUpdateAfterBind) {
            return result;
        } else if (deviceDescriptorIndexingFeatures.descriptorBindingStorageTexelBufferUpdateAfterBind !=
                   profileDescriptorIndexingFeatures.descriptorBindingStorageTexelBufferUpdateAfterBind) {
            return result;
        } else if (deviceDescriptorIndexingFeatures.descriptorBindingUpdateUnusedWhilePending !=
                   profileDescriptorIndexingFeatures.descriptorBindingUpdateUnusedWhilePending) {
            return result;
        } else if (deviceDescriptorIndexingFeatures.descriptorBindingPartiallyBound !=
                   profileDescriptorIndexingFeatures.descriptorBindingPartiallyBound) {
            return result;
        } else if (deviceDescriptorIndexingFeatures.descriptorBindingVariableDescriptorCount !=
                   profileDescriptorIndexingFeatures.descriptorBindingVariableDescriptorCount) {
            return result;
        } else if (deviceDescriptorIndexingFeatures.runtimeDescriptorArray !=
                   profileDescriptorIndexingFeatures.runtimeDescriptorArray) {
            return result;
        }
#endif  // VK_EXT_descriptor_indexing

#ifdef VK_EXT_host_query_reset
        if (deviceQueryResetFeatures.hostQueryReset != profileQueryResetFeatures.hostQueryReset) {
            return result;
        }
#endif  // VK_EXT_host_query_reset

#ifdef VK_KHR_shader_subgroup_extended_types
        if (deviceSubgroupExtendedTypesFeatures.shaderSubgroupExtendedTypes !=
            profileSubgroupExtendedTypesFeatures.shaderSubgroupExtendedTypes) {
            return result;
        }
#endif  // VK_KHR_shader_subgroup_extended_types

#ifdef VK_KHR_uniform_buffer_standard_layout
        if (deviceUniformBufferStandardLayoutFeatures.uniformBufferStandardLayout !=
            profileUniformBufferStandardLayoutFeatures.uniformBufferStandardLayout) {
            return result;
        }
#endif  // VK_KHR_uniform_buffer_standard_layout

#ifdef VK_KHR_shader_draw_parameters
        if (deviceShaderDrawParametersFeatures.shaderDrawParameters != profileShaderDrawParametersFeatures.shaderDrawParameters) {
            return result;
        }
#endif  // VK_KHR_shader_draw_parameters

#ifdef VK_KHR_shader_float16_int8
        if (deviceShaderFloat16Int8Features.shaderInt8 != profileShaderFloat16Int8Features.shaderInt8) {
            return result;
        }
#endif  // VK_KHR_shader_float16_int8

#ifdef VK_KHR_sampler_ycbcr_conversion
        if (deviceSamplerYcbcrConversionFeatures.samplerYcbcrConversion !=
            profileSamplerYcbcrConversionFeatures.samplerYcbcrConversion) {
            return result;
        }
#endif  // VK_KHR_sampler_ycbcr_conversion

#ifdef VK_KHR_variable_pointers
        if (deviceVariablePointerFeatures.variablePointersStorageBuffer !=
            profileVariablePointersFeatures.variablePointersStorageBuffer) {
            return result;
        } else if (deviceVariablePointerFeatures.variablePointers != profileVariablePointersFeatures.variablePointers) {
            return result;
        }
#endif  // VK_KHR_variable_pointers

#ifdef VK_KHR_portability_subset
        if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME) == 0) {
            if (devicePortabilitySubset.vertexAttributeAccessBeyondStride !=
                profilePortabilitySubset.vertexAttributeAccessBeyondStride) {
                return result;
            } else if (devicePortabilitySubset.separateStencilMaskRef != profilePortabilitySubset.separateStencilMaskRef) {
                return result;
            } else if (devicePortabilitySubset.mutableComparisonSamplers != profilePortabilitySubset.mutableComparisonSamplers) {
                return result;
            } else if (devicePortabilitySubset.multisampleArrayImage != profilePortabilitySubset.multisampleArrayImage) {
                return result;
            } else if (devicePortabilitySubset.imageViewFormatSwizzle != profilePortabilitySubset.imageViewFormatSwizzle) {
                return result;
            } else if (devicePortabilitySubset.imageViewFormatReinterpretation !=
                       profilePortabilitySubset.imageViewFormatReinterpretation) {
                return result;
            } else if (devicePortabilitySubset.events != profilePortabilitySubset.events) {
                return result;
            } else if (devicePortabilitySubset.constantAlphaColorBlendFactors !=
                       profilePortabilitySubset.constantAlphaColorBlendFactors) {
                return result;
            }
        }
#endif  // VK_KHR_portability_subset

        void *pDevicePropertiesNext = nullptr;

#ifdef VK_KHR_portability_subset
        VkPhysicalDevicePortabilitySubsetPropertiesKHR devicePortabilitySubsetProperties = {};
        devicePortabilitySubsetProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES_KHR;
        if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME) == 0) {
            devicePortabilitySubsetProperties.pNext = pDevicePropertiesNext;
            pDevicePropertiesNext = &devicePortabilitySubsetProperties;
        }
#endif  // VK_KHR_portability_subset

        VkPhysicalDeviceMaintenance3Properties deviceMaintenance3Properties = {};
        deviceMaintenance3Properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES;
        deviceMaintenance3Properties.pNext = pDevicePropertiesNext;
        pDevicePropertiesNext = &deviceMaintenance3Properties;

        VkPhysicalDeviceDepthStencilResolveProperties deviceDepthStencilResolveProperties = {};
        deviceDepthStencilResolveProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES;
        deviceDepthStencilResolveProperties.pNext = pDevicePropertiesNext;
        pDevicePropertiesNext = &deviceDepthStencilResolveProperties;

        VkPhysicalDeviceInlineUniformBlockPropertiesEXT deviceInlineUniformBlockProperties = {};
        deviceInlineUniformBlockProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_PROPERTIES_EXT;
        deviceInlineUniformBlockProperties.pNext = pDevicePropertiesNext;
        pDevicePropertiesNext = &deviceInlineUniformBlockProperties;

        VkPhysicalDeviceMultiviewProperties deviceMultiviewProperties = {};
        deviceMultiviewProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES;
        deviceMultiviewProperties.pNext = pDevicePropertiesNext;
        pDevicePropertiesNext = &deviceMultiviewProperties;

        VkPhysicalDeviceDescriptorIndexingProperties deviceDescriptorIndexingProperties = {};
        deviceDescriptorIndexingProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES;
        deviceDescriptorIndexingProperties.pNext = pDevicePropertiesNext;
        pDevicePropertiesNext = &deviceDescriptorIndexingProperties;

        VkPhysicalDeviceProperties2 deviceProperties{};
        deviceProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        deviceProperties.pNext = pDevicePropertiesNext;

        vkGetPhysicalDeviceProperties2(physicalDevice, &deviceProperties);

        void *pProfilePropertiesNext = nullptr;

#ifdef VK_KHR_portability_subset
        VkPhysicalDevicePortabilitySubsetPropertiesKHR profilePortabilitySubsetProperties = {};
        profilePortabilitySubsetProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES_KHR;
        if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME) == 0) {
            profilePortabilitySubsetProperties.pNext = pProfilePropertiesNext;
            pProfilePropertiesNext = &profilePortabilitySubsetProperties;
        }
#endif  // VK_KHR_portability_subset

        VkPhysicalDeviceMaintenance3Properties profileMaintenance3Properties = {};
        profileMaintenance3Properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES;
        profileMaintenance3Properties.pNext = pProfilePropertiesNext;
        pProfilePropertiesNext = &profileMaintenance3Properties;

        VkPhysicalDeviceDepthStencilResolveProperties profileDepthStencilResolveProperties = {};
        profileDepthStencilResolveProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES;
        profileDepthStencilResolveProperties.pNext = pProfilePropertiesNext;
        pProfilePropertiesNext = &profileDepthStencilResolveProperties;

        VkPhysicalDeviceInlineUniformBlockPropertiesEXT profileInlineUniformBlockProperties = {};
        profileInlineUniformBlockProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_PROPERTIES_EXT;
        profileInlineUniformBlockProperties.pNext = pProfilePropertiesNext;
        pProfilePropertiesNext = &profileInlineUniformBlockProperties;

        VkPhysicalDeviceMultiviewProperties profileMultiviewProperties = {};
        profileMultiviewProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES;
        profileMultiviewProperties.pNext = pProfilePropertiesNext;
        pProfilePropertiesNext = &profileMultiviewProperties;

        VkPhysicalDeviceDescriptorIndexingProperties profileDescriptorIndexingProperties = {};
        profileDescriptorIndexingProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES;
        profileDescriptorIndexingProperties.pNext = pProfilePropertiesNext;
        pProfilePropertiesNext = &profileDescriptorIndexingProperties;

        VkPhysicalDeviceProperties2 profileProperties{};
        profileProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        profileProperties.pNext = pProfilePropertiesNext;

        vpGetProfileStructures(pProfile, &profileProperties);

        if (deviceProperties.properties.limits.maxImageDimension1D < profileProperties.properties.limits.maxImageDimension1D) {
            return result;
        } else if (deviceProperties.properties.limits.maxImageDimension2D <
                   profileProperties.properties.limits.maxImageDimension2D) {
            return result;
        } else if (deviceProperties.properties.limits.maxImageDimension3D <
                   profileProperties.properties.limits.maxImageDimension3D) {
            return result;
        } else if (deviceProperties.properties.limits.maxImageDimensionCube <
                   profileProperties.properties.limits.maxImageDimensionCube) {
            return result;
        } else if (deviceProperties.properties.limits.maxImageArrayLayers <
                   profileProperties.properties.limits.maxImageArrayLayers) {
            return result;
        } else if (deviceProperties.properties.limits.maxTexelBufferElements <
                   profileProperties.properties.limits.maxTexelBufferElements) {
            return result;
        } else if (deviceProperties.properties.limits.maxUniformBufferRange <
                   profileProperties.properties.limits.maxUniformBufferRange) {
            return result;
        } else if (deviceProperties.properties.limits.maxStorageBufferRange <
                   profileProperties.properties.limits.maxStorageBufferRange) {
            return result;
        } else if (deviceProperties.properties.limits.maxPushConstantsSize <
                   profileProperties.properties.limits.maxPushConstantsSize) {
            return result;
        } else if (deviceProperties.properties.limits.maxMemoryAllocationCount <
                   profileProperties.properties.limits.maxMemoryAllocationCount) {
            return result;
        } else if (deviceProperties.properties.limits.maxSamplerAllocationCount <
                   profileProperties.properties.limits.maxSamplerAllocationCount) {
            return result;
        } else if (deviceProperties.properties.limits.bufferImageGranularity >
                   profileProperties.properties.limits.bufferImageGranularity) {
            return result;
        } else if (deviceProperties.properties.limits.sparseAddressSpaceSize <
                   profileProperties.properties.limits.sparseAddressSpaceSize) {
            return result;
        } else if (deviceProperties.properties.limits.maxBoundDescriptorSets <
                   profileProperties.properties.limits.maxBoundDescriptorSets) {
            return result;
        } else if (deviceProperties.properties.limits.maxPerStageDescriptorSamplers <
                   profileProperties.properties.limits.maxPerStageDescriptorSamplers) {
            return result;
        } else if (deviceProperties.properties.limits.maxPerStageDescriptorUniformBuffers <
                   profileProperties.properties.limits.maxPerStageDescriptorUniformBuffers) {
            return result;
        } else if (deviceProperties.properties.limits.maxPerStageDescriptorStorageBuffers <
                   profileProperties.properties.limits.maxPerStageDescriptorStorageBuffers) {
            return result;
        } else if (deviceProperties.properties.limits.maxPerStageDescriptorSampledImages <
                   profileProperties.properties.limits.maxPerStageDescriptorSampledImages) {
            return result;
        } else if (deviceProperties.properties.limits.maxPerStageDescriptorStorageImages <
                   profileProperties.properties.limits.maxPerStageDescriptorStorageImages) {
            return result;
        } else if (deviceProperties.properties.limits.maxPerStageDescriptorInputAttachments <
                   profileProperties.properties.limits.maxPerStageDescriptorInputAttachments) {
            return result;
        } else if (deviceProperties.properties.limits.maxPerStageResources <
                   profileProperties.properties.limits.maxPerStageResources) {
            return result;
        } else if (deviceProperties.properties.limits.maxDescriptorSetSamplers <
                   profileProperties.properties.limits.maxDescriptorSetSamplers) {
            return result;
        } else if (deviceProperties.properties.limits.maxDescriptorSetUniformBuffers <
                   profileProperties.properties.limits.maxDescriptorSetUniformBuffers) {
            return result;
        } else if (deviceProperties.properties.limits.maxDescriptorSetUniformBuffersDynamic <
                   profileProperties.properties.limits.maxDescriptorSetUniformBuffersDynamic) {
            return result;
        } else if (deviceProperties.properties.limits.maxDescriptorSetStorageBuffers <
                   profileProperties.properties.limits.maxDescriptorSetStorageBuffers) {
            return result;
        } else if (deviceProperties.properties.limits.maxDescriptorSetStorageBuffersDynamic <
                   profileProperties.properties.limits.maxDescriptorSetStorageBuffersDynamic) {
            return result;
        } else if (deviceProperties.properties.limits.maxDescriptorSetSampledImages <
                   profileProperties.properties.limits.maxDescriptorSetSampledImages) {
            return result;
        } else if (deviceProperties.properties.limits.maxDescriptorSetStorageImages <
                   profileProperties.properties.limits.maxDescriptorSetStorageImages) {
            return result;
        } else if (deviceProperties.properties.limits.maxDescriptorSetInputAttachments <
                   profileProperties.properties.limits.maxDescriptorSetInputAttachments) {
            return result;
        } else if (deviceProperties.properties.limits.maxVertexInputAttributes <
                   profileProperties.properties.limits.maxVertexInputAttributes) {
            return result;
        } else if (deviceProperties.properties.limits.maxVertexInputBindings <
                   profileProperties.properties.limits.maxVertexInputBindings) {
            return result;
        } else if (deviceProperties.properties.limits.maxVertexInputAttributeOffset <
                   profileProperties.properties.limits.maxVertexInputAttributeOffset) {
            return result;
        } else if (deviceProperties.properties.limits.maxVertexInputBindingStride <
                   profileProperties.properties.limits.maxVertexInputBindingStride) {
            return result;
        } else if (deviceProperties.properties.limits.maxVertexOutputComponents <
                   profileProperties.properties.limits.maxVertexOutputComponents) {
            return result;
        } else if (deviceProperties.properties.limits.maxTessellationGenerationLevel <
                   profileProperties.properties.limits.maxTessellationGenerationLevel) {
            return result;
        } else if (deviceProperties.properties.limits.maxTessellationPatchSize <
                   profileProperties.properties.limits.maxTessellationPatchSize) {
            return result;
        } else if (deviceProperties.properties.limits.maxTessellationControlPerVertexInputComponents <
                   profileProperties.properties.limits.maxTessellationControlPerVertexInputComponents) {
            return result;
        } else if (deviceProperties.properties.limits.maxTessellationControlPerVertexOutputComponents <
                   profileProperties.properties.limits.maxTessellationControlPerVertexOutputComponents) {
            return result;
        } else if (deviceProperties.properties.limits.maxTessellationControlPerPatchOutputComponents <
                   profileProperties.properties.limits.maxTessellationControlPerPatchOutputComponents) {
            return result;
        } else if (deviceProperties.properties.limits.maxTessellationControlTotalOutputComponents <
                   profileProperties.properties.limits.maxTessellationControlTotalOutputComponents) {
            return result;
        } else if (deviceProperties.properties.limits.maxTessellationEvaluationInputComponents <
                   profileProperties.properties.limits.maxTessellationEvaluationInputComponents) {
            return result;
        } else if (deviceProperties.properties.limits.maxTessellationEvaluationOutputComponents <
                   profileProperties.properties.limits.maxTessellationEvaluationOutputComponents) {
            return result;
        } else if (deviceProperties.properties.limits.maxFragmentInputComponents <
                   profileProperties.properties.limits.maxFragmentInputComponents) {
            return result;
        } else if (deviceProperties.properties.limits.maxFragmentOutputAttachments <
                   profileProperties.properties.limits.maxFragmentOutputAttachments) {
            return result;
        } else if (deviceProperties.properties.limits.maxFragmentDualSrcAttachments <
                   profileProperties.properties.limits.maxFragmentDualSrcAttachments) {
            return result;
        } else if (deviceProperties.properties.limits.maxFragmentCombinedOutputResources <
                   profileProperties.properties.limits.maxFragmentCombinedOutputResources) {
            return result;
        } else if (deviceProperties.properties.limits.maxComputeSharedMemorySize <
                   profileProperties.properties.limits.maxComputeSharedMemorySize) {
            return result;
        } else if (deviceProperties.properties.limits.maxComputeWorkGroupCount[0] <
                   profileProperties.properties.limits.maxComputeWorkGroupCount[0]) {
            return result;
        } else if (deviceProperties.properties.limits.maxComputeWorkGroupCount[1] <
                   profileProperties.properties.limits.maxComputeWorkGroupCount[1]) {
            return result;
        } else if (deviceProperties.properties.limits.maxComputeWorkGroupCount[2] <
                   profileProperties.properties.limits.maxComputeWorkGroupCount[2]) {
            return result;
        } else if (deviceProperties.properties.limits.maxComputeWorkGroupInvocations <
                   profileProperties.properties.limits.maxComputeWorkGroupInvocations) {
            return result;
        } else if (deviceProperties.properties.limits.maxComputeWorkGroupSize[0] <
                   profileProperties.properties.limits.maxComputeWorkGroupSize[0]) {
            return result;
        } else if (deviceProperties.properties.limits.maxComputeWorkGroupSize[1] <
                   profileProperties.properties.limits.maxComputeWorkGroupSize[1]) {
            return result;
        } else if (deviceProperties.properties.limits.maxComputeWorkGroupSize[2] <
                   profileProperties.properties.limits.maxComputeWorkGroupSize[2]) {
            return result;
        } else if (deviceProperties.properties.limits.subPixelPrecisionBits <
                   profileProperties.properties.limits.subPixelPrecisionBits) {
            return result;
        } else if (deviceProperties.properties.limits.subTexelPrecisionBits <
                   profileProperties.properties.limits.subTexelPrecisionBits) {
            return result;
        } else if (deviceProperties.properties.limits.mipmapPrecisionBits <
                   profileProperties.properties.limits.mipmapPrecisionBits) {
            return result;
        } else if (deviceProperties.properties.limits.maxDrawIndexedIndexValue <
                   profileProperties.properties.limits.maxDrawIndexedIndexValue) {
            return result;
        } else if (deviceProperties.properties.limits.maxDrawIndirectCount <
                   profileProperties.properties.limits.maxDrawIndirectCount) {
            return result;
        } else if (deviceProperties.properties.limits.maxSamplerLodBias < profileProperties.properties.limits.maxSamplerLodBias) {
            return result;
        } else if (deviceProperties.properties.limits.maxSamplerAnisotropy <
                   profileProperties.properties.limits.maxSamplerAnisotropy) {
            return result;
        } else if (deviceProperties.properties.limits.maxViewports < profileProperties.properties.limits.maxViewports) {
            return result;
        } else if (deviceProperties.properties.limits.maxViewportDimensions[0] <
                   profileProperties.properties.limits.maxViewportDimensions[0]) {
            return result;
        } else if (deviceProperties.properties.limits.maxViewportDimensions[1] <
                   profileProperties.properties.limits.maxViewportDimensions[1]) {
            return result;
        } else if (deviceProperties.properties.limits.viewportBoundsRange[0] >
                   profileProperties.properties.limits.viewportBoundsRange[0]) {
            return result;
        } else if (deviceProperties.properties.limits.viewportBoundsRange[1] <
                   profileProperties.properties.limits.viewportBoundsRange[1]) {
            return result;
        } else if (deviceProperties.properties.limits.minMemoryMapAlignment <
                   profileProperties.properties.limits.minMemoryMapAlignment) {
            return result;
        } else if (deviceProperties.properties.limits.minTexelBufferOffsetAlignment >
                   profileProperties.properties.limits.minTexelBufferOffsetAlignment) {
            return result;
        } else if (deviceProperties.properties.limits.minUniformBufferOffsetAlignment >
                   profileProperties.properties.limits.minUniformBufferOffsetAlignment) {
            return result;
        } else if (deviceProperties.properties.limits.minStorageBufferOffsetAlignment >
                   profileProperties.properties.limits.minStorageBufferOffsetAlignment) {
            return result;
        } else if (deviceProperties.properties.limits.minTexelOffset > profileProperties.properties.limits.minTexelOffset) {
            return result;
        } else if (deviceProperties.properties.limits.maxTexelOffset < profileProperties.properties.limits.maxTexelOffset) {
            return result;
        } else if (deviceProperties.properties.limits.minTexelGatherOffset >
                   profileProperties.properties.limits.minTexelGatherOffset) {
            return result;
        } else if (deviceProperties.properties.limits.maxTexelGatherOffset <
                   profileProperties.properties.limits.maxTexelGatherOffset) {
            return result;
        } else if (deviceProperties.properties.limits.minInterpolationOffset >
                   profileProperties.properties.limits.minInterpolationOffset) {
            return result;
        } else if (deviceProperties.properties.limits.maxInterpolationOffset <
                   profileProperties.properties.limits.maxInterpolationOffset) {
            return result;
        } else if (deviceProperties.properties.limits.subPixelInterpolationOffsetBits <
                   profileProperties.properties.limits.subPixelInterpolationOffsetBits) {
            return result;
        } else if (deviceProperties.properties.limits.maxFramebufferWidth <
                   profileProperties.properties.limits.maxFramebufferWidth) {
            return result;
        } else if (deviceProperties.properties.limits.maxFramebufferHeight <
                   profileProperties.properties.limits.maxFramebufferHeight) {
            return result;
        } else if (deviceProperties.properties.limits.maxFramebufferLayers <
                   profileProperties.properties.limits.maxFramebufferLayers) {
            return result;
        } else if (deviceProperties.properties.limits.framebufferColorSampleCounts <
                   profileProperties.properties.limits.framebufferColorSampleCounts) {
            return result;
        } else if (deviceProperties.properties.limits.framebufferDepthSampleCounts <
                   profileProperties.properties.limits.framebufferDepthSampleCounts) {
            return result;
        } else if (deviceProperties.properties.limits.framebufferStencilSampleCounts <
                   profileProperties.properties.limits.framebufferStencilSampleCounts) {
            return result;
        } else if (deviceProperties.properties.limits.framebufferNoAttachmentsSampleCounts <
                   profileProperties.properties.limits.framebufferNoAttachmentsSampleCounts) {
            return result;
        } else if (deviceProperties.properties.limits.maxColorAttachments <
                   profileProperties.properties.limits.maxColorAttachments) {
            return result;
        } else if (deviceProperties.properties.limits.sampledImageColorSampleCounts <
                   profileProperties.properties.limits.sampledImageColorSampleCounts) {
            return result;
        } else if (deviceProperties.properties.limits.sampledImageIntegerSampleCounts <
                   profileProperties.properties.limits.sampledImageIntegerSampleCounts) {
            return result;
        } else if (deviceProperties.properties.limits.sampledImageDepthSampleCounts <
                   profileProperties.properties.limits.sampledImageDepthSampleCounts) {
            return result;
        } else if (deviceProperties.properties.limits.sampledImageStencilSampleCounts <
                   profileProperties.properties.limits.sampledImageStencilSampleCounts) {
            return result;
        } else if (deviceProperties.properties.limits.storageImageSampleCounts <
                   profileProperties.properties.limits.storageImageSampleCounts) {
            return result;
        } else if (deviceProperties.properties.limits.maxSampleMaskWords > profileProperties.properties.limits.maxSampleMaskWords) {
            return result;
        } else if (deviceProperties.properties.limits.maxClipDistances < profileProperties.properties.limits.maxClipDistances) {
            return result;
        } else if (deviceProperties.properties.limits.maxCullDistances < profileProperties.properties.limits.maxCullDistances) {
            return result;
        } else if (deviceProperties.properties.limits.maxCombinedClipAndCullDistances <
                   profileProperties.properties.limits.maxCombinedClipAndCullDistances) {
            return result;
        } else if (deviceProperties.properties.limits.discreteQueuePriorities <
                   profileProperties.properties.limits.discreteQueuePriorities) {
            return result;
        } else if (deviceProperties.properties.limits.pointSizeRange[0] > profileProperties.properties.limits.pointSizeRange[0]) {
            return result;
        } else if (deviceProperties.properties.limits.pointSizeRange[1] < profileProperties.properties.limits.pointSizeRange[1]) {
            return result;
        } else if (deviceProperties.properties.limits.lineWidthRange[0] > profileProperties.properties.limits.lineWidthRange[0]) {
            return result;
        } else if (deviceProperties.properties.limits.lineWidthRange[1] < profileProperties.properties.limits.lineWidthRange[1]) {
            return result;
        } else if (deviceProperties.properties.limits.pointSizeGranularity >
                   profileProperties.properties.limits.pointSizeGranularity) {
            return result;
        } else if (deviceProperties.properties.limits.lineWidthGranularity >
                   profileProperties.properties.limits.lineWidthGranularity) {
            return result;
        }

        if (deviceDescriptorIndexingProperties.maxUpdateAfterBindDescriptorsInAllPools <
            profileDescriptorIndexingProperties.maxUpdateAfterBindDescriptorsInAllPools) {
            return result;
        } else if (deviceDescriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindSamplers <
                   profileDescriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindSamplers) {
            return result;
        } else if (deviceDescriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindUniformBuffers <
                   profileDescriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindUniformBuffers) {
            return result;
        } else if (deviceDescriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindStorageBuffers <
                   profileDescriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindStorageBuffers) {
            return result;
        } else if (deviceDescriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindSampledImages <
                   profileDescriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindSampledImages) {
            return result;
        } else if (deviceDescriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindStorageImages <
                   profileDescriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindStorageImages) {
            return result;
        } else if (deviceDescriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindInputAttachments <
                   profileDescriptorIndexingProperties.maxPerStageDescriptorUpdateAfterBindInputAttachments) {
            return result;
        } else if (deviceDescriptorIndexingProperties.maxPerStageUpdateAfterBindResources <
                   profileDescriptorIndexingProperties.maxPerStageUpdateAfterBindResources) {
            return result;
        } else if (deviceDescriptorIndexingProperties.maxDescriptorSetUpdateAfterBindSamplers <
                   profileDescriptorIndexingProperties.maxDescriptorSetUpdateAfterBindSamplers) {
            return result;
        } else if (deviceDescriptorIndexingProperties.maxDescriptorSetUpdateAfterBindUniformBuffers <
                   profileDescriptorIndexingProperties.maxDescriptorSetUpdateAfterBindUniformBuffers) {
            return result;
        } else if (deviceDescriptorIndexingProperties.maxDescriptorSetUpdateAfterBindUniformBuffersDynamic <
                   profileDescriptorIndexingProperties.maxDescriptorSetUpdateAfterBindUniformBuffersDynamic) {
            return result;
        } else if (deviceDescriptorIndexingProperties.maxDescriptorSetUpdateAfterBindStorageBuffers <
                   profileDescriptorIndexingProperties.maxDescriptorSetUpdateAfterBindStorageBuffers) {
            return result;
        } else if (deviceDescriptorIndexingProperties.maxDescriptorSetUpdateAfterBindStorageBuffersDynamic <
                   profileDescriptorIndexingProperties.maxDescriptorSetUpdateAfterBindStorageBuffersDynamic) {
            return result;
        } else if (deviceDescriptorIndexingProperties.maxDescriptorSetUpdateAfterBindSampledImages <
                   profileDescriptorIndexingProperties.maxDescriptorSetUpdateAfterBindSampledImages) {
            return result;
        } else if (deviceDescriptorIndexingProperties.maxDescriptorSetUpdateAfterBindStorageImages <
                   profileDescriptorIndexingProperties.maxDescriptorSetUpdateAfterBindStorageImages) {
            return result;
        } else if (deviceDescriptorIndexingProperties.maxDescriptorSetUpdateAfterBindInputAttachments <
                   profileDescriptorIndexingProperties.maxDescriptorSetUpdateAfterBindInputAttachments) {
            return result;
        }

        if (deviceMultiviewProperties.maxMultiviewInstanceIndex < profileMultiviewProperties.maxMultiviewInstanceIndex) {
            return result;
        } else if (deviceMultiviewProperties.maxMultiviewViewCount < profileMultiviewProperties.maxMultiviewViewCount) {
            return result;
        }

        if (deviceInlineUniformBlockProperties.maxInlineUniformBlockSize <
            profileInlineUniformBlockProperties.maxInlineUniformBlockSize) {
            return result;
        }

        if (deviceDepthStencilResolveProperties.independentResolve != profileDepthStencilResolveProperties.independentResolve) {
            return result;
        } else if (deviceDepthStencilResolveProperties.independentResolveNone !=
                   profileDepthStencilResolveProperties.independentResolveNone) {
            return result;
        } else if ((deviceDepthStencilResolveProperties.supportedDepthResolveModes &
                    profileDepthStencilResolveProperties.supportedDepthResolveModes) !=
                   profileDepthStencilResolveProperties.supportedDepthResolveModes) {
            return result;
        } else if ((deviceDepthStencilResolveProperties.supportedStencilResolveModes &
                    profileDepthStencilResolveProperties.supportedStencilResolveModes) !=
                   profileDepthStencilResolveProperties.supportedStencilResolveModes) {
            return result;
        }

        if (deviceMaintenance3Properties.maxPerSetDescriptors < profileMaintenance3Properties.maxPerSetDescriptors) {
            return result;
        } else if (deviceMaintenance3Properties.maxMemoryAllocationSize < profileMaintenance3Properties.maxMemoryAllocationSize) {
            return result;
        }
#ifdef VK_KHR_portability_subset
        if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME) == 0) {
            if (devicePortabilitySubsetProperties.minVertexInputBindingStrideAlignment <
                profilePortabilitySubsetProperties.minVertexInputBindingStrideAlignment) {
                return result;
            }
        }
#endif  // VK_KHR_portability_subset
    } else {
        return result;
    }

    *pSupported = VK_TRUE;

    return result;
}

VP_INLINE VkResult vpGetProfileExtensionProperties(const VpProfileProperties *pProfile, uint32_t *pPropertyCount,
                                                   VkExtensionProperties *pProperties) {
    if (pProperties == nullptr) {
        if (strcmp(pProfile->profileName, VP_KHR_ROADMAP_2022_NAME) == 0) {
            *pPropertyCount = _vpCountOf(_VP_KHR_ROADMAP_2022_EXTENSIONS);
        } else if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME) == 0) {
            *pPropertyCount = _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_EXTENSIONS);
        }
#ifdef VK_ENABLE_BETA_EXTENSIONS
        else if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME) == 0) {
            *pPropertyCount = _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_EXTENSIONS);
        }
#endif
        return VK_SUCCESS;
    }

    if (strcmp(pProfile->profileName, VP_KHR_ROADMAP_2022_NAME) == 0) {
        *pPropertyCount = std::min<uint32_t>(_vpCountOf(_VP_KHR_ROADMAP_2022_EXTENSIONS), *pPropertyCount);
        for (uint32_t i = 0, n = *pPropertyCount; i < n; ++i) {
            pProperties[i] = _VP_KHR_ROADMAP_2022_EXTENSIONS[i];
        }
        return _vpCountOf(_VP_KHR_ROADMAP_2022_EXTENSIONS) <= *pPropertyCount ? VK_SUCCESS : VK_INCOMPLETE;
    } else if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME) == 0) {
        *pPropertyCount = std::min<uint32_t>(_vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_EXTENSIONS), *pPropertyCount);
        for (uint32_t i = 0, n = *pPropertyCount; i < n; ++i) {
            pProperties[i] = _VP_LUNARG_DESKTOP_PORTABILITY_2021_EXTENSIONS[i];
        }
        return _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_EXTENSIONS) <= *pPropertyCount ? VK_SUCCESS : VK_INCOMPLETE;
    }
#ifdef VK_ENABLE_BETA_EXTENSIONS
    else if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME) == 0) {
        *pPropertyCount = std::min<uint32_t>(_vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_EXTENSIONS), *pPropertyCount);
        for (uint32_t i = 0, n = *pPropertyCount; i < n; ++i) {
            pProperties[i] = _VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_EXTENSIONS[i];
        }
        return _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_EXTENSIONS) <= *pPropertyCount ? VK_SUCCESS : VK_INCOMPLETE;
    }
#endif
    else {
        return VK_SUCCESS;
    }
}

VP_INLINE VkResult vpGetProfileStructureProperties(const VpProfileProperties *pProfile, uint32_t *pPropertyCount,
                                                   VpStructureProperties *pProperties) {
    if (pProperties == nullptr) {
        if (strcmp(pProfile->profileName, VP_KHR_ROADMAP_2022_NAME) == 0) {
            *pPropertyCount = _vpCountOf(_VP_KHR_ROADMAP_2022_STRUCTURE_PROPERTIES);
        } else if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME) == 0) {
            *pPropertyCount = _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_STRUCTURE_PROPERTIES);
        }
#ifdef VK_ENABLE_BETA_EXTENSIONS
        else if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME) == 0) {
            *pPropertyCount = _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_STRUCTURE_PROPERTIES);
        }
#endif
        else {
            *pPropertyCount = 0;
        }
        return VK_SUCCESS;
    }

    if (strcmp(pProfile->profileName, VP_KHR_ROADMAP_2022_NAME) == 0) {
        *pPropertyCount = std::min<uint32_t>(_vpCountOf(_VP_KHR_ROADMAP_2022_STRUCTURE_PROPERTIES), *pPropertyCount);
        for (uint32_t i = 0, n = *pPropertyCount; i < n; ++i) {
            pProperties[i] = _VP_KHR_ROADMAP_2022_STRUCTURE_PROPERTIES[i];
        }
        return _vpCountOf(_VP_KHR_ROADMAP_2022_STRUCTURE_PROPERTIES) <= *pPropertyCount ? VK_SUCCESS : VK_INCOMPLETE;
    } else if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME) == 0) {
        *pPropertyCount = std::min<uint32_t>(_vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_STRUCTURE_PROPERTIES), *pPropertyCount);
        for (uint32_t i = 0, n = *pPropertyCount; i < n; ++i) {
            pProperties[i] = _VP_LUNARG_DESKTOP_PORTABILITY_2021_STRUCTURE_PROPERTIES[i];
        }
        return _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_STRUCTURE_PROPERTIES) <= *pPropertyCount ? VK_SUCCESS : VK_INCOMPLETE;
    }
#ifdef VK_ENABLE_BETA_EXTENSIONS
    else if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME) == 0) {
        *pPropertyCount =
            std::min<uint32_t>(_vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_STRUCTURE_PROPERTIES), *pPropertyCount);
        for (uint32_t i = 0, n = *pPropertyCount; i < n; ++i) {
            pProperties[i] = _VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_STRUCTURE_PROPERTIES[i];
        }
        return _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_STRUCTURE_PROPERTIES) <= *pPropertyCount ? VK_SUCCESS
                                                                                                              : VK_INCOMPLETE;
    }
#endif
    else {
        return VK_SUCCESS;
    }
}

VP_INLINE VkResult vpGetProfileFormats(const VpProfileProperties *pProfile, uint32_t *pPropertyCount, VkFormat *pProperties) {
    if (pProperties == nullptr) {
        if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME) == 0) {
            *pPropertyCount = _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_FORMATS);
        }
#ifdef VK_ENABLE_BETA_EXTENSIONS
        else if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME) == 0) {
            *pPropertyCount = _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_FORMATS);
        }
#endif
        else {
            *pPropertyCount = 0;
        }
        return VK_SUCCESS;
    }

    if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME) == 0) {
        *pPropertyCount = std::min<uint32_t>(_vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_FORMATS), *pPropertyCount);
        for (uint32_t i = 0, n = *pPropertyCount; i < n; ++i) {
            pProperties[i] = _VP_LUNARG_DESKTOP_PORTABILITY_2021_FORMATS[i].format;
        }
        return _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_FORMATS) <= *pPropertyCount ? VK_SUCCESS : VK_INCOMPLETE;
    }
#ifdef VK_ENABLE_BETA_EXTENSIONS
    else if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME) == 0) {
        *pPropertyCount = std::min<uint32_t>(_vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_FORMATS), *pPropertyCount);
        for (uint32_t i = 0, n = *pPropertyCount; i < n; ++i) {
            pProperties[i] = _VP_LUNARG_DESKTOP_PORTABILITY_2021_FORMATS[i].format;
        }
        return _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_FORMATS) <= *pPropertyCount ? VK_SUCCESS : VK_INCOMPLETE;
    }
#endif
    else {
        return VK_SUCCESS;
    }
}

VP_INLINE void vpGetProfileFormatProperties(const VpProfileProperties *pProfile, VkFormat format, void *pNext) {
    if (pProfile == nullptr || pNext == nullptr) {
        return;
    }

    struct VkStruct {
        VkStructureType sType;
        void *pNext;
    };

    VkStruct *p = static_cast<VkStruct *>(pNext);

    if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME) == 0
#ifdef VK_ENABLE_BETA_EXTENSIONS
        || strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME) == 0
#endif
    ) {
        for (std::size_t i = 0, n = _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_FORMATS); i < n; ++i) {
            const VpFormatProperties &props = _VP_LUNARG_DESKTOP_PORTABILITY_2021_FORMATS[i];
            if (props.format != format) continue;

            while (p != nullptr) {
                switch (p->sType) {
#ifdef VK_KHR_get_physical_device_properties2
                    case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR: {
                        VkFormatProperties2 *pProperties = (VkFormatProperties2 *)p;
                        pProperties->formatProperties.bufferFeatures = static_cast<VkFormatFeatureFlags>(props.bufferFeatures);
                        pProperties->formatProperties.linearTilingFeatures =
                            static_cast<VkFormatFeatureFlags>(props.linearTilingFeatures);
                        pProperties->formatProperties.optimalTilingFeatures =
                            static_cast<VkFormatFeatureFlags>(props.optimalTilingFeatures);
                    } break;
#endif  // VK_KHR_get_physical_device_properties2
#ifdef VK_KHR_format_feature_flags2
                    case VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3_KHR: {
                        VkFormatProperties3KHR *pProperties = (VkFormatProperties3KHR *)p;
                        pProperties->bufferFeatures = props.bufferFeatures;
                        pProperties->linearTilingFeatures = props.linearTilingFeatures;
                        pProperties->optimalTilingFeatures = props.optimalTilingFeatures;
                    } break;
#endif  // VK_KHR_format_feature_flags2
                }
                p = static_cast<VkStruct *>(p->pNext);
            }
        }
    }
}

VP_INLINE VkResult vpGetProfileQueueFamilies(const VpProfileProperties *pProfile, uint32_t *pQueueFamilyPropertiesCount,
                                             VkQueueFamilyProperties *pQueueFamilyProperties) {
    if (pQueueFamilyProperties == nullptr) {
        if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME) == 0) {
            *pQueueFamilyPropertiesCount = _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_QUEUE_FAMILY_PROPERTIES);
        }
#ifdef VK_ENABLE_BETA_EXTENSIONS
        else if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME) == 0) {
            *pQueueFamilyPropertiesCount = _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_QUEUE_FAMILY_PROPERTIES);
        }
#endif
        else {
            *pQueueFamilyPropertiesCount = 0;
        }
        return VK_SUCCESS;
    }

    if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME) == 0) {
        std::size_t n = std::min<std::size_t>(_vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_QUEUE_FAMILY_PROPERTIES),
                                              *pQueueFamilyPropertiesCount);
        for (std::size_t i = 0; i < n; ++i) {
            pQueueFamilyProperties[i] = _VP_LUNARG_DESKTOP_PORTABILITY_2021_QUEUE_FAMILY_PROPERTIES[i];
        }
        return _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_QUEUE_FAMILY_PROPERTIES) <= *pQueueFamilyPropertiesCount
                   ? VK_SUCCESS
                   : VK_INCOMPLETE;
    }
#ifdef VK_ENABLE_BETA_EXTENSIONS
    else if (strcmp(pProfile->profileName, VP_LUNARG_DESKTOP_PORTABILITY_2021_SUBSET_NAME) == 0) {
        std::size_t n = std::min<std::size_t>(_vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_QUEUE_FAMILY_PROPERTIES),
                                              *pQueueFamilyPropertiesCount);
        for (std::size_t i = 0; i < n; ++i) {
            pQueueFamilyProperties[i] = _VP_LUNARG_DESKTOP_PORTABILITY_2021_QUEUE_FAMILY_PROPERTIES[i];
        }
        return _vpCountOf(_VP_LUNARG_DESKTOP_PORTABILITY_2021_QUEUE_FAMILY_PROPERTIES) <= *pQueueFamilyPropertiesCount
                   ? VK_SUCCESS
                   : VK_INCOMPLETE;
    }
#endif
    else {
        return VK_SUCCESS;
    }
}
