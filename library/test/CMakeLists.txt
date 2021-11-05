set(CMAKE_POSITION_INDEPENDENT_CODE ON)

function(add_unit_test NAME)
	set(TEST_FILE ./${NAME}.cpp)
    set(TEST_NAME library_${NAME})

    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} PRIVATE gtest gtest_main Vulkan::Headers Vulkan::Profiles vulkan)
    add_dependencies(${TEST_NAME} vulkan_profiles)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} --gtest_catch_exceptions=0)
endfunction(add_unit_test)

add_unit_test(test_util)
add_unit_test(test_profile_api)
add_unit_test(test_profile_api_not_beta)
add_unit_test(test_profile_api_generated)
add_unit_test(test_profile_support)
add_unit_test(test_profile_examples)
