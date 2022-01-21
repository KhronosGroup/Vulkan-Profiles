

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

TEST(layer, TestDesktopPortability2022Limits) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/VP_LUNARG_desktop_portability_2021.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_desktop_portability_2021");
    profiles_test::setProfilesEmulatePortabilitySubsetExtension(true);
    profiles_test::setProfilesFailOnError(false);

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
    EXPECT_GE(gpu_props.limits.maxTexelBufferElements, 67108864u);
    EXPECT_GE(gpu_props.limits.maxUniformBufferRange, 65536u);
    EXPECT_GE(gpu_props.limits.maxStorageBufferRange, 134217728u);
    EXPECT_GE(gpu_props.limits.maxPushConstantsSize, 128u);
    EXPECT_GE(gpu_props.limits.maxMemoryAllocationCount, 4096u);
    EXPECT_GE(gpu_props.limits.maxSamplerAllocationCount, 1024u);
    EXPECT_LE(gpu_props.limits.bufferImageGranularity, 1024);
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
    EXPECT_GE(gpu_props.limits.maxSamplerLodBias, 4.0f);
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
    EXPECT_GE(gpu_props.limits.maxCombinedClipAndCullDistances, 8u);
    EXPECT_GE(gpu_props.limits.discreteQueuePriorities, 2u);
    EXPECT_LE(gpu_props.limits.pointSizeRange[0], 1.0f);
    EXPECT_GE(gpu_props.limits.pointSizeRange[1], 64.0f);
    EXPECT_LE(gpu_props.limits.lineWidthRange[0], 1.0f);
    EXPECT_GE(gpu_props.limits.lineWidthRange[1], 1.0f);
    EXPECT_LE(gpu_props.limits.pointSizeGranularity, 1.0f);
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

TEST(layer, TestSetCombinationMode) {
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
        const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
        profiles_test::setProfilesFilename(filepath);
        profiles_test::setProfilesEmulatePortabilitySubsetExtension(true);
        profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
        profiles_test::setProfilesSimulateCapabilities(0);

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
        const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
        profiles_test::setProfilesFilename(filepath);
        profiles_test::setProfilesEmulatePortabilitySubsetExtension(true);
        profiles_test::setProfilesProfileName("VP_LUNARG_test_api_1_2_198");
        profiles_test::setProfilesSimulateCapabilities(profiles_test::SimulateCapabilityFlag::SIMULATE_EXTENSIONS_BIT);
        profiles_test::setProfilesFailOnError(false);

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
        const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
        profiles_test::setProfilesFilename(filepath);
        profiles_test::setProfilesEmulatePortabilitySubsetExtension(true);
        profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
        profiles_test::setProfilesSimulateCapabilities(profiles_test::SimulateCapabilityFlag::SIMULATE_EXTENSIONS_BIT);
        profiles_test::setProfilesFailOnError(false);

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

            ASSERT_EQ(extensions.size(), 234);
        }

        vkDestroyInstance(test_inst, nullptr);
        inst_builder.reset();
    }
}

TEST(layer, TestExtensionNotSupported) {
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
        const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
        profiles_test::setProfilesFilename(filepath);
        profiles_test::setProfilesEmulatePortabilitySubsetExtension(true);
        profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
        profiles_test::setProfilesSimulateCapabilities(profiles_test::SimulateCapabilityFlag::SIMULATE_EXTENSIONS_BIT);
        profiles_test::setProfilesFailOnError(true);

        err = inst_builder.makeInstance();
        ASSERT_EQ(err, VK_SUCCESS);

        VkInstance test_inst = inst_builder.getInstance();

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(&gpu);

        if (device_extensions.size() < 233) {
            ASSERT_EQ(err, VK_ERROR_INITIALIZATION_FAILED);
        }

        vkDestroyInstance(test_inst, nullptr);
        inst_builder.reset();
    }
}

