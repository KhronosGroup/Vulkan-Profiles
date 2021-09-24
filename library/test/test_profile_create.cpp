#include "vulkan_profiles.hpp"

#include <gtest/gtest.h>

#include <cstdio>
#include <memory>
#include <stdexcept>
#include <vector>

static const float DEFAULT_QUEUE_PRIORITY(0.0f);

class TestScaffold {
   public:
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    uint32_t queueFamilyIndex;
    VkDeviceQueueCreateInfo queueCreateInfo;

    TestScaffold() {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Testing scaffold";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 2, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 2, 0);
        appInfo.apiVersion = VK_API_VERSION_1_2;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        VkResult res = vkCreateInstance(&createInfo, nullptr, &instance);
        EXPECT_TRUE(res == VK_SUCCESS);

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        EXPECT_TRUE(deviceCount > 0);

        std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());
        physicalDevice = physicalDevices[0];
        queueCreateInfo = {};
        uint32_t queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());
        for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); ++i) {
            if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                queueFamilyIndex = i;
                queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueCreateInfo.queueFamilyIndex = i;
                queueCreateInfo.queueCount = 1;
                queueCreateInfo.pQueuePriorities = &DEFAULT_QUEUE_PRIORITY;
                break;
            }
        }
    }
};

TEST(test_profile, enumerate) {
    TestScaffold scaffold;

    std::vector<VpProfileProperties> profiles;
    uint32_t profileCount = 0;
    {
        vpEnumerateDeviceProfiles(scaffold.physicalDevice, nullptr, &profileCount, nullptr);

        profiles.resize(profileCount);
        vpEnumerateDeviceProfiles(scaffold.physicalDevice, nullptr, &profileCount, &profiles[0]);

        for (VpProfileProperties profile : profiles) {
            std::printf("Profile supported: %s, version %d\n", profile.profileName, profile.specVersion);
        }
    }

    EXPECT_TRUE(profileCount > 0);
}


TEST(test_profile, create_device) {
    return;

    TestScaffold scaffold;

    std::vector<VpProfileProperties> profiles;
    uint32_t profileCount = 0;
    {
        vpEnumerateDeviceProfiles(scaffold.physicalDevice, nullptr, &profileCount, nullptr);

        profiles.resize(profileCount);
        vpEnumerateDeviceProfiles(scaffold.physicalDevice, nullptr, &profileCount, &profiles[0]);

        for (VpProfileProperties profile : profiles) {
            std::printf("Profile supported: %s, version %d\n", profile.profileName, profile.specVersion);
        }
    }

    EXPECT_TRUE(profileCount > 0);

    int error = 0;

    static const char* extensions[] = {"VK_KHR_synchronization2", "VK_KHR_zero_initialize_workgroup_memory",
                                       "VK_KHR_shader_terminate_invocation", "VK_KHR_imageless_framebuffer"};

    for (const VpProfileProperties& profile : profiles) {
        std::printf("Creating a Vulkan device using profile %s, version %d: ", profile.profileName, profile.specVersion);

        VkDevice device;

        VkPhysicalDeviceFeatures enabledFeatures = {};
        enabledFeatures.robustBufferAccess = VK_TRUE;

        VkDeviceCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        info.pNext = nullptr;
        info.queueCreateInfoCount = 1;
        info.pQueueCreateInfos = &scaffold.queueCreateInfo;
        info.enabledExtensionCount = countof(extensions);
        info.ppEnabledExtensionNames = extensions;
        info.pEnabledFeatures = &enabledFeatures;

        VkResult res = vpCreateDevice(scaffold.physicalDevice, &profile, &info, nullptr, &device);
        if (res != VK_SUCCESS) {
            ++error;
            std::printf("FAILURE: %d\n", res);
        } else {
            std::printf("SUCCESS!\n");
        }
    }

    EXPECT_EQ(0, error);
}
