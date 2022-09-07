set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(PROFILES_BUILD_TESTS)
    add_subdirectory(test)
endif()

file(GLOB FILES_SOURCE ./*.cpp)
file(GLOB FILES_HEADER ./*.hpp)
file(GLOB FILES_DOCS ./*.md)

source_group("Docs Files" FILES ${FILES_DOCS})

set(FILES_ALL ${FILES_SOURCE} ${FILES_HEADER} ${FILES_DOCS} ${jsoncpp_generated_source} ${jsoncpp_generated_header})

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_executable(vp-generator ${FILES_ALL})

set_target_properties(vp-generator PROPERTIES FOLDER "Profiles generator")
