if(PROFILES_BUILD_TESTS)
    add_subdirectory(tests)
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E touch  ${CMAKE_SOURCE_DIR}/layer/profiles_generated.cpp)
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

if (WIN32)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS /bigobj")
    set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -D_CRT_SECURE_NO_WARNINGS /bigobj")
else()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpointer-arith")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpointer-arith")
endif()

source_group("JSON Manifest" FILES VkLayer_khronos_profiles.json.in)
source_group("Export Files" FILES VkLayer_khronos_profiles.def VkLayer_khronos_profiles.map)

add_library(VkLayer_khronos_profiles MODULE)
set_target_properties(VkLayer_khronos_profiles PROPERTIES FOLDER "Profiles layer")

if (MSVC)
    target_link_options(VkLayer_khronos_profiles PRIVATE /DEF:${CMAKE_CURRENT_SOURCE_DIR}/VkLayer_khronos_profiles.def)
elseif(MINGW)
    target_sources(VkLayer_khronos_profiles PRIVATE VkLayer_khronos_profiles.def)
elseif(APPLE)
    message(DEBUG "Functions are exported via PROFILES_EXPORT")
    set_target_properties(VkLayer_khronos_profiles PROPERTIES SUFFIX ".dylib")
elseif(ANDROID)
    message(DEBUG "Functions are exported via PROFILES_EXPORT")
else()
    target_link_options(VkLayer_khronos_profiles PRIVATE LINKER:--version-script=${CMAKE_CURRENT_SOURCE_DIR}/VkLayer_khronos_profiles.map,-Bsymbolic,--exclude-libs,ALL)
endif()

target_sources(VkLayer_khronos_profiles PRIVATE
    profiles_interface.cpp
    profiles_interface.h
    profiles_settings.cpp
    profiles_settings.h
    profiles_json.cpp
    profiles_json.h
    profiles_util.cpp
    profiles_util.h
    profiles_generated.cpp
    profiles.h
    vk_layer_table.cpp
    VkLayer_khronos_profiles.json.in
    VkLayer_khronos_profiles.def
    VkLayer_khronos_profiles.map
)

target_link_libraries(VkLayer_khronos_profiles PRIVATE
    Vulkan::LayerUtils
    Vulkan::Headers
    jsoncpp_static
    valijson
)

if(ANDROID)
    # Android needs -llog for __android_print_log()
    target_link_Libraries(VkLayer_khronos_profiles PRIVATE log)
endif()

set(LAYER_PYTHON_FILES ${CMAKE_SOURCE_DIR}/scripts/gen_profiles_layer.py)
source_group("Python Files" FILES ${LAYER_PYTHON_FILES})

add_custom_target(VpLayer_generate ALL
    COMMAND ${PYTHON_EXECUTABLE} ${LAYER_PYTHON_FILES}
        -registry ${VULKAN_HEADERS_REGISTRY_DIRECTORY}/vk.xml
        -outLayer ${CMAKE_SOURCE_DIR}/layer/profiles_generated.cpp
    VERBATIM
    SOURCES ${LAYER_PYTHON_FILES}
    DEPENDS ${VULKAN_HEADERS_REGISTRY_DIRECTORY}/vk.xml)
set_target_properties(VpLayer_generate PROPERTIES FOLDER "Profiles layer")
add_dependencies(VkLayer_khronos_profiles VpLayer_generate)
	
source_group("Python Files" FILES ${CMAKE_SOURCE_DIR}/scripts/gen_profiles_tests.py)
add_custom_target(VpLayer_generate_tests ALL
	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/gen_profiles_tests.py
		-registry ${VULKAN_HEADERS_REGISTRY_DIRECTORY}/vk.xml
		-outProfile ${CMAKE_SOURCE_DIR}/profiles/test/data/VP_LUNARG_test_api_generated.json
		-outTests ${CMAKE_SOURCE_DIR}/layer/tests/tests_generated.cpp
	VERBATIM
	DEPENDS VkLayer_khronos_profiles
	)
set_target_properties(VpLayer_generate_tests PROPERTIES FOLDER "Profiles layer")

set(INPUT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/VkLayer_khronos_profiles.json.in")
set(INTERMEDIATE_FILE "${CMAKE_CURRENT_BINARY_DIR}/json/profile.json")
set(OUTPUT_FILE_FINAL_NAME "VkLayer_khronos_profiles.json")
set(LAYER_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})
if (WIN32)
    set(LAYER_INSTALL_DIR ${CMAKE_INSTALL_BINDIR}) # WIN32/MINGW expect the dll in the `bin` dir, this matches our WIN32 SDK process
endif()

if (WIN32)
    set(JSON_LIBRARY_PATH ".\\\\VkLayer_khronos_profiles.dll")
elseif(APPLE)
    set(JSON_LIBRARY_PATH "./libVkLayer_khronos_profiles.dylib")
else()
    set(JSON_LIBRARY_PATH "./libVkLayer_khronos_profiles.so")
endif()

set(JSON_API_VERSION ${VulkanHeaders_VERSION})

configure_file(${INPUT_FILE} ${INTERMEDIATE_FILE} @ONLY)

# To support both multi/single configuration generators just copy the json to the correct directory
add_custom_command(TARGET VkLayer_khronos_profiles POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${INTERMEDIATE_FILE} $<TARGET_FILE_DIR:VkLayer_khronos_profiles>/${OUTPUT_FILE_FINAL_NAME}
)

# For UNIX-based systems, `library_path` should not contain a relative path (indicated by "./") before installing to system directories
# This json isn't used for regular local development, it's used for installation
if (UNIX)
    set(UNIX_INTERMEDIATE_FILE "${CMAKE_CURRENT_BINARY_DIR}/json/unix_install_profile.json")

    if(APPLE)
        set(JSON_LIBRARY_PATH "libVkLayer_khronos_profiles.dylib")
    else()
        set(JSON_LIBRARY_PATH "libVkLayer_khronos_profiles.so")
    endif()

    configure_file(${INPUT_FILE} ${UNIX_INTERMEDIATE_FILE} @ONLY)

    install(FILES ${UNIX_INTERMEDIATE_FILE} DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/vulkan/explicit_layer.d RENAME ${OUTPUT_FILE_FINAL_NAME})
endif()

if (WIN32)
    install(FILES ${INTERMEDIATE_FILE} DESTINATION ${LAYER_INSTALL_DIR} RENAME ${OUTPUT_FILE_FINAL_NAME})
endif()
if (MSVC)
    install(FILES $<TARGET_PDB_FILE:VkLayer_khronos_profiles> DESTINATION ${LAYER_INSTALL_DIR})
endif()

install(TARGETS VkLayer_khronos_profiles DESTINATION ${LAYER_INSTALL_DIR})
