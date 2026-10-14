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

#include "vulkan_loader.h"

#include <dlfcn.h>

VulkanLoader::~VulkanLoader() {
    if (handle_ != nullptr) {
        dlclose(handle_);
        handle_ = nullptr;
    }
}

bool VulkanLoader::LoadGlobal(std::string& error) {
    if (handle_ != nullptr) return true;

    // The system Vulkan loader. Only the well-known system library name is used;
    // no caller-controlled path is ever passed to dlopen.
    handle_ = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
    if (handle_ == nullptr) {
        const char* reason = dlerror();
        error = std::string("Failed to load libvulkan.so: ") + (reason ? reason : "unknown error");
        return false;
    }

    GetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(handle_, "vkGetInstanceProcAddr"));
    if (GetInstanceProcAddr == nullptr) {
        error = "libvulkan.so does not export vkGetInstanceProcAddr";
        return false;
    }

    EnumerateInstanceVersion = reinterpret_cast<PFN_vkEnumerateInstanceVersion>(
        GetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceVersion"));
    EnumerateInstanceExtensionProperties = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(
        GetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceExtensionProperties"));
    CreateInstance = reinterpret_cast<PFN_vkCreateInstance>(GetInstanceProcAddr(VK_NULL_HANDLE, "vkCreateInstance"));

    if (EnumerateInstanceExtensionProperties == nullptr || CreateInstance == nullptr) {
        error = "Vulkan loader is missing mandatory global entry points";
        return false;
    }
    return true;
}

void VulkanLoader::LoadInstance(VkInstance instance) {
    auto load = [&](const char* name) { return GetInstanceProcAddr(instance, name); };

    DestroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(load("vkDestroyInstance"));
    EnumeratePhysicalDevices = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(load("vkEnumeratePhysicalDevices"));
    GetPhysicalDeviceProperties =
        reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(load("vkGetPhysicalDeviceProperties"));
    EnumerateDeviceExtensionProperties =
        reinterpret_cast<PFN_vkEnumerateDeviceExtensionProperties>(load("vkEnumerateDeviceExtensionProperties"));

    // Prefer the core 1.1 entry point, fall back to VK_KHR_get_physical_device_properties2.
    GetPhysicalDeviceProperties2 =
        reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2>(load("vkGetPhysicalDeviceProperties2"));
    if (GetPhysicalDeviceProperties2 == nullptr) {
        GetPhysicalDeviceProperties2 =
            reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2>(load("vkGetPhysicalDeviceProperties2KHR"));
    }
}
