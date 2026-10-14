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

#include "ui.h"

#include <android/log.h>

#include <algorithm>
#include <cmath>
#include <cstdio>

namespace {

constexpr const char* kLogTag = "VulkanProfilesChecker";

// Palette (dark theme)
constexpr uint32_t kBackground = MakeColor(0x12, 0x12, 0x14);
constexpr uint32_t kText = MakeColor(0xE6, 0xE6, 0xE6);
constexpr uint32_t kMuted = MakeColor(0x9A, 0x9A, 0xA0);
constexpr uint32_t kAccent = MakeColor(0x8A, 0xB4, 0xF8);
constexpr uint32_t kTitle = MakeColor(0xFF, 0xFF, 0xFF);
constexpr uint32_t kPass = MakeColor(0x4C, 0xAF, 0x50);
constexpr uint32_t kFail = MakeColor(0xEF, 0x53, 0x50);
constexpr uint32_t kWarn = MakeColor(0xFF, 0xB7, 0x4D);
constexpr uint32_t kSeparator = MakeColor(0x2C, 0x2C, 0x30);
constexpr uint32_t kDetail = MakeColor(0xCF, 0xCF, 0xD4);
constexpr uint32_t kScrollbar = MakeColor(0x55, 0x55, 0x5A);

// Type scale in dp
constexpr float kTitleDp = 20.0f;
constexpr float kHeadingDp = 16.0f;
constexpr float kBodyDp = 14.0f;
constexpr float kSmallDp = 12.0f;
constexpr float kMarginDp = 16.0f;
constexpr float kTapSlopDp = 8.0f;

std::string Hex(uint32_t value) {
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "0x%04X", value);
    return buffer;
}

}  // namespace

Ui::Ui(TextRenderer& text) : text_(text) {}

void Ui::SetWindow(ANativeWindow* window) {
    window_ = window;
    if (window_ != nullptr) {
        ANativeWindow_setBuffersGeometry(window_, 0, 0, WINDOW_FORMAT_RGBA_8888);
    }
    layoutDirty_ = true;
    dirty_ = true;
}

void Ui::SetContentRect(const ARect& rect) {
    contentRect_ = rect;
    layoutDirty_ = true;
    dirty_ = true;
}

void Ui::SetDensity(float density) {
    if (density > 0.5f && density < 10.0f) density_ = density;
    layoutDirty_ = true;
    dirty_ = true;
}

void Ui::SetStatus(const std::string& status) {
    status_ = status;
    layoutDirty_ = true;
    dirty_ = true;
}

void Ui::SetReport(std::shared_ptr<const CheckReport> report, const std::string& reportPath) {
    report_ = std::move(report);
    reportPath_ = reportPath;
    rows_.clear();
    if (report_) {
        for (size_t g = 0; g < report_->gpus.size(); ++g) {
            for (size_t p = 0; p < report_->gpus[g].profiles.size(); ++p) {
                rows_.push_back(Row{g, p, false});
            }
        }
    }
    scrollY_ = 0.0f;
    layoutDirty_ = true;
    dirty_ = true;
}

// ---------------------------------------------------------------------------
// Layout
// ---------------------------------------------------------------------------

void Ui::AddText(const std::string& text, float size, uint32_t color, float indent, int tapTarget) {
    const float maxWidth = static_cast<float>(layoutWidth_) - Dp(kMarginDp) * 2.0f - indent;
    const float rowHeight = text_.LineHeight(size);
    for (const std::string& wrapped : text_.Wrap(text, size, maxWidth)) {
        Line line;
        line.text = wrapped;
        line.color = color;
        line.size = size;
        line.indent = indent;
        line.height = rowHeight;
        line.tapTarget = tapTarget;
        lines_.push_back(std::move(line));
    }
}

void Ui::AddSpacer(float height) {
    Line line;
    line.height = height;
    lines_.push_back(std::move(line));
}

void Ui::AddSeparator() {
    Line line;
    line.height = Dp(12.0f);
    line.separator = true;
    lines_.push_back(std::move(line));
}

void Ui::AddBadge(const std::string& text, uint32_t color, float indent, int tapTarget) {
    Line line;
    line.text = text;
    line.color = kTitle;
    line.size = Dp(kSmallDp);
    line.indent = indent;
    line.height = text_.LineHeight(line.size) + Dp(8.0f);
    line.tapTarget = tapTarget;
    line.badge = true;
    line.badgeColor = color;
    lines_.push_back(std::move(line));
}

