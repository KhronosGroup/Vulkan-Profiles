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

#include "text_renderer.h"

#include <android/log.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "stb_truetype.h"

namespace {

constexpr const char* kLogTag = "VulkanProfilesChecker";

// Candidate system fonts, in order of preference. Only fixed, well-known system
// paths are opened; nothing user-controlled is involved.
const char* const kFontCandidates[] = {
    "/system/fonts/Roboto-Regular.ttf",
    "/system/fonts/RobotoStatic-Regular.ttf",
    "/system/fonts/Roboto-Medium.ttf",
    "/system/fonts/DroidSans.ttf",
    "/system/fonts/NotoSans-Regular.ttf",
};

// Refuse absurdly large files to keep memory bounded (Roboto is ~1-2 MB).
constexpr long kMaxFontBytes = 32L * 1024L * 1024L;

bool ReadFile(const char* path, std::vector<uint8_t>& out) {
    FILE* file = fopen(path, "rb");
    if (file == nullptr) return false;
    bool ok = false;
    if (fseek(file, 0, SEEK_END) == 0) {
        const long size = ftell(file);
        if (size > 0 && size <= kMaxFontBytes && fseek(file, 0, SEEK_SET) == 0) {
            out.resize(static_cast<size_t>(size));
            ok = fread(out.data(), 1, out.size(), file) == out.size();
        }
    }
    fclose(file);
    if (!ok) out.clear();
    return ok;
}

inline void BlendPixel(uint32_t& dst, uint32_t color, uint32_t alpha) {
    if (alpha == 0) return;
    if (alpha >= 255) {
        dst = color;
        return;
    }
    const uint32_t inv = 255u - alpha;
    const uint32_t dr = dst & 0xFFu, dg = (dst >> 8) & 0xFFu, db = (dst >> 16) & 0xFFu;
    const uint32_t sr = color & 0xFFu, sg = (color >> 8) & 0xFFu, sb = (color >> 16) & 0xFFu;
    const uint32_t r = (sr * alpha + dr * inv) / 255u;
    const uint32_t g = (sg * alpha + dg * inv) / 255u;
    const uint32_t b = (sb * alpha + db * inv) / 255u;
    dst = 0xFF000000u | (b << 16) | (g << 8) | r;
}

}  // namespace

// ---------------------------------------------------------------------------
// Framebuffer
// ---------------------------------------------------------------------------

void Framebuffer::Clear(uint32_t color) const {
    if (pixels == nullptr) return;
    for (int y = 0; y < height; ++y) {
        uint32_t* row = pixels + static_cast<ptrdiff_t>(y) * stride;
        std::fill(row, row + width, color);
    }
}

void Framebuffer::FillRect(int x, int y, int w, int h, uint32_t color) const {
    if (pixels == nullptr) return;
    const int x0 = std::max(0, x), y0 = std::max(0, y);
    const int x1 = std::min(width, x + w), y1 = std::min(height, y + h);
    for (int yy = y0; yy < y1; ++yy) {
        uint32_t* row = pixels + static_cast<ptrdiff_t>(yy) * stride;
        std::fill(row + x0, row + x1, color);
    }
}

// ---------------------------------------------------------------------------
// UTF-8
// ---------------------------------------------------------------------------

uint32_t DecodeUtf8(const std::string& s, size_t& pos) {
    const size_t n = s.size();
    if (pos >= n) return 0;
    const unsigned char c0 = static_cast<unsigned char>(s[pos]);
    if (c0 < 0x80) {
        ++pos;
        return c0;
    }
    int extra = 0;
    uint32_t cp = 0;
    if ((c0 & 0xE0) == 0xC0) {
        extra = 1;
        cp = c0 & 0x1F;
    } else if ((c0 & 0xF0) == 0xE0) {
        extra = 2;
        cp = c0 & 0x0F;
    } else if ((c0 & 0xF8) == 0xF0) {
        extra = 3;
        cp = c0 & 0x07;
    } else {
        ++pos;
        return 0xFFFD;
    }
    for (int i = 1; i <= extra; ++i) {
        if (pos + i >= n) {
            ++pos;
            return 0xFFFD;
        }
        const unsigned char c = static_cast<unsigned char>(s[pos + i]);
        if ((c & 0xC0) != 0x80) {
            ++pos;
            return 0xFFFD;
        }
        cp = (cp << 6) | (c & 0x3F);
    }
    pos += static_cast<size_t>(extra) + 1;
    return cp;
}

