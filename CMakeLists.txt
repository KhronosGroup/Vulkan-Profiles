# ~~~
# Copyright (c) 2020-2026 LunarG, Inc.
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
#
# Authors: 
# - Christophe Riccio <christophe@lunarg.com>
# ~~~

cmake_minimum_required(VERSION 3.22.1)

project(VULKAN_PROFILES LANGUAGES CXX C)

# Enable GUI folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(GNUInstallDirs)

# This variable enables downstream users to customize the target API variant (e.g. Vulkan SC)
set(API_TYPE "vulkan")

set(PROFILES_CPP_STANDARD 17 CACHE STRING "Set the C++ standard to build against.")
set(CMAKE_CXX_STANDARD ${PROFILES_CPP_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_VISIBILITY_PRESET "hidden")
set(CMAKE_C_VISIBILITY_PRESET "hidden")
set(CMAKE_VISIBILITY_INLINES_HIDDEN "YES")

add_subdirectory(scripts)
set(MERGE_SCRIPT ${PROJECT_SOURCE_DIR}/scripts/gen_profiles_file.py)
set(SOLUTION_SCRIPT ${PROJECT_SOURCE_DIR}/scripts/gen_profiles_solution.py)
set(PROFILES_SCRIPT ${PROJECT_SOURCE_DIR}/scripts/profiles.py)

find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_package(VulkanHeaders REQUIRED CONFIG QUIET)
find_package(VulkanUtilityLibraries REQUIRED CONFIG QUIET)
find_package(valijson REQUIRED CONFIG)
find_package(jsoncpp REQUIRED CONFIG)

option(BUILD_TESTS "Build the tests")
if (BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED CONFIG)

    if(NOT ANDROID)
        find_package(VulkanLoader REQUIRED CONFIG)
    endif()

    add_subdirectory(scripts/tests)
endif()

option(BUILD_TESTS_EXTRA "Build the extra tests, for developers only")
if (BUILD_TESTS_EXTRA)
    add_definitions(-DVKU_FORCE_EXTRA_TESTS)
endif()

set(PROFILES_SCHEMA_FILENAME "profiles-0.8-latest.json")

# Generate profiles executable
function(add_pyinstaller_target TARGET_NAME)
    # Define the arguments the function accepts
    set(options)
    set(oneValueArgs SCRIPT OUTPUT_DIR FOLDER OUTPUT_NAME)
    set(multiValueArgs DEPENDS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT ARG_SCRIPT)
        message(FATAL_ERROR "add_pyinstaller_target: SCRIPT argument is required for target ${TARGET_NAME}")
    endif()

    if(NOT ARG_OUTPUT_DIR)
        set(ARG_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
    endif()

    if(NOT ARG_OUTPUT_NAME)
        set(ARG_OUTPUT_NAME "${TARGET_NAME}")
    endif()

    # Handle OS-specific executable extension
    if(WIN32)
        set(EXE_NAME "${ARG_OUTPUT_NAME}.exe")
    else()
        set(EXE_NAME "${ARG_OUTPUT_NAME}")
    endif()
    set(FINAL_EXECUTABLE "${ARG_OUTPUT_DIR}/${EXE_NAME}")

    # Generate the executable via PyInstaller
    add_custom_command(
        OUTPUT "${FINAL_EXECUTABLE}"
        COMMAND ${Python3_EXECUTABLE} -m PyInstaller --onefile --name "${ARG_OUTPUT_NAME}" --distpath "${ARG_OUTPUT_DIR}" "${ARG_SCRIPT}"
        DEPENDS ${ARG_DEPENDS}
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Packaging ${TARGET_NAME} into a standalone executable..."
        VERBATIM
    )

    # Create the custom target to hook into the build system
    add_custom_target(
        ${TARGET_NAME}
        DEPENDS "${FINAL_EXECUTABLE}"
    )

    # Set IDE folder property if provided
    if(ARG_FOLDER)
        set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "${ARG_FOLDER}")
    endif()
endfunction()

set(PYTHON_DEPENDENCIES
    "${PROFILES_SCRIPT}"
    "${CMAKE_CURRENT_SOURCE_DIR}/scripts/gen_profiles_file.py"
    "${CMAKE_CURRENT_SOURCE_DIR}/scripts/gen_profiles_layer.py"
    "${CMAKE_CURRENT_SOURCE_DIR}/scripts/gen_profiles_solution.py"
    "${CMAKE_CURRENT_SOURCE_DIR}/scripts/gen_profiles_tests.py"
    "${CMAKE_CURRENT_SOURCE_DIR}/scripts/source/profiles_parsing.py"
    "${CMAKE_CURRENT_SOURCE_DIR}/scripts/source/vulkan_object_expression_parsing.py"
    "${CMAKE_CURRENT_SOURCE_DIR}/scripts/source/vulkan_object_utils.py"
    "${CMAKE_CURRENT_SOURCE_DIR}/scripts/source/vulkan_object_version.py"
)

add_pyinstaller_target(VpProfilesProcessor
    OUTPUT_NAME "vkprofiles"
    SCRIPT "${PROFILES_SCRIPT}"
    OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/scripts"
    DEPENDS ${PYTHON_DEPENDENCIES}
    FOLDER "Profiles generator"
)

# The profiles directory regenerates the Profiles source and headers.
add_subdirectory(profiles)
add_subdirectory(library)
add_subdirectory(layer)

