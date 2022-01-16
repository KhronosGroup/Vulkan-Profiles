
add_subdirectory(tests)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_definitions(-DVK_USE_PLATFORM_WIN32_KHR -DVK_USE_PLATFORM_WIN32_KHX -DWIN32_LEAN_AND_MEAN -DNOMINMAX)
    set(DisplayServer Win32)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
    add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME MATCHES "BSD")
    if (BUILD_WSI_XCB_SUPPORT)
        add_definitions(-DVK_USE_PLATFORM_XCB_KHR -DVK_USE_PLATFORM_XCB_KHX)
        set(DisplayServer Xcb)
    endif()

    if (BUILD_WSI_XLIB_SUPPORT)
        if (NOT DisplayServer)
            set(DisplayServer Xlib)
        endif()
        add_definitions(-DVK_USE_PLATFORM_XLIB_KHR -DVK_USE_PLATFORM_XLIB_KHX)
    endif()

    if (BUILD_WSI_WAYLAND_SUPPORT)
       # TODO Add Wayland Support
       # add_definitions(-DVK_USE_PLATFORM_WAYLAND_KHR)
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    if (BUILD_MOLTENVK_SUPPORT)
        add_definitions(-DVK_USE_PLATFORM_MACOS_MVK)
    endif()

    if (BUILD_METAL_SUPPORT)
       # TODO Add Metal Support
       add_definitions(-DVK_USE_PLATFORM_METAL_EXT)
    endif()
else()
    message(FATAL_ERROR "Unsupported Platform!")
endif()

add_definitions(-DVK_ENABLE_BETA_EXTENSIONS)

set(TARGET_NAME khronos_profiles)

if (APPLE)
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_BINARY_DIR}
        ${jsoncpp_SOURCE_DIR}/include
        ${vulkan-headers_SOURCE_DIR}/include
        ${vulkan-validationlayers_SOURCE_DIR}/layers
        ${vulkan-validationlayers_SOURCE_DIR}/layers/generated
    )
else ()
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_BINARY_DIR}
        ${jsoncpp_SOURCE_DIR}/include
        ${vulkan-headers_SOURCE_DIR}/include
        ${vulkan-validationlayers_SOURCE_DIR}/layers
        ${vulkan-validationlayers_SOURCE_DIR}/layers/generated
    )
endif()

if (WIN32)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS /bigobj")
    set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -D_CRT_SECURE_NO_WARNINGS /bigobj")
else()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpointer-arith")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpointer-arith")
endif()

add_vk_layer(${TARGET_NAME} profiles.cpp vk_layer_table.cpp ${jsoncpp_SOURCE_DIR}/dist/jsoncpp.cpp ${jsoncpp_SOURCE_DIR}/include/json/json.h)

# json file creation

# The output file needs Unix "/" separators or Windows "\" separators
# Windows paths need to start with ".\", Unix paths do not.
# Windows separators actually need to be doubled because the json format uses backslash escapes
file(TO_NATIVE_PATH "./" RELATIVE_PATH_PREFIX)
string(REPLACE "\\" "\\\\" RELATIVE_PATH_PREFIX "${RELATIVE_PATH_PREFIX}")
# Run each .json.in file through the generator
# We need to create the generator.cmake script so that the generator can be run at compile time, instead of configure time
# Running at compile time lets us use cmake generator expressions (TARGET_FILE_NAME and TARGET_FILE_DIR, specifically)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/generator.cmake" "configure_file(\"\${INPUT_FILE}\" \"\${OUTPUT_FILE}\" @ONLY)")

set(CONFIG_DEFINES
    -DINPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/VkLayer_${TARGET_NAME}.json.in"
    -DVK_VERSION=1.3.203
)

# Get the needed properties from that target
set(CONFIG_DEFINES ${CONFIG_DEFINES}
    -DOUTPUT_FILE="$<TARGET_FILE_DIR:VkLayer_${TARGET_NAME}>/VkLayer_${TARGET_NAME}.json"
    -DRELATIVE_LAYER_BINARY="${RELATIVE_PATH_PREFIX}$<TARGET_FILE_NAME:VkLayer_${TARGET_NAME}>"
)

add_custom_target(VkLayer_${TARGET_NAME}-json ALL COMMAND ${CMAKE_COMMAND} ${CONFIG_DEFINES} -P "${CMAKE_CURRENT_BINARY_DIR}/generator.cmake")

# Add target for JSON file install.
if(NOT WIN32)
    # Add targets for JSON file install on Linux.
    # Need to remove the "./" from the library path before installing
    add_custom_target(VkLayer_${TARGET_NAME}-staging-json ALL
        COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/staging-json
        COMMAND cp $<TARGET_FILE_DIR:VkLayer_${TARGET_NAME}>/VkLayer_${TARGET_NAME}.json ${CMAKE_CURRENT_BINARY_DIR}/staging-json
        COMMAND sed -i -e "/.library_path.:/s=./libVkLayer=libVkLayer=" ${CMAKE_CURRENT_BINARY_DIR}/staging-json/VkLayer_${TARGET_NAME}.json
        VERBATIM
        DEPENDS VkLayer_${TARGET_NAME}-json
        )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/staging-json/VkLayer_${TARGET_NAME}.json DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/vulkan/explicit_layer.d)
endif()

