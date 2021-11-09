add_subdirectory(generator)
add_subdirectory(test)

add_library(VulkanProfiles INTERFACE)
target_include_directories(VulkanProfiles INTERFACE include)
add_library(Vulkan::Profiles ALIAS VulkanProfiles)

source_group("Python Files" FILES ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_library.py)

add_custom_target(library_profiles
	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/library/scripts/generate_library.py ${Vulkan-Headers_SOURCE_DIR}/registry/vk.xml ${CMAKE_CURRENT_SOURCE_DIR}/scripts/profiles ${CMAKE_CURRENT_SOURCE_DIR}/include/vulkan ${CMAKE_CURRENT_SOURCE_DIR}/source/
	SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_library.py ${CMAKE_CURRENT_SOURCE_DIR}/include/vulkan/vulkan_profiles.hpp)
