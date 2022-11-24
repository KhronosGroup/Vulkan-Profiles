cmake_minimum_required(VERSION 3.14.0)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

enable_testing()

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment version")

project(VULKAN_PROFILES LANGUAGES CXX C)

set(PROFILES_CPP_STANDARD 17 CACHE STRING "Set the C++ standard to build against.")
set(CMAKE_CXX_STANDARD ${PROFILES_CPP_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Regenerating profiles requires Python 3.7.2 or better (for typing.OrderedDict).
# Regeneration jsoncpp requires Python 3.
# Otherwise Python is not required, as checked-in generated code and
# headers will be used.
option(REGENERATE_PROFILES "Regenerate profiles source and headers" ON)
option(REGENERATE_JSONCPP "Regenerate jsoncpp source and headers" ON)
option(REGENERATE_LAYER "Regenerate profiles layer source" ON)

if (UPDATE_DEPS)
    find_package(PythonInterp 3 REQUIRED)

    if (CMAKE_GENERATOR_PLATFORM)
        set(_target_arch ${CMAKE_GENERATOR_PLATFORM})
    else()
        if (MSVC_IDE)
            message(WARNING "CMAKE_GENERATOR_PLATFORM not set. Using x64 as target architecture.")
        endif()
        set(_target_arch x64)
    endif()

    if (NOT CMAKE_BUILD_TYPE)
        message(WARNING "CMAKE_BUILD_TYPE not set. Using Debug for dependency build type")
        set(_build_type Debug)
    else()
        set(_build_type ${CMAKE_BUILD_TYPE})
    endif()

    message("********************************************************************************")
    message("* NOTE: Adding target profile_update_deps to run as needed for updating        *")
    message("*       dependencies.                                                          *")
    message("********************************************************************************")

    set(optional_args)
    if (NOT BUILD_TESTS)
        set(optional_args "--optional=tests")
    endif()

    if (UPDATE_DEPS_SKIP_EXISTING_INSTALL)
        set(optional_args ${optional_args} "--skip-existing-install")
    endif()

    if (DEFINED CMAKE_TOOLCHAIN_FILE)
        set(optional_args ${optional_args} "--cmake_var")
        set(optional_args ${optional_args} "CMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
    endif()

    # Add a target so that update_deps.py will run when necessary
    # NOTE: This is triggered off of the timestamps of known_good.json and helper.cmake
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_LIST_DIR}/external/helper.cmake
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/scripts/update_deps.py 
            --dir ${CMAKE_CURRENT_LIST_DIR}/external --arch ${_target_arch} --config ${_build_type} --generator "${CMAKE_GENERATOR}" ${optional_args}
        DEPENDS ${CMAKE_CURRENT_LIST_DIR}/scripts/known_good.json
    )

    # Check if update_deps.py needs to be run on first cmake run
    if (${CMAKE_CURRENT_LIST_DIR}/scripts/known_good.json IS_NEWER_THAN ${CMAKE_CURRENT_LIST_DIR}/external/helper.cmake)
        execute_process(
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/scripts/update_deps.py
                --dir ${CMAKE_CURRENT_LIST_DIR}/external --arch ${_target_arch} --config ${_build_type} --generator "${CMAKE_GENERATOR}" ${optional_args}
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
            RESULT_VARIABLE _update_deps_result
        )
        if (NOT (${_update_deps_result} EQUAL 0))
            message(FATAL_ERROR "Could not run update_deps.py which is necessary to download dependencies.")
        endif()
    endif()
    include(${CMAKE_CURRENT_LIST_DIR}/external/helper.cmake)
else()
    message("********************************************************************************")
    message("* NOTE: Not adding target to run update_deps.py automatically.                 *")
    message("********************************************************************************")
    find_package(PythonInterp 3 QUIET)
endif()

if (JSONCPP_INSTALL_DIR)
    list(APPEND CMAKE_PREFIX_PATH ${JSONCPP_INSTALL_DIR})
endif()
if (VALIJSON_INSTALL_DIR)
    list(APPEND CMAKE_PREFIX_PATH ${VALIJSON_INSTALL_DIR})
endif()
if (GOOGLETEST_INSTALL_DIR)
    list(APPEND CMAKE_PREFIX_PATH ${GOOGLETEST_INSTALL_DIR})
endif()
if (VULKAN_HEADERS_INSTALL_DIR)
    list(APPEND CMAKE_PREFIX_PATH ${VULKAN_HEADERS_INSTALL_DIR})
endif()

find_package(VulkanHeaders REQUIRED CONFIG QUIET)
set(VULKAN_HEADER_REGISTRY ${CMAKE_CURRENT_SOURCE_DIR}/external/Vulkan-Headers/registry)

include_directories(external/Vulkan-Headers/include)

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

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_VISIBILITY_PRESET "hidden")
set(CMAKE_C_VISIBILITY_PRESET "hidden")
set(CMAKE_VISIBILITY_INLINES_HIDDEN "YES")

