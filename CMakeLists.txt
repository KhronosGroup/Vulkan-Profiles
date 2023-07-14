cmake_minimum_required(VERSION 3.17.2)

project(VULKAN_PROFILES LANGUAGES CXX C)

include(GNUInstallDirs)

set(PROFILES_CPP_STANDARD 17 CACHE STRING "Set the C++ standard to build against.")
set(CMAKE_CXX_STANDARD ${PROFILES_CPP_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_VISIBILITY_PRESET "hidden")
set(CMAKE_C_VISIBILITY_PRESET "hidden")
set(CMAKE_VISIBILITY_INLINES_HIDDEN "YES")

add_subdirectory(scripts)

find_package(VulkanHeaders REQUIRED CONFIG QUIET)

find_package(VulkanUtilityLibraries REQUIRED CONFIG QUIET)

# Search the VVL include directory using the VVL vk_layer_config.h file
find_file(VVL_INCLUDE_DIR vk_layer_config.h REQUIRED CMAKE_FIND_ROOT_PATH_BOTH)
get_filename_component(VVL_INCLUDE_DIR ${VVL_INCLUDE_DIR} DIRECTORY)

# Effectively the header files installed by VVL are part of Vulkan::Headers
# This reflects the current structure of the VulkanSDK.
target_include_directories(Vulkan::Headers INTERFACE ${VVL_INCLUDE_DIR})

find_package(valijson REQUIRED CONFIG)

find_package(jsoncpp REQUIRED CONFIG)

option(BUILD_TESTS "Build the tests")
if (BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED CONFIG)
endif()

if (NOT ANDROID)
    # The vulkan loader search is:
    #     User-supplied setting of CMAKE_PREFIX_PATH
    #     VULKAN_LOADER_INSTALL_DIR defined via cmake option
    #     VULKAN_LOADER_INSTALL_DIR defined via environment variable
    #     Default findVulkan operation if the VULKAN_SDK environment variable is defined
    set(VULKAN_LOADER_INSTALL_DIR "LOADER-NOTFOUND" CACHE PATH "Absolute path to a Vulkan-Loader install directory")
    if (VULKAN_LOADER_INSTALL_DIR)
        message(STATUS "VULKAN_LOADER_INSTALL_DIR specified, using find_package to locate Vulkan")
    elseif(ENV{VULKAN_LOADER_INSTALL_DIR})
        message(STATUS "VULKAN_LOADER_INSTALL_DIR environment variable specified, using find_package to locate Vulkan")
    endif()
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH};${VULKAN_LOADER_INSTALL_DIR};${VULKAN_HEADERS_INSTALL_DIR};$ENV{VULKAN_LOADER_INSTALL_DIR};$ENV{VULKAN_HEADERS_INSTALL_DIR})
    find_package(Vulkan)
endif()

# Enable GUI folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# NOTE: The idiom for open source projects is to not enable warnings as errors.
# This reduces problems for users who simply want to build the repository.
option(BUILD_WERROR "Treat compiler warnings as errors" OFF)
if (BUILD_WERROR)
    add_compile_options("$<IF:$<CXX_COMPILER_ID:MSVC>,/WX,-Werror>")
endif()

# Platform-specific compiler switches
if(${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    add_compile_options(-Werror)
endif()
if(${CMAKE_C_COMPILER_ID} MATCHES "(GNU|Clang)")
    add_compile_options(-Werror)
elseif(MSVC)
    # PDBs aren't generated on Release builds by default.
    add_compile_options("$<$<CONFIG:Release>:/Zi>")
    add_link_options("$<$<CONFIG:Release>:/DEBUG:FULL>")
    # Remove unreferenced code/data.
    add_link_options("$<$<CONFIG:Release>:/OPT:REF>")
    # Merge duplicate data/functions
    add_link_options("$<$<CONFIG:Release>:/OPT:ICF>")

    add_compile_options(/W4 /WX)
    add_link_options(/WX)
    # Ignore some warnings that we know we'll generate.  In the future the
    # code that generates these warnings should be fixed properly.
    # vk_layer_logging.h provokes:
    #    warning C4100: 'default_flag_is_spec': unreferenced formal parameter
    # vk_loader_platform.h provokes:
    #    warning C4505: unreferenced local function has been removed
    # jsoncpp.cpp provokes:
    #    warning C4702: unreachable code
    # gtest.h provokes:
    #    warning C4389: '==': signed/unsigned mismatch
    #    warning C4018: '>=': signed/unsigned mismatch
    # vulkan_profiles.hpp provokes:
    #    warning C4245: '=': conversion from 'int' to 'uint64_t', signed/unsigned mismatch
    #    warning C4305: '=': truncation from 'double' to 'float'
    add_compile_options(/wd4100 /wd4505 /wd4702 /wd4389 /wd4245 /wd4305 /wd4018)
endif()

set(PROFILES_SCHEMA_FILENAME "profiles-0.8-latest.json")

find_package(Python3 REQUIRED)

# The profiles directory regenerates the Profiles source and headers.
add_subdirectory(profiles)

add_subdirectory(library)

add_subdirectory(layer)
