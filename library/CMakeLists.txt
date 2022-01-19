add_subdirectory(test)

add_library(VulkanProfiles INTERFACE)
target_include_directories(VulkanProfiles INTERFACE include)
add_library(Vulkan::Profiles ALIAS VulkanProfiles)

source_group("Python Files" FILES ${CMAKE_CURRENT_SOURCE_DIR}/scripts/genvp.py)

add_custom_target(VpLibrary_generator
	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/library/scripts/genvp.py -registry ${vulkan-headers_SOURCE_DIR}/registry/vk.xml -profiles ${CMAKE_CURRENT_SOURCE_DIR}/../profiles -outIncDir ${CMAKE_CURRENT_SOURCE_DIR}/include/vulkan -outSrcDir ${CMAKE_CURRENT_SOURCE_DIR}/source
	SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/scripts/genvp.py)
