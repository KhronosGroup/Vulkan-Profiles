#include <cstdio>
#include <cstdarg>
#include <cassert>
#include <cstring>
#include <string>

#define VP_INLINE inline
#include "source_api.h"
#include "source_data.h"
#include "source_util.h"
#include "source_impl.h"

static const char *TEXT_COPYRIGHT = R"(
/*
 * Copyright (c) 2021-2022 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License")
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
 *
 * Authors:
 * - Christophe Riccio <christophe@lunarg.com>
 */
)";

static const char *TEXT_IFNDEF = R"(
#ifndef VULKAN_PROFILES_
#define VULKAN_PROFILES_ 1

)";

static const char *TEXT_ENDIF = R"(
#endif//VULKAN_PROFILES_
)";

static const char *TEXT_CPP_BEGIN = R"(
#ifdef __cplusplus
    extern "C" {
#endif
)";

static const char *TEXT_CPP_END = R"(
#ifdef __cplusplus
}
#endif
)";

std::string format(const char *message, ...) {
    std::size_t const STRING_BUFFER(4096);

    assert(message != nullptr);
    assert(strlen(message) >= 0 && strlen(message) < STRING_BUFFER);

    char buffer[STRING_BUFFER];
    va_list list;

    va_start(list, message);
    vsprintf(buffer, message, list);
    va_end(list);

    return buffer;
}

std::string fetch(const char* path) {
    const std::string full_path = format("%s/%s", VULKAN_INCLUDE_DIR, path);
    FILE *file = std::fopen(full_path.c_str(), "r");
    if (file == nullptr) {
        return "";
    }

    std::fseek(file, 0, SEEK_END);
    std::size_t size = std::ftell(file);
    std::fseek(file, 0, SEEK_SET);  

    std::string buffer;
    buffer.reserve(size);

    char c;
    while ((c = fgetc(file)) != EOF) {
        buffer.push_back(c);
    }

    std::fread(&buffer[0], sizeof(char), size, file);
    std::fclose(file);

    return buffer;
}

int store(const char *path, std::string &buffer) {
    const std::string full_path = format("%s/%s", VULKAN_INCLUDE_DIR, path);
    FILE *file = fopen(full_path.c_str(), "w");
    if (file == nullptr) {
        return 1;
    }

    fwrite(buffer.c_str(), sizeof(char), buffer.size(), file);
    fclose(file);

    return 0;
}

int build_source_split(std::string &buffer) {
    int result = 0;

    buffer += TEXT_COPYRIGHT;
    buffer += "\n";
    buffer += "#include <vulkan/vulkan_profiles.h>\n";

    buffer += fetch("library/generator/source_data.h");
    buffer += fetch("library/generator/source_util.h");
    buffer += fetch("library/generator/source_impl.h");

    return result;
}

int build_header_split(std::string& buffer) {
    int result = 0;

    buffer += TEXT_COPYRIGHT;
    buffer += TEXT_IFNDEF;
    buffer += "#define VP_INLINE\n";
    buffer += TEXT_CPP_BEGIN;

    buffer += fetch("library/generator/source_api.h");

    buffer += TEXT_CPP_END;
    buffer += TEXT_ENDIF;

    return result;
}

int build_header_only(std::string &buffer) {
    int result = 0;

    buffer += TEXT_COPYRIGHT;
    buffer += TEXT_IFNDEF;
    buffer += "#define VP_INLINE inline\n";

    buffer += fetch("library/generator/source_api.h");

    buffer += fetch("library/generator/source_data.h");
    buffer += fetch("library/generator/source_util.h");
    buffer += fetch("library/generator/source_impl.h");

    buffer += TEXT_ENDIF;

    return result;
}

int main(int argc, char *argv[]) {
    int result = 0;

    {
        std::string buffer;
        result += build_header_only(buffer);
        result += store("library/include/vulkan/vulkan_profiles.hpp", buffer);
    }

    {
        std::string buffer;
        result += build_header_split(buffer);
        result += store("library/include/vulkan/vulkan_profiles.h", buffer);
    }

    {
        std::string buffer;
        result += build_source_split(buffer);
        result += store("library/source/vulkan_profiles.cpp", buffer);
    }

    return result;
}
