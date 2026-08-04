/*
 * Copyright (c) 2021-2026 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef VP_USE_OBJECT
#define VP_USE_OBJECT 1
#endif

#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES 1
#endif

#ifndef VP_DISABLE_STATIC_LINKING
#define VP_DISABLE_STATIC_LINKING 1
#endif

#ifndef VULKAN_PROFILES_HEADER_ONLY
#include <vulkan/vulkan_profiles.hpp>
#else
#include <vulkan/debug/vulkan_profiles.h>
#endif

#include <gtest/gtest.h>
#include <vulkan/vulkan.hpp>

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    int result = RUN_ALL_TESTS();

    return result;
}

TEST(no_prototypes, create_instance_with_dynamic_pointers) {
    vk::detail::DispatchLoaderDynamic dl;
    dl.init();

    VpFunctionsCreateInfo functionsCreateInfo{};

    VpFunctions functions;
    vpCreateFunctions(&functionsCreateInfo, nullptr, &functions);

    EXPECT_TRUE(vpInitializeGlobalFunctions(functions, dl.vkGetInstanceProcAddr) == VK_SUCCESS);

    VkApplicationInfo ai{};
    ai.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    ai.pApplicationName = "Testing scaffold";
    ai.applicationVersion = VK_MAKE_VERSION(1, 2, 0);
    ai.pEngineName = "No Engine";
    ai.engineVersion = VK_MAKE_VERSION(1, 2, 0);
    ai.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo ici{};
    ici.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    ici.pApplicationInfo = &ai;

#if defined(__APPLE__)
    const char* extensions[] = { VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME };
    ici.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    ici.enabledExtensionCount = 1;
    ici.ppEnabledExtensionNames = extensions;
#endif

    VpInstanceCreateInfo createInfo{ &ici, 0, 0, nullptr };

    VkInstance instance = VK_NULL_HANDLE;
    EXPECT_TRUE(vpCreateInstance(functions, &createInfo, nullptr, &instance) == VK_SUCCESS);
}