void Ui::Layout(int width) {
    lines_.clear();
    layoutWidth_ = width;

    const float title = Dp(kTitleDp), heading = Dp(kHeadingDp), body = Dp(kBodyDp), small = Dp(kSmallDp);
    const float indent1 = Dp(12.0f), indent2 = Dp(28.0f), indent3 = Dp(44.0f);

    // Keep clear of the status bar / display cutout.
    AddSpacer(static_cast<float>(std::max(contentRect_.top, 0)) + Dp(12.0f));
    AddText("Vulkan Profiles Checker", title, kTitle, 0.0f);
    AddText(AppBuildIdentityLine(), small, kMuted, 0.0f);
    AddSpacer(Dp(4.0f));

    if (!report_) {
        AddText(status_, body, kMuted, 0.0f);
        AddSpacer(Dp(24.0f));
        contentHeight_ = 0.0f;
        for (const Line& l : lines_) contentHeight_ += l.height;
        layoutDirty_ = false;
        return;
    }

    const CheckReport& r = *report_;

    // ---- System card ----
    {
        std::string device = r.system.manufacturer + " " + r.system.model;
        if (!r.system.device.empty()) device += " (" + r.system.device + ")";
        AddText(device, body, kText, 0.0f);
        std::string android = "Android " + r.system.androidVersion;
        if (!r.system.sdkLevel.empty()) android += " (API " + r.system.sdkLevel + ")";
        if (!r.system.securityPatch.empty()) android += ", patch " + r.system.securityPatch;
        AddText(android, small, kMuted, 0.0f);
        if (!r.system.socManufacturer.empty() || !r.system.socModel.empty()) {
            AddText("SoC: " + r.system.socManufacturer + " " + r.system.socModel, small, kMuted, 0.0f);
        }
        if (!r.system.hardwareVulkan.empty()) {
            AddText("ro.hardware.vulkan: " + r.system.hardwareVulkan, small, kMuted, 0.0f);
        }
        if (!r.system.gfxDriver0.empty()) AddText("ro.gfx.driver.0: " + r.system.gfxDriver0, small, kMuted, 0.0f);
        if (!r.system.gfxDriver1.empty()) AddText("ro.gfx.driver.1: " + r.system.gfxDriver1, small, kMuted, 0.0f);
    }
    AddSeparator();

    if (!r.ok) {
        AddText("Vulkan checks could not run", heading, kFail, 0.0f);
        AddSpacer(Dp(4.0f));
        AddText(r.error, body, kText, 0.0f);
        AddSeparator();
        AddText("Report: " + (reportPath_.empty() ? std::string("(not written)") : reportPath_), small, kMuted, 0.0f);
        contentHeight_ = 0.0f;
        for (const Line& l : lines_) contentHeight_ += l.height;
        layoutDirty_ = false;
        return;
    }

    AddText("Loader API version: " + r.instanceVersion + "   Vulkan-Headers: " + r.vulkanHeadersVersion, small, kMuted,
            0.0f);
    AddSeparator();

    int rowIndex = 0;
    for (size_t g = 0; g < r.gpus.size(); ++g) {
        const GpuReport& gpu = r.gpus[g];
        const DeviceInfo& d = gpu.info;

        // ---- GPU card ----
        AddText(d.deviceName.empty() ? "Unknown GPU" : d.deviceName, heading, kAccent, 0.0f);
        AddSpacer(Dp(2.0f));
        AddText(d.vendorName + " (" + Hex(d.vendorID) + "), device " + Hex(d.deviceID) + ", " + d.deviceType, small,
                kMuted, 0.0f);
        AddText("Vulkan API " + d.apiVersion, body, kText, 0.0f);
        AddText("Driver version: " + d.driverVersion, body, kText, 0.0f);
        if (d.hasDriverProperties) {
            if (!d.driverName.empty()) AddText("Driver: " + d.driverName, body, kText, 0.0f);
            if (!d.driverInfo.empty()) AddText(d.driverInfo, small, kMuted, indent1);
            std::string ids;
            if (!d.driverID.empty()) ids += "ID: " + d.driverID;
            if (!d.conformanceVersion.empty()) ids += (ids.empty() ? "" : "   ") + ("Conformance " + d.conformanceVersion);
            if (!ids.empty()) AddText(ids, small, kMuted, indent1);
        } else {
            AddText("VkPhysicalDeviceDriverProperties not available (needs Vulkan 1.2 or VK_KHR_driver_properties)",
                    small, kMuted, indent1);
        }
        AddSeparator();

        // ---- Profiles ----
        AddText("Profiles", heading, kTitle, 0.0f);
        AddText("Tap a profile to show or hide details.", small, kMuted, 0.0f);
        AddSpacer(Dp(8.0f));

        for (size_t p = 0; p < gpu.profiles.size(); ++p, ++rowIndex) {
            const ProfileResult& pr = gpu.profiles[p];
            const Row& row = rows_[static_cast<size_t>(rowIndex)];
            const int tap = rowIndex;

            AddText(pr.name, body, kText, 0.0f, tap);
            char meta[128];
            snprintf(meta, sizeof(meta), "spec v%u, requires Vulkan %s%s", pr.specVersion,
                     VulkanVersionToString(pr.minApiVersion).c_str(), pr.hasMultipleVariants ? ", multiple variants" : "");
            AddText(meta, small, kMuted, indent1, tap);

            if (pr.queryFailed) {
                AddBadge("ERROR", kWarn, indent1, tap);
                AddText(pr.error, small, kDetail, indent1, tap);
            } else if (pr.Supported()) {
                AddBadge("SUPPORTED", kPass, indent1, tap);
                if (row.expanded) {
                    if (!pr.blocks.empty()) {
                        AddText("Validated capability blocks:", small, kMuted, indent2);
                        for (const std::string& b : pr.blocks) AddText("- " + b, small, kDetail, indent3);
                    } else {
                        AddText("All profile requirements are met.", small, kDetail, indent2);
                    }
                }
            } else {
                char summary[96];
                snprintf(summary, sizeof(summary), "NOT SUPPORTED  (%zu unmet)", pr.UnmetCount());
                AddBadge(summary, kFail, indent1, tap);

                if (row.expanded) {
                    if (!pr.instanceSupported) {
                        AddText("Instance-level requirements not met", small, kWarn, indent2);
                    }
                    if (!pr.blocks.empty()) {
                        AddText("Failing capability blocks:", small, kMuted, indent2);
                        for (const std::string& b : pr.blocks) AddText("- " + b, small, kDetail, indent3);
                        AddSpacer(Dp(4.0f));
                    }

                    auto addGroup = [&](const char* title, const std::vector<UnmetRequirement>& unmet) {
                        if (unmet.empty()) return;
                        AddText(title, small, kMuted, indent2);
                        RequirementKind current = RequirementKind::Other;
                        bool first = true;
                        for (const UnmetRequirement& req : unmet) {
                            if (first || req.kind != current) {
                                AddSpacer(Dp(2.0f));
                                AddText(RequirementKindName(req.kind), small, kAccent, indent2);
                                current = req.kind;
                                first = false;
                            }
                            AddText("- " + req.detail, small, kDetail, indent3);
                        }
                        AddSpacer(Dp(4.0f));
                    };
                    addGroup("Unmet instance requirements:", pr.instanceUnmet);
                    addGroup("Unmet device requirements:", pr.deviceUnmet);
                    if (pr.UnmetCount() == 0) {
                        AddText("No item-level detail was reported for this failure (e.g. queue family requirements).",
                                small, kDetail, indent2);
                    }
                    if (pr.hasMultipleVariants) {
                        AddText("Note: this profile has multiple variants; the list covers every variant checked.",
                                small, kMuted, indent2);
                    }
                }
            }
            AddSpacer(Dp(10.0f));
        }
        AddSeparator();
    }

    AddText("Report: " + (reportPath_.empty() ? std::string("(not written)") : reportPath_), small, kMuted, 0.0f);
    AddText("adb logcat -s VulkanProfilesChecker", small, kMuted, 0.0f);
    // Bottom padding; also keeps the last lines clear of the navigation bar.
    AddSpacer(Dp(48.0f));

    contentHeight_ = 0.0f;
    for (const Line& l : lines_) contentHeight_ += l.height;
    layoutDirty_ = false;
}

