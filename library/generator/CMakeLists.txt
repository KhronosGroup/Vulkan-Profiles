set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_executable(library_generator generator.cpp)
target_link_libraries(library_generator PRIVATE Vulkan::Headers Vulkan::Profiles vulkan)
add_dependencies(library_generator library_profiles)

