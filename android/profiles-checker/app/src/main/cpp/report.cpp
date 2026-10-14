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

#include "report.h"

#include <android/log.h>

#include <cstdio>
#include <sstream>

#include "profile_checker.h"

namespace {

constexpr const char* kLogTag = "VulkanProfilesChecker";
constexpr const char* kReportFileName = "report.txt";

void AppendKeyValue(std::ostringstream& out, const char* key, const std::string& value) {
    if (value.empty()) return;
    out << "  " << key << ": " << value << "\n";
}

std::string Hex(uint32_t value) {
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "0x%04X", value);
    return buffer;
}

void AppendUnmet(std::ostringstream& out, const char* title, const std::vector<UnmetRequirement>& unmet) {
    if (unmet.empty()) return;
    out << "    " << title << " (" << unmet.size() << "):\n";
    RequirementKind current = RequirementKind::Other;
    bool first = true;
    for (const UnmetRequirement& req : unmet) {
        if (first || req.kind != current) {
            out << "      [" << RequirementKindName(req.kind) << "]\n";
            current = req.kind;
            first = false;
        }
        out << "        - " << req.detail << "\n";
    }
}

}  // namespace

std::string BuildTextReport(const CheckReport& report) {
    std::ostringstream out;
    out << "Vulkan Profiles Checker report\n";
    out << "==============================\n";
    out << "App version: " << report.appVersion << "\n";
    out << "Build time: " << report.buildTime << "\n";
    out << "Git commit: " << report.gitCommit << "\n";
    out << "App ABI: " << report.appAbi << "\n";
    out << "Vulkan-Headers: " << report.vulkanHeadersVersion << "\n\n";

    out << "System\n";
    AppendKeyValue(out, "Manufacturer", report.system.manufacturer);
    AppendKeyValue(out, "Model", report.system.model);
    AppendKeyValue(out, "Device", report.system.device);
    if (!report.system.androidVersion.empty()) {
        out << "  Android: " << report.system.androidVersion;
        if (!report.system.sdkLevel.empty()) out << " (API " << report.system.sdkLevel << ")";
        out << "\n";
    }
    AppendKeyValue(out, "Security patch", report.system.securityPatch);
    if (!report.system.socManufacturer.empty() || !report.system.socModel.empty()) {
        out << "  SoC: " << report.system.socManufacturer << " " << report.system.socModel << "\n";
    }
    AppendKeyValue(out, "Device ABI", report.system.abi);
    AppendKeyValue(out, "ro.hardware.vulkan", report.system.hardwareVulkan);
    AppendKeyValue(out, "ro.gfx.driver.0", report.system.gfxDriver0);
    AppendKeyValue(out, "ro.gfx.driver.1", report.system.gfxDriver1);
    out << "\n";

    if (!report.ok) {
        out << "ERROR: " << report.error << "\n";
        return out.str();
    }

    out << "Vulkan instance\n";
    out << "  Loader API version: " << report.instanceVersion << "\n";
    if (!report.enabledInstanceExtensions.empty()) {
        out << "  Enabled instance extensions:";
        for (const std::string& ext : report.enabledInstanceExtensions) out << " " << ext;
        out << "\n";
    }
    out << "\n";

    for (size_t g = 0; g < report.gpus.size(); ++g) {
        const GpuReport& gpu = report.gpus[g];
        const DeviceInfo& d = gpu.info;
        out << "GPU " << g << ": " << d.deviceName << "\n";
        out << "  Type: " << d.deviceType << "\n";
        out << "  Vendor: " << d.vendorName << " (" << Hex(d.vendorID) << "), device ID " << Hex(d.deviceID) << "\n";
        out << "  Vulkan API version: " << d.apiVersion << "\n";
        out << "  Driver version: " << d.driverVersion << "\n";
        if (d.hasDriverProperties) {
            AppendKeyValue(out, "Driver name", d.driverName);
            AppendKeyValue(out, "Driver info", d.driverInfo);
            AppendKeyValue(out, "Driver ID", d.driverID);
            AppendKeyValue(out, "Conformance version", d.conformanceVersion);
        }
        out << "  Device extensions: " << d.deviceExtensionCount << "\n\n";

        out << "  Profiles\n";
        for (const ProfileResult& pr : gpu.profiles) {
            const char* status = pr.queryFailed ? "ERROR" : (pr.Supported() ? "SUPPORTED" : "NOT SUPPORTED");
            out << "  - " << pr.name << " (v" << pr.specVersion << ", min Vulkan "
                << VulkanVersionToString(pr.minApiVersion) << "): " << status << "\n";
            if (pr.queryFailed) {
                out << "    " << pr.error << "\n";
                continue;
            }
            if (!pr.Supported()) {
                if (!pr.instanceSupported) out << "    Instance-level requirements not met\n";
                if (!pr.blocks.empty()) {
                    out << "    Failing capability blocks:";
                    for (size_t i = 0; i < pr.blocks.size(); ++i) out << (i == 0 ? " " : ", ") << pr.blocks[i];
                    out << "\n";
                }
                AppendUnmet(out, "Unmet instance requirements", pr.instanceUnmet);
                AppendUnmet(out, "Unmet device requirements", pr.deviceUnmet);
                if (pr.UnmetCount() == 0) {
                    out << "    No item-level detail reported (e.g. queue family requirements)\n";
                }
            } else if (!pr.blocks.empty() && pr.hasMultipleVariants) {
                out << "    Validated with blocks:";
                for (size_t i = 0; i < pr.blocks.size(); ++i) out << (i == 0 ? " " : ", ") << pr.blocks[i];
                out << "\n";
            }
        }
        out << "\n";
    }
    return out.str();
}

void LogTextReport(const std::string& text) {
    // logcat truncates long messages; emit one line per log entry.
    size_t start = 0;
    while (start < text.size()) {
        size_t end = text.find('\n', start);
        if (end == std::string::npos) end = text.size();
        const std::string line = text.substr(start, end - start);
        __android_log_print(ANDROID_LOG_INFO, kLogTag, "%s", line.empty() ? " " : line.c_str());
        start = end + 1;
    }
}

std::string WriteTextReport(const std::string& text, const std::string& internalDataPath) {
    if (internalDataPath.empty()) return std::string();

    // The destination is always the app's private files directory plus a fixed
    // file name; no externally controlled path components are involved.
    const std::string path = internalDataPath + "/" + kReportFileName;
    FILE* file = fopen(path.c_str(), "wb");
    if (file == nullptr) {
        __android_log_print(ANDROID_LOG_WARN, kLogTag, "Could not write %s", path.c_str());
        return std::string();
    }
    const size_t written = fwrite(text.data(), 1, text.size(), file);
    fclose(file);
    if (written != text.size()) return std::string();
    return path;
}
