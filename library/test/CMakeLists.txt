set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(test_libraries GTest::gtest GTest::gtest_main Vulkan::Headers Vulkan::Profiles)
if(ANDROID)
    list(APPEND test_libraries log android dl vulkan)
else()
    list(APPEND test_libraries Vulkan::Vulkan)
endif()

# The release source is in ../source/vulkan_profiles.cpp, but the debug source (with
# verbose messages during profile validation) is in ../source/debug/vulkan_profiles.cpp.
set(vulkan_profiles_source ../source$<$<CONFIG:Debug>:/debug>/vulkan_profiles.cpp)

function(add_unit_test NAME)
	set(TEST_FILE ./${NAME}.cpp)
    set(TEST_NAME_HO VpLibrary_${NAME}_header_only)
    set(TEST_NAME_WS VpLibrary_${NAME}_with_source)

    add_executable(${TEST_NAME_HO} ${TEST_FILE})
    target_compile_definitions(${TEST_NAME_HO} PUBLIC "VK_ENABLE_BETA_EXTENSIONS=1")
    target_include_directories(${TEST_NAME_HO} PUBLIC "${vulkan-headers_SOURCE_DIR}/include")
    target_link_libraries(${TEST_NAME_HO} PRIVATE ${test_libraries})
    add_dependencies(${TEST_NAME_HO} VpGenerated)
    add_test(NAME ${TEST_NAME_HO} COMMAND ${TEST_NAME_HO} --gtest_catch_exceptions=0)
    set_target_properties(${TEST_NAME_HO} PROPERTIES FOLDER "Profiles API library")

    add_executable(${TEST_NAME_WS} ${TEST_FILE} ${vulkan_profiles_source})
    target_compile_definitions(${TEST_NAME_WS} PUBLIC "VULKAN_PROFILES_HEADER_ONLY=1")
    target_compile_definitions(${TEST_NAME_WS} PUBLIC "VK_ENABLE_BETA_EXTENSIONS=1")
    target_include_directories(${TEST_NAME_WS} PUBLIC "${vulkan-headers_SOURCE_DIR}/include")
    target_link_libraries(${TEST_NAME_WS} PRIVATE ${test_libraries})
    add_dependencies(${TEST_NAME_WS} VpGenerated)
    add_test(NAME ${TEST_NAME_WS} COMMAND ${TEST_NAME_WS} --gtest_catch_exceptions=0)
    set_target_properties(${TEST_NAME_WS} PROPERTIES FOLDER "Profiles API library")
endfunction(add_unit_test)

add_unit_test(test_api_reflection)
add_unit_test(test_api_create_device)

function (create_android_package NAME)
    if (NOT ANDROID_SDK_HOME)
        set(ANDROID_SDK_HOME $ENV{ANDROID_SDK_HOME})
    endif()
    if (NOT ANDROID_SDK_HOME)
        message(FATAL_ERROR "Please specify ANDROID_SDK_HOME")
    endif()

    if (NOT ANDROID_NDK_HOME)
        set(ANDROID_NDK_HOME $ENV{ANDROID_NDK_HOME})
    endif()
    if (NOT ANDROID_NDK_HOME)
        message(FATAL_ERROR "Please specify ANDROID_NDK_HOME. This is commonly $ANDROID_SDK_HOME/ndk-bundle.")
    endif()

    if (NOT ANDROID_BUILD_TOOLS)
        set(ANDROID_BUILD_TOOLS $ENV{ANDROID_BUILD_TOOLS})
    endif()
    if (NOT ANDROID_BUILD_TOOLS)
        message(FATAL_ERROR "Please specify ANDROID_BUILD_TOOLS version ($ANDROID_SDK_HOME/build-tools/<version>)")
    endif()

    if (NOT ANDROID_STL)
        set(ANDROID_STL $ENV{ANDROID_STL})
    endif()
    if (NOT ANDROID_STL)
        set(ANDROID_STL c++_static)
    endif()

    set(apk_test_target ${NAME})
    target_include_directories(${apk_test_target} PRIVATE ${ANDROID_NDK_HOME}/sources/third_party/vulkan/src/common)

    set(_app_glue_dir ${ANDROID_NDK_HOME}/sources/android/native_app_glue)
    target_sources(${apk_test_target} PRIVATE ${_app_glue_dir}/android_native_app_glue.c)
    target_include_directories(${apk_test_target} PRIVATE ${_app_glue_dir})
    target_compile_definitions(${apk_test_target} PRIVATE PROFILES_LAYER_APK VK_PROTOTYPES)

    target_link_libraries(${apk_test_target} ${test_libraries})
    target_link_libraries(${apk_test_target} "-u ANativeActivity_onCreate")

    set(_android_jar ${ANDROID_SDK_HOME}/platforms/android-${ANDROID_PLATFORM}/android.jar)
    set(_aapt ${ANDROID_SDK_HOME}/build-tools/${ANDROID_BUILD_TOOLS}/aapt)
    set(_zipalign ${ANDROID_SDK_HOME}/build-tools/${ANDROID_BUILD_TOOLS}/zipalign)
    add_custom_command(TARGET ${apk_test_target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/apk/out
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${apk_test_target}> ${CMAKE_BINARY_DIR}/apk/out/lib/${ANDROID_ABI}/$<TARGET_FILE_NAME:${apk_test_target}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${NAME}> ${CMAKE_BINARY_DIR}/apk/out/lib/${ANDROID_ABI}/$<TARGET_FILE_NAME:${NAME}>
        COMMAND ${_aapt} package -f -M ${CMAKE_CURRENT_SOURCE_DIR}/platforms/android/AndroidManifest.xml -I ${_android_jar} -F ${CMAKE_BINARY_DIR}/apk/out/${apk_test_target}-unaligned.apk ${CMAKE_BINARY_DIR}/apk/out
        COMMAND jarsigner -verbose -keystore $ENV{HOME}/.android/debug.keystore -storepass android -keypass android ${CMAKE_BINARY_DIR}/apk/out/${apk_test_target}-unaligned.apk androiddebugkey
        COMMAND ${_zipalign} -f 4 ${CMAKE_BINARY_DIR}/apk/out/${apk_test_target}-unaligned.apk ${CMAKE_BINARY_DIR}/apk/out/${apk_test_target}.apk
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..)
endfunction(create_android_package)

function (add_unit_test_simple_android_apk NAME)
    set(TEST_FILE ./${NAME}.cpp)
    set(TEST_NAME_HO VpLibrary_${NAME})

    add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR)
    add_library(${TEST_NAME_HO} SHARED
                ${TEST_FILE}
                ${vulkan_profiles_source}
                )
    create_android_package(${TEST_NAME_HO})

    target_include_directories(${TEST_NAME_HO} PUBLIC "${vulkan-headers_SOURCE_DIR}/include")
    add_dependencies(${TEST_NAME_HO} VpGenerated)
    add_test(NAME ${TEST_NAME_HO} COMMAND ${TEST_NAME_HO} --gtest_catch_exceptions=0)
    set_target_properties(${TEST_NAME_HO} PROPERTIES FOLDER "Profiles API library")
