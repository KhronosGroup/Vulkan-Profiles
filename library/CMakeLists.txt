# The library_generator is not intended to be built on Android, only on desktop
# machines.  On Android we rely on the checked-in generated source and headers
# instead of generating.
if(NOT ANDROID)
    add_subdirectory(generator)
endif()
add_subdirectory(test)

add_library(VulkanProfiles INTERFACE)
target_include_directories(VulkanProfiles INTERFACE include)
add_library(Vulkan::Profiles ALIAS VulkanProfiles)

source_group("Python Files" FILES ${CMAKE_CURRENT_SOURCE_DIR}/scripts/genvp.py)

# Using Python 3
find_package(Python3 REQUIRED)

add_custom_target(library_profiles
	COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/library/scripts/genvp.py -registry ${Vulkan-Headers_SOURCE_DIR}/registry/vk.xml -profiles ${CMAKE_CURRENT_SOURCE_DIR}/../profiles -outIncDir ${CMAKE_CURRENT_SOURCE_DIR}/include/vulkan -outSrcDir ${CMAKE_CURRENT_SOURCE_DIR}/source
	SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/scripts/genvp.py)
