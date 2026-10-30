# ~~~
# Copyright (c) 2021-2026 LunarG, Inc.
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

# Create placeholders for the generated library files only when missing, so reconfiguring doesn't change their timestamps
# and trigger a rebuild of everything that includes them
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/library/include/vulkan/debug)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/library/source/debug)
foreach(placeholder
        library/include/vulkan/vulkan_profiles.h
        library/include/vulkan/vulkan_profiles.hpp
        library/include/vulkan/debug/vulkan_profiles.h
        library/include/vulkan/debug/vulkan_profiles.hpp
        library/source/vulkan_profiles.cpp
        library/source/debug/vulkan_profiles.cpp)
    if (NOT EXISTS ${CMAKE_SOURCE_DIR}/${placeholder})
        file(TOUCH ${CMAKE_SOURCE_DIR}/${placeholder})
    endif()
endforeach()

if (BUILD_TESTS AND NOT ANDROID)
    add_subdirectory(test)
endif()

file(GLOB PROFILES_FILES ${CMAKE_CURRENT_LIST_DIR})
source_group("Python Files" FILES ${PROFILES_SCRIPT})

set(PROFILES_FILES_FOR_INSTALL
    ${CMAKE_CURRENT_LIST_DIR}/build/VP_LUNARG_minimum_requirements.json
    ${CMAKE_CURRENT_LIST_DIR}/build/VP_LUNARG_desktop_baseline.json
    ${CMAKE_CURRENT_LIST_DIR}/build/VP_ANDROID_vulkan_profile_2021.json
    ${CMAKE_CURRENT_LIST_DIR}/build/VP_ANDROID_vulkan_profile_2022.json
    ${CMAKE_CURRENT_LIST_DIR}/build/VP_ANDROID_vulkan_profile_2025.json
    ${CMAKE_CURRENT_LIST_DIR}/build/VP_KHR_roadmap.json
)

if(WIN32)
    set(VKPROFILES_EXE "${PROJECT_SOURCE_DIR}/scripts/vkprofiles.exe")
else()
    set(VKPROFILES_EXE "${PROJECT_SOURCE_DIR}/scripts/vkprofiles")
endif()

set(VULKAN_REGISTRY_FILES_DIR ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry)

# Input profiles of the generation graph, the generated outputs are excluded so the graph doesn't depend on itself
file(GLOB_RECURSE PROFILES_GRAPH_INPUTS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/LunarG/VP_LUNARG_desktop_baseline_*/*.json
    ${CMAKE_CURRENT_LIST_DIR}/Android/*.json)

# Files written on every execution of the generation graph, used by CMake to only rerun the graph when an input changed
set(PROFILES_GRAPH_OUTPUTS
    ${PROJECT_SOURCE_DIR}/schema/${PROFILES_SCHEMA_FILENAME}
    ${CMAKE_CURRENT_LIST_DIR}/build/VP_LUNARG_minimum_requirements.json
    ${CMAKE_CURRENT_LIST_DIR}/build/VP_LUNARG_desktop_baseline.json
    ${PROJECT_SOURCE_DIR}/library/include/vulkan/vulkan_profiles.h
    ${PROJECT_SOURCE_DIR}/library/include/vulkan/vulkan_profiles.hpp
    ${PROJECT_SOURCE_DIR}/library/source/vulkan_profiles.cpp
    ${PROJECT_SOURCE_DIR}/library/include/vulkan/debug/vulkan_profiles.hpp
    ${PROJECT_SOURCE_DIR}/library/include/vulkan/android_vulkan_profiles.hpp
    ${PROJECT_SOURCE_DIR}/library/include/vulkan/android_vulkan_profiles_debug.hpp
    ${PROJECT_SOURCE_DIR}/library/include/vulkan/android_vulkan_requirements_debug.hpp
    ${PROJECT_SOURCE_DIR}/PROFILES.md
    ${PROJECT_SOURCE_DIR}/PROFILES_DESKTOP.md
    ${PROJECT_SOURCE_DIR}/PROFILES_ANDROID.md
)

# Execute the full generation graph
add_custom_command(
    OUTPUT ${PROFILES_GRAPH_OUTPUTS}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/schema
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_LIST_DIR}/build
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/library/include/vulkan/debug
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/library/source/debug
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${VULKAN_REGISTRY_FILES_DIR}/profiles/VP_KHR_roadmap.json
        ${CMAKE_CURRENT_LIST_DIR}/build/VP_KHR_roadmap.json
    COMMAND ${VKPROFILES_EXE} transform
        --strict
        --registry ${VULKAN_REGISTRY_FILES_DIR}/vk.xml
        --input ${CMAKE_CURRENT_LIST_DIR}/LunarG/VP_LUNARG_minimum_requirements.json
        --output ${CMAKE_CURRENT_LIST_DIR}/build/VP_LUNARG_minimum_requirements.json
        --pull aliases ignore-extension-versions
        --format flatten
    COMMAND ${VKPROFILES_EXE} graph
        --strict
        --registry ${VULKAN_REGISTRY_FILES_DIR}/vk.xml
        --input ${CMAKE_CURRENT_LIST_DIR}/vkprofiles_graph.json
        --verbose
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_LIST_DIR}/build/VP_LUNARG_desktop_baseline.json
        ${CMAKE_CURRENT_LIST_DIR}/LunarG/VP_LUNARG_desktop_baseline.json
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    DEPENDS
        ${VKPROFILES_EXE}
        ${VULKAN_REGISTRY_FILES_DIR}/vk.xml
        ${VULKAN_REGISTRY_FILES_DIR}/video.xml
        ${VULKAN_REGISTRY_FILES_DIR}/profiles/VP_KHR_roadmap.json
        ${CMAKE_CURRENT_LIST_DIR}/vkprofiles_graph.json
        ${CMAKE_CURRENT_LIST_DIR}/LunarG/VP_LUNARG_minimum_requirements.json
        ${PROFILES_GRAPH_INPUTS}
    COMMENT "Executing the Vulkan Profiles generation graph..."
    VERBATIM)

add_custom_target(VpGenerate-Profiles ALL
    SOURCES ${PROFILES_FILES}
    DEPENDS ${PROFILES_GRAPH_OUTPUTS})

set_target_properties(VpGenerate-Profiles PROPERTIES FOLDER "Profiles generator")
add_dependencies(VpGenerate-Profiles VpProfilesProcessor)

set(json_install_dir "${CMAKE_INSTALL_DATADIR}/vulkan/config/VK_LAYER_KHRONOS_profiles")
if (WIN32)
    set(json_install_dir "Config/VK_LAYER_KHRONOS_profiles")
endif()

install(
    FILES 
        ${PROFILES_FILES_FOR_INSTALL}
    DESTINATION
        ${json_install_dir}
)

install(
    FILES
        ${MERGE_SCRIPT}
        ${SOLUTION_SCRIPT}
        ${PROJECT_SOURCE_DIR}/schema/${PROFILES_SCHEMA_FILENAME}
    DESTINATION
        "${CMAKE_INSTALL_DATADIR}/vulkan/registry"
)