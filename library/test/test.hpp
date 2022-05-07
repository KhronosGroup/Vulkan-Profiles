/*
 * Copyright (c) 2021-2022 Valve Corporation
 * Copyright (c) 2021-2022 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Authors:
 * - Christophe Riccio <christophe@lunarg.com>
 */

#pragma once

#include <vulkan/vulkan_core.h>

#include <gtest/gtest.h>

#if defined(_WIN32)
#include <windows.h>
#include <crtdbg.h>
#include <errhandlingapi.h>
#endif
#include <algorithm>

struct DisableDebugPopup {
    DisableDebugPopup() {
#if defined(_WIN32)
        // Avoid "Abort, Retry, Ignore" dialog boxes
        SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOALIGNMENTFAULTEXCEPT | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
        _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
        _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
        _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
        _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
        _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif
    }
};

static DisableDebugPopup disableDebugPopup;

static const float DEFAULT_QUEUE_PRIORITY(0.0f);

class TestScaffold {
   public:
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    uint32_t queueFamilyIndex;
    VkDeviceQueueCreateInfo queueCreateInfo;

    TestScaffold() : instance(VK_NULL_HANDLE), physicalDevice(VK_NULL_HANDLE), queueFamilyIndex(0), queueCreateInfo{} {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Testing scaffold";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 2, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 2, 0);
        appInfo.apiVersion = VK_API_VERSION_1_2;

        const char* surface_extension = VK_KHR_SURFACE_EXTENSION_NAME;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        uint32_t propertyCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, nullptr);
        std::vector<VkExtensionProperties> properties(propertyCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, properties.data());
        if (std::any_of(properties.begin(), properties.end(), [&surface_extension](const VkExtensionProperties& p) {
                return strcmp(p.extensionName, surface_extension) == 0;
            })) {
            // Enable VK_KHR_surface if supported, as tests might use it
            createInfo.enabledExtensionCount = 1;
            createInfo.ppEnabledExtensionNames = &surface_extension;
        }

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

