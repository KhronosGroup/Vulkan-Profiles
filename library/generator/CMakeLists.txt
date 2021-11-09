set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(TEST_FILE generator.cpp)

add_executable(generator generator.cpp)
target_link_libraries(generator PRIVATE Vulkan::Headers Vulkan::Profiles vulkan)
add_dependencies(generator library_profiles)