// ---------------------------------------------------------------------------
// TextRenderer
// ---------------------------------------------------------------------------

struct TextRenderer::FontImpl {
    stbtt_fontinfo info{};
};

TextRenderer::TextRenderer() : font_(new FontImpl()) {}
TextRenderer::~TextRenderer() = default;

bool TextRenderer::LoadSystemFont() {
    for (const char* path : kFontCandidates) {
        std::vector<uint8_t> data;
        if (!ReadFile(path, data)) continue;
        const int offset = stbtt_GetFontOffsetForIndex(data.data(), 0);
        if (offset < 0) continue;
        stbtt_fontinfo info{};
        if (!stbtt_InitFont(&info, data.data(), offset)) continue;

        fontData_ = std::move(data);
        font_->info = info;
        // stbtt keeps a pointer to the data; re-init against the moved buffer.
        if (!stbtt_InitFont(&font_->info, fontData_.data(), offset)) {
            fontData_.clear();
            continue;
        }
        stbtt_GetFontVMetrics(&font_->info, &ascent_, &descent_, &lineGap_);
        loaded_ = true;
        cache_.clear();
        __android_log_print(ANDROID_LOG_INFO, kLogTag, "Using font %s", path);
        return true;
    }
    __android_log_print(ANDROID_LOG_ERROR, kLogTag, "No system TrueType font could be loaded");
    return false;
}

float TextRenderer::ScaleFor(float pixelSize) const {
    if (!loaded_) return 0.0f;
    return stbtt_ScaleForPixelHeight(&font_->info, pixelSize);
}

float TextRenderer::LineHeight(float pixelSize) const {
    if (!loaded_) return pixelSize * 1.3f;
    const float scale = ScaleFor(pixelSize);
    return (ascent_ - descent_ + lineGap_) * scale * 1.15f;
}

float TextRenderer::Ascent(float pixelSize) const {
    if (!loaded_) return pixelSize;
    return ascent_ * ScaleFor(pixelSize);
}

const TextRenderer::Glyph& TextRenderer::GetGlyph(uint32_t codepoint, float pixelSize) {
    // Quantize size to 1/4 px so the cache stays small.
    const uint32_t sizeKey = static_cast<uint32_t>(std::lround(pixelSize * 4.0f));
    const uint64_t key = (static_cast<uint64_t>(sizeKey) << 32) | codepoint;
    auto it = cache_.find(key);
    if (it != cache_.end()) return it->second;

    Glyph glyph;
    const float scale = ScaleFor(pixelSize);
    int glyphIndex = stbtt_FindGlyphIndex(&font_->info, static_cast<int>(codepoint));
    if (glyphIndex == 0 && codepoint != ' ') {
        // Missing glyph: fall back to '?' so the text stays readable.
        glyphIndex = stbtt_FindGlyphIndex(&font_->info, '?');
    }

    int advance = 0, lsb = 0;
    stbtt_GetGlyphHMetrics(&font_->info, glyphIndex, &advance, &lsb);
    glyph.advance = advance * scale;

    int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
    stbtt_GetGlyphBitmapBox(&font_->info, glyphIndex, scale, scale, &x0, &y0, &x1, &y1);
    glyph.width = std::max(0, x1 - x0);
    glyph.height = std::max(0, y1 - y0);
    glyph.xoff = x0;
    glyph.yoff = y0;
    if (glyph.width > 0 && glyph.height > 0 && glyph.width < 4096 && glyph.height < 4096) {
        glyph.coverage.assign(static_cast<size_t>(glyph.width) * glyph.height, 0);
        stbtt_MakeGlyphBitmap(&font_->info, glyph.coverage.data(), glyph.width, glyph.height, glyph.width, scale,
                              scale, glyphIndex);
    }

    // Keep the cache bounded.
    if (cache_.size() > 4096) cache_.clear();
    return cache_.emplace(key, std::move(glyph)).first->second;
}

