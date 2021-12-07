set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 11)

function(DevsimTest NAME)
	set(TEST_FILE ./${NAME}.cpp)
    set(TEST_NAME devsim_${NAME})
    
    file(GLOB FILES_JSON ./*.json)

    add_executable(${TEST_NAME} ${TEST_FILE} ${FILES_JSON} devsim_test_helper.h devsim_test_helper.cpp ${jsoncpp_SOURCE_DIR}/dist/jsoncpp.cpp)
    target_link_libraries(${TEST_NAME} gtest gtest_main Vulkan::Vulkan)
    target_compile_definitions(${TEST_NAME} PUBLIC TEST_SOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}")
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    # try to be OS-independent
    if(WIN32)
        string(REPLACE ";" "\\;" path "$ENV{PATH}")
        string(REPLACE "/" "\\" binary_dir "${CMAKE_BINARY_DIR}\\bin\\${CMAKE_BUILD_TYPE}")
        set(run_environment "VK_LAYER_PATH=${binary_dir}" "PATH=${binary_dir}\;${path}")
    elseif(UNIX)
        set(run_environment "VK_LAYER_PATH=${CMAKE_BINARY_DIR}/lib" "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/lib:$ENV{LD_LIBRARY_PATH}")
    else()
        message(FATAL_ERROR "Unsupported Platform ${CMAKE_SYSTEM_NAME}")
    endif()

    set_tests_properties(${TEST_NAME} PROPERTIES ENVIRONMENT "${run_environment}")
endfunction(DevsimTest)

DevsimTest(manual_devsim_tests)
