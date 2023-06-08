/*
 * Copyright (C) 2021-2023 Valve Corporation
 * Copyright (C) 2021-2023 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Ziga Markus <ziga@lunarg.com>
 * Author: Christophe Riccio <christophe@lunarg.com>
 * Author: Mark Lobodzinski <mark@lunarg.com>
 */

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_beta.h>

#include <gtest/gtest.h>
#include "profiles_test_helper.h"

#include <cstdarg>

class TestsMechanismPhysicalSelection : public VkTestFramework {
   public:
    TestsMechanismPhysicalSelection(){};
    ~TestsMechanismPhysicalSelection(){};

    static void SetUpTestSuite() {}
    static void TearDownTestSuite(){};
};

TEST_F(TestsMechanismPhysicalSelection, force_physical_device) {
    VkResult err = VK_SUCCESS;

    profiles_test::VulkanInstanceBuilder inst_builder;

    {
        const char* force_device = "FORCE_DEVICE_OFF";
        const char* force_device_name = "NVIDIA";
        const char* force_device_uuid = "BC4A01B15641805847A8151A395A80C7";

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsForceDevice, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &force_device},
            {kLayerName, kLayerSettingsForceDeviceName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &force_device_name},
            {kLayerName, kLayerSettingsForceDeviceUUID, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &force_device_uuid}
        };

        err = inst_builder.init(settings);
        ASSERT_EQ(err, VK_SUCCESS);

        VkPhysicalDevice gpu;
        err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);
        if (err != VK_SUCCESS) {
            inst_builder.reset();
            return;
        }

        inst_builder.reset();
    }
}

