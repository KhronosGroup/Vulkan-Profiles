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

TEST(profiles, TestPhysicalDeviceProperties) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    const auto& limits = gpu_props.properties.limits;
    EXPECT_GE(limits.maxImageDimension1D, 102u);
    EXPECT_GE(limits.maxImageDimension2D, 103u);
    EXPECT_GE(limits.maxImageDimension3D, 104u);
    EXPECT_GE(limits.maxImageDimensionCube, 105u);
    EXPECT_GE(limits.maxImageArrayLayers, 106u);
    EXPECT_GE(limits.maxTexelBufferElements, 107u);
    EXPECT_GE(limits.maxUniformBufferRange, 108u);
    EXPECT_GE(limits.maxStorageBufferRange, 109u);
    EXPECT_GE(limits.maxPushConstantsSize, 110u);
    EXPECT_GE(limits.maxMemoryAllocationCount, 111u);
    EXPECT_GE(limits.maxSamplerAllocationCount, 112u);
    EXPECT_GE(limits.bufferImageGranularity, 113u);
    EXPECT_GE(limits.sparseAddressSpaceSize, 114u);
    EXPECT_GE(limits.maxBoundDescriptorSets, 115u);
    EXPECT_GE(limits.maxPerStageDescriptorSamplers, 116u);
    EXPECT_GE(limits.maxPerStageDescriptorUniformBuffers, 117u);
    EXPECT_GE(limits.maxPerStageDescriptorStorageBuffers, 118u);
    EXPECT_GE(limits.maxPerStageDescriptorSampledImages, 119u);
    EXPECT_GE(limits.maxPerStageDescriptorStorageImages, 120u);
    EXPECT_GE(limits.maxPerStageDescriptorInputAttachments, 121u);
    EXPECT_GE(limits.maxPerStageResources, 122u);
    EXPECT_GE(limits.maxDescriptorSetSamplers, 123u);
    EXPECT_GE(limits.maxDescriptorSetUniformBuffers, 124u);
    EXPECT_GE(limits.maxDescriptorSetUniformBuffersDynamic, 125u);
    EXPECT_GE(limits.maxDescriptorSetStorageBuffers, 126u);
    EXPECT_GE(limits.maxDescriptorSetStorageBuffersDynamic, 127u);
    EXPECT_GE(limits.maxDescriptorSetSampledImages, 128u);
    EXPECT_GE(limits.maxDescriptorSetStorageImages, 129u);
    EXPECT_GE(limits.maxDescriptorSetInputAttachments, 130u);
    EXPECT_GE(limits.maxVertexInputAttributes, 131u);
    EXPECT_GE(limits.maxVertexInputBindings, 132u);
    EXPECT_GE(limits.maxVertexInputAttributeOffset, 133u);
    EXPECT_GE(limits.maxVertexInputBindingStride, 134u);
    EXPECT_GE(limits.maxVertexOutputComponents, 135u);
    EXPECT_GE(limits.maxTessellationGenerationLevel, 136u);
    EXPECT_GE(limits.maxTessellationPatchSize, 137u);
    EXPECT_GE(limits.maxTessellationControlPerVertexInputComponents, 138u);
    EXPECT_GE(limits.maxTessellationControlPerVertexOutputComponents, 139u);
    EXPECT_GE(limits.maxTessellationControlPerPatchOutputComponents, 140u);
    EXPECT_GE(limits.maxTessellationControlTotalOutputComponents, 141u);
    EXPECT_GE(limits.maxTessellationEvaluationInputComponents, 142u);
    EXPECT_GE(limits.maxTessellationEvaluationOutputComponents, 143u);
    EXPECT_GE(limits.maxGeometryShaderInvocations, 144u);
    EXPECT_GE(limits.maxGeometryInputComponents, 145u);
    EXPECT_GE(limits.maxGeometryOutputComponents, 146u);
    EXPECT_GE(limits.maxGeometryOutputVertices, 147u);
    EXPECT_GE(limits.maxGeometryTotalOutputComponents, 148u);
    EXPECT_GE(limits.maxFragmentInputComponents, 149u);
    EXPECT_GE(limits.maxFragmentOutputAttachments, 150u);
    EXPECT_GE(limits.maxFragmentDualSrcAttachments, 151u);
    EXPECT_GE(limits.maxFragmentCombinedOutputResources, 152u);
    EXPECT_GE(limits.maxComputeSharedMemorySize, 153u);
    EXPECT_GE(limits.maxComputeWorkGroupCount[0], 154u);
    EXPECT_GE(limits.maxComputeWorkGroupCount[1], 155u);
    EXPECT_GE(limits.maxComputeWorkGroupCount[2], 156u);
    EXPECT_GE(limits.maxComputeWorkGroupInvocations, 157u);
    EXPECT_GE(limits.maxComputeWorkGroupSize[0], 158u);
    EXPECT_GE(limits.maxComputeWorkGroupSize[1], 159u);
    EXPECT_GE(limits.maxComputeWorkGroupSize[2], 160u);
    EXPECT_GE(limits.subPixelPrecisionBits, 161u);
    EXPECT_GE(limits.subTexelPrecisionBits, 162u);
    EXPECT_GE(limits.mipmapPrecisionBits, 163u);
    EXPECT_GE(limits.maxDrawIndexedIndexValue, 164u);
    EXPECT_GE(limits.maxDrawIndirectCount, 165u);
    EXPECT_GE(limits.maxSamplerLodBias, 166.0f);
    EXPECT_GE(limits.maxSamplerAnisotropy, 167.0f);
    EXPECT_GE(limits.maxViewports, 168u);
    EXPECT_GE(limits.maxViewportDimensions[0], 169u);
    EXPECT_GE(limits.maxViewportDimensions[1], 170u);
    EXPECT_GE(limits.viewportBoundsRange[0], 171.0f);
    EXPECT_GE(limits.viewportBoundsRange[1], 172.0f);
    EXPECT_GE(limits.viewportSubPixelBits, 173u);
    EXPECT_GE(limits.minMemoryMapAlignment, 174u);
    EXPECT_GE(limits.minTexelBufferOffsetAlignment, 175u);
    EXPECT_GE(limits.minUniformBufferOffsetAlignment, 176u);
    EXPECT_GE(limits.minStorageBufferOffsetAlignment, 177u);
    EXPECT_GE(limits.minTexelOffset, -178);
    EXPECT_GE(limits.maxTexelOffset, 179u);
    EXPECT_GE(limits.minTexelGatherOffset, -180);
    EXPECT_GE(limits.maxTexelGatherOffset, 181u);
    EXPECT_GE(limits.minInterpolationOffset, 182.0f);
    EXPECT_GE(limits.maxInterpolationOffset, 183.0f);
    EXPECT_GE(limits.subPixelInterpolationOffsetBits, 184u);
    EXPECT_GE(limits.maxFramebufferWidth, 185u);
    EXPECT_GE(limits.maxFramebufferHeight, 186u);
    EXPECT_GE(limits.maxFramebufferLayers, 187u);
    EXPECT_EQ(limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(limits.framebufferDepthSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(limits.framebufferStencilSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(limits.framebufferNoAttachmentsSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_GE(limits.maxColorAttachments, 188u);
    EXPECT_EQ(limits.sampledImageColorSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(limits.sampledImageIntegerSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(limits.sampledImageDepthSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(limits.sampledImageStencilSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(limits.storageImageSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_GE(limits.maxSampleMaskWords, 189u);
    EXPECT_GE(limits.timestampComputeAndGraphics, VK_TRUE);
    EXPECT_GE(limits.timestampPeriod, 190.0f);
    EXPECT_GE(limits.maxClipDistances, 191u);
    EXPECT_GE(limits.maxCullDistances, 192u);
    EXPECT_GE(limits.maxCombinedClipAndCullDistances, 193u);
    EXPECT_GE(limits.discreteQueuePriorities, 194u);
    EXPECT_GE(limits.pointSizeRange[0], 195.0f);
    EXPECT_GE(limits.pointSizeRange[1], 196.0f);
    EXPECT_GE(limits.lineWidthRange[0], 197.0f);
    EXPECT_GE(limits.lineWidthRange[1], 198.0f);
    EXPECT_GE(limits.pointSizeGranularity, 1200.0f);
    EXPECT_GE(limits.lineWidthGranularity, 201.0f);
    EXPECT_GE(limits.strictLines, VK_TRUE);
    EXPECT_GE(limits.standardSampleLocations, VK_TRUE);
    EXPECT_GE(limits.optimalBufferCopyOffsetAlignment, 202u);
    EXPECT_GE(limits.optimalBufferCopyRowPitchAlignment, 203u);
    EXPECT_GE(limits.nonCoherentAtomSize, 204u);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestDepthStencilResolveProperties) {
#ifdef VK_KHR_depth_stencil_resolve
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceDepthStencilResolvePropertiesKHR depth_stencil_resolve_properties{};
    depth_stencil_resolve_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &depth_stencil_resolve_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(depth_stencil_resolve_properties.supportedDepthResolveModes & VK_RESOLVE_MODE_AVERAGE_BIT,
              VK_RESOLVE_MODE_AVERAGE_BIT);
    EXPECT_EQ(depth_stencil_resolve_properties.supportedStencilResolveModes & VK_RESOLVE_MODE_AVERAGE_BIT,
              VK_RESOLVE_MODE_AVERAGE_BIT);
    EXPECT_GE(depth_stencil_resolve_properties.independentResolveNone, VK_TRUE);
    EXPECT_GE(depth_stencil_resolve_properties.independentResolve, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestSubgroupProperties) {
#ifdef VK_VERSION_1_1
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceSubgroupProperties subgroup_properties{};
    subgroup_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &subgroup_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(subgroup_properties.subgroupSize, 205u);
    EXPECT_EQ(subgroup_properties.supportedStages & (VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT),
              (VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT));
    EXPECT_EQ(subgroup_properties.supportedOperations & (VK_SUBGROUP_FEATURE_BASIC_BIT), (VK_SUBGROUP_FEATURE_BASIC_BIT));
    EXPECT_EQ(subgroup_properties.quadOperationsInAllStages, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestDescriptorIndexingProperties) {
#ifdef VK_EXT_descriptor_indexing
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptor_indexing_properties{};
    descriptor_indexing_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &descriptor_indexing_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(descriptor_indexing_properties.maxUpdateAfterBindDescriptorsInAllPools, 233u);
    EXPECT_GE(descriptor_indexing_properties.shaderUniformBufferArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_GE(descriptor_indexing_properties.shaderSampledImageArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_GE(descriptor_indexing_properties.shaderStorageBufferArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_GE(descriptor_indexing_properties.shaderStorageImageArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_GE(descriptor_indexing_properties.shaderInputAttachmentArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_GE(descriptor_indexing_properties.robustBufferAccessUpdateAfterBind, VK_TRUE);
    EXPECT_GE(descriptor_indexing_properties.quadDivergentImplicitLod, VK_TRUE);
    EXPECT_GE(descriptor_indexing_properties.maxPerStageDescriptorUpdateAfterBindSamplers, 234u);
    EXPECT_GE(descriptor_indexing_properties.maxPerStageDescriptorUpdateAfterBindUniformBuffers, 235u);
    EXPECT_GE(descriptor_indexing_properties.maxPerStageDescriptorUpdateAfterBindStorageBuffers, 236u);
    EXPECT_GE(descriptor_indexing_properties.maxPerStageDescriptorUpdateAfterBindSampledImages, 237u);
    EXPECT_GE(descriptor_indexing_properties.maxPerStageDescriptorUpdateAfterBindStorageImages, 238u);
    EXPECT_GE(descriptor_indexing_properties.maxPerStageDescriptorUpdateAfterBindInputAttachments, 239u);
    EXPECT_GE(descriptor_indexing_properties.maxPerStageUpdateAfterBindResources, 240u);
    EXPECT_GE(descriptor_indexing_properties.maxDescriptorSetUpdateAfterBindSamplers, 241u);
    EXPECT_GE(descriptor_indexing_properties.maxDescriptorSetUpdateAfterBindUniformBuffers, 242u);
    EXPECT_GE(descriptor_indexing_properties.maxDescriptorSetUpdateAfterBindUniformBuffersDynamic, 243u);
    EXPECT_GE(descriptor_indexing_properties.maxDescriptorSetUpdateAfterBindStorageBuffers, 244u);
    EXPECT_GE(descriptor_indexing_properties.maxDescriptorSetUpdateAfterBindStorageBuffersDynamic, 245u);
    EXPECT_GE(descriptor_indexing_properties.maxDescriptorSetUpdateAfterBindSampledImages, 246u);
    EXPECT_GE(descriptor_indexing_properties.maxDescriptorSetUpdateAfterBindStorageImages, 247u);
    EXPECT_GE(descriptor_indexing_properties.maxDescriptorSetUpdateAfterBindInputAttachments, 248u);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestFloatControlsProperties) {
#ifdef VK_KHR_shader_float_controls
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceFloatControlsPropertiesKHR float_control_properties{};
    float_control_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT_CONTROLS_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &float_control_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(float_control_properties.denormBehaviorIndependence & VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL,
              VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL);
    EXPECT_EQ(float_control_properties.roundingModeIndependence & VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL,
              VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL);
    EXPECT_GE(float_control_properties.shaderSignedZeroInfNanPreserveFloat16, VK_TRUE);
    EXPECT_GE(float_control_properties.shaderSignedZeroInfNanPreserveFloat32, VK_TRUE);
    EXPECT_GE(float_control_properties.shaderSignedZeroInfNanPreserveFloat64, VK_TRUE);
    EXPECT_GE(float_control_properties.shaderDenormPreserveFloat16, VK_TRUE);
    EXPECT_GE(float_control_properties.shaderDenormPreserveFloat32, VK_TRUE);
    EXPECT_GE(float_control_properties.shaderDenormPreserveFloat64, VK_TRUE);
    EXPECT_GE(float_control_properties.shaderDenormFlushToZeroFloat16, VK_TRUE);
    EXPECT_GE(float_control_properties.shaderDenormFlushToZeroFloat32, VK_TRUE);
    EXPECT_GE(float_control_properties.shaderDenormFlushToZeroFloat64, VK_TRUE);
    EXPECT_GE(float_control_properties.shaderRoundingModeRTEFloat16, VK_TRUE);
    EXPECT_GE(float_control_properties.shaderRoundingModeRTEFloat32, VK_TRUE);
    EXPECT_GE(float_control_properties.shaderRoundingModeRTEFloat64, VK_TRUE);
    EXPECT_GE(float_control_properties.shaderRoundingModeRTZFloat16, VK_TRUE);
    EXPECT_GE(float_control_properties.shaderRoundingModeRTZFloat32, VK_TRUE);
    EXPECT_GE(float_control_properties.shaderRoundingModeRTZFloat64, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestMaintenance3Properties) {
#ifdef VK_KHR_maintenance3
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceMaintenance3PropertiesKHR maintenance_3_properties{};
    maintenance_3_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &maintenance_3_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(maintenance_3_properties.maxPerSetDescriptors, 208u);
    EXPECT_GE(maintenance_3_properties.maxMemoryAllocationSize, 209u);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestMaintenance4Properties) {
#ifdef VK_KHR_maintenance4
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceMaintenance4PropertiesKHR maintenance_4_properties{};
    maintenance_4_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &maintenance_4_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(maintenance_4_properties.maxBufferSize, 260u);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestMultiviewProperties) {
#ifdef VK_KHR_multiview
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceMultiviewPropertiesKHR multiview_properties{};
    multiview_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &multiview_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(multiview_properties.maxMultiviewViewCount, 206u);
    EXPECT_GE(multiview_properties.maxMultiviewInstanceIndex, 207u);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestPointClippingProperties) {
#ifdef VK_KHR_maintenance2
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevicePointClippingPropertiesKHR point_clipping_properties{};
    point_clipping_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_POINT_CLIPPING_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &point_clipping_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(point_clipping_properties.pointClippingBehavior & VK_POINT_CLIPPING_BEHAVIOR_ALL_CLIP_PLANES,
              VK_POINT_CLIPPING_BEHAVIOR_ALL_CLIP_PLANES);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestProtectedMemoryProperties) {
#ifdef VK_VERSION_1_1
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceProtectedMemoryProperties protected_memory_properties{};
    protected_memory_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_PROPERTIES;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &protected_memory_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(protected_memory_properties.protectedNoFault, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestTimelineSemaphoreProperties) {
#ifdef VK_KHR_timeline_semaphore
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceTimelineSemaphorePropertiesKHR timeline_semaphore_properties{};
    timeline_semaphore_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_PROPERTIES;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &timeline_semaphore_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(timeline_semaphore_properties.maxTimelineSemaphoreValueDifference, 249u);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestSamplerFilterMinmaxProperties) {
#ifdef VK_EXT_sampler_filter_minmax
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT sampler_filter_minmax_properties{};
    sampler_filter_minmax_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_FILTER_MINMAX_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &sampler_filter_minmax_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(sampler_filter_minmax_properties.filterMinmaxSingleComponentFormats, VK_TRUE);
    EXPECT_GE(sampler_filter_minmax_properties.filterMinmaxImageComponentMapping, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestAccelerationStructurePropertiesKHR) {
#ifdef VK_KHR_acceleration_structure
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceAccelerationStructurePropertiesKHR acceleration_structure_properties{};
    acceleration_structure_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &acceleration_structure_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(acceleration_structure_properties.maxGeometryCount, 361u);
    EXPECT_GE(acceleration_structure_properties.maxInstanceCount, 362u);
    EXPECT_GE(acceleration_structure_properties.maxPrimitiveCount, 363u);
    EXPECT_GE(acceleration_structure_properties.maxPerStageDescriptorAccelerationStructures, 364u);
    EXPECT_GE(acceleration_structure_properties.maxPerStageDescriptorUpdateAfterBindAccelerationStructures, 365u);
    EXPECT_GE(acceleration_structure_properties.maxDescriptorSetAccelerationStructures, 366u);
    EXPECT_GE(acceleration_structure_properties.maxDescriptorSetUpdateAfterBindAccelerationStructures, 367u);
    EXPECT_GE(acceleration_structure_properties.minAccelerationStructureScratchOffsetAlignment, 368u);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestPerformanceQueryProperties) {
#ifdef VK_KHR_performance_query
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevicePerformanceQueryPropertiesKHR performance_query_properties{};
    performance_query_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &performance_query_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(performance_query_properties.allowCommandBufferQueryCopies, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestPushDescriptorProperties) {
#ifdef VK_KHR_push_descriptor
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevicePushDescriptorPropertiesKHR push_descriptor_properties{};
    push_descriptor_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &push_descriptor_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(push_descriptor_properties.maxPushDescriptors, 250u);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestRayTracingPipelineProperties) {
#ifdef VK_KHR_ray_tracing_pipeline
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceRayTracingPipelinePropertiesKHR ray_tracing_pipeline_properties{};
    ray_tracing_pipeline_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &ray_tracing_pipeline_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(ray_tracing_pipeline_properties.shaderGroupHandleSize, 369u);
    EXPECT_GE(ray_tracing_pipeline_properties.maxRayRecursionDepth, 370u);
    EXPECT_GE(ray_tracing_pipeline_properties.maxShaderGroupStride, 371u);
    EXPECT_GE(ray_tracing_pipeline_properties.shaderGroupBaseAlignment, 372u);
    EXPECT_GE(ray_tracing_pipeline_properties.shaderGroupHandleCaptureReplaySize, 373u);
    EXPECT_GE(ray_tracing_pipeline_properties.maxRayDispatchInvocationCount, 374u);
    EXPECT_GE(ray_tracing_pipeline_properties.shaderGroupHandleAlignment, 375u);
    EXPECT_GE(ray_tracing_pipeline_properties.maxRayHitAttributeSize, 376u);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestShaderIntegerDotProductProperties) {
#ifdef VK_KHR_shader_integer_dot_product
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR shader_integer_dot_product_properties{};
    shader_integer_dot_product_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &shader_integer_dot_product_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(shader_integer_dot_product_properties.integerDotProduct8BitUnsignedAccelerated, VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProduct8BitSignedAccelerated, VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProduct8BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProduct4x8BitPackedUnsignedAccelerated, VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProduct4x8BitPackedSignedAccelerated, VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProduct4x8BitPackedMixedSignednessAccelerated, VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProduct16BitUnsignedAccelerated, VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProduct16BitSignedAccelerated, VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProduct16BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProduct32BitUnsignedAccelerated, VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProduct32BitSignedAccelerated, VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProduct32BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProduct64BitUnsignedAccelerated, VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProduct64BitSignedAccelerated, VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProduct64BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating8BitUnsignedAccelerated, VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating8BitSignedAccelerated, VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating8BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating4x8BitPackedUnsignedAccelerated,
              VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating4x8BitPackedSignedAccelerated, VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating4x8BitPackedMixedSignednessAccelerated,
              VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating16BitUnsignedAccelerated, VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating16BitSignedAccelerated, VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating16BitMixedSignednessAccelerated,
              VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating32BitUnsignedAccelerated, VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating32BitSignedAccelerated, VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating32BitMixedSignednessAccelerated,
              VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating64BitUnsignedAccelerated, VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating64BitSignedAccelerated, VK_TRUE);
    EXPECT_GE(shader_integer_dot_product_properties.integerDotProductAccumulatingSaturating64BitMixedSignednessAccelerated,
              VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestBlendOperationAdvancedProperties) {
#ifdef VK_EXT_blend_operation_advanced
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT blend_operation_advanced_properties{};
    blend_operation_advanced_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &blend_operation_advanced_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(blend_operation_advanced_properties.advancedBlendMaxColorAttachments, 281u);
    EXPECT_GE(blend_operation_advanced_properties.advancedBlendIndependentBlend, VK_TRUE);
    EXPECT_GE(blend_operation_advanced_properties.advancedBlendNonPremultipliedSrcColor, VK_TRUE);
    EXPECT_GE(blend_operation_advanced_properties.advancedBlendNonPremultipliedDstColor, VK_TRUE);
    EXPECT_GE(blend_operation_advanced_properties.advancedBlendCorrelatedOverlap, VK_TRUE);
    EXPECT_GE(blend_operation_advanced_properties.advancedBlendAllOperations, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestConservativeRasterizationProperties) {
#ifdef VK_EXT_conservative_rasterization
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceConservativeRasterizationPropertiesEXT conservative_rasterization_properties{};
    conservative_rasterization_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONSERVATIVE_RASTERIZATION_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &conservative_rasterization_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(conservative_rasterization_properties.primitiveOverestimationSize, 268.0f);
    EXPECT_GE(conservative_rasterization_properties.maxExtraPrimitiveOverestimationSize, 269.0f);
    EXPECT_GE(conservative_rasterization_properties.extraPrimitiveOverestimationSizeGranularity, 270.0f);
    EXPECT_GE(conservative_rasterization_properties.primitiveUnderestimation, VK_TRUE);
    EXPECT_GE(conservative_rasterization_properties.conservativePointAndLineRasterization, VK_TRUE);
    EXPECT_GE(conservative_rasterization_properties.degenerateTrianglesRasterized, VK_TRUE);
    EXPECT_GE(conservative_rasterization_properties.degenerateLinesRasterized, VK_TRUE);
    EXPECT_GE(conservative_rasterization_properties.fullyCoveredFragmentShaderInputVariable, VK_TRUE);
    EXPECT_GE(conservative_rasterization_properties.conservativeRasterizationPostDepthCoverage, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestCustomBorderColorProperties) {
#ifdef VK_EXT_custom_border_color
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceCustomBorderColorPropertiesEXT custom_border_color_properties{};
    custom_border_color_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &custom_border_color_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(custom_border_color_properties.maxCustomBorderColorSamplers, 356u);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestDiscardRectangleProperties) {
#ifdef VK_EXT_discard_rectangles
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceDiscardRectanglePropertiesEXT discard_rectangle_properties{};
    discard_rectangle_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DISCARD_RECTANGLE_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &discard_rectangle_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(discard_rectangle_properties.maxDiscardRectangles, 267u);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestExternalMemoryHostPropertiesEXT) {
#ifdef VK_EXT_external_memory_host
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceExternalMemoryHostPropertiesEXT external_memory_host_properties{};
    external_memory_host_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_HOST_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &external_memory_host_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(external_memory_host_properties.minImportedHostPointerAlignment, 296u);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestFragmentDensityMapProperties) {
#ifdef VK_EXT_fragment_density_map
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceFragmentDensityMapPropertiesEXT fragment_density_map_properties{};
    fragment_density_map_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &fragment_density_map_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_LE(fragment_density_map_properties.minFragmentDensityTexelSize.width, 334u);
    EXPECT_LE(fragment_density_map_properties.minFragmentDensityTexelSize.height, 335u);
    EXPECT_GE(fragment_density_map_properties.maxFragmentDensityTexelSize.width, 336u);
    EXPECT_GE(fragment_density_map_properties.maxFragmentDensityTexelSize.height, 337u);
    EXPECT_GE(fragment_density_map_properties.fragmentDensityInvocations, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestInlineUniformBlockProperties) {
#ifdef VK_EXT_inline_uniform_block
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceInlineUniformBlockPropertiesEXT inline_uniform_block_properties{};
    inline_uniform_block_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &inline_uniform_block_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(inline_uniform_block_properties.maxInlineUniformBlockSize, 271u);
    EXPECT_GE(inline_uniform_block_properties.maxPerStageDescriptorInlineUniformBlocks, 272u);
    EXPECT_GE(inline_uniform_block_properties.maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks, 273u);
    EXPECT_GE(inline_uniform_block_properties.maxDescriptorSetInlineUniformBlocks, 274u);
    EXPECT_GE(inline_uniform_block_properties.maxDescriptorSetUpdateAfterBindInlineUniformBlocks, 275u);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestLineRasterizationProperties) {
#ifdef VK_EXT_line_rasterization
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceLineRasterizationPropertiesEXT line_rasterization_properties{};
    line_rasterization_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &line_rasterization_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(line_rasterization_properties.lineSubPixelPrecisionBits, 342u);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestMultiDrawProperties) {
#ifdef VK_EXT_multi_draw
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceMultiDrawPropertiesEXT mutli_draw_properties{};
    mutli_draw_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &mutli_draw_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(mutli_draw_properties.maxMultiDrawCount, 360u);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestProvokingVertexProperties) {
#ifdef VK_EXT_provoking_vertex
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceProvokingVertexPropertiesEXT provoking_vertex_properties{};
    provoking_vertex_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &provoking_vertex_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(provoking_vertex_properties.provokingVertexModePerPipeline, VK_TRUE);
    EXPECT_GE(provoking_vertex_properties.transformFeedbackPreservesTriangleFanProvokingVertex, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestRobustness2Properties) {
#ifdef VK_EXT_robustness2
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceRobustness2PropertiesEXT robustness_2_properties{};
    robustness_2_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &robustness_2_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(robustness_2_properties.robustStorageBufferAccessSizeAlignment, 354u);
    EXPECT_GE(robustness_2_properties.robustUniformBufferAccessSizeAlignment, 355u);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestSampleLocationsProperties) {
#ifdef VK_EXT_sample_locations
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceSampleLocationsPropertiesEXT sample_locations_properties{};
    sample_locations_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLE_LOCATIONS_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &sample_locations_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    // EXPECT_EQ(sample_locations_properties.sampleLocationSampleCounts & VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_GE(sample_locations_properties.maxSampleLocationGridSize.width, 276u);
    EXPECT_GE(sample_locations_properties.maxSampleLocationGridSize.height, 277u);
    EXPECT_GE(sample_locations_properties.sampleLocationCoordinateRange[0], 278.0f);
    EXPECT_GE(sample_locations_properties.sampleLocationCoordinateRange[1], 279.0f);
    EXPECT_GE(sample_locations_properties.sampleLocationSubPixelBits, 280u);
    EXPECT_GE(sample_locations_properties.variableSampleLocations, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestSubgroupSizeControlProperties) {
#ifdef VK_EXT_subgroup_size_control
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceSubgroupSizeControlPropertiesEXT subgroup_size_control_properties{};
    subgroup_size_control_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &subgroup_size_control_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(subgroup_size_control_properties.minSubgroupSize, 338u);
    EXPECT_GE(subgroup_size_control_properties.maxSubgroupSize, 339u);
    EXPECT_GE(subgroup_size_control_properties.maxComputeWorkgroupSubgroups, 340u);
    EXPECT_EQ(subgroup_size_control_properties.requiredSubgroupSizeStages & VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_VERTEX_BIT);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestTexelBufferAlignmentProperties) {
#ifdef VK_EXT_texel_buffer_alignment
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT texel_buffer_alignment_properties{};
    texel_buffer_alignment_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &texel_buffer_alignment_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(texel_buffer_alignment_properties.storageTexelBufferOffsetAlignmentBytes, 352u);
    EXPECT_GE(texel_buffer_alignment_properties.storageTexelBufferOffsetSingleTexelAlignment, VK_TRUE);
    EXPECT_GE(texel_buffer_alignment_properties.uniformTexelBufferOffsetAlignmentBytes, 353u);
    EXPECT_GE(texel_buffer_alignment_properties.uniformTexelBufferOffsetSingleTexelAlignment, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestTransformFeedbackProperties) {
#ifdef VK_EXT_transform_feedback
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceTransformFeedbackPropertiesEXT transform_feedback_properties{};
    transform_feedback_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &transform_feedback_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(transform_feedback_properties.maxTransformFeedbackStreams, 261u);
    EXPECT_GE(transform_feedback_properties.maxTransformFeedbackBuffers, 262u);
    EXPECT_GE(transform_feedback_properties.maxTransformFeedbackBufferSize, 263u);
    EXPECT_GE(transform_feedback_properties.maxTransformFeedbackStreamDataSize, 264u);
    EXPECT_GE(transform_feedback_properties.maxTransformFeedbackBufferDataSize, 265u);
    EXPECT_GE(transform_feedback_properties.maxTransformFeedbackBufferDataStride, 266u);
    EXPECT_GE(transform_feedback_properties.transformFeedbackQueries, VK_TRUE);
    EXPECT_GE(transform_feedback_properties.transformFeedbackStreamsLinesTriangles, VK_TRUE);
    EXPECT_GE(transform_feedback_properties.transformFeedbackRasterizationStreamSelect, VK_TRUE);
    EXPECT_GE(transform_feedback_properties.transformFeedbackDraw, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestVertexAttributeDivisorProperties) {
#ifdef VK_EXT_vertex_attribute_divisor
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT vertex_attribute_divisor_properties{};
    vertex_attribute_divisor_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &vertex_attribute_divisor_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(vertex_attribute_divisor_properties.maxVertexAttribDivisor, 312u);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestFragmentShadingRateProperties) {
#ifdef VK_KHR_fragment_shading_rate
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceFragmentShadingRatePropertiesKHR fragment_shading_rate_properties{};
    fragment_shading_rate_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &fragment_shading_rate_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_LE(fragment_shading_rate_properties.minFragmentShadingRateAttachmentTexelSize.width, 251u);
    EXPECT_LE(fragment_shading_rate_properties.minFragmentShadingRateAttachmentTexelSize.height, 252u);
    EXPECT_GE(fragment_shading_rate_properties.maxFragmentShadingRateAttachmentTexelSize.width, 253u);
    EXPECT_GE(fragment_shading_rate_properties.maxFragmentShadingRateAttachmentTexelSize.height, 254u);
    EXPECT_GE(fragment_shading_rate_properties.maxFragmentShadingRateAttachmentTexelSizeAspectRatio, 255u);
    EXPECT_GE(fragment_shading_rate_properties.primitiveFragmentShadingRateWithMultipleViewports, VK_TRUE);
    EXPECT_GE(fragment_shading_rate_properties.layeredShadingRateAttachments, VK_TRUE);
    EXPECT_GE(fragment_shading_rate_properties.fragmentShadingRateNonTrivialCombinerOps, VK_TRUE);
    EXPECT_GE(fragment_shading_rate_properties.maxFragmentSize.width, 256u);
    EXPECT_GE(fragment_shading_rate_properties.maxFragmentSize.height, 257u);
    EXPECT_GE(fragment_shading_rate_properties.maxFragmentSizeAspectRatio, 258u);
    EXPECT_GE(fragment_shading_rate_properties.maxFragmentShadingRateCoverageSamples, 259u);
    EXPECT_EQ(fragment_shading_rate_properties.maxFragmentShadingRateRasterizationSamples & VK_SAMPLE_COUNT_1_BIT,
              VK_SAMPLE_COUNT_1_BIT);
    EXPECT_GE(fragment_shading_rate_properties.fragmentShadingRateWithShaderDepthStencilWrites, VK_TRUE);
    EXPECT_GE(fragment_shading_rate_properties.fragmentShadingRateWithSampleMask, VK_TRUE);
    EXPECT_GE(fragment_shading_rate_properties.fragmentShadingRateWithShaderSampleMask, VK_TRUE);
    EXPECT_GE(fragment_shading_rate_properties.fragmentShadingRateWithConservativeRasterization, VK_TRUE);
    EXPECT_GE(fragment_shading_rate_properties.fragmentShadingRateWithFragmentShaderInterlock, VK_TRUE);
    EXPECT_GE(fragment_shading_rate_properties.fragmentShadingRateWithCustomSampleLocations, VK_TRUE);
    EXPECT_GE(fragment_shading_rate_properties.fragmentShadingRateStrictMultiplyCombiner, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestShaderCoreProperties) {
#ifdef VK_AMD_shader_core_properties
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceShaderCorePropertiesAMD shader_core_properties{};
    shader_core_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_AMD;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &shader_core_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(shader_core_properties.shaderEngineCount, 297u);
    EXPECT_GE(shader_core_properties.shaderArraysPerEngineCount, 298u);
    EXPECT_GE(shader_core_properties.computeUnitsPerShaderArray, 2300u);
    EXPECT_GE(shader_core_properties.simdPerComputeUnit, 301u);
    EXPECT_GE(shader_core_properties.wavefrontsPerSimd, 302u);
    EXPECT_GE(shader_core_properties.wavefrontSize, 303u);
    EXPECT_GE(shader_core_properties.sgprsPerSimd, 304u);
    EXPECT_GE(shader_core_properties.minSgprAllocation, 305u);
    EXPECT_GE(shader_core_properties.maxSgprAllocation, 306u);
    EXPECT_GE(shader_core_properties.sgprAllocationGranularity, 307u);
    EXPECT_GE(shader_core_properties.vgprsPerSimd, 308u);
    EXPECT_GE(shader_core_properties.minVgprAllocation, 309u);
    EXPECT_GE(shader_core_properties.maxVgprAllocation, 310u);
    EXPECT_GE(shader_core_properties.vgprAllocationGranularity, 311u);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestShaderCoreProperties2) {
#ifdef VK_AMD_shader_core_properties2
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceShaderCoreProperties2AMD shader_core_properties_2{};
    shader_core_properties_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_2_AMD;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &shader_core_properties_2;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(shader_core_properties_2.shaderCoreFeatures, 0u);
    EXPECT_GE(shader_core_properties_2.activeComputeUnitCount, 341u);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestSubpassShadingProperties) {
#ifdef VK_HUAWEI_subpass_shading
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceSubpassShadingPropertiesHUAWEI subpass_shading_properties{};
    subpass_shading_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_SHADING_PROPERTIES_HUAWEI;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &subpass_shading_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(subpass_shading_properties.maxSubpassShadingWorkgroupSizeAspectRatio, 359u);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestCooperativeMatrixProperties) {
#ifdef VK_NV_cooperative_matrix
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceCooperativeMatrixPropertiesNV cooperative_matrix_properties{};
    cooperative_matrix_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &cooperative_matrix_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(cooperative_matrix_properties.cooperativeMatrixSupportedStages & VK_SHADER_STAGE_VERTEX_BIT,
              VK_SHADER_STAGE_VERTEX_BIT);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestDeviceGeneratedCommandProperties) {
#ifdef VK_NV_device_generated_commands
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceDeviceGeneratedCommandsPropertiesNV device_generated_commands_properties{};
    device_generated_commands_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &device_generated_commands_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(device_generated_commands_properties.maxGraphicsShaderGroupCount, 343u);
    EXPECT_GE(device_generated_commands_properties.maxIndirectSequenceCount, 344u);
    EXPECT_GE(device_generated_commands_properties.maxIndirectCommandsTokenCount, 345u);
    EXPECT_GE(device_generated_commands_properties.maxIndirectCommandsStreamCount, 346u);
    EXPECT_GE(device_generated_commands_properties.maxIndirectCommandsTokenOffset, 347u);
    EXPECT_GE(device_generated_commands_properties.maxIndirectCommandsStreamStride, 348u);
    EXPECT_GE(device_generated_commands_properties.minSequencesCountBufferOffsetAlignment, 349u);
    EXPECT_GE(device_generated_commands_properties.minSequencesIndexBufferOffsetAlignment, 350u);
    EXPECT_GE(device_generated_commands_properties.minIndirectCommandsBufferOffsetAlignment, 351u);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestFragmentShadingRateEnumsProperties) {
#ifdef VK_NV_fragment_shading_rate_enums
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceFragmentShadingRateEnumsPropertiesNV fragment_shading_rate_enums_properties{};
    fragment_shading_rate_enums_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_ENUMS_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &fragment_shading_rate_enums_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(fragment_shading_rate_enums_properties.maxFragmentShadingRateInvocationCount &
                  (VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT),
              VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_4_BIT);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestMeshShaderProperties) {
#ifdef VK_NV_mesh_shader
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceMeshShaderPropertiesNV mesh_shader_properties{};
    mesh_shader_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &mesh_shader_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(mesh_shader_properties.maxDrawMeshTasksCount, 313u);
    EXPECT_GE(mesh_shader_properties.maxTaskWorkGroupInvocations, 314u);
    EXPECT_GE(mesh_shader_properties.maxTaskWorkGroupSize[0], 315u);
    EXPECT_GE(mesh_shader_properties.maxTaskWorkGroupSize[1], 316u);
    EXPECT_GE(mesh_shader_properties.maxTaskWorkGroupSize[2], 317u);
    EXPECT_GE(mesh_shader_properties.maxTaskTotalMemorySize, 318u);
    EXPECT_GE(mesh_shader_properties.maxTaskOutputCount, 319u);
    EXPECT_GE(mesh_shader_properties.maxMeshWorkGroupInvocations, 320u);
    EXPECT_GE(mesh_shader_properties.maxMeshWorkGroupSize[0], 321u);
    EXPECT_GE(mesh_shader_properties.maxMeshWorkGroupSize[1], 322u);
    EXPECT_GE(mesh_shader_properties.maxMeshWorkGroupSize[2], 323u);
    EXPECT_GE(mesh_shader_properties.maxMeshTotalMemorySize, 324u);
    EXPECT_GE(mesh_shader_properties.maxMeshOutputVertices, 325u);
    EXPECT_GE(mesh_shader_properties.maxMeshOutputPrimitives, 326u);
    EXPECT_GE(mesh_shader_properties.maxMeshMultiviewViewCount, 327u);
    EXPECT_GE(mesh_shader_properties.meshOutputPerVertexGranularity, 328u);
    EXPECT_GE(mesh_shader_properties.meshOutputPerPrimitiveGranularity, 329u);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestRayTracingProperties) {
#ifdef VK_NV_ray_tracing
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceRayTracingPropertiesNV ray_tracing_properties{};
    ray_tracing_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &ray_tracing_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(ray_tracing_properties.shaderGroupHandleSize, 288u);
    EXPECT_GE(ray_tracing_properties.maxRecursionDepth, 289u);
    EXPECT_GE(ray_tracing_properties.maxShaderGroupStride, 290u);
    EXPECT_GE(ray_tracing_properties.shaderGroupBaseAlignment, 291u);
    EXPECT_GE(ray_tracing_properties.maxGeometryCount, 292u);
    EXPECT_GE(ray_tracing_properties.maxInstanceCount, 293u);
    EXPECT_GE(ray_tracing_properties.maxTriangleCount, 294u);
    EXPECT_GE(ray_tracing_properties.maxDescriptorSetAccelerationStructures, 295u);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestShaderSMBuiltinsProperties) {
#ifdef VK_NV_shader_sm_builtins
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceShaderSMBuiltinsPropertiesNV shader_sm_builtins_properties{};
    shader_sm_builtins_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &shader_sm_builtins_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(shader_sm_builtins_properties.shaderSMCount, 282u);
    EXPECT_GE(shader_sm_builtins_properties.shaderWarpsPerSM, 283u);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestShadingRateImageProperties) {
#ifdef VK_NV_shading_rate_image
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceShadingRateImagePropertiesNV shading_rate_image_properties{};
    shading_rate_image_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &shading_rate_image_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(shading_rate_image_properties.shadingRateTexelSize.width, 284u);
    EXPECT_GE(shading_rate_image_properties.shadingRateTexelSize.height, 285u);
    EXPECT_GE(shading_rate_image_properties.shadingRatePaletteSize, 286u);
    EXPECT_GE(shading_rate_image_properties.shadingRateMaxCoarseSamples, 287u);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestFragmentDensityMapOffsetProperties) {
#ifdef VK_QCOM_fragment_density_map_offset
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceFragmentDensityMapOffsetPropertiesQCOM fragment_density_map_offset_properties{};
    fragment_density_map_offset_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_OFFSET_PROPERTIES_QCOM;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &fragment_density_map_offset_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_GE(fragment_density_map_offset_properties.fragmentDensityOffsetGranularity.width, 7u);
    EXPECT_GE(fragment_density_map_offset_properties.fragmentDensityOffsetGranularity.height, 6u);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestPhysicalDeviceFeatures) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceFeatures2 features{};
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(features.features.robustBufferAccess, VK_TRUE);
    EXPECT_EQ(features.features.fullDrawIndexUint32, VK_TRUE);
    EXPECT_EQ(features.features.imageCubeArray, VK_TRUE);
    EXPECT_EQ(features.features.independentBlend, VK_TRUE);
    EXPECT_EQ(features.features.geometryShader, VK_TRUE);
    EXPECT_EQ(features.features.tessellationShader, VK_TRUE);
    EXPECT_EQ(features.features.sampleRateShading, VK_TRUE);
    EXPECT_EQ(features.features.dualSrcBlend, VK_TRUE);
    EXPECT_EQ(features.features.logicOp, VK_TRUE);
    EXPECT_EQ(features.features.multiDrawIndirect, VK_TRUE);
    EXPECT_EQ(features.features.drawIndirectFirstInstance, VK_TRUE);
    EXPECT_EQ(features.features.depthClamp, VK_TRUE);
    EXPECT_EQ(features.features.depthBiasClamp, VK_TRUE);
    EXPECT_EQ(features.features.fillModeNonSolid, VK_TRUE);
    EXPECT_EQ(features.features.depthBounds, VK_TRUE);
    EXPECT_EQ(features.features.wideLines, VK_TRUE);
    EXPECT_EQ(features.features.largePoints, VK_TRUE);
    EXPECT_EQ(features.features.alphaToOne, VK_TRUE);
    EXPECT_EQ(features.features.multiViewport, VK_TRUE);
    EXPECT_EQ(features.features.samplerAnisotropy, VK_TRUE);
    EXPECT_EQ(features.features.textureCompressionETC2, VK_TRUE);
    EXPECT_EQ(features.features.textureCompressionASTC_LDR, VK_TRUE);
    EXPECT_EQ(features.features.textureCompressionBC, VK_TRUE);
    EXPECT_EQ(features.features.occlusionQueryPrecise, VK_TRUE);
    EXPECT_EQ(features.features.pipelineStatisticsQuery, VK_TRUE);
    EXPECT_EQ(features.features.vertexPipelineStoresAndAtomics, VK_TRUE);
    EXPECT_EQ(features.features.fragmentStoresAndAtomics, VK_TRUE);
    EXPECT_EQ(features.features.shaderTessellationAndGeometryPointSize, VK_TRUE);
    EXPECT_EQ(features.features.shaderImageGatherExtended, VK_TRUE);
    EXPECT_EQ(features.features.shaderStorageImageExtendedFormats, VK_TRUE);
    EXPECT_EQ(features.features.shaderStorageImageMultisample, VK_TRUE);
    EXPECT_EQ(features.features.shaderStorageImageReadWithoutFormat, VK_TRUE);
    EXPECT_EQ(features.features.shaderStorageImageWriteWithoutFormat, VK_TRUE);
    EXPECT_EQ(features.features.shaderUniformBufferArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(features.features.shaderSampledImageArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(features.features.shaderStorageBufferArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(features.features.shaderStorageImageArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(features.features.shaderClipDistance, VK_TRUE);
    EXPECT_EQ(features.features.shaderCullDistance, VK_TRUE);
    EXPECT_EQ(features.features.shaderFloat64, VK_TRUE);
    EXPECT_EQ(features.features.shaderInt64, VK_TRUE);
    EXPECT_EQ(features.features.shaderInt16, VK_TRUE);
    EXPECT_EQ(features.features.shaderResourceResidency, VK_TRUE);
    EXPECT_EQ(features.features.shaderResourceMinLod, VK_TRUE);
    EXPECT_EQ(features.features.sparseBinding, VK_TRUE);
    EXPECT_EQ(features.features.sparseResidencyBuffer, VK_TRUE);
    EXPECT_EQ(features.features.sparseResidencyImage2D, VK_TRUE);
    EXPECT_EQ(features.features.sparseResidencyImage3D, VK_TRUE);
    EXPECT_EQ(features.features.sparseResidency2Samples, VK_TRUE);
    EXPECT_EQ(features.features.sparseResidency4Samples, VK_TRUE);
    EXPECT_EQ(features.features.sparseResidency8Samples, VK_TRUE);
    EXPECT_EQ(features.features.sparseResidency16Samples, VK_TRUE);
    EXPECT_EQ(features.features.sparseResidencyAliased, VK_TRUE);
    EXPECT_EQ(features.features.variableMultisampleRate, VK_TRUE);
    EXPECT_EQ(features.features.inheritedQueries, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestHostQueryResetFeatures) {
#ifdef VK_EXT_host_query_reset
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceHostQueryResetFeaturesEXT host_query_reset_features{};
    host_query_reset_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &host_query_reset_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(host_query_reset_features.hostQueryReset, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestMaintenance4Features) {
#ifdef VK_KHR_maintenance4
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceMaintenance4FeaturesKHR maintenance_4_features{};
    maintenance_4_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &maintenance_4_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(maintenance_4_features.maintenance4, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, Test16BitStorageFeatures) {
#ifdef VK_KHR_16bit_storage
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice16BitStorageFeaturesKHR f_16_bit_storage_features{};
    f_16_bit_storage_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &f_16_bit_storage_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(f_16_bit_storage_features.storageBuffer16BitAccess, VK_TRUE);
    EXPECT_EQ(f_16_bit_storage_features.uniformAndStorageBuffer16BitAccess, VK_TRUE);
    EXPECT_EQ(f_16_bit_storage_features.storagePushConstant16, VK_TRUE);
    EXPECT_EQ(f_16_bit_storage_features.storageInputOutput16, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, Test8BitStorageFeatures) {
#ifdef VK_KHR_8bit_storage
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice8BitStorageFeaturesKHR f_8_bit_storage_features{};
    f_8_bit_storage_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &f_8_bit_storage_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(f_8_bit_storage_features.storageBuffer8BitAccess, VK_TRUE);
    EXPECT_EQ(f_8_bit_storage_features.uniformAndStorageBuffer8BitAccess, VK_TRUE);
    EXPECT_EQ(f_8_bit_storage_features.storagePushConstant8, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestBufferDeviceAddressFeatures) {
#ifdef VK_KHR_buffer_device_address
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceBufferDeviceAddressFeaturesKHR buffer_device_address_features{};
    buffer_device_address_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &buffer_device_address_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(buffer_device_address_features.bufferDeviceAddress, VK_TRUE);
    EXPECT_EQ(buffer_device_address_features.bufferDeviceAddressCaptureReplay, VK_TRUE);
    EXPECT_EQ(buffer_device_address_features.bufferDeviceAddressMultiDevice, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestDescriptorIndexingFeatures) {
#ifdef VK_EXT_descriptor_indexing
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceDescriptorIndexingFeaturesEXT descriptor_indexing_features{};
    descriptor_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &descriptor_indexing_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(descriptor_indexing_features.shaderInputAttachmentArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.shaderUniformTexelBufferArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.shaderStorageTexelBufferArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.shaderUniformBufferArrayNonUniformIndexing, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.shaderSampledImageArrayNonUniformIndexing, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.shaderStorageBufferArrayNonUniformIndexing, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.shaderStorageImageArrayNonUniformIndexing, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.shaderInputAttachmentArrayNonUniformIndexing, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.shaderUniformTexelBufferArrayNonUniformIndexing, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.shaderStorageTexelBufferArrayNonUniformIndexing, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.descriptorBindingUniformBufferUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.descriptorBindingSampledImageUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.descriptorBindingStorageImageUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.descriptorBindingStorageBufferUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.descriptorBindingUniformTexelBufferUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.descriptorBindingStorageTexelBufferUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.descriptorBindingUpdateUnusedWhilePending, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.descriptorBindingPartiallyBound, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.descriptorBindingVariableDescriptorCount, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_features.runtimeDescriptorArray, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestImagelessFramebufferFeatures) {
#ifdef VK_KHR_imageless_framebuffer
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceImagelessFramebufferFeaturesKHR imageless_framebuffer_features{};
    imageless_framebuffer_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &imageless_framebuffer_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(imageless_framebuffer_features.imagelessFramebuffer, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestMultiviewFeatures) {
#ifdef VK_KHR_multiview
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceMultiviewFeaturesKHR multiview_features{};
    multiview_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &multiview_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(multiview_features.multiview, VK_TRUE);
    EXPECT_EQ(multiview_features.multiviewGeometryShader, VK_TRUE);
    EXPECT_EQ(multiview_features.multiviewTessellationShader, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestProtectedMemoryFeatures) {
#ifdef VK_VERSION_1_1
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceProtectedMemoryFeatures protected_memory_features{};
    protected_memory_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &protected_memory_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(protected_memory_features.protectedMemory, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestSamplerYcbcrConversionFeatures) {
#ifdef VK_KHR_sampler_ycbcr_conversion
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR sampler_ycbcr_conversion_features{};
    sampler_ycbcr_conversion_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &sampler_ycbcr_conversion_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(sampler_ycbcr_conversion_features.samplerYcbcrConversion, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestScalarBlockLayoutFeatures) {
#ifdef VK_EXT_scalar_block_layout
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceScalarBlockLayoutFeaturesEXT scalar_block_layout_features{};
    scalar_block_layout_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &scalar_block_layout_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(scalar_block_layout_features.scalarBlockLayout, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestSeparateDepthStencilLayoutsFeatures) {
#ifdef VK_KHR_separate_depth_stencil_layouts
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR separate_depth_stencil_layout_featuers{};
    separate_depth_stencil_layout_featuers.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &separate_depth_stencil_layout_featuers;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(separate_depth_stencil_layout_featuers.separateDepthStencilLayouts, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestShaderAtomicInt64Features) {
#ifdef VK_KHR_shader_atomic_int64
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceShaderAtomicInt64FeaturesKHR shader_atomic_int64_features{};
    shader_atomic_int64_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &shader_atomic_int64_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(shader_atomic_int64_features.shaderBufferInt64Atomics, VK_TRUE);
    EXPECT_EQ(shader_atomic_int64_features.shaderSharedInt64Atomics, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestShaderDrawParametersFeatures) {
#ifdef VK_VERSION_1_1
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceShaderDrawParametersFeatures shader_draw_paramenters_features{};
    shader_draw_paramenters_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &shader_draw_paramenters_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(shader_draw_paramenters_features.shaderDrawParameters, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestShaderFloat16Int8Features) {
#ifdef VK_KHR_shader_float16_int8
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceShaderFloat16Int8FeaturesKHR shader_float16_int8_features{};
    shader_float16_int8_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &shader_float16_int8_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(shader_float16_int8_features.shaderFloat16, VK_TRUE);
    EXPECT_EQ(shader_float16_int8_features.shaderInt8, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestShaderSubgroupExtendedTypesFeatures) {
#ifdef VK_KHR_shader_subgroup_extended_types
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR shader_subgroup_extended_types_features{};
    shader_subgroup_extended_types_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &shader_subgroup_extended_types_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(shader_subgroup_extended_types_features.shaderSubgroupExtendedTypes, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestTimelineSemaphoreFeatures) {
#ifdef VK_KHR_timeline_semaphore
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceTimelineSemaphoreFeaturesKHR timeline_semaphore_features{};
    timeline_semaphore_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &timeline_semaphore_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(timeline_semaphore_features.timelineSemaphore, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestUniformBufferStandardLayoutFeatures) {
#ifdef VK_KHR_uniform_buffer_standard_layout
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR uniform_buffer_standard_layout_features{};
    uniform_buffer_standard_layout_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &uniform_buffer_standard_layout_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(uniform_buffer_standard_layout_features.uniformBufferStandardLayout, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestVariablePointersFeatures) {
#ifdef VK_KHR_variable_pointers
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceVariablePointersFeaturesKHR variable_pointer_features{};
    variable_pointer_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &variable_pointer_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(variable_pointer_features.variablePointersStorageBuffer, VK_TRUE);
    EXPECT_EQ(variable_pointer_features.variablePointers, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestVulkanMemoryModelFeatures) {
#ifdef VK_KHR_vulkan_memory_model
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceVulkanMemoryModelFeaturesKHR vulkan_memory_model_features{};
    vulkan_memory_model_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &vulkan_memory_model_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(vulkan_memory_model_features.vulkanMemoryModel, VK_TRUE);
    EXPECT_EQ(vulkan_memory_model_features.vulkanMemoryModelDeviceScope, VK_TRUE);
    EXPECT_EQ(vulkan_memory_model_features.vulkanMemoryModelAvailabilityVisibilityChains, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestZeroInitializeWorkgroupMemoryFeatures) {
#ifdef VK_KHR_zero_initialize_workgroup_memory
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR zero_initialize_workgroup_memory_features{};
    zero_initialize_workgroup_memory_features.sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_WORKGROUP_MEMORY_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &zero_initialize_workgroup_memory_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(zero_initialize_workgroup_memory_features.shaderZeroInitializeWorkgroupMemory, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestAccelerationStructureFeatures) {
#ifdef VK_KHR_acceleration_structure
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceAccelerationStructureFeaturesKHR acceleration_structure_features{};
    acceleration_structure_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &acceleration_structure_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(acceleration_structure_features.accelerationStructure, VK_TRUE);
    EXPECT_EQ(acceleration_structure_features.accelerationStructureCaptureReplay, VK_TRUE);
    EXPECT_EQ(acceleration_structure_features.accelerationStructureIndirectBuild, VK_TRUE);
    EXPECT_EQ(acceleration_structure_features.accelerationStructureHostCommands, VK_TRUE);
    EXPECT_EQ(acceleration_structure_features.descriptorBindingAccelerationStructureUpdateAfterBind, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestPerformanceQueryFeatures) {
#ifdef VK_KHR_performance_query
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevicePerformanceQueryFeaturesKHR performance_query_features{};
    performance_query_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &performance_query_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(performance_query_features.performanceCounterQueryPools, VK_TRUE);
    EXPECT_EQ(performance_query_features.performanceCounterMultipleQueryPools, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestPipelineExecutablePropertiesFeatures) {
#ifdef VK_KHR_pipeline_executable_properties
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR pipeline_executable_properties_features{};
    pipeline_executable_properties_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_EXECUTABLE_PROPERTIES_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &pipeline_executable_properties_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(pipeline_executable_properties_features.pipelineExecutableInfo, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestPresentIdFeatures) {
#ifdef VK_KHR_present_id
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevicePresentIdFeaturesKHR present_id_features{};
    present_id_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_ID_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &present_id_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(present_id_features.presentId, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestPresentWaitFeatures) {
#ifdef VK_KHR_present_wait
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevicePresentWaitFeaturesKHR present_wait_features{};
    present_wait_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_WAIT_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &present_wait_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(present_wait_features.presentWait, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestRayQueryFeatures) {
#ifdef VK_KHR_ray_query
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceRayQueryFeaturesKHR ray_query_features{};
    ray_query_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &ray_query_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(ray_query_features.rayQuery, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestRayTracingPipelineFeatures) {
#ifdef VK_TEST
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceRayTracingPipelineFeaturesKHR ray_tracing_pipeline_features{};
    ray_tracing_pipeline_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &ray_tracing_pipeline_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(ray_tracing_pipeline_features.rayTracingPipeline, VK_TRUE);
    EXPECT_EQ(ray_tracing_pipeline_features.rayTracingPipelineShaderGroupHandleCaptureReplay, VK_TRUE);
    EXPECT_EQ(ray_tracing_pipeline_features.rayTracingPipelineShaderGroupHandleCaptureReplayMixed, VK_TRUE);
    EXPECT_EQ(ray_tracing_pipeline_features.rayTracingPipelineTraceRaysIndirect, VK_TRUE);
    EXPECT_EQ(ray_tracing_pipeline_features.rayTraversalPrimitiveCulling, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestShaderClockFeatures) {
#ifdef VK_KHR_ray_tracing_pipeline
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceShaderClockFeaturesKHR shader_clock_features{};
    shader_clock_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &shader_clock_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(shader_clock_features.shaderSubgroupClock, VK_TRUE);
    EXPECT_EQ(shader_clock_features.shaderDeviceClock, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestShaderIntegerDotProductFeatures) {
#ifdef VK_KHR_shader_integer_dot_product
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR shader_integer_dot_product_features{};
    shader_integer_dot_product_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &shader_integer_dot_product_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(shader_integer_dot_product_features.shaderIntegerDotProduct, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestShaderSubgroupUniformControlFlowFeatures) {
#ifdef VK_KHR_shader_subgroup_uniform_control_flow
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR shader_subgroup_uniform_control_flow_features{};
    shader_subgroup_uniform_control_flow_features.sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &shader_subgroup_uniform_control_flow_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(shader_subgroup_uniform_control_flow_features.shaderSubgroupUniformControlFlow, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestShaderTerminateInvocationFeatures) {
#ifdef VK_KHR_shader_terminate_invocation
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR shader_terminate_invocation_features{};
    shader_terminate_invocation_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &shader_terminate_invocation_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(shader_terminate_invocation_features.shaderTerminateInvocation, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestSynchronization2Features) {
#ifdef VK_KHR_synchronization2
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceSynchronization2FeaturesKHR synchronization_2_features{};
    synchronization_2_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &synchronization_2_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(synchronization_2_features.synchronization2, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestWorkgroupMemoryExplicitLayoutFeatures) {
#ifdef VK_KHR_workgroup_memory_explicit_layout
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR workgroup_memory_explicit_layout_features{};
    workgroup_memory_explicit_layout_features.sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &workgroup_memory_explicit_layout_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(workgroup_memory_explicit_layout_features.workgroupMemoryExplicitLayout, VK_TRUE);
    EXPECT_EQ(workgroup_memory_explicit_layout_features.workgroupMemoryExplicitLayoutScalarBlockLayout, VK_TRUE);
    EXPECT_EQ(workgroup_memory_explicit_layout_features.workgroupMemoryExplicitLayout8BitAccess, VK_TRUE);
    EXPECT_EQ(workgroup_memory_explicit_layout_features.workgroupMemoryExplicitLayout16BitAccess, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, Test4444FormatsFeatures) {
#ifdef VK_EXT_4444_formats
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice4444FormatsFeaturesEXT f_4444_formats_features{};
    f_4444_formats_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_4444_FORMATS_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &f_4444_formats_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(f_4444_formats_features.formatA4R4G4B4, VK_TRUE);
    EXPECT_EQ(f_4444_formats_features.formatA4B4G4R4, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestASTCDecodeFeatures) {
#ifdef VK_EXT_astc_decode_mode
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceASTCDecodeFeaturesEXT astc_decode_features{};
    astc_decode_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ASTC_DECODE_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &astc_decode_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(astc_decode_features.decodeModeSharedExponent, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestBlendOperationAdvancedFeatures) {
#ifdef VK_EXT_blend_operation_advanced
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT blend_operation_advanced_features{};
    blend_operation_advanced_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &blend_operation_advanced_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(blend_operation_advanced_features.advancedBlendCoherentOperations, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestBorderColorSwizzleFeatures) {
#ifdef VK_EXT_border_color_swizzle
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceBorderColorSwizzleFeaturesEXT border_color_swizzle_features{};
    border_color_swizzle_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BORDER_COLOR_SWIZZLE_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &border_color_swizzle_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(border_color_swizzle_features.borderColorSwizzle, VK_TRUE);
    EXPECT_EQ(border_color_swizzle_features.borderColorSwizzleFromImage, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestColorWriteEnableFeatures) {
#ifdef VK_EXT_color_write_enable
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceColorWriteEnableFeaturesEXT color_write_enable_features{};
    color_write_enable_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COLOR_WRITE_ENABLE_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &color_write_enable_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(color_write_enable_features.colorWriteEnable, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestConditionalRenderingFeatures) {
#ifdef VK_EXT_conditional_rendering
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceConditionalRenderingFeaturesEXT conditional_rendering_features{};
    conditional_rendering_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONDITIONAL_RENDERING_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &conditional_rendering_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(conditional_rendering_features.conditionalRendering, VK_TRUE);
    EXPECT_EQ(conditional_rendering_features.inheritedConditionalRendering, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestCustomBorderColorFeatures) {
#ifdef VK_EXT_custom_border_color
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceCustomBorderColorFeaturesEXT custom_border_color_features{};
    custom_border_color_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &custom_border_color_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(custom_border_color_features.customBorderColors, VK_TRUE);
    EXPECT_EQ(custom_border_color_features.customBorderColorWithoutFormat, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestDepthClipEnableFeatures) {
#ifdef VK_EXT_depth_clip_enable
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceDepthClipEnableFeaturesEXT depth_clip_enable_features{};
    depth_clip_enable_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &depth_clip_enable_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(depth_clip_enable_features.depthClipEnable, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestDeviceMemoryReportFeatures) {
#ifdef VK_EXT_device_memory_report
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceDeviceMemoryReportFeaturesEXT device_memory_report_features{};
    device_memory_report_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_MEMORY_REPORT_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &device_memory_report_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(device_memory_report_features.deviceMemoryReport, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestExtendedDynamicStateFeatures) {
#ifdef VK_EXT_extended_dynamic_state
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT extended_dynamic_state_features{};
    extended_dynamic_state_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &extended_dynamic_state_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(extended_dynamic_state_features.extendedDynamicState, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestExtendedDynamicState2Features) {
#ifdef VK_EXT_extended_dynamic_state2
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceExtendedDynamicState2FeaturesEXT extended_dynamic_state_2_features{};
    extended_dynamic_state_2_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &extended_dynamic_state_2_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(extended_dynamic_state_2_features.extendedDynamicState2, VK_TRUE);
    EXPECT_EQ(extended_dynamic_state_2_features.extendedDynamicState2LogicOp, VK_TRUE);
    EXPECT_EQ(extended_dynamic_state_2_features.extendedDynamicState2PatchControlPoints, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestFragmentDensityMapFeatures) {
#ifdef VK_EXT_fragment_density_map
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceFragmentDensityMapFeaturesEXT fragment_density_map_features{};
    fragment_density_map_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &fragment_density_map_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(fragment_density_map_features.fragmentDensityMap, VK_TRUE);
    EXPECT_EQ(fragment_density_map_features.fragmentDensityMapDynamic, VK_TRUE);
    EXPECT_EQ(fragment_density_map_features.fragmentDensityMapNonSubsampledImages, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestFragmentShaderInterlockFeatures) {
#ifdef VK_EXT_fragment_shader_interlock
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT fragment_shader_interlock_features{};
    fragment_shader_interlock_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &fragment_shader_interlock_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(fragment_shader_interlock_features.fragmentShaderSampleInterlock, VK_TRUE);
    EXPECT_EQ(fragment_shader_interlock_features.fragmentShaderPixelInterlock, VK_TRUE);
    EXPECT_EQ(fragment_shader_interlock_features.fragmentShaderShadingRateInterlock, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestGlobalPriorityQueryFeatures) {
#ifdef VK_EXT_global_priority_query
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT global_priority_query_features{};
    global_priority_query_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GLOBAL_PRIORITY_QUERY_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &global_priority_query_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(global_priority_query_features.globalPriorityQuery, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestImageRobustnessFeatures) {
#ifdef VK_EXT_image_robustness
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceImageRobustnessFeaturesEXT image_robustness_features{};
    image_robustness_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &image_robustness_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(image_robustness_features.robustImageAccess, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestIndexTypeUint8Features) {
#ifdef VK_EXT_index_type_uint8
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceIndexTypeUint8FeaturesEXT index_type_uint8_features{};
    index_type_uint8_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &index_type_uint8_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(index_type_uint8_features.indexTypeUint8, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestInlineUniformBlockFeatures) {
#ifdef VK_EXT_inline_uniform_block
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceInlineUniformBlockFeaturesEXT inline_uniform_block_features{};
    inline_uniform_block_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &inline_uniform_block_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(inline_uniform_block_features.inlineUniformBlock, VK_TRUE);
    EXPECT_EQ(inline_uniform_block_features.descriptorBindingInlineUniformBlockUpdateAfterBind, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestLineRasterizationFeatures) {
#ifdef VK_EXT_line_rasterization
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceLineRasterizationFeaturesEXT line_rasterization_features{};
    line_rasterization_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &line_rasterization_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(line_rasterization_features.rectangularLines, VK_TRUE);
    EXPECT_EQ(line_rasterization_features.bresenhamLines, VK_TRUE);
    EXPECT_EQ(line_rasterization_features.smoothLines, VK_TRUE);
    EXPECT_EQ(line_rasterization_features.stippledRectangularLines, VK_TRUE);
    EXPECT_EQ(line_rasterization_features.stippledBresenhamLines, VK_TRUE);
    EXPECT_EQ(line_rasterization_features.stippledSmoothLines, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestMemoryPriorityFeatures) {
#ifdef VK_EXT_memory_priority
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceMemoryPriorityFeaturesEXT memory_priority_features{};
    memory_priority_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PRIORITY_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &memory_priority_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(memory_priority_features.memoryPriority, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestMultiDrawFeatures) {
#ifdef VK_EXT_multi_draw
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceMultiDrawFeaturesEXT multi_draw_features{};
    multi_draw_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &multi_draw_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(multi_draw_features.multiDraw, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestPageableDeviceLocalMemoryFeatures) {
#ifdef VK_EXT_pageable_device_local_memory
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT pageable_device_local_memory_features{};
    pageable_device_local_memory_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PAGEABLE_DEVICE_LOCAL_MEMORY_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &pageable_device_local_memory_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(pageable_device_local_memory_features.pageableDeviceLocalMemory, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestPipelineCreationCacheControlFeatures) {
#ifdef VK_EXT_pipeline_creation_cache_control
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT pipeline_creation_cache_control_features{};
    pipeline_creation_cache_control_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &pipeline_creation_cache_control_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(pipeline_creation_cache_control_features.pipelineCreationCacheControl, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestPrimitiveTopologyListRestartFeatures) {
#ifdef VK_EXT_primitive_topology_list_restart
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT primitive_topology_list_restart_features{};
    primitive_topology_list_restart_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVE_TOPOLOGY_LIST_RESTART_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &primitive_topology_list_restart_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(primitive_topology_list_restart_features.primitiveTopologyListRestart, VK_TRUE);
    EXPECT_EQ(primitive_topology_list_restart_features.primitiveTopologyPatchListRestart, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestPrivateDataFeatures) {
#ifdef VK_EXT_private_data
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevicePrivateDataFeaturesEXT private_data_features{};
    private_data_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &private_data_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(private_data_features.privateData, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestProvokingVertexFeatures) {
#ifdef VK_EXT_provoking_vertex
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceProvokingVertexFeaturesEXT provoking_vertex_features{};
    provoking_vertex_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &provoking_vertex_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(provoking_vertex_features.provokingVertexLast, VK_TRUE);
    EXPECT_EQ(provoking_vertex_features.transformFeedbackPreservesProvokingVertex, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestRGBA10X6FormatsFeatures) {
#ifdef VK_EXT_rgba10x6_formats
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT rgba10x6_formats_features{};
    rgba10x6_formats_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RGBA10X6_FORMATS_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &rgba10x6_formats_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(rgba10x6_formats_features.formatRgba10x6WithoutYCbCrSampler, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestRobustness2Features) {
#ifdef VK_EXT_robustness2
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceRobustness2FeaturesEXT robustness_2_features{};
    robustness_2_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &robustness_2_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(robustness_2_features.robustBufferAccess2, VK_TRUE);
    EXPECT_EQ(robustness_2_features.robustImageAccess2, VK_TRUE);
    EXPECT_EQ(robustness_2_features.nullDescriptor, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestShaderAtomicFloatFeatures) {
#ifdef VK_EXT_shader_atomic_float
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceShaderAtomicFloatFeaturesEXT shader_atomic_float_features{};
    shader_atomic_float_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &shader_atomic_float_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(shader_atomic_float_features.shaderBufferFloat32Atomics, VK_TRUE);
    EXPECT_EQ(shader_atomic_float_features.shaderBufferFloat32AtomicAdd, VK_TRUE);
    EXPECT_EQ(shader_atomic_float_features.shaderBufferFloat64Atomics, VK_TRUE);
    EXPECT_EQ(shader_atomic_float_features.shaderBufferFloat64AtomicAdd, VK_TRUE);
    EXPECT_EQ(shader_atomic_float_features.shaderSharedFloat32Atomics, VK_TRUE);
    EXPECT_EQ(shader_atomic_float_features.shaderSharedFloat32AtomicAdd, VK_TRUE);
    EXPECT_EQ(shader_atomic_float_features.shaderSharedFloat64Atomics, VK_TRUE);
    EXPECT_EQ(shader_atomic_float_features.shaderSharedFloat64AtomicAdd, VK_TRUE);
    EXPECT_EQ(shader_atomic_float_features.shaderImageFloat32Atomics, VK_TRUE);
    EXPECT_EQ(shader_atomic_float_features.shaderImageFloat32AtomicAdd, VK_TRUE);
    EXPECT_EQ(shader_atomic_float_features.sparseImageFloat32Atomics, VK_TRUE);
    EXPECT_EQ(shader_atomic_float_features.sparseImageFloat32AtomicAdd, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestShaderAtomicFloat2Features) {
#ifdef VK_EXT_shader_atomic_float2
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT shader_atomic_float_2_features{};
    shader_atomic_float_2_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_2_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &shader_atomic_float_2_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(shader_atomic_float_2_features.shaderBufferFloat16Atomics, VK_TRUE);
    EXPECT_EQ(shader_atomic_float_2_features.shaderBufferFloat16AtomicAdd, VK_TRUE);
    EXPECT_EQ(shader_atomic_float_2_features.shaderBufferFloat16AtomicMinMax, VK_TRUE);
    EXPECT_EQ(shader_atomic_float_2_features.shaderBufferFloat32AtomicMinMax, VK_TRUE);
    EXPECT_EQ(shader_atomic_float_2_features.shaderBufferFloat64AtomicMinMax, VK_TRUE);
    EXPECT_EQ(shader_atomic_float_2_features.shaderSharedFloat16Atomics, VK_TRUE);
    EXPECT_EQ(shader_atomic_float_2_features.shaderSharedFloat16AtomicAdd, VK_TRUE);
    EXPECT_EQ(shader_atomic_float_2_features.shaderSharedFloat16AtomicMinMax, VK_TRUE);
    EXPECT_EQ(shader_atomic_float_2_features.shaderSharedFloat32AtomicMinMax, VK_TRUE);
    EXPECT_EQ(shader_atomic_float_2_features.shaderSharedFloat64AtomicMinMax, VK_TRUE);
    EXPECT_EQ(shader_atomic_float_2_features.shaderImageFloat32AtomicMinMax, VK_TRUE);
    EXPECT_EQ(shader_atomic_float_2_features.sparseImageFloat32AtomicMinMax, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestShaderDemoteToHelperInvocationFeatures) {
#ifdef VK_EXT_shader_demote_to_helper_invocation
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT shader_demote_to_helper_invocation_features{};
    shader_demote_to_helper_invocation_features.sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &shader_demote_to_helper_invocation_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(shader_demote_to_helper_invocation_features.shaderDemoteToHelperInvocation, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestShaderImageAtomicInt64Features) {
#ifdef VK_EXT_shader_image_atomic_int64
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT shader_image_atomic_int_64_features{};
    shader_image_atomic_int_64_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_ATOMIC_INT64_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &shader_image_atomic_int_64_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(shader_image_atomic_int_64_features.shaderImageInt64Atomics, VK_TRUE);
    EXPECT_EQ(shader_image_atomic_int_64_features.sparseImageInt64Atomics, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestSubgroupSizeControlFeatures) {
#ifdef VK_EXT_subgroup_size_control
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceSubgroupSizeControlFeaturesEXT subgroup_size_control_features{};
    subgroup_size_control_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &subgroup_size_control_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(subgroup_size_control_features.subgroupSizeControl, VK_TRUE);
    EXPECT_EQ(subgroup_size_control_features.computeFullSubgroups, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestTexelBufferAlignmentFeatures) {
#ifdef VK_EXT_texel_buffer_alignment
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT texel_buffer_alignment_features{};
    texel_buffer_alignment_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &texel_buffer_alignment_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(texel_buffer_alignment_features.texelBufferAlignment, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestTextureCompressionASTCHDRFeatures) {
#ifdef VK_EXT_texture_compression_astc_hdr
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT texture_compression_astchdr_features{};
    texture_compression_astchdr_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_HDR_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &texture_compression_astchdr_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(texture_compression_astchdr_features.textureCompressionASTC_HDR, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestTransformFeedbackFeatures) {
#ifdef VK_EXT_transform_feedback
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceTransformFeedbackFeaturesEXT transform_feedback_features{};
    transform_feedback_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &transform_feedback_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(transform_feedback_features.transformFeedback, VK_TRUE);
    EXPECT_EQ(transform_feedback_features.geometryStreams, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestVertexAttributeDivisorFeatures) {
#ifdef VK_EXT_vertex_attribute_divisor
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT vertex_attribute_divisor_features{};
    vertex_attribute_divisor_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &vertex_attribute_divisor_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(vertex_attribute_divisor_features.vertexAttributeInstanceRateDivisor, VK_TRUE);
    EXPECT_EQ(vertex_attribute_divisor_features.vertexAttributeInstanceRateZeroDivisor, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestVertexInputDynamicStateFeatures) {
#ifdef VK_EXT_vertex_input_dynamic_state
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT vertex_input_dynamic_state_features{};
    vertex_input_dynamic_state_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_INPUT_DYNAMIC_STATE_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &vertex_input_dynamic_state_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(vertex_input_dynamic_state_features.vertexInputDynamicState, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestYcbcr2Plane444FormatsFeatures) {
#ifdef VK_EXT_ycbcr_2plane_444_formats
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT ycbcr_2_plane_444_formats_features{};
    ycbcr_2_plane_444_formats_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_2_PLANE_444_FORMATS_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &ycbcr_2_plane_444_formats_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(ycbcr_2_plane_444_formats_features.ycbcr2plane444Formats, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestYcbcrImageArraysFeatures) {
#ifdef VK_EXT_ycbcr_image_arrays
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceYcbcrImageArraysFeaturesEXT ycbcr_image_arrays_features{};
    ycbcr_image_arrays_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_IMAGE_ARRAYS_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &ycbcr_image_arrays_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(ycbcr_image_arrays_features.ycbcrImageArrays, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestFragmentShadingRateFeatures) {
#ifdef VK_KHR_fragment_shading_rate
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceFragmentShadingRateFeaturesKHR fragment_shading_rate_features{};
    fragment_shading_rate_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &fragment_shading_rate_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(fragment_shading_rate_features.pipelineFragmentShadingRate, VK_TRUE);
    EXPECT_EQ(fragment_shading_rate_features.primitiveFragmentShadingRate, VK_TRUE);
    EXPECT_EQ(fragment_shading_rate_features.attachmentFragmentShadingRate, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestCoherentMemoryFeatures) {
#ifdef VK_AMD_device_coherent_memory
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceCoherentMemoryFeaturesAMD coherent_memory_features{};
    coherent_memory_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COHERENT_MEMORY_FEATURES_AMD;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &coherent_memory_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(coherent_memory_features.deviceCoherentMemory, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestInvocationMaskFeatures) {
#ifdef VK_HUAWEI_invocation_mask
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceInvocationMaskFeaturesHUAWEI invocation_mask_features{};
    invocation_mask_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INVOCATION_MASK_FEATURES_HUAWEI;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &invocation_mask_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(invocation_mask_features.invocationMask, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestSubpassShadingFeatures) {
#ifdef VK_HUAWEI_subpass_shading
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceSubpassShadingFeaturesHUAWEI subpass_shading_features{};
    subpass_shading_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_SHADING_FEATURES_HUAWEI;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &subpass_shading_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(subpass_shading_features.subpassShading, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestShaderIntegerFunctions2Features) {
#ifdef VK_INTEL_shader_integer_functions2
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL shader_integer_functions_2_features{};
    shader_integer_functions_2_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_FUNCTIONS_2_FEATURES_INTEL;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &shader_integer_functions_2_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(shader_integer_functions_2_features.shaderIntegerFunctions2, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestComputeShaderDerivativesFeatures) {
#ifdef VK_NV_compute_shader_derivatives
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceComputeShaderDerivativesFeaturesNV compute_shader_derivatives_features{};
    compute_shader_derivatives_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMPUTE_SHADER_DERIVATIVES_FEATURES_NV;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &compute_shader_derivatives_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(compute_shader_derivatives_features.computeDerivativeGroupQuads, VK_TRUE);
    EXPECT_EQ(compute_shader_derivatives_features.computeDerivativeGroupLinear, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestCooperativeMatrixFeatures) {
#ifdef VK_NV_cooperative_matrix
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceCooperativeMatrixFeaturesNV cooperative_matrix_features{};
    cooperative_matrix_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_NV;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &cooperative_matrix_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(cooperative_matrix_features.cooperativeMatrix, VK_TRUE);
    EXPECT_EQ(cooperative_matrix_features.cooperativeMatrixRobustBufferAccess, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestCornerSampledImageFeatures) {
#ifdef VK_NV_corner_sampled_image
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceCornerSampledImageFeaturesNV corner_sampled_image_features{};
    corner_sampled_image_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CORNER_SAMPLED_IMAGE_FEATURES_NV;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &corner_sampled_image_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(corner_sampled_image_features.cornerSampledImage, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestCoverageReductionModeFeatures) {
#ifdef VK_NV_coverage_reduction_mode
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceCoverageReductionModeFeaturesNV coverage_reduction_mode_features{};
    coverage_reduction_mode_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COVERAGE_REDUCTION_MODE_FEATURES_NV;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &coverage_reduction_mode_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(coverage_reduction_mode_features.coverageReductionMode, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestDedicatedAllocationImageAliasingFeatures) {
#ifdef VK_NV_dedicated_allocation_image_aliasing
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV dedicated_allocation_image_aliasing_features{};
    dedicated_allocation_image_aliasing_features.sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEDICATED_ALLOCATION_IMAGE_ALIASING_FEATURES_NV;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &dedicated_allocation_image_aliasing_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(dedicated_allocation_image_aliasing_features.dedicatedAllocationImageAliasing, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestDiagnosticsConfigFeatures) {
#ifdef VK_NV_device_diagnostics_config
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceDiagnosticsConfigFeaturesNV diagnostic_config_features{};
    diagnostic_config_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DIAGNOSTICS_CONFIG_FEATURES_NV;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &diagnostic_config_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(diagnostic_config_features.diagnosticsConfig, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestDeviceGeneratedCommandsFeatures) {
#ifdef VK_NV_device_generated_commands
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV device_generated_commmands_features{};
    device_generated_commmands_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DIAGNOSTICS_CONFIG_FEATURES_NV;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &device_generated_commmands_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(device_generated_commmands_features.deviceGeneratedCommands, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestExternalMemoryRDMAFeatures) {
#ifdef VK_NV_external_memory_rdma
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceExternalMemoryRDMAFeaturesNV external_memory_rdma_features{};
    external_memory_rdma_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_RDMA_FEATURES_NV;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &external_memory_rdma_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(external_memory_rdma_features.externalMemoryRDMA, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestFragmentShaderBarycentricFeatures) {
#ifdef VK_NV_fragment_shader_barycentric
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV fragment_shader_barycentric_features{};
    fragment_shader_barycentric_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_BARYCENTRIC_FEATURES_NV;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &fragment_shader_barycentric_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(fragment_shader_barycentric_features.fragmentShaderBarycentric, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestFragmentShadingRateEnumsFeatures) {
#ifdef VK_NV_fragment_shading_rate_enums
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV fragment_shading_rate_enums_features{};
    fragment_shading_rate_enums_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_ENUMS_FEATURES_NV;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &fragment_shading_rate_enums_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(fragment_shading_rate_enums_features.fragmentShadingRateEnums, VK_TRUE);
    EXPECT_EQ(fragment_shading_rate_enums_features.supersampleFragmentShadingRates, VK_TRUE);
    EXPECT_EQ(fragment_shading_rate_enums_features.noInvocationFragmentShadingRates, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestInheritedViewportScissorFeatures) {
#ifdef VK_NV_inherited_viewport_scissor
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceInheritedViewportScissorFeaturesNV inherited_viewport_scissor_features{};
    inherited_viewport_scissor_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INHERITED_VIEWPORT_SCISSOR_FEATURES_NV;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &inherited_viewport_scissor_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(inherited_viewport_scissor_features.inheritedViewportScissor2D, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestMeshShaderFeatures) {
#ifdef VK_NV_mesh_shader
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceMeshShaderFeaturesNV mesh_shader_features{};
    mesh_shader_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_NV;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &mesh_shader_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(mesh_shader_features.taskShader, VK_TRUE);
    EXPECT_EQ(mesh_shader_features.meshShader, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestRayTracingMotionBlurFeatures) {
#ifdef VK_NV_ray_tracing_motion_blur
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceRayTracingMotionBlurFeaturesNV ray_tracing_motion_blur_features{};
    ray_tracing_motion_blur_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_MOTION_BLUR_FEATURES_NV;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &ray_tracing_motion_blur_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(ray_tracing_motion_blur_features.rayTracingMotionBlur, VK_TRUE);
    EXPECT_EQ(ray_tracing_motion_blur_features.rayTracingMotionBlurPipelineTraceRaysIndirect, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestRepresentativeFragmentTestFeatures) {
#ifdef VK_NV_representative_fragment_test
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV representative_fragment_test_features{};
    representative_fragment_test_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_REPRESENTATIVE_FRAGMENT_TEST_FEATURES_NV;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &representative_fragment_test_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(representative_fragment_test_features.representativeFragmentTest, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestExclusiveScissorFeatures) {
#ifdef VK_NV_scissor_exclusive
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceExclusiveScissorFeaturesNV exclusive_scissor_features{};
    exclusive_scissor_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXCLUSIVE_SCISSOR_FEATURES_NV;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &exclusive_scissor_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(exclusive_scissor_features.exclusiveScissor, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TesthaderImageFootprintFeatures) {
#ifdef VK_NV_shader_image_footprint
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceShaderImageFootprintFeaturesNV shader_image_footprint_features{};
    shader_image_footprint_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXCLUSIVE_SCISSOR_FEATURES_NV;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &shader_image_footprint_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(shader_image_footprint_features.imageFootprint, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestShaderSMBuiltinsFeatures) {
#ifdef VK_NV_shader_sm_builtins
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceShaderSMBuiltinsFeaturesNV shader_sm_builtins_features{};
    shader_sm_builtins_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_FEATURES_NV;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &shader_sm_builtins_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(shader_sm_builtins_features.shaderSMBuiltins, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestShadingRateImageFeatures) {
#ifdef VK_NV_shading_rate_image
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceShadingRateImageFeaturesNV shading_rate_image_features{};
    shading_rate_image_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_FEATURES_NV;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &shading_rate_image_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(shading_rate_image_features.shadingRateImage, VK_TRUE);
    EXPECT_EQ(shading_rate_image_features.shadingRateCoarseSampleOrder, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestMutableDescriptorTypeFeatures) {
#ifdef VK_VALVE_mutable_descriptor_type
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE mutable_descriptor_type_features{};
    mutable_descriptor_type_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MUTABLE_DESCRIPTOR_TYPE_FEATURES_VALVE;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &mutable_descriptor_type_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(mutable_descriptor_type_features.mutableDescriptorType, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestDynamicRenderingFeatures) {
#ifdef VK_KHR_dynamic_rendering
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_features{};
    dynamic_rendering_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &dynamic_rendering_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(dynamic_rendering_features.dynamicRendering, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestImageViewMinLodFeatures) {
#ifdef VK_EXT_image_view_min_lod
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceImageViewMinLodFeaturesEXT image_view_min_lod_features{};
    image_view_min_lod_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_VIEW_MIN_LOD_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &image_view_min_lod_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(image_view_min_lod_features.minLod, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestFragmentDensityMap2Features) {
#ifdef VK_EXT_fragment_density_map2
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceFragmentDensityMap2FeaturesEXT fragment_density_map_2_features{};
    fragment_density_map_2_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &fragment_density_map_2_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(fragment_density_map_2_features.fragmentDensityMapDeferred, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestFragmentDensityMapOffsetFeatures) {
#ifdef VK_QCOM_fragment_density_map_offset
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM fragment_density_map_offset_features{};
    fragment_density_map_offset_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_OFFSET_FEATURES_QCOM;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &fragment_density_map_offset_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(fragment_density_map_offset_features.fragmentDensityMapOffset, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestFragmentDepthClipControlFeatures) {
#ifdef VK_EXT_depth_clip_control
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceDepthClipControlFeaturesEXT depth_clip_control_features{};
    depth_clip_control_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_CONTROL_FEATURES_EXT;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &depth_clip_control_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(depth_clip_control_features.depthClipControl, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestRasterizationOrderAttachmentAccessFeatures) {
#ifdef VK_ARM_rasterization_order_attachment_access
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM rasterization_order_attachment_access_features{};
    rasterization_order_attachment_access_features.sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RASTERIZATION_ORDER_ATTACHMENT_ACCESS_FEATURES_ARM;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &rasterization_order_attachment_access_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    // Todo: needs spec fix and profiles update
    /*EXPECT_EQ(rasterization_order_attachment_access_features.rasterizationOrderColorAttachmentAccess, VK_TRUE);
    EXPECT_EQ(rasterization_order_attachment_access_features.rasterizationOrderDepthAttachmentAccess, VK_TRUE);
    EXPECT_EQ(rasterization_order_attachment_access_features.rasterizationOrderStencilAttachmentAccess, VK_TRUE);*/

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestLinearColorAttachmentFeatures) {
#ifdef VK_NV_linear_color_attachment
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDeviceLinearColorAttachmentFeaturesNV linear_color_attachment_features{};
    linear_color_attachment_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINEAR_COLOR_ATTACHMENT_FEATURES_NV;

    VkPhysicalDeviceFeatures2 features;
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &linear_color_attachment_features;
    vkGetPhysicalDeviceFeatures2(gpu, &features);

    EXPECT_EQ(linear_color_attachment_features.linearColorAttachment, VK_TRUE);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestFormatR4G4UnormPack8) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R4G4_UNORM_PACK8;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
                                                   VK_FORMAT_FEATURE_BLIT_DST_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR4G4B4A4UnormPack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R4G4B4A4_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/
        ;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
                                                   VK_FORMAT_FEATURE_BLIT_DST_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatB4G4R4A4UnormPack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_B4G4R4A4_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
                                                  VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
                                                  VK_FORMAT_FEATURE_BLIT_SRC_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/ |
                                                  VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR5G6B5UnormPack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R5G6B5_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatB5G6R5UnormPack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_B5G6R5_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_DISJOINT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                           VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR5G5B5A1UnormPack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R5G5B5A1_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatB5G5R5A1UnormPack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_B5G5R5A1_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_DISJOINT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        ;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatA1R5G5B5UnormPack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_A1R5G5B5_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8Unorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8Snorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8Uscaled) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/ |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8Sscaled) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_DISJOINT_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8Uint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8Sint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8Srgb) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8_SRGB;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/ |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_DST_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8G8Unorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8G8_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8G8Snorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8G8_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8G8Uscaled) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8G8_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8G8Sscaled) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8G8_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/ |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8G8Uint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8G8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8G8Sint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8G8_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
                                                  VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8G8Srgb) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8G8_SRGB;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8G8B8Unorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8G8B8_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8G8B8Snorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8G8B8_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8G8B8Uscaled) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8G8B8_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        ;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8G8B8Sscaled) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8G8B8_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8G8B8Uint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8G8B8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_DISJOINT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8G8B8Sint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8G8B8_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
                                                   VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8G8B8Srgb) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8G8B8_SRGB;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/
        ;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatB8G8R8Unorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_B8G8R8_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/ |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
                                           VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatB8G8R8Snorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_B8G8R8_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                                  VK_FORMAT_FEATURE_DISJOINT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatB8G8R8Uscaled) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_B8G8R8_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatB8G8R8Sscaled) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_B8G8R8_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/ |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatB8G8R8Uint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_B8G8R8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
        | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatB8G8R8Sint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_B8G8R8_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_DISJOINT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/ |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
                                                   VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatB8G8R8Srgb) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_B8G8R8_SRGB;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_BLIT_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8G8B8A8Unorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                   VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8G8B8A8Snorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8G8B8A8_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/ |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
                                           VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8G8B8A8Uscaled) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8G8B8A8_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8G8B8A8Sscaled) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8G8B8A8_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_BLIT_DST_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
                                       /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
        ;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8G8B8A8Uint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8G8B8A8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8G8B8A8Sint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8G8B8A8_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR8G8B8A8Srgb) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatB8G8R8A8Unorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_B8G8R8A8_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatB8G8R8A8Snorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_B8G8R8A8_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
                                           VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatB8G8R8A8Uscaled) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_B8G8R8A8_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/
        ;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatB8G8R8A8Sscaled) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_B8G8R8A8_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/ |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatB8G8R8A8Uint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_B8G8R8A8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
                                                             /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_DISJOINT_BIT |
                                                   VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
                                           VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatB8G8R8A8Sint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_B8G8R8A8_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatB8G8R8A8Srgb) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_B8G8R8A8_SRGB;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatA8B8G8R8UnormPack32) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_A8B8G8R8_UNORM_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_TRANSFER_SRC_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatA8B8G8R8SnormPack32) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_A8B8G8R8_SNORM_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_TRANSFER_DST_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatA8B8G8R8UscaledPack32) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_A8B8G8R8_USCALED_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
                                                  VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
                                                  VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        ;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatA8B8G8R8SscaledPack32) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_A8B8G8R8_SSCALED_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
                                                  VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
                                                  VK_FORMAT_FEATURE_BLIT_SRC_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_DISJOINT_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
                                           VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
                                           VK_FORMAT_FEATURE_DISJOINT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatA8B8G8R8UintPack32) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_A8B8G8R8_UINT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatA8B8G8R8SintPack32) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_A8B8G8R8_SINT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatA8B8G8R8SrgbPack32) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_A8B8G8R8_SRGB_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatA2R10G10B10UnormPack32) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_A2R10G10B10_UNORM_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatA2R10G10B10SnormPack32) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_A2R10G10B10_SNORM_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
                                           VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatA2R10G10B10UscaledPack32) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_A2R10G10B10_USCALED_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_DISJOINT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatA2R10G10B10SscaledPack32) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_A2R10G10B10_SSCALED_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatA2R10G10B10UintPack32) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_A2R10G10B10_UINT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatA2R10G10B10SintPack32) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_A2R10G10B10_SINT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatA2B10G10R10UnormPack32) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_A2B10G10R10_UNORM_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features =
        /*VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/ VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatA2B10G10R10SnormPack32) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_A2B10G10R10_SNORM_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatA2B10G10R10UscaledPack32) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_A2B10G10R10_USCALED_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatA2B10G10R10SscaledPack32) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_A2B10G10R10_SSCALED_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatA2B10G10R10UintPack32) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_A2B10G10R10_UINT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_TRANSFER_DST_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/ |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatA2B10G10R10SintPack32) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_A2B10G10R10_SINT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16Unorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16Snorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                                   VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
                                                   VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16Uscaled) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
                                           VK_FORMAT_FEATURE_DISJOINT_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16Sscaled) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/
        ;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16Uint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/ |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16Sint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16Sfloat) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
                                           VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16G16Unorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16G16_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16G16Snorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16G16_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
                                           VK_FORMAT_FEATURE_DISJOINT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16G16Uscaled) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16G16_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
        ;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_BLIT_DST_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG |
                                                   VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16G16Sscaled) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16G16_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16G16Uint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16G16_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16G16Sint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16G16_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16G16Sfloat) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16G16_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16G16B16Unorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16G16B16_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16G16B16Snorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16G16B16_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/ |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16G16B16Uscaled) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16G16B16_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/ |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16G16B16Sscaled) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16G16B16_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT |
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16G16B16Uint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16G16B16_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
                                                  VK_FORMAT_FEATURE_DISJOINT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16G16B16Sint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16G16B16_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16G16B16Sfloat) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16G16B16_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
                                                     /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/
        ;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                           VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16G16B16A16Unorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16G16B16A16_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
                                                  VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                                  VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16G16B16A16Snorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16G16B16A16_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
        VK_FORMAT_FEATURE_TRANSFER_DST_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/ |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16G16B16A16Uscaled) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16G16B16A16_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16G16B16A16Sscaled) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16G16B16A16_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
        | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16G16B16A16Uint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16G16B16A16_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        ;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16G16B16A16Sint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16G16B16A16_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR16G16B16A16Sfloat) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R16G16B16A16_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/ |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR32Uint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R32_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
                                           VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
                                           VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR32Sint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R32_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/ |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR32Sfloat) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R32_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR32G32Uint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R32G32_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                                  VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
                                                  VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
                                           VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR32G32Sint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R32G32_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
        VK_FORMAT_FEATURE_TRANSFER_DST_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
                                           VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR32G32Sfloat) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R32G32_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT |
                                           VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR32G32B32Uint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R32G32B32_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/
        ;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR32G32B32Sint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R32G32B32_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR32G32B32Sfloat) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R32G32B32_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR32G32B32A32Uint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R32G32B32A32_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
        ;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                                   VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR32G32B32A32Sint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R32G32B32A32_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/ |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/
        ;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR32G32B32A32Sfloat) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R32G32B32A32_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                                   VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR64Uint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R64_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT |
                                                   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR64Sint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R64_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
                                                  VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
                                                  VK_FORMAT_FEATURE_BLIT_SRC_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/ |
                                                  VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR64Sfloat) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R64_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
                                                     /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
                                           VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR64G64Uint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R64G64_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR64G64Sint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R64G64_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR64G64Sfloat) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R64G64_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR64G64B64Uint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R64G64B64_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
                                                   VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
                                           VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR64G64B64Sint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R64G64B64_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_DISJOINT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/ |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR64G64B64Sfloat) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R64G64B64_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR64G64B64A64Uint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R64G64B64A64_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/
        ;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR64G64B64A64Sint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R64G64B64A64_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/ |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        /*VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/ VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR64G64B64A64Sfloat) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R64G64B64A64_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_TRANSFER_SRC_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatB10G11R11UfloatPack32) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_B10G11R11_UFLOAT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/ |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatE5b9g9r9UfloatPack32) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatD16Unorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_D16_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/
        ;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatX8D24UnormPack32) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_X8_D24_UNORM_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatD32Sfloat) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_D32_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_TRANSFER_DST_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/ |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatS8Uint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_S8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
                                                  VK_FORMAT_FEATURE_BLIT_DST_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatD16UnormS8Uint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_D16_UNORM_S8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/ |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatD24UnormS8Uint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_D24_UNORM_S8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatD32SfloatS8Uint) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_D32_SFLOAT_S8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatBC1RGBUnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_BC1_RGB_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatBC1RGBSrgbBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_BC1_RGB_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatBC1RGBAUnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/ |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatBC1RGBASrgbBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
        VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_TRANSFER_DST_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatBC2UnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_BC2_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatBC2SrgbBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_BC2_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/ |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatBC3UnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_BC3_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatBC3SrgbBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_BC3_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/
        ;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatBC4UnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_BC4_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/ |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
                                                   VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatBC4SnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_BC4_SNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                                   VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR |
                                                   VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatBC5UnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_BC5_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
                                                  VK_FORMAT_FEATURE_DISJOINT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatBC5SnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_BC5_SNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatBC6hUfloatBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_BC6H_UFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatBC6hSfloatBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_BC6H_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
                                                  VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
                                                  VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT |
                                                   VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT |
                                           VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
                                           VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatBC7UnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_BC7_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/ |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatBC7SrgbBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_BC7_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatEtc2R8G8B8UnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatEtc2R8G8B8SrgbBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_DISJOINT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/ /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatEtc2R8G8B8A1UnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatEtc2R8G8B8A1SrgbBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatEtc2R8G8B8A8UnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatEtc2R8G8B8A8SrgbBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
                                                  VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
                                                  VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatEacR11UnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_EAC_R11_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT |
                                                   VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatEacR11SnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_EAC_R11_SNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT |
                                                   VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatEacR11G11UnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_EAC_R11G11_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatEacR11G11SnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_EAC_R11G11_SNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc4x4UnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc4x4SrgbBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_4x4_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
                                           VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc5x4UnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc5x4SrgbBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_5x4_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_DST_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc5x5UnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc5x5SrgbBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_5x5_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc6x5UnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc6x5SrgbBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_6x5_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_DISJOINT_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_TRANSFER_SRC_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc6x6UnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
                                                   VK_FORMAT_FEATURE_DISJOINT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc6x6SrgbBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_6x6_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/ |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc8x5UnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        ;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc8x5SrgbBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_8x5_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc8x6UnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
        ;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc8x6SrgbBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_8x6_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/ |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
        VK_FORMAT_FEATURE_BLIT_SRC_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/ |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc8x8UnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
        | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc8x8SrgbBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_8x8_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc10x5UnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc10x5SrgbBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_10x5_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_DISJOINT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
                                                   VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
                                                   VK_FORMAT_FEATURE_DISJOINT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc10x6UnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT |
                                                  VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT |
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc10x6SrgbBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_10x6_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc10x8UnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc10x8SrgbBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_10x8_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
                                                  VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
                                                  VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc10x10UnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc10x10SrgbBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_10x10_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_DISJOINT_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
                                           VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc12x10UnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT |
                                                  VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc12x10SrgbBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_12x10_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc12x12UnormBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/ /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc12x12SrgbBLock) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_12x12_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatG8B8G8R8422Unorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G8B8G8R8_422_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/ |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatB8G8R8G8422Unorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_B8G8R8G8_422_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatG8B8R83Plane420Unorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatG8B8R82Plane420Unorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G8_B8R8_2PLANE_420_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
        ;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatG8B8R83Plane422Unorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/ |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatG8B8R82Plane422Unorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G8_B8R8_2PLANE_422_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG |
                                                  VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/ |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatG8B8R83Plane444Unorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR10X6UnormPack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R10X6_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
        VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/ |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/
        ;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/
        ;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR10X6g10x6Unorm2Pack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R10X6G10X6_UNORM_2PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR10X6g10x6b10x6a10x6Unorm4Pack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT |
                                                   VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatG10X6b10x6g10x6r10x6422Unorm4Pack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatB10X6g10x6r10x6g10x6422Unorm4Pack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
                                           VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
                                           VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatG10X6B10X6R10X63Plane420Unorm3Pack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
                                                       /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/
        | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatG10X6B10X6r10x62Plane420Unorm3Pack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatG10X6B10X6R10X63Plane422Unorm3Pack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/ |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
                                           VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatG10X6B10X6r10x62Plane422Unorm3Pack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
                                                   VK_FORMAT_FEATURE_DISJOINT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatG10X6B10X6R10X63Plane444Unorm3Pack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR12X4UnormPack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R12X4_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR12X4g12x4Unorm2Pack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R12X4G12X4_UNORM_2PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                  VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatR12X4g12x4b12x4a12x4Unorm4Pack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatG12X4b12x4g12x4r12x4422Unorm4Pack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatB12X4g12x4r12x4g12x4422Unorm4Pack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/
        ;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
                                                   VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT |
                                                   VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatG12X4B12X4R12X43Plane420Unorm3Pack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
                                                  VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT |
                                                  VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatG12X4B12X4r12x42Plane420Unorm3Pack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG |
                                                  VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR |
                                                  VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatG12X4B12X4R12X43Plane422Unorm3Pack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                                  VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
                                                  VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatG12X4B12X4r12x42Plane422Unorm3Pack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/ |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatG12X4B12X4R12X43Plane444Unorm3Pack16) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT |
                                                   VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatG16B16G16R16422Unorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G16B16G16R16_422_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatB16G16R16G16422Unorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_B16G16R16G16_422_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/ |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatG16B16R163Plane420Unorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/ |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatG16B16R162Plane420Unorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G16_B16R16_2PLANE_420_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatG16B16R163Plane422Unorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/
        ;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
                                                   VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
                                                   VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
                                           VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatG16B16R162Plane422Unorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G16_B16R16_2PLANE_422_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/
        ;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatG16B16R163Plane444Unorm) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
                                                  VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
                                                  VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatPvrtc12BPpUnormBLockImg) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatPvrtc14BPpUnormBLockImg) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
        VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatPvrtc22BPpUnormBLockImg) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT |
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatPvrtc24BPpUnormBLockImg) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/
        /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatPvrtc12BPpSrgbBLockImg) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
                                                   VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatPvrtc14BPpSrgbBLockImg) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatPvrtc22BPpSrgbBLockImg) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/ |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
                                                   VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT |
                                                   VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatPvrtc24BPpSrgbBLockImg) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc4x4SfloatBLockExt) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc5x4SfloatBLockExt) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                   VK_FORMAT_FEATURE_DISJOINT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT |
                                           VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc5x5SfloatBLockExt) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/ |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT |
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc6x5SfloatBLockExt) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/ |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc6x6SfloatBLockExt) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT |
                                                   VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
                                                   VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc8x5SfloatBLockExt) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc8x6SfloatBLockExt) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc8x8SfloatBLockExt) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc10x5SfloatBLockExt) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT |
        VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc10x6SfloatBLockExt) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                                                   VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc10x8SfloatBLockExt) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/
        ;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc10x10SfloatBLockExt) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/ |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/ |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc12x10SfloatBLockExt) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR |
        VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatAstc12x12SfloatBLockExt) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
                                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
}

TEST(profiles, TestFormatG8B8R82Plane444UnormExt) {
#ifdef VK_EXT_ycbcr_2plane_444_formats
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G8_B8R8_2PLANE_444_UNORM_EXT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/ |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestFormatG10X6B10X6r10x62Plane444Unorm3Pack16Ext) {
#ifdef VK_EXT_ycbcr_2plane_444_formats
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR*/;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestFormatG12X4B12X4r12x42Plane444Unorm3Pack16Ext) {
#ifdef VK_EXT_ycbcr_2plane_444_formats
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT |
                                                  VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestFormatG16B16R162Plane444UnormExt) {
#ifdef VK_EXT_ycbcr_2plane_444_formats
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_G16_B16R16_2PLANE_444_UNORM_EXT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
                                                   VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
                                                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT |
                                           VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT |
                                           VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestFormatA4R4G4B4UnormPack16Ext) {
#ifdef VK_EXT_4444_formats
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT |
        VK_FORMAT_FEATURE_DISJOINT_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR*/ |
        VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT |
                                                   VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT |
                                                   VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
#endif
}

TEST(profiles, TestFormatA4B4G4R4UnormPack16Ext) {
#ifdef VK_EXT_4444_formats
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
    profiles_test::setProfilesSimulateAllCapabilities();

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    err = inst_builder.makeInstance();
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    ASSERT_EQ(err, VK_SUCCESS);

    VkFormat format = VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features =
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features =
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT |
        VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT /*|VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR*/;
    VkFormatFeatureFlags buffer_features = 0;

    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);

    vkDestroyInstance(test_inst, nullptr);
#endif
}
