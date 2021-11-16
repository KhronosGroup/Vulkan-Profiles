set(CMAKE_POSITION_INDEPENDENT_CODE ON)

function(add_unit_test NAME)
	set(TEST_FILE ./${NAME}.cpp)
    set(TEST_NAME_HO library_${NAME}_header_only)
    set(TEST_NAME_WS library_${NAME}_with_source)

    add_executable(${TEST_NAME_HO} ${TEST_FILE})
    target_compile_definitions(${TEST_NAME_HO} PUBLIC "VK_ENABLE_BETA_EXTENSIONS=1")
    target_link_libraries(${TEST_NAME_HO} PRIVATE gtest gtest_main Vulkan::Headers Vulkan::Profiles vulkan)
    #add_dependencies(${TEST_NAME_HO} library_generator)
    add_test(NAME ${TEST_NAME_HO} COMMAND ${TEST_NAME_HO} --gtest_catch_exceptions=0)

    add_executable(${TEST_NAME_WS} ${TEST_FILE} ../source/vulkan_profiles.cpp)
    target_compile_definitions(${TEST_NAME_WS} PUBLIC "VULKAN_PROFILES_HEADER_ONLY=1")
    target_compile_definitions(${TEST_NAME_WS} PUBLIC "VK_ENABLE_BETA_EXTENSIONS=1")
    target_link_libraries(${TEST_NAME_WS} PRIVATE gtest gtest_main Vulkan::Headers Vulkan::Profiles vulkan)
    #add_dependencies(${TEST_NAME_WS} library_generator)
    add_test(NAME ${TEST_NAME_WS} COMMAND ${TEST_NAME_WS} --gtest_catch_exceptions=0)
endfunction(add_unit_test)

add_unit_test(test_api_create_device)
add_unit_test(test_api_get_profiles_beta)
add_unit_test(test_api_get_profile_extensions)
add_unit_test(test_api_get_profile_formats)
add_unit_test(test_api_get_profile_memory_types)
add_unit_test(test_api_get_profile_queue_families)
add_unit_test(test_api_get_profile_structures)
add_unit_test(test_api_get_profile_support)
add_unit_test(test_profile_support)
add_unit_test(test_profile_examples)

function(add_unit_test_core NAME)
	set(TEST_FILE ./${NAME}.cpp)
    set(TEST_NAME_HO library_${NAME}_header_only)
    set(TEST_NAME_WS library_${NAME}_with_source)

    add_executable(${TEST_NAME_HO} ${TEST_FILE})
    target_link_libraries(${TEST_NAME_HO} PRIVATE gtest gtest_main Vulkan::Headers Vulkan::Profiles vulkan)
    #add_dependencies(${TEST_NAME_HO} library_generator)
    add_test(NAME ${TEST_NAME_HO} COMMAND ${TEST_NAME_HO} --gtest_catch_exceptions=0)

    add_executable(${TEST_NAME_WS} ${TEST_FILE} ../source/vulkan_profiles.cpp)
    target_compile_definitions(${TEST_NAME_WS} PUBLIC "VULKAN_PROFILES_HEADER_ONLY=1")
    target_link_libraries(${TEST_NAME_WS} PRIVATE gtest gtest_main Vulkan::Headers Vulkan::Profiles vulkan)
    #add_dependencies(${TEST_NAME_WS} library_generator)
    add_test(NAME ${TEST_NAME_WS} COMMAND ${TEST_NAME_WS} --gtest_catch_exceptions=0)
endfunction(add_unit_test_core)

add_unit_test_core(test_api_get_profiles_core)

function(add_unit_test_simple NAME)
	set(TEST_FILE ./${NAME}.cpp)
    set(TEST_NAME_HO library_${NAME})

    add_executable(${TEST_NAME_HO} ${TEST_FILE})
    target_link_libraries(${TEST_NAME_HO} PRIVATE gtest gtest_main Vulkan::Headers Vulkan::Profiles vulkan)
    #add_dependencies(${TEST_NAME_HO} library_generator)
    add_test(NAME ${TEST_NAME_HO} COMMAND ${TEST_NAME_HO} --gtest_catch_exceptions=0)
endfunction(add_unit_test_simple)

add_unit_test_simple(test_util)