void Ui::ClampScroll() {
    const float maxScroll = std::max(0.0f, contentHeight_ - viewHeight_);
    scrollY_ = std::min(std::max(scrollY_, 0.0f), maxScroll);
}

// ---------------------------------------------------------------------------
// Input
// ---------------------------------------------------------------------------

void Ui::OnTouchDown(float /*x*/, float y) {
    touching_ = true;
    dragged_ = false;
    touchStartY_ = y;
    touchLastY_ = y;
}

void Ui::OnTouchMove(float /*x*/, float y) {
    if (!touching_) return;
    const float dy = y - touchLastY_;
    touchLastY_ = y;
    if (std::fabs(y - touchStartY_) > Dp(kTapSlopDp)) dragged_ = true;
    if (dragged_) {
        scrollY_ -= dy;
        ClampScroll();
        dirty_ = true;
    }
}

void Ui::OnTouchUp(float /*x*/, float y) {
    if (!touching_) return;
    touching_ = false;
    if (dragged_) return;

    // Tap: find the line under the finger.
    float cursor = -scrollY_;
    for (const Line& line : lines_) {
        if (y >= cursor && y < cursor + line.height) {
            if (line.tapTarget >= 0 && static_cast<size_t>(line.tapTarget) < rows_.size()) {
                rows_[static_cast<size_t>(line.tapTarget)].expanded =
                    !rows_[static_cast<size_t>(line.tapTarget)].expanded;
                layoutDirty_ = true;
                dirty_ = true;
            }
            return;
        }
        cursor += line.height;
    }
}

