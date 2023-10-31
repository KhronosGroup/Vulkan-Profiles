set(LAYER_PYTHON_FILES ${PROJECT_SOURCE_DIR}/scripts/gen_profiles_solution.py)

set(TEST_FILE test_validate.cpp)
set(TEST_NAME VpProfile_test_schema_validation)

file(GLOB FILES_UNIT_TEST ${CMAKE_CURRENT_SOURCE_DIR}/data/*.json)
file(GLOB FILES_PROFILE ${CMAKE_CURRENT_SOURCE_DIR}/../*.json)
file(GLOB SCHEMA_PROFILE ${CMAKE_CURRENT_SOURCE_DIR}/../../schema/${PROFILES_SCHEMA_FILENAME})

source_group("JSON Files" FILES ${FILES_PROFILE} ${FILES_UNIT_TEST})
source_group("JSON Schema" FILES ${SCHEMA_PROFILE})

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_executable(${TEST_NAME} ${TEST_FILE} ${FILES_PROFILE} ${FILES_UNIT_TEST} ${SCHEMA_PROFILE})
target_compile_definitions(${TEST_NAME} PRIVATE 
	PROFILES_SCHEMA_FILENAME="${CMAKE_SOURCE_DIR}/schema/${PROFILES_SCHEMA_FILENAME}" 
	JSON_TEST_FILES_PATH="${CMAKE_SOURCE_DIR}/profiles/test/data/"
	PROFILE_FILES_PATH="${CMAKE_SOURCE_DIR}/profiles/")
add_dependencies(${TEST_NAME} VpGenerated)

target_link_libraries(${TEST_NAME} PRIVATE GTest::gtest GTest::gtest_main jsoncpp_static valijson)
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} --gtest_catch_exceptions=0)

set_target_properties(${TEST_NAME} PROPERTIES FOLDER "Profiles schema")

add_dependencies(VpProfile_test_schema_validation VpGenerated)

add_custom_target(VpLibrary_test_generated_ProfilesRequirements
	COMMAND Python3::Interpreter ${LAYER_PYTHON_FILES}
		--api ${API_TYPE}
		--registry ${VULKAN_HEADERS_INSTALL_DIR}/${CMAKE_INSTALL_DATADIR}/vulkan/registry/vk.xml
		--input ${CMAKE_CURRENT_LIST_DIR}/data/VP_LUNARG_test_profile_requirements
		--output-library-inc ${CMAKE_CURRENT_LIST_DIR}/data/VP_LUNARG_test_profile_requirements
		--output-library-src ${CMAKE_CURRENT_LIST_DIR}/data/VP_LUNARG_test_profile_requirements
		--validate
	VERBATIM
	SOURCES ${LAYER_PYTHON_FILES} ${CMAKE_CURRENT_LIST_DIR}/data/VP_LUNARG_test_profile_requirements
	DEPENDS ${LAYER_PYTHON_FILES} ${CMAKE_CURRENT_LIST_DIR}/data/VP_LUNARG_test_profile_requirements)

set_target_properties(VpLibrary_test_generated_ProfilesRequirements PROPERTIES FOLDER "Profiles API library")

add_dependencies(VpLibrary_test_generated_ProfilesRequirements VpGenerated)