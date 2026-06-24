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
# ~~~

set(TEST_FILE test_validate.cpp)
set(TEST_NAME VpProfile_test_schema_validation)

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
add_dependencies(${TEST_NAME} VpGenerated)

target_link_libraries(${TEST_NAME} PRIVATE GTest::gtest GTest::gtest_main jsoncpp_static valijson)
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} --gtest_catch_exceptions=0)

set_target_properties(${TEST_NAME} PROPERTIES FOLDER "Profiles schema")

add_dependencies(VpProfile_test_schema_validation VpGenerated)

function(add_python_unit_test test_base_name)
    # Check and install 'vulkan-object' if missing, on Windows multiple python installed could be in different place depending on the IDE...
    if(WIN32)
        get_property(vulkan_object_checked GLOBAL PROPERTY VULKAN_OBJECT_CHECKED)
        if(NOT vulkan_object_checked)
            # Fetch the path of the discovered Python interpreter
            get_target_property(PYTHON_EXE Python3::Interpreter IMPORTED_LOCATION)
            if(NOT PYTHON_EXE)
                set(PYTHON_EXE "${Python3_EXECUTABLE}")
            endif()

            message(STATUS "Checking for Python module 'vulkan_object'...")
        
            # See if it's already installed to avoid unnecessary pip calls
            execute_process(
                COMMAND "${PYTHON_EXE}" -c "import vulkan_object"
                RESULT_VARIABLE ERROR_CODE
                OUTPUT_QUIET
                ERROR_QUIET
            )

            # If not installed (ERROR_CODE is non-zero), install it
            if(NOT ERROR_CODE EQUAL 0)
                message(STATUS "Module 'vulkan_object' not found. Installing via pip...")
                execute_process(
                    COMMAND "${PYTHON_EXE}" -m pip install vulkan-object
                    RESULT_VARIABLE pip_status
                )
            
                if(NOT pip_status EQUAL 0)
                    message(FATAL_ERROR "Failed to install 'vulkan-object'. Please install it manually.")
                endif()
            else()
                message(STATUS "Module 'vulkan_object' is already installed.")
            endif()

            # Cache the check globally so it only runs once per CMake configuration
            set_property(GLOBAL PROPERTY VULKAN_OBJECT_CHECKED TRUE)
        endif()
    endif()

    # Construct the target name and the path to the script
    set(target_name "python_unit_test_${test_base_name}")
    set(script_path "${PROJECT_SOURCE_DIR}/scripts/tests/test_${test_base_name}.py")

    # Change the command from a direct file path to a module invocation (-m)
    # add_custom_target(${target_name}
    #     COMMAND Python3::Interpreter -m scripts.tests.test_${test_base_name} -v
    #     WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    #     VERBATIM
    #     SOURCES "${script_path}"
    #     DEPENDS "${script_path}"
    # )

    # set_target_properties(${target_name} PROPERTIES FOLDER "Profiles schema")
    # add_dependencies(${target_name} VpGenerated)

    # Apply the same module invocation fix to the CTest definition
    add_test(NAME ${target_name} 
        COMMAND Python3::Interpreter -m scripts.tests.test_${test_base_name} -v
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endfunction()

if (BUILD_TESTS_EXTRA)
    add_python_unit_test(util)
    add_python_unit_test(json)
    add_python_unit_test(expression_tree)
    add_python_unit_test(vk_xml_parsing)
endif()
