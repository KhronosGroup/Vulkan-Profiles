# ~~~
# Copyright (c) 2020-2025 LunarG, Inc.
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
endif()

option(BUILD_TESTS_EXTRA "Build the extra tests, for developers only")
if (BUILD_TESTS_EXTRA)
    add_definitions(-DVKU_FORCE_EXTRA_TESTS)
endif()

set(PROFILES_SCHEMA_FILENAME "profiles-0.8-latest.json")

find_package(Python3 REQUIRED)

# The profiles directory regenerates the Profiles source and headers.
add_subdirectory(profiles)
add_subdirectory(library)
add_subdirectory(layer)

