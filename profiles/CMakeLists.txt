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

if (BUILD_TESTS AND NOT ANDROID)
    add_subdirectory(test)
endif()

file(GLOB PROFILES_FILES ${CMAKE_CURRENT_LIST_DIR})
source_group("Python Files" FILES ${PROFILES_SCRIPT})

set(PROFILES_FILES_FOR_INSTALL
    VP_LUNARG_minimum_requirements.json
    VP_LUNARG_desktop_baseline.json
    VP_ANDROID_vulkan_profile_2021.json
    VP_ANDROID_vulkan_profile_2022.json
    VP_ANDROID_vulkan_profile_2025.json
    VP_KHR_roadmap.json
)

if(WIN32)
    set(VKPROFILES_EXE "${PROJECT_SOURCE_DIR}/scripts/vkprofiles.exe")
else()
    set(VKPROFILES_EXE "${PROJECT_SOURCE_DIR}/scripts/vkprofiles")
endif()

# Single custom target executing full generation graph
add_custom_target(VpGenerate-Profiles ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/schema
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/library/include/vulkan/debug
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/library/source/debug
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/profiles/VP_KHR_roadmap.json
        ${CMAKE_CURRENT_LIST_DIR}/VP_KHR_roadmap.json
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_LIST_DIR}/LunarG/VP_LUNARG_minimum_requirements.json
        ${CMAKE_CURRENT_LIST_DIR}/VP_LUNARG_minimum_requirements.json
    COMMAND ${VKPROFILES_EXE} graph
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_LIST_DIR}/vkprofiles_graph.json
        --verbose
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_LIST_DIR}/LunarG/VP_LUNARG_desktop_baseline.json
        ${CMAKE_CURRENT_LIST_DIR}/VP_LUNARG_desktop_baseline.json
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    VERBATIM
    SOURCES ${PROFILES_FILES}
    DEPENDS ${PROFILES_FILES} python_venv)

set_target_properties(VpGenerate-Profiles PROPERTIES FOLDER "Profiles generator")
add_dependencies(VpGenerate-Profiles VpProfilesProcessor python_venv)

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