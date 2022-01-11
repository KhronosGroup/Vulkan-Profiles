

#include <vulkan/vulkan_core.h>

#include <gtest/gtest.h>
#include "profiles_test_helper.h"

#ifdef _WIN32
#ifdef _DEBUG
static const char* CONFIG_PATH = "bin/Debug";
#else
static const char* CONFIG_PATH = "bin/Release";
#endif
#else 
static const char* CONFIG_PATH = "lib";
#endif

TEST(profiles, TestDesktopPortability2022Limits) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    std::vector<std::string> filepaths = {TEST_SOURCE_PATH "/../../profiles/VP_LUNARG_desktop_portability_2021.json"};
    profiles_test::setProfilesFilenames(filepaths);
    profiles_test::setProfilesProfileName("VP_LUNARG_desktop_portability_2021");
    profiles_test::setProfilesEmulatePortabilitySubsetExtension(true);

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu;
    err = inst_builder.getPhysicalDevice(&gpu);
    if (err != VK_SUCCESS) {
        printf("Profile not supported on device, skipping test.\n");
        vkDestroyInstance(test_inst, nullptr);
        return;
    }

    VkPhysicalDeviceProperties gpu_props{};
    vkGetPhysicalDeviceProperties(gpu, &gpu_props);

    VkPhysicalDeviceFeatures gpu_feats{};
    vkGetPhysicalDeviceFeatures(gpu, &gpu_feats);

    VkPhysicalDeviceMemoryProperties gpu_mem_props{};
    vkGetPhysicalDeviceMemoryProperties(gpu, &gpu_mem_props);

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_family_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_family_props(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_family_count, queue_family_props.data());

    // Limits
    EXPECT_GE(gpu_props.limits.maxImageDimension1D, 16384u);
    EXPECT_GE(gpu_props.limits.maxImageDimension2D, 16384u);
    EXPECT_GE(gpu_props.limits.maxImageDimension3D, 2048u);
    EXPECT_GE(gpu_props.limits.maxImageDimensionCube, 16384u);
    EXPECT_GE(gpu_props.limits.maxImageArrayLayers, 2048u);
    EXPECT_GE(gpu_props.limits.maxTexelBufferElements, 67108900u);
    EXPECT_GE(gpu_props.limits.maxUniformBufferRange, 65536u);
    EXPECT_GE(gpu_props.limits.maxStorageBufferRange, 134217728u);
    EXPECT_GE(gpu_props.limits.maxPushConstantsSize, 128u);
    EXPECT_GE(gpu_props.limits.maxMemoryAllocationCount, 4096u);
    EXPECT_GE(gpu_props.limits.maxSamplerAllocationCount, 1024u);
    EXPECT_LE(gpu_props.limits.bufferImageGranularity, 1024);
    EXPECT_GE(gpu_props.limits.sparseAddressSpaceSize, 1073741824);
    EXPECT_GE(gpu_props.limits.maxBoundDescriptorSets, 8u);
    EXPECT_GE(gpu_props.limits.maxPerStageDescriptorSamplers, 16u);
    EXPECT_GE(gpu_props.limits.maxPerStageDescriptorUniformBuffers, 15u);
    EXPECT_GE(gpu_props.limits.maxPerStageDescriptorStorageBuffers, 16u);
    EXPECT_GE(gpu_props.limits.maxPerStageDescriptorSampledImages, 128u);
    EXPECT_GE(gpu_props.limits.maxPerStageDescriptorStorageImages, 8u);
    EXPECT_GE(gpu_props.limits.maxPerStageDescriptorInputAttachments, 8u);
    EXPECT_GE(gpu_props.limits.maxPerStageResources, 128u);
    EXPECT_GE(gpu_props.limits.maxDescriptorSetSamplers, 80u);
    EXPECT_GE(gpu_props.limits.maxDescriptorSetUniformBuffers, 90u);
    EXPECT_GE(gpu_props.limits.maxDescriptorSetUniformBuffersDynamic, 8u);
    EXPECT_GE(gpu_props.limits.maxDescriptorSetStorageBuffers, 155u);
    EXPECT_GE(gpu_props.limits.maxDescriptorSetStorageBuffersDynamic, 8u);
    EXPECT_GE(gpu_props.limits.maxDescriptorSetSampledImages, 256u);
    EXPECT_GE(gpu_props.limits.maxDescriptorSetStorageImages, 40u);
    EXPECT_GE(gpu_props.limits.maxDescriptorSetInputAttachments, 8u);
    EXPECT_GE(gpu_props.limits.maxVertexInputAttributes, 28u);
    EXPECT_GE(gpu_props.limits.maxVertexInputBindings, 28u);
    EXPECT_GE(gpu_props.limits.maxVertexInputAttributeOffset, 2047u);
    EXPECT_GE(gpu_props.limits.maxVertexInputBindingStride, 2048u);
    EXPECT_GE(gpu_props.limits.maxVertexOutputComponents, 124u);
    EXPECT_GE(gpu_props.limits.maxTessellationGenerationLevel, 64u);
    EXPECT_GE(gpu_props.limits.maxTessellationPatchSize, 32u);
    EXPECT_GE(gpu_props.limits.maxTessellationControlPerVertexInputComponents, 124u);
    EXPECT_GE(gpu_props.limits.maxTessellationControlPerVertexOutputComponents, 124u);
    EXPECT_GE(gpu_props.limits.maxTessellationControlPerPatchOutputComponents, 120u);
    EXPECT_GE(gpu_props.limits.maxTessellationControlTotalOutputComponents, 2048u);
    EXPECT_GE(gpu_props.limits.maxTessellationEvaluationInputComponents, 124u);
    EXPECT_GE(gpu_props.limits.maxTessellationEvaluationOutputComponents, 124u);
    EXPECT_GE(gpu_props.limits.maxFragmentInputComponents, 116u);
    EXPECT_GE(gpu_props.limits.maxFragmentOutputAttachments, 8u);
    EXPECT_GE(gpu_props.limits.maxFragmentDualSrcAttachments, 1u);
    EXPECT_GE(gpu_props.limits.maxFragmentCombinedOutputResources, 8u);
    EXPECT_GE(gpu_props.limits.maxComputeSharedMemorySize, 32768u);
    EXPECT_GE(gpu_props.limits.maxComputeWorkGroupCount[0], 65535u);
    EXPECT_GE(gpu_props.limits.maxComputeWorkGroupCount[1], 65535u);
    EXPECT_GE(gpu_props.limits.maxComputeWorkGroupCount[2], 65535u);
    EXPECT_GE(gpu_props.limits.maxComputeWorkGroupInvocations, 1024u);
    EXPECT_GE(gpu_props.limits.maxComputeWorkGroupSize[0], 1024u);
    EXPECT_GE(gpu_props.limits.maxComputeWorkGroupSize[1], 1024u);
    EXPECT_GE(gpu_props.limits.maxComputeWorkGroupSize[2], 64u);
    EXPECT_GE(gpu_props.limits.subPixelPrecisionBits, 4u);
    EXPECT_GE(gpu_props.limits.subTexelPrecisionBits, 4u);
    EXPECT_GE(gpu_props.limits.mipmapPrecisionBits, 4u);
    EXPECT_GE(gpu_props.limits.maxDrawIndexedIndexValue, 429467295u);
    EXPECT_GE(gpu_props.limits.maxDrawIndirectCount, 1073740000u);
    EXPECT_GE(gpu_props.limits.maxSamplerLodBias, 14.0f);
    EXPECT_GE(gpu_props.limits.maxSamplerAnisotropy, 16.0f);
    EXPECT_GE(gpu_props.limits.maxViewports, 16u);
    EXPECT_GE(gpu_props.limits.maxViewportDimensions[0], 16384u);
    EXPECT_GE(gpu_props.limits.maxViewportDimensions[1], 16384u);
    EXPECT_LE(gpu_props.limits.viewportBoundsRange[0], -32768.0f);
    EXPECT_GE(gpu_props.limits.viewportBoundsRange[1], 32767.0f);
    EXPECT_GE(gpu_props.limits.minMemoryMapAlignment, 64u);
    EXPECT_LE(gpu_props.limits.minTexelBufferOffsetAlignment, 64u);
    EXPECT_LE(gpu_props.limits.minUniformBufferOffsetAlignment, 256u);
    EXPECT_LE(gpu_props.limits.minStorageBufferOffsetAlignment, 64u);
    EXPECT_LE(gpu_props.limits.minTexelOffset, -8);
    EXPECT_GE(gpu_props.limits.maxTexelOffset, 7u);
    EXPECT_LE(gpu_props.limits.minTexelGatherOffset, -8);
    EXPECT_GE(gpu_props.limits.maxTexelGatherOffset, 7u);
    EXPECT_LE(gpu_props.limits.minInterpolationOffset, -0.5f);
    EXPECT_GE(gpu_props.limits.maxInterpolationOffset, 0.4375f);
    EXPECT_GE(gpu_props.limits.subPixelInterpolationOffsetBits, 4u);
    EXPECT_GE(gpu_props.limits.maxFramebufferWidth, 16384u);
    EXPECT_GE(gpu_props.limits.maxFramebufferHeight, 16384u);
    EXPECT_GE(gpu_props.limits.maxFramebufferLayers, 1024u);
    EXPECT_EQ(gpu_props.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(gpu_props.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
    EXPECT_EQ(gpu_props.limits.framebufferDepthSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(gpu_props.limits.framebufferDepthSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
    EXPECT_EQ(gpu_props.limits.framebufferStencilSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(gpu_props.limits.framebufferStencilSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
    EXPECT_GE(gpu_props.limits.maxColorAttachments, 8u);
    EXPECT_EQ(gpu_props.limits.sampledImageColorSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(gpu_props.limits.sampledImageColorSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
    EXPECT_EQ(gpu_props.limits.sampledImageIntegerSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(gpu_props.limits.sampledImageIntegerSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
    EXPECT_EQ(gpu_props.limits.sampledImageDepthSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(gpu_props.limits.sampledImageDepthSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
    EXPECT_EQ(gpu_props.limits.sampledImageStencilSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(gpu_props.limits.sampledImageStencilSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
    EXPECT_EQ(gpu_props.limits.storageImageSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_LE(gpu_props.limits.maxSampleMaskWords, 1u);
    EXPECT_GE(gpu_props.limits.maxClipDistances, 8u);
    EXPECT_GE(gpu_props.limits.maxCullDistances, 8u);
    EXPECT_GE(gpu_props.limits.maxCombinedClipAndCullDistances, 8u);
    EXPECT_GE(gpu_props.limits.discreteQueuePriorities, 2u);
    EXPECT_LE(gpu_props.limits.pointSizeRange[0], 1.0f);
    EXPECT_GE(gpu_props.limits.pointSizeRange[1], 64.0f);
    EXPECT_LE(gpu_props.limits.lineWidthRange[0], 1.0f);
    EXPECT_GE(gpu_props.limits.lineWidthRange[1], 1.0f);
    EXPECT_LE(gpu_props.limits.pointSizeGranularity, 0.125f);
    EXPECT_LE(gpu_props.limits.lineWidthGranularity, 0.5f);

    // Features
    EXPECT_EQ(gpu_feats.depthBiasClamp, VK_TRUE);
    EXPECT_EQ(gpu_feats.depthClamp, VK_TRUE);
    EXPECT_EQ(gpu_feats.drawIndirectFirstInstance, VK_TRUE);
    EXPECT_EQ(gpu_feats.dualSrcBlend, VK_TRUE);
    EXPECT_EQ(gpu_feats.fillModeNonSolid, VK_TRUE);
    EXPECT_EQ(gpu_feats.fragmentStoresAndAtomics, VK_TRUE);
    EXPECT_EQ(gpu_feats.fullDrawIndexUint32, VK_TRUE);
    EXPECT_EQ(gpu_feats.imageCubeArray, VK_TRUE);
    EXPECT_EQ(gpu_feats.independentBlend, VK_TRUE);
    EXPECT_EQ(gpu_feats.inheritedQueries, VK_TRUE);
    EXPECT_EQ(gpu_feats.largePoints, VK_TRUE);
    EXPECT_EQ(gpu_feats.multiDrawIndirect, VK_TRUE);
    EXPECT_EQ(gpu_feats.multiViewport, VK_TRUE);
    EXPECT_EQ(gpu_feats.occlusionQueryPrecise, VK_TRUE);
    EXPECT_EQ(gpu_feats.robustBufferAccess, VK_TRUE);
    EXPECT_EQ(gpu_feats.sampleRateShading, VK_TRUE);
    EXPECT_EQ(gpu_feats.samplerAnisotropy, VK_TRUE);
    EXPECT_EQ(gpu_feats.shaderClipDistance, VK_TRUE);
    EXPECT_EQ(gpu_feats.shaderImageGatherExtended, VK_TRUE);
    EXPECT_EQ(gpu_feats.shaderSampledImageArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(gpu_feats.shaderStorageBufferArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(gpu_feats.shaderStorageImageArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(gpu_feats.shaderStorageImageExtendedFormats, VK_TRUE);
    EXPECT_EQ(gpu_feats.shaderStorageImageWriteWithoutFormat, VK_TRUE);
    EXPECT_EQ(gpu_feats.shaderTessellationAndGeometryPointSize, VK_TRUE);
    EXPECT_EQ(gpu_feats.shaderUniformBufferArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(gpu_feats.tessellationShader, VK_TRUE);
    EXPECT_EQ(gpu_feats.textureCompressionBC, VK_TRUE);
    EXPECT_EQ(gpu_feats.vertexPipelineStoresAndAtomics, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestSelectingProfile) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    std::vector<std::string> filepaths = {TEST_SOURCE_PATH "/../../profiles/VP_LUNARG_desktop_portability_2021.json",
                                          TEST_SOURCE_PATH "/../../profiles/VP_KHR_roadmap_2022.json",
                                          TEST_SOURCE_PATH "/../../profiles/VP_ANDROID_baseline_2021.json"};
    profiles_test::setProfilesFilenames(filepaths);
    profiles_test::setProfilesEmulatePortabilitySubsetExtension(true);
    profiles_test::setProfilesModifyExtensionList(profiles_test::SetCombinationMode::SET_FROM_PROFILE_OVERRIDE);

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    {
        profiles_test::setProfilesProfileName("VP_LUNARG_desktop_portability_2021");

        err = inst_builder.makeInstance();
        ASSERT_EQ(err, VK_SUCCESS);

        VkInstance test_inst = inst_builder.getInstance();

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(&gpu);
        ASSERT_EQ(err, VK_SUCCESS);

        VkPhysicalDeviceProperties gpu_props{};
        vkGetPhysicalDeviceProperties(gpu, &gpu_props);

        VkPhysicalDeviceFeatures gpu_feats{};
        vkGetPhysicalDeviceFeatures(gpu, &gpu_feats);

        // Limits
        EXPECT_GE(gpu_props.limits.maxImageDimension1D, 16384u);
        EXPECT_GE(gpu_props.limits.maxImageDimension2D, 16384u);
        EXPECT_GE(gpu_props.limits.maxImageDimension3D, 2048u);
        EXPECT_GE(gpu_props.limits.maxImageDimensionCube, 16384u);
        EXPECT_GE(gpu_props.limits.maxImageArrayLayers, 2048u);
        EXPECT_GE(gpu_props.limits.maxTexelBufferElements, 67108900u);
        EXPECT_GE(gpu_props.limits.maxUniformBufferRange, 65536u);
        EXPECT_GE(gpu_props.limits.maxStorageBufferRange, 134217728u);
        EXPECT_GE(gpu_props.limits.maxPushConstantsSize, 128u);
        EXPECT_GE(gpu_props.limits.maxMemoryAllocationCount, 4096u);
        EXPECT_GE(gpu_props.limits.maxSamplerAllocationCount, 1024u);
        EXPECT_LE(gpu_props.limits.bufferImageGranularity, 1024);
        EXPECT_GE(gpu_props.limits.sparseAddressSpaceSize, 1073741824);
        EXPECT_GE(gpu_props.limits.maxBoundDescriptorSets, 8u);
        EXPECT_GE(gpu_props.limits.maxPerStageDescriptorSamplers, 16u);
        EXPECT_GE(gpu_props.limits.maxPerStageDescriptorUniformBuffers, 15u);
        EXPECT_GE(gpu_props.limits.maxPerStageDescriptorStorageBuffers, 16u);
        EXPECT_GE(gpu_props.limits.maxPerStageDescriptorSampledImages, 128u);
        EXPECT_GE(gpu_props.limits.maxPerStageDescriptorStorageImages, 8u);
        EXPECT_GE(gpu_props.limits.maxPerStageDescriptorInputAttachments, 8u);
        EXPECT_GE(gpu_props.limits.maxPerStageResources, 128u);
        EXPECT_GE(gpu_props.limits.maxDescriptorSetSamplers, 80u);
        EXPECT_GE(gpu_props.limits.maxDescriptorSetUniformBuffers, 90u);
        EXPECT_GE(gpu_props.limits.maxDescriptorSetUniformBuffersDynamic, 8u);
        EXPECT_GE(gpu_props.limits.maxDescriptorSetStorageBuffers, 155u);
        EXPECT_GE(gpu_props.limits.maxDescriptorSetStorageBuffersDynamic, 8u);
        EXPECT_GE(gpu_props.limits.maxDescriptorSetSampledImages, 256u);
        EXPECT_GE(gpu_props.limits.maxDescriptorSetStorageImages, 40u);
        EXPECT_GE(gpu_props.limits.maxDescriptorSetInputAttachments, 8u);
        EXPECT_GE(gpu_props.limits.maxVertexInputAttributes, 28u);
        EXPECT_GE(gpu_props.limits.maxVertexInputBindings, 28u);
        EXPECT_GE(gpu_props.limits.maxVertexInputAttributeOffset, 2047u);
        EXPECT_GE(gpu_props.limits.maxVertexInputBindingStride, 2048u);
        EXPECT_GE(gpu_props.limits.maxVertexOutputComponents, 124u);
        EXPECT_GE(gpu_props.limits.maxTessellationGenerationLevel, 64u);
        EXPECT_GE(gpu_props.limits.maxTessellationPatchSize, 32u);
        EXPECT_GE(gpu_props.limits.maxTessellationControlPerVertexInputComponents, 124u);
        EXPECT_GE(gpu_props.limits.maxTessellationControlPerVertexOutputComponents, 124u);
        EXPECT_GE(gpu_props.limits.maxTessellationControlPerPatchOutputComponents, 120u);
        EXPECT_GE(gpu_props.limits.maxTessellationControlTotalOutputComponents, 2048u);
        EXPECT_GE(gpu_props.limits.maxTessellationEvaluationInputComponents, 124u);
        EXPECT_GE(gpu_props.limits.maxTessellationEvaluationOutputComponents, 124u);
        EXPECT_GE(gpu_props.limits.maxFragmentInputComponents, 116u);
        EXPECT_GE(gpu_props.limits.maxFragmentOutputAttachments, 8u);
        EXPECT_GE(gpu_props.limits.maxFragmentDualSrcAttachments, 1u);
        EXPECT_GE(gpu_props.limits.maxFragmentCombinedOutputResources, 8u);
        EXPECT_GE(gpu_props.limits.maxComputeSharedMemorySize, 32768u);
        EXPECT_GE(gpu_props.limits.maxComputeWorkGroupCount[0], 65535u);
        EXPECT_GE(gpu_props.limits.maxComputeWorkGroupCount[1], 65535u);
        EXPECT_GE(gpu_props.limits.maxComputeWorkGroupCount[2], 65535u);
        EXPECT_GE(gpu_props.limits.maxComputeWorkGroupInvocations, 1024u);
        EXPECT_GE(gpu_props.limits.maxComputeWorkGroupSize[0], 1024u);
        EXPECT_GE(gpu_props.limits.maxComputeWorkGroupSize[1], 1024u);
        EXPECT_GE(gpu_props.limits.maxComputeWorkGroupSize[2], 64u);
        EXPECT_GE(gpu_props.limits.subPixelPrecisionBits, 4u);
        EXPECT_GE(gpu_props.limits.subTexelPrecisionBits, 4u);
        EXPECT_GE(gpu_props.limits.mipmapPrecisionBits, 4u);
        EXPECT_GE(gpu_props.limits.maxDrawIndexedIndexValue, 429467295u);
        EXPECT_GE(gpu_props.limits.maxDrawIndirectCount, 1073740000u);
        EXPECT_GE(gpu_props.limits.maxSamplerLodBias, 14.0f);
        EXPECT_GE(gpu_props.limits.maxSamplerAnisotropy, 16.0f);
        EXPECT_GE(gpu_props.limits.maxViewports, 16u);
        EXPECT_GE(gpu_props.limits.maxViewportDimensions[0], 16384u);
        EXPECT_GE(gpu_props.limits.maxViewportDimensions[1], 16384u);
        EXPECT_LE(gpu_props.limits.viewportBoundsRange[0], -32768.0f);
        EXPECT_GE(gpu_props.limits.viewportBoundsRange[1], 32767.0f);
        EXPECT_GE(gpu_props.limits.minMemoryMapAlignment, 64u);
        EXPECT_LE(gpu_props.limits.minTexelBufferOffsetAlignment, 64u);
        EXPECT_LE(gpu_props.limits.minUniformBufferOffsetAlignment, 256u);
        EXPECT_LE(gpu_props.limits.minStorageBufferOffsetAlignment, 64u);
        EXPECT_LE(gpu_props.limits.minTexelOffset, -8);
        EXPECT_GE(gpu_props.limits.maxTexelOffset, 7u);
        EXPECT_LE(gpu_props.limits.minTexelGatherOffset, -8);
        EXPECT_GE(gpu_props.limits.maxTexelGatherOffset, 7u);
        EXPECT_LE(gpu_props.limits.minInterpolationOffset, -0.5f);
        EXPECT_GE(gpu_props.limits.maxInterpolationOffset, 0.4375f);
        EXPECT_GE(gpu_props.limits.subPixelInterpolationOffsetBits, 4u);
        EXPECT_GE(gpu_props.limits.maxFramebufferWidth, 16384u);
        EXPECT_GE(gpu_props.limits.maxFramebufferHeight, 16384u);
        EXPECT_GE(gpu_props.limits.maxFramebufferLayers, 1024u);
        EXPECT_EQ(gpu_props.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
        EXPECT_EQ(gpu_props.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
        EXPECT_EQ(gpu_props.limits.framebufferDepthSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
        EXPECT_EQ(gpu_props.limits.framebufferDepthSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
        EXPECT_EQ(gpu_props.limits.framebufferStencilSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
        EXPECT_EQ(gpu_props.limits.framebufferStencilSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
        EXPECT_GE(gpu_props.limits.maxColorAttachments, 8u);
        EXPECT_EQ(gpu_props.limits.sampledImageColorSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
        EXPECT_EQ(gpu_props.limits.sampledImageColorSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
        EXPECT_EQ(gpu_props.limits.sampledImageIntegerSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
        EXPECT_EQ(gpu_props.limits.sampledImageIntegerSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
        EXPECT_EQ(gpu_props.limits.sampledImageDepthSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
        EXPECT_EQ(gpu_props.limits.sampledImageDepthSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
        EXPECT_EQ(gpu_props.limits.sampledImageStencilSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
        EXPECT_EQ(gpu_props.limits.sampledImageStencilSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
        EXPECT_EQ(gpu_props.limits.storageImageSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
        EXPECT_LE(gpu_props.limits.maxSampleMaskWords, 1u);
        EXPECT_GE(gpu_props.limits.maxClipDistances, 8u);
        EXPECT_GE(gpu_props.limits.maxCullDistances, 8u);
        EXPECT_GE(gpu_props.limits.maxCombinedClipAndCullDistances, 8u);
        EXPECT_GE(gpu_props.limits.discreteQueuePriorities, 2u);
        EXPECT_LE(gpu_props.limits.pointSizeRange[0], 1.0f);
        EXPECT_GE(gpu_props.limits.pointSizeRange[1], 64.0f);
        EXPECT_LE(gpu_props.limits.lineWidthRange[0], 1.0f);
        EXPECT_GE(gpu_props.limits.lineWidthRange[1], 1.0f);
        EXPECT_LE(gpu_props.limits.pointSizeGranularity, 0.125f);
        EXPECT_LE(gpu_props.limits.lineWidthGranularity, 0.5f);

        // Features
        EXPECT_EQ(gpu_feats.depthBiasClamp, VK_TRUE);
        EXPECT_EQ(gpu_feats.depthClamp, VK_TRUE);
        EXPECT_EQ(gpu_feats.drawIndirectFirstInstance, VK_TRUE);
        EXPECT_EQ(gpu_feats.dualSrcBlend, VK_TRUE);
        EXPECT_EQ(gpu_feats.fillModeNonSolid, VK_TRUE);
        EXPECT_EQ(gpu_feats.fragmentStoresAndAtomics, VK_TRUE);
        EXPECT_EQ(gpu_feats.fullDrawIndexUint32, VK_TRUE);
        EXPECT_EQ(gpu_feats.imageCubeArray, VK_TRUE);
        EXPECT_EQ(gpu_feats.independentBlend, VK_TRUE);
        EXPECT_EQ(gpu_feats.inheritedQueries, VK_TRUE);
        EXPECT_EQ(gpu_feats.largePoints, VK_TRUE);
        EXPECT_EQ(gpu_feats.multiDrawIndirect, VK_TRUE);
        EXPECT_EQ(gpu_feats.multiViewport, VK_TRUE);
        EXPECT_EQ(gpu_feats.occlusionQueryPrecise, VK_TRUE);
        EXPECT_EQ(gpu_feats.robustBufferAccess, VK_TRUE);
        EXPECT_EQ(gpu_feats.sampleRateShading, VK_TRUE);
        EXPECT_EQ(gpu_feats.samplerAnisotropy, VK_TRUE);
        EXPECT_EQ(gpu_feats.shaderClipDistance, VK_TRUE);
        EXPECT_EQ(gpu_feats.shaderImageGatherExtended, VK_TRUE);
        EXPECT_EQ(gpu_feats.shaderSampledImageArrayDynamicIndexing, VK_TRUE);
        EXPECT_EQ(gpu_feats.shaderStorageBufferArrayDynamicIndexing, VK_TRUE);
        EXPECT_EQ(gpu_feats.shaderStorageImageArrayDynamicIndexing, VK_TRUE);
        EXPECT_EQ(gpu_feats.shaderStorageImageExtendedFormats, VK_TRUE);
        EXPECT_EQ(gpu_feats.shaderStorageImageWriteWithoutFormat, VK_TRUE);
        EXPECT_EQ(gpu_feats.shaderTessellationAndGeometryPointSize, VK_TRUE);
        EXPECT_EQ(gpu_feats.shaderUniformBufferArrayDynamicIndexing, VK_TRUE);
        EXPECT_EQ(gpu_feats.tessellationShader, VK_TRUE);
        EXPECT_EQ(gpu_feats.textureCompressionBC, VK_TRUE);
        EXPECT_EQ(gpu_feats.vertexPipelineStoresAndAtomics, VK_TRUE);

        vkDestroyInstance(test_inst, nullptr);
    }

    {
        profiles_test::setProfilesProfileName("VP_KHR_roadmap_2022");

        err = inst_builder.makeInstance();
        ASSERT_EQ(err, VK_SUCCESS);

        VkInstance test_inst = inst_builder.getInstance();

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(&gpu);
        ASSERT_EQ(err, VK_SUCCESS);

        VkPhysicalDeviceProperties gpu_props{};
        vkGetPhysicalDeviceProperties(gpu, &gpu_props);

        VkPhysicalDeviceFeatures gpu_feats{};
        vkGetPhysicalDeviceFeatures(gpu, &gpu_feats);

        // Limits
        EXPECT_GE(gpu_props.limits.maxImageDimension1D, 8192u);
        EXPECT_GE(gpu_props.limits.maxImageDimension2D, 8192u);
        EXPECT_GE(gpu_props.limits.maxImageDimensionCube, 8192u);
        EXPECT_GE(gpu_props.limits.maxImageArrayLayers, 2048u);
        EXPECT_GE(gpu_props.limits.maxUniformBufferRange, 65536u);
        EXPECT_GE(gpu_props.limits.bufferImageGranularity, 4096u);
        EXPECT_GE(gpu_props.limits.maxPerStageDescriptorSamplers, 64u);
        EXPECT_GE(gpu_props.limits.maxPerStageDescriptorUniformBuffers, 15u);
        EXPECT_GE(gpu_props.limits.maxPerStageDescriptorStorageBuffers, 30u);
        EXPECT_GE(gpu_props.limits.maxPerStageDescriptorSampledImages, 200u);
        EXPECT_GE(gpu_props.limits.maxPerStageDescriptorStorageImages, 16u);
        EXPECT_GE(gpu_props.limits.maxPerStageResources, 200u);
        EXPECT_GE(gpu_props.limits.maxDescriptorSetSamplers, 576u);
        EXPECT_GE(gpu_props.limits.maxDescriptorSetUniformBuffers, 90u);
        EXPECT_GE(gpu_props.limits.maxDescriptorSetStorageBuffers, 96u);
        EXPECT_GE(gpu_props.limits.maxDescriptorSetSampledImages, 1800u);
        EXPECT_GE(gpu_props.limits.maxDescriptorSetStorageImages, 144u);
        EXPECT_GE(gpu_props.limits.maxFragmentCombinedOutputResources, 16u);
        EXPECT_GE(gpu_props.limits.maxComputeWorkGroupInvocations, 256u);
        EXPECT_GE(gpu_props.limits.maxComputeWorkGroupSize[0], 1024u);
        EXPECT_GE(gpu_props.limits.maxComputeWorkGroupSize[1], 1024u);
        EXPECT_GE(gpu_props.limits.maxComputeWorkGroupSize[2], 64u);
        EXPECT_GE(gpu_props.limits.subTexelPrecisionBits, 8u);
        EXPECT_GE(gpu_props.limits.mipmapPrecisionBits, 6u);
        EXPECT_GE(gpu_props.limits.maxSamplerLodBias, 14u);
        EXPECT_GE(gpu_props.limits.pointSizeGranularity, 0.125f);
        EXPECT_GE(gpu_props.limits.lineWidthGranularity, 0.5f);
        EXPECT_EQ(gpu_props.limits.standardSampleLocations, VK_TRUE);
        EXPECT_GE(gpu_props.limits.maxColorAttachments, 7u);

        // Features
        EXPECT_EQ(gpu_feats.robustBufferAccess, VK_TRUE);

        EXPECT_EQ(gpu_feats.fullDrawIndexUint32, VK_TRUE);
        EXPECT_EQ(gpu_feats.imageCubeArray, VK_TRUE);
        EXPECT_EQ(gpu_feats.independentBlend, VK_TRUE);
        EXPECT_EQ(gpu_feats.sampleRateShading, VK_TRUE);
        EXPECT_EQ(gpu_feats.drawIndirectFirstInstance, VK_TRUE);
        EXPECT_EQ(gpu_feats.depthClamp, VK_TRUE);
        EXPECT_EQ(gpu_feats.depthBiasClamp, VK_TRUE);
        EXPECT_EQ(gpu_feats.samplerAnisotropy, VK_TRUE);
        EXPECT_EQ(gpu_feats.occlusionQueryPrecise, VK_TRUE);
        EXPECT_EQ(gpu_feats.fragmentStoresAndAtomics, VK_TRUE);
        EXPECT_EQ(gpu_feats.shaderStorageImageExtendedFormats, VK_TRUE);
        EXPECT_EQ(gpu_feats.shaderUniformBufferArrayDynamicIndexing, VK_TRUE);
        EXPECT_EQ(gpu_feats.shaderSampledImageArrayDynamicIndexing, VK_TRUE);
        EXPECT_EQ(gpu_feats.shaderStorageBufferArrayDynamicIndexing, VK_TRUE);
        EXPECT_EQ(gpu_feats.shaderStorageImageArrayDynamicIndexing, VK_TRUE);

        vkDestroyInstance(test_inst, nullptr);
    }

    {
        profiles_test::setProfilesProfileName("VP_ANDROID_baseline_2021");

        err = inst_builder.makeInstance();
        ASSERT_EQ(err, VK_SUCCESS);

        VkInstance test_inst = inst_builder.getInstance();

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(&gpu);
        ASSERT_EQ(err, VK_SUCCESS);

        VkPhysicalDeviceProperties gpu_props{};
        vkGetPhysicalDeviceProperties(gpu, &gpu_props);

        VkPhysicalDeviceFeatures gpu_feats{};
        vkGetPhysicalDeviceFeatures(gpu, &gpu_feats);

        // Limits
        EXPECT_GE(gpu_props.limits.maxImageDimension1D, 8192u);
        EXPECT_GE(gpu_props.limits.maxImageDimension2D, 8192u);
        EXPECT_GE(gpu_props.limits.maxImageDimension3D, 2048u);
        EXPECT_GE(gpu_props.limits.maxImageDimensionCube, 8192u);
        EXPECT_GE(gpu_props.limits.maxImageArrayLayers, 256u);
        EXPECT_GE(gpu_props.limits.maxTexelBufferElements, 65536u);
        EXPECT_GE(gpu_props.limits.maxUniformBufferRange, 16384u);
        EXPECT_GE(gpu_props.limits.maxStorageBufferRange, 134217728u);
        EXPECT_GE(gpu_props.limits.maxPushConstantsSize, 128u);
        EXPECT_GE(gpu_props.limits.maxMemoryAllocationCount, 4096u);
        EXPECT_GE(gpu_props.limits.maxSamplerAllocationCount, 4000u);
        EXPECT_GE(gpu_props.limits.maxBoundDescriptorSets, 4u);
        EXPECT_GE(gpu_props.limits.maxPerStageDescriptorSamplers, 16u);
        EXPECT_GE(gpu_props.limits.maxPerStageDescriptorUniformBuffers, 12u);
        EXPECT_GE(gpu_props.limits.maxPerStageDescriptorStorageBuffers, 4u);
        EXPECT_GE(gpu_props.limits.maxPerStageDescriptorSampledImages, 16u);
        EXPECT_GE(gpu_props.limits.maxPerStageDescriptorStorageImages, 4u);
        EXPECT_GE(gpu_props.limits.maxPerStageDescriptorInputAttachments, 4u);
        EXPECT_GE(gpu_props.limits.maxPerStageResources, 44u);
        EXPECT_GE(gpu_props.limits.maxDescriptorSetSamplers, 96u);
        EXPECT_GE(gpu_props.limits.maxDescriptorSetUniformBuffers, 72u);
        EXPECT_GE(gpu_props.limits.maxDescriptorSetUniformBuffersDynamic, 8u);
        EXPECT_GE(gpu_props.limits.maxDescriptorSetStorageBuffers, 24u);
        EXPECT_GE(gpu_props.limits.maxDescriptorSetStorageBuffersDynamic, 4u);
        EXPECT_GE(gpu_props.limits.maxDescriptorSetSampledImages, 96u);
        EXPECT_GE(gpu_props.limits.maxDescriptorSetStorageImages, 24u);
        EXPECT_GE(gpu_props.limits.maxDescriptorSetInputAttachments, 4u);
        EXPECT_GE(gpu_props.limits.maxVertexInputAttributes, 16u);
        EXPECT_GE(gpu_props.limits.maxVertexInputBindings, 16u);
        EXPECT_GE(gpu_props.limits.maxVertexInputAttributeOffset, 2047u);
        EXPECT_GE(gpu_props.limits.maxVertexInputBindingStride, 2048u);
        EXPECT_GE(gpu_props.limits.maxVertexOutputComponents, 64u);
        EXPECT_GE(gpu_props.limits.maxFragmentInputComponents, 64u);
        EXPECT_GE(gpu_props.limits.maxFragmentOutputAttachments, 4u);
        EXPECT_GE(gpu_props.limits.maxFragmentCombinedOutputResources, 8u);
        EXPECT_GE(gpu_props.limits.maxComputeSharedMemorySize, 16384u);
        EXPECT_GE(gpu_props.limits.maxComputeWorkGroupCount[0], 65535u);
        EXPECT_GE(gpu_props.limits.maxComputeWorkGroupCount[1], 65535u);
        EXPECT_GE(gpu_props.limits.maxComputeWorkGroupCount[2], 65535u);
        EXPECT_GE(gpu_props.limits.maxComputeWorkGroupInvocations, 128u);
        EXPECT_GE(gpu_props.limits.maxComputeWorkGroupSize[0], 128u);
        EXPECT_GE(gpu_props.limits.maxComputeWorkGroupSize[1], 128u);
        EXPECT_GE(gpu_props.limits.maxComputeWorkGroupSize[2], 64u);
        EXPECT_GE(gpu_props.limits.subPixelPrecisionBits, 4u);
        EXPECT_GE(gpu_props.limits.subTexelPrecisionBits, 4u);
        EXPECT_GE(gpu_props.limits.mipmapPrecisionBits, 4u);
        EXPECT_GE(gpu_props.limits.maxDrawIndexedIndexValue, 4294967295u);
        EXPECT_GE(gpu_props.limits.maxDrawIndirectCount, 1u);
        EXPECT_GE(gpu_props.limits.maxSamplerLodBias, 2.0f);
        EXPECT_GE(gpu_props.limits.maxSamplerAnisotropy, 1.0f);
        EXPECT_GE(gpu_props.limits.maxViewports, 1u);
        EXPECT_GE(gpu_props.limits.maxViewportDimensions[0], 8192u);
        EXPECT_GE(gpu_props.limits.maxViewportDimensions[1], 8192u);
        EXPECT_GE(gpu_props.limits.viewportBoundsRange[0], -8192);
        EXPECT_GE(gpu_props.limits.viewportBoundsRange[1], 8192u);
        EXPECT_GE(gpu_props.limits.minMemoryMapAlignment, 4096u);
        EXPECT_GE(gpu_props.limits.minTexelBufferOffsetAlignment, 256u);
        EXPECT_GE(gpu_props.limits.minUniformBufferOffsetAlignment, 256u);
        EXPECT_GE(gpu_props.limits.minStorageBufferOffsetAlignment, 256u);
        EXPECT_GE(gpu_props.limits.minTexelOffset, -8);
        EXPECT_GE(gpu_props.limits.maxTexelOffset, 7u);
        EXPECT_GE(gpu_props.limits.minTexelGatherOffset, -8);
        EXPECT_GE(gpu_props.limits.maxTexelGatherOffset, 7u);
        EXPECT_GE(gpu_props.limits.minInterpolationOffset, -0.5f);
        EXPECT_GE(gpu_props.limits.maxInterpolationOffset, 0.4375f);
        EXPECT_GE(gpu_props.limits.subPixelInterpolationOffsetBits, 4u);
        EXPECT_GE(gpu_props.limits.maxFramebufferWidth, 8192u);
        EXPECT_GE(gpu_props.limits.maxFramebufferHeight, 8192u);
        EXPECT_GE(gpu_props.limits.maxFramebufferLayers, 256u);
        EXPECT_GE(gpu_props.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
        EXPECT_GE(gpu_props.limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
        EXPECT_GE(gpu_props.limits.framebufferDepthSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
        EXPECT_GE(gpu_props.limits.framebufferDepthSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
        EXPECT_GE(gpu_props.limits.framebufferStencilSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
        EXPECT_GE(gpu_props.limits.framebufferStencilSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
        EXPECT_GE(gpu_props.limits.framebufferNoAttachmentsSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
        EXPECT_GE(gpu_props.limits.framebufferNoAttachmentsSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
        EXPECT_GE(gpu_props.limits.maxColorAttachments, 4u);
        EXPECT_GE(gpu_props.limits.sampledImageColorSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
        EXPECT_GE(gpu_props.limits.sampledImageColorSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
        EXPECT_GE(gpu_props.limits.sampledImageIntegerSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
        EXPECT_GE(gpu_props.limits.sampledImageDepthSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
        EXPECT_GE(gpu_props.limits.sampledImageDepthSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
        EXPECT_GE(gpu_props.limits.sampledImageStencilSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
        EXPECT_GE(gpu_props.limits.sampledImageStencilSampleCounts & VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_4_BIT);
        EXPECT_GE(gpu_props.limits.storageImageSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
        EXPECT_GE(gpu_props.limits.maxSampleMaskWords, 1u);
        EXPECT_GE(gpu_props.limits.discreteQueuePriorities, 2u);
        EXPECT_GE(gpu_props.limits.pointSizeRange[0], 1.0f);
        EXPECT_GE(gpu_props.limits.pointSizeRange[1], 1.0f);
        EXPECT_GE(gpu_props.limits.lineWidthRange[0], 1.0f);
        EXPECT_GE(gpu_props.limits.lineWidthRange[1], 1.0f);
        EXPECT_GE(gpu_props.limits.pointSizeGranularity, 1u);
        EXPECT_GE(gpu_props.limits.lineWidthGranularity, 1u);
        EXPECT_GE(gpu_props.limits.standardSampleLocations, VK_TRUE);

        // Features
        EXPECT_EQ(gpu_feats.robustBufferAccess, VK_TRUE);

        EXPECT_EQ(gpu_feats.depthBiasClamp, VK_TRUE);
        EXPECT_EQ(gpu_feats.fragmentStoresAndAtomics, VK_TRUE);
        EXPECT_EQ(gpu_feats.fullDrawIndexUint32, VK_TRUE);
        EXPECT_EQ(gpu_feats.imageCubeArray, VK_TRUE);
        EXPECT_EQ(gpu_feats.independentBlend, VK_TRUE);
        EXPECT_EQ(gpu_feats.largePoints, VK_TRUE);
        EXPECT_EQ(gpu_feats.robustBufferAccess, VK_TRUE);
        EXPECT_EQ(gpu_feats.sampleRateShading, VK_TRUE);
        EXPECT_EQ(gpu_feats.shaderImageGatherExtended, VK_TRUE);
        EXPECT_EQ(gpu_feats.shaderSampledImageArrayDynamicIndexing, VK_TRUE);
        EXPECT_EQ(gpu_feats.shaderStorageImageArrayDynamicIndexing, VK_TRUE);
        EXPECT_EQ(gpu_feats.shaderUniformBufferArrayDynamicIndexing, VK_TRUE);
        EXPECT_EQ(gpu_feats.textureCompressionASTC_LDR, VK_TRUE);
        EXPECT_EQ(gpu_feats.textureCompressionETC2, VK_TRUE);

        vkDestroyInstance(test_inst, nullptr);
    }
}

TEST(profiles, TestSetCombinationMode) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    std::vector<VkExtensionProperties> device_extensions;
    {
        err = inst_builder.makeInstance();
        ASSERT_EQ(err, VK_SUCCESS);

        VkInstance test_inst = inst_builder.getInstance();

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(&gpu);

        if (err != VK_SUCCESS) {
            printf("Profile not supported on device, skipping test.\n");
        } else {
            uint32_t count;
            vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, nullptr);
            device_extensions.resize(count);
            vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, device_extensions.data());
        }

        vkDestroyInstance(test_inst, nullptr);
        inst_builder.reset();
    }

    {
        inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");
        std::vector<std::string> filepaths = {TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json"};
        profiles_test::setProfilesFilenames(filepaths);
        profiles_test::setProfilesEmulatePortabilitySubsetExtension(true);
        profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
        profiles_test::setProfilesModifyExtensionList(profiles_test::SetCombinationMode::SET_CHECK_SUPPORT);

        err = inst_builder.makeInstance();
        ASSERT_EQ(err, VK_SUCCESS);

        VkInstance test_inst = inst_builder.getInstance();

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(&gpu);

        if (err != VK_SUCCESS) {
            printf("Profile not supported on device, skipping test.\n");
        } else {
            VkPhysicalDeviceProperties gpu_props{};
            vkGetPhysicalDeviceProperties(gpu, &gpu_props);

            VkPhysicalDeviceFeatures gpu_feats{};
            vkGetPhysicalDeviceFeatures(gpu, &gpu_feats);

            uint32_t count;
            vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, nullptr);
            std::vector<VkExtensionProperties> extensions(count);
            vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, extensions.data());

            size_t portability_subset_add = 1;
            for (const auto& ext : device_extensions) {
                if (strcmp(ext.extensionName, "VK_KHR_portability_subset") == 0) {
                    portability_subset_add = 0;
                    break;
                }
            }
            ASSERT_EQ(device_extensions.size() + portability_subset_add, extensions.size());
        }

        vkDestroyInstance(test_inst, nullptr);
        inst_builder.reset();
    }

    {
        inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");
        std::vector<std::string> filepaths = {TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json"};
        profiles_test::setProfilesFilenames(filepaths);
        profiles_test::setProfilesEmulatePortabilitySubsetExtension(true);
        profiles_test::setProfilesProfileName("VP_LUNARG_test_api_1_2_198");
        profiles_test::setProfilesModifyExtensionList(profiles_test::SetCombinationMode::SET_CHECK_SUPPORT);

        err = inst_builder.makeInstance();
        ASSERT_EQ(err, VK_SUCCESS);

        VkInstance test_inst = inst_builder.getInstance();

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(&gpu);

        if (err != VK_SUCCESS) {
            printf("Profile not supported on device, skipping test.\n");
        } else {
            VkPhysicalDeviceProperties gpu_props{};
            vkGetPhysicalDeviceProperties(gpu, &gpu_props);

            VkPhysicalDeviceFeatures gpu_feats{};
            vkGetPhysicalDeviceFeatures(gpu, &gpu_feats);

            uint32_t count;
            vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, nullptr);
            std::vector<VkExtensionProperties> extensions(count);
            vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, extensions.data());

            size_t portability_subset_add = 1;
            for (const auto& ext : device_extensions) {
                if (strcmp(ext.extensionName, "VK_KHR_portability_subset") == 0) {
                    portability_subset_add = 0;
                    break;
                }
            }
            ASSERT_EQ(device_extensions.size() + portability_subset_add, extensions.size());
        }

        vkDestroyInstance(test_inst, nullptr);
        inst_builder.reset();
    }

    {
        inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");
        std::vector<std::string> filepaths = {TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json"};
        profiles_test::setProfilesFilenames(filepaths);
        profiles_test::setProfilesEmulatePortabilitySubsetExtension(true);
        profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
        profiles_test::setProfilesModifyExtensionList(profiles_test::SetCombinationMode::SET_FROM_PROFILE);

        err = inst_builder.makeInstance();
        ASSERT_EQ(err, VK_SUCCESS);

        VkInstance test_inst = inst_builder.getInstance();

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(&gpu);

        if (err != VK_SUCCESS) {
            printf("Profile not supported on device, skipping test.\n");
        } else {
            VkPhysicalDeviceProperties gpu_props{};
            vkGetPhysicalDeviceProperties(gpu, &gpu_props);

            VkPhysicalDeviceFeatures gpu_feats{};
            vkGetPhysicalDeviceFeatures(gpu, &gpu_feats);

            uint32_t count;
            vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, nullptr);
            std::vector<VkExtensionProperties> extensions(count);
            vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, extensions.data());

            ASSERT_EQ(extensions.size(), 231);
        }

        vkDestroyInstance(test_inst, nullptr);
        inst_builder.reset();
    }
}

TEST(profiles, TestExtensionNotSupported) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    std::vector<VkExtensionProperties> device_extensions;
    {
        err = inst_builder.makeInstance();
        ASSERT_EQ(err, VK_SUCCESS);

        VkInstance test_inst = inst_builder.getInstance();

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(&gpu);
        ASSERT_EQ(err, VK_SUCCESS);

        uint32_t count;
        vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, nullptr);
        device_extensions.resize(count);
        vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, device_extensions.data());

        vkDestroyInstance(test_inst, nullptr);
        inst_builder.reset();
    }

    {
        inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");
        std::vector<std::string> filepaths = {TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json"};
        profiles_test::setProfilesFilenames(filepaths);
        profiles_test::setProfilesEmulatePortabilitySubsetExtension(true);
        profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
        profiles_test::setProfilesModifyExtensionList(profiles_test::SetCombinationMode::SET_FROM_PROFILE);

        err = inst_builder.makeInstance();
        ASSERT_EQ(err, VK_SUCCESS);

        VkInstance test_inst = inst_builder.getInstance();

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(&gpu);

        if (device_extensions.size() < 231) {
            ASSERT_EQ(err, VK_ERROR_INITIALIZATION_FAILED);
        }

        vkDestroyInstance(test_inst, nullptr);
        inst_builder.reset();
    }
}

TEST(profiles, TestSelectingProfileAndCapabilities) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    {
        std::vector<std::string> filepaths = {TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_selecting_profile.json"};
        profiles_test::setProfilesFilenames(filepaths);
        profiles_test::setProfilesProfileName("VP_LUNARG_test_selecting_profile");
        profiles_test::setProfilesEmulatePortabilitySubsetExtension(true);
        profiles_test::setProfilesModifyExtensionList(profiles_test::SetCombinationMode::SET_FROM_PROFILE_OVERRIDE);

        inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

        err = inst_builder.makeInstance();
        ASSERT_EQ(err, VK_SUCCESS);

        VkInstance test_inst = inst_builder.getInstance();

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(&gpu);
        if (err != VK_SUCCESS) {
            printf("Profile not supported on device, skipping test.\n");
            vkDestroyInstance(test_inst, nullptr);
            return;
        }

        VkPhysicalDeviceProperties gpu_props{};
        vkGetPhysicalDeviceProperties(gpu, &gpu_props);

        EXPECT_EQ(gpu_props.limits.maxImageDimension1D, 16384u);

        inst_builder.reset();
    }

    {
        std::vector<std::string> filepaths = {TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_selecting_profile.json"};
        profiles_test::setProfilesFilenames(filepaths);
        profiles_test::setProfilesProfileName("VP_LUNARG_test_selecting_profile_subset");
        profiles_test::setProfilesEmulatePortabilitySubsetExtension(true);
        profiles_test::setProfilesModifyExtensionList(profiles_test::SetCombinationMode::SET_FROM_PROFILE_OVERRIDE);

        inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

        err = inst_builder.makeInstance();
        ASSERT_EQ(err, VK_SUCCESS);

        VkInstance test_inst = inst_builder.getInstance();

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(&gpu);
        if (err != VK_SUCCESS) {
            printf("Profile not supported on device, skipping test.\n");
            vkDestroyInstance(test_inst, nullptr);
            return;
        }

        VkPhysicalDeviceProperties gpu_props{};
        vkGetPhysicalDeviceProperties(gpu, &gpu_props);

        EXPECT_EQ(gpu_props.limits.maxImageDimension1D, 4096u);

        inst_builder.reset();
    }
}
