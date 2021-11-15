#include <cstdio>
#include <cstdarg>
#include <cassert>
#include <string>

#include "source_api.h"

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

int build_source_split(std::string &buffer) {
    int result = 0;

    buffer += TEXT_COPYRIGHT;
    buffer += "\n";
    buffer += "#include <vulkan/vulkan_profiles.h>";



    return result;
}

int build_header_split(std::string& buffer) {
    int result = 0;

    buffer += TEXT_COPYRIGHT;
    buffer += TEXT_IFNDEF;
    buffer += TEXT_CPP_BEGIN;

    buffer += TEXT_API_DECL;



    buffer += TEXT_CPP_END;
    buffer += TEXT_ENDIF;

    return result;
}

int build_header_only(std::string &buffer) {
    int result = 0;

    buffer += TEXT_COPYRIGHT;
    buffer += TEXT_IFNDEF;

    buffer += TEXT_API_DECL;




    buffer += TEXT_ENDIF;

    return result;
}

int store(const char* path, std::string &buffer) {
    const std::string full_path = format("%s/%s", VULKAN_INCLUDE_DIR, path);
    FILE *file = fopen(full_path.c_str(), "w");
    if (file == nullptr) {
        return 1;
    }

    fwrite(buffer.c_str(), sizeof(char), buffer.size(), file);
    fclose(file);

    return 0;
}

int main(int argc, char *argv[]) {
    int result = 0;

    {
        std::string buffer;
        result += build_header_only(buffer);
        result += store("library/include/vulkan/vulkan_profiles_gen.hpp", buffer);
    }

    {
        std::string buffer;
        result += build_header_split(buffer);
        result += store("library/include/vulkan/vulkan_profiles_inl.hpp", buffer);
    }

    {
        std::string buffer;
        result += build_source_split(buffer);
        result += store("library/include/vulkan/vulkan_profiles_src.cpp", buffer);
    }

    return result;
}
