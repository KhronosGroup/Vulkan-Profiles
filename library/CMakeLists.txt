add_subdirectory(test)

source_group("Python Files" FILES ${CMAKE_SOURCE_DIR}/library/scripts/generate_library.py)

add_custom_target(vulkan_profiles
	COMMAND python ${CMAKE_SOURCE_DIR}/library/scripts/generate_library.py ${CMAKE_SOURCE_DIR}/library/scripts/profiles ${CMAKE_SOURCE_DIR}/library/include/vulkan ${CMAKE_SOURCE_DIR}/library/source/)