void Ui::OnTouchCancel() {
    touching_ = false;
    dragged_ = false;
}

// ---------------------------------------------------------------------------
// Rendering
// ---------------------------------------------------------------------------

void Ui::Render() {
    dirty_ = false;
    if (window_ == nullptr) return;

    ANativeWindow_Buffer buffer{};
    if (ANativeWindow_lock(window_, &buffer, nullptr) != 0) {
        __android_log_print(ANDROID_LOG_WARN, kLogTag, "ANativeWindow_lock failed");
        return;
    }
    if (buffer.format != WINDOW_FORMAT_RGBA_8888 && buffer.format != WINDOW_FORMAT_RGBX_8888) {
        ANativeWindow_unlockAndPost(window_);
        __android_log_print(ANDROID_LOG_WARN, kLogTag, "Unexpected window format %d", buffer.format);
        return;
    }

    Framebuffer fb;
    fb.pixels = static_cast<uint32_t*>(buffer.bits);
    fb.width = buffer.width;
    fb.height = buffer.height;
    fb.stride = buffer.stride;

    if (layoutDirty_ || layoutWidth_ != fb.width) Layout(fb.width);
    viewHeight_ = static_cast<float>(fb.height);
    ClampScroll();

    fb.Clear(kBackground);

    const float margin = Dp(kMarginDp);
    float cursor = -scrollY_;
    for (const Line& line : lines_) {
        const float top = cursor;
        cursor += line.height;
        if (cursor < 0.0f) continue;
        if (top > fb.height) break;

        if (line.separator) {
            fb.FillRect(static_cast<int>(margin), static_cast<int>(std::lround(top + line.height * 0.5f)),
                        fb.width - static_cast<int>(margin) * 2, std::max(1, static_cast<int>(Dp(1.0f))), kSeparator);
            continue;
        }
        if (line.text.empty()) continue;

        const float x = margin + line.indent;
        if (line.badge) {
            const float padX = Dp(8.0f), padY = Dp(4.0f);
            const float textWidth = text_.MeasureWidth(line.text, line.size);
            const float textHeight = text_.LineHeight(line.size);
            fb.FillRect(static_cast<int>(x), static_cast<int>(std::lround(top + padY * 0.5f)),
                        static_cast<int>(std::lround(textWidth + padX * 2.0f)),
                        static_cast<int>(std::lround(textHeight + padY)), line.badgeColor);
            text_.Draw(fb, x + padX, top + padY * 0.5f + padY * 0.5f + text_.Ascent(line.size), line.text, line.size,
                       line.color);
        } else {
            text_.Draw(fb, x, top + text_.Ascent(line.size) + (line.height - text_.LineHeight(line.size)) * 0.5f,
                       line.text, line.size, line.color);
        }
    }

    // Mask the areas covered by the system bars so scrolled content disappears
    // beneath them instead of showing through.
    if (contentRect_.top > 0) fb.FillRect(0, 0, fb.width, contentRect_.top, kBackground);
    if (contentRect_.bottom > contentRect_.top && contentRect_.bottom < fb.height) {
        fb.FillRect(0, contentRect_.bottom, fb.width, fb.height - contentRect_.bottom, kBackground);
    }

    // Scrollbar
    if (contentHeight_ > viewHeight_ && viewHeight_ > 0.0f) {
        const float ratio = viewHeight_ / contentHeight_;
        const int barHeight = std::max(static_cast<int>(Dp(24.0f)), static_cast<int>(viewHeight_ * ratio));
        const int barY = static_cast<int>((scrollY_ / contentHeight_) * viewHeight_);
        const int barWidth = std::max(2, static_cast<int>(Dp(3.0f)));
        fb.FillRect(fb.width - barWidth - static_cast<int>(Dp(2.0f)), barY, barWidth, barHeight, kScrollbar);
    }

    ANativeWindow_unlockAndPost(window_);
}