# Define GNU standard installation directories.
include(GNUInstallDirs)

if(WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    # Windows: if install locations not set by user, set install prefix to "<build_dir>\install".
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE)
endif()

# Enable GUI folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# "Helper" targets that don't have interesting source code should set their FOLDER property to this
set(LAYERS_HELPER_FOLDER "Helper Targets")

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

# NOTE: jsoncpp is located in SUBPROJECTS_DIR.
set (SUBPROJECTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)

find_path(JSONCPP_INCLUDE_DIR json/json.h HINTS "${SUBPROJECTS_DIR}/jsoncpp/dist"
                                                   "${SUBPROJECTS_DIR}/JsonCpp/dist"
                                                   "${SUBPROJECTS_DIR}/JsonCPP/dist"
                                                   "${SUBPROJECTS_DIR}/JSONCPP/dist"
                                                   "${CMAKE_SOURCE_DIR}/../jsoncpp/dist"
                                             DOC "Path to jsoncpp/dist/json/json.h")

find_path(JSONCPP_SOURCE_DIR jsoncpp.cpp HINTS "${SUBPROJECTS_DIR}/jsoncpp/dist"
                                                   "${SUBPROJECTS_DIR}/JsonCpp/dist"
                                                   "${SUBPROJECTS_DIR}/JsonCPP/dist"
                                                   "${SUBPROJECTS_DIR}/JSONCPP/dist"
                                                   "${CMAKE_SOURCE_DIR}/../jsoncpp/dist"
                                             DOC "Path to jsoncpp/dist/jsoncpp.cpp")

find_library(JSONCPP_LIB NAMES jsoncpp HINTS ${JSONCPP_SEARCH_PATH} )

if (WIN32)
    add_library(jsoncpp         STATIC IMPORTED)
    find_library(JSONCPP_DLIB NAMES jsoncpp
                 HINTS ${JSONCPP_DEBUG_SEARCH_PATH} )
    set_target_properties(jsoncpp PROPERTIES
                         IMPORTED_LOCATION       "${JSONCPP_LIB}"
                         IMPORTED_LOCATION_DEBUG "${JSONCPP_DLIB}")
endif()

find_package(jsoncpp REQUIRED CONFIG)
find_package(valijson REQUIRED CONFIG)

# Always need C++2011 throughout the project.
set(CMAKE_CXX_STANDARD 17)

# FetchContent_MakeAvailable(jsoncpp) will set these values (which
# affect the whole build).  Make sure they're set even if we
# don't regenerate jsoncpp.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib" CACHE PATH "Archive output dir.")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib" CACHE PATH "Library output dir.")
set(CMAKE_PDB_OUTPUT_DIRECTORY     "${CMAKE_BINARY_DIR}/bin" CACHE PATH "PDB (MSVC debug symbol)output dir.")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" CACHE PATH "Executable/dll output dir.")


#set(jsoncpp_generated_dir ${CMAKE_SOURCE_DIR}/external/generated-jsoncpp)
#set(jsoncpp_generated_source ${jsoncpp_generated_dir}/jsoncpp.cpp)
#set(jsoncpp_generated_include_dir ${jsoncpp_generated_dir})
#set(jsoncpp_generated_header ${jsoncpp_generated_include_dir}/json/json.h)

# Vulkan-ValidationLayers settings
set(UPDATE_DEPS ON)
set(BUILD_TESTS OFF)
set(_build_type ${CMAKE_BUILD_TYPE})
set(VVL_CPP_STANDARD 11)
set(_build_type ${CMAKE_BUILD_TYPE})
set(PROFILES_SCHEMA_FILENAME "profiles-0.8-latest.json")

include_directories(external)
include_directories(external/valijson/include)
include_directories(external/jsoncpp/include)

set(API_NAME "Vulkan" CACHE STRING "API name to use when building")
string(TOLOWER ${API_NAME} API_LOWERCASE)
add_definitions(-DAPI_NAME="${API_NAME}")

# The scripts directory regenerates the Profiles source and headers.
if(REGENERATE_PROFILES)
    add_subdirectory(scripts)
endif()

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

if (PROFILES_BUILD_TESTS)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    add_subdirectory(external/googletest)

    add_subdirectory(profiles/test)
endif()