endfunction(add_unit_test_simple_android_apk)

if (ANDROID)
    add_unit_test_simple_android_apk(test_api_create_device_android)
endif()

function(add_unit_test_simple NAME)
    set(TEST_FILE ./${NAME}.cpp)
    set(TEST_NAME_HO VpLibrary_${NAME})

    add_executable(${TEST_NAME_HO} ${TEST_FILE})
    target_include_directories(${TEST_NAME_HO} PUBLIC "${vulkan-headers_SOURCE_DIR}/include")
    target_link_libraries(${TEST_NAME_HO} PRIVATE ${test_libraries})
    add_dependencies(${TEST_NAME_HO} VpGenerated)
    add_test(NAME ${TEST_NAME_HO} COMMAND ${TEST_NAME_HO} --gtest_catch_exceptions=0)
    set_target_properties(${TEST_NAME_HO} PROPERTIES FOLDER "Profiles API library")
endfunction(add_unit_test_simple)

add_unit_test_simple(test_util)

function(add_unit_test_with_debug_messages_variant NAME)
	set(TEST_FILE ./${NAME}.cpp)
    set(TEST_NAME VpLibrary_${NAME})
    set(TEST_NAME_DEBUG_MESSAGES VpLibrary_${NAME}_dbgmsg)

    add_executable(${TEST_NAME} ${TEST_FILE})
    target_include_directories(${TEST_NAME} PUBLIC "${vulkan-headers_SOURCE_DIR}/include")
    target_link_libraries(${TEST_NAME} PRIVATE ${test_libraries})
    add_dependencies(${TEST_NAME} VpGenerated)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} --gtest_catch_exceptions=0)
    set_target_properties(${TEST_NAME} PROPERTIES FOLDER "Profiles API library")

    add_executable(${TEST_NAME_DEBUG_MESSAGES} ${TEST_FILE})
    target_compile_definitions(${TEST_NAME_DEBUG_MESSAGES} PUBLIC "WITH_DEBUG_MESSAGES=1")
    target_include_directories(${TEST_NAME_DEBUG_MESSAGES} PUBLIC "${vulkan-headers_SOURCE_DIR}/include")
    target_link_libraries(${TEST_NAME_DEBUG_MESSAGES} PRIVATE ${test_libraries})
    add_dependencies(${TEST_NAME_DEBUG_MESSAGES} VpGenerated)
    add_test(NAME ${TEST_NAME_DEBUG_MESSAGES} COMMAND ${TEST_NAME_DEBUG_MESSAGES} --gtest_catch_exceptions=0)
    set_target_properties(${TEST_NAME_DEBUG_MESSAGES} PROPERTIES FOLDER "Profiles API library")
endfunction(add_unit_test_with_debug_messages_variant)

add_unit_test_with_debug_messages_variant(test_mocked_api_get_instance_profile_support)
add_unit_test_simple(test_mocked_api_create_instance)
add_unit_test_with_debug_messages_variant(test_mocked_api_get_physdev_profile_support)
add_unit_test_simple(test_mocked_api_create_device)
