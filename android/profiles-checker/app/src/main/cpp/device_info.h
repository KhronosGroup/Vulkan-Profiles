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

#include <cstdint>
#include <string>

class VulkanLoader;

// Android system information gathered from system properties.
struct SystemInfo {
    std::string manufacturer;
    std::string model;
    std::string device;
    std::string androidVersion;
    std::string sdkLevel;
    std::string securityPatch;
    std::string socManufacturer;
    std::string socModel;
    std::string hardwareVulkan;  // ro.hardware.vulkan
    std::string gfxDriver0;      // ro.gfx.driver.0 (updatable driver package, if any)
    std::string gfxDriver1;      // ro.gfx.driver.1
    std::string abi;             // ro.product.cpu.abi
};

// Physical device / driver information.
struct DeviceInfo {
    std::string deviceName;
    std::string deviceType;
    uint32_t vendorID = 0;
    uint32_t deviceID = 0;
    std::string vendorName;

    uint32_t apiVersionRaw = 0;
    std::string apiVersion;

    uint32_t driverVersionRaw = 0;
    std::string driverVersion;  // vendor-specific decoding of driverVersionRaw

    // From VkPhysicalDeviceDriverProperties (Vulkan 1.2 or VK_KHR_driver_properties)
    bool hasDriverProperties = false;
    std::string driverName;
    std::string driverInfo;
    std::string driverID;
    std::string conformanceVersion;

    uint32_t deviceExtensionCount = 0;
};

SystemInfo QuerySystemInfo();

// `instanceApiVersion` is the version the VkInstance was created with; it gates
// whether vkGetPhysicalDeviceProperties2 may be used without the KHR extension.
DeviceInfo QueryDeviceInfo(const VulkanLoader& vk, VkPhysicalDevice physicalDevice, uint32_t instanceApiVersion);

std::string VulkanVersionToString(uint32_t version);
std::string VendorIdToString(uint32_t vendorID);
std::string DecodeDriverVersion(uint32_t vendorID, uint32_t driverVersion);
