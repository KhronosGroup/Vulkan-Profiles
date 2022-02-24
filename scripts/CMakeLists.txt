source_group("Python Files" FILES ${CMAKE_CURRENT_SOURCE_DIR}/scripts/genvp.py)

add_custom_target(VpGenerated
	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/genvp.py
		-registry ${vulkan-headers_SOURCE_DIR}/registry/vk.xml
		-profiles ${CMAKE_SOURCE_DIR}/profiles
		-outIncDir ${CMAKE_SOURCE_DIR}/library/include/vulkan
		-outSrcDir ${CMAKE_SOURCE_DIR}/library/source
		-outSchema ${CMAKE_SOURCE_DIR}/schema/profiles-latest.json
		-outDoc ${CMAKE_SOURCE_DIR}/PROFILES.md
		-validate
		-generateDebugLibrary
	SOURCES ${CMAKE_SOURCE_DIR}/scripts/genvp.py)

set_target_properties(VpGenerated PROPERTIES FOLDER "Profiles scripts")
