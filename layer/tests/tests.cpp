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

        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_api.json";
        settings.emulate_portability = true;
        settings.profile_name = "VP_LUNARG_test_api";
        settings.simulate_capabilities = 0;

        err = inst_builder.makeInstance(&settings);
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

        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_api.json";
        settings.emulate_portability = true;
        settings.profile_name = "VP_LUNARG_test_api_1_2_198";
        settings.simulate_capabilities = SimulateCapabilityFlag::SIMULATE_EXTENSIONS_BIT;

        err = inst_builder.makeInstance(&settings);
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

        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_api.json";
        settings.emulate_portability = true;
        settings.profile_name = "VP_LUNARG_test_api";
        settings.simulate_capabilities = SimulateCapabilityFlag::SIMULATE_EXTENSIONS_BIT;

        err = inst_builder.makeInstance(&settings);
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

            ASSERT_GE(extensions.size(), 234);
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

        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_api.json";
        settings.emulate_portability = true;
        settings.profile_name = "VP_LUNARG_test_api";
        settings.simulate_capabilities = SimulateCapabilityFlag::SIMULATE_EXTENSIONS_BIT;
        settings.debug_fail_on_error = true;

        err = inst_builder.makeInstance(&settings);
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

TEST(layer, TestExcludingDeviceExtensions) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    VkProfileLayerSettingsEXT settings;
    settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_desktop_portability_2021.json";
    settings.profile_name = "VP_LUNARG_desktop_portability_2021";
    settings.emulate_portability = true;
    settings.debug_fail_on_error = false;
    settings.exclude_device_extensions = {
        VK_KHR_MAINTENANCE_1_EXTENSION_NAME,
        VK_KHR_MAINTENANCE_2_EXTENSION_NAME,
        VK_KHR_MAINTENANCE_3_EXTENSION_NAME,
        VK_KHR_MAINTENANCE_4_EXTENSION_NAME};

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

    uint32_t count = 0;
    vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, nullptr);
    std::vector<VkExtensionProperties> device_extensions(count);
    vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, device_extensions.data());

    bool maintenance1 = false;
    bool maintenance2 = false;
    bool maintenance3 = false;
    bool maintenance4 = false;

    for (const auto& ext : device_extensions) {
        if (strcmp(ext.extensionName, VK_KHR_MAINTENANCE_1_EXTENSION_NAME) == 0) {
            maintenance1 = true;
        }
        if (strcmp(ext.extensionName, VK_KHR_MAINTENANCE_2_EXTENSION_NAME) == 0) {
            maintenance2 = true;
        }
        if (strcmp(ext.extensionName, VK_KHR_MAINTENANCE_3_EXTENSION_NAME) == 0) {
            maintenance3 = true;
        }
        if (strcmp(ext.extensionName, VK_KHR_MAINTENANCE_4_EXTENSION_NAME) == 0) {
            maintenance4 = true;
        }
    }

    ASSERT_FALSE(maintenance1);
    ASSERT_FALSE(maintenance2);
    ASSERT_FALSE(maintenance3);
    ASSERT_FALSE(maintenance4);
}

TEST(layer, TestExcludingFormats) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

    VkProfileLayerSettingsEXT settings;
    settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_desktop_portability_2021.json";
    settings.profile_name = "VP_LUNARG_desktop_portability_2021";
    settings.emulate_portability = true;
    settings.debug_fail_on_error = false;
    settings.exclude_formats = {"VK_FORMAT_R8G8B8A8_UNORM"};

    err = inst_builder.makeInstance(&settings);    ASSERT_EQ(err, VK_SUCCESS);

    VkInstance test_inst = inst_builder.getInstance();

    VkPhysicalDevice gpu;
    err = inst_builder.getPhysicalDevice(&gpu);
    if (err != VK_SUCCESS) {
        printf("Profile not supported on device, skipping test.\n");
        vkDestroyInstance(test_inst, nullptr);
        return;
    }

    VkFormatProperties format_properties = {};
    vkGetPhysicalDeviceFormatProperties(gpu, VK_FORMAT_R8G8B8A8_UNORM, &format_properties);

    ASSERT_EQ(format_properties.linearTilingFeatures, 0);
    ASSERT_EQ(format_properties.optimalTilingFeatures, 0);
    ASSERT_EQ(format_properties.bufferFeatures, 0);
}

