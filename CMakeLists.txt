cmake_minimum_required(VERSION 3.11.0)

enable_testing()

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment version")

project (VULKAN_PROFILES)

# Define GNU standard installation directories.
include(GNUInstallDirs)

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
  vulkan-headers
  GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers
  GIT_TAG v1.2.182
)

FetchContent_MakeAvailable(vulkan-headers)

# Library project
add_subdirectory(library)

# Layer project
add_subdirectory(layer)
