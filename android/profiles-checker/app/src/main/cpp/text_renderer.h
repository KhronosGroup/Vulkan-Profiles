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
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Pixel format: RGBA8888 as stored in an ANativeWindow buffer (R in the low byte).
constexpr uint32_t MakeColor(uint8_t r, uint8_t g, uint8_t b) {
    return 0xFF000000u | (static_cast<uint32_t>(b) << 16) | (static_cast<uint32_t>(g) << 8) | r;
}

// A CPU framebuffer (typically a locked ANativeWindow buffer).
struct Framebuffer {
    uint32_t* pixels = nullptr;
    int width = 0;
    int height = 0;
    int stride = 0;  // in pixels

    void Clear(uint32_t color) const;
    void FillRect(int x, int y, int w, int h, uint32_t color) const;
};

// Rasterizes text with stb_truetype using a system TrueType font.
class TextRenderer {
   public:
    TextRenderer();
    ~TextRenderer();

    TextRenderer(const TextRenderer&) = delete;
    TextRenderer& operator=(const TextRenderer&) = delete;

    // Tries the well-known Android system fonts. Returns false if none could be loaded.
    bool LoadSystemFont();
    bool IsLoaded() const { return loaded_; }

    // Metrics for a given pixel size.
    float LineHeight(float pixelSize) const;
    float Ascent(float pixelSize) const;
    float MeasureWidth(const std::string& utf8, float pixelSize);

    // Draws text with its baseline at `baselineY`. Returns the pen x after drawing.
    float Draw(const Framebuffer& fb, float x, float baselineY, const std::string& utf8, float pixelSize,
               uint32_t color);

    // Word-wraps `utf8` to `maxWidth`. Long tokens are broken at character boundaries.
    std::vector<std::string> Wrap(const std::string& utf8, float pixelSize, float maxWidth);

   private:
    struct Glyph {
        int width = 0;
        int height = 0;
        int xoff = 0;
        int yoff = 0;
        float advance = 0.0f;
        std::vector<uint8_t> coverage;
    };

    struct FontImpl;

    const Glyph& GetGlyph(uint32_t codepoint, float pixelSize);
    float ScaleFor(float pixelSize) const;

    std::unique_ptr<FontImpl> font_;
    std::vector<uint8_t> fontData_;
    bool loaded_ = false;
    int ascent_ = 0;
    int descent_ = 0;
    int lineGap_ = 0;
    std::unordered_map<uint64_t, Glyph> cache_;
};

// Decodes one UTF-8 code point starting at `pos`; advances `pos`. Invalid
// sequences decode to U+FFFD and advance by one byte.
uint32_t DecodeUtf8(const std::string& s, size_t& pos);
