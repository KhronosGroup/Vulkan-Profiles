if(PROFILES_BUILD_TESTS)
    add_subdirectory(test)
endif()

add_library(VulkanProfiles INTERFACE)
target_include_directories(VulkanProfiles INTERFACE include)
add_library(Vulkan::Profiles ALIAS VulkanProfiles)
