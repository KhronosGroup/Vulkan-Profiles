# ~~~
# Copyright (c) 2021-2024 Valve Corporation
# Copyright (c) 2021-2025 LunarG, Inc.
# Copyright (c) 2024 RasterGrid Kft.
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
#
# Authors:
# - Christophe Riccio <christophe@lunarg.com>
# ~~~

execute_process(COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_SOURCE_DIR}/layer/profiles.cpp)

if (ANDROID)
    set(LAYER_TEST_FILES
        tests_mechanism.cpp
    )
elseif(APPLE)
    set(LAYER_TEST_FILES
        tests
        tests_promoted
        tests_generated
        tests_mechanism
        tests_mechanism_format
        tests_mechanism_check_values
        tests_mechanism_physical_device_selection
        tests_util
    )
else()
    set(LAYER_TEST_FILES
        tests
        tests_promoted
        tests_generated
        tests_mechanism
        tests_mechanism_format
        tests_mechanism_api_version
        tests_mechanism_check_values
        tests_mechanism_physical_device_selection
        tests_mechanism_video_profiles
        tests_combine_union
        tests_combine_intersection
        tests_util
    )
endif()

function(LayerTest NAME)
	set(TEST_FILENAME ./${NAME}.cpp)
    set(TEST_NAME VkLayer_${NAME})
    
    file(GLOB TEST_JSON_FILES ${CMAKE_SOURCE_DIR}/profiles/test/data/)

    add_executable(${TEST_NAME}
                   ${TEST_FILENAME}
                   profiles_test_helper.h
                   profiles_test_helper.cpp
                   layer_tests_main.cpp
                   vktestframework.cpp)
    add_dependencies(${TEST_NAME} ProfilesLayer ${TEST_JSON_FILES})
    target_link_libraries(${TEST_NAME} Vulkan::CompilerConfiguration Vulkan::CompilerConfigurationExtra Vulkan::Headers Vulkan::Loader GTest::gtest GTest::gtest_main Vulkan::LayerSettings)
    target_compile_definitions(${TEST_NAME} PUBLIC JSON_TEST_FILES_PATH="${CMAKE_SOURCE_DIR}/profiles/test/data/")
    target_compile_definitions(${TEST_NAME} PUBLIC JSON_PROFILES_PATH="${CMAKE_SOURCE_DIR}/profiles/")
    target_compile_definitions(${TEST_NAME} PUBLIC TEST_BINARY_PATH="$<TARGET_FILE_DIR:ProfilesLayer>")

    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    set_tests_properties(${TEST_NAME} PROPERTIES ENVIRONMENT
        "VK_LAYER_PATH=$<TARGET_FILE_DIR:ProfilesLayer>"
    )

    set_target_properties(${TEST_NAME} PROPERTIES FOLDER "Profiles layer/Tests")
endfunction(LayerTest)

function(LayerTestAndroid NAME)
    set(ANDROID_APK_NAME ${NAME})

    file(GLOB_RECURSE TEST_FILES_JSON ${CMAKE_SOURCE_DIR}/profiles/*.json)
    set(PROFILES_LAYER_APK ON)
    add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR)
    add_library(${ANDROID_APK_NAME} SHARED
                ${LAYER_TEST_FILES}
                profiles_test_helper.h
                profiles_test_helper.cpp
                layer_tests_main.cpp
                vktestframework.cpp)

    add_test(NAME ${ANDROID_APK_NAME} COMMAND profiles_${NAME})
    add_dependencies(${ANDROID_APK_NAME} ProfilesLayer VpCreateDesktopBaseline)

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

    set(_app_glue_dir ${ANDROID_NDK_HOME}/sources/android/native_app_glue)
    target_compile_definitions(${ANDROID_APK_NAME} PUBLIC
                               JSON_TEST_FILES_PATH="/sdcard/Android/data/com.example.VulkanProfilesLayerTests/files/"
                               JSON_PROFILES_PATH="/sdcard/Android/data/com.example.VulkanProfilesLayerTests/files/"
                               TEST_BINARY_PATH="$<TARGET_FILE_DIR:ProfilesLayer>"
                               PROFILES_LAYER_APK VK_PROTOTYPES
                               )
    target_include_directories(${ANDROID_APK_NAME} PUBLIC
                              ${ANDROID_NDK_HOME}/sources/third_party/vulkan/src/common
                              ${_app_glue_dir}
                              )
    target_sources(${ANDROID_APK_NAME} PRIVATE ${_app_glue_dir}/android_native_app_glue.c)
    target_link_libraries(${ANDROID_APK_NAME} "-u ANativeActivity_onCreate")
    target_link_libraries(${ANDROID_APK_NAME} log android dl vulkan)
    target_link_libraries(${ANDROID_APK_NAME} Vulkan::Headers GTest::gtest GTest::gtest_main Vulkan::LayerSettings Vulkan::CompilerConfiguration Vulkan::CompilerConfigurationExtra)

    set(_android_jar ${ANDROID_SDK_HOME}/platforms/android-${ANDROID_PLATFORM}/android.jar)
    set(_aapt ${ANDROID_SDK_HOME}/build-tools/${ANDROID_BUILD_TOOLS}/aapt)
    set(_zipalign ${ANDROID_SDK_HOME}/build-tools/${ANDROID_BUILD_TOOLS}/zipalign)
    add_custom_command(TARGET ${ANDROID_APK_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/apk/assets
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${TEST_FILES_JSON} ${PROJECT_BINARY_DIR}/apk/assets
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/apk/out
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${ANDROID_APK_NAME}> ${PROJECT_BINARY_DIR}/apk/out/lib/${ANDROID_ABI}/$<TARGET_FILE_NAME:${ANDROID_APK_NAME}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:ProfilesLayer> ${PROJECT_BINARY_DIR}/apk/out/lib/${ANDROID_ABI}/$<TARGET_FILE_NAME:ProfilesLayer>
        COMMAND ${_aapt} package -f -M ${CMAKE_CURRENT_SOURCE_DIR}/platforms/android/AndroidManifest.xml -I ${_android_jar} -A ${PROJECT_BINARY_DIR}/apk/assets -F ${PROJECT_BINARY_DIR}/apk/out/${ANDROID_APK_NAME}-unaligned.apk ${PROJECT_BINARY_DIR}/apk/out
        COMMAND ${_zipalign} -f 4 ${PROJECT_BINARY_DIR}/apk/out/${ANDROID_APK_NAME}-unaligned.apk ${PROJECT_BINARY_DIR}/apk/out/${ANDROID_APK_NAME}.apk
        COMMAND apksigner sign --verbose --ks $ENV{HOME}/.android/debug.keystore --ks-pass pass:android ${PROJECT_BINARY_DIR}/apk/out/${ANDROID_APK_NAME}.apk
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..)

    set_tests_properties(${TEST_NAME} PROPERTIES ENVIRONMENT "${run_environment}")
    set_target_properties(${TEST_NAME} PROPERTIES FOLDER "Profiles layer/Tests")
endfunction(LayerTestAndroid)

if (NOT ANDROID)
    foreach(test_item ${LAYER_TEST_FILES})
        LayerTest(${test_item})
    endforeach()

    if (NOT APPLE)
        add_dependencies(VkLayer_tests_combine_intersection VpTestIntersect)
        add_dependencies(VkLayer_tests_combine_union VpTestUnion)
        add_dependencies(VkLayer_tests_generated VpLayer_generate_tests)
        add_dependencies(VkLayer_tests_generated VpTestHostImageCopy)
    endif()
else()
    LayerTestAndroid("VulkanProfilesLayerTests")
endif()
