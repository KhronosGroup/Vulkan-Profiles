#include <vulkan/vulkan_profiles.hpp>

#include <gtest/gtest.h>

#include <iostream>
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
        for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++) {
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

TEST(test_profile_create_generated, create_device) {
    return;

    std::vector<VpProfile> profiles;
    uint32_t profileCount = 0;

    {
        TestScaffold scaffold;
        vpGetPhysicalDeviceProfiles(scaffold.physicalDevice, &profileCount, nullptr);

        profiles.resize(profileCount);
        vpGetPhysicalDeviceProfiles(scaffold.physicalDevice, &profileCount, &profiles[0]);

        for (VpProfile profile : profiles) {
            std::cout << "Profile supported: " << profile << std::endl;
        }
    }

    EXPECT_TRUE(profileCount > 0);

    for (VpProfile profile : profiles) {
        TestScaffold scaffold;
        VkDevice device;
        VkDeviceCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        info.queueCreateInfoCount = 1;
        info.pQueueCreateInfos = &scaffold.queueCreateInfo;
        info.enabledExtensionCount = 0;
        info.ppEnabledExtensionNames = nullptr;
        auto res = vpCreateDevice(scaffold.physicalDevice, profile, &info, nullptr, &device);
        if (res != VK_SUCCESS) {
            std::cout << "FAILURE: " << res << std::endl;
        } else {
            std::cout << "SUCCESS?" << std::endl;
        }

        EXPECT_EQ(VK_SUCCESS, res);
    }
}
