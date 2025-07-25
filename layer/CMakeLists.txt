# ~~~
# Copyright (c) 2021-2025 LunarG, Inc.
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

# This variable enables downstream users to customize the CMake targets
# based on the target API variant (e.g. Vulkan SC)
set(LAYER_NAME "VkLayer_khronos_profiles")

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E touch  ${CMAKE_SOURCE_DIR}/layer/profiles_generated.cpp)
execute_process(COMMAND ${CMAKE_COMMAND} -E touch  ${CMAKE_SOURCE_DIR}/layer/tests/tests_generated.cpp)

set_source_files_properties(${CMAKE_SOURCE_DIR}/layer/profiles_generated.cpp PROPERTIES GENERATED true)
set_source_files_properties(${CMAKE_SOURCE_DIR}/layer/tests/tests_generated.cpp PROPERTIES GENERATED true)

if(WIN32)
    # We need -DNOMINMAX for valijson build
    add_definitions(-DVK_USE_PLATFORM_WIN32_KHR -DVK_USE_PLATFORM_WIN32_KHX -DNOMINMAX)
    set(DisplayServer Win32)
elseif(ANDROID)
    add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR)
elseif(APPLE)
    add_compile_definitions(VK_USE_PLATFORM_METAL_EXT)
    if (IOS)
        add_compile_definitions(VK_USE_PLATFORM_IOS_MVK)
    endif()
    if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        add_compile_definitions(VK_USE_PLATFORM_MACOS_MVK)
    endif()
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux|BSD")
    if (BUILD_WSI_XCB_SUPPORT)
        add_definitions(-DVK_USE_PLATFORM_XCB_KHR -DVK_USE_PLATFORM_XCB_KHX)
        set(DisplayServer Xcb)
    endif()

    if (BUILD_WSI_XLIB_SUPPORT)
        if (NOT DisplayServer)
            set(DisplayServer Xlib)
        endif()
        add_definitions(-DVK_USE_PLATFORM_XLIB_KHR -DVK_USE_PLATFORM_XLIB_KHX)
    endif()

    if (BUILD_WSI_WAYLAND_SUPPORT)
       # TODO Add Wayland Support
       # add_definitions(-DVK_USE_PLATFORM_WAYLAND_KHR)
    endif()
else()
    message(FATAL_ERROR "Unsupported Platform!")
endif()

option(BUILD_MOCK_ANDROID_SUPPORT "Build with Android Platform headers" OFF)
if(BUILD_MOCK_ANDROID_SUPPORT)
    add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR)
endif()

add_definitions(-DVK_ENABLE_BETA_EXTENSIONS)

source_group("JSON Manifest" FILES ${LAYER_NAME}.json.in)
source_group("Export Files" FILES ${LAYER_NAME}.def ${LAYER_NAME}.map)

# ProfilesLayer build target

if(IOS)
    add_library(ProfilesLayer SHARED)
else()
    add_library(ProfilesLayer MODULE)
endif()

set_target_properties(ProfilesLayer PROPERTIES FOLDER "Profiles layer")
set_target_properties(ProfilesLayer PROPERTIES OUTPUT_NAME ${LAYER_NAME})

if (MSVC)
    target_compile_options(ProfilesLayer PRIVATE "$<$<CONFIG:Release>:/Zi>")
    target_link_options(ProfilesLayer PRIVATE /DEF:${CMAKE_CURRENT_SOURCE_DIR}/${LAYER_NAME}.def)
    target_link_options(ProfilesLayer PRIVATE "$<$<CONFIG:Release>:/DEBUG:FULL>")
    target_link_options(ProfilesLayer PRIVATE "$<$<CONFIG:Release>:/OPT:REF>")
    target_link_options(ProfilesLayer PRIVATE "$<$<CONFIG:Release>:/OPT:ICF>")
elseif(MINGW)
    target_sources(ProfilesLayer PRIVATE ${LAYER_NAME}.def)
elseif(APPLE)
    message(DEBUG "Functions are exported via PROFILES_EXPORT")

if(IOS)
      set_target_properties(ProfilesLayer PROPERTIES
      FRAMEWORK TRUE
        MACOSX_FRAMEWORK_BUNDLE_VERSION "${VulkanHeaders_VERSION}"
        MACOSX_FRAMEWORK_SHORT_VERSION_STRING "${VulkanHeaders_VERSION}"
        MACOSX_FRAMEWORK_IDENTIFIER com.khronos.VkLayer_khronos_profiles
      )
else()
    set_target_properties(ProfilesLayer PROPERTIES SUFFIX ".dylib")
endif()

elseif(ANDROID)
    message(DEBUG "Functions are exported via PROFILES_EXPORT")
else()
    target_link_options(ProfilesLayer PRIVATE LINKER:--version-script=${CMAKE_CURRENT_SOURCE_DIR}/${LAYER_NAME}.map,-Bsymbolic,--exclude-libs,ALL)
endif()

set(TESTS_PYTHON_SCRIPT ${CMAKE_SOURCE_DIR}/scripts/gen_profiles_tests.py)
set(LAYER_PYTHON_SCRIPT ${CMAKE_SOURCE_DIR}/scripts/gen_profiles_layer.py)
set(PYTHON_SCRIPTS
    ${LAYER_PYTHON_SCRIPT}
    ${TESTS_PYTHON_SCRIPT}
    ${CMAKE_SOURCE_DIR}/scripts/gen_profiles_solution.py
)

