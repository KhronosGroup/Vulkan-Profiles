set(LAYER_PYTHON_FILES ${CMAKE_SOURCE_DIR}/scripts/genvp.py)
file(GLOB PROFILES_FILES ${CMAKE_SOURCE_DIR}/profiles)
source_group("Python Files" FILES ${LAYER_PYTHON_FILES})

add_custom_target(VpGenerated
	COMMAND ${PYTHON_EXECUTABLE} ${LAYER_PYTHON_FILES}
		-registry ${vulkan-headers_SOURCE_DIR}/registry/vk.xml
		-profiles ${CMAKE_SOURCE_DIR}/profiles
		-outIncDir ${CMAKE_SOURCE_DIR}/library/include/vulkan
		-outSrcDir ${CMAKE_SOURCE_DIR}/library/source
		-outSchema ${CMAKE_SOURCE_DIR}/schema/${PROFILES_SCHEMA_FILENAME}
		-outDoc ${CMAKE_SOURCE_DIR}/PROFILES.md
		-validate
		-generateDebugLibrary
	VERBATIM
	SOURCES ${LAYER_PYTHON_FILES} ${PROFILES_FILES}
	DEPENDS ${LAYER_PYTHON_FILES} ${PROFILES_FILES})

set_target_properties(VpGenerated PROPERTIES FOLDER "Profiles scripts")

set(MERGE_PYTHON_FILES ${CMAKE_SOURCE_DIR}/scripts/combine_profiles.py)

add_custom_target(VpMergeIntersect ALL
	COMMAND ${PYTHON_EXECUTABLE} ${MERGE_PYTHON_FILES}
		-registry ${vulkan-headers_SOURCE_DIR}/registry/vk.xml
		-input_dir ${CMAKE_SOURCE_DIR}/profiles/test/data/intersect
		-input_profiles "VP_LUNARG_desktop_portability_2021_intersect_data1,VP_LUNARG_desktop_portability_2021_intersect_data2"
		-output_path ${CMAKE_SOURCE_DIR}/profiles/test/data/intersect.json
		-output_profile VP_LUNARG_desktop_portability_2021
		-mode intersection
	VERBATIM
	SOURCES ${MERGE_PYTHON_FILES} ${PROFILES_FILES}
	DEPENDS ${MERGE_PYTHON_FILES} ${PROFILES_FILES})

set_target_properties(VpMergeIntersect PROPERTIES FOLDER "Profiles scripts")

add_custom_target(VpMergeUnion ALL
	COMMAND ${PYTHON_EXECUTABLE} ${MERGE_PYTHON_FILES}
		-registry ${vulkan-headers_SOURCE_DIR}/registry/vk.xml
		-input_dir ${CMAKE_SOURCE_DIR}/profiles/test/data/union
		-input_profiles "VP_LUNARG_desktop_portability_2021_union_data1,VP_LUNARG_desktop_portability_2021_union_data2"
		-output_path ${CMAKE_SOURCE_DIR}/profiles/test/data/union.json
		-output_profile VP_LUNARG_desktop_portability_2021
		-mode union
	VERBATIM
	SOURCES ${MERGE_PYTHON_FILES} ${PROFILES_FILES}
	DEPENDS ${MERGE_PYTHON_FILES} ${PROFILES_FILES})

set_target_properties(VpMergeUnion PROPERTIES FOLDER "Profiles scripts")

add_custom_target(VpTestIntersect ALL
	COMMAND ${PYTHON_EXECUTABLE} ${MERGE_PYTHON_FILES}
		-registry ${vulkan-headers_SOURCE_DIR}/registry/vk.xml
		-input_dir ${CMAKE_SOURCE_DIR}/profiles/test/data/VP_LUNARG_test_combine_intersect
		-output_path ${CMAKE_SOURCE_DIR}/profiles/test/data/VP_LUNARG_test_combine_intersect.json
		-output_profile VP_LUNARG_test_combine_intersect
		-mode intersection
	VERBATIM
	SOURCES ${MERGE_PYTHON_FILES} ${PROFILES_FILES}
	DEPENDS ${MERGE_PYTHON_FILES} ${PROFILES_FILES})

set_target_properties(VpTestIntersect PROPERTIES FOLDER "Profiles scripts")

add_custom_target(VpTestUnion ALL
	COMMAND ${PYTHON_EXECUTABLE} ${MERGE_PYTHON_FILES}
		-registry ${vulkan-headers_SOURCE_DIR}/registry/vk.xml
		-input_dir ${CMAKE_SOURCE_DIR}/profiles/test/data/VP_LUNARG_test_combine_union
		-output_path ${CMAKE_SOURCE_DIR}/profiles/test/data/VP_LUNARG_test_combine_union.json
		-output_profile VP_LUNARG_test_combine_union
		-mode union
	VERBATIM
	SOURCES ${MERGE_PYTHON_FILES} ${PROFILES_FILES}
	DEPENDS ${MERGE_PYTHON_FILES} ${PROFILES_FILES})

set_target_properties(VpTestUnion PROPERTIES FOLDER "Profiles scripts")

add_custom_target(VpCreateGeneratedName ALL
	COMMAND ${PYTHON_EXECUTABLE} ${MERGE_PYTHON_FILES}
		-registry ${vulkan-headers_SOURCE_DIR}/registry/vk.xml
		-input_dir ${CMAKE_SOURCE_DIR}/profiles/test/data/VP_LUNARG_test_combine_intersect
		-output_path ${CMAKE_SOURCE_DIR}/profiles/test/data/VP_LUNARG_test_generated_name.json
	VERBATIM
	SOURCES ${MERGE_PYTHON_FILES} ${PROFILES_FILES}
	DEPENDS ${MERGE_PYTHON_FILES} ${PROFILES_FILES})

set_target_properties(VpCreateGeneratedName PROPERTIES FOLDER "Profiles scripts")

#add_custom_target(VpCreateDesktop2022 ALL
#	COMMAND ${PYTHON_EXECUTABLE} ${MERGE_PYTHON_FILES}
#		-registry ${vulkan-headers_SOURCE_DIR}/registry/vk.xml
#		-input_dir ${CMAKE_SOURCE_DIR}/profiles/VP_LUNARG_desktop_portability_2022/windows
#		-output_path ${CMAKE_SOURCE_DIR}/profiles/VP_LUNARG_desktop_portability_2022/VP_LUNARG_desktop_portability_2022_windows.json
#		-output_profile VP_LUNARG_desktop_portability_2022
#	VERBATIM
#	SOURCES ${MERGE_PYTHON_FILES} ${PROFILES_FILES}
#	DEPENDS ${MERGE_PYTHON_FILES} ${PROFILES_FILES})

# set_target_properties(VpCreateDesktop2022 PROPERTIES FOLDER "Profiles scripts")
