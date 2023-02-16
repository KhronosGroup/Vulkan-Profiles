if(PROFILES_BUILD_TESTS)
    add_subdirectory(tests)
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E touch  ${CMAKE_SOURCE_DIR}/layer/profiles.cpp)
execute_process(COMMAND ${CMAKE_COMMAND} -E touch  ${CMAKE_SOURCE_DIR}/layer/tests/tests_generated.cpp)

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

if (WIN32)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS /bigobj")
    set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -D_CRT_SECURE_NO_WARNINGS /bigobj")
else()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpointer-arith")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpointer-arith")
endif()

add_vk_layer(${TARGET_NAME} profiles.cpp vk_layer_table.cpp)

set(LAYER_PYTHON_FILES ${CMAKE_SOURCE_DIR}/scripts/gen_profiles_layer.py)
source_group("Python Files" FILES ${LAYER_PYTHON_FILES})

add_custom_target(VpLayer_generate ALL
    COMMAND ${PYTHON_EXECUTABLE} ${LAYER_PYTHON_FILES}
        -registry ${VULKAN_HEADERS_REGISTRY_DIRECTORY}/vk.xml
        -outLayer ${CMAKE_SOURCE_DIR}/layer/profiles.cpp
    VERBATIM
    SOURCES ${LAYER_PYTHON_FILES}
    DEPENDS ${VULKAN_HEADERS_REGISTRY_DIRECTORY}/vk.xml)
set_target_properties(VpLayer_generate PROPERTIES FOLDER "Profiles layer")
add_dependencies(VkLayer_${TARGET_NAME} VpLayer_generate)
	
source_group("Python Files" FILES ${CMAKE_SOURCE_DIR}/scripts/gen_profiles_tests.py)
add_custom_target(VpLayer_generate_tests ALL
	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/gen_profiles_tests.py
		-registry ${VULKAN_HEADERS_REGISTRY_DIRECTORY}/vk.xml
		-outProfile ${CMAKE_SOURCE_DIR}/profiles/test/data/VP_LUNARG_test_api_generated.json
		-outTests ${CMAKE_SOURCE_DIR}/layer/tests/tests_generated.cpp
	VERBATIM
	DEPENDS VkLayer_${TARGET_NAME}
	)
set_target_properties(VpLayer_generate_tests PROPERTIES FOLDER "Profiles layer")

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

# Use HINTS instead of PATH to search these locations before
# searching system environment variables like $PATH that may
# contain SDK directories.
find_path(VulkanHeaders_INCLUDE_DIR
    NAMES vulkan/vulkan.h
    HINTS
        ${VULKAN_HEADERS_INSTALL_DIR}/include
        "$ENV{VULKAN_HEADERS_INSTALL_DIR}/include"
        "$ENV{VULKAN_SDK}/include")

# Determine the major/minor/patch version from the vulkan header

set(VulkanHeaders_VERSION_MAJOR "0")
set(VulkanHeaders_VERSION_MINOR "0")
set(VulkanHeaders_VERSION_PATCH "0")
set(VulkanHeaders_main_header ${VulkanHeaders_INCLUDE_DIR}/vulkan/vulkan_core.h)
message(STATUS "Parsing ${VulkanHeaders_main_header} to get Vulkan Version")

# Find all lines in the header file that contain any version we may be interested in
#  NOTE: They start with #define and then have other keywords
file(STRINGS
        ${VulkanHeaders_main_header}
        VulkanHeader_lines
        REGEX "^#define VK_HEADER_VERSION(_COMPLETE)? ")

foreach(VulkanHeader_line ${VulkanHeader_lines})

    # First, handle the case where we have a major/minor version
    #   Format is:
    #        #define VK_HEADER_VERSION_COMPLETE VK_MAKE_API_VERSION(0, X, Y, VK_HEADER_VERSION)
    #   We grab the major version (X) and minor version (Y) out of the parentheses
    string(REGEX MATCH "VK_HEADER_VERSION_COMPLETE VK_MAKE_API_VERSION\\(.*\\)" VulkanHeaders_out ${VulkanHeader_line})
    string(REGEX MATCHALL "[0-9]+" VulkanHeaders_MAJOR_MINOR "${VulkanHeaders_out}")
    if (VulkanHeaders_MAJOR_MINOR)
        list (GET VulkanHeaders_MAJOR_MINOR 1 VulkanHeaders_cur_major)
        list (GET VulkanHeaders_MAJOR_MINOR 2 VulkanHeaders_cur_minor)
        if (${VulkanHeaders_cur_major} GREATER ${VulkanHeaders_VERSION_MAJOR})
            set(VulkanHeaders_VERSION_MAJOR ${VulkanHeaders_cur_major})
            set(VulkanHeaders_VERSION_MINOR ${VulkanHeaders_cur_minor})
        endif()
        if (${VulkanHeaders_cur_major} EQUAL ${VulkanHeaders_VERSION_MAJOR} AND
            ${VulkanHeaders_cur_minor} GREATER ${VulkanHeaders_VERSION_MINOR})
            set(VulkanHeaders_VERSION_MINOR ${VulkanHeaders_cur_minor})
        endif()
    endif()

    # Second, handle the case where we have the patch version
    #   Format is:
    #      #define VK_HEADER_VERSION Z
    #   Where Z is the patch version which we just grab off the end
    string(REGEX MATCH "define.*VK_HEADER_VERSION[^_].*[0-9]+" VulkanHeaders_out ${VulkanHeader_line})
    list(LENGTH VulkanHeaders_out VulkanHeaders_len)
    if (VulkanHeaders_len)
        string(REGEX MATCH "[0-9]+" VulkanHeaders_VERSION_PATCH "${VulkanHeaders_out}")
    endif()

endforeach()
MESSAGE(STATUS
        "Detected Vulkan Version ${VulkanHeaders_VERSION_MAJOR}."
        "${VulkanHeaders_VERSION_MINOR}."
        "${VulkanHeaders_VERSION_PATCH}")


set(CONFIG_DEFINES
    -DINPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/VkLayer_${TARGET_NAME}.json.in"
    -DVK_VERSION="${VulkanHeaders_VERSION_MAJOR}.${VulkanHeaders_VERSION_MINOR}.${VulkanHeaders_VERSION_PATCH}"
)

# Get the needed properties from that target
set(CONFIG_DEFINES ${CONFIG_DEFINES}
    -DOUTPUT_FILE="$<TARGET_FILE_DIR:VkLayer_${TARGET_NAME}>/VkLayer_${TARGET_NAME}.json"
    -DRELATIVE_LAYER_BINARY="${RELATIVE_PATH_PREFIX}$<TARGET_FILE_NAME:VkLayer_${TARGET_NAME}>"
)

add_custom_target(VkLayer_${TARGET_NAME}-json ALL COMMAND ${CMAKE_COMMAND} ${CONFIG_DEFINES} -P "${CMAKE_CURRENT_BINARY_DIR}/generator.cmake")
set_target_properties(VkLayer_${TARGET_NAME}-json PROPERTIES FOLDER "Profiles layer")

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
    set_target_properties(VkLayer_${TARGET_NAME}-staging-json PROPERTIES FOLDER "Profiles layer")
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/staging-json/VkLayer_${TARGET_NAME}.json DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/vulkan/explicit_layer.d)
endif()

