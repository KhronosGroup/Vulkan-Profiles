set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 11)

function(LayerTest NAME)
	set(TEST_FILE ./${NAME}.cpp)
    set(TEST_NAME VkLayer_${NAME})
    
    file(GLOB FILES_JSON ./*.json)

    add_executable(${TEST_NAME} ${TEST_FILE} ${FILES_JSON} profiles_test_helper.h profiles_test_helper.cpp ${jsoncpp_SOURCE_DIR}/dist/jsoncpp.cpp)
    add_dependencies(${TEST_NAME} VkLayer_khronos_profiles)

    target_include_directories(${TEST_NAME} PUBLIC "${vulkan-headers_SOURCE_DIR}/include")
    target_link_libraries(${TEST_NAME} gtest gtest_main)
    if(ANDROID)
        # Android executables generally need these libraries.
        target_link_libraries(${TEST_NAME} log android dl vulkan)
    else()
        # Everyone else links to the built loader.
        target_link_libraries(${TEST_NAME} Vulkan::Vulkan)
    endif()
    target_compile_definitions(${TEST_NAME} PUBLIC TEST_SOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}")
    target_compile_definitions(${TEST_NAME} PUBLIC TEST_BINARY_PATH="${CMAKE_BINARY_DIR}/")
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    # try to be OS-independent
    if(WIN32)
        string(REPLACE ";" "\\;" path "$ENV{PATH}")
        string(REPLACE "/" "\\" binary_dir "${CMAKE_BINARY_DIR}\\bin")
        set(run_environment "VK_LAYER_PATH=${binary_dir}" "PATH=${binary_dir}\;${path}")
    elseif(UNIX)
        set(run_environment "VK_LAYER_PATH=${CMAKE_BINARY_DIR}/lib" "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/lib:$ENV{LD_LIBRARY_PATH}")
    else()
        message(FATAL_ERROR "Unsupported Platform ${CMAKE_SYSTEM_NAME}")
    endif()

    set_tests_properties(${TEST_NAME} PROPERTIES ENVIRONMENT "${run_environment}")
    set_target_properties(${TEST_NAME} PROPERTIES FOLDER "Profiles layer")
endfunction(LayerTest)

LayerTest(tests)
LayerTest(tests_promoted)
LayerTest(tests_mechanism)
LayerTest(tests_desktop_portability)
LayerTest(tests_capabilities)
