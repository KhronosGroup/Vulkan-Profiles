cmake_minimum_required(VERSION 3.14.0)

enable_testing()

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment version")
set(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION_MAXIMUM FALSE) # Workaround to fix C.I. build on VS2015

project(VULKAN_PROFILES)
set(VULKAN_TAG v1.3.204)

# Define GNU standard installation directories.
include(GNUInstallDirs)

# Using Python 3.7.2 or better (required for typing.OrderedDict)
find_package(PythonInterp 3.7.2 REQUIRED)

# Third-party libraries
include(FetchContent)

# Always need C++2011 throughout the project.
set(CMAKE_CXX_STANDARD 11)

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
	GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers
	GIT_TAG ${VULKAN_TAG}
)
FetchContent_Declare(
	vulkan-validationlayers
	GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-ValidationLayers
	GIT_TAG v1.2.203
#	GIT_REPOSITORY git@gitlab.khronos.org:vulkan/Vulkan-ValidationLayers.git
#	GIT_TAG tony-13staging-gitlab
)
# Desktop builds link against the loader vulkan library.  Android
# builds just link against the system vulkan library.
if (NOT ANDROID)
    FetchContent_Declare(
        vulkan-loader
        GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Loader.git
        GIT_TAG sdk-1.3.204
    )
endif()

# After all the content is declared, it can safely be made available.
FetchContent_MakeAvailable(googletest)
FetchContent_MakeAvailable(jsoncpp)
execute_process(COMMAND ${PYTHON_EXECUTABLE} ${jsoncpp_SOURCE_DIR}/amalgamate.py WORKING_DIRECTORY ${jsoncpp_SOURCE_DIR})

set(UPDATE_DEPS ON)
set(BUILD_TESTS OFF)
set(_build_type ${CMAKE_BUILD_TYPE})
set(VVL_CPP_STANDARD 11)
set(_build_type ${CMAKE_BUILD_TYPE})

# For SDK (flat) builds, we'll receive pointers to our dependency repository builds.
# Otherwise, we'll fetch and build ourselves.  Other repositories also respect environment
# variables, so we're doing it here too.

# FetchContent() doesn't seem to actually do a Vulkan-Headers build at all.
# Consequently, we're really only interested in the source.
if(VULKAN_HEADERS_SOURCE_DIR)
    message(STATUS "Using Vulkan-Headers source located at ${VULKAN_HEADERS_SOURCE_DIR}")
    set(vulkan-headers_SOURCE_DIR ${VULKAN_HEADERS_SOURCE_DIR})
elseif(DEFINED ENV{VULKAN_HEADERS_SOURCE_DIR})
    message(STATUS "Using Vulkan-Headers source from environment VULKAN_HEADERS_SOURCE_DIR=$ENV{VULKAN_HEADERS_SOURCE_DIR}")
    set(vulkan-headers_SOURCE_DIR $ENV{VULKAN_HEADERS_SOURCE_DIR})
else()
    message(STATUS "Fetching Vulkan-Headers source")
    FetchContent_MakeAvailable(vulkan-headers)
endif()

# We really only want source for Vulkan-ValidationLayers.  We do not want or need
# a CMake sub-build (which would in turn pull in all VVL dependencies).
if(VULKAN_VALIDATIONLAYERS_SOURCE_DIR)
    message(STATUS "Using Vulkan-ValidationLayers source located at ${VULKAN_VALIDATIONLAYERS_SOURCE_DIR}")
    set(vulkan-validationlayers_SOURCE_DIR ${VULKAN_VALIDATIONLAYERS_SOURCE_DIR})
elseif(DEFINED ENV{VULKAN_VALIDATIONLAYERS_SOURCE_DIR})
    message(STATUS "Using Vulkan-ValidationLayers source from environment VULKAN_VALIDATIONLAYERS_SOURCE_DIR=$ENV{VULKAN_VALIDATIONLAYERS_SOURCE_DIR}")
    set(vulkan-validationlayers_SOURCE_DIR $ENV{VULKAN_VALIDATIONLAYERS_SOURCE_DIR})