float TextRenderer::MeasureWidth(const std::string& utf8, float pixelSize) {
    if (!loaded_) return static_cast<float>(utf8.size()) * pixelSize * 0.6f;
    float x = 0.0f;
    size_t pos = 0;
    while (pos < utf8.size()) {
        const uint32_t cp = DecodeUtf8(utf8, pos);
        x += GetGlyph(cp, pixelSize).advance;
    }
    return x;
}

float TextRenderer::Draw(const Framebuffer& fb, float x, float baselineY, const std::string& utf8, float pixelSize,
                         uint32_t color) {
    if (!loaded_ || fb.pixels == nullptr) return x;
    size_t pos = 0;
    while (pos < utf8.size()) {
        const uint32_t cp = DecodeUtf8(utf8, pos);
        const Glyph& glyph = GetGlyph(cp, pixelSize);
        if (!glyph.coverage.empty()) {
            const int gx = static_cast<int>(std::lround(x)) + glyph.xoff;
            const int gy = static_cast<int>(std::lround(baselineY)) + glyph.yoff;
            for (int row = 0; row < glyph.height; ++row) {
                const int py = gy + row;
                if (py < 0 || py >= fb.height) continue;
                uint32_t* dstRow = fb.pixels + static_cast<ptrdiff_t>(py) * fb.stride;
                const uint8_t* src = glyph.coverage.data() + static_cast<size_t>(row) * glyph.width;
                for (int col = 0; col < glyph.width; ++col) {
                    const int px = gx + col;
                    if (px < 0 || px >= fb.width) continue;
                    BlendPixel(dstRow[px], color, src[col]);
                }
            }
        }
        x += glyph.advance;
    }
    return x;
}

std::vector<std::string> TextRenderer::Wrap(const std::string& utf8, float pixelSize, float maxWidth) {
    std::vector<std::string> lines;
    if (utf8.empty()) {
        lines.emplace_back();
        return lines;
    }
    if (maxWidth <= 0.0f) {
        lines.push_back(utf8);
        return lines;
    }

    // Split into tokens on spaces; also allow breaking after common separators
    // found in Vulkan identifiers so very long conditions wrap gracefully.
    std::vector<std::string> tokens;
    std::string current;
    for (size_t i = 0; i < utf8.size(); ++i) {
        const char c = utf8[i];
        current.push_back(c);
        const bool breakAfter = (c == ' ') || (c == '|') || (c == ',') || (c == '_') || (c == ':') || (c == '.');
        if (breakAfter) {
            tokens.push_back(current);
            current.clear();
        }
    }
    if (!current.empty()) tokens.push_back(current);

    std::string line;
    float lineWidth = 0.0f;
    for (const std::string& token : tokens) {
        float tokenWidth = MeasureWidth(token, pixelSize);
        if (lineWidth + tokenWidth <= maxWidth || line.empty()) {
            if (tokenWidth > maxWidth && line.empty()) {
                // Token longer than a line: break at character boundaries.
                size_t pos = 0;
                std::string piece;
                float pieceWidth = 0.0f;
                while (pos < token.size()) {
                    const size_t start = pos;
                    const uint32_t cp = DecodeUtf8(token, pos);
                    const float w = GetGlyph(cp, pixelSize).advance;
                    if (pieceWidth + w > maxWidth && !piece.empty()) {
                        lines.push_back(piece);
                        piece.clear();
                        pieceWidth = 0.0f;
                    }
                    piece.append(token, start, pos - start);
                    pieceWidth += w;
                }
                line = piece;
                lineWidth = pieceWidth;
                continue;
            }
            line += token;
            lineWidth += tokenWidth;
        } else {
            // Trim trailing space before committing the line.
            while (!line.empty() && line.back() == ' ') line.pop_back();
            lines.push_back(line);
            line = token;
            lineWidth = tokenWidth;
        }
    }
    while (!line.empty() && line.back() == ' ') line.pop_back();
    if (!line.empty() || lines.empty()) lines.push_back(line);
    return lines;
}
