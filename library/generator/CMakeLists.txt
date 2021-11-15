set(CMAKE_POSITION_INDEPENDENT_CODE ON)

file(GLOB FILES_SOURCE ./*.cpp)
file(GLOB FILES_HEADER ./*.h)

add_executable(library_generator ${FILES_SOURCE} ${FILES_HEADER})
add_custom_command(TARGET library_generator POST_BUILD COMMAND library_generator)
target_compile_definitions(library_generator PUBLIC "_CRT_SECURE_NO_WARNINGS=1")
target_compile_definitions(library_generator PUBLIC "VULKAN_INCLUDE_DIR=\"${CMAKE_SOURCE_DIR}\"")
target_link_libraries(library_generator PRIVATE Vulkan::Headers Vulkan::Profiles vulkan)
add_dependencies(library_generator library_profiles)

