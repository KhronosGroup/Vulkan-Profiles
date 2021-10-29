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

set(TARGET_NAMES
    VkLayer_device_simulation_manual
    )

if (WIN32)
    macro(add_vk_layer target)
    FILE(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/VkLayer_${target}.def DEF_FILE)
    add_custom_target(copy-${target}-def-file ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DEF_FILE} VkLayer_${target}.def
        VERBATIM
    )
    add_library(VkLayer_${target} SHARED ${ARGN} VkLayer_${target}.def)
    target_link_Libraries(VkLayer_${target} ${VkLayer_utils_LIBRARY} vku)
    endmacro()
else()
    macro(add_vk_layer target)
    add_library(VkLayer_${target} SHARED ${ARGN})
    target_link_Libraries(VkLayer_${target} ${VkLayer_utils_LIBRARY} vku)
    if (NOT APPLE)
        set_target_properties(VkLayer_${target} PROPERTIES LINK_FLAGS "-Wl,-Bsymbolic")
    endif ()
    install(TARGETS VkLayer_${target} DESTINATION ${CMAKE_INSTALL_LIBDIR})
    endmacro()
endif()

if (APPLE)
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_BINARY_DIR}
        ${jsoncpp_SOURCE_DIR}/include
        ${Vulkan-Headers_SOURCE_DIR}/include
        ${Vulkan-ValidationLayers_SOURCE_DIR}/layers
        ${Vulkan-ValidationLayers_SOURCE_DIR}/layers/generated
    )
else ()
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_BINARY_DIR}
        ${jsoncpp_SOURCE_DIR}/include
        ${Vulkan-Headers_SOURCE_DIR}/include
        ${Vulkan-ValidationLayers_SOURCE_DIR}/layers
        ${Vulkan-ValidationLayers_SOURCE_DIR}/layers/generated
    )
endif()

if (WIN32)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS /bigobj")
    set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -D_CRT_SECURE_NO_WARNINGS /bigobj")
else()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpointer-arith")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpointer-arith")
endif()

add_vk_layer(device_simulation_manual device_simulation.cpp vk_layer_table.cpp ${jsoncpp_SOURCE_DIR}/dist/jsoncpp.cpp ${jsoncpp_SOURCE_DIR}/include/json/json.h)

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
foreach(TARGET_NAME ${TARGET_NAMES})
    set(CONFIG_DEFINES
        -DINPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_NAME}.json.in"
        -DVK_VERSION="1.2.195"
    )
    # Get the needed properties from that target
    set(CONFIG_DEFINES ${CONFIG_DEFINES}
        -DOUTPUT_FILE="$<TARGET_FILE_DIR:${TARGET_NAME}>/${TARGET_NAME}.json"
        -DRELATIVE_LAYER_BINARY="${RELATIVE_PATH_PREFIX}$<TARGET_FILE_NAME:${TARGET_NAME}>"
        )

    add_custom_target(${TARGET_NAME}-json ALL COMMAND ${CMAKE_COMMAND} ${CONFIG_DEFINES} -P "${CMAKE_CURRENT_BINARY_DIR}/generator.cmake")

    # Add target for JSON file install.
    if(NOT WIN32)
        # Add targets for JSON file install on Linux.
        # Need to remove the "./" from the library path before installing
        add_custom_target(${TARGET_NAME}-staging-json ALL
            COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/staging-json
            COMMAND cp $<TARGET_FILE_DIR:${TARGET_NAME}>/${TARGET_NAME}.json ${CMAKE_CURRENT_BINARY_DIR}/staging-json
            COMMAND sed -i -e "/.library_path.:/s=./libVkLayer=libVkLayer=" ${CMAKE_CURRENT_BINARY_DIR}/staging-json/${TARGET_NAME}.json
            VERBATIM
            DEPENDS ${TARGET_NAME}-json
            )
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/staging-json/${TARGET_NAME}.json DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/vulkan/explicit_layer.d)
    endif()

endforeach()