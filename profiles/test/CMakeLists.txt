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

set(TEST_FILE test_validate.cpp)
set(TEST_NAME VpProfile_test_schema_validation)

set(REGISTRY_PATH "${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry")

file(GLOB FILES_UNIT_TEST ${CMAKE_CURRENT_SOURCE_DIR}/data/*.json)
file(GLOB FILES_PROFILE ${CMAKE_CURRENT_SOURCE_DIR}/../*.json)
file(GLOB SCHEMA_PROFILE ${CMAKE_CURRENT_SOURCE_DIR}/../../schema/${PROFILES_SCHEMA_FILENAME})

source_group("JSON Files" FILES ${FILES_PROFILE} ${FILES_UNIT_TEST})
source_group("JSON Schema" FILES ${SCHEMA_PROFILE})

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_executable(${TEST_NAME} ${TEST_FILE} ${FILES_PROFILE} ${FILES_UNIT_TEST} ${SCHEMA_PROFILE})
target_compile_definitions(${TEST_NAME} PRIVATE 
    PROFILES_SCHEMA_FILENAME="${CMAKE_SOURCE_DIR}/schema/${PROFILES_SCHEMA_FILENAME}" 
    JSON_TEST_FILES_PATH="${CMAKE_SOURCE_DIR}/profiles/test/data/"
    PROFILE_FILES_PATH="${CMAKE_SOURCE_DIR}/profiles/")
target_link_libraries(${TEST_NAME} PRIVATE GTest::gtest GTest::gtest_main jsoncpp_static valijson)
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} --gtest_catch_exceptions=0)
set_target_properties(${TEST_NAME} PROPERTIES FOLDER "Profiles schema")
set_tests_properties(${TEST_NAME} PROPERTIES ENVIRONMENT "PYTHONPATH=${REGISTRY_PATH}")
add_dependencies(${TEST_NAME} VpGenerate-Profiles)

if(WIN32)
    set(VKPROFILES_EXE "${PROJECT_SOURCE_DIR}/scripts/vkprofiles.exe")
else()
    set(VKPROFILES_EXE "${PROJECT_SOURCE_DIR}/scripts/vkprofiles")
endif()

add_custom_target(VpTestIntersect ALL
    COMMAND ${VKPROFILES_EXE} combine
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_test_combine_intersect
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_test_combine_intersect.json
        --profile-name VP_LUNARG_test_combine_intersect
        --mode intersection
    VERBATIM
    SOURCES ${FILES_UNIT_TEST}
    DEPENDS ${FILES_UNIT_TEST})
set_target_properties(VpTestIntersect PROPERTIES FOLDER "Profiles generator/Tests")
add_dependencies(VpTestIntersect VpProfilesProcessor)

add_custom_target(VpTestUnion ALL
    COMMAND ${VKPROFILES_EXE} combine
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_test_combine_union
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_test_combine_union.json
        --profile-name VP_LUNARG_test_combine_union
        --mode union
    VERBATIM
    SOURCES ${FILES_UNIT_TEST}
    DEPENDS ${FILES_UNIT_TEST})
set_target_properties(VpTestUnion PROPERTIES FOLDER "Profiles generator/Tests")
add_dependencies(VpTestUnion VpProfilesProcessor)

add_custom_target(VpTestGeneratedName ALL
    COMMAND ${VKPROFILES_EXE} combine
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_test_combine_intersect
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_test_generated_name.json
    VERBATIM
    SOURCES ${FILES_UNIT_TEST}
    DEPENDS ${FILES_UNIT_TEST})
set_target_properties(VpTestGeneratedName PROPERTIES FOLDER "Profiles generator/Tests")
add_dependencies(VpTestGeneratedName VpProfilesProcessor)

add_custom_target(VpTestHostImageCopy ALL
    COMMAND ${VKPROFILES_EXE} combine
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_test_host_image_copy
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_test_host_image_copy.json
    VERBATIM
    SOURCES ${FILES_UNIT_TEST}
    DEPENDS ${FILES_UNIT_TEST})
set_target_properties(VpTestHostImageCopy PROPERTIES FOLDER "Profiles generator/Tests")
add_dependencies(VpTestHostImageCopy VpProfilesProcessor)

add_custom_target(VpTestGeneratedRequirements ALL
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/1_transform ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/2_extract ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/3_consolidate ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/4_diff ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/5_sort
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/1_transform
    COMMAND ${VKPROFILES_EXE} transform
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/../LunarG/VP_LUNARG_minimum_requirements.json
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/1_transform
        --pull requirements aliases ignore-extension-versions
        --sort
        --format flatten
    COMMAND ${VKPROFILES_EXE} transform
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/1_transform/VP_LUNARG_generated_requirements.json
        --pull requirements aliases ignore-extension-versions
        --sort
        --format flatten
    COMMAND ${VKPROFILES_EXE} extract
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/1_transform
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/2_extract/vulkan10/VP_LUNARG_gen_min_requirements_1_0.json
        --input-profiles VP_LUNARG_gen_min_requirements_1_0
        --mode pull-required-profiles
        --format flatten
    COMMAND ${VKPROFILES_EXE} extract
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/1_transform
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/2_extract/vulkan10/VP_LUNARG_minimum_requirements_1_0.json
        --input-profiles VP_LUNARG_minimum_requirements_1_0
        --mode pull-required-profiles
        --format flatten
    COMMAND ${VKPROFILES_EXE} extract
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/1_transform
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/2_extract/vulkan11/VP_LUNARG_gen_min_requirements_1_1.json
        --input-profiles VP_LUNARG_gen_min_requirements_1_1
        --mode pull-required-profiles
        --format flatten
    COMMAND ${VKPROFILES_EXE} extract
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/1_transform
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/2_extract/vulkan11/VP_LUNARG_minimum_requirements_1_1.json
        --input-profiles VP_LUNARG_minimum_requirements_1_1
        --mode pull-required-profiles
        --format flatten
    COMMAND ${VKPROFILES_EXE} extract
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/1_transform
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/2_extract/vulkan12/VP_LUNARG_gen_min_requirements_1_2.json
        --input-profiles VP_LUNARG_gen_min_requirements_1_2
        --mode pull-required-profiles
        --format flatten
    COMMAND ${VKPROFILES_EXE} extract
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/1_transform
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/2_extract/vulkan12/VP_LUNARG_minimum_requirements_1_2.json
        --input-profiles VP_LUNARG_minimum_requirements_1_2
        --mode pull-required-profiles
        --format flatten
    COMMAND ${VKPROFILES_EXE} extract
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/1_transform
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/2_extract/vulkan13/VP_LUNARG_gen_min_requirements_1_3.json
        --input-profiles VP_LUNARG_gen_min_requirements_1_3
        --mode pull-required-profiles
        --format flatten
    COMMAND ${VKPROFILES_EXE} extract
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/1_transform
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/2_extract/vulkan13/VP_LUNARG_minimum_requirements_1_3.json
        --input-profiles VP_LUNARG_minimum_requirements_1_3
        --mode pull-required-profiles
        --format flatten
    COMMAND ${VKPROFILES_EXE} extract
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/1_transform
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/2_extract/vulkan14/VP_LUNARG_gen_min_requirements_1_4.json
        --input-profiles VP_LUNARG_gen_min_requirements_1_4
        --mode pull-required-profiles
        --format flatten
    COMMAND ${VKPROFILES_EXE} extract
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/1_transform
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/2_extract/vulkan14/VP_LUNARG_minimum_requirements_1_4.json
        --input-profiles VP_LUNARG_minimum_requirements_1_4
        --mode pull-required-profiles
        --format flatten
    COMMAND ${VKPROFILES_EXE} transform
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/2_extract/vulkan10
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/3_consolidate/vulkan10
        --consolidate
        --sort
        --format flatten
    COMMAND ${VKPROFILES_EXE} transform
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/2_extract/vulkan11
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/3_consolidate/vulkan11
        --consolidate
        --sort
        --format flatten
    COMMAND ${VKPROFILES_EXE} transform
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/2_extract/vulkan12
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/3_consolidate/vulkan12
        --consolidate
        --sort
        --format flatten
    COMMAND ${VKPROFILES_EXE} transform
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/2_extract/vulkan13
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/3_consolidate/vulkan13
        --consolidate
        --sort
        --format flatten
    COMMAND ${VKPROFILES_EXE} transform
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/2_extract/vulkan14
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/3_consolidate/vulkan14
        --consolidate
        --sort
        --format flatten
    COMMAND ${VKPROFILES_EXE} combine
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/3_consolidate/vulkan10
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/4_diff/vulkan10
        --mode difference
        --format flatten
    COMMAND ${VKPROFILES_EXE} combine
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/3_consolidate/vulkan11
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/4_diff/vulkan11
        --mode difference
        --format flatten
    COMMAND ${VKPROFILES_EXE} combine
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/3_consolidate/vulkan12
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/4_diff/vulkan12
        --mode difference
        --format flatten
    COMMAND ${VKPROFILES_EXE} combine
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/3_consolidate/vulkan13
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/4_diff/vulkan13
        --mode difference
        --format flatten
    COMMAND ${VKPROFILES_EXE} combine
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/3_consolidate/vulkan14
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/4_diff/vulkan14
        --mode difference
        --format flatten
    COMMAND ${VKPROFILES_EXE} transform
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/4_diff/vulkan10
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/5_sort/vulkan10
        --sort
        --format flatten
    COMMAND ${VKPROFILES_EXE} transform
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/4_diff/vulkan11
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/5_sort/vulkan11
        --sort
        --format flatten
    COMMAND ${VKPROFILES_EXE} transform
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/4_diff/vulkan12
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/5_sort/vulkan12
        --sort
        --format flatten
    COMMAND ${VKPROFILES_EXE} transform
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/4_diff/vulkan13
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/5_sort/vulkan13
        --sort
        --format flatten
    COMMAND ${VKPROFILES_EXE} transform
        --registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
        --input ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/4_diff/vulkan14
        --output ${CMAKE_CURRENT_SOURCE_DIR}/data/VP_LUNARG_generated_requirements/5_sort/vulkan14
        --sort
        --format flatten
    VERBATIM
    SOURCES ${FILES_UNIT_TEST}
    DEPENDS ${FILES_UNIT_TEST})
set_target_properties(VpTestGeneratedRequirements PROPERTIES FOLDER "Profiles generator/Tests")
add_dependencies(VpTestGeneratedRequirements VpProfilesProcessor)