TEST(layer, TestSelectingProfileAndCapabilities) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    {
        const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_selecting_profile.json";
        profiles_test::setProfilesFilename(filepath);
        profiles_test::setProfilesProfileName("VP_LUNARG_test_selecting_profile");
        profiles_test::setProfilesEmulatePortabilitySubsetExtension(true);
        profiles_test::setProfilesSimulateCapabilities(profiles_test::SimulateCapabilityFlag::SIMULATE_EXTENSIONS_BIT |
                                                       profiles_test::SimulateCapabilityFlag::SIMULATE_PROPERTIES_BIT);

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
        const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_selecting_profile.json";
        profiles_test::setProfilesFilename(filepath);
        profiles_test::setProfilesProfileName("VP_LUNARG_test_selecting_profile_subset");
        profiles_test::setProfilesEmulatePortabilitySubsetExtension(true);
        profiles_test::setProfilesSimulateCapabilities(profiles_test::SimulateCapabilityFlag::SIMULATE_EXTENSIONS_BIT |
                                                       profiles_test::SimulateCapabilityFlag::SIMULATE_PROPERTIES_BIT);

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

TEST(layer, TestParsingAllFormatProperties) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_formats.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_formats");
    profiles_test::setProfilesEmulatePortabilitySubsetExtension(true);
    profiles_test::setProfilesSimulateAllCapabilities();
    profiles_test::setProfilesFailOnError(false);

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

    {
        VkFormat format = VK_FORMAT_R4G4_UNORM_PACK8;
        VkFormatProperties format_properties;
        vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

        VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
        VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
        VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;

        EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
        EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
        EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
    }

    {
        VkFormat format = VK_FORMAT_R4G4B4A4_UNORM_PACK16;
        VkFormatProperties2 format_properties2 = {};
        format_properties2.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
        vkGetPhysicalDeviceFormatProperties2(gpu, format, &format_properties2);

        const VkFormatProperties& format_properties = format_properties2.formatProperties;

        VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
        VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
        VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

        EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
        EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
        EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
    }

#ifdef VK_KHR_format_feature_flags2
    {
        VkFormat format = VK_FORMAT_B4G4R4A4_UNORM_PACK16;
        VkFormatProperties2 format_properties2 = {};
        format_properties2.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
        VkFormatProperties3 format_properties3 = {};
        format_properties3.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3;
        format_properties2.pNext = &format_properties3;
        vkGetPhysicalDeviceFormatProperties2(gpu, format, &format_properties2);

        VkFormatFeatureFlags2KHR linear_tiling_features = VK_FORMAT_FEATURE_2_STORAGE_TEXEL_BUFFER_ATOMIC_BIT_KHR;
        VkFormatFeatureFlags2KHR optimal_tiling_features = VK_FORMAT_FEATURE_2_STORAGE_IMAGE_BIT_KHR;
        VkFormatFeatureFlags2KHR buffer_features = VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BLEND_BIT_KHR;

        EXPECT_EQ(format_properties3.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
        EXPECT_EQ(format_properties3.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
        EXPECT_EQ(format_properties3.bufferFeatures & buffer_features, buffer_features);
    }
    {
        VkFormat format = VK_FORMAT_R5G6B5_UNORM_PACK16;
        VkFormatProperties2 format_properties2 = {};
        format_properties2.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
        VkDrmFormatModifierPropertiesList2EXT drm_format_modifier_properties = {};
        drm_format_modifier_properties.sType = VK_STRUCTURE_TYPE_DRM_FORMAT_MODIFIER_PROPERTIES_LIST_2_EXT;
        format_properties2.pNext = &drm_format_modifier_properties;
        vkGetPhysicalDeviceFormatProperties2(gpu, format, &format_properties2);

        std::vector<VkFormatFeatureFlags2KHR> drm_format_modifier_tiling_features = {
            VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT,
            VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_2_VERTEX_BUFFER_BIT};
        uint32_t count = (uint32_t)drm_format_modifier_tiling_features.size();

        EXPECT_EQ(drm_format_modifier_properties.drmFormatModifierCount, count);
        if (drm_format_modifier_properties.drmFormatModifierCount == count) {
            EXPECT_EQ(drm_format_modifier_properties.pDrmFormatModifierProperties[0].drmFormatModifier, 1);
            EXPECT_EQ(drm_format_modifier_properties.pDrmFormatModifierProperties[0].drmFormatModifierPlaneCount, 2);
            EXPECT_EQ(drm_format_modifier_properties.pDrmFormatModifierProperties[0].drmFormatModifierTilingFeatures &
                          drm_format_modifier_tiling_features[0],
                      drm_format_modifier_tiling_features[0]);

            EXPECT_EQ(drm_format_modifier_properties.pDrmFormatModifierProperties[1].drmFormatModifier, 3);
            EXPECT_EQ(drm_format_modifier_properties.pDrmFormatModifierProperties[1].drmFormatModifierPlaneCount, 4);
            EXPECT_EQ(drm_format_modifier_properties.pDrmFormatModifierProperties[1].drmFormatModifierTilingFeatures &
                          drm_format_modifier_tiling_features[1],
                      drm_format_modifier_tiling_features[1]);
        }
    }
#endif

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestVulkan11Properties) {
#ifdef VK_VERSION_1_2
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_promoted_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance instance = inst_builder.getInstance();

    VkPhysicalDevice physical_device;
    err = inst_builder.getPhysicalDevice(&physical_device);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    if (gpu == VK_NULL_HANDLE) return;

    VkPhysicalDeviceVulkan11Properties vulkan_11_properties{};
    vulkan_11_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &vulkan_11_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(vulkan_11_properties.subgroupSize, 211);
    EXPECT_EQ(vulkan_11_properties.subgroupSupportedStages & VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_VERTEX_BIT);
    EXPECT_EQ(vulkan_11_properties.subgroupSupportedOperations & VK_SUBGROUP_FEATURE_BASIC_BIT, VK_SUBGROUP_FEATURE_BASIC_BIT);
    EXPECT_EQ(vulkan_11_properties.subgroupQuadOperationsInAllStages, VK_TRUE);
    EXPECT_EQ(vulkan_11_properties.maxMultiviewViewCount, 212);
    EXPECT_EQ(vulkan_11_properties.maxMultiviewInstanceIndex, 213);
    EXPECT_EQ(vulkan_11_properties.protectedNoFault, VK_TRUE);
    EXPECT_EQ(vulkan_11_properties.maxPerSetDescriptors, 214);
    EXPECT_EQ(vulkan_11_properties.maxMemoryAllocationSize, 215);
#endif
}

TEST(profiles, TestVulkan11Features) {
#ifdef VK_VERSION_1_2
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_promoted_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance instance = inst_builder.getInstance();

    VkPhysicalDevice physical_device;
    err = inst_builder.getPhysicalDevice(&physical_device);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    if (gpu == VK_NULL_HANDLE) return;

    VkPhysicalDeviceVulkan11Features vulkan_11_features{};
    vulkan_11_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &vulkan_11_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(vulkan_11_features.storageBuffer16BitAccess, VK_TRUE);
    EXPECT_EQ(vulkan_11_features.uniformAndStorageBuffer16BitAccess, VK_TRUE);
    EXPECT_EQ(vulkan_11_features.storagePushConstant16, VK_TRUE);
    EXPECT_EQ(vulkan_11_features.storageInputOutput16, VK_TRUE);
    EXPECT_EQ(vulkan_11_features.multiview, VK_TRUE);
    EXPECT_EQ(vulkan_11_features.multiviewGeometryShader, VK_TRUE);
    EXPECT_EQ(vulkan_11_features.multiviewTessellationShader, VK_TRUE);
    EXPECT_EQ(vulkan_11_features.variablePointersStorageBuffer, VK_TRUE);
    EXPECT_EQ(vulkan_11_features.variablePointers, VK_TRUE);
    EXPECT_EQ(vulkan_11_features.protectedMemory, VK_TRUE);
    EXPECT_EQ(vulkan_11_features.samplerYcbcrConversion, VK_TRUE);
    EXPECT_EQ(vulkan_11_features.shaderDrawParameters, VK_TRUE);
#endif
}

TEST(profiles, TestVulkan12Properties) {
#ifdef VK_VERSION_1_2
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_promoted_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance instance = inst_builder.getInstance();

    VkPhysicalDevice physical_device;
    err = inst_builder.getPhysicalDevice(&physical_device);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    if (gpu == VK_NULL_HANDLE) return;

    VkPhysicalDeviceVulkan12Properties vulkan_12_properties{};
    vulkan_12_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &vulkan_12_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(vulkan_12_properties.denormBehaviorIndependence, VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL);
    EXPECT_EQ(vulkan_12_properties.roundingModeIndependence, VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL);
    EXPECT_EQ(vulkan_12_properties.shaderSignedZeroInfNanPreserveFloat16, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderSignedZeroInfNanPreserveFloat32, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderSignedZeroInfNanPreserveFloat64, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderDenormPreserveFloat16, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderDenormPreserveFloat32, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderDenormPreserveFloat64, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderDenormFlushToZeroFloat16, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderDenormFlushToZeroFloat32, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderDenormFlushToZeroFloat64, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderRoundingModeRTEFloat16, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderRoundingModeRTEFloat32, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderRoundingModeRTEFloat64, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderRoundingModeRTZFloat16, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderRoundingModeRTZFloat32, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderRoundingModeRTZFloat64, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.maxUpdateAfterBindDescriptorsInAllPools, 216);
    EXPECT_EQ(vulkan_12_properties.shaderUniformBufferArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderSampledImageArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderStorageBufferArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderStorageImageArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.shaderInputAttachmentArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.robustBufferAccessUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.quadDivergentImplicitLod, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.maxPerStageDescriptorUpdateAfterBindSamplers, 217);
    EXPECT_EQ(vulkan_12_properties.maxPerStageDescriptorUpdateAfterBindUniformBuffers, 218);
    EXPECT_EQ(vulkan_12_properties.maxPerStageDescriptorUpdateAfterBindStorageBuffers, 219);
    EXPECT_EQ(vulkan_12_properties.maxPerStageDescriptorUpdateAfterBindSampledImages, 220);
    EXPECT_EQ(vulkan_12_properties.maxPerStageDescriptorUpdateAfterBindStorageImages, 221);
    EXPECT_EQ(vulkan_12_properties.maxPerStageDescriptorUpdateAfterBindInputAttachments, 222);
    EXPECT_EQ(vulkan_12_properties.maxPerStageUpdateAfterBindResources, 223);
    EXPECT_EQ(vulkan_12_properties.maxDescriptorSetUpdateAfterBindSamplers, 224);
    EXPECT_EQ(vulkan_12_properties.maxDescriptorSetUpdateAfterBindUniformBuffers, 225);
    EXPECT_EQ(vulkan_12_properties.maxDescriptorSetUpdateAfterBindUniformBuffersDynamic, 226);
    EXPECT_EQ(vulkan_12_properties.maxDescriptorSetUpdateAfterBindStorageBuffers, 227);
    EXPECT_EQ(vulkan_12_properties.maxDescriptorSetUpdateAfterBindStorageBuffersDynamic, 228);
    EXPECT_EQ(vulkan_12_properties.maxDescriptorSetUpdateAfterBindSampledImages, 229);
    EXPECT_EQ(vulkan_12_properties.maxDescriptorSetUpdateAfterBindStorageImages, 230);
    EXPECT_EQ(vulkan_12_properties.maxDescriptorSetUpdateAfterBindInputAttachments, 231);
    EXPECT_EQ(vulkan_12_properties.supportedDepthResolveModes & VK_RESOLVE_MODE_AVERAGE_BIT, VK_RESOLVE_MODE_AVERAGE_BIT);
    EXPECT_EQ(vulkan_12_properties.supportedStencilResolveModes & VK_RESOLVE_MODE_AVERAGE_BIT, VK_RESOLVE_MODE_AVERAGE_BIT);
    EXPECT_EQ(vulkan_12_properties.independentResolveNone, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.independentResolve, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.filterMinmaxSingleComponentFormats, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.filterMinmaxImageComponentMapping, VK_TRUE);
    EXPECT_EQ(vulkan_12_properties.maxTimelineSemaphoreValueDifference, 232);
    EXPECT_EQ(vulkan_12_properties.framebufferIntegerColorSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
#endif
}

TEST(profiles, TestVulkan12Features) {
#ifdef VK_VERSION_1_2
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_promoted_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance instance = inst_builder.getInstance();

    VkPhysicalDevice physical_device;
    err = inst_builder.getPhysicalDevice(&physical_device);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    if (gpu == VK_NULL_HANDLE) return;

    VkPhysicalDeviceVulkan12Features vulkan_12_features{};
    vulkan_12_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &vulkan_12_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(vulkan_12_features.samplerMirrorClampToEdge, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.drawIndirectCount, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.storageBuffer8BitAccess, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.uniformAndStorageBuffer8BitAccess, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.storagePushConstant8, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderBufferInt64Atomics, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderSharedInt64Atomics, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderFloat16, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderInt8, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.descriptorIndexing, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderInputAttachmentArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderUniformTexelBufferArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderStorageTexelBufferArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderUniformBufferArrayNonUniformIndexing, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderSampledImageArrayNonUniformIndexing, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderStorageBufferArrayNonUniformIndexing, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderStorageImageArrayNonUniformIndexing, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderInputAttachmentArrayNonUniformIndexing, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderUniformTexelBufferArrayNonUniformIndexing, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderStorageTexelBufferArrayNonUniformIndexing, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.descriptorBindingUniformBufferUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.descriptorBindingSampledImageUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.descriptorBindingStorageImageUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.descriptorBindingStorageBufferUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.descriptorBindingUniformTexelBufferUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.descriptorBindingStorageTexelBufferUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.descriptorBindingUpdateUnusedWhilePending, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.descriptorBindingPartiallyBound, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.descriptorBindingVariableDescriptorCount, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.runtimeDescriptorArray, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.samplerFilterMinmax, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.scalarBlockLayout, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.imagelessFramebuffer, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.uniformBufferStandardLayout, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderSubgroupExtendedTypes, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.separateDepthStencilLayouts, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.hostQueryReset, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.timelineSemaphore, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.bufferDeviceAddress, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.bufferDeviceAddressCaptureReplay, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.bufferDeviceAddressMultiDevice, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.vulkanMemoryModel, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.vulkanMemoryModelDeviceScope, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.vulkanMemoryModelAvailabilityVisibilityChains, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderOutputViewportIndex, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.shaderOutputLayer, VK_TRUE);
    EXPECT_EQ(vulkan_12_features.subgroupBroadcastDynamicId, VK_TRUE);
#endif
}

TEST(profiles, TestVulkan13Properties) {
#ifdef VK_VERSION_1_3
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_promoted_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance instance = inst_builder.getInstance();

    VkPhysicalDevice physical_device;
    err = inst_builder.getPhysicalDevice(&physical_device);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    if (gpu == VK_NULL_HANDLE) return;

    VkPhysicalDeviceVulkan13Properties vulkan_13_properties{};
    vulkan_13_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &vulkan_13_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(vulkan_13_properties.minSubgroupSize, 338);
    EXPECT_EQ(vulkan_13_properties.maxSubgroupSize, 339);
    EXPECT_EQ(vulkan_13_properties.maxComputeWorkgroupSubgroups, 340);
    EXPECT_EQ(vulkan_13_properties.requiredSubgroupSizeStages & (VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
              VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
    EXPECT_EQ(vulkan_13_properties.maxInlineUniformBlockSize, 271);
    EXPECT_EQ(vulkan_13_properties.maxPerStageDescriptorInlineUniformBlocks, 272);
    EXPECT_EQ(vulkan_13_properties.maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks, 273);
    EXPECT_EQ(vulkan_13_properties.maxDescriptorSetInlineUniformBlocks, 274);
    EXPECT_EQ(vulkan_13_properties.maxDescriptorSetUpdateAfterBindInlineUniformBlocks, 275);
    EXPECT_EQ(vulkan_13_properties.maxInlineUniformTotalSize, 76);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct8BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct8BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct8BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct4x8BitPackedUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct4x8BitPackedSignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct4x8BitPackedMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct16BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct16BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct16BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct32BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct32BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct32BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct64BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct64BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProduct64BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating8BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating8BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating8BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating4x8BitPackedUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating4x8BitPackedSignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating4x8BitPackedMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating16BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating16BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating16BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating32BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating32BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating32BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating64BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating64BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.integerDotProductAccumulatingSaturating64BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.storageTexelBufferOffsetAlignmentBytes, 352);
    EXPECT_EQ(vulkan_13_properties.storageTexelBufferOffsetSingleTexelAlignment, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.uniformTexelBufferOffsetAlignmentBytes, 353);
    EXPECT_EQ(vulkan_13_properties.uniformTexelBufferOffsetSingleTexelAlignment, VK_TRUE);
    EXPECT_EQ(vulkan_13_properties.maxBufferSize, 260);
#endif
}

TEST(profiles, TestVulkan13Features) {
#ifdef VK_VERSION_1_2
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_promoted_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance instance = inst_builder.getInstance();

    VkPhysicalDevice physical_device;
    err = inst_builder.getPhysicalDevice(&physical_device);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    if (gpu == VK_NULL_HANDLE) return;

    VkPhysicalDeviceVulkan13Features vulkan_13_features{};
    vulkan_13_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &vulkan_13_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(vulkan_13_features.robustImageAccess, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.inlineUniformBlock, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.descriptorBindingInlineUniformBlockUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.pipelineCreationCacheControl, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.privateData, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.shaderDemoteToHelperInvocation, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.shaderTerminateInvocation, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.subgroupSizeControl, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.computeFullSubgroups, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.synchronization2, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.textureCompressionASTC_HDR, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.shaderZeroInitializeWorkgroupMemory, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.dynamicRendering, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.shaderIntegerDotProduct, VK_TRUE);
    EXPECT_EQ(vulkan_13_features.maintenance4, VK_TRUE);
#endif
}

TEST(profiles, TestDuplicatedMembers) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_duplicated.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_duplicated");
    profiles_test::setProfilesSimulateAllCapabilities();
    profiles_test::setProfilesFailOnError(true);

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance instance = inst_builder.getInstance();

    VkPhysicalDevice physical_device;
    err = inst_builder.getPhysicalDevice(&physical_device);
    ASSERT_EQ(err, VK_ERROR_INITIALIZATION_FAILED);
}
