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

function(add_vulkan_python_test TARGET_NAME SCRIPT_FILENAME)
    set(REGISTRY_PATH "${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry")
    set(FULL_SCRIPT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT_FILENAME}")

    # Register the script as a CTest test case
    add_test(NAME ${TARGET_NAME}
        COMMAND ${VENV_PYTHON_EXECUTABLE} "${FULL_SCRIPT_PATH}" --registry "${REGISTRY_PATH}/vk.xml"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )

    # Cleanly inject environment variables into the test environment
    set_tests_properties(${TARGET_NAME} PROPERTIES
        ENVIRONMENT "PYTHONPATH=${REGISTRY_PATH}"
    )
endfunction()

if(NOT APPLE) # macOS doesn't handle dynamically loading Python jsonschema'
    add_vulkan_python_test(VpProfilesProcessor_TestVulkanProfilesUtil       test_util.py)
    add_vulkan_python_test(VpProfilesProcessor_TestVulkanObjectEmpty        test_init.py)
    add_vulkan_python_test(VpProfilesProcessor_TestVulkanVersion            test_vulkan_version.py)
    add_vulkan_python_test(VpProfilesProcessor_TestVulkanObjectUtils        test_vulkan_object_utils.py)
    add_vulkan_python_test(VpProfilesProcessor_TestVulkanObjectDepends      test_vulkan_object_expression_parsing.py)
    add_vulkan_python_test(VpProfilesProcessor_TestVulkanProfilesJson       test_vulkan_profiles.py)
    add_vulkan_python_test(VpProfilesProcessor_TestFormatFlagsConverter     test_format_flags.py)
    add_vulkan_python_test(VpProfilesProcessor_TestConvertPullRequiredExts  test_convert_pull_required_extensions.py)
    add_vulkan_python_test(VpProfilesProcessor_TestConvertPullRequiredCaps  test_convert_pull_required_capabilities.py)
    add_vulkan_python_test(VpProfilesProcessor_TestConvertPullPromotedExts  test_convert_pull_promoted_extensions.py)
    add_vulkan_python_test(VpProfilesProcessor_TestConvertPullAliases       test_convert_pull_aliases.py)
    add_vulkan_python_test(VpProfilesProcessor_TestConvertStripDups         test_convert_strip_duplication.py)
    add_vulkan_python_test(VpProfilesProcessor_TestConvertConsolidate       test_convert_consolidate.py)
    add_vulkan_python_test(VpProfilesProcessor_TestConvertStripPromotedExts test_convert_strip_promoted_extensions.py)
    add_vulkan_python_test(VpProfilesProcessor_TestValidate                 test_validate.py)
endif()

# Convert Android Profiles custom target
set(REGISTRY_PATH "${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry")
set(PROFILES_INPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../profiles/Android")
set(PROFILES_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/profiles")

add_custom_target(convert_android
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROFILES_OUTPUT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E env "PYTHONPATH=${REGISTRY_PATH}"
        ${VENV_PYTHON_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/../profiles.py" convert
        --registry "${REGISTRY_PATH}/vk.xml"
        --input "${PROFILES_INPUT_DIR}"
        --output "${PROFILES_OUTPUT_DIR}"
        --mode pull-extension-dependencies
               pull-promoted-extensions
               ignore-extension-versions
               pull-required-capabilities
               pull-aliases
               strip-duplication
        --format flatten
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Testing vkprofiles with Android Vulkan profiles..."
    VERBATIM
)

