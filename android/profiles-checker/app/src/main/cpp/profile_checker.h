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

#include <cstdint>
#include <string>
#include <vector>

#include "device_info.h"

// Category of an unmet requirement reported by the debug profiles library.
enum class RequirementKind {
    ApiVersion,
    Extension,
    Feature,
    Property,
    Format,
    VideoCapability,
    Other,
};

const char* RequirementKindName(RequirementKind kind);

struct UnmetRequirement {
    RequirementKind kind = RequirementKind::Other;
    std::string detail;  // e.g. "VkPhysicalDeviceVulkan12Features::shaderInt8 == VK_TRUE"
};

struct ProfileResult {
    std::string name;
    uint32_t specVersion = 0;
    uint32_t minApiVersion = 0;
    bool hasMultipleVariants = false;

    bool instanceSupported = false;
    bool deviceSupported = false;
    bool queryFailed = false;  // the profiles library returned an error; see `error`
    std::string error;

    // When unsupported: names of the capability blocks that failed.
    // When supported: names of the capability blocks used to validate the profile.
    std::vector<std::string> blocks;

    // Instance-level problems (API version / instance extensions).
    std::vector<UnmetRequirement> instanceUnmet;
    // Device-level problems. For multi-variant profiles this may include
    // conditions from variants that were not needed; it is only meaningful when
    // deviceSupported == false.
    std::vector<UnmetRequirement> deviceUnmet;

    bool Supported() const { return !queryFailed && instanceSupported && deviceSupported; }
    size_t UnmetCount() const { return instanceUnmet.size() + deviceUnmet.size(); }
};

struct GpuReport {
    DeviceInfo info;
    std::vector<ProfileResult> profiles;
};

struct CheckReport {
    bool ok = false;         // false when checks could not run at all
    std::string error;       // populated when ok == false
    std::string vulkanHeadersVersion;
    std::string instanceVersion;  // loader/instance API version
    uint32_t instanceVersionRaw = 0;
    std::vector<std::string> enabledInstanceExtensions;
    SystemInfo system;
    std::vector<GpuReport> gpus;
};

// Runs all profile checks. Safe to call from a worker thread.
CheckReport RunProfileChecks();
