/*
 * Copyright (C) 2021-2022 Valve Corporation
 * Copyright (C) 2021-2022 LunarG, Inc.
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
 */

#include <vulkan/vulkan_core.h>

#include <gtest/gtest.h>
#include "profiles_test_helper.h"

static VkPhysicalDevice gpu = VK_NULL_HANDLE;
static profiles_test::VulkanInstanceBuilder inst_builder;

class TestsUnion : public VkTestFramework {
   public:
    TestsUnion(){};
    ~TestsUnion(){};

    static void SetUpTestSuite(){
        VkResult err = VK_SUCCESS;

        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_combine_union.json";
        settings.emulate_portability = true;
        settings.profile_name = "VP_LUNARG_test_combine_union";
        settings.simulate_capabilities = SimulateCapabilityFlag::SIMULATE_MAX_ENUM;
        settings.debug_reports =
            DEBUG_REPORT_ERROR_BIT | DEBUG_REPORT_WARNING_BIT | DEBUG_REPORT_NOTIFICATION_BIT | DEBUG_REPORT_DEBUG_BIT;

        err = inst_builder.init(&settings);
        ASSERT_EQ(err, VK_SUCCESS);

        err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);
        ASSERT_EQ(err, VK_SUCCESS);
    }

    static void TearDownTestSuite(){ 
        inst_builder.reset();
    };
};

TEST_F(TestsUnion, Extension) {
    uint32_t count;
    vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, nullptr);
    std::vector<VkExtensionProperties> device_extensions(count);
    vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, device_extensions.data());

    ASSERT_STREQ("VK_KHR_maintenance1", device_extensions[0].extensionName);
    ASSERT_STREQ("VK_KHR_maintenance3", device_extensions[1].extensionName);
    ASSERT_STREQ("VK_KHR_maintenance2", device_extensions[2].extensionName);
}
/*
TEST_F(TestsUnion, Feature) {
    {
        VkPhysicalDeviceProperties gpu_props{};
        vkGetPhysicalDeviceProperties(gpu, &gpu_props);

        EXPECT_EQ(gpu_props.limits.maxImageDimension1D, 4096u);
    }
}

TEST_F(TestsUnion, Properties) {
    {
        VkPhysicalDeviceProperties gpu_props{};
        vkGetPhysicalDeviceProperties(gpu, &gpu_props);

        EXPECT_EQ(gpu_props.limits.maxImageDimension1D, 4096u);
    }
}
*/