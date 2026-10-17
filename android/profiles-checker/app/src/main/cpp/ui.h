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

#include <android/native_window.h>
#include <android/rect.h>

#include <memory>
#include <string>
#include <vector>

#include "profile_checker.h"
#include "text_renderer.h"

// Software-rendered, scrollable report view.
//
// All drawing happens on the main thread into a locked ANativeWindow buffer.
// The view is re-laid out only when the content, window size or expansion
// state changes.
class Ui {
   public:
    explicit Ui(TextRenderer& text);

    void SetWindow(ANativeWindow* window);
    void SetContentRect(const ARect& rect);
    void SetDensity(float density);  // pixels per dp

    void SetStatus(const std::string& status);  // shown while checks are running
    void SetReport(std::shared_ptr<const CheckReport> report, const std::string& reportPath);

    // Touch handling (coordinates in window pixels). Returns true if handled.
    void OnTouchDown(float x, float y);
    void OnTouchMove(float x, float y);
    void OnTouchUp(float x, float y);
    void OnTouchCancel();

    bool NeedsRedraw() const { return dirty_; }
    void Invalidate() { dirty_ = true; }
    void Render();

   private:
    struct Line {
        std::string text;
        uint32_t color = 0;
        float size = 0.0f;   // px
        float indent = 0.0f; // px
        float height = 0.0f; // px (row height)
        int tapTarget = -1;  // index into rows_ that toggles expansion, or -1
        bool separator = false;
        bool badge = false;  // draw `text` as a filled pill
        uint32_t badgeColor = 0;
    };

    struct Row {
        size_t gpu = 0;
        size_t profile = 0;
        bool expanded = false;
    };

    void Layout(int width);
    void AddText(const std::string& text, float size, uint32_t color, float indent, int tapTarget = -1);
    void AddSpacer(float height);
    void AddSeparator();
    void AddBadge(const std::string& text, uint32_t color, float indent, int tapTarget);
    float Dp(float dp) const { return dp * density_; }
    void ClampScroll();

    TextRenderer& text_;
    ANativeWindow* window_ = nullptr;
    ARect contentRect_{0, 0, 0, 0};
    float density_ = 2.5f;

    std::string status_ = "Running Vulkan profile checks...";
    std::shared_ptr<const CheckReport> report_;
    std::string reportPath_;
    std::vector<Row> rows_;

    std::vector<Line> lines_;
    float contentHeight_ = 0.0f;
    int layoutWidth_ = 0;
    bool layoutDirty_ = true;
    bool dirty_ = true;

    float scrollY_ = 0.0f;
    float viewHeight_ = 0.0f;

    // Touch state
    bool touching_ = false;
    bool dragged_ = false;
    float touchStartY_ = 0.0f;
    float touchLastY_ = 0.0f;
};
