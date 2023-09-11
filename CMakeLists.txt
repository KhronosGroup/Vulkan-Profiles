cmake_minimum_required(VERSION 3.17.2)

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

find_package(VulkanHeaders REQUIRED CONFIG QUIET)

find_package(VulkanUtilityLibraries REQUIRED CONFIG QUIET)

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

set(PROFILES_SCHEMA_FILENAME "profiles-0.8-latest.json")

find_package(Python3 REQUIRED)

# The profiles directory regenerates the Profiles source and headers.
add_subdirectory(profiles)

add_subdirectory(library)

add_subdirectory(layer)
