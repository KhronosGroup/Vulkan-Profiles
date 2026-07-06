# ~~~
# Copyright (c) 2026-2026 LunarG, Inc.
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

find_package(Python3 REQUIRED COMPONENTS Interpreter)

#add_custom_target(VpTestVulkanObjectEmpty ALL
#    COMMAND ${CMAKE_COMMAND} -E env "PYTHONPATH=${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry"
#        ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_empty.py
#        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
#    VERBATIM
#    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
#    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/test_empty.py
#    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test_empty.py)

#set_target_properties(VpTestVulkanObjectEmpty PROPERTIES FOLDER "Profiles generator/Tests")

#add_custom_target(VpTestVulkanObjectUtils ALL
#    COMMAND ${CMAKE_COMMAND} -E env "PYTHONPATH=${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry"
#        ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_vulkan_object_utils.py
#        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
#    VERBATIM
#    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
#    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/test_vulkan_object_utils.py
#    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test_vulkan_object_utils.py)

#set_target_properties(VpTestVulkanObjectUtils PROPERTIES FOLDER "Profiles generator/Tests")

#add_custom_target(VpTestVulkanProfilesJson ALL
#    COMMAND ${CMAKE_COMMAND} -E env "PYTHONPATH=${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry"
#        ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_json.py
#        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
#    VERBATIM
#    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
#    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/test_json.py
#    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test_json.py)

# set_target_properties(VpTestVulkanProfilesJson PROPERTIES FOLDER "Profiles generator/Tests")

function(add_vulkan_python_test TARGET_NAME SCRIPT_FILENAME)
    set(REGISTRY_PATH "${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry")
    set(FULL_SCRIPT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT_FILENAME}")

    add_custom_target(${TARGET_NAME} ALL
        COMMAND ${CMAKE_COMMAND} -E env "PYTHONPATH=${REGISTRY_PATH}"
            ${Python3_EXECUTABLE} "${FULL_SCRIPT_PATH}"
            --registry "${REGISTRY_PATH}/vk.xml"
        VERBATIM
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        SOURCES "${FULL_SCRIPT_PATH}"
        DEPENDS "${FULL_SCRIPT_PATH}"
    )

    set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Profiles generator/Tests")
endfunction()

add_vulkan_python_test(VpTestVulkanObjectEmpty   test_empty.py)
add_vulkan_python_test(VpTestVulkanObjectUtils   test_vulkan_object_utils.py)
add_vulkan_python_test(VpTestVulkanProfilesJson  test_json.py)
add_vulkan_python_test(VpTestVulkanProfilesUtil  test_util.py)
add_vulkan_python_test(VpTestVulkanObjectXML     test_vk_xml_parsing.py)
add_vulkan_python_test(VpTestVulkanObjectDepends test_expression_tree.py)