source_group("Python Files" FILES ${PYTHON_SCRIPTS})

target_sources(ProfilesLayer PRIVATE
    profiles_settings.cpp
    profiles_settings.h
    profiles_json.cpp
    profiles_json.h
    profiles_util.cpp
    profiles_util.h
    profiles_interface.cpp
    profiles_interface.h
    profiles_generated.cpp
    profiles.h
    vk_layer_table.cpp
    vk_layer_table.h
    ${LAYER_NAME}.json.in
    ${LAYER_NAME}.def
    ${LAYER_NAME}.map
    ${PYTHON_SCRIPTS}
)

target_link_libraries(ProfilesLayer PRIVATE
    Vulkan::CompilerConfiguration 
    Vulkan::CompilerConfigurationExtra
    Vulkan::LayerSettings
    Vulkan::Headers
    Vulkan::UtilityHeaders
    jsoncpp_static
    valijson
)

if(ANDROID)
    # Android needs -llog for __android_print_log()
    target_link_Libraries(ProfilesLayer PRIVATE log)
endif()

add_custom_target(VpLayer_generate ALL
    COMMAND Python3::Interpreter ${LAYER_PYTHON_SCRIPT}
        --api ${API_TYPE}
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --out-layer ${CMAKE_SOURCE_DIR}/layer/profiles_generated.cpp
    VERBATIM
    SOURCES ${LAYER_PYTHON_SCRIPT}
    DEPENDS ${LAYER_PYTHON_SCRIPT}
            ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
            ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/video.xml)
set_target_properties(VpLayer_generate PROPERTIES FOLDER "Profiles layer")
add_dependencies(ProfilesLayer VpLayer_generate)

source_group("Python Files" FILES ${TESTS_PYTHON_SCRIPT})
add_custom_target(VpLayer_generate_tests ALL
	COMMAND Python3::Interpreter ${TESTS_PYTHON_SCRIPT}
		--api ${API_TYPE}
		--registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
		--out-profile ${CMAKE_SOURCE_DIR}/profiles/test/data/VP_LUNARG_test_api_generated.json
		--out-tests ${CMAKE_SOURCE_DIR}/layer/tests/tests_generated.cpp
	VERBATIM
    SOURCES ${TESTS_PYTHON_SCRIPT}
	DEPENDS ${TESTS_PYTHON_SCRIPT}
            ProfilesLayer
            VpLayer_generate
	)
set_target_properties(VpLayer_generate_tests PROPERTIES FOLDER "Profiles layer")

set(INPUT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${LAYER_NAME}.json.in")
set(INTERMEDIATE_FILE "${CMAKE_CURRENT_BINARY_DIR}/json/profile.json")
set(OUTPUT_FILE_FINAL_NAME "${LAYER_NAME}.json")
set(LAYER_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})
if (WIN32)
    set(LAYER_INSTALL_DIR ${CMAKE_INSTALL_BINDIR}) # WIN32/MINGW expect the dll in the `bin` dir, this matches our WIN32 SDK process
endif()

if (WIN32)
    set(JSON_LIBRARY_PATH ".\\\\${LAYER_NAME}.dll")
elseif(APPLE)
    set(JSON_LIBRARY_PATH "./lib${LAYER_NAME}.dylib")
else()
    set(JSON_LIBRARY_PATH "./lib${LAYER_NAME}.so")
endif()

set(JSON_API_VERSION ${VulkanHeaders_VERSION})

configure_file(${INPUT_FILE} ${INTERMEDIATE_FILE} @ONLY)

# To support both multi/single configuration generators just copy the json to the correct directory
add_custom_command(TARGET ProfilesLayer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${INTERMEDIATE_FILE} $<TARGET_FILE_DIR:ProfilesLayer>/${OUTPUT_FILE_FINAL_NAME}
)

# For UNIX-based systems, `library_path` should not contain a relative path (indicated by "./") before installing to system directories
# This json isn't used for regular local development, it's used for installation
if (UNIX)
    set(UNIX_INTERMEDIATE_FILE "${CMAKE_CURRENT_BINARY_DIR}/json/unix_install_profile.json")

    if(APPLE)
        set(JSON_LIBRARY_PATH "lib${LAYER_NAME}.dylib")
    else()
        set(JSON_LIBRARY_PATH "lib${LAYER_NAME}.so")
    endif()

    configure_file(${INPUT_FILE} ${UNIX_INTERMEDIATE_FILE} @ONLY)

    install(FILES ${UNIX_INTERMEDIATE_FILE} DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${API_TYPE}/explicit_layer.d RENAME ${OUTPUT_FILE_FINAL_NAME})
endif()

if (WIN32)
    install(FILES ${INTERMEDIATE_FILE} DESTINATION ${LAYER_INSTALL_DIR} RENAME ${OUTPUT_FILE_FINAL_NAME})
endif()
if (MSVC)
    install(FILES $<TARGET_PDB_FILE:ProfilesLayer> DESTINATION ${LAYER_INSTALL_DIR})
endif()

install(TARGETS ProfilesLayer DESTINATION ${LAYER_INSTALL_DIR})
