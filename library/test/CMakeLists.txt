set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Vulkan REQUIRED)

link_libraries(${Vulkan_LIBRARIES})
include_directories(${vulkan-headers_SOURCE_DIR}/include ../include)

function(add_unit_test NAME)
	set(TEST_FILE ./${NAME}.cpp)
    set(TEST_NAME library_${NAME})

    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} gtest gtest_main)
    add_dependencies(${TEST_NAME} vulkan_profiles)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction(add_unit_test)

add_unit_test(test_util)
add_unit_test(test_profile_create)
add_unit_test(test_hand_written_profile)
