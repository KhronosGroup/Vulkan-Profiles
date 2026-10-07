if(BUILD_TESTS)
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
