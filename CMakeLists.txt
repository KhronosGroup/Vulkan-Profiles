cmake_minimum_required(VERSION 3.11.0)

enable_testing()

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment version")
set(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION_MAXIMUM FALSE) # Workaround to fix C.I. build on VS2015

project(VULKAN_PROFILES)
set(VULKAN_TAG v1.2.193)

# Define GNU standard installation directories.
include(GNUInstallDirs)

# Using Python 3
find_package(PythonInterp 3 REQUIRED)

# Third-party libraries
include(FetchContent)

FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest
	GIT_TAG release-1.11.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
	json
	GIT_REPOSITORY https://github.com/nlohmann/json
	GIT_TAG v3.10.0
)

FetchContent_MakeAvailable(json)

FetchContent_Declare(
	jsoncpp
	GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp
	GIT_TAG 1.9.4
)

set(JSONCPP_WITH_TESTS OFF CACHE BOOL "" FORCE)
set(JSONCPP_WITH_POST_BUILD_UNITTEST OFF CACHE BOOL "" FORCE)
set(JSONCPP_WITH_WARNING_AS_ERROR OFF CACHE BOOL "" FORCE)
set(JSONCPP_WITH_PKGCONFIG_SUPPORT OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_OBJECT_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(jsoncpp)

set(UPDATE_DEPS ON)
set(BUILD_TESTS OFF)
set(VVL_CPP_STANDARD 11)
set(_build_type ${CMAKE_BUILD_TYPE})

FetchContent_Declare(
	Vulkan-Headers
	GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers
	GIT_TAG main
)

FetchContent_MakeAvailable(Vulkan-Headers)

FetchContent_Declare(
	Vulkan-Loader
	GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Loader
	GIT_TAG master
)

FetchContent_MakeAvailable(Vulkan-Loader)

FetchContent_Declare(
	Vulkan-ValidationLayers
	GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-ValidationLayers
	GIT_TAG ${VULKAN_TAG}
)

set(UPDATE_DEPS ON)
set(BUILD_TESTS OFF)
set(VVL_CPP_STANDARD 11)
FetchContent_MakeAvailable(Vulkan-ValidationLayers)

# Library project
add_subdirectory(library)

# Layer project
add_subdirectory(layer)
