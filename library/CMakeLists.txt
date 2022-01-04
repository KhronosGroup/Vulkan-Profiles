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
