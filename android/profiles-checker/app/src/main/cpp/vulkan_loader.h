/*
 * Copyright 2026 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <vulkan/vulkan.h>

#include <string>

// Minimal dynamic Vulkan loader. The app does not link against libvulkan.so so
// that it can still start (and show a readable report) on devices without a
// usable Vulkan implementation.
class VulkanLoader {
   public:
    VulkanLoader() = default;
    ~VulkanLoader();

    VulkanLoader(const VulkanLoader&) = delete;
    VulkanLoader& operator=(const VulkanLoader&) = delete;

    // Loads libvulkan.so and the global-level entry points.
    // Returns false and fills `error` on failure.
    bool LoadGlobal(std::string& error);

    // Loads the instance-level entry points used by the app.
    void LoadInstance(VkInstance instance);

    bool IsLoaded() const { return handle_ != nullptr; }

    // Global level
    PFN_vkGetInstanceProcAddr GetInstanceProcAddr = nullptr;
    PFN_vkEnumerateInstanceVersion EnumerateInstanceVersion = nullptr;  // null on Vulkan 1.0 loaders
    PFN_vkEnumerateInstanceExtensionProperties EnumerateInstanceExtensionProperties = nullptr;
    PFN_vkCreateInstance CreateInstance = nullptr;

    // Instance level
    PFN_vkDestroyInstance DestroyInstance = nullptr;
    PFN_vkEnumeratePhysicalDevices EnumeratePhysicalDevices = nullptr;
    PFN_vkGetPhysicalDeviceProperties GetPhysicalDeviceProperties = nullptr;
    PFN_vkGetPhysicalDeviceProperties2 GetPhysicalDeviceProperties2 = nullptr;  // core or KHR alias, may be null
    PFN_vkEnumerateDeviceExtensionProperties EnumerateDeviceExtensionProperties = nullptr;

   private:
    void* handle_ = nullptr;
};
