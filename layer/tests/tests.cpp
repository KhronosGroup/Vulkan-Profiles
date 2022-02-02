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
        profiles_test::setProfilesSimulateCapabilities(SimulateCapabilityFlag::SIMULATE_EXTENSIONS_BIT);
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
        profiles_test::setProfilesSimulateCapabilities(SimulateCapabilityFlag::SIMULATE_EXTENSIONS_BIT);
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
        const std::string filepath = TEST_SOURCE_PATH "/../../profiles/test/data/VP_LUNARG_test_api.json";
        profiles_test::setProfilesFilename(filepath);
        profiles_test::setProfilesEmulatePortabilitySubsetExtension(true);
        profiles_test::setProfilesProfileName("VP_LUNARG_test_api");
        profiles_test::setProfilesSimulateCapabilities(SimulateCapabilityFlag::SIMULATE_EXTENSIONS_BIT);
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

TEST(layer, TestExcludingDeviceExtensions) {
    VkResult err = VK_SUCCESS;

    const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
    profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

    profiles_test::VulkanInstanceBuilder inst_builder;

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/VP_LUNARG_desktop_portability_2021.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_desktop_portability_2021");
    profiles_test::setProfilesEmulatePortabilitySubsetExtension(true);
    profiles_test::setProfilesFailOnError(false);
    std::vector<std::string> disable = {
        VK_KHR_MAINTENANCE_1_EXTENSION_NAME,
        VK_KHR_MAINTENANCE_2_EXTENSION_NAME,
        VK_KHR_MAINTENANCE_3_EXTENSION_NAME,
        VK_KHR_MAINTENANCE_4_EXTENSION_NAME,
    };
    profiles_test::setExcludeDeviceExtensions(disable);

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

    const std::string filepath = TEST_SOURCE_PATH "/../../profiles/VP_LUNARG_desktop_portability_2021.json";
    profiles_test::setProfilesFilename(filepath);
    profiles_test::setProfilesProfileName("VP_LUNARG_desktop_portability_2021");
    profiles_test::setProfilesEmulatePortabilitySubsetExtension(true);
    profiles_test::setProfilesFailOnError(false);
    std::vector<std::string> disable = {
        "VK_FORMAT_R8G8B8A8_UNORM",
    };
    profiles_test::setExcludeFormats(disable);

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

    VkFormatProperties format_properties = {};
    vkGetPhysicalDeviceFormatProperties(gpu, VK_FORMAT_R8G8B8A8_UNORM, &format_properties);

    ASSERT_EQ(format_properties.linearTilingFeatures, 0);
    ASSERT_EQ(format_properties.optimalTilingFeatures, 0);
    ASSERT_EQ(format_properties.bufferFeatures, 0);
}
