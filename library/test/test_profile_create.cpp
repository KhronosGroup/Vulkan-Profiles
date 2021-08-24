#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_profiles.h>

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
        if (res != VK_SUCCESS) {
            throw std::runtime_error("Failed to create instance.");
        }
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            throw std::runtime_error("No Vulkan devices found.");
        }
        std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());
        physicalDevice = physicalDevices[0];
        queueCreateInfo = {};
        uint32_t queueFamilyCount;
        const float defaultQueuePriority(0.0f);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());
        for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++) {
            if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                queueFamilyIndex = i;
                queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueCreateInfo.queueFamilyIndex = i;
                queueCreateInfo.queueCount = 1;
                queueCreateInfo.pQueuePriorities = &defaultQueuePriority;
                break;
            }
        }
    }
};

int main() {
    std::vector<VpProfile> profiles{VP_PROFILE_ANGLE_GOOGLE, VP_PROFILE_DESKTOP_PORTABILITY_LUNARG, VP_PROFILE_EMPTY_EXT,
                                    VP_PROFILE_NULL_EXT,     VP_PROFILE_ROADMAP_2022_KHR,           VP_PROFILE_ROADMAP_2025_KHR};
    {
        TestScaffold scaffold;
        std::vector<VkBool32> supportedProfiles(profiles.size());
        vpGetPhysicalDeviceProfiles(scaffold.physicalDevice, profiles.size(), profiles.data(), supportedProfiles.data());

        for (VkBool32 support : supportedProfiles) {
            std::cout << "Profile supported: " << support << std::endl;
        }
    }

    for (VpProfile profile : profiles) {
        TestScaffold scaffold;
        VkDevice device;
        VkDeviceCreateInfo info = {};
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
    }
    return 0;
}
