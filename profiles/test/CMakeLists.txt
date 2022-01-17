set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(TEST_FILE test_validate.cpp)
set(TEST_NAME VpProfile_test_schema_validation)

file(GLOB FILES_UNIT_TEST ${CMAKE_CURRENT_SOURCE_DIR}/data/*.json)
file(GLOB FILES_PROFILE ${CMAKE_CURRENT_SOURCE_DIR}/../*.json)
file(GLOB SCHEMA_PROFILE ${CMAKE_CURRENT_SOURCE_DIR}/../../schema/profile_schema.json)

source_group("JSON Files" FILES ${FILES_PROFILE} ${FILES_UNIT_TEST})
source_group("JSON Schema" FILES ${SCHEMA_PROFILE})

include_directories(${jsoncpp_generated_include_dir})

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_executable(${TEST_NAME} ${TEST_FILE} ${jsoncpp_generated_source} ${jsoncpp_generated_header} ${FILES_PROFILE} ${FILES_UNIT_TEST} ${SCHEMA_PROFILE})
add_custom_command(TARGET ${TEST_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory  ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR})
add_custom_command(TARGET ${TEST_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory  ${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR})
add_custom_command(TARGET ${TEST_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different  ${CMAKE_CURRENT_SOURCE_DIR}/../../schema/profile_schema.json ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${TEST_NAME} PRIVATE gtest gtest_main)
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} --gtest_catch_exceptions=0)

set_target_properties(${TEST_NAME} PROPERTIES FOLDER "Profiles schema")
