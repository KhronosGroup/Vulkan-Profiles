add_subdirectory(test)

source_group("Python Files" FILES ${CMAKE_SOURCE_DIR}/library/scripts/generate_library.py)

add_custom_target(vulkan_profiles
	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/library/scripts/generate_library.py ${vulkan-headers_SOURCE_DIR}/registry/vk.xml ${CMAKE_SOURCE_DIR}/library/scripts/profiles ${CMAKE_SOURCE_DIR}/library/include/vulkan ${CMAKE_SOURCE_DIR}/library/source/)
