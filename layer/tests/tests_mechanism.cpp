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

TEST(layer, selecting_profile) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    {
        inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_selecting_profile.json";
        settings.emulate_portability = true;
        settings.profile_name = "VP_LUNARG_test_selecting_profile";
        settings.simulate_capabilities = SimulateCapabilityFlag::SIMULATE_EXTENSIONS_BIT |
                                         SimulateCapabilityFlag::SIMULATE_PROPERTIES_BIT;

        err = inst_builder.makeInstance(&settings);
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

    {
        inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_selecting_profile.json";
        settings.emulate_portability = true;
        settings.profile_name = "VP_LUNARG_test_selecting_profile_subset";
        settings.simulate_capabilities = SimulateCapabilityFlag::SIMULATE_EXTENSIONS_BIT |
                                         SimulateCapabilityFlag::SIMULATE_PROPERTIES_BIT;

        err = inst_builder.makeInstance(&settings);
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

        EXPECT_EQ(gpu_props.limits.maxImageDimension1D, 2048u);

        inst_builder.reset();
    }
}

TEST(layer, reading_flags) {
#if defined(VK_NV_fragment_shading_rate_enums) && defined(VK_KHR_fragment_shading_rate) && defined(VK_KHR_shader_float_controls)
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    VkProfileLayerSettingsEXT settings;
    settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_api_alternate.json";
    settings.emulate_portability = true;
    settings.profile_name = "VP_LUNARG_test_api";
    settings.simulate_capabilities = SimulateCapabilityFlag::SIMULATE_ALL_CAPABILITIES;

    err = inst_builder.makeInstance(&settings);
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance instance = inst_builder.getInstance();

    VkPhysicalDevice physical_device;
    err = inst_builder.getPhysicalDevice(&physical_device);
    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    err = inst_builder.getPhysicalDevice(&gpu);
    if (gpu == VK_NULL_HANDLE) return;

    VkPhysicalDeviceFragmentShadingRateEnumsPropertiesNV fragment_shading_rate_enums_properties{};
    fragment_shading_rate_enums_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_ENUMS_PROPERTIES_NV;

    VkPhysicalDeviceFragmentShadingRatePropertiesKHR fragment_shading_rate_properties{};
    fragment_shading_rate_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR;
    fragment_shading_rate_properties.pNext = &fragment_shading_rate_enums_properties;

    VkPhysicalDeviceFloatControlsPropertiesKHR float_control_properties{};
    float_control_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT_CONTROLS_PROPERTIES_KHR;
    float_control_properties.pNext = &fragment_shading_rate_properties;

    VkPhysicalDeviceProperties2 gpu_props{};
    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props.pNext = &float_control_properties;
    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);

    EXPECT_EQ(float_control_properties.denormBehaviorIndependence, VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_32_BIT_ONLY);
    EXPECT_EQ(float_control_properties.roundingModeIndependence, VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_32_BIT_ONLY);

    EXPECT_EQ(fragment_shading_rate_properties.maxFragmentShadingRateRasterizationSamples, VK_SAMPLE_COUNT_2_BIT);

    EXPECT_EQ(fragment_shading_rate_enums_properties.maxFragmentShadingRateInvocationCount, VK_SAMPLE_COUNT_2_BIT);
#endif
}

TEST(layer, reading_duplicated_members) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    VkProfileLayerSettingsEXT settings;
    settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_duplicated.json";
    settings.emulate_portability = true;
    settings.profile_name = "VP_LUNARG_test_duplicated";
    settings.simulate_capabilities = SimulateCapabilityFlag::SIMULATE_ALL_CAPABILITIES;
    settings.debug_fail_on_error = true;

    err = inst_builder.makeInstance(&settings);
    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance instance = inst_builder.getInstance();

    VkPhysicalDevice physical_device;
    err = inst_builder.getPhysicalDevice(&physical_device);
    ASSERT_EQ(err, VK_ERROR_INITIALIZATION_FAILED);
}


TEST(layer, TestParsingAllFormatProperties) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    VkProfileLayerSettingsEXT settings;
    settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_formats.json";
    settings.emulate_portability = true;
    settings.profile_name = "VP_LUNARG_test_formats";
    settings.simulate_capabilities = SimulateCapabilityFlag::SIMULATE_ALL_CAPABILITIES;

    err = inst_builder.makeInstance(&settings);
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
#endif

    vkDestroyInstance(test_inst, nullptr);
}