else()
    # We use FetchContent_Populate() instead of FetchContent_MakeAvailable()
    # because we don't want to pull in dependencies and do a build.
    # We protect the FetchContent_Populate() call as a best practice,
    # in case this project is a dependency of a higher-level project
    # that also directly populates Vulkan-ValidationLayers.
    message(STATUS "Fetching Vulkan-ValidationLayers source")
    FetchContent_GetProperties(vulkan-validationlayers)
    if(NOT vulkan-validationlayers_POPULATED)
        FetchContent_Populate(vulkan-validationlayers)
    endif()
endif()

# We don't need to build a loader on Android, but we do everywhere else.
if (NOT ANDROID)
    if(VULKAN_LOADER_INSTALL_DIR OR DEFINED ENV{VULKAN_LOADER_INSTALL_DIR})
        if(VULKAN_LOADER_INSTALL_DIR)
            message(STATUS "Using Vulkan-Loader build located at ${VULKAN_LOADER_INSTALL_DIR}")
        else()
            message(STATUS "Using Vulkan-Loader build from environment VULKAN_LOADER_INSTALL_DIR=$ENV{VULKAN_LOADER_INSTALL_DIR}")
            set(VULKAN_LOADER_INSTALL_DIR $ENV{VULKAN_LOADER_INSTALL_DIR})
        endif()

        find_file(VULKAN_LOADER
            # Don't search any system paths
            NO_DEFAULT_PATH
            # vulkan-1.lib on Windows, libvulkan.so on Linux, libvulkan.1.dylib on Mac
            NAMES vulkan-1.lib libvulkan.so.1 libvulkan.1.dylib
            PATHS ${VULKAN_LOADER_INSTALL_DIR}
            # lib subdirectory on Windows, Linux, and Mac
            PATH_SUFFIXES lib
        )
        message(STATUS "Using Vulkan-Loader library ${VULKAN_LOADER}")
        # Sub-builds look for Vulkan::Vulkan to link the loader.
        add_library(Vulkan::Vulkan STATIC IMPORTED)
        set_target_properties(Vulkan::Vulkan PROPERTIES IMPORTED_LOCATION "${VULKAN_LOADER}")
    else()
        message(STATUS "Fetching and building Vulkan-Loader")
        FetchContent_MakeAvailable(vulkan-loader)
    endif()
endif()

include_directories(external)

set(API_NAME "Vulkan" CACHE STRING "API name to use when building")
string(TOLOWER ${API_NAME} API_LOWERCASE)
add_definitions(-DAPI_NAME="${API_NAME}")

add_subdirectory(scripts)

add_subdirectory(library)

add_subdirectory(layer-utils)

# add_vk_layer macro is used by layer/.
if (WIN32)
    macro(add_vk_layer target)
    FILE(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/VkLayer_${target}.def DEF_FILE)
    add_custom_target(VkLayer_copy-${target}-def-file ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DEF_FILE} VkLayer_${target}.def
        VERBATIM
    )
    set_target_properties(VkLayer_copy-${target}-def-file PROPERTIES FOLDER "Profiles layer")

    add_library(VkLayer_${target} SHARED ${ARGN} VkLayer_${target}.def)
    target_link_Libraries(VkLayer_${target} ${VkLayer_utils_LIBRARY} vku)
    set_target_properties(VkLayer_${target} PROPERTIES FOLDER "Profiles layer")
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
    set_target_properties(VkLayer_${target} PROPERTIES FOLDER "Profiles layer")
    install(TARGETS VkLayer_${target} DESTINATION ${CMAKE_INSTALL_LIBDIR})
    endmacro()
endif()
add_subdirectory(layer)

add_subdirectory(profiles/test)
