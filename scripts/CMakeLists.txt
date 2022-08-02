set(MERGE_PYTHON_FILES ${CMAKE_SOURCE_DIR}/scripts/gen_profile.py)
set(LAYER_PYTHON_FILES ${CMAKE_SOURCE_DIR}/scripts/gen_profile_solution.py)

file(GLOB PROFILES_FILES ${CMAKE_SOURCE_DIR}/profiles)
source_group("Python Files" FILES ${MERGE_PYTHON_FILES} ${LAYER_PYTHON_FILES})

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

set_target_properties(VpGenerated PROPERTIES FOLDER "Profiles generator")

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

set_target_properties(VpTestIntersect PROPERTIES FOLDER "Profiles generator")

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

set_target_properties(VpTestUnion PROPERTIES FOLDER "Profiles generator")

add_custom_target(VpCreateGeneratedName ALL
	COMMAND ${PYTHON_EXECUTABLE} ${MERGE_PYTHON_FILES}
		-registry ${vulkan-headers_SOURCE_DIR}/registry/vk.xml
		-input_dir ${CMAKE_SOURCE_DIR}/profiles/test/data/VP_LUNARG_test_combine_intersect
		-output_path ${CMAKE_SOURCE_DIR}/profiles/test/data/VP_LUNARG_test_generated_name.json
	VERBATIM
	SOURCES ${MERGE_PYTHON_FILES} ${PROFILES_FILES}
	DEPENDS ${MERGE_PYTHON_FILES} ${PROFILES_FILES})

set_target_properties(VpCreateGeneratedName PROPERTIES FOLDER "Profiles generator")

set(PROFILE_DESKTOP2022_LABEL "LunarG Vulkan Desktop Baseline 2022 profile")
set(PROFILE_DESKTOP2022_DESC "A baseline desktop profile that supports Vulkan Header 162")

add_custom_target(VpCreateDesktop2022 ALL
	COMMAND ${PYTHON_EXECUTABLE} ${MERGE_PYTHON_FILES}
		-registry ${vulkan-headers_SOURCE_DIR}/registry/vk.xml
		-input_dir ${CMAKE_SOURCE_DIR}/profiles/VP_LUNARG_desktop_baseline_2022
		-output_path ${CMAKE_SOURCE_DIR}/profiles/VP_LUNARG_desktop_baseline_2022.json
		-output_profile VP_LUNARG_desktop_baseline_2022
		-profile_label ${PROFILE_DESKTOP2022_LABEL}
		-profile_desc ${PROFILE_DESKTOP2022_DESC}
	VERBATIM
	SOURCES ${MERGE_PYTHON_FILES} ${PROFILES_FILES}
	DEPENDS ${MERGE_PYTHON_FILES} ${PROFILES_FILES})

 set_target_properties(VpCreateDesktop2022 PROPERTIES FOLDER "Profiles generator")
 
set(PROFILE_DESKTOP2022_SUBSET_LABEL "LunarG Vulkan Desktop Baseline 2022 Subset profile")
set(PROFILE_DESKTOP2022_SUBSET_DESC "A baseline macOS specific profile variant of VP_LUNARG_desktop_baseline_2022")

add_custom_target(VpCreateDesktop2022Subset ALL
	COMMAND ${PYTHON_EXECUTABLE} ${MERGE_PYTHON_FILES}
		-registry ${vulkan-headers_SOURCE_DIR}/registry/vk.xml
		-input_dir ${CMAKE_SOURCE_DIR}/profiles/VP_LUNARG_desktop_baseline_2022_subset
		-output_path ${CMAKE_SOURCE_DIR}/profiles/VP_LUNARG_desktop_baseline_2022_subset.json
		-output_profile VP_LUNARG_desktop_baseline_2022_subset
		-profile_label ${PROFILE_DESKTOP2022_SUBSET_LABEL}
		-profile_desc ${PROFILE_DESKTOP2022_SUBSET_DESC}
		-mode union
	VERBATIM
	SOURCES ${MERGE_PYTHON_FILES} ${PROFILES_FILES}
	DEPENDS ${MERGE_PYTHON_FILES} ${PROFILES_FILES})

add_custom_command(
	TARGET VpCreateDesktop2022Subset PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
		${CMAKE_SOURCE_DIR}/profiles/VP_LUNARG_desktop_baseline_2022.json
		${CMAKE_SOURCE_DIR}/profiles/VP_LUNARG_desktop_baseline_2022_subset)

 set_target_properties(VpCreateDesktop2022Subset PROPERTIES FOLDER "Profiles generator")

 add_dependencies(VpCreateDesktop2022Subset VpCreateDesktop2022)
