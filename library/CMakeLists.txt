execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory  ${CMAKE_SOURCE_DIR}/library/include/vulkan/debug)
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory  ${CMAKE_SOURCE_DIR}/library/source/debug/)
execute_process(COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_SOURCE_DIR}/library/include/vulkan/vulkan_profiles.h)
execute_process(COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_SOURCE_DIR}/library/include/vulkan/vulkan_profiles.hpp)
execute_process(COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_SOURCE_DIR}/library/include/vulkan/debug/vulkan_profiles.h)
execute_process(COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_SOURCE_DIR}/library/include/vulkan/debug/vulkan_profiles.hpp)
execute_process(COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_SOURCE_DIR}/library/source/vulkan_profiles.cpp)
execute_process(COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_SOURCE_DIR}/library/source/debug/vulkan_profiles.cpp)

if(PROFILES_BUILD_TESTS)
    add_subdirectory(test)
endif()

add_library(VulkanProfiles INTERFACE)
target_include_directories(VulkanProfiles INTERFACE include)
add_library(Vulkan::Profiles ALIAS VulkanProfiles)

# NOTE: vulkan_profiles.h should NOT be installed!
install(
    FILES
        ${CMAKE_SOURCE_DIR}/library/include/vulkan/debug/vulkan_profiles.hpp
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/vulkan/
)
