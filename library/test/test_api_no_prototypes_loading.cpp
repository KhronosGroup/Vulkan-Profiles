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

    VpCapabilitiesCreateInfo cci{};
    cci.flags = VP_CAPABILITIES_CREATE_DYNAMIC_BIT;
    VpVulkanFunctions vf{};
    vf.GetInstanceProcAddr = dl.vkGetInstanceProcAddr;
    cci.pVulkanFunctions = &vf;
    EXPECT_TRUE(vpInitialize(&cci) == VK_SUCCESS);

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
    VkInstance instance{};
    EXPECT_TRUE(dl.vkCreateInstance(&ici, nullptr, &instance) == VK_SUCCESS);

    EXPECT_TRUE(vpLoadInstance(instance, {}) == VK_SUCCESS);
}
