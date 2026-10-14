# ~~~
# Copyright 2026 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ~~~
#
# Native build for the Vulkan Profiles Checker Android app.
#
# This CMake project:
#   1. Fetches Vulkan-Headers (for vulkan.h, vk.xml and VP_KHR_roadmap.json)
#   2. Creates a private Python venv with the modules needed by vkprofiles.py
#   3. Runs the repository's vkprofiles.py to generate a *debug* header-only
#      Vulkan Profiles API library for the selected profiles. The debug variant
#      reports every unmet extension/feature/property/format condition through
#      VP_DEBUG_MESSAGE_CALLBACK, which the app captures to explain failures.
#   4. Builds libprofiles_checker.so, a NativeActivity with a software-rendered UI.

cmake_minimum_required(VERSION 3.22.1)

project(vulkan_profiles_checker LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------------------------------
# Options
# ------------------------------------------------------------------------------

# Keep in sync with scripts/known_good.json in the repository root.
set(VULKAN_HEADERS_GIT_TAG "v1.4.364" CACHE STRING "Vulkan-Headers tag used for headers, vk.xml and the roadmap profiles")

# App version. The single source of truth is app/build.gradle.kts, which passes
# these through -D arguments; the defaults only matter for bare CMake configures.
set(VP_CHECKER_VERSION_NAME "0.0.0" CACHE STRING "App versionName (set by Gradle)")
set(VP_CHECKER_VERSION_CODE "0" CACHE STRING "App versionCode (set by Gradle)")

# stb_truetype.h is used to rasterize the system font for the native UI.
# Pinned to a specific commit and verified by SHA-256.
set(STB_TRUETYPE_COMMIT "2c980bb59875b0d32144a71867fbdebb2f77cd20" CACHE STRING "nothings/stb commit providing stb_truetype.h")
set(STB_TRUETYPE_SHA256 "ecd30b05e0dd4fea3a13c26810dd9e1992dc379049482c393d5a19e6b5090aab" CACHE STRING "SHA-256 of stb_truetype.h at STB_TRUETYPE_COMMIT")

# Profiles compiled into the app. All must be present in the generated header.
set(VP_CHECKER_PROFILES
    "VP_ANDROID_15_requirements,VP_ANDROID_16_requirements,VP_ANDROID_17_requirements,VP_ANDROID_18_requirements,VP_ANDROID_vulkan_profile_2021,VP_ANDROID_vulkan_profile_2022,VP_ANDROID_vulkan_profile_2025,VP_KHR_roadmap_2022,VP_KHR_roadmap_2024,VP_KHR_roadmap_2026"
    CACHE STRING "Comma separated list of profiles to include in the checker")

# Escape hatch: point at an already generated *debug* header to skip Python/generation.
set(VULKAN_PROFILES_PREBUILT_HEADER "" CACHE FILEPATH "Optional pre-generated debug vulkan_profiles header to use instead of generating one")

# ------------------------------------------------------------------------------
# Repository layout
# ------------------------------------------------------------------------------

# app/src/main/cpp -> android/profiles-checker/app/src/main/cpp ; repo root is 6 levels up.
get_filename_component(VP_REPO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../.." ABSOLUTE)
if (NOT EXISTS "${VP_REPO_ROOT}/scripts/vkprofiles.py" OR NOT EXISTS "${VP_REPO_ROOT}/profiles/Android")
    message(FATAL_ERROR "Vulkan-Profiles repository root not found at '${VP_REPO_ROOT}'. "
                        "The app must be built from within the Vulkan-Profiles repository.")
endif()

# ------------------------------------------------------------------------------
# Third-party: Vulkan-Headers and stb_truetype
# ------------------------------------------------------------------------------

include(FetchContent)

FetchContent_Declare(VulkanHeaders
    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
    GIT_TAG        ${VULKAN_HEADERS_GIT_TAG}
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(VulkanHeaders)

set(VK_REGISTRY_DIR "${vulkanheaders_SOURCE_DIR}/registry")
if (NOT EXISTS "${VK_REGISTRY_DIR}/vk.xml")
    message(FATAL_ERROR "vk.xml not found in fetched Vulkan-Headers at '${VK_REGISTRY_DIR}'")
endif()

set(STB_DIR "${CMAKE_CURRENT_BINARY_DIR}/third_party/stb")
set(STB_TRUETYPE_HEADER "${STB_DIR}/stb_truetype.h")
if (NOT EXISTS "${STB_TRUETYPE_HEADER}")
    file(MAKE_DIRECTORY "${STB_DIR}")
    message(STATUS "Downloading stb_truetype.h @ ${STB_TRUETYPE_COMMIT}")
    file(DOWNLOAD
        "https://raw.githubusercontent.com/nothings/stb/${STB_TRUETYPE_COMMIT}/stb_truetype.h"
        "${STB_TRUETYPE_HEADER}"
        EXPECTED_HASH SHA256=${STB_TRUETYPE_SHA256}
        TLS_VERIFY ON
        STATUS _stb_status)
    list(GET _stb_status 0 _stb_code)
    if (NOT _stb_code EQUAL 0)
        file(REMOVE "${STB_TRUETYPE_HEADER}")
        message(FATAL_ERROR "Failed to download stb_truetype.h: ${_stb_status}")
    endif()
endif()

# ------------------------------------------------------------------------------
# Profiles library generation
# ------------------------------------------------------------------------------

set(GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(GEN_INCLUDE_DIR "${GEN_DIR}/include")
set(GEN_HEADER_NAME "vulkan_profiles_checker")
set(GEN_HEADER "${GEN_INCLUDE_DIR}/${GEN_HEADER_NAME}.hpp")

if (VULKAN_PROFILES_PREBUILT_HEADER)
    if (NOT EXISTS "${VULKAN_PROFILES_PREBUILT_HEADER}")
        message(FATAL_ERROR "VULKAN_PROFILES_PREBUILT_HEADER='${VULKAN_PROFILES_PREBUILT_HEADER}' does not exist")
    endif()
    message(STATUS "Using pre-generated profiles header: ${VULKAN_PROFILES_PREBUILT_HEADER}")
    add_custom_command(
        OUTPUT "${GEN_HEADER}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${GEN_INCLUDE_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${VULKAN_PROFILES_PREBUILT_HEADER}" "${GEN_HEADER}"
        DEPENDS "${VULKAN_PROFILES_PREBUILT_HEADER}"
        VERBATIM)
else()
    find_package(Python3 3.10 REQUIRED COMPONENTS Interpreter)

    # Private virtual environment with jsonschema/pyparsing (same requirements as the root build).
    set(VENV_DIR "${CMAKE_CURRENT_BINARY_DIR}/python-venv")
    if (CMAKE_HOST_WIN32)
        set(VENV_PYTHON "${VENV_DIR}/Scripts/python.exe")
    else()
        set(VENV_PYTHON "${VENV_DIR}/bin/python3")
    endif()
    set(VENV_STAMP "${VENV_DIR}/venv.stamp")
    set(VP_REQUIREMENTS "${VP_REPO_ROOT}/requirements.txt")

    add_custom_command(
        OUTPUT "${VENV_STAMP}"
        COMMAND ${Python3_EXECUTABLE} -m venv "${VENV_DIR}"
        COMMAND "${VENV_PYTHON}" -m pip install --disable-pip-version-check --no-cache-dir -q -r "${VP_REQUIREMENTS}"
        COMMAND ${CMAKE_COMMAND} -E touch "${VENV_STAMP}"
        DEPENDS "${VP_REQUIREMENTS}"
        COMMENT "Creating Python virtual environment for vkprofiles.py"
        VERBATIM)

    file(GLOB ANDROID_PROFILE_FILES CONFIGURE_DEPENDS "${VP_REPO_ROOT}/profiles/Android/*.json")
    file(GLOB VP_SCRIPT_FILES CONFIGURE_DEPENDS
        "${VP_REPO_ROOT}/scripts/*.py"
        "${VP_REPO_ROOT}/scripts/source/*.py")
    set(ROADMAP_PROFILE_FILE "${VK_REGISTRY_DIR}/profiles/VP_KHR_roadmap.json")
    set(VKPROFILES_PY "${VP_REPO_ROOT}/scripts/vkprofiles.py")

    # Mirrors the "transform_android" and "library_*_debug" steps of
    # profiles/vkprofiles_graph.json, but writes everything into the build tree.
    add_custom_command(
        OUTPUT "${GEN_HEADER}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${GEN_DIR}/profiles" "${GEN_INCLUDE_DIR}" "${GEN_DIR}/intermediate"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ROADMAP_PROFILE_FILE}" "${GEN_DIR}/profiles/VP_KHR_roadmap.json"
        COMMAND ${CMAKE_COMMAND} -E env "PYTHONPATH=${VK_REGISTRY_DIR}"
                "${VENV_PYTHON}" "${VKPROFILES_PY}" transform
                    --registry "${VK_REGISTRY_DIR}/vk.xml"
                    --input "${VP_REPO_ROOT}/profiles/Android"
                    --output "${GEN_DIR}/profiles"
                    --pull requirements ignore-unsupported aliases
                    --strip helper-values
                    --format flatten
        COMMAND ${CMAKE_COMMAND} -E env "PYTHONPATH=${VK_REGISTRY_DIR}"
                "${VENV_PYTHON}" "${VKPROFILES_PY}" library
                    --registry "${VK_REGISTRY_DIR}/vk.xml"
                    --input "${GEN_DIR}/profiles"
                    --input-profiles "${VP_CHECKER_PROFILES}"
                    --output "${GEN_INCLUDE_DIR}"
                    --output-filename "${GEN_HEADER_NAME}"
                    --mode header-only
                    --strip
                    --intermediate "${GEN_DIR}/intermediate"
                    --config debug
        DEPENDS
            "${VENV_STAMP}"
            "${ROADMAP_PROFILE_FILE}"
            ${ANDROID_PROFILE_FILES}
            ${VP_SCRIPT_FILES}
        WORKING_DIRECTORY "${VP_REPO_ROOT}"
        COMMENT "Generating debug Vulkan Profiles library header (${GEN_HEADER_NAME}.hpp)"
        VERBATIM)
endif()

add_custom_target(generate_profiles_header DEPENDS "${GEN_HEADER}")

# ------------------------------------------------------------------------------
# Build info (version, build time, git commit) shown in the UI and the report
# ------------------------------------------------------------------------------

set(BUILD_INFO_HEADER "${GEN_INCLUDE_DIR}/vp_checker_build_info.h")
# No OUTPUT on purpose: the target runs on every build so the timestamp is
# refreshed; gen_build_info.cmake only rewrites the header when content changes.
add_custom_target(generate_build_info
    COMMAND ${CMAKE_COMMAND} -E make_directory "${GEN_INCLUDE_DIR}"
    COMMAND ${CMAKE_COMMAND}
        -DOUTPUT=${BUILD_INFO_HEADER}
        -DVERSION_NAME=${VP_CHECKER_VERSION_NAME}
        -DVERSION_CODE=${VP_CHECKER_VERSION_CODE}
        -DREPO_ROOT=${VP_REPO_ROOT}
        -P "${CMAKE_CURRENT_SOURCE_DIR}/gen_build_info.cmake"
    BYPRODUCTS "${BUILD_INFO_HEADER}"
    COMMENT "Generating build info header"
    VERBATIM)

# ------------------------------------------------------------------------------
# NativeActivity shared library
# ------------------------------------------------------------------------------

set(NATIVE_APP_GLUE_DIR "${ANDROID_NDK}/sources/android/native_app_glue")
if (NOT EXISTS "${NATIVE_APP_GLUE_DIR}/android_native_app_glue.c")
    message(FATAL_ERROR "android_native_app_glue not found in NDK at '${NATIVE_APP_GLUE_DIR}'")
endif()

add_library(native_app_glue STATIC "${NATIVE_APP_GLUE_DIR}/android_native_app_glue.c")
target_include_directories(native_app_glue PUBLIC "${NATIVE_APP_GLUE_DIR}")
target_link_libraries(native_app_glue PUBLIC android log)

add_library(profiles_checker SHARED
    main.cpp
    vulkan_loader.cpp
    device_info.cpp
    profile_checker.cpp
    report.cpp
    text_renderer.cpp
    ui.cpp
    "${GEN_HEADER}"
)
add_dependencies(profiles_checker generate_profiles_header generate_build_info)

target_include_directories(profiles_checker PRIVATE
    "${GEN_INCLUDE_DIR}"
    "${STB_DIR}"
)

target_compile_definitions(profiles_checker PRIVATE
    VK_NO_PROTOTYPES=1                 # Vulkan is dlopen'ed so 1.0-only devices still get a report
    VK_USE_PLATFORM_ANDROID_KHR=1      # Required for VP_ANDROID_* profiles (VK_KHR_android_surface)
    VP_USE_OBJECT=1                    # Use the VpFunctions object API of the profiles library
    VP_CHECKER_GENERATED_HEADER="${GEN_HEADER_NAME}.hpp"
    VP_CHECKER_VULKAN_HEADERS_TAG="${VULKAN_HEADERS_GIT_TAG}"
)

target_compile_options(profiles_checker PRIVATE
    -Wall -Wextra
    -Wno-unused-function
    -Wno-missing-field-initializers
    -Wno-unused-parameter
)

# Vulkan::Headers must come before the NDK sysroot so the fetched (newer) headers win.
target_link_libraries(profiles_checker PRIVATE
    Vulkan::Headers
    native_app_glue
    android
    log
)

# NativeActivity looks up this symbol from the glue; make sure the linker keeps it.
target_link_options(profiles_checker PRIVATE "-Wl,-u,ANativeActivity_onCreate")
