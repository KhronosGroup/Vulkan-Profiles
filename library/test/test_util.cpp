/*
 * Copyright (c) 2021-2022 Valve Corporation
 * Copyright (c) 2021-2022 LunarG, Inc.
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
 *
 * Authors:
 * - Christophe Riccio <christophe@lunarg.com>
 */

#include "test.hpp"

#include "vulkan_profiles.hpp"

TEST(test_library_util, GetStructure) { 
    VkPhysicalDeviceVulkan11Features pNext_1 = {};
    pNext_1.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    pNext_1.pNext = nullptr;

    VkPhysicalDeviceVulkan12Features pNext_2 = {};
    pNext_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    pNext_2.pNext = &pNext_1;

    VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR pNext_3 = {};
    pNext_3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES_KHR;
    pNext_3.pNext = &pNext_2;

    VkPhysicalDeviceSynchronization2FeaturesKHR pNext_4 = {};
    pNext_4.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR;
    pNext_4.pNext = &pNext_3;

    VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR pNext_5 = {};
    pNext_5.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_WORKGROUP_MEMORY_FEATURES_KHR;
    pNext_5.pNext = &pNext_4;

    VkPhysicalDeviceImageRobustnessFeaturesEXT pNext_6 = {};
    pNext_6.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES_EXT;
    pNext_6.pNext = &pNext_5;

    VkPhysicalDeviceInlineUniformBlockFeaturesEXT pNext_7 = {};
    pNext_7.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT;
    pNext_7.pNext = &pNext_6;

    VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT pNext_8 = {};
    pNext_8.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES_EXT;
    pNext_8.pNext = &pNext_7;

    VkPhysicalDevicePrivateDataFeaturesEXT pNext_9 = {};
    pNext_9.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES_EXT;
    pNext_9.pNext = &pNext_8;

    EXPECT_EQ(&pNext_9, vpGetStructure(&pNext_9, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES_EXT));
    EXPECT_EQ(&pNext_2, vpGetStructure(&pNext_9, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES));
    EXPECT_EQ(nullptr, vpGetStructure(&pNext_9, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT));
}
