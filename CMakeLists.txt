cmake_minimum_required(VERSION 3.14.0)

enable_testing()

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment version")
set(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION_MAXIMUM FALSE) # Workaround to fix C.I. build on VS2015

project(VULKAN_PROFILES)
set(VULKAN_TAG v1.2.199)

# Define GNU standard installation directories.
include(GNUInstallDirs)

# Using Python 3
find_package(PythonInterp 3 REQUIRED)

# Third-party libraries
include(FetchContent)

# Declare all content before making available, to ensure that these
# settings are used instead of any settings that may be encountered
# inside these projects.
#
# Names should be lower-case, since FetchContent always sets 
# <lowercaseName>_* variables.  Sometimes (only when using 
# FetchContent_MakeAvailable) it will also set <givenName>_*
# variables, but this isn't consistent and isn't guaranteed
# by the documentation.
FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest
	GIT_TAG release-1.11.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_Declare(
	json
	GIT_REPOSITORY https://github.com/nlohmann/json
	GIT_TAG v3.10.0
)
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
FetchContent_Declare(
	vulkan-headers
	GIT_REPOSITORY git@gitlab.khronos.org:vulkan/Vulkan-Headers.git
	GIT_TAG 13staging
)
FetchContent_Declare(
	vulkan-validationlayers
	GIT_REPOSITORY git@gitlab.khronos.org:vulkan/Vulkan-ValidationLayers.git
	GIT_TAG tony-13staging-gitlab
)
# Desktop builds link against the loader vulkan library.  Android
# builds just link against the system vulkan library.
if (NOT ANDROID)
    FetchContent_Declare(
        vulkan-loader
        GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Loader
        GIT_TAG ${VULKAN_TAG}
    )
endif()

# After all the content is declared, it can safely be made available.
FetchContent_MakeAvailable(googletest)
FetchContent_MakeAvailable(json)
FetchContent_MakeAvailable(jsoncpp)
execute_process(COMMAND python ${jsoncpp_SOURCE_DIR}/amalgamate.py WORKING_DIRECTORY ${jsoncpp_SOURCE_DIR})

set(UPDATE_DEPS ON)
set(BUILD_TESTS OFF)
set(_build_type ${CMAKE_BUILD_TYPE})
set(VVL_CPP_STANDARD 11)
set(_build_type ${CMAKE_BUILD_TYPE})
FetchContent_MakeAvailable(vulkan-headers)
if (NOT ANDROID)
    FetchContent_MakeAvailable(vulkan-loader)
endif()

# We want the code for Vulkan-ValidationLayers, to get source required
# to build devsim, but we don't need its build and build dependencies;
# we just need some source from its tree.  We avoid using
# FetchContent_MakeAvailable() because it will add the dependency build
# (and all of its dependencies, recursively) to our build; by calling 
# FetchContent_Populate() directly, we avoid the troubles that can cause.
FetchContent_GetProperties(vulkan-validationlayers)
# We protect the FetchContent_Populate() call just in case this project
# is a dependent project of a higher-level project.
if(NOT vulkan-validationlayers_POPULATED)
    FetchContent_Populate(vulkan-validationlayers)
endif()

set(API_NAME "Vulkan" CACHE STRING "API name to use when building")
string(TOLOWER ${API_NAME} API_LOWERCASE)
add_definitions(-DAPI_NAME="${API_NAME}")

add_subdirectory(library)

add_subdirectory(vku)

# add_vk_layer macro is used by both device_simulation/ and layer/.
if (WIN32)
    macro(add_vk_layer target)
    FILE(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/VkLayer_${target}.def DEF_FILE)
    add_custom_target(copy-${target}-def-file ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DEF_FILE} VkLayer_${target}.def
        VERBATIM
    )
    add_library(VkLayer_${target} SHARED ${ARGN} VkLayer_${target}.def)
    target_link_Libraries(VkLayer_${target} ${VkLayer_utils_LIBRARY} vku)
    endmacro()
else()
    macro(add_vk_layer target)
    add_library(VkLayer_${target} SHARED ${ARGN})
    target_link_Libraries(VkLayer_${target} ${VkLayer_utils_LIBRARY} vku)
    if(ANDROID)
        # Android needs -llog for __android_print_log()
        target_link_Libraries(VkLayer_${target} log)
    endif()
    if (NOT APPLE)
        set_target_properties(VkLayer_${target} PROPERTIES LINK_FLAGS "-Wl,-Bsymbolic")
    endif ()
    install(TARGETS VkLayer_${target} DESTINATION ${CMAKE_INSTALL_LIBDIR})
    endmacro()
endif()
add_subdirectory(device_simulation)
add_subdirectory(layer)

include_directories(external)

add_subdirectory(profiles/test)