TEST(layer, TestMissingPhysDevProps2) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");
    inst_builder.setApiVersion(VK_API_VERSION_1_0);

    VkProfileLayerSettingsEXT settings;
    settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_desktop_portability_2021.json";
    settings.profile_name = "VP_LUNARG_desktop_portability_2021";
    settings.emulate_portability = false;
    settings.debug_fail_on_error = false;
    settings.simulate_capabilities = SIMULATE_ALL_CAPABILITIES;

    err = inst_builder.makeInstance(&settings);    ASSERT_EQ(err, VK_SUCCESS);

    VkPhysicalDevice gpu;
    err = inst_builder.getPhysicalDevice(&gpu);

    uint32_t count = 0;
    vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, nullptr);
    ASSERT_EQ(count, 19);
}

TEST(layer, TestExcludedExtensions) {
#ifdef VK_EXT_shader_atomic_float2
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    VkPhysicalDeviceTransformFeedbackPropertiesEXT device_features{};
    device_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT;
    {
        inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");
        inst_builder.setApiVersion(VK_API_VERSION_1_3);

        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_api.json";
        settings.profile_name = "VP_LUNARG_test_api";
        settings.emulate_portability = true;
        settings.debug_fail_on_error = false;
        settings.simulate_capabilities = SIMULATE_API_VERSION_BIT;

        err = inst_builder.makeInstance(&settings);
        ASSERT_EQ(err, VK_SUCCESS);

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(&gpu);

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &device_features;
        vkGetPhysicalDeviceFeatures2(gpu, &features);
        inst_builder.reset();
    }

    {
        inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");
        inst_builder.setApiVersion(VK_API_VERSION_1_3);

        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_api.json";
        settings.profile_name = "VP_LUNARG_test_api";
        settings.emulate_portability = true;
        settings.debug_fail_on_error = false;
        settings.simulate_capabilities = SIMULATE_FORMAT_PROPERTIES_BIT;
        settings.exclude_device_extensions.push_back(VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME);

        err = inst_builder.makeInstance(&settings);
        ASSERT_EQ(err, VK_SUCCESS);

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(&gpu);

        VkPhysicalDeviceTransformFeedbackPropertiesEXT profile_features{};
        profile_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT;

        VkPhysicalDeviceFeatures2 features;
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features.pNext = &profile_features;
        vkGetPhysicalDeviceFeatures2(gpu, &features);

        ASSERT_EQ(device_features.maxTransformFeedbackStreams, profile_features.maxTransformFeedbackStreams);
        ASSERT_EQ(device_features.maxTransformFeedbackBuffers, profile_features.maxTransformFeedbackBuffers);
        ASSERT_EQ(device_features.maxTransformFeedbackBufferSize, profile_features.maxTransformFeedbackBufferSize);
        ASSERT_EQ(device_features.maxTransformFeedbackStreamDataSize, profile_features.maxTransformFeedbackStreamDataSize);
        ASSERT_EQ(device_features.maxTransformFeedbackBufferDataSize, profile_features.maxTransformFeedbackBufferDataSize);
        ASSERT_EQ(device_features.maxTransformFeedbackBufferDataStride, profile_features.maxTransformFeedbackBufferDataStride);
        ASSERT_EQ(device_features.transformFeedbackQueries, profile_features.transformFeedbackQueries);
        ASSERT_EQ(device_features.transformFeedbackStreamsLinesTriangles, profile_features.transformFeedbackStreamsLinesTriangles);
        ASSERT_EQ(device_features.transformFeedbackRasterizationStreamSelect,
                  profile_features.transformFeedbackRasterizationStreamSelect);
        ASSERT_EQ(device_features.transformFeedbackDraw, profile_features.transformFeedbackDraw);
    }
#endif
}
