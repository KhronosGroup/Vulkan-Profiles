set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(TEST_FILE test_validate.cpp)
set(TEST_NAME profile_test_validate)

include_directories(${jsoncpp_SOURCE_DIR}/include)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_executable(${TEST_NAME} ${TEST_FILE} ${jsoncpp_SOURCE_DIR}/dist/jsoncpp.cpp ${jsoncpp_SOURCE_DIR}/include/json/json.h)
add_custom_command(TARGET ${TEST_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory  ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR})
add_custom_command(TARGET ${TEST_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different  ${CMAKE_CURRENT_SOURCE_DIR}/../../schema/profile.json ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${TEST_NAME} PRIVATE gtest gtest_main)
add_dependencies(${TEST_NAME} vulkan_profiles)
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} --gtest_catch_exceptions=0)
